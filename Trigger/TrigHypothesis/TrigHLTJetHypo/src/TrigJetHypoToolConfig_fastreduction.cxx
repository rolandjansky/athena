/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigJetHypoToolConfig_fastreduction.h"

#include "GaudiKernel/StatusCode.h"

#include "./RepeatedCondition.h"
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




ConditionPtrs
TrigJetHypoToolConfig_fastreduction::getRepeatedConditions() const {

  ConditionPtrs conditions;

  // collect the Conditions objects from the various sources

  for(const auto& cm : m_conditionMakers){
    conditions.push_back(cm->getRepeatedCondition());
  }
      
  return conditions;
}


std::vector<std::unique_ptr<ConditionFilter>>
TrigJetHypoToolConfig_fastreduction::getConditionFilters() const {

  auto filters = std::vector<std::unique_ptr<ConditionFilter>>();
  
  for(const auto& cm : m_filtConditionMakers){

    ConditionsMT filterConditions;  // will contain a single Condition
    ConditionPtr repeatedCondition = cm->getRepeatedCondition();

    // repeatedPtr is a nullptr is there are no contained conditions.
    // this means the filter should act as pass-through.
    if (repeatedCondition) {
      filterConditions.push_back(std::move(repeatedCondition));
    }
    
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

  auto repeatedConds = getRepeatedConditions();

  if(repeatedConds.empty()){
    return std::unique_ptr<IJetsMatcherMT>(nullptr);
  }

  auto matcher =  std::unique_ptr<IJetsMatcherMT>();

  auto conditions = std::move(repeatedConds);
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

