/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigJetHypoToolMT_leaf.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
//
// ********************************************************************

#include "TrigJetHypoToolConfig_leaf.h"

#include "GaudiKernel/StatusCode.h"

#include "./conditionsFactoryMT.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/SingleJetGrouper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CombinationsGrouper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJetFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/groupsMatcherFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/TrigHLTJetHypoHelper2.h"
#include "./groupsMatcherFactoryMT.h"
#include "./svec2dvec.h"

#include "DecisionHandling/TrigCompositeUtils.h"

#include <iostream>

using TrigCompositeUtils::DecisionID;
using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;

TrigJetHypoToolConfig_leaf::TrigJetHypoToolConfig_leaf(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent) :
  base_class(type, name, parent){

}


TrigJetHypoToolConfig_leaf::~TrigJetHypoToolConfig_leaf(){
}

StatusCode TrigJetHypoToolConfig_leaf::initialize() {
  CHECK(checkVals());
  return StatusCode::SUCCESS;
}


std::optional<ConditionsMT>
TrigJetHypoToolConfig_leaf::getConditions() const {

  ConditionsMT compoundConditions;
	  

  // collect the Conditions objects from the various sources
  // return an invalid optional if any src signals a problem
  
  // m_condition makers is a list of compound condition makers
  for(const auto& cm : m_conditionMakers) {
    compoundConditions.push_back(cm->getCondition());
  }
  std::cout<<name() << ":getConditions. No of conditions: "
	   << compoundConditions.size() << '\n';
  
  return std::make_optional<ConditionsMT>(std::move(compoundConditions));
}


std::size_t
TrigJetHypoToolConfig_leaf::requiresNJets() const {
  std::size_t result{0};
  auto opt_conds = getConditions();
  if(!opt_conds.has_value()){return result;}

  for(const auto& c : *opt_conds){result += c->capacity();}

  return result;
}

 
std::unique_ptr<IJetGrouper>
TrigJetHypoToolConfig_leaf::getJetGrouper() const {

  auto opt_conds = getConditions();

  if(!opt_conds.has_value()){
    return std::unique_ptr<IJetGrouper>(nullptr);
  }

  const auto& f_cond =  (*opt_conds).front();
  auto f_capacity = f_cond->capacity();
  
  if(f_capacity == 1){
    return std::make_unique<SingleJetGrouper>();
  } else {
    return std::make_unique<CombinationsGrouper>(f_capacity);
  }  
}

std::unique_ptr<IGroupsMatcherMT>
TrigJetHypoToolConfig_leaf::getMatcher () const {

  auto opt_conds = getConditions();

  if(!opt_conds.has_value()){
    return std::unique_ptr<IGroupsMatcherMT>(nullptr);
  }

  return groupsMatcherFactoryMT_MaxBipartite(std::move(*opt_conds));
}

StatusCode TrigJetHypoToolConfig_leaf::checkVals() const {
  if (m_conditionMakers.empty()){
    ATH_MSG_ERROR("No conditions supplied");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

std::vector<std::shared_ptr<ICleaner>> 
TrigJetHypoToolConfig_leaf::getCleaners() const {
  std::vector<std::shared_ptr<ICleaner>> v;
  return v;
}


