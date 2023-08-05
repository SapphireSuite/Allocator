# Copyright (c) 2023 Sapphire's Suite. All Rights Reserved.



# Project

project(SA_AllocatorProto)



# Executable

## Add executable target.
add_executable(SA_AllocatorProto "main.cpp")



# Dependencies

### Add library dependencies.
target_link_libraries(SA_AllocatorProto PRIVATE SA_Allocator)



# Tests

add_test(NAME CSA_AllocatorProto COMMAND SA_AllocatorProto)
