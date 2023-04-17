cmake_minimum_required(VERSION 3.10)

project(cpp_demo)

message("CMAKE_IMPORT_LIBRARY_SUFFIX = ${CMAKE_IMPORT_LIBRARY_SUFFIX}")
#message("CMAKE_LINK_LIBRARY_SUFFIX = ${CMAKE_LINK_LIBRARY_SUFFIX}")
#message("CMAKE_SHARED_LIBRARY_SUFFIX = ${CMAKE_SHARED_LIBRARY_SUFFIX}")
#message("CMAKE_SHARED_LIBRARY_PREFIX = ${CMAKE_SHARED_LIBRARY_PREFIX}")

if (CMAKE_SYSTEM_NAME MATCHES Linux)
	add_definitions(-DLINUX)
	add_compile_options(-std=c++11)
elseif (Windows)
endif()
include_directories(../y1 ../y2)

set(mySourceFiles main.cpp)
set(myHeaderFiles cxx_tm_call.hpp)

add_executable(cpp_demo ${mySourceFiles} ${myHeaderFiles})

