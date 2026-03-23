cmake_minimum_required(VERSION 3.10)
project(radar LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_executable(radar main.cpp)
target_include_directories(radar PRIVATE ${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})
target_link_libraries(radar ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
