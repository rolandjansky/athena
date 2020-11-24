/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MonitorROS.h"
#include "../counters/CounterROS.h"

MonitorROS::MonitorROS(const std::string& name, const MonitoredRange* parent)
  : MonitorBase(name, parent) {
}


StatusCode MonitorROS::newEvent(const CostData& /*data*/, const float /*weight*/) {

  return StatusCode::SUCCESS;
}


std::unique_ptr<CounterBase> MonitorROS::newCounter(const std::string& name) {
  return std::make_unique<CounterROS>(name, this);
} 
