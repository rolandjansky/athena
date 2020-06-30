/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*
  Instantiator for Ht Conditions
 */
#include "TrigJetConditionConfig_ht.h"
#include "GaudiKernel/StatusCode.h"
#include "./HTConditionMT.h"
#include "./ArgStrToDouble.h"

TrigJetConditionConfig_ht::TrigJetConditionConfig_ht(const std::string& type,
								     const std::string& name,
								     const IInterface* parent) :
  base_class(type, name, parent){
  
}


StatusCode TrigJetConditionConfig_ht::initialize() {
  CHECK(checkVals());
  
  return StatusCode::SUCCESS;
}


ConditionMT TrigJetConditionConfig_ht::getCondition() const {
  auto a2d = ArgStrToDouble();
  return std::make_unique<HTConditionMT>(a2d(m_htmin),
					 a2d(m_etmin),
					 a2d(m_etamin),
					 a2d(m_etamax));
}

 
StatusCode TrigJetConditionConfig_ht::checkVals() const {
  auto a2d = ArgStrToDouble();
  if (a2d(m_htmin) < 0.){
    ATH_MSG_ERROR("ht min < 0.");
    return StatusCode::FAILURE;
  }

  if (a2d(m_etmin) < 0.){
    ATH_MSG_ERROR("etmin < 0");
    return StatusCode::FAILURE;
  }

  if (a2d(m_etamin) < 0.){
    ATH_MSG_ERROR("abseta min < 0");
    return StatusCode::FAILURE;
  }

  
  if (a2d(m_etamin) > a2d(m_etamax)){
    ATH_MSG_ERROR("abseta min > abseta max");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

bool TrigJetConditionConfig_ht::addToCapacity(std::size_t) {
  return false;
}

std::size_t TrigJetConditionConfig_ht::capacity() const {
  return getCondition()->capacity();
}
