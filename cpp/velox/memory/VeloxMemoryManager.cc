/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "VeloxMemoryManager.h"

#include "utils/exception.h"

namespace gluten {

using namespace facebook;

// So far HbmMemoryAllocator would not work correctly since the underlying
//   gluten allocator is only used to do allocation-reporting to Spark in mmap case
// This allocator only hook `allocateBytes` and `freeBytes`, we can not ensure this behavior is safe enough,
// so, only use this allocator when build with GLUTEN_ENABLE_HBM.
class VeloxMemoryAllocator final : public velox::memory::MemoryAllocator {
 public:
  VeloxMemoryAllocator(gluten::MemoryAllocator* glutenAlloc, velox::memory::MemoryAllocator* veloxAlloc)
      : glutenAlloc_(glutenAlloc), veloxAlloc_(veloxAlloc) {}

  Kind kind() const override {
    return veloxAlloc_->kind();
  }

  bool allocateNonContiguous(
      velox::memory::MachinePageCount numPages,
      velox::memory::Allocation& out,
      ReservationCallback reservationCB,
      velox::memory::MachinePageCount minSizeClass) override {
    return veloxAlloc_->allocateNonContiguous(numPages, out, reservationCB, minSizeClass);
  }

  int64_t freeNonContiguous(velox::memory::Allocation& allocation) override {
    int64_t freedBytes = veloxAlloc_->freeNonContiguous(allocation);
    return freedBytes;
  }

  bool allocateContiguous(
      velox::memory::MachinePageCount numPages,
      velox::memory::Allocation* collateral,
      velox::memory::ContiguousAllocation& allocation,
      ReservationCallback reservationCB) override {
    return veloxAlloc_->allocateContiguous(numPages, collateral, allocation, reservationCB);
  }

  void freeContiguous(velox::memory::ContiguousAllocation& allocation) override {
    veloxAlloc_->freeContiguous(allocation);
  }

  void* allocateBytes(uint64_t bytes, uint16_t alignment) override {
    void* out;
    VELOX_CHECK(glutenAlloc_->allocateAligned(alignment, bytes, &out))
    return out;
  }

  void freeBytes(void* p, uint64_t size) noexcept override {
    VELOX_CHECK(glutenAlloc_->free(p, size));
  }

  bool checkConsistency() const override {
    return veloxAlloc_->checkConsistency();
  }

  velox::memory::MachinePageCount numAllocated() const override {
    return veloxAlloc_->numAllocated();
  }

  velox::memory::MachinePageCount numMapped() const override {
    return veloxAlloc_->numMapped();
  }

  std::string toString() const override {
    return veloxAlloc_->toString();
  }

  size_t capacity() const override {
    return veloxAlloc_->capacity();
  }

 private:
  gluten::MemoryAllocator* glutenAlloc_;
  velox::memory::MemoryAllocator* veloxAlloc_;
};

/// We assume in a single Spark task. No thread-safety should be guaranteed.
class ListenableArbitrator : public velox::memory::MemoryArbitrator {
 public:
  ListenableArbitrator(const Config& config, AllocationListener* listener)
      : MemoryArbitrator(config), listener_(listener) {}

  void reserveMemory(velox::memory::MemoryPool* pool, uint64_t) override {
    growPool(pool, initMemoryPoolCapacity_);
  }

  uint64_t releaseMemory(velox::memory::MemoryPool* pool, uint64_t bytes) override {
    uint64_t freeBytes = pool->shrink(bytes);
    listener_->allocationChanged(-freeBytes);
    if (bytes == 0 && pool->capacity() != 0) {
      // So far only MemoryManager::dropPool() calls with 0 bytes. Let's assert the pool
      //   gives all capacity back to Spark
      //
      // We are likely in destructor, do not throw. INFO log is fine since we have leak checks from Spark's memory
      //   manager
      LOG(INFO) << "Memory pool " << pool->name() << " not completely shrunk when Memory::dropPool() is called";
    }
    return freeBytes;
  }

