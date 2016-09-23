/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     FlowNetwork.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//

#include <stdexcept>
#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>
#include <limits>

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/FlowEdge.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/FlowNetwork.h"

FlowNetwork::FlowNetwork(int v):
  m_v{v}, m_e{0},m_adj{std::vector<FlowEdgeBag>(v)}{}

/* Commenting out this constructor used for testing due to inane complaints
   from Coverity 

FlowNetwork::FlowNetwork(std::fstream& in){
  int v, e;
  in >> v;

  if(v < 0){
    throw std::out_of_range("FlowNetwork: vertex count must be non-negative");
  } else {
    // silly branch to pacify the static code checker Coverity
  }

  m_v = v;
  m_adj = std::vector<FlowEdgeBag>(v);
  in >> e;
  if(e < 0){
    throw std::out_of_range("FlowNetwork: edge count must be non-negative");
  } else {
    // silly branch to pacify the static code checker Coverity
  }

  m_e = 0;
    
  for(auto i = 0; i != e; ++i){
    int v, w;
    in >> v >> w;
    if(v < 0 || v > m_v){
      throw std::out_of_range("FlowNetwork: vertex " +
                              std::to_string(v) + 
                              " not in range [0, " +
                              std::to_string(m_v) +
                              ")");
    }
    
    if(w < 0 || w > m_v){
      throw std::out_of_range("FlowNetwork: vertex " +
                              std::to_string(w) + 
                              " not in range [0, " +
                              std::to_string(m_v) +
                              ")"
                              );
    }
      
    double capacity;
    in >> capacity;
    addEdge(std::make_shared<FlowEdge>(v, w, capacity));
  }
}
*/
              
int FlowNetwork::V() const noexcept {return m_v;}
int FlowNetwork::E() const noexcept {return m_e;}

void FlowNetwork::addEdge(spFE e){
  int v = e->from();
  int w = e->to();

  validateVertex(v);
  validateVertex(w);

  m_adj[v].push_back(e);
  m_adj[w].push_back(e);

  ++m_e;
}

FlowEdgeBag FlowNetwork::adj(int v) const {
  validateVertex(v);
  return m_adj[v];
}

std::vector<spFE> FlowNetwork::edges() const {
  std::vector<spFE> edges;
  for (auto v = 0; v != m_v; ++v){
    for (auto e : m_adj[v]){if(e->to() != v){edges.push_back(e);}} 
  }
  return edges;
}

void FlowNetwork::validateVertex(int v) const {
  if (v < 0 || v >= m_v){

    std::ostringstream ost;    
    ost << "FlowNetwork: invalid vertex: " 
        << v 
        << " must be in [0, "
        << m_v
        <<")";

    throw std::out_of_range(ost.str());
  }
}


std::ostream& operator << (std::ostream& out, const FlowNetwork& n){
  auto n_v = n.V();
  out << std::to_string(n_v) 
      << " " 
      << std::to_string(n.E()) + '\n';

  for(auto v = 0; v < n_v ; ++v){
    out << std::to_string(v) + ": ";
    for(auto e : n.adj(v)){
      // if(e.to() != v){out << e << " ";}
      out << *e << " ";
    }
    out << '\n';
  }
  return out;
}




