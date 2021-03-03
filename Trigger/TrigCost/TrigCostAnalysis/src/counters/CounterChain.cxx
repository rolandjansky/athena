/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODTrigger/TrigCompositeContainer.h"
#include "TrigDataAccessMonitoring/ROBDataMonitor.h"

#include "CounterChain.h"

CounterChain::CounterChain(const std::string& name, const MonitorBase* parent) 
  : CounterBase(name, parent)
{

}


StatusCode CounterChain::newEvent(const CostData& /*data*/, size_t /*index*/, const float /*weight*/) {

  return StatusCode::SUCCESS;
}
