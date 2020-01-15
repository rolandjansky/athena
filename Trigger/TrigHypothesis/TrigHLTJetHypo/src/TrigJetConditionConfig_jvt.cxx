/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
  Instantiator for JVT Condition
 */
#include "TrigJetConditionConfig_jvt.h"
#include "GaudiKernel/StatusCode.h"
#include "./JVTConditionMT.h"
#include "./ArgStrToDouble.h"

TrigJetConditionConfig_jvt::TrigJetConditionConfig_jvt(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent){
}


StatusCode TrigJetConditionConfig_jvt::initialize() {
  CHECK(checkVals());
  
  return StatusCode::SUCCESS;
}


ConditionMT TrigJetConditionConfig_jvt::getCondition() const {
  auto a2d = ArgStrToDouble();
  return std::make_unique<JVTConditionMT>(a2d(m_min));
}

 
StatusCode TrigJetConditionConfig_jvt::checkVals() const {
  return StatusCode::SUCCESS;
}

bool TrigJetConditionConfig_jvt::addToCapacity(std::size_t) {
  return false;
}

std::size_t TrigJetConditionConfig_jvt::capacity() const {
  return getCondition()->capacity();
}
