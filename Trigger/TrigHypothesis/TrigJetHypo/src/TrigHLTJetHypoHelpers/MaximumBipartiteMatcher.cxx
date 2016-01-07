/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     MaximumBipartiteMatcher.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//

#include <stdexcept>
#include "TrigJetHypo/TrigHLTJetHypoUtils/MaximumBipartiteMatcher.h"
#include "./FlowNetwork.h"
#include "./FordFulkerson.h"

MaximumBipartiteMatcher::MaximumBipartiteMatcher(const Conditions& cs,
						 const std::string& name):
  Matcher(name), m_conditions(cs), m_pass(false){}
  
void MaximumBipartiteMatcher::match(JetIter b, JetIter e){

  // setup a FlowNetwork with number of nodes = number of conditions
  // + number of jets + 2 (source and sink)
  m_passed_jets.clear();
  m_failed_jets.clear();

  int n_conditions = m_conditions.size();
  int n_jets = e-b;

  if (n_conditions > n_jets){  // caller guarantees n_conditions > 0 
    for(auto it = b; it != e; ++it){m_failed_jets.assign(b, e);}
    m_pass = false;
    return;
  }
    
  int V = n_conditions + n_jets + 2;
  FlowNetwork G(V);

  constexpr double capacity = 1.0;

  // add in edges - source node to each condition node
  // the condition labels start at 1. 0 is reserved for the source node.  
  std::map<int, Conditions::value_type*> conditionsmap;
  int inode = 1;
  for(auto c = m_conditions.begin(); c != m_conditions.end(); ++c){
    G.addEdge(std::make_shared<FlowEdge>(0, inode, capacity));
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
  std::map<int, const xAOD::Jet*> jetmap;
  for (auto j = b; j != e; ++j){
    jetmap[inode] = *j;
    G.addEdge(std::make_shared<FlowEdge>(inode, V-1, capacity));
    ++inode;
  }

  if (inode != V-1){
    throw std::runtime_error("Last node in flow graph nothe sink node");
  }

  // add in edges - condition node to jet node for each jet-condition pair where
  //    the jet satisfies the condition

  for(auto c : conditionsmap){
    for (auto j : jetmap){
      if (c.second->isSatisfied(j.second)){
        G.addEdge(std::make_shared<FlowEdge>(c.first, j.first, capacity));
      }
    }
  }

  // std::cout << "FlowNetwork:\n " << G <<'\n';

  FordFulkerson ff (G, 0, V-1);
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

  JetSet passing_jets;
  
  if(m_pass){
    for(auto edge : G.edges()){
      auto it = jetmap.find(edge->to());  // label corresponds ot jet?
      if (it == mapend){continue;}  
      if (std::round(edge->flow()) == 1){ // jet participates in hypo decision?
        passing_jets.insert(it->second);
        jetmap.erase(it);  // remove passing jets from map
      }
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


std::pair<JetCIter, JetCIter> 
MaximumBipartiteMatcher::passed_iters() const {
  return  std::pair<JetCIter, JetCIter> (m_passed_jets.begin(),
                                         m_passed_jets.end());}

std::pair<JetCIter, JetCIter> 
MaximumBipartiteMatcher::failed_iters() const {
  return  std::pair<JetCIter, JetCIter> (m_failed_jets.begin(),
                                         m_failed_jets.end());}


std::string MaximumBipartiteMatcher::toString() const noexcept {
  std::string s = Matcher::toString();
  for(auto c : m_conditions){ s += "\n" + c.toString();}
  return s;
}


const Conditions& MaximumBipartiteMatcher::getConditions() const noexcept {
  return m_conditions;
}
    
