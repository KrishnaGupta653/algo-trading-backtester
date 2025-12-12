cmake_minimum_required(VERSION 3.10)
project(TradingBacktester VERSION 1.0 LANGUAGES CXX)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/main.cpp
    src/CSVParser.cpp
    src/TechnicalIndicators.cpp
    src/Backtester.cpp
)

# Create executable
add_executable(backtester ${SOURCES})

# Link math library
target_link_libraries(backtester m)

# Installation
install(TARGETS backtester DESTINATION bin)

# Print build information
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ Flags: ${CMAKE_CXX_FLAGS}")