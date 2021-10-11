/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigJetConditionConfig_smc.h"

#include "GaudiKernel/StatusCode.h"
#include "./SMCCondition.h"

TrigJetConditionConfig_smc::TrigJetConditionConfig_smc(const std::string& type,
						       const std::string& name,
						       const IInterface* parent) :
  base_class(type, name, parent){

}


StatusCode TrigJetConditionConfig_smc::initialize() {
  CHECK(checkVals());
  return StatusCode::SUCCESS;
}


Condition TrigJetConditionConfig_smc::getCondition() const {
  auto a2d = ArgStrToDouble();
  return std::make_unique<SMCCondition>(a2d(m_min), a2d(m_max));
}


StatusCode TrigJetConditionConfig_smc::checkVals() const {

  auto a2d = ArgStrToDouble();
  if (a2d(m_min) > a2d(m_max)){
    ATH_MSG_ERROR(" min smc >  max smc");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
