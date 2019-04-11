/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#
#include "./EtaEtConditionMT.h"
#include "./IConditionVisitor.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

#include <sstream>
#include <cmath>
#include <TLorentzVector.h>

EtaEtConditionMT::EtaEtConditionMT(double etaMin,
                                   double etaMax,
                                   double threshold): 
  m_etaMin(etaMin), m_etaMax(etaMax), m_threshold(threshold){
}


bool EtaEtConditionMT::isSatisfied(const pHypoJet& ip,
                                   IConditionVisitor* visitor) const {
  auto abseta = std::abs(ip->eta());
  auto et = ip->et();
  bool result =
    m_etaMin <= abseta and
    m_etaMax > abseta and
    m_threshold <= et;
  if(visitor){
    visitor->visit(this,
                   std::to_string(abseta) + " " +
                   std::to_string(et) + " " +
                   std::to_string(result) + '\n');
  }
  return result;
}


bool 
EtaEtConditionMT::isSatisfied(const HypoJetVector& ips,
                              IConditionVisitor* v) const {
  auto result =  isSatisfied(ips[0], v);
  return result;
}


std::string EtaEtConditionMT::toString() const noexcept {
  std::stringstream ss;
  ss << "Eta Et ConditionMT: etaMin "
     <<  m_etaMin 
     << " etaMax " 
     << m_etaMax 
     << " Et threshold: " 
     << m_threshold
     <<'\n';

  return ss.str();
}
