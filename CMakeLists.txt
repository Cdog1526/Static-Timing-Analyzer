cmake_minimum_required(VERSION 3.16)
project(mini_sta LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)

add_library(verilog
    src/verilog/lexer.cpp
    src/verilog/parser.cpp
)
target_include_directories(verilog PUBLIC src)
add_executable(sta
    src/main.cpp
    src/sta/timing_graph.cpp
    src/sta/sta.cpp
)
target_link_libraries(sta PRIVATE verilog)