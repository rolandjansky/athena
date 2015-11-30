/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigJetHypo/TrigHLTJetHypoUtils/TLACondition.h"
#include <sstream>
#include <exception>


TLACondition::TLACondition(double etaMin, double etaMax,
			   double ystarMin, double ystarMax,
			   double massMin, double massMax): 
  m_etaMin(etaMin), m_etaMax(etaMax), 
  m_ystarMin(ystarMin), m_ystarMax(ystarMax), 
  m_massMin(massMin), m_massMax(massMax){
}

bool TLACondition::isSatisfied(const xAOD::IParticle*) const {
  std::stringstream ss;
  ss << "TLA::isSatisfied must see exactly 2 particles, but recieved 1\n";
  throw std::runtime_error(ss.str());
}

bool
TLACondition::isSatisfied(const std::vector<const xAOD::IParticle*>& ips) const{
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
  auto eta0 =  p4_0.Eta();
  auto eta1 =  p4_1.Eta();
  auto ystar = std::abs(eta0 - eta1);

    
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
  ss << "Condition: etaMin "
     <<  m_etaMin 
     << " etaMax " 
     << m_etaMax 
     << " ystart min: " 
     << m_ystarMin
     << " ystart max: " 
     << m_ystarMax
     << " mass min: " 
     << m_massMin
     << " mass max: " 
     << m_massMax
     <<'\n';

  return ss.str();
}


double TLACondition::orderingParameter() const noexcept {return m_massMin;}
