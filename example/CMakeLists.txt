include(CheckFunctionExists)

include_directories(${PROJECT_SOURCE_DIR}/src)

add_executable(simple-usage simple_usage.c)
target_link_libraries(simple-usage ${PROJECT_NAME})

check_function_exists(clock_gettime HAVE_CLOCK_GETTIME)
if(HAVE_CLOCK_GETTIME)
    add_executable(benchmark benchmark.c)
    target_link_libraries(benchmark ${PROJECT_NAME})
else()
    message(WARNING "Missing function clock_gettime, benchmark example not built")
endif()
