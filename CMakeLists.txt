#
# Copyright (c) 2020 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#
cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

set(PROJECT_VERSION "1.0.0")

project(smart_sensor VERSION ${PROJECT_VERSION})

include_directories(src/)

# Include the source files from the main folder
FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})

# Include source and header files from the subdirectories:
FILE(GLOB_RECURSE SOURCE_FILES 
    LIST_DIRECTORIES false
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.c"
    )

# Add source to the app target
target_sources(app PRIVATE ${SOURCE_FILES})