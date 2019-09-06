/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     UnifiedFlowNetworkMatcher.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//

#include "./UnifiedFlowNetworkMatcher.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/FlowNetwork.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/FordFulkerson.h"
#include "./ITrigJetHypoInfoCollector.h"
#include "./xAODJetCollector.h"
#include "./UnifiedFlowNetworkBuilder.h"
#include "./Tree.h"

#include <cmath>
#include <sstream>
#include <algorithm>
#include <map>

UnifiedFlowNetworkMatcher::UnifiedFlowNetworkMatcher(ConditionsMT&& cs,
						     const std::vector<std::size_t>& treeVec) : FlowNetworkMatcherBase(cs.size()){

  Tree tree(treeVec);
  // calculate the network capacity by adding the capacities of the
  // nodes - first generation nodes - that will be connected to the source.

  // obtain the node ids, then the nodes, then their capacities
  auto firstGenConditions = tree.firstGeneration();
  double totalCapacity =
    std::accumulate(firstGenConditions.begin(),
		    firstGenConditions.end(),
		    0.,
		    [&cs](auto sum, auto index) {
		      return  sum + cs[index]->capacity();});

  m_totalCapacity = totalCapacity;

  m_flowNetworkBuilder =
    std::move(std::make_unique<UnifiedFlowNetworkBuilder>(std::move(cs),
							  tree));
  
}

std::string UnifiedFlowNetworkMatcher::toString() const  {
  std::stringstream ss;

  ss << "UnifiedFlowNetworkMatcher \n";
  // ss << m_flowNetworkBuilder -> toString(); 

  return ss.str();
}

