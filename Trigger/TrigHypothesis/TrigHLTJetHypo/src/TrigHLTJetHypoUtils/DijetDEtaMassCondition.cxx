/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/DijetDEtaMassCondition.h"
#include <sstream>
#include <stdexcept>
#include <TLorentzVector.h>
#include <limits>
// #include <iostream>
DijetDEtaMassCondition::DijetDEtaMassCondition(const std::vector<double>& etaMins,
                                               const std::vector<double>& etaMaxs,
                                               const std::vector<double>& etThresholds,
                                               const std::vector<double>& dEtaMins,
                                               const std::vector<double>& dEtaMaxs,
                                               const std::vector<double>& massMins,
                                               const std::vector<double>& massMaxs){

  m_etaMin0 = etaMins[0];
  m_etaMax0 = etaMaxs[0];

  m_etaMin1 = etaMins[1];
  m_etaMax1 = etaMaxs[1];

  m_etMin0 = etThresholds[0];
  m_etMin1 = etThresholds[1];
  
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
    ss << "DijetDEtaMass::isSatisfied must see exactly 2 particles, but received "
       << ips.size()
       << '\n';
    
    throw std::runtime_error(ss.str());
  }

  return passCuts(ips[0], ips[1]) or passCuts(ips[1], ips[0]);
}

bool DijetDEtaMassCondition::passCuts(pHypoJet j0, pHypoJet j1) const {
  auto et0 = j0->et();
  auto et1 = j1->et();

  auto p4_0 = j0->p4();
  auto p4_1 = j1->p4();

  auto eta0 =  j0->eta();
  auto eta1 =  j1->eta();

  auto mass = (p4_0 + p4_1).M();
  auto dEta = std::abs(eta0 -eta1);

  auto absEta0 = std::abs(eta0);
  auto absEta1 = std::abs(eta1);
 

  /*
  auto result =  test(et0, et1, absEta0, absEta1, dEta, mass);
  std::cout << "DijetDEtaMassCondition : " << std::boolalpha << result << '\n'
            << std::setprecision(3) << std::scientific 
            << "jet 0 et " << m_etMin0 << " " << et0 << '\n'
            << "jet 1 et " << m_etMin1 << " " << et1 << '\n'
            << "jet 0 eta " << m_etaMin0 << " " << eta0 << " " << m_etaMax0 << '\n'
            << "jet 1 eta " << m_etaMin1 << " " << eta1 << " " << m_etaMax1 << '\n'
            << "dEta " << m_dEtaMin << " " << dEta << " " << m_dEtaMax << '\n'
            << "mass " << m_massMin << " " << mass << " " <<m_massMax << '\n';

  return result;
  */
  return test(et0, et1, absEta0, absEta1, dEta, mass);

}
 
 bool DijetDEtaMassCondition::test(double et0, double et1, 
                                   double abseta0, double abseta1, 
                                   double dEta, 
                                   double mass) const noexcept {
   return m_etMin0 <= et0 and
     m_etMin1 <= et1 and
     
     m_etaMin0 <= abseta0 and
     m_etaMin1 <= abseta1 and
     
     m_etaMax0 > abseta0 and
     m_etaMax1 > abseta1 and
     
     m_dEtaMin <= dEta and
     m_dEtaMax > dEta and
     
     m_massMin <= mass and
     m_massMax > mass;
}

std::string DijetDEtaMassCondition::toString() const noexcept {
  std::stringstream ss;
  ss << "DijetDEtaMassCondition: "

     << "cuts jet0: "
     << "etMin "
     <<  m_etMin0 
     << " etaMin "
     <<  m_etaMin0 
     << " etaMax " 
     << m_etaMax0 

     << " cuts jet1:"
     << " etMin "
     <<  m_etMin1 
     << " etaMin "
     <<  m_etaMin1 
     << " etaMax " 
     << m_etaMax1

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
