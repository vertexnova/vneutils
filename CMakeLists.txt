#==============================================================================
# Copyright (c) 2025 Ajeet Singh Yadav. All rights reserved.
# Licensed under the Apache License, Version 2.0 (the "License")
#
# Author:    Ajeet Singh Yadav
# Created:   January 2025
#
# Autodoc:   yes
#==============================================================================

cmake_minimum_required(VERSION 3.16)

project(vneutils VERSION 1.0.0 LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

#==============================================================================
# Add custom CMake modules path
#==============================================================================
# VneCMake shared modules (submodule)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/vnecmake/modules")
# Project-specific modules
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

#==============================================================================
# Project Setup
#==============================================================================
include(ProjectSetup)

#==============================================================================
# Build Options
#==============================================================================

option(BUILD_TESTS "Build the test suite" ON)
option(BUILD_EXAMPLES "Build example programs" OFF)

# Enable cache system
option(ENABLE_CACHE "Enable compiler cache if available" OFF)
include(CCache)

# Enable static code analysis tools options
option(ENABLE_CPPCHECK "Enable static analysis with cppcheck" OFF)
include(CppCheck)
option(ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" OFF)
include(ClangTidy)

#==============================================================================
# Compiler Warnings
#==============================================================================

add_library(UtilsWarnings INTERFACE)
include(ProjectWarnings)
set_project_warnings(UtilsWarnings)
add_library(vne::utils::Warnings ALIAS UtilsWarnings)

#==============================================================================
# Build Settings Interface Library
#==============================================================================

add_library(UtilsBuildSettings INTERFACE)
add_library(vne::utils::BuildSettings ALIAS UtilsBuildSettings)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(UtilsBuildSettings INTERFACE VNE_BUILD_TYPE_DEBUG VNE_DEVELOPER_BUILD)
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_definitions(UtilsBuildSettings INTERFACE VNE_BUILD_TYPE_RELEASE)
endif()

#==============================================================================
# Include Directories
#==============================================================================

set(VNE_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(VNE_SRC_DIR ${PROJECT_SOURCE_DIR}/src)

#==============================================================================
# Library
#==============================================================================

add_library(vneutils STATIC
    src/command_line_parser.cpp
)

add_library(vne::utils ALIAS vneutils)

target_include_directories(vneutils
    PUBLIC
        $<BUILD_INTERFACE:${VNE_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${VNE_SRC_DIR}
)

target_link_libraries(vneutils
    PRIVATE
        vne::utils::Warnings
        vne::utils::BuildSettings
)

#==============================================================================
# Platform-specific linking
#==============================================================================

if(NOT WIN32)
    target_link_libraries(vneutils PRIVATE pthread)
endif()

#==============================================================================
# Tests
#==============================================================================

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

#==============================================================================
# Examples
#==============================================================================

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
