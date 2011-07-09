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

# Headers, sources and resources
file(GLOB Headers *.h)
file(GLOB Sources *.cpp)
file(GLOB Resources media/*)

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
    # Make a proper app bundle
    set_source_files_properties(${Resources} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set(MACOSX_BUNDLE_ICON_FILE cube)
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "edu.princeton.\${PRODUCT_NAME:identifier}")
    set(MACOSX_BUNDLE_BUNDLE_NAME "Demo Frustum")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "0.2")
    set(MACOSX_BUNDLE_COPYRIGHT "2011 Simon Leblanc, Couzin Lab @ Princeton University")
    set(TARGET_NAME "Demo Frustum")
    set(TARGET_TYPE "MACOSX_BUNDLE")
endif(APPLE)

# Executable from headers, sources and resources
add_executable(${TARGET_NAME} ${TARGET_TYPE} ${Headers} ${Sources} ${Resources})

# Include directory
include_directories(${PROJECT_SOURCE_DIR} ${IRRLICHT_INCLUDE_DIR})

# Linking libraries
target_link_libraries(${TARGET_NAME} ${IRRLICHT_LIBRARIES} ${EXTRA_LIBS})
