# Copyright (c) 2011 Simon Leblanc, for the Couzin Lab @ Princeton University
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

cmake_minimum_required(VERSION 2.8)

# Project name
project(DemoFrustum CXX)

# Needs Irrlicht
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
find_package(Irrlicht REQUIRED)

# Executable from headers and sources
file(GLOB Headers *.h)
file(GLOB Sources *.cpp)
add_executable(demofrustum ${Headers} ${Sources})

# Enable all warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

if(APPLE)
    # Irrlicht needs some frameworks
    find_library(COCOA Cocoa)
    find_library(OPENGL OpenGL)
    find_library(CARBON Carbon)
    find_library(IOKIT IOKit)
    set(EXTRA_LIBS ${COCOA} ${OPENGL} ${CARBON} ${IOKIT})
    # Irrlicht also prefer sources to be compiled with visibility "hidden"
    set_source_files_properties(${Sources} COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
endif(APPLE)

# Include directory
include_directories(${PROJECT_SOURCE_DIR} ${IRRLICHT_INCLUDE_DIR})

# Linking libraries
target_link_libraries(demofrustum ${IRRLICHT_LIBRARIES} ${EXTRA_LIBS})
