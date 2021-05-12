/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*
  Instantiator for Dijet_Dphi Conditions
 */
#include "TrigJetConditionConfig_dijet_dphi.h"
#include "GaudiKernel/StatusCode.h"
#include "./DijetDPhiCondition.h"
#include "./ArgStrToDouble.h"


TrigJetConditionConfig_dijet_dphi::TrigJetConditionConfig_dijet_dphi(const std::string& type,
								     const std::string& name,
								     const IInterface* parent) :
  base_class(type, name, parent){
  
}


StatusCode TrigJetConditionConfig_dijet_dphi::initialize() {
  CHECK(checkVals());
  
  return StatusCode::SUCCESS;
}


Condition TrigJetConditionConfig_dijet_dphi::getCondition() const {
  auto a2d = ArgStrToDouble();
  return std::make_unique<DijetDPhiCondition>(a2d(m_min), a2d(m_max));
}


StatusCode TrigJetConditionConfig_dijet_dphi::checkVals() const {
  
  auto a2d = ArgStrToDouble();
  
  if(a2d(m_min) >a2d(m_max)){
    ATH_MSG_ERROR("Error: dPhiMin > dphiMax");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
