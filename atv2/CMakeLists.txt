cmake_minimum_required(VERSION 3.1)
project(atv2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_GXX_FLAGS "-Wall -Wextra -Wconversion  -pedantic")

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(face_splitter src/haar_face_splitter.cpp)
target_link_libraries(face_splitter ${OpenCV_LIBS})