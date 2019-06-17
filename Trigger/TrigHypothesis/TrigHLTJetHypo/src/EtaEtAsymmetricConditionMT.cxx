/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./EtaEtAsymmetricConditionMT.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "./ITrigJetHypoInfoCollector.h"

#include <sstream>
#include <cmath>
#include <TLorentzVector.h>


EtaEtAsymmetricConditionMT::EtaEtAsymmetricConditionMT(double etaMin, 
                                                       double etaMax, 
                                                       double threshold):

  m_etaMin(etaMin), m_etaMax(etaMax), m_threshold(threshold){
}


bool
EtaEtAsymmetricConditionMT::isSatisfied(const pHypoJet& ip,
                                        const std::unique_ptr<ITrigJetHypoInfoCollector>&) const {
  auto eta = ip->eta();
  auto et = ip->et();
  return 
    m_etaMin <= eta and
    m_etaMax > eta and
    m_threshold <= et;
}


bool
EtaEtAsymmetricConditionMT::isSatisfied(const HypoJetVector& ips,
                                        const std::unique_ptr<ITrigJetHypoInfoCollector>& v) const {
  return isSatisfied(ips[0], v);
}


std::string EtaEtAsymmetricConditionMT::toString() const noexcept {
  std::stringstream ss;
  ss << "EtaEtAsymmetric ConditionMT: etaMin "
     <<  m_etaMin 
     << " etaMax " 
     << m_etaMax 
     << " threshold: " 
     << m_threshold
     <<'\n';

  return ss.str();
}

