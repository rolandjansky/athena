/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODTrigger/TrigCompositeContainer.h"
#include "TrigDataAccessMonitoring/ROBDataMonitor.h"

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
  regHistogram("Request_perEvent", "Number of requests/Event;Number of requests;Events", VariableType::kPerEvent, LogType::kLinear, -0.5, 10.5, 11);
  regHistogram("NetworkRequest_perEvent", "Number of network requests/Event;Number of requests;Events", VariableType::kPerEvent, LogType::kLinear, -0.5, 10.5, 11);
  regHistogram("CachedROBSize_perEvent", "Total ROB Size/Event;ROB size;Events", VariableType::kPerEvent, LogType::kLinear, 0, 1024, 50);
  regHistogram("NetworkROBSize_perEvent", "Total ROB Size/Event;ROB size;Events", VariableType::kPerEvent, LogType::kLinear, 0, 1024, 50);
  regHistogram("RequestTime_perEvent", "ROB Elapsed Time/Event;Elapsed Time [ms];Events", VariableType::kPerEvent);
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

  // Monitor data requests per algorithm
  if (data.algToRequestMap().count(index)) {
    for (size_t requestIdx : data.algToRequestMap().at(index)) {
      const xAOD::TrigComposite* request = data.rosCollection().at(requestIdx);
      const std::vector<unsigned> robs_history = request->getDetail<std::vector<unsigned>>("robs_history");
      const std::vector<uint32_t> robs_size = request->getDetail<std::vector<uint32_t>>("robs_size");

      bool networkRequestIncremented = false;
      for (size_t i = 0; i < robs_size.size(); ++i) {
        // ROB request was fetched over the network
        if (robs_history[i] == robmonitor::RETRIEVED) {
          ATH_CHECK( fill("NetworkROBSize_perEvent", robs_size[i], weight) );
          networkRequestIncremented = true;
        }
        // ROB request was cached
        else if (robs_history[i] == robmonitor::HLT_CACHED || robs_history[i] == robmonitor::DCM_CACHED) {
          ATH_CHECK( fill("CachedROBSize_perEvent", robs_size[i], weight) );
        }
      }

      ATH_CHECK( increment("Request_perEvent", weight) );

      if (networkRequestIncremented) {
        ATH_CHECK( increment("NetworkRequest_perEvent", weight) );
      }

      const float rosTime = timeToMilliSec(request->getDetail<uint64_t>("start"), request->getDetail<uint64_t>("stop"));
      ATH_CHECK( fill("Time_perEvent", rosTime, weight) );
    }
  }

  return StatusCode::SUCCESS;
}
