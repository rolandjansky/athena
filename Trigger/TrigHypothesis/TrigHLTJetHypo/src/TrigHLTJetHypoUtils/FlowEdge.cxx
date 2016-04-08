/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     FlowEdge.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//


#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cmath>

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/FlowEdge.h"



FlowEdge::FlowEdge(): m_v{0}, m_w{0}, m_capacity(0.), m_flow(0.){}

FlowEdge::FlowEdge(int v, int w, double capacity):
  m_v{v}, m_w{w}, m_capacity(capacity), m_flow(0.)
  {
    
    if (m_v < 0) {
      throw std::out_of_range("FlowEdge: vertex label must be an int >= 0");
    }
    
    if (m_w < 0) {
      throw std::out_of_range("FlowEdge: vertex label must be an int >= 0");
    }
    
    if (capacity < 0.) {
      throw std::invalid_argument("FlowEdge: edge capacity must be >= 0.");
    }
  }

int FlowEdge::from() const noexcept {return m_v;}

int FlowEdge::to() const noexcept {return m_w;}

double FlowEdge::capacity() const noexcept {return m_capacity;}

double FlowEdge::flow() const noexcept {return m_flow;}

double FlowEdge::other(int vertex) const {
  if (vertex == m_v){return m_w;}
  if (vertex == m_w){return m_v;}
  throw std::invalid_argument("FlowEdge: Illegal endpoint");
}

double FlowEdge::residualCapacityTo(int vertex) const {
  if(vertex == m_v){return m_flow;}
  if(vertex == m_w){return m_capacity - m_flow;}
  throw std::invalid_argument("FlowEdge: Illegal endpoint");
}

  
void FlowEdge::addResidualFlowTo(int vertex, double delta){
  if(vertex == m_v){
    m_flow -= delta;  // forward edge
  } else if(vertex == m_w){
    m_flow += delta;  // backward edge
  } else {
    throw std::invalid_argument("FlowEdge: Illegal endpoint");
  }

  if(std::isnan(delta)){
    throw std::invalid_argument("FlowEdge: Change in flow == NaN");
  }
     
  if(m_flow < 0.){throw std::invalid_argument("FlowEdge: flow < 0.");}
  if(m_flow > m_capacity){
    throw std::invalid_argument("FlowEdge: exceeds capacity");
  }
}

////////////////////////////////////////////////////////////////////

std::ostream& operator << (std::ostream& out, const FlowEdge& e) {
  return out << e.from()
             << "->" 
             << e.to() 
             << " " 
             << e.flow() 
             <<"/" 
             << e.capacity();
}

bool operator < (const FlowEdge& lhs, const FlowEdge& rhs){
  if(lhs.from() > rhs.from()){return false;}
  if(lhs.to() > rhs.to()){return false;}
  return true;
}




