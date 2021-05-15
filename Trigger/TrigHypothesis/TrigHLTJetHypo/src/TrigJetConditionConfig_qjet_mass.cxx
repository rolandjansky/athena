/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*
  Instantiator for Qjet_Mass Conditions
 */
#include "TrigJetConditionConfig_qjet_mass.h"
#include "GaudiKernel/StatusCode.h"
#include "./QjetMassCondition.h"
#include "./ArgStrToDouble.h"

TrigJetConditionConfig_qjet_mass::TrigJetConditionConfig_qjet_mass(const std::string& type,
								     const std::string& name,
								     const IInterface* parent) :
  base_class(type, name, parent){
  
}


StatusCode TrigJetConditionConfig_qjet_mass::initialize() {
  CHECK(checkVals());
  
  return StatusCode::SUCCESS;
}


Condition TrigJetConditionConfig_qjet_mass::getCondition() const {
  auto a2d = ArgStrToDouble();
  return std::make_unique<QjetMassCondition>(a2d(m_min), a2d(m_max));
}

 
StatusCode TrigJetConditionConfig_qjet_mass::checkVals() const {
  auto a2d = ArgStrToDouble();
  if (a2d(m_min) > a2d(m_max)){
    ATH_MSG_ERROR("mass min > mass max");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
