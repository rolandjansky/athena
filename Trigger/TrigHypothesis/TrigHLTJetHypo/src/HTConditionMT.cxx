/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "./HTConditionMT.h"
#include "./ITrigJetHypoInfoCollector.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

#include <sstream>
#include <cmath>
#include <algorithm>
#include <numeric>

HTConditionMT::HTConditionMT(double ht_min,
			     double et_min,
			     double abseta_min,
			     double abseta_max): m_htMin(ht_min) {
  
  if (et_min > 0 or
      abseta_min > 0 or
      abseta_max < std::numeric_limits<double>::max()) {
    m_filter.reset(new Filter(et_min, abseta_min, abseta_max));
  }
}


bool
HTConditionMT::isSatisfied(const HypoJetVector& ips,
			   const std::unique_ptr<ITrigJetHypoInfoCollector>& infoCollector) const {

  if (m_filter) {
    HypoJetVector ips_c(ips.begin(), ips.end());
    auto iter = std::partition(ips_c.begin(), ips_c.end(), *m_filter);
    if (infoCollector) {
      infoCollector -> collect("", "No of jets after filtering " +
			       std::to_string(iter - ips_c.begin()));
    } 

    return std::accumulate(ips_c.begin(),
			   iter,
			   0.0,
			   [](double sum, const HypoJet::IJet* jp){
			     return sum + jp->et();}) > m_htMin;
  } else {
    if(infoCollector) {
      infoCollector -> collect("", "Nofiltering");
    }

    return std::accumulate(ips.begin(),
			   ips.end(),
			   0.0,
			   [](double sum, const HypoJet::IJet* jp){
			     return sum + jp->et();}) > m_htMin;
  }
}
    

std::string HTConditionMT::toString() const noexcept {
  std::stringstream ss;
  ss << "HTConditionMT: htMin: "
     << m_htMin;

  if (m_filter) {
    ss << " filter etmin: " << m_filter->etmin_
       << " absetamin: " << m_filter->absetamin_
       << " absetamax: " << m_filter->absetamax_;
  } else {
    ss << " no filter";
  }

  ss <<'\n';

  return ss.str();
}

