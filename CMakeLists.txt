cmake_minimum_required(VERSION 3.15)
project(ve)

set(CMAKE_CXX_STANDARD 14)

add_executable(ve
        main.cpp
        shader.cpp
        vox.cpp
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_impl_glfw.cpp
        imgui/imgui_impl_opengl3.cpp
        imgui/imgui_widgets.cpp
        )

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(GLEW REQUIRED)

target_link_libraries(ve OpenGL::GL)
target_link_libraries(ve glfw)
target_link_libraries(ve GLEW::glew)