/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     MaximumBipartiteGroupsMatcherMT.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//

#include "./MaximumBipartiteGroupsMatcherMT.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/FlowNetwork.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/FordFulkerson.h"
#include "./ITrigJetHypoInfoCollector.h"
#include "./xAODJetCollector.h"
#include "./MultijetFlowNetworkBuilder.h"

#include <cmath>
#include <sstream>
#include <algorithm>
#include <map>

MaximumBipartiteGroupsMatcherMT::MaximumBipartiteGroupsMatcherMT(ConditionsMT&& cs) : FlowNetworkMatcherBase(cs.size()){

  double totalCapacity{0};
  for(const auto& cond : cs){
    totalCapacity += cond->capacity();
  }
  setTotalCapacity(totalCapacity);
  
  
  auto builder = std::make_unique<MultijetFlowNetworkBuilder>(std::move(cs));
  setFlowNetworkBuilder(std::move(builder));
}

std::string MaximumBipartiteGroupsMatcherMT::toString() const  {
  std::stringstream ss;

  ss << "MaximumBipartiteGroupsMatcherMT \n";
  ss << FlowNetworkMatcherBase::toString(); 

  return ss.str();
}
