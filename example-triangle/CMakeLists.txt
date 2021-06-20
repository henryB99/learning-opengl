cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
cmake_policy(VERSION 3.16)

set(PROJECT_NAME "example-triangle.out")

project(${PROJECT_NAME} C CXX)

file(
    GLOB_RECURSE PROJECT_FILES
    CONFIGURE_DEPENDS
    "src/**/*.cpp"
    "src/**/*.hpp"
    "src/main.cpp"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

add_executable(${PROJECT_NAME} ${PROJECT_FILES})

target_include_directories(${PROJECT_NAME} BEFORE PRIVATE src ${OPEN_GL_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE GL glfw ${GLEW_LIBRARIES})
