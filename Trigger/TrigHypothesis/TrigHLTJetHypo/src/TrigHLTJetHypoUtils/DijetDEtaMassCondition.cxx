/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/DijetDEtaMassCondition.h"
#include <sstream>
#include <stdexcept>
#include <TLorentzVector.h>
#include <limits>

DijetDEtaMassCondition::DijetDEtaMassCondition(const std::vector<double>& etaMins,
                                               const std::vector<double>& etaMaxs,
                                               const std::vector<double>& dEtaMins,
                                               const std::vector<double>& dEtaMaxs,
                                               const std::vector<double>& massMins,
                                               const std::vector<double>& massMaxs){

  m_etaMin = etaMins[0];
  m_etaMax = etaMaxs[0];
  
  m_dEtaMin = 0.;
  m_dEtaMax = std::numeric_limits<double>::max();
  
  m_massMin = 0.;
  m_massMax = std::numeric_limits<double>::max();

  if (!dEtaMins.empty()){
    m_dEtaMin = dEtaMins[0];
  }


  if (!dEtaMaxs.empty()){
    m_dEtaMax = dEtaMaxs[0];
  }

  if (!massMins.empty()){
    m_massMin = massMins[0];
  }

  if (!massMaxs.empty()){
    m_massMax = massMaxs[0];
  }

}


bool
DijetDEtaMassCondition::isSatisfied(const HypoJetVector& ips) const{
  if(ips.size() != 2){
    std::stringstream ss;
    ss << "DijetDEtaMass::isSatisfied must see exactly 2 particles, but recieved "
       << ips.size()
       << '\n';
    
    throw std::runtime_error(ss.str());
  }

  auto p4_0 = ips[0]->p4();
  auto p4_1 = ips[1]->p4();
  auto mass = (p4_0 + p4_1).M();
  auto eta0 =  ips[0]->eta();
  auto eta1 =  ips[1]->eta();
  auto dEta = std::abs(eta0 -eta1);
  eta0 = std::abs(eta0);
  eta1 = std::abs(eta1);
    
  return 
    m_etaMin <= eta0 and
    m_etaMin <= eta1 and
    m_etaMax > eta0 and
    m_etaMax  > eta1 and
    m_dEtaMin <= dEta and
    m_dEtaMax > dEta and
    m_massMin <= mass and
    m_massMax > mass;
}

std::string DijetDEtaMassCondition::toString() const noexcept {
  std::stringstream ss;
  ss << "Condition: etaMin "
     <<  m_etaMin 
     << " etaMax " 
     << m_etaMax 
     << " dEta min: " 
     << m_dEtaMin
     << " dEta max: " 
     << m_dEtaMax
     << " mass min: " 
     << m_massMin
     << " mass max: " 
     << m_massMax
     <<'\n';

  return ss.str();
}


double DijetDEtaMassCondition::orderingParameter() const noexcept {
  return m_massMin;
}
