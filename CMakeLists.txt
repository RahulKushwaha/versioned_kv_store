cmake_minimum_required(VERSION 3.24)
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")

project(KeyValueStoreWithStableIteror)

set(CMAKE_CXX_STANDARD 17)

include_directories(/opt/homebrew/include)
link_directories(/opt/homebrew/lib)

add_executable(KeyValueStoreWithStableIteror main.cpp OrderedMap.h)

enable_testing()

add_executable(KeyValueStoreTests OrderedMap.h OrderedMapTest.cc)
target_link_libraries(KeyValueStoreTests gtest gtest_main)
