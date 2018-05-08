cmake_minimum_required(VERSION 3.11.1)
project(sat_solve)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -Wextra -pedantic -pipe -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Og")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SAT_SOLVE_SOURCES
    src/main.cpp
    src/file.cpp
)

add_executable(sat_solve ${SAT_SOLVE_SOURCES})

target_include_directories(sat_solve PRIVATE
    src
)
