/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/groupsMatcherFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/MaximumBipartiteGroupsMatcher.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/SingleConditionMatcher.h"


std::unique_ptr<IGroupsMatcher> 
groupsMatcherFactory(const Conditions& conditions){
  
  auto matcher = std::unique_ptr<IGroupsMatcher> (nullptr);

  if (conditions.size() == 1) {
    matcher.reset(new SingleConditionMatcher(conditions[0]));
  } else {
    matcher.reset(new MaximumBipartiteGroupsMatcher(conditions));
  }

  // matcher.reset(new MaximumBipartiteGroupsMatcher(conditions));
  return matcher;
}



