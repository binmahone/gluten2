//
// Created by chang on 23-9-22.
//
#include "velox/dwio/parquet/reader/ParquetReader.h"

using namespace facebook::velox;
using namespace facebook::velox::common;
using namespace facebook::velox::dwio::common;
using namespace facebook::velox::parquet;

namespace {
auto defaultPool = memory::addDefaultLeafMemoryPool();
}

ParquetReader createReader(
    const std::string& path,
    const ReaderOptions& opts) {
  return ParquetReader(
      std::make_unique<BufferedInput>(
          std::make_shared<LocalReadFile>(path), opts.getMemoryPool()),
      opts);
}

dwio::common::RowReaderOptions getReaderOpts(
    const RowTypePtr& rowType,
    bool fileColumnNamesReadAsLowerCase = false) {
  dwio::common::RowReaderOptions rowReaderOpts;
  rowReaderOpts.select(
      std::make_shared<facebook::velox::dwio::common::ColumnSelector>(
          rowType,
          rowType->names(),
          nullptr,
          fileColumnNamesReadAsLowerCase));

  return rowReaderOpts;
}

std::shared_ptr<facebook::velox::common::ScanSpec> makeScanSpec(
    const RowTypePtr& rowType) {
  auto scanSpec = std::make_shared<facebook::velox::common::ScanSpec>("");
  scanSpec->addAllChildFields(*rowType);
  return scanSpec;
}


int main(int /*argc*/, char** /*argv*/) {

  const std::string sample("/home/chang/OpenSource/velox/velox/dwio/parquet/tests/examples/sample.parquet");
  ReaderOptions readerOptions{defaultPool.get()};
  ParquetReader reader = createReader(sample, readerOptions);

  auto schema = ROW({"a", "b"}, {BIGINT(), DOUBLE()});
  auto rowReaderOpts = getReaderOpts(schema);
  auto scanSpec = makeScanSpec(schema);
  rowReaderOpts.setScanSpec(scanSpec);
  auto rowReader = reader.createRowReader(rowReaderOpts);
  uint64_t total = 0;
  VectorPtr result = BaseVector::create(schema, 0, defaultPool.get());
  rowReader->next(1000, result);
  total += result->size();
  std::cout << total << '\n';
  return 0;
}
