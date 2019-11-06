/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
  Instantiator for Dijet_Dphi Conditions
 */
#include "TrigJetConditionConfig_dijet_dphi.h"
#include "GaudiKernel/StatusCode.h"
#include "./DijetDPhiConditionMT.h"
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


ConditionMT TrigJetConditionConfig_dijet_dphi::getCondition() const {
  auto a2d = ArgStrToDouble();
  return std::make_unique<DijetDPhiConditionMT>(a2d(m_min), a2d(m_max));
}


StatusCode TrigJetConditionConfig_dijet_dphi::checkVals() const {
  
  auto a2d = ArgStrToDouble();
  
  if(a2d(m_min) >a2d(m_max)){
    ATH_MSG_ERROR("Error: dPhiMin > dphiMax");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

bool TrigJetConditionConfig_dijet_dphi::addToCapacity(std::size_t) {
  return false;
}

std::size_t TrigJetConditionConfig_dijet_dphi::capacity() const {
  return getCondition()->capacity();
}
