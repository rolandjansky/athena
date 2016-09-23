/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     FordFulkerson.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//

#include <stdexcept>
#include <cmath>
#include <vector>

#include <limits>
#include <queue>

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/FordFulkerson.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/FlowNetwork.h"

FordFulkerson::FordFulkerson(const FlowNetwork& G, int s, int t):
  m_marked{}, m_edgeTo{}, m_value(0){
  validate(s, G.V());
  validate(t, G.V());
  if(s == t){
    throw std::invalid_argument("FordFulkerson: source equals sink");
  }
  
  checkFeasibility(G, s, t);  // throws if error

  // while there exists an augmenting path, use it
  m_value = excess(G, t);
  while (hasAugmentingPath(G, s, t)) {
    
    // compute bottleneck capacity
    double bottle = std::numeric_limits<double>::infinity();
    for (int v = t; v != s; v = m_edgeTo[v]->other(v)) {
      bottle = std::min(bottle, m_edgeTo[v]->residualCapacityTo(v));
    }

    // augment flow
    for (int v = t; v != s; v = m_edgeTo[v]->other(v)) {
      m_edgeTo[v]->addResidualFlowTo(v, bottle); 
    }

    m_value += bottle;
  }

  check(G, s, t);  // check optimality conditions, throws if error

 }
 

double FordFulkerson::value() const noexcept {return m_value;}

// is v in the s side of the min s-t cut?

 bool FordFulkerson::inCut(int v) {
   validate(v, m_marked.size());
   return m_marked[v];
 }


void FordFulkerson::validate(int v, int V) {
  if (v < 0 || v >= V) {

    throw std::out_of_range("FordFulkerson: invalid vertex: " + 
                            std::to_string(v) + 
                            " must be in [0, " +
                            std::to_string(V) +
                            ")"
                            );
  }
}


void FordFulkerson::checkFeasibility(const FlowNetwork& G, int s, int t) const{
  constexpr auto EPSILON = 1E-11;
    
  // check that capacity constraints are satisfied
  for(auto v = 0; v < G.V(); v++) {
    for(auto e : G.adj(v)) {
      if (e->flow() < -EPSILON || e->flow() > e->capacity() + EPSILON) {
        throw 
          std::runtime_error("Edge does not satisfy capacity constraints");
      }
    }
  }

  // check that net flow into a vertex equals zero, except at source and sink
  if (std::abs(m_value + excess(G, s)) > EPSILON) {
    throw std::runtime_error("FordFulkerson: " 
                             "Excess at source = " + 
                             std::to_string(excess(G, s)) + 
                             '\n' + 
                             "Max flow         = " + 
                             std::to_string(m_value));
  }
    
  if (std::abs(m_value - excess(G, t)) > EPSILON) {
    throw std::runtime_error("FordFulkerson: "
                             "Excess at sink   = " + 
                             std::to_string(excess(G, t)) + 
                             "Max flow         = " + 
                             std::to_string(m_value));
  }
        
  for (int v = 0; v < G.V(); v++) {
    if (v == s || v == t) continue;
    else if (std::abs(excess(G, v)) > EPSILON) {
      throw std::runtime_error("Net flow out of " + 
                                 std::to_string(v) + 
                               " doesn't equal zero");
    }
  }
}


double FordFulkerson::excess(const FlowNetwork& G, int v) const noexcept{
  double xs = 0.;
  for(auto e : G.adj(v)) {
    if (v == e->from()) {
      xs -= e->flow();
    } else {
      xs += e->flow();
    }
  }
  
  return xs;
}
  

void FordFulkerson::check(const FlowNetwork& G, int s, int t) {
  // check optimality conditions
  
  // check that flow is feasible
  checkFeasibility(G, s, t);  // thows if error
  
  // check that s is on the source side of min cut and that t 
  // is not on source side
  if (!inCut(s)) {
    throw std::runtime_error("FordFulkerson: source " + 
                             std::to_string(s) + 
                             " is not on source side of min cut");
  }
  if (inCut(t)) {
    throw std::runtime_error("FordFulkerson: sink " + 
                             std::to_string(t) + 
                             " is on source side of min cut");
  }

  // check that value of min cut = value of max flow
  double mincutValue = 0.0;
  for (int v = 0; v < G.V(); v++) {
    for (auto e : G.adj(v)) {
      if ((v == e->from()) && inCut(e->from()) && !inCut(e->to()))
        mincutValue += e->capacity();
    }
  }

  constexpr auto EPSILON = 1E-11;
  if (std::abs(mincutValue - m_value) > EPSILON) {
    throw std::runtime_error("Max flow value = " + 
                             std::to_string(m_value) +
                             ", min cut value = " + 
                             std::to_string(mincutValue));
  }
  
}
 
bool FordFulkerson::hasAugmentingPath(const FlowNetwork& G, int s, int t) {
  /* is there an augmenting path? 
     if so, upon termination m_edgeTo[] will contain a parent-link 
     representation of such a path
     this implementation finds a shortest augmenting path 
     (fewest number of edges),
     which performs well both in theory and in practice */

  m_edgeTo = std::vector<spFE>(G.V());
  m_marked = std::vector<bool> (G.V(), false);
  
  // breadth-first search
  std::queue<int> queue;
  queue.push(s);
  m_marked[s] = true;
  while (!queue.empty() && !m_marked[t]) {
    int v = queue.front();
    queue.pop();

    for (auto e : G.adj(v)) {
      int w = e->other(v);

      // if residual capacity from v to w
      if (e->residualCapacityTo(w) > 0) {
        if (!m_marked[w]) {
          m_edgeTo[w] = e;
          m_marked[w] = true;
          queue.push(w);
        }
      }
    }
  }
  
  // is there an augmenting path?
  return m_marked[t];
}



