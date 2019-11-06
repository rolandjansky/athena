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

						     const std::vector<std::size_t>& treeVec,
						     const std::vector<std::vector<int>>& sharedNodes
						     ) : FlowNetworkMatcherBase(cs.size()){

  Tree tree(treeVec);
  // calculate the network capacity by adding the capacities of the
  // nodes - first generation nodes - that will be connected to the source.

  // obtain the node ids, then the nodes, then their capacities

  //  array of indices for the Conditions vector
  auto firstGenerations = tree.firstGeneration();
    
  double totalCapacity =
    std::accumulate(firstGenerations.begin(),
		    firstGenerations.end(),
		    0.,
		    [&cs](auto sum, auto ind) {
		      //cnvrt from tree indices to Condition vector inds by -1:
		      return  sum + cs[ind]->capacity();});  
  setTotalCapacity(totalCapacity);

  // create the flow network builder. Foe each event this will
  // create a flow work from its conditions and the jets of the event.
  setFlowNetworkBuilder(std::move(std::make_unique<UnifiedFlowNetworkBuilder>(std::move(cs),
									      tree,
									      sharedNodes))
			);			
}

std::string UnifiedFlowNetworkMatcher::toString() const  {
  std::stringstream ss;

  ss << "UnifiedFlowNetworkMatcher \n";
  ss <<  FlowNetworkMatcherBase::toString(); 

  return ss.str();
}

