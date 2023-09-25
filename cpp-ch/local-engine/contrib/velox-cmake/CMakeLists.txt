set(VELOX_ROOT_DIR "/home/chang/OpenSource/velox" CACHE PATH "Velox root directory")

# Sets new behavior for CMP0135, which controls how timestamps are extracted
# when using ExternalProject_Add():
# https://cmake.org/cmake/help/latest/policy/CMP0135.html
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
    set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)
endif()

set(WRAP_VELOX_CMAKE "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

list(PREPEND CMAKE_MODULE_PATH "${VELOX_ROOT_DIR}/CMake")
include(ResolveDependency)

set(gflags_SOURCE BUNDLED)
set(glog_SOURCE BUNDLED)
set(folly_SOURCE BUNDLED)
set(WITH_GTEST OFF)
set(WITH_PKGCONFIG OFF)
set(BUILD_TESTING OFF)
resolve_dependency(gflags COMPONENTS shared)
if(NOT TARGET gflags::gflags)
    # This is a bit convoluted, but we want to be able to use gflags::gflags as a
    # target even when velox is built as a subproject which uses
    # `find_package(gflags)` which does not create a globally imported target that
    # we can ALIAS.
    add_library(gflags_gflags INTERFACE)
    target_link_libraries(gflags_gflags INTERFACE gflags)
    add_library(gflags::gflags ALIAS gflags_gflags)
endif()
resolve_dependency(glog)

target_compile_options(gflags_nothreads_static PRIVATE
        -Wno-zero-as-null-pointer-constant
        -Wno-format-nonliteral
        -Wno-extra-semi-stmt
        -Wno-implicit-fallthrough
        -Wno-shadow
        -Wno-covered-switch-default
        -Wno-unreachable-code
        -Wno-unused-member-function
)

target_compile_options(gflags_static PRIVATE
        -Wno-zero-as-null-pointer-constant
        -Wno-format-nonliteral
        -Wno-extra-semi-stmt
        -Wno-implicit-fallthrough
        -Wno-shadow
        -Wno-covered-switch-default
        -Wno-unreachable-code
        -Wno-unused-member-function
)

target_compile_options(glogbase PRIVATE
        -Wno-format-nonliteral
        -Wno-zero-as-null-pointer-constant
        -Wno-reserved-macro-identifier
        -Wno-header-hygiene
        -Wno-reserved-identifier
        -Wno-reserved-macro-identifier
        -Wno-suggest-override
        -Wno-suggest-destructor-override
        -Wno-deprecated-dynamic-exception-spec
        -Wno-unused-macros
        -Wno-atomic-implicit-seq-cst
        -Wno-unreachable-code-return
        -Wno-unused-but-set-variable
        -Wno-shorten-64-to-32
        -Wno-unused-member-function
        -Wno-extra-semi-stmt
        -Wno-cast-function-type-strict
        -Wno-missing-noreturn
        -Wno-old-style-cast
)

# folly
list(PREPEND CMAKE_MODULE_PATH ${WRAP_VELOX_CMAKE})
set(FOLLY_BINARY_DIR "${GLUTEN_CONTRIB_BINARY_DIR}/folly")

## TODO: why check failed?
set(FOLLY_HAVE_OPENSSL_ASN1_TIME_DIFF ON)
set(FOLLY_CPP_ATOMIC_BUILTIN ON)
set(FOLLY_HAVE_WEAK_SYMBOLS 1)
add_subdirectory("${VELOX_ROOT_DIR}/CMake/resolve_dependency_modules/folly" "${FOLLY_BINARY_DIR}")

#TODO: How to transfer to the other target?
include_directories(${VELOX_ROOT_DIR})

# gluten_dwio_common
set(DWIO_COMMON_DIR "${VELOX_ROOT_DIR}/velox/dwio/common")
add_library(
        gluten_dwio_common
        ${DWIO_COMMON_DIR}/BitConcatenation.cpp
        ${DWIO_COMMON_DIR}/BitPackDecoder.cpp
        ${DWIO_COMMON_DIR}/BufferedInput.cpp
        ${DWIO_COMMON_DIR}/CachedBufferedInput.cpp
        ${DWIO_COMMON_DIR}/CacheInputStream.cpp
        ${DWIO_COMMON_DIR}/ColumnSelector.cpp
        ${DWIO_COMMON_DIR}/DataBufferHolder.cpp
        ${DWIO_COMMON_DIR}/DataSink.cpp
        ${DWIO_COMMON_DIR}/DecoderUtil.cpp
        ${DWIO_COMMON_DIR}/DirectDecoder.cpp
        ${DWIO_COMMON_DIR}/DwioMetricsLog.cpp
        ${DWIO_COMMON_DIR}/FlatMapHelper.cpp
        ${DWIO_COMMON_DIR}/InputStream.cpp
        ${DWIO_COMMON_DIR}/IntDecoder.cpp
        ${DWIO_COMMON_DIR}/IoStatistics.cpp
        # ${DWIO_COMMON_DIR}/MetadataFilter.cpp                        # Remove velox expression dependency
        ${DWIO_COMMON_DIR}/Options.cpp
        ${DWIO_COMMON_DIR}/OutputStream.cpp
        ${DWIO_COMMON_DIR}/Range.cpp
        ${DWIO_COMMON_DIR}/Reader.cpp
        ${DWIO_COMMON_DIR}/ReaderFactory.cpp
        ${DWIO_COMMON_DIR}/ScanSpec.cpp
        ${DWIO_COMMON_DIR}/ColumnLoader.cpp
        ${DWIO_COMMON_DIR}/SelectiveByteRleColumnReader.cpp
        ${DWIO_COMMON_DIR}/SelectiveColumnReader.cpp
        ${DWIO_COMMON_DIR}/SelectiveRepeatedColumnReader.cpp
        ${DWIO_COMMON_DIR}/SelectiveStructColumnReader.cpp
        ${DWIO_COMMON_DIR}/SeekableInputStream.cpp
        ${DWIO_COMMON_DIR}/TypeUtils.cpp
        ${DWIO_COMMON_DIR}/TypeWithId.cpp
        ${DWIO_COMMON_DIR}/WriterFactory.cpp
)
target_compile_options(
        gluten_dwio_common PRIVATE
        -Wno-shadow-uncaptured-local
        -Wno-shorten-64-to-32
        -Wno-reserved-macro-identifier
        -Wno-undef
        -Wno-comma
        -Wno-reserved-identifier
        -Wno-deprecated
        -Wno-old-style-cast
        -Wno-zero-as-null-pointer-constant
        -Wno-suggest-override
        -Wno-extra-semi-stmt
        -Wno-unknown-warning-option
        -Wno-documentation
        -Wno-suggest-destructor-override
        -Wno-gnu-zero-variadic-macro-arguments
        -Wno-gnu-statement-expression-from-macro-expansion
        -Wno-unused-parameter
        -Wno-cast-align
        -Wno-nullable-to-nonnull-conversion
        -Wno-shadow
        -Wno-unused-variable
        -Wno-sign-compare
        -Wno-cast-qual
        -Wno-gnu-statement-expression
        -Wno-extra-semi
        -Wno-inconsistent-missing-destructor-override
        -Wno-undefined-reinterpret-cast
        -Wno-covered-switch-default
        -Wno-thread-safety-analysis
        -Wno-nullability-completeness
        -Wno-missing-noreturn
        -Wno-shadow-field
        -Wno-ignored-qualifiers
        -Wno-implicit-float-conversion
        -Wno-unused-member-function
        -Wno-mismatched-tags
        -Wno-float-conversion
)
target_link_libraries(
        gluten_dwio_common
        Folly::folly
        ch_contrib::fmt
        ch_contrib::protobuf
        ch_contrib::xsimd
)

# gluten_dwio_native_parquet_reader
set(PARQUET_READER_DIR "${VELOX_ROOT_DIR}/velox/dwio/parquet/reader")
add_library(
        gluten_dwio_native_parquet_reader
        ${PARQUET_READER_DIR}/NestedStructureDecoder.cpp
        ${PARQUET_READER_DIR}/ParquetReader.cpp
        ${PARQUET_READER_DIR}/ParquetTypeWithId.cpp
        ${PARQUET_READER_DIR}/PageReader.cpp
        ${PARQUET_READER_DIR}/ParquetColumnReader.cpp
        ${PARQUET_READER_DIR}/ParquetData.cpp
        ${PARQUET_READER_DIR}/RepeatedColumnReader.cpp
        ${PARQUET_READER_DIR}/RleBpDecoder.cpp
        ${PARQUET_READER_DIR}/Statistics.cpp
        ${PARQUET_READER_DIR}/StructColumnReader.cpp
        ${PARQUET_READER_DIR}/StringColumnReader.cpp
)
target_compile_options(
        gluten_dwio_native_parquet_reader PRIVATE
        -Wno-shadow-uncaptured-local
        -Wno-shorten-64-to-32
        -Wno-reserved-macro-identifier
        -Wno-undef
        -Wno-comma
        -Wno-reserved-identifier
        -Wno-deprecated
        -Wno-old-style-cast
        -Wno-zero-as-null-pointer-constant
        -Wno-suggest-override
        -Wno-extra-semi-stmt
        -Wno-unknown-warning-option
        -Wno-documentation
        -Wno-suggest-destructor-override
        -Wno-gnu-zero-variadic-macro-arguments
        -Wno-gnu-statement-expression-from-macro-expansion
        -Wno-unused-parameter
        -Wno-cast-align
        -Wno-nullable-to-nonnull-conversion
        -Wno-shadow
        -Wno-unused-variable
        -Wno-sign-compare
        -Wno-cast-qual
        -Wno-gnu-statement-expression
        -Wno-extra-semi
        -Wno-inconsistent-missing-destructor-override
        -Wno-undefined-reinterpret-cast
        -Wno-covered-switch-default
        -Wno-thread-safety-analysis
        -Wno-nullability-completeness
        -Wno-missing-noreturn
        -Wno-shadow-field
        -Wno-ignored-qualifiers
        -Wno-implicit-float-conversion
        -Wno-unused-member-function
        -Wno-mismatched-tags
        -Wno-float-conversion
        #
        -Wno-unused-private-field
        -Wno-unused-but-set-variable
        -Wno-macro-redefined
        -Wno-unreachable-code-return
)
target_link_libraries(
        gluten_dwio_native_parquet_reader
        Folly::folly
        ch_contrib::fmt
        ch_contrib::parquet
        ch_contrib::protobuf
        ch_contrib::xsimd
        ch_contrib::lz4
        ch_contrib::zstd
        ch_contrib::snappy
)