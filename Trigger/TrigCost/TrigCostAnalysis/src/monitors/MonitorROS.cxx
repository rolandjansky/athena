/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MonitorROS.h"
#include "../counters/CounterROS.h"

#include <algorithm>

MonitorROS::MonitorROS(const std::string& name, const MonitoredRange* parent)
  : MonitorBase(name, parent) {
}


StatusCode MonitorROS::newEvent(const CostData& data, const float weight) {

  // Prepare ROB id per corresponding ROS name map
  if (m_robToRos.empty()) {
    const std::map<std::string, std::vector<uint32_t>> rosToRobMap = data.rosToRobMap();
    for (auto rosRequest : rosToRobMap) {
      for (uint32_t robId : rosRequest.second) {
        m_robToRos[robId] = rosRequest.first;
      }
    }
  }

  for (const xAOD::TrigComposite* tc : data.rosCollection()) {
    auto robIds = tc->getDetail<std::vector<uint32_t>>("robs_id");

    // Create set of unique ROS for this request
    std::set<std::string> rosPerRequest;
    for (uint32_t robId : robIds) {
      rosPerRequest.insert(m_robToRos[robId]);
    }

    for (const std::string& rosName : rosPerRequest) {
      ATH_CHECK( getCounter(rosName)->newEvent(data, tc->index(), weight) );
    }
  }

  return StatusCode::SUCCESS;
}


std::unique_ptr<CounterBase> MonitorROS::newCounter(const std::string& name) {
  return std::make_unique<CounterROS>(name, this);
} 
