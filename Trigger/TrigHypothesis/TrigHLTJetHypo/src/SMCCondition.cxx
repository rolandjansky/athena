/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#
#include "./SMCCondition.h"
#include "./ITrigJetHypoInfoCollector.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

#include <sstream>
#include <cmath>
#include <TLorentzVector.h>

SMCCondition::SMCCondition(double massMin,
			       double massMax):
m_min(massMin), m_max(massMax){
}


bool
SMCCondition::isSatisfied(const pHypoJet& ip,
			       const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const {
  
  auto mass = std::abs(ip->m());
  bool pass = m_min <= mass and m_max > mass;
    
  if(collector){
    const void* address = static_cast<const void*>(this);

    std::stringstream ss0;
    ss0 << "SMCCondition: (" << address << ") " 
        << " mass[" << m_min << ", " << m_max << "]" 
        << " pass: "  << std::boolalpha << pass << '\n';

    auto j_addr = static_cast<const void*>(ip.get());
    std::stringstream ss1;
    ss1 <<  "     jet : ("<< j_addr << ") jet mass " << mass << '\n';

    collector->collect(ss0.str(), ss1.str());

  }
  return pass;
}


bool 
SMCCondition::isSatisfied(const HypoJetVector& ips,
			    const std::unique_ptr<ITrigJetHypoInfoCollector>& c) const {
  auto result =  isSatisfied(ips[0], c);
  return result;
}


std::string SMCCondition::toString() const {
  std::stringstream ss;
  ss << "SMCCondition (" << this << ") mass min "
     <<  m_min 
     << " mass max " 
     << m_max 
       <<'\n';

  return ss.str();
}
