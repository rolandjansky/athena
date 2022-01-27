/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODTrigger/TrigCompositeContainer.h"
#include "TrigDataAccessMonitoring/ROBDataMonitor.h"

#include "CounterSequence.h"


CounterSequence::CounterSequence(const std::string& name, const MonitorBase* parent) 
  : CounterBase(name, parent)
{
  regHistogram("Sequence_perEvent", "Sequnece calls/Event;Sequence call;Events", VariableType::kPerEvent, kLinear, -0.5, 49.5);
  regHistogram("AlgCalls_perEvent", "Algorithm Calls/Event;Calls;Events", VariableType::kPerEvent, kLinear, -0.5, 499.5, 100);
  regHistogram("Time_perCall", "CPU Time/Call;Time [ms];Calls", VariableType::kPerCall);
  regHistogram("Time_perEvent", "CPU Time/Event;Time [ms];Events", VariableType::kPerEvent);
  regHistogram("Request_perEvent", "Number of requests/Event;Number of requests;Events", VariableType::kPerEvent, LogType::kLinear, -0.5, 299.5, 300);
  regHistogram("NetworkRequest_perEvent", "Number of network requests/Event;Number of requests;Events", VariableType::kPerEvent, LogType::kLinear, -0.5, 149.5, 150);
  regHistogram("CachedROBSize_perEvent", "Cached ROB Size/Event;ROB size;Events", VariableType::kPerEvent, LogType::kLinear, 0, 1024, 50);
  regHistogram("NetworkROBSize_perEvent", "Network ROB Size/Event;ROB size;Events", VariableType::kPerEvent, LogType::kLinear, 0, 1024, 50);
  regHistogram("RequestTime_perEvent", "ROB Elapsed Time/Event;Elapsed Time [ms];Events", VariableType::kPerEvent);
}


StatusCode CounterSequence::newEvent(const CostData& data, size_t index, const float weight) {

  ATH_CHECK( increment("Sequence_perEvent", weight) );
  float viewTime = 0;
  // Monitor algorithms associated with sequence name
  for (const size_t algIndex :  data.sequencersMap().at(getName()).at(index)){
    
    const xAOD::TrigComposite* alg = data.costCollection().at(algIndex);
    const uint32_t slot = alg->getDetail<uint32_t>("slot");
    if (slot != data.onlineSlot()) {
      continue; // When monitoring the master slot, this Monitor ignores algs running in different slots 
    }

    ATH_CHECK( increment("AlgCalls_perEvent", weight) );

    const uint64_t start = alg->getDetail<uint64_t>("start"); // in mus
    const uint64_t stop  = alg->getDetail<uint64_t>("stop"); // in mus
    const float cpuTime = timeToMilliSec(start, stop);
    ATH_CHECK( fill("Time_perEvent", cpuTime, weight) );
    viewTime += cpuTime;

    // Monitor data requests
    if (!data.algToRequestMap().count(algIndex)) continue;

    for (size_t requestIdx : data.algToRequestMap().at(algIndex)) {
      const xAOD::TrigComposite* request = data.rosCollection().at(requestIdx);
      const std::vector<unsigned> robs_history = request->getDetail<std::vector<unsigned>>("robs_history");
      const std::vector<uint32_t> robs_size = request->getDetail<std::vector<uint32_t>>("robs_size");

      bool networkRequestIncremented = false;
      for (size_t i = 0; i < robs_size.size(); ++i) {
        // ROB request was fetched over the network
        if (robs_history[i] == robmonitor::RETRIEVED) {
          // size is stored in words, should be in kilobytes
          ATH_CHECK( fill("NetworkROBSize_perEvent", robs_size[i] / 500., weight) );
          networkRequestIncremented = true;
        }
        // ROB request was cached
        else if (robs_history[i] == robmonitor::HLT_CACHED || robs_history[i] == robmonitor::DCM_CACHED) {
          ATH_CHECK( fill("CachedROBSize_perEvent", robs_size[i] / 500., weight) );
        }
      }

      ATH_CHECK( increment("Request_perEvent", weight) );

      if (networkRequestIncremented) {
        ATH_CHECK( increment("NetworkRequest_perEvent", weight) );
      }

      const float rosTime = timeToMilliSec(request->getDetail<uint64_t>("start"), request->getDetail<uint64_t>("stop"));
      ATH_CHECK( fill("RequestTime_perEvent", rosTime, weight) );
    }
  }

  ATH_CHECK( fill("Time_perCall", viewTime, weight) );

  return StatusCode::SUCCESS;
}
