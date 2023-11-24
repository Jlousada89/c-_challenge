cmake_minimum_required(VERSION 3.16)

project(shared_ptr)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++2b")

find_package(Boost COMPONENTS filesystem system REQUIRED)
find_package(PkgConfig REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

enable_testing()
# Unit tests
add_subdirectory(src)