  bool growMemory(
      velox::memory::MemoryPool* pool,
      const std::vector<std::shared_ptr<velox::memory::MemoryPool>>& candidatePools,
      uint64_t targetBytes) override {
    GLUTEN_CHECK(candidatePools.size() == 1, "ListenableArbitrator should only be used within a single root pool");
    auto candidate = candidatePools.back();
    GLUTEN_CHECK(pool->root() == candidate.get(), "Illegal state in ListenableArbitrator");
    growPool(pool, targetBytes);
    return true;
  }

  Stats stats() const override {
    Stats stats; // no-op
    return stats;
  }

  std::string toString() const override {
    return fmt::format(
        "ARBITRATOR[{}] CAPACITY {} {}", kindString(kind_), velox::succinctBytes(capacity_), stats().toString());
  }

 private:
  void growPool(velox::memory::MemoryPool* pool, uint64_t bytes) {
    listener_->allocationChanged(bytes);
    pool->grow(bytes);
  }

  void abort(velox::memory::MemoryPool* pool) {
    try {
      pool->abort();
    } catch (const std::exception& e) {
      LOG(WARNING) << "Failed to abort memory pool " << pool->toString();
    }
    // NOTE: no matter memory pool abort throws or not, it should have been marked
    // as aborted to prevent any new memory arbitration triggered from the aborted
    // memory pool.
    VELOX_CHECK(pool->aborted());
  }

  gluten::AllocationListener* listener_;
};

VeloxMemoryManager::VeloxMemoryManager(
    std::string name,
    std::shared_ptr<MemoryAllocator> allocator,
    std::shared_ptr<AllocationListener> listener)
    : MemoryManager(), name_(name), listener_(std::move(listener)) {
  auto veloxAlloc = velox::memory::MemoryAllocator::getInstance();
  glutenAlloc_ = std::make_shared<ListenableMemoryAllocator>(allocator.get(), listener_);
#ifdef GLUTEN_ENABLE_HBM
  auto wrappedAlloc = std::make_shared<VeloxMemoryAllocator>(allocator.get(), veloxAlloc);
#endif
  velox::memory::MemoryArbitrator::Config arbitratorConfig{
      velox::memory::MemoryArbitrator::Kind::kNoOp, // do not use shared arbitrator as it will mess up the thread
                                                    // contexts (one Spark task reclaims memory from another)
      velox::memory::kMaxMemory, // the 2nd capacity
      0,
      32 << 20,
      true};
  velox::memory::IMemoryManager::Options mmOptions{
      velox::memory::MemoryAllocator::kMaxAlignment,
      velox::memory::kMaxMemory, // the 1st capacity, Velox requires for a couple of different capacity numbers
      true,
      false,
#ifdef GLUTEN_ENABLE_HBM
      wrappedAlloc.get(),
#else
      veloxAlloc,
#endif
      [=]() { return std::make_unique<ListenableArbitrator>(arbitratorConfig, listener_.get()); },
  };
  veloxMemoryManager_ = std::make_unique<velox::memory::MemoryManager>(mmOptions);
  veloxPool_ = veloxMemoryManager_->addRootPool(
      name_ + "_root",
      velox::memory::kMaxMemory, // the 3rd capacity
      facebook::velox::memory::MemoryReclaimer::create());
  veloxLeafPool_ = veloxPool_->addLeafChild(name_ + "_default_leaf");
}

velox::memory::IMemoryManager* getDefaultVeloxMemoryManager() {
  return &(facebook::velox::memory::defaultMemoryManager());
}

static std::shared_ptr<velox::memory::MemoryPool> rootVeloxMemoryPool() {
  auto* mm = getDefaultVeloxMemoryManager();
  static auto root = mm->addRootPool(
      "gluten_root", mm->capacity(), facebook::velox::memory::MemoryReclaimer::create()); // the 3rd capacity
  return root;
}

std::shared_ptr<velox::memory::MemoryPool> defaultLeafVeloxMemoryPool() {
  static auto leaf =
      rootVeloxMemoryPool()->addLeafChild("default_leaf", true, facebook::velox::memory::MemoryReclaimer::create());
  return leaf;
}

} // namespace gluten
