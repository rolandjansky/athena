/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigJetHypoToolConfig_fastreduction.h"

#include "GaudiKernel/StatusCode.h"

#include "./conditionsFactoryMT.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJetFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/TrigHLTJetHypoHelper2.h"
#include "./CapacityCheckedCondition.h"
#include "./FastReductionMatcher.h"
#include "./Tree.h"
#include "./ConditionsDefsMT.h"

#include "TrigCompositeUtils/TrigCompositeUtils.h"

using TrigCompositeUtils::DecisionID;
using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;

TrigJetHypoToolConfig_fastreduction::TrigJetHypoToolConfig_fastreduction(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent) :
  base_class(type, name, parent){

}


TrigJetHypoToolConfig_fastreduction::~TrigJetHypoToolConfig_fastreduction(){
}

StatusCode TrigJetHypoToolConfig_fastreduction::initialize() {

  if(m_conditionMakers.size() != m_treeVec.size()){
    ATH_MSG_ERROR("No. of conditions mismatch with tree vector size");
    return StatusCode::FAILURE;
  }
  
  if(m_conditionMakers.size() < 2){ // first  node is root, need more
    ATH_MSG_ERROR("No. of conditions < 2");
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}




std::optional<ConditionPtrs>
TrigJetHypoToolConfig_fastreduction::getCapacityCheckedConditions() const {

  ConditionPtrs conditions;

  // collect the Conditions objects from the various sources
  // return an invalid optional if any src signals a problem

  for(const auto& cm : m_conditionMakers){
    conditions.push_back(cm->getCapacityCheckedCondition());
  }
      
  return std::make_optional<ConditionPtrs>(std::move(conditions));
}

std::optional<ConditionsMT>
TrigJetHypoToolConfig_fastreduction::getConditions() const {
  /* obtain an unchecked set of conditions */
  
  ConditionsMT conditions;
  for(const auto& cm : m_conditionMakers){
    conditions.push_back(cm->getCapacityCheckedCondition());
  }

  for(const auto& cm : m_antiConditionMakers){
    conditions.push_back(cm->getCapacityCheckedAntiCondition());
  }
  
  return std::make_optional<ConditionsMT>(std::move(conditions));
}


std::vector<std::unique_ptr<ConditionFilter>>
TrigJetHypoToolConfig_fastreduction::getConditionFilters() const {

  auto filters = std::vector<std::unique_ptr<ConditionFilter>>();
  
  for(const auto& cm : m_filtConditionMakers){

    ConditionsMT filterConditions;  // will contain a single Condition
    filterConditions.push_back(cm->getCapacityCheckedCondition());
    auto cf = std::make_unique<ConditionFilter>(filterConditions);
    filters.push_back(std::move(cf));
  }
  
  return filters;
}

// following function not used for treeless hypos
std::size_t
TrigJetHypoToolConfig_fastreduction::requiresNJets() const {
  return 0;
}


std::unique_ptr<IJetsMatcherMT>
TrigJetHypoToolConfig_fastreduction::getMatcher () const {

  auto opt_conds = getCapacityCheckedConditions();

  if(!opt_conds.has_value()){
    return std::unique_ptr<IJetsMatcherMT>(nullptr);
  }

  auto matcher =  std::unique_ptr<IJetsMatcherMT>();
  //  matcher.reset(new FastReductionMatcher(std::move(*opt_conds),
  //					 Tree(m_treeVec)));

  auto conditions = std::move(*opt_conds);
  auto filters = getConditionFilters();
  auto fpm = new FastReductionMatcher(conditions,
				      filters,
				      Tree(m_treeVec));
  matcher.reset(fpm);
  return matcher;
}

StatusCode TrigJetHypoToolConfig_fastreduction::checkVals() const {
  return StatusCode::SUCCESS;
}

std::vector<std::shared_ptr<ICleaner>> 
TrigJetHypoToolConfig_fastreduction::getCleaners() const {
  std::vector<std::shared_ptr<ICleaner>> v;
  return v;
}


