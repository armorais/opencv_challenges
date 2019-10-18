find_package(OpenCV 4)

set (CMAKE_CXX_STANDARD 11)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(vignette src/vignette.cpp)
target_link_libraries(vignette ${OpenCV_LIBRARIES})

add_executable(red_segmentation src/red_segmentation.cpp)
target_link_libraries(red_segmentation ${OpenCV_LIBRARIES})