/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HTCondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

#include <sstream>
#include <cmath>
#include <algorithm>
#include <numeric>

HTCondition::HTCondition(double htMin): m_htMin(htMin){
}


bool 
HTCondition::isSatisfied(const HypoJetVector& ips) const {
  return std::accumulate(ips.begin(),
                         ips.end(),
                         0.0,
                         [](double sum, const HypoJet::IJet* jp){
                           return sum + jp->et();}) > m_htMin;
}


std::string HTCondition::toString() const noexcept {
  std::stringstream ss;
  ss << "HTCondition: htMin: "
     << m_htMin
     <<'\n';

  return ss.str();
}


double HTCondition::orderingParameter() const noexcept {return m_htMin;}
