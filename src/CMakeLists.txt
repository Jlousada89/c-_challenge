cmake_minimum_required(VERSION 3.16)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

# Gather the source files
file(GLOB UNIT_TESTS_SRC_FILES *.cpp)

# Dependencies
find_package(Boost COMPONENTS filesystem system unit_test_framework REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# Build unit tests
# add_definitions(-DBOOST_TEST_DYN_LINK)
add_executable(shared_ptr_tests ${UNIT_TESTS_SRC_FILES} ${SRC_FILES})
target_link_libraries(shared_ptr_tests ${Boost_LIBRARIES})
enable_testing()
add_test(shared_ptr_tests ${CMAKE_BINARY_DIR}/shared_ptr_tests)
