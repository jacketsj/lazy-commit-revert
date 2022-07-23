cmake_minimum_required(VERSION 3.16)

project(lazy-commit-revert LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(SRC ${CMAKE_SOURCE_DIR}/src)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
include_directories("${SRC}")
set(CMAKE_CXX_FLAGS "-Wall -pthread -O3 -g")

find_package(Threads)
find_package(X11)

file(GLOB SOURCES ${SRC}/*.cpp)

add_executable(lcr_test ${SOURCES})
