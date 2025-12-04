cmake_minimum_required(VERSION 3.10)
project(device_agent LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(device_agent src/main.cpp)

if (WIN32)
    # nothing extra for now; linking to Kernel32 is automatic with MSVC
endif()
