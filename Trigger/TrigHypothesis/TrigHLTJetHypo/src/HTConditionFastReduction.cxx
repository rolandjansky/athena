/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "./HTConditionFastReduction.h"
#include "./ITrigJetHypoInfoCollector.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"

#include <sstream>
#include <cmath>
#include <algorithm>
#include <numeric>

HTConditionFastReduction::HTConditionFastReduction(double ht_min,
						   double ht_max):
  m_htMin(ht_min), m_htMax(ht_max) {
}


bool
HTConditionFastReduction::isSatisfied(const HypoJetVector& ips,
				      const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const {
  
  auto htsum =  std::accumulate(ips.begin(),
				ips.end(),
				0.0,
				[](double sum, const pHypoJet& jp){
				  return sum + jp->et();});
  bool pass = htsum > m_htMin;
  
  if(collector){
    std::stringstream ss0;
    const void* address = static_cast<const void*>(this);
    ss0 << "HTCondition: (" << address << ") Sum(et) "
	<< htsum << " "
	<< std::boolalpha << pass <<  " jet group: \n";
    
    std::stringstream ss1;
    
    for(const auto& ip : ips){
      address = static_cast<const void*>(ip.get());
      ss1 << "    "  << address << " " << ip->eta() << " e " << ip->e() << '\n';
    }
    ss1 << '\n';
    collector -> collect(ss0.str(), ss1.str());
  }
  
  return pass;
  
}
    

std::string HTConditionFastReduction::toString() const {
  std::stringstream ss;
  ss << "HTConditionFastReduction: htMin: "
     << m_htMin;

  

  ss <<'\n';

  return ss.str();
}

