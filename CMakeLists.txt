cmake_minimum_required(VERSION 3.12)

project(LSLLeapMotion
		DESCRIPTION "Sample from Leap Motion device and stream over LabStreamingLayer"
		LANGUAGES C CXX
		VERSION 0.1)

# also look for CMake modules in the cmake subfolder
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND Leap_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/LeapSDK")

find_package(LSL REQUIRED
		HINTS ${LSL_INSTALL_ROOT}
		"${CMAKE_CURRENT_LIST_DIR}/../../LSL/liblsl/build/"
		"${CMAKE_CURRENT_LIST_DIR}/../../LSL/liblsl/build/install"
		"${CMAKE_CURRENT_LIST_DIR}/../../LSL/liblsl/out/build/x64-Release"
		"${CMAKE_CURRENT_LIST_DIR}/../../LSL/liblsl/out/install/x64-Release"
		PATH_SUFFIXES share/LSL
)
get_filename_component(LSL_PATH ${LSL_CONFIG} DIRECTORY)
message(STATUS "Found LSL lib in ${LSL_PATH}")

find_package(Threads REQUIRED)
find_package(Leap REQUIRED)

# Add executable targets, the default target has the same name as the project

add_executable(${PROJECT_NAME} MACOSX_BUNDLE WIN32
	src/main.cpp
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
	Threads::Threads
	LSL::lsl
	Leap::Motion
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_14)

# Setting up deployment (let CPack generate a zip/dmg file)
installLSLApp(${PROJECT_NAME})
installLSLAuxFiles(${PROJECT_NAME}
	${PROJECT_NAME}.cfg
)

if (WIN32)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_FILE:LSL::lsl>
			$<TARGET_FILE:Leap::Motion>
			$<TARGET_FILE_DIR:${PROJECT_NAME}>)
	add_custom_command(
			TARGET ${PROJECT_NAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy
			${CMAKE_CURRENT_SOURCE_DIR}//${PROJECT_NAME}.cfg
			$<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif()


LSLGenerateCPackConfig()
