/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1Interfaces/MuCTPIL1Topo.h"

namespace LVL1 {

  LVL1::MuCTPIL1Topo::MuCTPIL1Topo() {}


  LVL1::MuCTPIL1Topo::MuCTPIL1Topo(const std::vector<MuCTPIL1TopoCandidate> & candList)
    : m_muonTopoCandidates(candList)
  {}

  LVL1::MuCTPIL1Topo&
  LVL1::MuCTPIL1Topo::operator=( const MuCTPIL1Topo& a ) {
    if( this != &a ) {  
      m_muonTopoCandidates = a.getCandidates();
    }
    return *this;      
  }
  
  LVL1::MuCTPIL1Topo&
  LVL1::MuCTPIL1Topo::operator+=( const MuCTPIL1Topo& a) {
    for(const MuCTPIL1TopoCandidate & muCand : a.getCandidates())
    {
      addCandidate( muCand );
    } 
    return *this;    
  }

  LVL1::MuCTPIL1Topo
  LVL1::MuCTPIL1Topo::operator+( const MuCTPIL1Topo& a ) {
    MuCTPIL1Topo result = *this;
    result += a;
    return result;
  }
  
  const std::vector<MuCTPIL1TopoCandidate> &
  LVL1::MuCTPIL1Topo::getCandidates() const {
    return m_muonTopoCandidates;
  }

  void
  LVL1::MuCTPIL1Topo::setCandidates(const std::vector<MuCTPIL1TopoCandidate> & candList)  {
    m_muonTopoCandidates = candList;
  }
  
  void
  LVL1::MuCTPIL1Topo::addCandidate(const MuCTPIL1TopoCandidate & candidate){
    m_muonTopoCandidates.push_back(candidate);
  }

  void
  LVL1::MuCTPIL1Topo::clearCandidates(){
    m_muonTopoCandidates.clear();
  }

  void
  LVL1::MuCTPIL1Topo::print() const {
    for (const MuCTPIL1TopoCandidate & muCand : m_muonTopoCandidates)
    {
      muCand.print();
    }
  }

} // namespace LVL1
