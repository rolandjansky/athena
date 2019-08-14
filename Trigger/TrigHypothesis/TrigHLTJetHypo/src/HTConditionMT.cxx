/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "./HTConditionMT.h"
#include "./ITrigJetHypoInfoCollector.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

#include <sstream>
#include <cmath>
#include <algorithm>
#include <numeric>

HTConditionMT::HTConditionMT(double htMin,
			     unsigned int conditionID):
  m_htMin(htMin), m_conditionID(conditionID){
}


bool HTConditionMT::isSatisfied(const HypoJetVector& ips,
                                const std::unique_ptr<ITrigJetHypoInfoCollector>&) const {
  return std::accumulate(ips.begin(),
                         ips.end(),
                         0.0,
                         [](double sum, const HypoJet::IJet* jp){
                           return sum + jp->et();}) > m_htMin;
}


std::string HTConditionMT::toString() const noexcept {
  std::stringstream ss;
  ss << "HTConditionMT: htMin: "
     << m_htMin
     <<'\n';

  return ss.str();
}

