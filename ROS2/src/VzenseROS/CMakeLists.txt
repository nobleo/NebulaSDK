cmake_minimum_required(VERSION 3.5)
set(CMAKE_INSTALL_RPATH "$ORIGIN/")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
project(vzenseros)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(pcl_conversions REQUIRED) 
find_package(PCL REQUIRED COMPONENTS common io)
find_package(image_transport REQUIRED)
#find_package(boost REQUIRED)

add_definitions(${PCL_DEFINITIONS})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # uncomment the line when a copyright and license is not present in all source files
  #set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # uncomment the line when this package is not in a git repo
  #set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()


find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem)

find_library(libNebula_api NAMES libNebula_api.so PATHS dependencies/Lib/)
include_directories(${PCL_INCLUDE_DIRS})
include_directories(include include dependencies/Include ${OpenCV_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} 
 /usr/include/eigen3/
 /usr/include/boost/)

 include_directories(${tf2_geometry_msgs_INCLUDE_DIRS})
 include_directories(${tf2_ros_INCLUDE_DIRS})
 
 
LINK_DIRECTORIES(
    ${PCL_LIBRARY_DIRS})
 
add_executable(vzense_camera src/vzense_driver.cpp
                                 src/vzense_manager.cpp
                                 include/vzense_manager.hpp)

file(GLOB PCL_LIBRARIES /usr/lib/x86_64-linux-gnu/libpcl_*)
                                 
target_link_libraries(vzense_camera  ${libNebula_api} ${OpenCV_LIBRARIES} ${PCL_LIBRARIES} ${Boost_LIBRARIES})

set(dependencies
  cv_bridge
  image_transport
  rclcpp
  rclcpp_components
  std_msgs
  sensor_msgs
  tf2
  tf2_geometry_msgs
  tf2_ros
  pcl_conversions
)

# ament_target_dependencies(vzense_camera rclcpp sensor_msgs std_msgs cv_bridge tf2 tf2_geometry_msgs image_transport tf2_ros pcl_conversions)

ament_target_dependencies(vzense_camera
  ${dependencies}
)


install(
  DIRECTORY dependencies/Lib/ 
  DESTINATION lib/${PROJECT_NAME}  
)

install(TARGETS
  vzense_camera
  DESTINATION lib/${PROJECT_NAME}
)

ament_export_dependencies(${dependencies})

ament_package()
