// Copyright 2022 FastSense, Samsung Research
#include "mppic/critics/twirling_critic.hpp"

namespace mppi::critics
{

void TwirlingCritic::initialize()
{
  auto node = parent_.lock();

  auto getParam = utils::getParamGetter(node, name_);
  getParam(power_, "twirling_cost_power", 1);
  getParam(weight_, "twirling_cost_weight", 10.0);

  RCLCPP_INFO(
    logger_, "TwirlingCritic instantiated with %d power and %f weight.", power_, weight_);
}

void TwirlingCritic::score(
  const geometry_msgs::msg::PoseStamped & robot_pose, const xt::xtensor<double, 3> & trajectories,
  const xt::xtensor<double, 2> & path, xt::xtensor<double, 1> & costs,
  nav2_core::GoalChecker * goal_checker)
{
  auto goal_x = xt::view(path, -1, 0);
  auto goal_y = xt::view(path, -1, 1);
  auto traj_xs = xt::view(trajectories, xt::all(), xt::all(), 0);
  auto traj_ys = xt::view(trajectories, xt::all(), xt::all(), 1);
  auto traj_yaws = xt::view(trajectories, xt::all(), xt::all(), 2);

  auto yaws_between_points = atan2(goal_y - traj_ys, goal_x - traj_xs);
  auto yaws = xt::abs(traj_yaws - yaws_between_points);

  costs += xt::pow(xt::mean(yaws, {1}) * weight_, power_);
}

}  // namespace mppi::critics

#include <pluginlib/class_list_macros.hpp>

PLUGINLIB_EXPORT_CLASS(mppi::critics::TwirlingCritic, mppi::critics::CriticFunction)
