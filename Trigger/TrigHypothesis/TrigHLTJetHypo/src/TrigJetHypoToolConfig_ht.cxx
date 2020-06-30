/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigJetHypoToolConfig_ht.h"
#include "GaudiKernel/StatusCode.h"

#include "./conditionsFactoryMT.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/AllJetsGrouper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJetFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/TrigHLTJetHypoHelper2.h"
#include "./groupsMatcherFactoryMT.h"

#include "TrigCompositeUtils/TrigCompositeUtils.h"

using TrigCompositeUtils::DecisionID;
using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;

TrigJetHypoToolConfig_ht::TrigJetHypoToolConfig_ht(const std::string& type,
						   const std::string& name,
						   const IInterface* parent) :
  base_class(type, name, parent){

}


TrigJetHypoToolConfig_ht::~TrigJetHypoToolConfig_ht(){
}

StatusCode TrigJetHypoToolConfig_ht::initialize() {

  if(m_conditionMakers.empty()){
    ATH_MSG_ERROR("No. of conditions mismatch configurers supplied");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}




std::optional<ConditionsMT>
TrigJetHypoToolConfig_ht::getConditions() const {

  ConditionsMT conditions;

  // collect the Conditions objects from the various sources
  // return an invalid optional if any src signals a problem
  for(const auto& cm : m_conditionMakers){
    conditions.push_back(cm->getCondition());
  }
    
  return std::make_optional<ConditionsMT>(std::move(conditions));
}


// following function not used for treeless hypos
std::size_t
TrigJetHypoToolConfig_ht::requiresNJets() const {
  return 0;
}

 
std::unique_ptr<IJetGrouper>
TrigJetHypoToolConfig_ht::getJetGrouper() const {
  return std::make_unique<AllJetsGrouper>();
}

std::unique_ptr<IGroupsMatcherMT>
TrigJetHypoToolConfig_ht::getMatcher () const {

  auto opt_conds = getConditions();

  if(!opt_conds.has_value()){
    return std::unique_ptr<IGroupsMatcherMT>(nullptr);
  }

  return groupsMatcherFactoryMT_SingleCondition(std::move(*opt_conds));
}

StatusCode TrigJetHypoToolConfig_ht::checkVals() const {
  return StatusCode::SUCCESS;
}

std::vector<std::shared_ptr<ICleaner>> 
TrigJetHypoToolConfig_ht::getCleaners() const {
  std::vector<std::shared_ptr<ICleaner>> v;
  return v;
}


