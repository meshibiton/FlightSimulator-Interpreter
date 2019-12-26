cmake_minimum_required(VERSION 3.14)
project(untitled2)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS -pthread)

add_executable(untitled2 main.cpp Command.cpp Command.h Expression.h Interpreter.cpp Interpreter.h GlobalFunction.h GlobalFunction.cpp)