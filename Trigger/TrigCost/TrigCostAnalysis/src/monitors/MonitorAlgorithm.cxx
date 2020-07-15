/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MonitorAlgorithm.h"
#include "../counters/CounterAlgorithm.h"

MonitorAlgorithm::MonitorAlgorithm(const std::string& name, const MonitoredRange* parent)
  : MonitorBase(name, parent) {
}

StatusCode MonitorAlgorithm::newEvent(const CostData& data, const float weight) {

  for (const xAOD::TrigComposite* tc : data.costCollection()) {
    const uint32_t slot = tc->getDetail<uint32_t>("slot");
    if (slot != data.onlineSlot()) {
      continue; // When monitoring the master slot, this Monitor ignores algs running in different slots 
    }
    const uint32_t nameHash = tc->getDetail<TrigConf::HLTHash>("alg");
    const std::string name = TrigConf::HLTUtils::hash2string(nameHash, "ALG");
    ATH_CHECK( getCounter(name)->newEvent(data, tc->index(), weight) );
  }

  return StatusCode::SUCCESS;
}


std::unique_ptr<CounterBase> MonitorAlgorithm::newCounter(const std::string& name) {
  return std::make_unique<CounterAlgorithm>(name, this);
} 
