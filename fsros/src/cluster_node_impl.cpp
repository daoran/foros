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

#include "cluster_node_impl.hpp"

#include <rclcpp/node_interfaces/node_base.hpp>

#include <memory>
#include <string>

namespace akit {
namespace failsafe {
namespace fsros {

ClusterNodeImpl::ClusterNodeImpl(
    rclcpp::node_interfaces::NodeBaseInterface::SharedPtr node_base,
    rclcpp::node_interfaces::NodeServicesInterface::SharedPtr node_services,
    ClusterNodeInterface &node_interface)
    : node_base_(node_base),
      raft_fsm_(std::make_unique<raft::StateMachine>(node_services)),
      lifecycle_fsm_(std::make_unique<lifecycle::StateMachine>()),
      node_interface_(node_interface) {
  lifecycle_fsm_->Subscribe(this);
}

void ClusterNodeImpl::visit_publishers(
    std::function<void(std::shared_ptr<ClusterNodeInterface>)> f) {
  auto pub = publishers_.begin();
  while (pub != publishers_.end()) {
    auto pub_shared = pub->lock();
    if (pub_shared == nullptr) {
      pub = publishers_.erase(pub);
    } else {
      f(pub_shared);
    }
  }
}

void ClusterNodeImpl::handle(const lifecycle::StateType &state) {
  switch (state) {
    case lifecycle::StateType::kStandby:
      node_interface_.on_standby();
      visit_publishers([](auto publisher) { publisher->on_standby(); });
      break;
    case lifecycle::StateType::kActive:
      node_interface_.on_activated();
      visit_publishers([](auto publisher) { publisher->on_activated(); });
      break;
    case lifecycle::StateType::kInactive:
      node_interface_.on_deactivated();
      visit_publishers([](auto publisher) { publisher->on_deactivated(); });
      break;
    default:
      std::cerr << "Invalid lifecycle state : " << static_cast<int>(state)
                << std::endl;
      break;
  }
}

void ClusterNodeImpl::handle(const raft::StateType &state) {
  switch (state) {
    case raft::StateType::kStandby:
      break;
    case raft::StateType::kFollower:
      break;
    case raft::StateType::kCandidate:
      break;
    case raft::StateType::kLeader:
      break;
    default:
      std::cerr << "Invalid raft state : " << static_cast<int>(state)
                << std::endl;
      break;
  }
}

void ClusterNodeImpl::add_publisher(
    std::shared_ptr<ClusterNodeInterface> publisher) {
  publishers_.push_back(publisher);
}

void ClusterNodeImpl::remove_publisher(
    std::shared_ptr<ClusterNodeInterface> publisher) {
  auto pub = publishers_.begin();
  while (pub != publishers_.end()) {
    auto pub_shared = pub->lock();
    if (pub_shared == publisher) {
      pub = publishers_.erase(pub);
    }
  }
}

}  // namespace fsros
}  // namespace failsafe
}  // namespace akit
