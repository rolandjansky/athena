/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HTCondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/EtaEtCondition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"

#include <sstream>
#include <cmath>
#include <algorithm>
#include <numeric>

HTCondition::HTCondition(double etaMin, double etaMax, 
                         double etMin, double htMin): 
  m_etaMin(etaMin), m_etaMax(etaMax), 
  m_etMin(etMin), m_htMin(htMin){
}


bool 
HTCondition::isSatisfied(const HypoJetVector& ips) const {

  EtaEtCondition condition(m_etaMin, m_etaMax, m_etMin);
  // temporary copy: the partitioning will be moved to cleaning
  HypoJetVector v(ips.begin(), ips.end());  


  auto newEnd = 
    std::partition(v.begin(), 
                   v.end(), 
                   [&condition](const HypoJet::IJet* jp){
    
                   return condition.isSatisfied(jp);});

  return std::accumulate(v.begin(),
                         newEnd,
                         0.0,
                         [](double sum, const HypoJet::IJet* jp){
                           return sum + jp->et();}) > m_htMin;
}


std::string HTCondition::toString() const noexcept {
  std::stringstream ss;
  ss << "HTCondition: etaMin "
     <<  m_etaMin 
     << " etaMax " 
     << m_etaMax 
     << " htMin: " 
     << m_htMin
     <<'\n';

  return ss.str();
}


double HTCondition::orderingParameter() const noexcept {return m_htMin;}
