/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     MaximumBipartiteMatcher.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/MaximumBipartiteMatcher.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/FlowNetwork.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/FordFulkerson.h"
#include <map>
#include <cmath>
#include <sstream>
#include <stdexcept>

MaximumBipartiteMatcher::MaximumBipartiteMatcher(const Conditions& cs,
						 const std::string& name):
  Matcher(name), m_conditions(cs), m_pass(false), m_G(FlowNetwork(0)){}
  
void MaximumBipartiteMatcher::match(const HypoJetIter& jets_b,
				    const HypoJetIter& jets_e){
  // setup a FlowNetwork with number of nodes = number of conditions
  // + number of jets + 2 (source and sink)
  m_passed_jets.clear();
  m_failed_jets.clear();

  int n_conditions = m_conditions.size();
  int n_jets = jets_e - jets_b;

  if (n_conditions > n_jets){  // caller guarantees n_conditions > 0 
    //for(auto it = b; it != e; ++it){
    m_failed_jets.assign(jets_b, jets_e);
    m_pass = false;
    return;
  }
    
  int V = n_conditions + n_jets + 2;
  m_G = FlowNetwork(V);

  constexpr double capacity = 1.0;

  // add in edges - source node to each condition node
  // the condition labels start at 1. 0 is reserved for the source node.  
  std::map<int, Conditions::value_type*> conditionsmap;
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
  std::map<int, pHypoJet> jetmap;
  for (auto c = jets_b; c != jets_e;  ++c){
    jetmap[inode] = *c;
    m_G.addEdge(std::make_shared<FlowEdge>(inode, V-1, capacity));
    ++inode;
  }

  if (inode != V-1){
    throw std::runtime_error("Last node in flow graph nothe sink node");
  }

  // add in edges - condition node to jet node for each jet-condition pair where
  //    the jet satisfies the condition

  for(auto c : conditionsmap){
    for (auto j : jetmap){
      HypoJetVector v{j.second}; // load the jet into a container
      // if (c.second->isSatisfied(j.second)){
      if (c.second->isSatisfied(v)){
        m_G.addEdge(std::make_shared<FlowEdge>(c.first, j.first, capacity));
      }
    }
  }

  // std::cout << "FlowNetwork:\n " << G <<'\n';

  FordFulkerson ff (m_G, 0, V-1);
  m_pass = std::round(ff.value()) == m_conditions.size();
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
        passing_jets.insert(it->second);
        jetmap.erase(it);  // remove passing jets from map
      }
  }

  m_passed_jets.assign(passing_jets.begin(), passing_jets.end());
  
  // only "failing" jets (or non-particpatory jets) left in jetmap
  for(auto p : jetmap){m_failed_jets.push_back(p.second);}

  /*
  if(m_pass){
    std::cout<<"pass:.... " << '\n';
  } else {
    std::cout<<"fail:... " << '\n';
  }
  std::cout << G <<'\n';
  std::cout<<"no of passed jets " << m_passed_jets.size() << '\n';
  for(auto j : m_passed_jets){
    std::cout << (j->p4()).Et() << "  " << j -> eta() << '\n';
  }
  std::cout<<"no of failed jets " << m_failed_jets.size() << '\n';
  for(auto j : m_failed_jets){
    std::cout << (j->p4()).Et() << "  " << j -> eta() << '\n';
  }
  std::cout<<"all jets " << e-b << '\n';
  for (auto j = b; j != e; ++j){
    std::cout << ((*j)->p4()).Et() << "  " << (*j) -> eta() << '\n';
  }
  std::cout<<"start conditions " << '\n';
  for (auto c: m_conditions){
    std::cout << c << '\n';
  }
  std::cout<<"end conditions " << '\n';
  */
  

}


bool MaximumBipartiteMatcher::pass() const {return m_pass;}


const HypoJetVector&
MaximumBipartiteMatcher::passedJets() const noexcept {return m_passed_jets;}


const HypoJetVector&
MaximumBipartiteMatcher::failedJets() const noexcept {return m_failed_jets;}


std::string MaximumBipartiteMatcher::toString() const noexcept {
  std::stringstream ss;

  ss << "MaimumBipartiteMAther/" << Matcher::toString() << '\n';
  ss << "Conditions:\n";

  for(auto c : m_conditions){ ss << c.toString() << '\n';}

  ss << "Passed jets:[" << m_passed_jets.size() <<"]\n";
  for(auto j : m_passed_jets){ss << j->toString() << '\n';}

  ss << "Failed jets:[" << m_failed_jets.size() <<"]\n";
  for(auto j : m_failed_jets){ss <<  j->toString() << '\n';}

  ss << "FlowNetwork:\n";
  ss << m_G << '\n';
  return ss.str();
}


const Conditions& MaximumBipartiteMatcher::getConditions() const noexcept {
  return m_conditions;
}
    
