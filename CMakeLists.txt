cmake_minimum_required(VERSION 2.8.3)
project(roshz)

find_package(catkin REQUIRED COMPONENTS
  roscpp
)

include_directories(include ${catkin_INCLUDE_DIRS})
link_directories(${catkin_LIBRARY_DIRS})
add_definitions(${catkin_DEFINITIONS})

add_executable(${PROJECT_NAME}
  src/roshz.cpp
  src/statistics.cpp
)

target_link_libraries(${PROJECT_NAME}
  ${catkin_LIBRARIES}
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
