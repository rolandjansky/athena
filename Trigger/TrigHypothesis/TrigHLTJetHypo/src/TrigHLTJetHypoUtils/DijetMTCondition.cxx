/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/DijetMTCondition.h"
#include <sstream>
#include <stdexcept>
#include <TLorentzVector.h>
#include <limits>
#include <memory>

DijetMTCondition::DijetMTCondition(double massMin,
                                   double massMax,
                                   double detaMin,
                                   double detaMax,
                                   double dphiMin,
                                   double dphiMax,
                                   bool debug
                                   ) :
  m_massMin(massMin),
  m_massMax(massMax),
  m_detaMin(detaMin),
  m_detaMax(detaMax),
  m_dphiMin(dphiMin),
  m_dphiMax(dphiMax),
  m_debug(debug),
  m_history(std::make_unique<std::vector<CallHistory>>()){
  
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

  auto eta0 =  j0->eta();
  auto eta1 =  j1->eta();
  auto adeta = std::abs(eta0 -eta1);
  auto dphi = std::abs(rj0.DeltaPhi(rj1));


  typedef std::pair<std::string, double> CallHistoryElement;

  if (m_debug){
    m_history->push_back(CallHistory{
        CallHistoryElement("mass", mass),
          CallHistoryElement("deta", adeta),
          CallHistoryElement("dphi", dphi)}
      );
  }

  if (m_dphiMin > dphi or dphi >= m_dphiMax){return false;}
  if (m_massMin > mass or mass >= m_massMax){return false;}
  if (m_detaMin > adeta or adeta >= m_detaMax){return false;}
  
  return true;

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

  if (m_debug){
    ss << "details length( " << m_history->size() << "):\n";
    for(const auto& he : *m_history){
      for(const auto& item: he){
        ss << "  " << item.first << " " << item.second <<'\n';
      }
    }
  }
  
  return ss.str();
}


double DijetMTCondition::orderingParameter() const noexcept {
  return m_massMin;
}

void DijetMTCondition::resetHistory() noexcept {
  m_history->clear();
}
