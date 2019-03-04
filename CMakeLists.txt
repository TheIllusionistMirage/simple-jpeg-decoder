#
#  libKPEG - Simple JPEG Decoder
# ===============================================
#
# CMake build script

cmake_minimum_required(VERSION 3.0)
project(kpeg)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
      "Choose the type of build, options are: Debug Release."
      FORCE)
endif(NOT CMAKE_BUILD_TYPE)

if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -g")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
endif()

# Add sources
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/*.cpp")

# Specify include directory
include_directories("${PROJECT_SOURCE_DIR}/include/")

# Compile and generate the executable
add_executable(kpeg main.cpp src/Decoder.cpp src/Image.cpp src/HuffmanTree.cpp src/MCU.cpp src/Transform.cpp src/Utility.cpp)

set_property(TARGET kpeg PROPERTY CXX_STANDARD 14)
set_property(TARGET kpeg PROPERTY CXX_STANDARD_REQUIRED ON)