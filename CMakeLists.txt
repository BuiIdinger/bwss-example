cmake_minimum_required(VERSION 3.10)

# Set the project name
project(
  bwss-test
  VERSION 1.0.0 
  DESCRIPTION "Test exec for BWSS." 
  LANGUAGES CXX
)

# Set C++ standards
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set build type
set(CMAKE_BUILD_TYPE Debug)

# Add a executable
add_executable(bwss-test main.cpp)

# Find bwss
find_package(bwss REQUIRED)

# Link bwss
target_link_libraries(bwss-test PRIVATE bwss::bwss)
include_directories(${BWSS_INCLUDE_DIR})