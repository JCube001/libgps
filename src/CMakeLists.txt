# FIXME: Add installation directives for libgps

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_library(${PROJECT_NAME} STATIC gps.c)
