# Se up cache variables
set(Leap_ROOT_DIR
    "${Leap_ROOT_DIR}"
    CACHE PATH "The root to search for Leap SDK"
)

find_path(Leap_INCLUDE_DIR
    name "Leap.h"
    PATHS ${Leap_ROOT_DIR}
    PATH_SUFFIXES include
)

find_library(Leap_LIBRARY
    name Leap
    PATHS ${Leap_ROOT_DIR}
    PATH_SUFFIXES lib/x64
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Leap REQUIRED_VARS Leap_INCLUDE_DIR Leap_LIBRARY)

if(Leap_FOUND)
    set(Leap_INCLUDE_DIRS ${Leap_INCLUDE_DIR})
    set(Leap_LIBRARIES ${Leap_LIBRARY})

    if(NOT TARGET Leap::Motion)
        add_library(Leap::Motion SHARED IMPORTED)
        # On Windows, the IMPORTED_LIB is the .lib, and the IMPORTED_LOCATION (below) is the .dll
        get_filename_component(libext vendorsdk_SDK_LIB EXT)
        if(libext STREQUAL ".lib")
            set_target_properties(Leap::Device PROPERTIES
                IMPORTED_IMPLIB ${Leap_LIBRARY})
            string(REPLACE ".lib" ".dll" Leap_LIBRARY ${Leap_LIBRARY})
        endif()

        set_target_properties(Leap::Motion PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${Leap_INCLUDE_DIR}"
            IMPORTED_LOCATION ${Leap_LIBRARY}
            IMPORTED_LINK_INTERFACE_LANGUAGES "C"
            INTERFACE_COMPILE_FEATURES cxx_std_14
        )

    endif(NOT TARGET Leap::Motion)
endif(Leap_FOUND)
mark_as_advanced(Leap_ROOT_DIR Leap_INCLUDE_DIR Leap_LIBRARY)
