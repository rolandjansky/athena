/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#
#include "./AcceptAllConditionMT.h"
#include "./ITrigJetHypoInfoCollector.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

#include <sstream>
#include <cmath>
#include <TLorentzVector.h>

bool 
AcceptAllConditionMT::isSatisfied(const HypoJetVector& ips,
                              const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const {

  if(collector){
    std::stringstream ss0;
    const void* address = static_cast<const void*>(this);
    ss0 << "AcceptAllConditionMT: (" << address << ") "
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



std::string AcceptAllConditionMT::toString() const {
  std::stringstream ss;
  
  ss << "AcceptAllConditionMT (" << this << ")\n";
  return ss.str();
}
