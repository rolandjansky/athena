/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*
  Instantiator for JVT Condition
 */
#include "TrigJetConditionConfig_jvt.h"
#include "GaudiKernel/StatusCode.h"
#include "./JVTCondition.h"
#include "./ArgStrToDouble.h"

TrigJetConditionConfig_jvt::TrigJetConditionConfig_jvt(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent){
}


StatusCode TrigJetConditionConfig_jvt::initialize() {
  CHECK(checkVals());
  
  return StatusCode::SUCCESS;
}


Condition TrigJetConditionConfig_jvt::getCondition() const {
  auto a2d = ArgStrToDouble();
  return std::make_unique<JVTCondition>(a2d(m_min));
}

 
StatusCode TrigJetConditionConfig_jvt::checkVals() const {
  return StatusCode::SUCCESS;
}
