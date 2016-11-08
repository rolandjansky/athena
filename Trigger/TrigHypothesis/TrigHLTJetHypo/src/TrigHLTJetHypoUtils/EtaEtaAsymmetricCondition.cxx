/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/EtaEtAsymmetricCondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

#include <sstream>
#include <cmath>
#include <TLorentzVector.h>


EtaEtAsymmetricCondition::EtaEtAsymmetricCondition(double etaMin, 
                                                   double etaMax, 
                                                   double threshold): 
  m_etaMin(etaMin), m_etaMax(etaMax), m_threshold(threshold){}


bool EtaEtAsymmetricCondition::isSatisfied(const pHypoJet& ip) const {
  auto eta = ip->eta();
  auto et = ip->et();
  return 
    m_etaMin <= eta and
    m_etaMax > eta and
    m_threshold <= et;
}


bool 
EtaEtAsymmetricCondition::isSatisfied(const HypoJetVector& ips) 
  const {
  return isSatisfied(ips[0]);
}


std::string EtaEtAsymmetricCondition::toString() const noexcept {
  std::stringstream ss;
  ss << "EtaEtAsymmetric Condition: etaMin "
     <<  m_etaMin 
     << " etaMax " 
     << m_etaMax 
     << " threshold: " 
     << m_threshold
     <<'\n';

  return ss.str();
}


double EtaEtAsymmetricCondition::orderingParameter() const noexcept {
  return m_threshold;
}
