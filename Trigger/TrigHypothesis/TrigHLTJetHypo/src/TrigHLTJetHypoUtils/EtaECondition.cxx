/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/EtaECondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

#include <sstream>
#include <cmath>
#include <TLorentzVector.h>


EtaECondition::EtaECondition(double etaMin, double etaMax, double threshold): 
  m_etaMin(etaMin), m_etaMax(etaMax), m_threshold(threshold){}


bool EtaECondition::isSatisfied(const pHypoJet& ip) const {
  auto abseta = std::abs(ip->eta());
  auto e = ip->e();
  return 
    m_etaMin <= abseta and
    m_etaMax > abseta and
    m_threshold <= e;
}


bool 
EtaECondition::isSatisfied(const HypoJetVector& ips) 
  const {
  return isSatisfied(ips[0]);
}


std::string EtaECondition::toString() const noexcept {
  std::stringstream ss;
  ss << "Eta E Condition: etaMin "
     <<  m_etaMin 
     << " etaMax " 
     << m_etaMax 
     << " E threshold: " 
     << m_threshold
     <<'\n';

  return ss.str();
}


double EtaECondition::orderingParameter() const noexcept {return m_threshold;}
