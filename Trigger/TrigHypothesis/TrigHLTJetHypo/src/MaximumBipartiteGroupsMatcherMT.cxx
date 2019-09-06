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

  for(const auto& cond : cs){
    m_totalCapacity += cond->capacity();
  }

  m_flowNetworkBuilder =
    std::move(std::make_unique<MultijetFlowNetworkBuilder>(std::move(cs)));
  
}

std::string MaximumBipartiteGroupsMatcherMT::toString() const  {
  std::stringstream ss;

  ss << "MaximumBipartiteGroupsMatcherMT \n";
  ss << m_flowNetworkBuilder -> toString(); 

  return ss.str();
}
