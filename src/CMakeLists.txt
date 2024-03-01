cmake_minimum_required(VERSION 3.25)

if(NOT TARGET libgps)
    add_library(libgps STATIC gps.c)
    target_include_directories(libgps PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endif()
