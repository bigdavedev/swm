project (swm CXX)

set (TARGET ${PROJECT_NAME})

option (X86_BUILD "Target 32bit systems" ON)

find_package (PkgConfig)
pkg_search_module (XCB REQUIRED xcb)

add_executable (${PROJECT_NAME} "")

target_compile_options (
	${TARGET}

	PUBLIC
	-std=c++17
	-Wall
	-Wextra
	-Wpedantic
	-Werror
)

target_sources (
	${TARGET}

	PUBLIC
	swm.cpp
)

if (X86_BUILD)
	set_target_properties (
		${TARGET}
		PROPERTIES
		COMPILE_FLAGS "-m32"
		LINK_FLAGS "-m32"
	)
endif ()

target_link_libraries (
	${TARGET}

	PUBLIC
	${XCB_LIBRARIES}
)

