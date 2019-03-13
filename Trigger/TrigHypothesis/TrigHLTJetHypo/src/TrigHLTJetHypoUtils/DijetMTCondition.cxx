/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/DijetMTCondition.h"
#include <sstream>
#include <stdexcept>
#include <TLorentzVector.h>
#include <limits>
// #include <iostream>
DijetMTCondition::DijetMTCondition(double massMin,
                                   double massMax,
                                   double detaMin,
                                   double detaMax,
                                   double dphiMin,
                                   double dphiMax
                                   ){
  m_massMin = massMin;
  m_massMax = massMax;
  m_detaMin = detaMin;
  m_detaMax = detaMax;
  m_dphiMin = dphiMin;
  m_dphiMax = dphiMax;
}


bool DijetMTCondition::isSatisfied(const HypoJetVector& ips) const{
  if(ips.size() != 2){
    std::stringstream ss;
    ss << "DijetMT::isSatisfied must see exactly 2 particles, but received "
       << ips.size()
       << '\n';
    
    throw std::runtime_error(ss.str());
  }

  auto j0 = ips[0];
  auto j1 = ips[1];

  auto rj0 = 0.001 * (j0 -> p4());
  auto rj1 = 0.001 * (j1 -> p4());

  auto mass = (rj0 + rj1).M();
  if (m_massMin > mass or mass >= m_massMax){return false;}

  
  auto eta0 =  j0->eta();
  auto eta1 =  j1->eta();
  auto adeta = std::abs(eta0 -eta1);
  if (m_detaMin > adeta or adeta >= m_detaMax){return false;}


  auto dphi = std::abs(rj0.DeltaPhi(rj1));
  if (m_dphiMin > dphi or dphi >= m_dphiMax){return false;}

  return true;

  /*
  auto result =  test(et0, et1, absEta0, absEta1, dEta, mass);
  std::cout << "DijetMTCondition : " << std::boolalpha << result << '\n'
            << std::setprecision(3) << std::scientific 
            << "jet 0 et " << m_etThreshold0 << " " << et0 << '\n'
            << "jet 1 et " << m_etThreshold1 << " " << et1 << '\n'
            << "jet 0 eta " << m_etaMin0 << " " << eta0 << " " << m_etaMax0 << '\n'
            << "jet 1 eta " << m_etaMin1 << " " << eta1 << " " << m_etaMax1 << '\n'
            << "dEta " << m_dEtaMin << " " << dEta << " " << m_dEtaMax << '\n'
            << "mass " << m_massMin << " " << mass << " " <<m_massMax << '\n';

  return result;
  */

}

std::string DijetMTCondition::toString() const noexcept {
  std::stringstream ss;
  ss << "DijetMTCondition: "

     << " mass min: " 
     << m_massMin
     << " mass max: " 
     << m_massMax

     << " dEta min: " 
     << m_detaMin
     << " dEta max: " 
     << m_detaMax

     << " dPhi min: " 
     << m_dphiMin
     << " dPhi max: " 
     << m_dphiMax

     <<'\n';

  return ss.str();
}


double DijetMTCondition::orderingParameter() const noexcept {
  return m_massMin;
}
