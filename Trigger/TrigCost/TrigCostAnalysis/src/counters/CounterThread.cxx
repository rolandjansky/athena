/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODTrigger/TrigCompositeContainer.h"

#include "CounterThread.h"

CounterThread::CounterThread(const std::string& name, const MonitorBase* parent) 
  : CounterBase(name, parent), m_lowTimestamp(std::numeric_limits<uint64_t>::max()), m_highTimestamp(0),
  m_globalLowTimestamp(std::numeric_limits<uint64_t>::max()), m_globalHighTimestamp(0)
{
  regHistogram("AlgTime_perEvent", "Algorithm CPU Time/Event;Time [ms];Events", VariableType::kPerEvent);
  regHistogram("FrameworkTime_perEvent", "Framework CPU Tim/Evente;Time [ms];Events", VariableType::kPerEvent);
  regHistogram("UnmonitoredTime_perEvent", "Unmonitored CPU Tim/Evente;Time [ms];Events", VariableType::kPerEvent);

  regHistogram("AlgTime_perEventFractional", "Algorithm CPU Time Fractional;Fractional Time;Events", VariableType::kPerEvent, kLinear, 0., 1.);
  regHistogram("FrameworTime_perEventFractional", "CPU Time/Event CPU Time Fractional;Fractional Time;Events", VariableType::kPerEvent, kLinear, 0., 1.);
  regHistogram("UnmonitoredTime_perEventFractional", "Unmonitored CPU Time Fractional;Fractional Time;Events", VariableType::kPerEvent, kLinear, 0., 1.);


  regHistogram("AlgCalls_perEvent", "Algorithm Calls/Event;Calls;Events", VariableType::kPerEvent, kLinear, -0.5, 499.5);
  regHistogram("AlgSlot_perCall", "Algorithm Slot/Call;Slot;Calls", VariableType::kPerCall, kLinear, -0.5, 20.5, 21);

}


StatusCode CounterThread::newEvent(const CostData& data, size_t index, const float weight) {
  const xAOD::TrigComposite* alg = data.costCollection().at(index);

  const uint64_t start = alg->getDetail<uint64_t>("start"); // in mus
  const uint64_t stop  = alg->getDetail<uint64_t>("stop"); // in mus
  const uint32_t slot  = alg->getDetail<uint32_t>("slot");
  const float cpuTime = timeToMilliSec(start, stop);
  ATH_CHECK( fill("AlgTime_perEvent", cpuTime, weight) );
  ATH_CHECK( fill("AlgTime_perEventFractional", cpuTime, weight) );  
  ATH_CHECK( increment("AlgCalls_perEvent", weight) );
  ATH_CHECK( fill("AlgSlot_perCall", slot, weight) );
  m_lowTimestamp = std::min(m_lowTimestamp, start);
  m_highTimestamp = std::max(m_highTimestamp, stop);

  return StatusCode::SUCCESS;
}

StatusCode CounterThread::postProcess(float weight) {
  const float eventTime = timeToMilliSec(m_globalLowTimestamp, m_globalHighTimestamp);
  float unmonitoredTime = 0;
  if (m_highTimestamp > 0) {
    unmonitoredTime = timeToMilliSec(m_globalLowTimestamp, m_lowTimestamp) + timeToMilliSec(m_highTimestamp, m_globalHighTimestamp);
  }
  const float frameWorkTime = eventTime - unmonitoredTime - getVariable("AlgTime_perEvent").getAccumulator();

  ATH_CHECK( fill("UnmonitoredTime_perEvent", unmonitoredTime, weight) );
  ATH_CHECK( fill("UnmonitoredTime_perEventFractional", unmonitoredTime, weight) );

  ATH_CHECK( fill("FrameworkTime_perEvent", frameWorkTime, weight) );
  ATH_CHECK( fill("FrameworTime_perEventFractional", frameWorkTime, weight) );

  ATH_CHECK( setDenominator("AlgTime_perEventFractional", eventTime) );
  ATH_CHECK( setDenominator("FrameworTime_perEventFractional", eventTime) );
  ATH_CHECK( setDenominator("UnmonitoredTime_perEventFractional", eventTime) );

  m_lowTimestamp = std::numeric_limits<uint64_t>::max();
  m_highTimestamp = 0;
  m_globalLowTimestamp = std::numeric_limits<uint64_t>::max();
  m_globalHighTimestamp = 0;

  return StatusCode::SUCCESS; 
}


uint64_t CounterThread::getLowTimestamp() const {
  return m_lowTimestamp;
}


uint64_t CounterThread::getHighTimestamp() const {
  return m_highTimestamp;
}


void CounterThread::setAllThreadsTimestamps(uint64_t low, uint64_t high) {
  m_globalLowTimestamp = low;
  m_globalHighTimestamp = high;
}
