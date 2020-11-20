/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODTrigger/TrigCompositeContainer.h"

#include "CounterROS.h"

CounterROS::CounterROS(const std::string& name, const MonitorBase* parent) 
  : CounterBase(name, parent)
{}

StatusCode CounterROS::newEvent(const CostData& /*data*/, size_t /*incrementWalltime*/, const float /*weight*/) {
  return StatusCode::SUCCESS;
}

int CounterROS::getROBHistoryBin(const unsigned history){
  int history_bin;
  switch (history) {
    case 1: // SCHEDULED
      history_bin = 1;
      break;
    case 2: // RETRIEVED
      history_bin = 1;
      break;
    case 4: // HLT_CACHED
      history_bin = 3;
      break;
    case 8: // DCM_CACHED
      history_bin = 4;
      break;
    case 16: // IGNORED
      history_bin = 5;
      break;
    case 32: // DISABLED
      history_bin = 6;
      break;
    default: // UNCLASSIFIED 
      history_bin = 0;
      break;
  }
  
  return history_bin;
}
