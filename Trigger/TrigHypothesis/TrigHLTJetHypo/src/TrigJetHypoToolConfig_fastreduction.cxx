/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigJetHypoToolConfig_fastreduction.h"

#include "GaudiKernel/StatusCode.h"

#include "./conditionsFactoryMT.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/SingleJetGrouper.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJetFactory.h"
// #include "TrigHLTJetHypo/TrigHLTJetHypoUtils/groupsMatcherFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CleanerFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/TrigHLTJetHypoHelper2.h"
#include "./groupsMatcherFactoryMT.h"

#include "DecisionHandling/TrigCompositeUtils.h"

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
    return StatusCode::FAILURE;
  }
  
  if(m_conditionMakers.size() < 2){ // first  node is root, need more
    return StatusCode::FAILURE;
  }
  
  // gymnastics as cannot pass vecor<vecotr<int>> as a Gaudi::Property
  if(m_sharedNodesVec.empty()){return StatusCode::FAILURE;}

  std::vector<int> shared;
  for(const auto& i : m_sharedNodesVec){
    if(i  == -1){
      m_sharedNodes.push_back(shared);
      shared = std::vector<int>();
    } else {
      shared.push_back(i);
    }
  }
  if(!shared.empty()){
    m_sharedNodes.push_back(shared);
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




std::optional<ConditionsMT>
TrigJetHypoToolConfig_fastreduction::getConditions() const {

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
TrigJetHypoToolConfig_fastreduction::requiresNJets() const {
  return 0;
}

 
std::unique_ptr<IJetGrouper>
TrigJetHypoToolConfig_fastreduction::getJetGrouper() const {
  return std::make_unique<SingleJetGrouper>();
}

std::unique_ptr<IGroupsMatcherMT>
TrigJetHypoToolConfig_fastreduction::getMatcher () const {

  auto opt_conds = getConditions();

  if(!opt_conds.has_value()){
    return std::unique_ptr<IGroupsMatcherMT>(nullptr);
  }

  return groupsMatcherFactoryMT_FastReduction(std::move(*opt_conds),
					      m_treeVec,
					      m_sharedNodes);
}

StatusCode TrigJetHypoToolConfig_fastreduction::checkVals() const {
  return StatusCode::SUCCESS;
}

std::vector<std::shared_ptr<ICleaner>> 
TrigJetHypoToolConfig_fastreduction::getCleaners() const {
  std::vector<std::shared_ptr<ICleaner>> v;
  return v;
}


