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

void
MaximumBipartiteGroupsMatcherMT::reportPassingJets(const std::map<int, pHypoJet>& nodeToJet,
						   const std::unique_ptr<FlowNetwork>& G,
						   const std::unique_ptr<ITrigJetHypoInfoCollector>& collector,
						   xAODJetCollector& jetCollector
						   ) const{
  HypoJetVector passing_jets;

  auto V = G->V();
  auto edges = G->edges();

  auto iter =
    std::partition(edges.begin(),
		   edges.end(),
		   [V](const auto& edge){return edge->to() == V-1 and
					 std::round(edge->flow()) == 1;});
  
  std::transform(edges.begin(),
                  iter,
		 std::back_inserter(passing_jets),
		 [&nodeToJet](const auto& edge){
		   return nodeToJet.at(edge->from());});

  
  if(collector){
    std::stringstream ss;
    for(const auto& j  : passing_jets){
      ss << "e: " << j->e() << " et " << j->et() << " eta " << j->eta() << '\n';
    }
    collector->collect("MaximumBipartiteGroupsMatcher passing jets", ss.str());
  }

  jetCollector.addJets(passing_jets.cbegin(), passing_jets.cend());
  
}
