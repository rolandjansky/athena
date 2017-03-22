/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/SingleJetMassCondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

#include <sstream>
#include <cmath>
#include <TLorentzVector.h>


SingleJetMassCondition::SingleJetMassCondition(
        double etaMin, 
        double etaMax, 
        double threshold,
        double massMin,
        double massMax) : EtaEtCondition(etaMin, etaMax, threshold),
m_massMin(massMin),m_massMax(massMax){}


bool SingleJetMassCondition::isSatisfied(const pHypoJet& ip) const {

  if (!EtaEtCondition::isSatisfied(ip)) 
      return false;

  auto mass = ip->m();
  return 
    m_massMin <= mass and
    m_massMax > mass;
}


bool 
SingleJetMassCondition::isSatisfied(const HypoJetVector& ips) 
  const {
  return isSatisfied(ips[0]);
}


std::string SingleJetMassCondition::toString() const noexcept {
  std::stringstream ss;
  ss << EtaEtCondition::toString()
     << " Mass Window: massMin "
     << m_massMin
     << " massMax: "
     << m_massMax
     <<'\n';

  return ss.str();
}


double SingleJetMassCondition::orderingParameter() const noexcept {return m_massMin;}
