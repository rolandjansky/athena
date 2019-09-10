/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigJetHypoToolMT_combgen.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
//
// ********************************************************************

#include "TrigJetHypoToolConfig_combgen.h"
#include "./conditionsFactoryMT.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CombinationsGrouper.h"
#include "./CombinationsGroupsToHelpersMatcherMT.h"

TrigJetHypoToolConfig_combgen::TrigJetHypoToolConfig_combgen(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent) :
  base_class(type, name, parent){

}


TrigJetHypoToolConfig_combgen::~TrigJetHypoToolConfig_combgen(){
}

StatusCode TrigJetHypoToolConfig_combgen::initialize() {
  CHECK(checkVals());
  
  auto mult = (*m_children.begin()) -> requiresNJets();
  for (const auto& c : m_children){
    if(c->requiresNJets() != mult){
      ATH_MSG_ERROR("Children require differing number of jets:");
      return StatusCode::FAILURE;
    }
  }

  if(!mult){
    ATH_MSG_ERROR("Children require 0 jets:");
    return StatusCode::FAILURE;
  }
    
  m_size = mult;
  
  return StatusCode::SUCCESS;
}




std::optional<ConditionsMT>
TrigJetHypoToolConfig_combgen::getConditions() const {

  auto conditions = conditionsFactoryEtaEtMT(m_etaMins,
                                             m_etaMaxs,
                                             m_EtThresholds,
                                             m_asymmetricEtas);
  if(conditions.empty()){
     return std::make_optional<ConditionsMT>(std::move(conditions));
  }

  // allow only very simple conditions
  if(std::any_of(conditions.begin(),
		 conditions.end(),
		 [](const auto& c) {
		   return c->capacity() != 1;})){
    ATH_MSG_ERROR("There is a condition with capacity != 1");
    return std::optional<ConditionsMT>();
  }
  
  return std::make_optional<ConditionsMT>(std::move(conditions));
}

 
std::unique_ptr<IJetGrouper>
TrigJetHypoToolConfig_combgen::getJetGrouper() const {
  return std::make_unique<CombinationsGrouper>(m_size);
}

StatusCode TrigJetHypoToolConfig_combgen::checkVals() const {
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
TrigJetHypoToolConfig_combgen::getCleaners() const {
  std::vector<std::shared_ptr<ICleaner>> v;
  return v;
}


std::unique_ptr<IGroupsMatcherMT>
TrigJetHypoToolConfig_combgen::getMatcher () const {
  /* Provides a matcher that matcheZ<s single HypoJetVectors to
     multiple ITrigJetHypoToolHelperMT*/

  return std::make_unique<CombinationsGroupsToHelpersMatcherMT>(m_children);
}

std::size_t
TrigJetHypoToolConfig_combgen::requiresNJets() const {
  std::size_t result{0};
  for(const auto& c : m_children){result += c->requiresNJets();}
  return result;
}
