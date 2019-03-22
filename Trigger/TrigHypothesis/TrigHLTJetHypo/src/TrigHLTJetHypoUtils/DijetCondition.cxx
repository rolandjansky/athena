/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/DijetCondition.h"
#include <sstream>
#include <stdexcept>
#include <TLorentzVector.h>
#include <limits>
#include "GaudiKernel/SystemOfUnits.h"

// #include <iostream>
DijetCondition::DijetCondition(
                               double etThreshold0,
                               double etThreshold1,
                               double etaMin0,
                               double etaMax0,
                               double etaMin1,
                               double etaMax1,
                               double massMin,
                               double massMax,
                               double detaMin,
                               double detaMax,
                               double dphiMin,
                               double dphiMax
                               ){

  m_etThreshold0 = etThreshold0;
  m_etThreshold1 = etThreshold1;
  m_etaMin0 = etaMin0;
  m_etaMax0 = etaMax0;
  m_etaMin1 = etaMin1;
  m_etaMax1 = etaMax1;
  m_massMin = massMin;
  m_massMax = massMax;
  m_detaMin = detaMin;
  m_detaMax = detaMax;
  m_dphiMin = dphiMin;
  m_dphiMax = dphiMax;
}


bool DijetCondition::isSatisfied(const HypoJetVector& ips) const{
  if(ips.size() != 2){
    std::stringstream ss;
    ss << "Dijet::isSatisfied must see exactly 2 particles, but received "
       << ips.size()
       << '\n';
    
    throw std::runtime_error(ss.str());
  }

  auto j0 = ips[0];
  auto j1 = ips[1];
  return ( (passJetCuts(j0, j1) or passJetCuts(j1, j0)) and
           passDijetCuts(j0, j1));
}

bool DijetCondition::passJetCuts(pHypoJet j0, pHypoJet j1) const {

  constexpr double invGeV = 1./Gaudi::Units::GeV;

  auto et0 = j0->et() * invGeV;
  auto et1 = j1->et() * invGeV;

  auto eta0 =  j0->eta();
  auto eta1 =  j1->eta();

  auto absEta0 = std::abs(eta0);
  auto absEta1 = std::abs(eta1);

  return (m_etThreshold0 <= et0 and
          m_etThreshold1 <= et1 and
    
          m_etaMin0 <= absEta0 and
          m_etaMin1 <= absEta1 and
          
          m_etaMax0 > absEta0 and
          m_etaMax1 > absEta1);
}


bool DijetCondition::passDijetCuts(pHypoJet j0, pHypoJet j1) const {

  constexpr double invGeV = 1./Gaudi::Units::GeV;
  auto rj0 = (j0 -> p4()) * invGeV;
  auto rj1 = (j1 -> p4()) * invGeV;

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
  std::cout << "DijetCondition : " << std::boolalpha << result << '\n'
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

std::string DijetCondition::toString() const noexcept {
  std::stringstream ss;
  ss << "DijetCondition: "

     << "cuts jet0: "
     << "etThreshold "
     <<  m_etThreshold0 
     << " etaMin "
     <<  m_etaMin0 
     << " etaMax " 
     << m_etaMax0 

     << " cuts jet1:"
     << " etThreshold "
     <<  m_etThreshold1 
     << " etaMin "
     <<  m_etaMin1 
     << " etaMax " 
     << m_etaMax1

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


double DijetCondition::orderingParameter() const noexcept {
  return m_massMin;
}
