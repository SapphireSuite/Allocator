# Copyright (c) 2023 Sapphire's Suite. All Rights Reserved.

cmake_minimum_required(VERSION 3.16)

# Sapphire Suite's CMake tools.
add_subdirectory(ThirdParty/SA/CMake)



# Project

project(SA_Allocator)
SA_ConfigureProject(SA_Allocator)



# Library

add_library(SA_Allocator STATIC)
SA_ConfigureTarget(SA_Allocator)
SA_TargetSources(SA_Allocator)

# Options

## Add SA_Allocator's tests to build tree.
option(SA_Allocator_BUILD_TESTS_OPT "Should build SA_Allocator tests" OFF)



# Entrypoints

add_subdirectory(ThirdParty)

if(SA_ALLOCATOR_BUILD_TESTS_OPT)

	### Enable testing for this directory and below.
	enable_testing()

	add_subdirectory(Tests)
endif()
