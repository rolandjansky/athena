/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/FalseCondition.h"
#include <sstream>
#include <stdexcept>
#include <TLorentzVector.h>
#include <limits>
// #include <iostream>
//
FalseCondition::FalseCondition(
                double limitMin,
                double limitMax
                              ){
  m_limitMin = limitMin;
  m_limitMax = limitMax;
}


bool FalseCondition::isSatisfied(const HypoJetVector& ips) const{
  if(ips.size() != 1){
    std::stringstream ss;
    ss << "False::isSatisfied must see exactly 1 particle, but received "
       << ips.size()
       << '\n';
    
    throw std::runtime_error(ss.str());
  }

  return false;
}


std::string FalseCondition::toString() const noexcept {
  std::stringstream ss;
  ss << "FalseCondition: "
     <<'\n';

  return ss.str();
}


double FalseCondition::orderingParameter() const noexcept {
  return 1;
}

