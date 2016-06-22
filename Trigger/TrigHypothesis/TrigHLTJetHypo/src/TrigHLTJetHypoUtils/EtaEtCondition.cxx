/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/EtaEtCondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

#include <sstream>
#include <cmath>
#include <TLorentzVector.h>


EtaEtCondition::EtaEtCondition(double etaMin, double etaMax, double threshold): 
  m_etaMin(etaMin), m_etaMax(etaMax), m_threshold(threshold){}


bool EtaEtCondition::isSatisfied(const pHypoJet& ip) const {
  auto abseta = std::abs(ip->eta());
  auto et = ip->et();
  return 
    m_etaMin <= abseta and
    m_etaMax > abseta and
    m_threshold <= et;
}


bool 
EtaEtCondition::isSatisfied(const HypoJetVector& ips) 
  const {
  return isSatisfied(ips[0]);
}


std::string EtaEtCondition::toString() const noexcept {
  std::stringstream ss;
  ss << "Condition: etaMin "
     <<  m_etaMin 
     << " etaMax " 
     << m_etaMax 
     << " threshold: " 
     << m_threshold
     <<'\n';

  return ss.str();
}


double EtaEtCondition::orderingParameter() const noexcept {return m_threshold;}
