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
#include <map>
#include <cmath>
#include <sstream>
#include <stdexcept>

MaximumBipartiteGroupsMatcherMT::MaximumBipartiteGroupsMatcherMT(const ConditionsMT& cs):
  m_conditions(cs), 
  m_pass(false),
  m_G(FlowNetwork(0)){
}
  
bool
MaximumBipartiteGroupsMatcherMT::match(const HypoJetGroupCIter& jets_b,
                                       const HypoJetGroupCIter& jets_e,
                                       std::unique_ptr<IConditionVisitor>& visitor){
  // setup a FlowNetwork with number of nodes = number of conditions
  // + number of jets + 2 (source and sink)
  int n_conditions = m_conditions.size();
  int n_jets = jets_e - jets_b;

  if (n_conditions > n_jets){  // caller guarantees n_conditions > 0 
    return false;
  }
    
  int V = n_conditions + n_jets + 2;
  m_G = FlowNetwork(V);

  constexpr double capacity = 1.0;

  // add in edges - source node to each condition node
  // the condition labels start at 1. 0 is reserved for the source node.  
  std::map<int, ConditionsMT::value_type*> conditionsmap;
  int inode = 1;
  for(auto c = m_conditions.begin(); c != m_conditions.end(); ++c){
    m_G.addEdge(std::make_shared<FlowEdge>(0, inode, capacity));
    conditionsmap[inode] = &(*c);
    /*
    std::cout << "FordFulkerson stored condition "
              << inode << " "
              << *c
              <<" "
              << &(*c)
              << '\n';
    */
    ++inode;
  }

  /*
  for(const auto& c : conditionsmap){
      std::cout << "FordFulkerson condition "
                << c.first << " "
                << c.second << " "
                << *(c.second)
                << '\n';
  }
  */
  // add in edges - jet node to sink node of the jet meet a condition.
  // the jet labels start after the conditions labels.
  std::map<int, HypoJetVector> jetmap;
  for (auto c = jets_b; c != jets_e;  ++c){
    jetmap[inode] = *c;
    m_G.addEdge(std::make_shared<FlowEdge>(inode, V-1, capacity));
    ++inode;
  }

  if (inode != V-1){
    throw std::runtime_error("Last node in flow graph not the sink node");
  }

  // add in edges - condition node to jet node for each jet-condition pair where
  //    the jet satisfies the condition

  for(auto c : conditionsmap){
    for (auto j : jetmap){
        //NEEDSFIX START
      // HypoJetVector v{j.second}; // load the jet into a container
      if (c.second->isSatisfied(j.second, visitor)){
        //NEEDSFIX end
        // if (c.second->isSatisfied(v)){
        m_G.addEdge(std::make_shared<FlowEdge>(c.first, j.first, capacity));
      }
    }
  }

  // std::cout << "FlowNetwork:\n " << G <<'\n';

  FordFulkerson ff (m_G, 0, V-1);
  bool pass = std::round(ff.value()) == m_conditions.size();
  /*
  std::cout<<"FordFulkerson value: " 
           << ff.value() 
           << " conditions: "  
           << m_conditions.size()
           << " pass: "
           << std::boolalpha
           << m_pass
           << '\n';
  */
  // record which jets participate in the pass decision
  auto mapend = jetmap.end();

  // loop over edges, figure out if it is a condition - jet edge
  // figure out if it the jet is used (flow == 1)
  // add the jet to passing_jets. As this is a set, duplicates
  // will be removed (each edge appears twice in G).

  HypoJetSet passing_jets;
  
  for(auto edge : m_G.edges()){
    auto it = jetmap.find(edge->to());  // label corresponds ot jet?
      if (it == mapend){continue;}  
      if (std::round(edge->flow()) == 1){ // jet participates in hypo decision?
        //NEEDSFIX START
        // passing_jets.insert(it->second);
        for(auto j : it->second){passing_jets.insert(j);}
        //NEEDSFIX end

        jetmap.erase(it);  // remove passing jets from map
      }
  }
  
  return pass;
  
}


std::string MaximumBipartiteGroupsMatcherMT::toString() const noexcept {
  std::stringstream ss;

  ss << "MaimumBipartiteMatcherMT\n";
  ss << "ConditionsMT:\n";

  for(auto c : m_conditions){ ss << c.toString() << '\n';}
  ss << "FlowNetwork:\n";
  ss << m_G << '\n';
  return ss.str();
}


ConditionsMT MaximumBipartiteGroupsMatcherMT::getConditions() const noexcept {
  return m_conditions;
}


