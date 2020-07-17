/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MonitorGlobal.h"
#include "../counters/CounterGlobal.h"

MonitorGlobal::MonitorGlobal(const std::string& name, const MonitoredRange* parent)
  : MonitorBase(name, parent) {
}

StatusCode MonitorGlobal::newEvent(const CostData& data, const float weight) {

  std::stringstream ss;
  ss << "LumiBlock_" << std::setfill('0') << std::setw(5) << data.lb();
  const size_t incrementWalltime = ( data.liveTimeIsPerEvent() or !counterExists(ss.str()) ) ? 1 : 0;

  ATH_CHECK( getCounter(ss.str())->newEvent(data, incrementWalltime, weight) );
  ATH_CHECK( getCounter("All")->newEvent(data, incrementWalltime, weight) );

  return StatusCode::SUCCESS;
}


std::unique_ptr<CounterBase> MonitorGlobal::newCounter(const std::string& name) {
  return std::make_unique<CounterGlobal>(name, this);
} 
