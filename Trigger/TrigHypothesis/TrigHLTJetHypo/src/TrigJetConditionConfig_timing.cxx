/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*
  Instantiator for TIMING Condition
 */
#include "TrigJetConditionConfig_timing.h"
#include "GaudiKernel/StatusCode.h"
#include "./TimingCondition.h"
#include "./ArgStrToDouble.h"

TrigJetConditionConfig_timing::TrigJetConditionConfig_timing(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent){
}


StatusCode TrigJetConditionConfig_timing::initialize() {
  CHECK(checkVals());
  
  return StatusCode::SUCCESS;
}


Condition TrigJetConditionConfig_timing::getCondition() const {
  auto a2d = ArgStrToDouble();
  return std::make_unique<TimingCondition>(a2d(m_min), a2d(m_max));
}

 
StatusCode TrigJetConditionConfig_timing::checkVals() const {
  return StatusCode::SUCCESS;
}
