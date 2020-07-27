/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODTrigger/TrigCompositeContainer.h"

#include "CounterAlgorithm.h"

CounterAlgorithm::CounterAlgorithm(const std::string& name, const MonitorBase* parent) 
  : CounterBase(name, parent)
{
  regHistogram("Time_perCall", "CPU Time/Call;Time [ms];Calls", VariableType::kPerCall);
  regHistogram("FirstTime_perEvent", "First Call CPU Time;Time [ms];Events", VariableType::kPerCall);
  regHistogram("Time_perEvent", "CPU Time/Event;Time [ms];Events", VariableType::kPerEvent);
  regHistogram("Time_perEventFractional", "CPU Time/Event CPU Time;Fractional Time;Events", VariableType::kPerEvent, kLinear, 0., 1.);
  regHistogram("AlgCalls_perEvent", "Calls/Event;Calls;Events", VariableType::kPerEvent, kLinear, -0.5, 49.5);
  regHistogram("InEventView_perCall", "In Event View;Yes or No;Calls", VariableType::kPerCall, kLinear, -0.5, 1.5, 2);
  regHistogram("RoIID_perCall", "RoI ID;RoI ID;Calls", VariableType::kPerCall, kLinear, -1.5, 20.5, 22);
}


StatusCode CounterAlgorithm::newEvent(const CostData& data, size_t index, const float weight) {
  const xAOD::TrigComposite* alg = data.costCollection().at(index);

  const uint64_t start = alg->getDetail<uint64_t>("start"); // in mus
  const uint64_t stop  = alg->getDetail<uint64_t>("stop"); // in mus
  const float cpuTime = timeToMilliSec(start, stop);

  ATH_CHECK( fill("Time_perCall", cpuTime, weight) );

  ATH_CHECK( fill("Time_perEvent", cpuTime, weight) );

  Variable& firstTime_perEvent = getVariable("FirstTime_perEvent");
  if (firstTime_perEvent.getCalls() == 0) {
    ATH_CHECK( firstTime_perEvent.fill(cpuTime, weight) );
  }

  ATH_CHECK( setDenominator("Time_perEventFractional", data.algTotalTimeMilliSec()) );
  ATH_CHECK( fill("Time_perEventFractional", cpuTime, weight) );

  ATH_CHECK( increment("AlgCalls_perEvent", weight) );

  const bool globalScope = (TrigConf::HLTUtils::hash2string( alg->getDetail<TrigConf::HLTHash>("store"), "STORE").find("StoreGateSvc") != std::string::npos);
  ATH_CHECK( fill("InEventView_perCall", (globalScope ? 0.0 : 1.0), weight) );

  ATH_CHECK( fill("RoIID_perCall", alg->getDetail<int32_t>("roi"), weight) );

  return StatusCode::SUCCESS;
}
