# - Try to find cmocka
# Once done this will define
#  CMOCKA_FOUND        - System has cmocka
#  CMOCKA_INCLUDE_DIRS - The cmocka include directories
#  CMOCKA_LIBRARIES    - The libraries needed to use cmocka
#  CMOCKA_DEFINITIONS  - Compiler switches required for using cmocka (currently none)

if(WIN32)
    # Try the 64-bit Program Files path if building a 64-bit application
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(_PATH_PREFIX "C:/Program Files/cmocka")
    else()
        set(_PATH_PREFIX "C:/Program Files (x86)/cmocka")
    endif()
    set(_INCLUDE_DIRS "${_PATH_PREFIX}/include")
    set(_LIBRARY_DIRS "${_PATH_PREFIX}/lib" "${_PATH_PREFIX}/bin")
endif()

find_path(
    CMOCKA_INCLUDE_DIR
    NAMES cmocka.h cmocka_pbc.h
    PATHS ${_INCLUDE_DIRS}
)

find_library(
    CMOCKA_LIBRARY
    NAMES libcmocka.a cmocka
    PATHS ${_LIBRARY_DIRS}
)

set(CMOCKA_INCLUDE_DIRS ${CMOCKA_INCLUDE_DIR})
set(CMOCKA_LIBRARIES ${CMOCKA_LIBRARY})

# Handle arguments such as QUIETLY and REQUIRED
# Sets CMOCKA_FOUND to TRUE if the other variables passed in here are set
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    cmocka DEFAULT_MSG
    CMOCKA_LIBRARY
    CMOCKA_INCLUDE_DIR
)

mark_as_advanced(CMOCKA_INCLUDE_DIR CMOCKA_LIBRARY)

unset(_PATH_PREFIX)
unset(_INCLUDE_DIRS)
unset(_LIBRARY_DIRS)
