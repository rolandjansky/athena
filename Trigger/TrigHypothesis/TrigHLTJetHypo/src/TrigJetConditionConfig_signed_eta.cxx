/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigJetConditionConfig_signed_eta.h"

#include "GaudiKernel/StatusCode.h"
#include "./EtaConditionSigned.h"

TrigJetConditionConfig_signed_eta::TrigJetConditionConfig_signed_eta(const std::string& type,
						       const std::string& name,
						       const IInterface* parent) :
  base_class(type, name, parent){

}


StatusCode TrigJetConditionConfig_signed_eta::initialize() {
  CHECK(checkVals());
  return StatusCode::SUCCESS;
}


Condition TrigJetConditionConfig_signed_eta::getCondition() const {
  auto a2d = ArgStrToDouble();
  return std::make_unique<EtaConditionSigned>(a2d(m_min), a2d(m_max));
}


StatusCode TrigJetConditionConfig_signed_eta::checkVals() const {

  auto a2d = ArgStrToDouble();

  auto min_val = a2d(m_min);
  auto max_val = a2d(m_max);
  
  if (min_val > max_val){
    ATH_MSG_ERROR(" min eta >  max eta: " << min_val << " " << max_val);
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
