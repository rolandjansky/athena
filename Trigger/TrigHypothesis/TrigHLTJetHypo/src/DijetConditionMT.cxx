/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./DijetConditionMT.h"
#include "./ITrigJetHypoInfoCollector.h"
#include <sstream>
#include <stdexcept>
#include <TLorentzVector.h>
#include <limits>
#include <memory>

DijetConditionMT::DijetConditionMT(double massMin,
                                   double massMax,
                                   double detaMin,
                                   double detaMax,
                                   double dphiMin,
                                   double dphiMax) :

  m_massMin(massMin),
  m_massMax(massMax),
  m_detaMin(detaMin),
  m_detaMax(detaMax),
  m_dphiMin(dphiMin),
  m_dphiMax(dphiMax) {
}
  

bool
DijetConditionMT::isSatisfied(const HypoJetVector& ips,
                              const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const{

  if(ips.size() != 2){
    std::stringstream ss;
    ss << "DijetMT::isSatisfied must see exactly 2 particles, but received "
       << ips.size()
       << '\n';
    
    throw std::runtime_error(ss.str());
  }

  auto j0 = ips[0];
  auto j1 = ips[1];

  auto rj0 = j0 -> p4();
  auto rj1 = j1 -> p4();

  auto mass = (rj0 + rj1).M();

  auto eta0 =  j0->eta();
  auto eta1 =  j1->eta();
  auto adeta = std::abs(eta0 -eta1);
  auto dphi = std::abs(rj0.DeltaPhi(rj1));


  bool pass{true};
  
  if (m_dphiMin > dphi or dphi >= m_dphiMax){pass = false;}
  if (m_massMin > mass or mass >= m_massMax){pass = false;}
  if (m_detaMin > adeta or adeta >= m_detaMax){pass = false;}

   if(collector){
     std::stringstream ss0;
     const void* address = static_cast<const void*>(this);
     ss0 << "DijetConditionMT: " << address << " dphi "
        << dphi <<  " mass " <<  mass <<  " adeta "  << adeta <<  " pass: " 
        <<std::boolalpha << pass <<  " jet group: \n";

     std::stringstream ss1;

     for(auto ip : ips){
       address = static_cast<const void*>(ip);
       ss1 << "    "  << address << " " << ip->eta() << " e " << ip->e() << '\n';
     }
     ss1 << '\n';
     collector -> collect(ss0.str(), ss1.str());
   }
   return pass;

}

std::string DijetConditionMT::toString() const noexcept {


  std::stringstream ss;
  const void* address = static_cast<const void*>(this);
  ss << "DijetConditionMT: " << address << " "
    
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
