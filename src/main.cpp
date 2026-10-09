#include "ur_io_ros2_manager/ur_io_ros2_manager.hpp"
int main(int argc, char * argv[])
{
  // Initialize ROS2, with the command line arguments
  rclcpp::init(argc, argv);

  // Create an instance of the PlaceManager class
  auto ur_interface = std::make_shared<UrIoInterface>();

  rclcpp::executors::MultiThreadedExecutor executor;
  executor.add_node(ur_interface);
  executor.spin();

  // when the loop is exited, shut down the node
  rclcpp::shutdown();
  return 0;
}

