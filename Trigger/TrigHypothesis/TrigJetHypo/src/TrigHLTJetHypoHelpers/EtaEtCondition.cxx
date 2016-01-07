/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigJetHypo/TrigHLTJetHypoUtils/EtaEtCondition.h"
#include <sstream>
#include <cmath>


EtaEtCondition::EtaEtCondition(double etaMin, double etaMax, double threshold): 
  m_etaMin(etaMin), m_etaMax(etaMax), m_threshold(threshold){}


bool EtaEtCondition::isSatisfied(const xAOD::IParticle* ip) const {
  auto abseta = std::abs((ip->p4()).Eta());
  auto et = (ip->p4()).Et();
  return 
    m_etaMin <= abseta and
    m_etaMax > abseta and
    m_threshold <= et;
}


bool 
EtaEtCondition::isSatisfied(const std::vector<const xAOD::IParticle*>& ips) 
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
