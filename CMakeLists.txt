cmake_minimum_required(VERSION 3.15)
project(fpgrowth)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_executable(fpgrowth
        include/fptree.hpp
        src/main.cpp)
