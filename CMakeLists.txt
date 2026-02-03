cmake_minimum_required(VERSION 3.16)
project(mini_sta LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)

add_executable(sta
    src/main.cpp
    src/timing_graph.cpp
)