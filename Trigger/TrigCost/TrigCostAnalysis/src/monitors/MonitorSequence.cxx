/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MonitorSequence.h"
#include "../counters/CounterSequence.h"

MonitorSequence::MonitorSequence(const std::string& name, const MonitoredRange* parent)
  : MonitorBase(name, parent) {
}

StatusCode MonitorSequence::newEvent(const CostData& data, const float weight) {
  for (const auto& sequencePair : data.sequencersMap()) {
    if (sequencePair.first.empty()) continue;
    for (const auto& viewAlgsPair : sequencePair.second){
      ATH_CHECK( getCounter(sequencePair.first)->newEvent(data, viewAlgsPair.first, weight) );
    }
  }

  return StatusCode::SUCCESS;
}


std::unique_ptr<CounterBase> MonitorSequence::newCounter(const std::string& name) {
  return std::make_unique<CounterSequence>(name, this);
} 
