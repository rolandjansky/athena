/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./groupsMatcherFactoryMT.h"
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
groupsMatcherFactoryMT_FastReduction (ConditionPtrs&& conditions,
				      const std::vector<std::size_t>& treeVec){
  
  if (conditions.size() == 1) {
    return std::make_unique<SingleConditionMatcherMT>(std::move(conditions[0]));
  } else {
    return std::make_unique<FastReductionMatcher>(std::move(conditions),
						  treeVec);
  } 
}



