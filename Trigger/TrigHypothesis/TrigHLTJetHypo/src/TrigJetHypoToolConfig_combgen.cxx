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
      ATH_MSG_ERROR(name() << " Children require differing number of jets:");
      ATH_MSG_ERROR(" First child name: " << (*m_children.begin()) -> name());
      ATH_MSG_ERROR(" Differing mult child name: " <<(c -> name()));
	
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
  
  ConditionsMT compoundConditions;
	  

  // collect the Conditions objects from the various sources
  // return an invalid optional if any src signals a problem
  
  // m_condition makers is a list of compound condition makers
  for(const auto& cm : m_conditionMakers) {
    compoundConditions.push_back(cm->getCondition());
  }
  
  return std::make_optional<ConditionsMT>(std::move(compoundConditions));

}

 
std::unique_ptr<IJetGrouper>
TrigJetHypoToolConfig_combgen::getJetGrouper() const {
  return std::make_unique<CombinationsGrouper>(m_size);
}

StatusCode TrigJetHypoToolConfig_combgen::checkVals() const {
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
