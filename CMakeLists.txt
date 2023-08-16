cmake_minimum_required(VERSION 3.10)

project(Chess)

# Add your source files
add_executable(aurora src/main.cpp)

# Include directories (if needed)
# target_include_directories(aurora PRIVATE include)

# Optional: Add libraries and dependencies
# find_package(SomeLibrary REQUIRED)
# target_link_libraries(MyExecutable PRIVATE SomeLibrary::SomeLibrary)