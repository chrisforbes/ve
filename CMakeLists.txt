cmake_minimum_required(VERSION 3.15)
project(ve)

set(CMAKE_CXX_STANDARD 14)

add_executable(ve main.cpp ve.h shader.cpp vox.cpp)

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(GLEW REQUIRED)

target_link_libraries(ve OpenGL::GL)
target_link_libraries(ve glfw)
target_link_libraries(ve GLEW::glew)