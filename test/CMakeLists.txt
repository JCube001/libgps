find_package(CMocka REQUIRED)

add_definitions(${CMOCKA_DEFINITIONS})

include_directories(
    "${PROJECT_SOURCE_DIR}/src"
    ${CMOCKA_INCLUDE_DIRS}
)

add_executable(test-gps test_gps.c)
target_link_libraries(
    test-gps
    ${PROJECT_NAME}
    ${CMOCKA_LIBRARIES}
)
