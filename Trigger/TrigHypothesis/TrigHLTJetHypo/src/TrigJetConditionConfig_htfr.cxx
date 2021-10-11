/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*
  Instantiator for Htfr Conditions
 */
#include "TrigJetConditionConfig_htfr.h"
#include "GaudiKernel/StatusCode.h"
#include "./HTConditionFastReduction.h"
#include "./ArgStrToDouble.h"

TrigJetConditionConfig_htfr::TrigJetConditionConfig_htfr(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent) :
  base_class(type, name, parent){

}


StatusCode TrigJetConditionConfig_htfr::initialize() {
  CHECK(checkVals());

  return StatusCode::SUCCESS;
}


Condition TrigJetConditionConfig_htfr::getCondition() const {
  auto a2d = ArgStrToDouble();
  return std::make_unique<HTConditionFastReduction>(a2d(m_min), a2d(m_max));
}


StatusCode TrigJetConditionConfig_htfr::checkVals() const {
    auto a2d = ArgStrToDouble();
  if (a2d(m_min) > a2d(m_max)){
    ATH_MSG_ERROR("htMin > htMax");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
