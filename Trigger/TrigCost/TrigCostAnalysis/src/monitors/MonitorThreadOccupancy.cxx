/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MonitorThreadOccupancy.h"
#include "../counters/CounterThread.h"

MonitorThreadOccupancy::MonitorThreadOccupancy(const std::string& name, const MonitoredRange* parent)
  : MonitorBase(name, parent),
  m_threadToCounterMap(),
  m_threadCounter(0) {}


StatusCode MonitorThreadOccupancy::newEvent(const CostData& data, const float weight) {

  // Only look at events in the master slot
  if (not data.isMasterSlot()) {
    return StatusCode::SUCCESS;
  }

  for (const xAOD::TrigComposite* tc : data.costCollection()) {
    const uint32_t threadID = tc->getDetail<uint32_t>("thread");
    if (m_threadToCounterMap.count(threadID) == 0) {
      std::stringstream threadIDStr;
      threadIDStr << "Thread_" << std::setfill('0') << std::setw(5) << threadID;
      m_threadToCounterMap[threadID] = threadIDStr.str();
    }
    ATH_CHECK( getCounter(m_threadToCounterMap[threadID])->newEvent(data, tc->index(), weight) );
  }

  ATH_CHECK( postProcess(weight) );


  return StatusCode::SUCCESS; 
}


StatusCode MonitorThreadOccupancy::postProcess(float weight) {
  uint64_t lowTimestamp = std::numeric_limits<uint64_t>::max();
  uint64_t highTimestamp = 0;
  for (const auto& nameCounterPair : m_counters) {
    const CounterThread* ptr = dynamic_cast<const CounterThread*>(nameCounterPair.second.get());
    lowTimestamp = std::min(lowTimestamp, ptr->getLowTimestamp());
    highTimestamp = std::max(highTimestamp, ptr->getHighTimestamp());
  }
  for (auto& nameCounterPair : m_counters) {
    CounterThread* ptr = dynamic_cast<CounterThread*>(nameCounterPair.second.get());
    ptr->setAllThreadsTimestamps(lowTimestamp, highTimestamp);
    ATH_CHECK( ptr->postProcess(weight) );
  }
  return StatusCode::SUCCESS;
}


std::unique_ptr<CounterBase> MonitorThreadOccupancy::newCounter(const std::string& name) {
  return std::make_unique<CounterThread>(name, this);
} 
