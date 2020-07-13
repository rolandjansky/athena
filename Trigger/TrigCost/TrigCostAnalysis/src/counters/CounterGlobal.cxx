/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODTrigger/TrigCompositeContainer.h"

#include "CounterGlobal.h"

CounterGlobal::CounterGlobal(const std::string& name, const MonitorBase* parent) 
  : CounterBase(name, parent)
{
  regHistogram("AlgTime_perCall", "Algorithm CPU Time/Call;Time [ms];Events", VariableType::kPerCall);
  regHistogram("AlgTime_perEvent", "Algorithm CPU Time/Event;Time [ms];Events", VariableType::kPerEvent);
  regHistogram("AlgCalls_perEvent", "Algorithm Calls/Event;Calls;Events", VariableType::kPerEvent, kLinear, -0.5, 499.5);
  regHistogram("SteeringTime_perEvent", "Steering Time/Event;Time [ms];Events", VariableType::kPerEvent);
  regHistogram("LbLength", "LBLength;Entry;LB Length [s]", VariableType::kPerEvent, kLinear, -0.5, 0.5, 1);
}


StatusCode CounterGlobal::newEvent(const CostData& data, size_t incrementWalltime, const float weight) {

  uint64_t lowTimestamp = std::numeric_limits<uint64_t>::max();
  uint64_t highTimestamp = 0;
  for (const xAOD::TrigComposite* tc : data.costCollection()) {
    const uint32_t slot = tc->getDetail<uint32_t>("slot");
    if (slot != data.onlineSlot()) {
      continue; // When monitoring the master slot, this Counter ignores algs running in different slots 
    }

    const uint64_t start = tc->getDetail<uint64_t>("start"); // in mus
    const uint64_t stop  = tc->getDetail<uint64_t>("stop"); // in mus
    const float cpuTime = timeToMilliSec(start, stop);
    ATH_CHECK( fill("AlgTime_perCall", cpuTime, weight) );
    ATH_CHECK( fill("AlgTime_perEvent", cpuTime, weight) );
    ATH_CHECK( increment("AlgCalls_perEvent", weight) );
    lowTimestamp = std::min(lowTimestamp, start);
    highTimestamp = std::max(highTimestamp, stop);
  }

  const float steeringTime = timeToMilliSec(lowTimestamp, highTimestamp);
  ATH_CHECK( fill("SteeringTime_perEvent", steeringTime, weight) );

  if (incrementWalltime == 1) {
    // Note: This accumulator doesn't use the standard weight.
    // For P1 data taking it will be filled exactly once per LB
    ATH_CHECK( fill("LbLength", 0.0, data.liveTime()) );
  }

  return StatusCode::SUCCESS;
}
