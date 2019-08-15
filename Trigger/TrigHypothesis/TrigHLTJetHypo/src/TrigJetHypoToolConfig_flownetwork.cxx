/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigJetHypoToolConfig_flownetwork.h"

#include "GaudiKernel/StatusCode.h"

#include "./conditionsFactoryMT.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/SingleJetGrouper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJetFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/groupsMatcherFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/TrigHLTJetHypoHelper2.h"
#include "./groupsMatcherFactoryMT.h"

#include "DecisionHandling/TrigCompositeUtils.h"

using TrigCompositeUtils::DecisionID;
using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;

TrigJetHypoToolConfig_flownetwork::TrigJetHypoToolConfig_flownetwork(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent) :
  base_class(type, name, parent){

}


TrigJetHypoToolConfig_flownetwork::~TrigJetHypoToolConfig_flownetwork(){
}

StatusCode TrigJetHypoToolConfig_flownetwork::initialize() {
  return StatusCode::SUCCESS;
}




std::optional<ConditionsMT>
TrigJetHypoToolConfig_flownetwork::getConditions() const {

  ConditionsMT conditions;

  // collect the Conditions objects from the various sources
  // return an invalid optional if any src signals a problem
  for(const auto& cm : m_conditionMakers){
    auto opConds = cm->getConditions();
    if(opConds.has_value()){
      conditions.insert(conditions.end(),
			std::make_move_iterator(opConds->begin()),
			std::make_move_iterator(opConds->end()));
    } else {
      return std::optional<ConditionsMT>{};
    }
  }
    

  // sort the conditions by the conditionID. The ordering must match the
  // that expected by the "parents" vector.
  std::sort(conditions.begin(), conditions.end(), [](const auto& l,
						     const auto& r){
	      return l->conditionID() < r->conditionID();});

  return std::make_optional<ConditionsMT>(std::move(conditions));
}


// following function not used for treeless hypos
std::size_t
TrigJetHypoToolConfig_flownetwork::requiresNJets() const {
  return 0;
}

 
std::unique_ptr<IJetGrouper>
TrigJetHypoToolConfig_flownetwork::getJetGrouper() const {
  return std::make_unique<SingleJetGrouper>();
}

std::unique_ptr<IGroupsMatcherMT>
TrigJetHypoToolConfig_flownetwork::getMatcher () const {

  auto opt_conds = getConditions();

  if(!opt_conds.has_value()){
    return std::unique_ptr<IGroupsMatcherMT>(nullptr);
  }
  
  return groupsMatcherFactoryMT_MaxBipartite(std::move(*opt_conds));
}

StatusCode TrigJetHypoToolConfig_flownetwork::checkVals() const {
  for(const auto& cm : m_conditionMakers){
    CHECK(cm->checkVals());
  }
    
  return StatusCode::SUCCESS;
}

std::vector<std::shared_ptr<ICleaner>> 
TrigJetHypoToolConfig_flownetwork::getCleaners() const {
  std::vector<std::shared_ptr<ICleaner>> v;
  return v;
}


