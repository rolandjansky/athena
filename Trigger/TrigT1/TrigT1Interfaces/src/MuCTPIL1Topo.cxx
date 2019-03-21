/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1Interfaces/MuCTPIL1Topo.h"

namespace LVL1 {

  MuCTPIL1Topo::MuCTPIL1Topo( ):m_bcidOffset(0){

  }
  MuCTPIL1Topo::MuCTPIL1Topo(std::vector<MuCTPIL1TopoCandidate> candList):m_bcidOffset(0){
    m_muonTopoCandidates = candList;
  }

  MuCTPIL1Topo& MuCTPIL1Topo::operator=( const MuCTPIL1Topo& a ) {
    
    clearCandidates();
    m_muonTopoCandidates = a.getCandidates();

    return *this;      
  }
  
  MuCTPIL1Topo& MuCTPIL1Topo::operator+=( const MuCTPIL1Topo& a) {

    std::vector<MuCTPIL1TopoCandidate>::iterator it;
    std::vector<MuCTPIL1TopoCandidate> a_candvect = a.getCandidates();
    for(it = a_candvect.begin(); it != a_candvect.end(); ++it) {
      addCandidate( *it );
    } 
    return *this;    
  }

  MuCTPIL1Topo MuCTPIL1Topo::operator+( const MuCTPIL1Topo& a ) {
    
    MuCTPIL1Topo result = *this;
      result += a;
      return result;
  }
  
  
  std::vector<MuCTPIL1TopoCandidate> MuCTPIL1Topo::getCandidates() const {
     return m_muonTopoCandidates;
   }

  void MuCTPIL1Topo::setCandidates(std::vector<MuCTPIL1TopoCandidate> candList)  {
    clearCandidates();
    m_muonTopoCandidates = candList;
   }
  
  void MuCTPIL1Topo::addCandidate(MuCTPIL1TopoCandidate candidate){
    m_muonTopoCandidates.push_back(candidate);
  }

  void  MuCTPIL1Topo::clearCandidates(){
    m_muonTopoCandidates.clear();
  }

  void MuCTPIL1Topo::print() const {
    for (std::vector<MuCTPIL1TopoCandidate>::const_iterator it =  m_muonTopoCandidates.begin(); 
	 it != m_muonTopoCandidates.end(); ++it) {
      it->print();
    }

  }

} // namespace LVL1
