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

MaximumBipartiteGroupsMatcherMT::MaximumBipartiteGroupsMatcherMT(const ConditionsMT& cs): m_conditions(cs){
  m_flowNetworkBuilder = std::unique_ptr<IFlowNetworkBuilder>();
  m_flowNetworkBuilder.reset(new MultijetFlowNetworkBuilder(cs));
  m_nConditions = cs.size();
}

std::optional<bool>
MaximumBipartiteGroupsMatcherMT::match(const HypoJetGroupCIter& groups_b,
                                       const HypoJetGroupCIter& groups_e,
				       xAODJetCollector& jetCollector,
                                       const std::unique_ptr<ITrigJetHypoInfoCollector>& collector,
                                       bool) const {
  /* setup a FlowNetwork. 
     if group size == 1:
     nodes include source, conditions, jets groups, sink
     otherwise:
     nodes include source, conditions, jets groups, jets, sink
     
     Then solve the network (FordFulkerson)
  */
  
  if(!m_nConditions){
    if(collector){
      collector->collect("MaximumBipartiteGroupsMatcherMT", "No conditions");
    }
    return std::make_optional<bool>(false);
  }
  
  // Determine jet group - condition satisfaction.

  auto iter_diff = groups_e - groups_b;  // number of jet groups
  if (iter_diff < 0){return std::optional<bool>();}  // must be postive
  if(static_cast<std::size_t>(iter_diff) < m_nConditions) {
    if(collector){
      collector->collect("MaximumBipartiteGroupsMatcherMT",
			 "Too few jet groups " +
			 std::to_string(m_nConditions) + " " +
			 std::to_string(iter_diff));
    }
    return std::make_optional<bool>(false);
  }

  std::map<int, pHypoJet> nodeToJet; 

  // auto edges = make_flowEdges(groups_b, groups_e, collector, V, nodeToJet);
  auto G = m_flowNetworkBuilder -> create(groups_b,
                                          groups_e,
                                          collector,
                                          nodeToJet);
  if(!G.has_value()){
    if(collector){collector -> collect("MaximumBipartiteGroupsMatcher",
				       "Network construction early return");}
    return std::make_optional<bool> (false);
  }

  
  if(collector){
    std::stringstream ss;
    ss << **G;
    collector->collect("MaximumBipartiteGroupsMatcher - before", ss.str());
  }

  
  auto V = (*G)->V();
  FordFulkerson ff (**G, 0, V-1);
  if(ff.error()){
    if(collector){
      std::string msg("FordFulkerson error: ");
      msg += ff.errMsg() + "\n";
      collector -> collect("MaximumBipartiteGroupsMatcher", msg);
    }
    return std::optional<bool> ();
  }

  if(collector){
    std::stringstream ss;
    ss << **G;
    collector->collect("MaximumBipartiteGroupsMatcher - after", ss.str());
  }
  
  auto edges = (*G)->edges();
  
  
  double totalCapacity = 0;
  for(const auto& cond : m_conditions){
    totalCapacity += cond.capacity();
  }
  bool pass = std::round(ff.value()) == totalCapacity;

  // loop over edges, figure out if it is a condition - jet edge
  // figure out if it the jet is used (flow == 1)
  // add the jet to passing_jets. As this is a set, duplicates
  // will be removed (each edge appears twice in G).
  
  HypoJetVector passing_jets;
  auto iter = std::partition(edges.begin(),
                             edges.end(),
                             [V](const auto& edge){return edge->to() == V-1 and
                                                   std::round(edge->flow()) == 1;});
  
  std::transform(edges.begin(),
                 iter,
                 std::back_inserter(passing_jets),
                 [&nodeToJet](const auto& edge){return nodeToJet[edge->from()];});

  jetCollector.addJets(passing_jets.cbegin(), passing_jets.cend());
	       
  
  return std::make_optional<bool>(pass);
  
}


std::string MaximumBipartiteGroupsMatcherMT::toString() const noexcept {
  std::stringstream ss;

  ss << "MaximumBipartiteMatcherMT\n";
  ss << "ConditionsMT:\n";

  for(auto c : m_conditions){ ss << c.toString() << '\n';}
  ss << "FlowNetwork:\n";
  return ss.str();
}


ConditionsMT MaximumBipartiteGroupsMatcherMT::getConditions() const noexcept {
  return m_conditions;
}

