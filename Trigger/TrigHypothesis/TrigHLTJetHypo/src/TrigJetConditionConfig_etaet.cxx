/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigJetConditionConfig_etaet.h"

#include "GaudiKernel/StatusCode.h"
#include "./conditionsFactoryMT.h"
#include "./svec2dvec.h"

TrigJetConditionConfig_etaet::TrigJetConditionConfig_etaet(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent) :
  base_class(type, name, parent){

}


TrigJetConditionConfig_etaet::~TrigJetConditionConfig_etaet(){
}

StatusCode TrigJetConditionConfig_etaet::initialize() {
  CHECK(checkVals());
  return StatusCode::SUCCESS;
}




std::optional<ConditionsMT>
TrigJetConditionConfig_etaet::getConditions() const {

  return
    std::make_optional<ConditionsMT>(conditionsFactoryEtaEtMT(svec2dvec(m_etaMins),
							      svec2dvec(m_etaMaxs),
							      svec2dvec(m_EtThresholds),
							      m_asymmetricEtas));
}


StatusCode TrigJetConditionConfig_etaet::checkVals() const {
  if (m_EtThresholds.size() != m_etaMins.size() or
      m_EtThresholds.size() != m_etaMaxs.size() or
      m_asymmetricEtas.size() != m_etaMaxs.size()){
    
    ATH_MSG_ERROR(name()
                  << ": mismatch between number of thresholds "
                  << "and eta min, max boundaries or asymmetric eta flags: "
                  << m_EtThresholds.size() << " "
                  << m_etaMins.size() << " "
                  << m_etaMaxs.size() << " "
                  << m_asymmetricEtas.size() << " "
                  );
    
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


