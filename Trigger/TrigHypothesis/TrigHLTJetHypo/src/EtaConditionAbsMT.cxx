/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#
#include "./EtaConditionAbsMT.h"
#include "./ITrigJetHypoInfoCollector.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

#include <sstream>
#include <cmath>
#include <TLorentzVector.h>

EtaConditionAbsMT::EtaConditionAbsMT(double etaMin,
				     double etaMax):
m_min(etaMin), m_max(etaMax){
}


bool
EtaConditionAbsMT::isSatisfied(const pHypoJet& ip,
			       const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const {
  
  auto abseta = std::abs(ip->eta());
  bool pass = m_min <= abseta and m_max > abseta;
    
  if(collector){
    const void* address = static_cast<const void*>(this);

    std::stringstream ss0;
    ss0 << "EtaCondition: (" << address << ") " 
        << " eta[" << m_min << ", " << m_max << "]" 
        << " pass: "  << std::boolalpha << pass << '\n';

    auto j_addr = static_cast<const void*>(ip.get());
    std::stringstream ss1;
    ss1 <<  "     jet : ("<< j_addr << ") abseta " << abseta << '\n';

    collector->collect(ss0.str(), ss1.str());

  }
  return pass;
}


bool 
EtaConditionAbsMT::isSatisfied(const HypoJetVector& ips,
			    const std::unique_ptr<ITrigJetHypoInfoCollector>& c) const {
  auto result =  isSatisfied(ips[0], c);
  return result;
}


std::string EtaConditionAbsMT::toString() const {
  std::stringstream ss;
  ss << "EtaConditionAbsMT (" << this << ") Capacity: " << s_capacity
     << " etaMin "<<  m_min 
     << " etaMax " << m_max 
       <<'\n';

  return ss.str();
}
