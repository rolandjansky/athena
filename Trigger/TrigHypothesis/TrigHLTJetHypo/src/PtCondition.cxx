/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#
#include "./PtCondition.h"
#include "./ITrigJetHypoInfoCollector.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

#include <sstream>
#include <cmath>
#include <TLorentzVector.h>

PtCondition::PtCondition(double threshold) : m_min(threshold) {
}


bool PtCondition::isSatisfied(const pHypoJet& ip,
				const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const {

  auto pt = ip->pt();
  bool pass = m_min <= pt;

  
  if(collector){
    const void* address = static_cast<const void*>(this);

    std::stringstream ss0;
    ss0 << "PtCondition: (" << address << ") " 
        << " pt thresh " << m_min
        << " pass: "  << std::boolalpha << pass << '\n';

    auto j_addr = static_cast<const void*>(ip.get());
    std::stringstream ss1;
    ss1 <<  "     jet : ("<< j_addr << ")"
        " pt " << pt << '\n';
    
    collector->collect(ss0.str(), ss1.str());

  }
  return pass;
}


bool 
PtCondition::isSatisfied(const HypoJetVector& ips,
			   const std::unique_ptr<ITrigJetHypoInfoCollector>& c) const {
  auto result =  isSatisfied(ips[0], c);
  return result;
}


std::string PtCondition::toString() const {
  std::stringstream ss;
  ss << "PtCondition (" << this << ") "
     << " Pt threshold: " 
     << m_min
     <<'\n';

  return ss.str();
}
