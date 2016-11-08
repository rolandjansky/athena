/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/TLACondition.h"
#include <sstream>
#include <stdexcept>
#include <TLorentzVector.h>
#include <limits>

TLACondition::TLACondition(const std::vector<double>& etaMins,
                           const std::vector<double>& etaMaxs,
                           const std::vector<double>& ystarMins,
                           const std::vector<double>& ystarMaxs,
                           const std::vector<double>& massMins,
                           const std::vector<double>& massMaxs){

  m_etaMin = etaMins[0];
  m_etaMax = etaMaxs[0];
  
  m_ystarMin = 0.;
  m_ystarMax = std::numeric_limits<double>::max();
  
  m_massMin = 0.;
  m_massMax = std::numeric_limits<double>::max();

  if (!ystarMins.empty()){
    m_ystarMin = ystarMins[0];
  }


  if (!ystarMaxs.empty()){
    m_ystarMax = ystarMaxs[0];
  }

  if (!massMins.empty()){
    m_massMin = massMins[0];
  }

  if (!massMaxs.empty()){
    m_massMax = massMaxs[0];
  }

}

TLACondition::TLACondition(double etaMin,
                           double etaMax,
                           double ystarMin,
                           double ystarMax,
                           double massMin,
                           double massMax):
  m_etaMin(etaMin), m_etaMax(etaMax),
  m_ystarMin(ystarMin), m_ystarMax(ystarMax),
  m_massMin(massMin),m_massMax(massMax){
}

bool TLACondition::isSatisfied(const HypoJetVector& ips) const{
  if(ips.size() != 2){
    std::stringstream ss;
    ss << "TLA::isSatisfied must see exactly 2 particles, but recieved "
       << ips.size()
       << '\n';
    
    throw std::runtime_error(ss.str());
  }

  auto p4_0 = ips[0]->p4();
  auto p4_1 = ips[1]->p4();
  auto mass = (p4_0 + p4_1).M();
  auto eta0 =  ips[0]->eta();
  auto eta1 =  ips[1]->eta();
  auto rap0 = ips[0]->rapidity();
  auto rap1 = ips[1]->rapidity();
  auto ystar = 0.5*std::abs(rap0 - rap1);

    
  return 
    m_etaMin <= eta0 and
    m_etaMin <= eta1 and
    m_etaMax > eta0 and
    m_etaMax  > eta1 and
    m_ystarMin <= ystar and
    m_ystarMax > ystar and
    m_massMin <= mass and
    m_massMax > mass;
}

std::string TLACondition::toString() const noexcept {
  std::stringstream ss;
  ss << "TLACondition: etaMin "
     <<  m_etaMin 
     << " etaMax " 
     << m_etaMax 
     << " ystar min: " 
     << m_ystarMin
     << " ystar max: " 
     << m_ystarMax
     << " mass min: " 
     << m_massMin
     << " mass max: " 
     << m_massMax
     <<'\n';

  return ss.str();
}


double TLACondition::orderingParameter() const noexcept {
  return m_massMin;
}
