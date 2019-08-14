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
                                                       double threshold,
						       unsigned int conditionID):

  m_etaMin(etaMin), m_etaMax(etaMax), m_threshold(threshold),
  m_conditionID(conditionID){
}


bool
EtaEtAsymmetricConditionMT::isSatisfied(const pHypoJet& ip,
                                        const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const {
  auto eta = ip->eta();
  auto et = ip->et();
  
  bool pass =
    m_etaMin <= eta and
    m_etaMax > eta and
    m_threshold <= et;
  
  if(collector){
    std::stringstream ss0;
    const void* address = static_cast<const void*>(this);
    ss0 << "EtatAsymmetricConditionMT: (" << address << ") eta ["
        << m_etaMin << ", " <<  m_etaMax << "] "
        << " et thresh " <<  m_threshold <<  " pass " 
        << std::boolalpha << pass <<  '\n';
                                      
                                      
    std::stringstream ss1;
    auto j_addr = static_cast<const void*>(ip);
    ss1 << "    jet: ("  << j_addr << ")"
        << " eta " << eta
        << " et " << et << '\n';
    
    collector->collect(ss0.str(), ss1.str());
  }
  
  return pass;

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

