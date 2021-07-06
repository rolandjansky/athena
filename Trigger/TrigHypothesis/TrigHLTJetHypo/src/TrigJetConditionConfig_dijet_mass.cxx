/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*
  Instantiator for Dijet_Mass Conditions
 */
#include "TrigJetConditionConfig_dijet_mass.h"
#include "GaudiKernel/StatusCode.h"
#include "./DijetMassCondition.h"
#include "./ArgStrToDouble.h"

TrigJetConditionConfig_dijet_mass::TrigJetConditionConfig_dijet_mass(const std::string& type,
								     const std::string& name,
								     const IInterface* parent) :
  base_class(type, name, parent){
  
}


StatusCode TrigJetConditionConfig_dijet_mass::initialize() {
  CHECK(checkVals());
  
  return StatusCode::SUCCESS;
}


Condition TrigJetConditionConfig_dijet_mass::getCondition() const {
  auto a2d = ArgStrToDouble();
  return std::make_unique<DijetMassCondition>(a2d(m_min), a2d(m_max));
}

 
StatusCode TrigJetConditionConfig_dijet_mass::checkVals() const {
  auto a2d = ArgStrToDouble();
  if (a2d(m_min) > a2d(m_max)){
    ATH_MSG_ERROR("mass min > mass max");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
