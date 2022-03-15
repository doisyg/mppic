#ifndef MPPIC__CRITIC_MANAGER_HPP_
#define MPPIC__CRITIC_MANAGER_HPP_

#include <xtensor/xtensor.hpp>
#include <pluginlib/class_loader.hpp>

#include "rclcpp_lifecycle/lifecycle_node.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "geometry_msgs/msg/twist_stamped.hpp"
#include "nav_msgs/msg/path.hpp"
#include "nav2_costmap_2d/costmap_2d_ros.hpp"

#include "mppic/critic_function.hpp"
#include "mppic/utils.hpp"

namespace mppi
{

class CriticManager
{
public:
  CriticManager() = default;

  void on_configure(
    rclcpp_lifecycle::LifecycleNode::WeakPtr parent,
    const std::string & name,
    std::shared_ptr<nav2_costmap_2d::Costmap2DROS> costmap_ros);

  std::string getFullName(const std::string & name);

  void getParams();

  void loadCritics();

  /**
   * @brief Evaluate cost for each trajectory
   *
   * @param trajectories: tensor of shape [ ..., ..., 3 ]
   * where 3 stands for x, y, yaw
   * @return Cost for each trajectory
   */
  xt::xtensor<double, 1> evalTrajectoriesScores(
    const xt::xtensor<double, 3> & trajectories, const nav_msgs::msg::Path & global_plan,
    const geometry_msgs::msg::PoseStamped & robot_pose) const;

protected:
  rclcpp_lifecycle::LifecycleNode::WeakPtr parent_;
  std::shared_ptr<nav2_costmap_2d::Costmap2DROS> costmap_ros_;
  std::string name_;

  std::vector<std::string> critics_names_;
  std::unique_ptr<pluginlib::ClassLoader<critics::CriticFunction>> loader_;
  std::vector<std::unique_ptr<critics::CriticFunction>> critics_;

  rclcpp::Logger logger_{rclcpp::get_logger("MPPIController")};
};

} // namespace mppi

#endif  // MPPIC__CRITIC_MANAGER_HPP_