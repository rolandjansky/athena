/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigJetConditionConfig_phi.h"

#include "GaudiKernel/StatusCode.h"
#include "./PhiConditionMT.h"
#include <cmath>

TrigJetConditionConfig_phi::TrigJetConditionConfig_phi(const std::string& type,
						       const std::string& name,
						       const IInterface* parent) :
  base_class(type, name, parent){

}

StatusCode TrigJetConditionConfig_phi::initialize() {

  auto convert = [](const std::string& s) {
    
    if (s == "PI"){
      return M_PI;
    } else if (s == "-PI") {
      return -M_PI;      
    }
    return std::stod(s);
  };

  try {
    m_min = convert(m_strmin);
  } catch (...) {
    ATH_MSG_ERROR ("Cannot convert " + m_strmin + " to double");
    return StatusCode::FAILURE;
  }
  
  try {
    m_max = convert(m_strmax);
  } catch (...) {
    ATH_MSG_ERROR ("Cannot convert " +  m_strmax + " to double");
    return StatusCode::FAILURE;
  }
    
  CHECK(checkVals());
  return StatusCode::SUCCESS;
}


ConditionMT TrigJetConditionConfig_phi::getCondition() const {
  return std::make_unique<PhiConditionMT>(m_min, m_max);
}


StatusCode TrigJetConditionConfig_phi::checkVals() const {

  if (m_min > m_max){
    ATH_MSG_ERROR(" min phi >  max phi");
    return StatusCode::FAILURE;
  }

  if (m_min < -M_PI) {
    ATH_MSG_ERROR(" min phi " << m_min << " out of range");
    return StatusCode::FAILURE;
  }

  if (m_max > M_PI) {
    ATH_MSG_ERROR(" max phi " << m_max << " out of range");
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}


bool TrigJetConditionConfig_phi::addToCapacity(std::size_t) {
  return false;
}

std::size_t TrigJetConditionConfig_phi::capacity() const {
  return getCondition()->capacity();
}
