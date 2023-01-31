cmake_minimum_required(VERSION 3.24)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER "clang++")


find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}" "-checks=-*,-header-filter=.*,modernize-*,-modernize-use-trailing-return-type")

project(KeyValueStoreWithStableIterator)

set(CMAKE_CXX_STANDARD 20)

include_directories(/opt/homebrew/include)
link_directories(/opt/homebrew/lib)

add_library(key_value_store_static STATIC OrderedMap.h OrderedMap.cc
        KeyValueIterator.cc KeyValueIterator.h KeyValueSegment.h Common.h)

add_executable(KeyValueStoreWithStableIterator main.cpp OrderedMap.h)
target_link_libraries(KeyValueStoreWithStableIterator jemalloc key_value_store_static)

enable_testing()

add_executable(KeyValueStoreTests
        tests/OrderedMapTest.cc
        tests/KeyValueSegmentIteratorTest.cc
        tests/RandomOperationsTest.cc
        tests/KeyValueIteratorTest.cc)
target_link_libraries(KeyValueStoreTests gtest gtest_main -fsanitize=undefined key_value_store_static)

#set_target_properties(key_value_store_static PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")