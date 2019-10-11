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
TrigJetHypoToolConfig_partgen::getJetGrouper() const {

  std::vector<std::size_t> mults;
  for(const auto& c : m_children){
    mults.push_back(c->requiresNJets());
  }
    
  return std::make_unique<PartitionsGrouper>(mults);
}

StatusCode TrigJetHypoToolConfig_partgen::checkVals() const {    
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
