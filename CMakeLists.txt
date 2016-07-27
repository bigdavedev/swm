cmake_minimum_required ( VERSION 2.8 )

project ( swm C )

set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11" )
set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -Werror" )
set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${XCB_CFLAGS_OTHER}" )

find_package( PkgConfig )
pkg_search_module( XCB REQUIRED xcb )

set ( SOURCES source/swm.c )
add_executable ( ${PROJECT_NAME} ${SOURCES} )

set ( LIBRARIES xcb )
target_link_libraries ( ${PROJECT_NAME} ${XCB_LIBRARIES} )

#target_compile_options( ${PROJECT_NAME} PUBLIC ${XCB_CFLAGS_OTHER} )
