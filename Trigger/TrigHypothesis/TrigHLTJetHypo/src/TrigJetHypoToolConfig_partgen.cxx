/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigJetHypoToolMT_partgen.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
//
// ********************************************************************

#include "TrigJetHypoToolConfig_partgen.h"
#include "./conditionsFactoryMT.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/PartitionsGrouper.h"
#include "./PartitionsGroupsToHelpersMatcherMT.h"

TrigJetHypoToolConfig_partgen::TrigJetHypoToolConfig_partgen(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent) :
  base_class(type, name, parent){

}


TrigJetHypoToolConfig_partgen::~TrigJetHypoToolConfig_partgen(){
}

StatusCode TrigJetHypoToolConfig_partgen::initialize() {
  CHECK(checkVals());
  return StatusCode::SUCCESS;
}




std::optional<ConditionsMT>
TrigJetHypoToolConfig_partgen::getConditions() const {

  auto conditions = conditionsFactoryEtaEtMT(m_etaMins,
                                             m_etaMaxs,
                                             m_EtThresholds,
                                             m_asymmetricEtas);
  
  return std::make_optional<ConditionsMT>(std::move(conditions));
}

 
std::unique_ptr<IJetGrouper>
TrigJetHypoToolConfig_partgen::getJetGrouper() const {

  std::vector<std::size_t> mults;
  for(const auto& c : m_children){
    mults.push_back(c->requiresNJets());
  }
    
  return std::make_unique<PartitionsGrouper>(mults);
}

StatusCode TrigJetHypoToolConfig_partgen::checkVals() const {
  if (m_EtThresholds.size() != m_etaMins.size() or
      m_EtThresholds.size() != m_etaMaxs.size() or
      m_asymmetricEtas.size() != m_etaMaxs.size()){
    
    ATH_MSG_ERROR(name()<< ": mismatch between number of thresholds "
                  << "and eta min, max boundaries or asymmetric eta flags: "
                  << m_EtThresholds.size() << " "
                  << m_etaMins.size() << " "
                  << m_etaMaxs.size() << " "
                  << m_asymmetricEtas.size() << " "
                  );
    
    return StatusCode::FAILURE;
  }

  if(m_children.empty()){
    ATH_MSG_ERROR(name()<< ": No children ");
    return StatusCode::FAILURE;
  }
    
  return StatusCode::SUCCESS;
}

std::vector<std::shared_ptr<ICleaner>> 
TrigJetHypoToolConfig_partgen::getCleaners() const {
  std::vector<std::shared_ptr<ICleaner>> v;
  return v;
}


std::unique_ptr<IGroupsMatcherMT>
TrigJetHypoToolConfig_partgen::getMatcher () const {
  /* Provides a matcher that matches single HypoJetVectors to
     multiple ITrigJetHypoToolHelperMT*/

  return std::make_unique<PartitionsGroupsToHelpersMatcherMT>(m_children);
}
