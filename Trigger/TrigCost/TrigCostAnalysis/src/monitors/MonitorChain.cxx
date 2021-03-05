/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MonitorChain.h"
#include "../counters/CounterChain.h"

MonitorChain::MonitorChain(const std::string& name, const MonitoredRange* parent)
  : MonitorBase(name, parent) {
}

StatusCode MonitorChain::newEvent(const CostData& data, const float weight) {

  const std::vector<TrigCompositeUtils::AlgToChainTool::ChainInfo> seededChains = data.seededChains();
  for (size_t i = 0; i < seededChains.size(); ++i){
    ATH_CHECK( getCounter(seededChains[i].name)->newEvent(data, i, weight) );
  }

  return StatusCode::SUCCESS;
}


std::unique_ptr<CounterBase> MonitorChain::newCounter(const std::string& name) {
  return std::make_unique<CounterChain>(name, this);
} 
