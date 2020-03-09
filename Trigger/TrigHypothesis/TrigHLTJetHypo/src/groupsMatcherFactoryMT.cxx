/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./groupsMatcherFactoryMT.h"
#include "./MaximumBipartiteGroupsMatcherMT.h"
#include "./PartitionsGroupsMatcherMT.h"
#include "./SingleConditionMatcherMT.h"
#include "./FastReductionMatcher.h"

std::unique_ptr<IGroupsMatcherMT> 
groupsMatcherFactoryMT_SingleCondition (ConditionsMT&& conditions){

  // check the number of conditions to decide the Matcher type.
  if (conditions.size() != 1) {
    return std::make_unique<SingleConditionMatcherMT>(nullptr);
  }
  return std::make_unique<SingleConditionMatcherMT>(std::move(conditions[0]));
}

std::unique_ptr<IGroupsMatcherMT> 
groupsMatcherFactoryMT_MaxBipartite (ConditionsMT&& conditions){

  if (conditions.empty()){
    return std::make_unique<SingleConditionMatcherMT>(nullptr);
  }

  // check the number of conditions to decide the Matcher type.
  if (conditions.size() == 1) {
    return std::make_unique<SingleConditionMatcherMT>(std::move(conditions[0]));
  } else {
    return std::make_unique<MaximumBipartiteGroupsMatcherMT>(std::move(conditions));
  }
  
}


std::unique_ptr<IGroupsMatcherMT> 
groupsMatcherFactoryMT_Partitions (ConditionsMT&& conditions){
  
  if (conditions.size() == 1) {
    return std::make_unique<SingleConditionMatcherMT>(std::move(conditions[0]));
  } else {
    return std::make_unique<PartitionsGroupsMatcherMT>(std::move(conditions));
  }
}


std::unique_ptr<IGroupsMatcherMT> 
groupsMatcherFactoryMT_FastReduction (ConditionsMT&& conditions,
				      const std::vector<std::size_t>& treeVec,
				      const std::vector<std::vector<int>>& sharedNodes){
  
  if (conditions.size() == 1) {
    return std::make_unique<SingleConditionMatcherMT>(std::move(conditions[0]));
  } else {
    return std::make_unique<FastReductionMatcher>(std::move(conditions),
						  treeVec,
						  sharedNodes);
  } 
}



