/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigJetConditionConfig_abs_eta.h"

#include "GaudiKernel/StatusCode.h"
#include "./EtaConditionAbs.h"

TrigJetConditionConfig_abs_eta::TrigJetConditionConfig_abs_eta(const std::string& type,
						       const std::string& name,
						       const IInterface* parent) :
  base_class(type, name, parent){

}


StatusCode TrigJetConditionConfig_abs_eta::initialize() {
  CHECK(checkVals());
  return StatusCode::SUCCESS;
}


Condition TrigJetConditionConfig_abs_eta::getCondition() const {
  auto a2d = ArgStrToDouble();
  return std::make_unique<EtaConditionAbs>(a2d(m_min), a2d(m_max));
}


StatusCode TrigJetConditionConfig_abs_eta::checkVals() const {

  auto a2d = ArgStrToDouble();

  auto min_val = a2d(m_min);
  auto max_val = a2d(m_max);
  
  if (min_val < 0.){
     ATH_MSG_ERROR(" min eta < 0 " << m_min);
    return StatusCode::FAILURE;
  }

  if (min_val > max_val){
    ATH_MSG_ERROR(" min eta >  max eta");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

