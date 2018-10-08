/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/DijetDPhiCondition.h"
#include <sstream>
#include <stdexcept>
#include <TLorentzVector.h>
#include <limits>
// #include <iostream>
DijetDPhiCondition::DijetDPhiCondition(const std::vector<double>& dPhiMins,
                                       const std::vector<double>& dPhiMaxs){

  m_dPhiMin = 0.;
  m_dPhiMax = std::numeric_limits<double>::max();

  if (!dPhiMins.empty()){
    m_dPhiMin = dPhiMins[0];
  }

  if (!dPhiMaxs.empty()){
    m_dPhiMax = dPhiMaxs[0];
  }

}

bool DijetDPhiCondition::isSatisfied(const HypoJetVector& ips) const{
  if(ips.size() != 2){
    std::stringstream ss;
    ss << "DijetDPhi::isSatisfied must see exactly 2 particles, but received "
       << ips.size()
       << '\n';
    
    throw std::runtime_error(ss.str());
  }

  double dPhi = std::abs( (ips[0]->p4()).DeltaPhi(ips[1]->p4()) );
                         
  return m_dPhiMin <= dPhi and m_dPhiMax > dPhi;

}


std::string DijetDPhiCondition::toString() const noexcept {
  std::stringstream ss;
  ss << "DijetDPhiCondition: "

     << " dPhi min: " 
     << m_dPhiMin
     << " dPhi max: " 
     << m_dPhiMax
     << " mass min: " 
     <<'\n';

  return ss.str();
}


double DijetDPhiCondition::orderingParameter() const noexcept {
  return m_dPhiMin;
}
