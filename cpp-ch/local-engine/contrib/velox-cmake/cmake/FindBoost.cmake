message(STATUS "Using Boost - ClickHouse Bundled")
set(Boost_FOUND TRUE)
set(Boost_LIBRARIES boost::headers_only)
message(STATUS "Boost targets: ${Boost_LIBRARIES}")

#
# let's hack
#
list(REMOVE_ITEM CMAKE_MODULE_PATH ${WRAP_VELOX_CMAKE})
list(PREPEND CMAKE_MODULE_PATH ${WRAP_VELOX_CMAKE})