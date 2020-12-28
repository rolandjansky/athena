/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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


ConditionMT TrigJetConditionConfig_htfr::getCondition() const {
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

bool TrigJetConditionConfig_htfr::addToCapacity(std::size_t) {
  return false;
}

std::size_t TrigJetConditionConfig_htfr::capacity() const {
  return getCondition()->capacity();
}
