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
  
  
  /* set the capacity of the acceptAll nodes (or nay
     nodes with modifiable capciity.

     Algorithm:
     initialise: create an bool array checked
     with length the number of nodes in the tree
     find the index of the last node whicih is not checked.

    execute: while there are uncheckled nodes{
		 do{
                    find last unchecked node
		    obtain its capacity.
                    set node as checked.
		    move index to parent node.
		    attempt ot set its capacity with child's capacity.
		    if ok: break 
		    obtain the capacity of the current node
		    set current node to checked
		   }
              }
  */

  

  std::vector<bool> checked(m_treeVec.size(), false);
  
  const std::size_t start = checked.size() - 1;
  while(true){

    auto it = std::find(checked.rbegin(),
			checked.rend(),
			false);
    
    if (it == checked.rend()){
      break;
    }
    (*it) = true;
    
    std::size_t ind = start - (it - checked.rbegin());
    
    std::size_t cap{0};
    while(true){
      cap = m_conditionMakers[ind]->capacity();
      ind = m_treeVec[ind];
      // path upwards already traversed from this point if checked = true
      if (checked[ind]){break;}
      if((m_conditionMakers[ind]->addToCapacity(cap))){
	break;
      } else {
	cap = m_conditionMakers[ind]->capacity();
	checked[ind] = true;
      }
    }
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
  
  return std::make_optional<ConditionsMT>(std::move(conditions));
}


std::vector<std::unique_ptr<ConditionFilter>>
TrigJetHypoToolConfig_fastreduction::getConditionFilters() const {

  auto filters = std::vector<std::unique_ptr<ConditionFilter>>();
  
  for(const auto& cm : m_filtConditionMakers){
    ConditionPtrs filterConditions;  // will contain a single Condition
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


