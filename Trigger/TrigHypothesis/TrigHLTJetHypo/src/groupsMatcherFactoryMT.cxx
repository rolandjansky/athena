/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./groupsMatcherFactoryMT.h"
#include "./MaximumBipartiteGroupsMatcherMT.h"
#include "./PartitionsGroupsMatcherMT.h"
#include "./SingleConditionMatcherMT.h"


std::unique_ptr<IGroupsMatcherMT> 
groupsMatcherFactoryMT_MaxBipartite (const ConditionsMT& conditions){
  
  auto matcher = std::unique_ptr<IGroupsMatcherMT> (nullptr);
  
  if (conditions.size() == 1) {
    matcher.reset(new SingleConditionMatcherMT(conditions[0]));
  } else {
    matcher.reset(new MaximumBipartiteGroupsMatcherMT(conditions));
  }

  return matcher;
}


std::unique_ptr<IGroupsMatcherMT> 
groupsMatcherFactoryMT_Partitions (const ConditionsMT& conditions){
  
  auto matcher = std::unique_ptr<IGroupsMatcherMT> (nullptr);
  
  if (conditions.size() == 1) {
    matcher.reset(new SingleConditionMatcherMT(conditions[0]));
  } else {
    matcher.reset(new PartitionsGroupsMatcherMT(conditions));
  }

  return matcher;
}



