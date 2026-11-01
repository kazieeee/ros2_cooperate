cmake_minimum_required(VERSION 3.10)
project(robot_speed_pkg2)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
)

catkin_package(
 CATKIN_DEPENDS roscpp std_msgs
)

include_directories(
  ${catkin_INCLUDE_DIRS}
)

add_executable(speed_publisher src/speed_publisher.cpp)
target_link_libraries(speed_publisher ${catkin_LIBRARIES})

add_executable(speed_monitor src/speed_monitor.cpp)
target_link_libraries(speed_monitor ${catkin_LIBRARIES})
