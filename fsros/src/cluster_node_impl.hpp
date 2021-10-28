/*
 * Copyright (c) 2021 42dot All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AKIT_FAILSAFE_FSROS_CLUSTER_NODE_IMPL_HPP_
#define AKIT_FAILSAFE_FSROS_CLUSTER_NODE_IMPL_HPP_

#include <rclcpp/node_interfaces/node_base_interface.hpp>
#include <rclcpp/node_interfaces/node_clock_interface.hpp>
#include <rclcpp/node_interfaces/node_graph_interface.hpp>
#include <rclcpp/node_interfaces/node_services_interface.hpp>
#include <rclcpp/node_interfaces/node_timers_interface.hpp>
#include <rclcpp/node_options.hpp>

#include <functional>
#include <list>
#include <memory>
#include <string>
#include <vector>

#include "akit/failsafe/fsros/cluster_node_interface.hpp"
#include "akit/failsafe/fsros/cluster_node_options.hpp"
#include "common/observer.hpp"
#include "lifecycle/state_machine.hpp"
#include "lifecycle/state_type.hpp"
#include "raft/context.hpp"
#include "raft/state_machine.hpp"

namespace akit {
namespace failsafe {
namespace fsros {

class ClusterNodeImpl final : Observer<lifecycle::StateType>,
                              Observer<raft::StateType> {
 public:
  explicit ClusterNodeImpl(
      const std::vector<std::string> &cluster_node_names,
      rclcpp::node_interfaces::NodeBaseInterface::SharedPtr node_base,
      rclcpp::node_interfaces::NodeGraphInterface::SharedPtr node_graph,
      rclcpp::node_interfaces::NodeServicesInterface::SharedPtr node_services,
      rclcpp::node_interfaces::NodeTimersInterface::SharedPtr node_timers,
      rclcpp::node_interfaces::NodeClockInterface::SharedPtr node_clock,
      ClusterNodeInterface &node_interface, const ClusterNodeOptions &options);

  ~ClusterNodeImpl();

  void handle(const lifecycle::StateType &state) override;
  void handle(const raft::StateType &state) override;

  void add_publisher(std::shared_ptr<ClusterNodeInterface> publisher);
  void remove_publisher(std::shared_ptr<ClusterNodeInterface> publisher);

 private:
  void visit_publishers(
      std::function<void(std::shared_ptr<ClusterNodeInterface>)> f);

  std::shared_ptr<raft::Context> raft_context_;
  std::unique_ptr<raft::StateMachine> raft_fsm_;
  std::unique_ptr<lifecycle::StateMachine> lifecycle_fsm_;
  ClusterNodeInterface &node_interface_;
  std::list<std::weak_ptr<ClusterNodeInterface>> publishers_;
};

}  // namespace fsros
}  // namespace failsafe
}  // namespace akit

#endif  // AKIT_FAILSAFE_FSROS_CLUSTER_NODE_IMPL_HPP_
