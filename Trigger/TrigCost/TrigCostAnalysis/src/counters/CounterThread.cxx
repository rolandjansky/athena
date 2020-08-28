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
  regHistogram("FrameworkTime_perEvent", "Framework CPU Time/Event;Time [ms];Events", VariableType::kPerEvent);
  regHistogram("UnmonitoredTime_perEvent", "Unmonitored CPU Time/Event;Time [ms];Events", VariableType::kPerEvent);
  regHistogram("WallTime_perEvent", "Total walltime from HLT start to stop/Event;Time [ms];Events", VariableType::kPerEvent);

  regHistogram("AlgTime_perEventFractional", "Algorithm CPU Time Fractional;Fractional Time;Events", VariableType::kPerEvent, kLinear, 0., 1.);
  regHistogram("FrameworkTime_perEventFractional", "Framework Time/Event CPU Time Fractional;Fractional Time;Events", VariableType::kPerEvent, kLinear, 0., 1.);
  regHistogram("UnmonitoredTime_perEventFractional", "Unmonitored CPU Time Fractional;Fractional Time;Events", VariableType::kPerEvent, kLinear, 0., 1.);


  regHistogram("AlgCalls_perEvent", "Algorithm Calls/Event;Calls;Events", VariableType::kPerEvent, kLinear, -0.5, 499.5);
  regHistogram("ThisAlgCalls_perEvent", "This Event Algorithm Calls/Event;Calls;Events", VariableType::kPerEvent, kLinear, -0.5, 499.5);
  regHistogram("OtherAlgCalls_perEvent", "Other Event Algorithm Calls/Event;Calls;Events", VariableType::kPerEvent, kLinear, -0.5, 499.5);
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
  if (slot == 0) {
    ATH_CHECK( increment("ThisAlgCalls_perEvent", weight) );
  } else {
    ATH_CHECK( increment("OtherAlgCalls_perEvent", weight) );
  }
  ATH_CHECK( fill("AlgSlot_perCall", slot, weight) );
  m_lowTimestamp = std::min(m_lowTimestamp, start);
  m_highTimestamp = std::max(m_highTimestamp, stop);

  return StatusCode::SUCCESS;
}

StatusCode CounterThread::postProcess(float weight) {
  // Total time from start of L1Decoder to "stop" of SummaryMaker.
  const float eventTime = timeToMilliSec(m_globalLowTimestamp, m_globalHighTimestamp);
  
  // We know that we may loose data on algs which started executing before the L1Decoder triggered the data collection.
  // But we truncate the data for algs which are running still when we stop collecting data.
  float unmonitoredTime = 0;
  if (m_highTimestamp > 0) {
    const float unmonitoredStart = timeToMilliSec(m_globalLowTimestamp, m_lowTimestamp);
    const float unmonitoredEnd = timeToMilliSec(m_highTimestamp, m_globalHighTimestamp);
    unmonitoredTime = unmonitoredStart + unmonitoredEnd;
    // As of T12, unmonitoredEnd should be zero due to truncating this record online.
  }

  // Time spent outside of any algorithm. E.g. in Gaudi
  const float frameworkTime = eventTime - unmonitoredTime - getVariable("AlgTime_perEvent").getAccumulator();

  ATH_CHECK( fill("UnmonitoredTime_perEvent", unmonitoredTime, weight) );
  ATH_CHECK( fill("UnmonitoredTime_perEventFractional", unmonitoredTime, weight) );

  ATH_CHECK( fill("FrameworkTime_perEvent", frameworkTime, weight) );
  ATH_CHECK( fill("FrameworkTime_perEventFractional", frameworkTime, weight) );

  ATH_CHECK( fill("WallTime_perEvent", eventTime, weight) );

  ATH_CHECK( setDenominator("AlgTime_perEventFractional", eventTime) );
  ATH_CHECK( setDenominator("FrameworkTime_perEventFractional", eventTime) );
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
