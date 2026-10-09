#include <functional>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "std_msgs/msg/bool.hpp"

#include <ur_msgs/msg/io_states.hpp> // count reset made by universal robot digital input
#include <ur_msgs/srv/set_io.hpp> // service to set the state of the digital outputs of the robot
#include <yaml-cpp/yaml.h>

/**
 * @class UrIoInterface
 * @brief This class is responsible of interfacing UR Digital IOs with the topics used by qb-integration.
 *
 * This class listen to the topic "/io_and_status_controller/io_states" published by the ur_robot_driver
 * and publish on the topics "reset_count" and "task_pause" according to the state of the digital inputs.
 *
 * The latch_reset_ and latch_pause_ variables are used to avoid publishing the same message multiple times
 * when the state of the digital inputs does not change.
 *
 * @author Giuliano Dami
 */

class UrIoInterface : public rclcpp::Node {
public:
  /**
   * @brief Constructor.
   */
  UrIoInterface();

  /**
   * @brief Destructor.
   */
  ~UrIoInterface();

private:
  /**
   * @brief This callback is called when a new message is published on the topic "/io_and_status_controller/io_states".
   * The message is a struct which contains the state of all the digital inputs and outputs of the robot.
   */
  void io_states_callback(const ur_msgs::msg::IOStates & msg);

  /**
   * @brief Initialize the Publishers and Subscriptions for the topics used to interface with the UR Digital IOs.
   * It links Digital IOs to the topics following the configuration specified in the config/config.yaml file.
   */
  void init_interface_config();

  /**
   * @brief Subscription to the topic "/io_and_status_controller/io_states".
   */
  rclcpp::Subscription<ur_msgs::msg::IOStates>::SharedPtr io_states_subscription_;

  /**
   * @brief Subscriptions to the topics that will be used to trigger the digital outputs of the robot.
   */
  std::vector<rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr> ros2_topics_output_subscriptions_;

  /**
   * @brief Publishers to the topics that will be used to publish the state of the digital inputs of the robot.
   */
  std::map<int, rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr> ros2_topics_input_publishers_;

  /**
    * @brief Client for service SetIO. This service is used to set the state of the digital outputs of the robot.
    */ 
  rclcpp::Client<ur_msgs::srv::SetIO>::SharedPtr set_io_client_; 

  /**
   * @brief Latch to avoid publishing the same message multiple times when the state of the digital inputs does not change.
   */
  std::vector<bool> latch_inputs_;
};

