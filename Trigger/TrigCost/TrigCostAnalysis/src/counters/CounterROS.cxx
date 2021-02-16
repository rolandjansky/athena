/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODTrigger/TrigCompositeContainer.h"
#include "TrigDataAccessMonitoring/ROBDataMonitor.h"

#include "CounterROS.h"

#include <algorithm>
#include <string>
#include <vector>

CounterROS::CounterROS(const std::string& name, const MonitorBase* parent) 
  : CounterBase(name, parent) {

  regHistogram("Request_perEvent", "Number of requests/Event;Number of requests;Events", VariableType::kPerEvent, LogType::kLinear, -0.5, 20.5, 21);
  regHistogram("NetworkRequest_perEvent", "Number of network requests/Event;Number of requests;Events", VariableType::kPerEvent, LogType::kLinear, -0.5, 10.5, 11);
  regHistogram("CachedROBSize_perEvent", "Total ROB Size/Event;ROB size;Events", VariableType::kPerEvent, LogType::kLinear, 0, 1024, 50);
  regHistogram("NetworkROBSize_perEvent", "Total ROB Size/Event;ROB size;Events", VariableType::kPerEvent, LogType::kLinear, 0, 1024, 50);
  regHistogram("Time_perEvent", "ROB Elapsed Time/Event;Elapsed Time [ms];Events", VariableType::kPerEvent);
  regHistogram("ROBStatus_perCall", "ROB status/Call;Status;Events", VariableType::kPerCall, LogType::kLinear, 0, robmonitor::NUM_ROBHIST_CODES+1, robmonitor::NUM_ROBHIST_CODES+1);
}

StatusCode CounterROS::newEvent(const CostData& data, size_t index, const float weight) {

  // Monitor only ROB data for corresponding ROS
  const xAOD::TrigComposite* tc = data.rosCollection()[index];
  const std::vector<uint32_t> robIdsPerRequest = tc->getDetail<std::vector<uint32_t>>("robs_id");
  const std::vector<uint32_t> robs_size = tc->getDetail<std::vector<uint32_t>>("robs_size");
  const std::vector<unsigned> robs_history = tc->getDetail<std::vector<unsigned>>("robs_history");
  const std::vector<unsigned short> robs_status = tc->getDetail<std::vector<unsigned short>>("robs_status");

  if (m_robIdsPerROS.size() == 0) {
    m_robIdsPerROS = data.rosToRobMap().at(getName());
  }

  // Find all ROB requests that are both in request and correspond to this ROS
  bool networkRequestIncremented = false;
  for (size_t i = 0; i < robIdsPerRequest.size(); ++i) {
    if (std::find(m_robIdsPerROS.begin(), m_robIdsPerROS.end(), robIdsPerRequest[i]) != m_robIdsPerROS.end()) {
      ATH_CHECK( fill("ROBStatus_perCall", getROBHistoryBin(robs_history[i]), weight) );
      if (robs_status[i]) {
        // The last bin of ROBStatus_perCall histogram store isStatusOk bool value
        ATH_CHECK( fill("ROBStatus_perCall", robmonitor::NUM_ROBHIST_CODES, weight) );
      }

      // ROB request was fetched over the network
      if (robs_history[i] == robmonitor::RETRIEVED) {
        // size is stored in words, should be in kilobytes
        ATH_CHECK( fill("NetworkROBSize_perEvent", robs_size[i] / 500., weight) );
        networkRequestIncremented = true;
      }
      else if (robs_history[i] == robmonitor::HLT_CACHED || robs_history[i] == robmonitor::DCM_CACHED) {
        ATH_CHECK( fill("CachedROBSize_perEvent", robs_size[i] / 500., weight) );
      }
    }
  }

  ATH_CHECK( increment("Request_perEvent", weight) );

  if (networkRequestIncremented) {
    ATH_CHECK( increment("NetworkRequest_perEvent", weight) );
  }

  const float rosTime = timeToMilliSec(tc->getDetail<uint64_t>("start"), tc->getDetail<uint64_t>("stop"));
  ATH_CHECK( fill("Time_perEvent", rosTime, weight) );

  return StatusCode::SUCCESS;
}

int CounterROS::getROBHistoryBin(const unsigned history){
  int history_bin;
  switch (history) {
    case robmonitor::RETRIEVED:
      history_bin = 1;
      break;
    case robmonitor::HLT_CACHED:
      history_bin = 2;
      break;
    case robmonitor::DCM_CACHED:
      history_bin = 3;
      break;
    case robmonitor::IGNORED:
      history_bin = 4;
      break;
    case robmonitor::UNDEFINED:
      history_bin = 5;
      break;
    default: // UNCLASSIFIED 
      history_bin = 0;
      break;
  }
  
  return history_bin;
}
