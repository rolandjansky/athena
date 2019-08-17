/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*
  Instantiator for Dijet Conditions
 */
#include "TrigJetConditionConfig_dijet.h"
#include "GaudiKernel/StatusCode.h"
#include "./conditionsFactoryMT.h"
#include "svec2dvec.h"
#include <algorithm>

TrigJetConditionConfig_dijet::TrigJetConditionConfig_dijet(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent) :
  base_class(type, name, parent){

}


TrigJetConditionConfig_dijet::~TrigJetConditionConfig_dijet(){
}

StatusCode TrigJetConditionConfig_dijet::initialize() {
  CHECK(checkVals());

  return StatusCode::SUCCESS;
}




std::optional<ConditionsMT>
TrigJetConditionConfig_dijet::getConditions() const {
  return
    std::make_optional<ConditionsMT>(conditionsFactoryDijetMT(svec2dvec(m_massMins),
							      svec2dvec(m_massMaxs),
							      svec2dvec(m_dEtaMins),
							      svec2dvec(m_dEtaMaxs),
							      svec2dvec(m_dPhiMins),
							      svec2dvec(m_dPhiMaxs)));
}

 
StatusCode TrigJetConditionConfig_dijet::checkVals() const {
  // check cionsistent sizes

  auto sz = m_massMins.size();
  if (sz != m_massMaxs.size() or
      sz != m_dEtaMins.size() or
      sz != m_dEtaMaxs.size() or
      sz != m_dPhiMins.size() or
      sz != m_dPhiMaxs.size()){
    ATH_MSG_ERROR(name()
                  << ": mismatch between number of thresholds "
                  << "and min, max fro mass, dEta, dPhi "
                  << m_massMins.size() << " "
                  << m_massMaxs.size() << " "
                  << m_dEtaMins.size() << " "
                  << m_dEtaMaxs.size() << " "
                  << m_dPhiMins.size() << " "
                  << m_dPhiMaxs.size() << " ");
    
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

