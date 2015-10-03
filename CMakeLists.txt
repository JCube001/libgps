cmake_minimum_required(VERSION 2.8)
project(gps C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

set(BUILD_EXAMPLES OFF CACHE BOOL "Build example programs")
set(BUILD_DOCUMENTATION OFF CACHE BOOL "Build public API documentation")
set(BUILD_UNIT_TESTS OFF CACHE BOOL "Build unit tests")
set(BUILD_STATIC_LIB OFF CACHE BOOL "Build a static library in addition to the shared library")

add_subdirectory(src)

if(BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

if(BUILD_DOCUMENTATION)
    add_subdirectory(doc)
endif()

if(BUILD_UNIT_TESTS)
    add_subdirectory(test)
    enable_testing()
    add_test(NAME test-gps COMMAND test-gps)
endif()
