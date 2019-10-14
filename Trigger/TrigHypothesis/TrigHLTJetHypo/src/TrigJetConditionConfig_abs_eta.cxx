/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigJetConditionConfig_abs_eta.h"

#include "GaudiKernel/StatusCode.h"
#include "./EtaConditionAbsMT.h"

TrigJetConditionConfig_abs_eta::TrigJetConditionConfig_abs_eta(const std::string& type,
						       const std::string& name,
						       const IInterface* parent) :
  base_class(type, name, parent){

}


StatusCode TrigJetConditionConfig_abs_eta::initialize() {
  CHECK(checkVals());
  return StatusCode::SUCCESS;
}


ConditionMT TrigJetConditionConfig_abs_eta::getCondition() const {
  auto a2d = ArgStrToDouble();
  return std::make_unique<EtaConditionAbsMT>(a2d(m_min), a2d(m_max));
}


StatusCode TrigJetConditionConfig_abs_eta::checkVals() const {

  auto a2d = ArgStrToDouble();
  if (a2d(m_min) > a2d(m_max)){
    ATH_MSG_ERROR(" min eta >  max eta");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


