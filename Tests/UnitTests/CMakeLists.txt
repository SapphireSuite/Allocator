# Copyright (c) 2023 Sapphire's Suite. All Rights Reserved.



# Project

project(SA_AllocatorUT)

SA_FetchGoogleTest()



# Executable

## Inputs

file(GLOB_RECURSE SA_Allocator_UT_SOURCES "*.cpp")

## Add executable target.
add_executable(SA_AllocatorUT ${SA_Allocator_UT_SOURCES})



# Dependencies

### Add library dependencies.
target_link_libraries(SA_AllocatorUT PRIVATE SA_Logger)

### Google test module dependency.
target_link_libraries(SA_AllocatorUT PRIVATE gtest)



# Tests

add_test(NAME CSA_AllocatorUT COMMAND SA_AllocatorUT)
