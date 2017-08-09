project (swm CXX)

set (TARGET ${PROJECT_NAME})

find_package (PkgConfig)
pkg_search_module (XCB REQUIRED xcb)

add_executable (${PROJECT_NAME} "")

target_compile_options (
	${TARGET}

	PUBLIC
	-std=c++14
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

target_link_libraries (
	${TARGET}

	PUBLIC
	${XCB_LIBRARIES}
)

