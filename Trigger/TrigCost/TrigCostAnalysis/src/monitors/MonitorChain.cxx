/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MonitorChain.h"
#include "../counters/CounterChain.h"

MonitorChain::MonitorChain(const std::string& name, const MonitoredRange* parent)
  : MonitorBase(name, parent) {
}

StatusCode MonitorChain::newEvent(const CostData& data, const float weight) {

  const std::set<TrigCompositeUtils::DecisionID> seededChains = data.seededChains();
  int i = 0;
  for (const TrigCompositeUtils::DecisionID& chain : seededChains){
    const std::string chainName = TrigConf::HLTUtils::hash2string(chain);
    ATH_CHECK( getCounter(chainName)->newEvent(data, i, weight) );
    ++i;
  }

  return StatusCode::SUCCESS;
}


std::unique_ptr<CounterBase> MonitorChain::newCounter(const std::string& name) {
  return std::make_unique<CounterChain>(name, this);
} 
