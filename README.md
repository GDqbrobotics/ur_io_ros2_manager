# ur_io_ros2_manager

A ROS 2 package that bridges Universal Robots digital I/O with ROS topics.

## Overview

This node listens to the UR robot IO status topic and exposes configured digital inputs as ROS `std_msgs/Bool` topics. It also listens for Boolean commands on configured output topics and forwards them to the UR `SetIO` service.

This is useful when a robot cell needs to integrate UR digital I/O with higher-level ROS applications without directly coupling those applications to the UR driver API.

## Features

- Subscribes to `/io_and_status_controller/io_states`
- Publishes a ROS Boolean message whenever a configured digital input changes state
- Uses edge detection to avoid publishing repeated messages while a signal remains stable
- Subscribes to configured output topics and calls the UR `SetIO` service to set digital outputs
- Loads IO mapping from a YAML configuration file

## Configuration

The default configuration is defined in `config/config.yaml`.

Example:

```yaml
digital_io:
  - topic_name: /reset_count
    io_index: 0
    io_type: input
  - topic_name: /task_pause
    io_index: 1
    io_type: input
  - topic_name: /led_ring
    io_index: 0
    io_type: output
```

Each entry maps a digital IO index to a ROS topic name and type:

- `input`: the node publishes topic messages when the UR input changes state
- `output`: the node subscribes to the topic and sends the command to the robot output

## Build

From a ROS 2 workspace:

```bash
colcon build --packages-select ur_io_ros2_manager
```

## Run

Source the workspace and start the node:

```bash
source install/setup.bash
ros2 run ur_io_ros2_manager ur_io_ros2_manager_node
```

You can override the configuration file path at runtime with a parameter:

```bash
ros2 run ur_io_ros2_manager ur_io_ros2_manager_node --ros-args -p config_file_path:=/path/to/config.yaml
```

## Dependencies

- ROS 2
- `rclcpp`
- `std_msgs`
- `ur_msgs`
- `yaml-cpp`

## License

This project is licensed under the Apache 2.0 License.
