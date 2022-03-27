/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/*
  Instantiator for JVT Condition
 */
#include "TrigJetConditionConfig_bdips.h"
#include "GaudiKernel/StatusCode.h"
#include "./DipsCondition.h"
#include "./ArgStrToDouble.h"

TrigJetConditionConfig_bdips::TrigJetConditionConfig_bdips(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent){
}


StatusCode TrigJetConditionConfig_bdips::initialize() {
  CHECK(checkVals());
  
  return StatusCode::SUCCESS;
}


Condition TrigJetConditionConfig_bdips::getCondition() const {
  auto a2d = ArgStrToDouble();
  return std::make_unique<DipsCondition>(a2d(m_min));
}

 
StatusCode TrigJetConditionConfig_bdips::checkVals() const {
  return StatusCode::SUCCESS;
}
