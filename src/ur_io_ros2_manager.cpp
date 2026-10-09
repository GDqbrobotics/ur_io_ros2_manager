#include "ur_io_ros2_manager/ur_io_ros2_manager.hpp"

using std::placeholders::_1;

UrIoInterface::UrIoInterface()
    : Node("ur_io_ros2_manager"){
      rclcpp::SubscriptionOptions options;
      options.callback_group = this->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
      io_states_subscription_ = this->create_subscription<ur_msgs::msg::IOStates>("/io_and_status_controller/io_states", 10, std::bind(&UrIoInterface::io_states_callback, this, _1),options);
      set_io_client_ = this->create_client<ur_msgs::srv::SetIO>("/io_and_status_controller/set_io");
      init_interface_config();
    }

UrIoInterface::~UrIoInterface() = default;

void UrIoInterface::io_states_callback(const ur_msgs::msg::IOStates & msg) {
  if (latch_inputs_.size() < msg.digital_in_states.size()) {
    latch_inputs_.resize(msg.digital_in_states.size(), false);
  }

  for(int i=0; i<msg.digital_in_states.size(); i++){
    if(msg.digital_in_states[i].state == true && latch_inputs_[i] == false){
      std_msgs::msg::Bool messg;
      messg.data = true;
      ros2_topics_input_publishers_[i]->publish(messg);
      latch_inputs_[i] = true;
    } else if(msg.digital_in_states[i].state == false && latch_inputs_[i] == true){
      latch_inputs_[i] = false; 
      std_msgs::msg::Bool messg;
      messg.data = false;
      ros2_topics_input_publishers_[i]->publish(messg);   
    };
  }
}

void UrIoInterface::init_interface_config(){
  // Load the configuration file
  std::string config_file_path = this->declare_parameter<std::string>("config_file_path", "config/config.yaml");
  YAML::Node config = YAML::LoadFile(config_file_path);

  // Initialize the publishers and subscriptions for the topics used to interface with the UR Digital IOs
  for (const auto& io : config["digital_io"]) {
    std::string topic_name = io["topic_name"].as<std::string>();
    int io_index = io["io_index"].as<int>();
    std::string io_type = io["io_type"].as<std::string>();

    if (io_type == "input") {
      ros2_topics_input_publishers_[io_index] = this->create_publisher<std_msgs::msg::Bool>(topic_name, 10);
    } else if (io_type == "output") {
      ros2_topics_output_subscriptions_.push_back(this->create_subscription<std_msgs::msg::Bool>(topic_name, 10, [this, io_index](const std_msgs::msg::Bool & msg) {
        auto request = std::make_shared<ur_msgs::srv::SetIO::Request>();
        request->fun = ur_msgs::srv::SetIO::Request::FUN_SET_DIGITAL_OUT;
        request->pin = io_index;
        request->state = msg.data;
        set_io_client_->async_send_request(request);
      }));
    }
  }
}
