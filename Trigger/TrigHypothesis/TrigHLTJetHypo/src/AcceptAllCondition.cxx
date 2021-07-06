/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#
#include "./AcceptAllCondition.h"
#include "./ITrigJetHypoInfoCollector.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

#include <sstream>
#include <cmath>
#include <TLorentzVector.h>

bool 
AcceptAllCondition::isSatisfied(const HypoJetVector& ips,
                              const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const {

  if(collector){
    std::stringstream ss0;
    const void* address = static_cast<const void*>(this);
    ss0 << "AcceptAllCondition: (" << address << ") "
	<<   " true jet group: \n";
    
    std::stringstream ss1;
    
    for(const auto& ip : ips){
      address = static_cast<const void*>(ip.get());
      ss1 << "    "  << address << " " << ip->eta() << " e " << ip->e() << '\n';
    }
    ss1 << '\n';
    collector -> collect(ss0.str(), ss1.str());
  }

  return true;
}



std::string AcceptAllCondition::toString() const {
  std::stringstream ss;
  
  ss << "AcceptAllCondition (" << this << ")\n";
  return ss.str();
}
