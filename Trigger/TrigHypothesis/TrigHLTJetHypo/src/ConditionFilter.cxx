/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>

#include "./ConditionFilter.h"


ConditionFilter::ConditionFilter(ConditionsMT& conditions):
  m_conditions(std::move(conditions)) {
}


struct FilterPred{
  
  FilterPred(const ConditionMT& cptr,
	     const std::unique_ptr<ITrigJetHypoInfoCollector>& collector):
    m_cptr(cptr), m_collector(collector) {
  }

  bool operator() (pHypoJet pjet) {
    auto hjv = HypoJetVector{pjet};
    return m_cptr->isSatisfied(hjv, m_collector);
  }
  
  const ConditionMT& m_cptr;
  const std::unique_ptr<ITrigJetHypoInfoCollector>& m_collector;
};

std::pair<HypoJetCIter, HypoJetCIter>
ConditionFilter::filter (const HypoJetCIter& begin,
			 const HypoJetCIter& end,
			 const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) {
  
  if (m_conditions.empty()) {
    return std::make_pair(begin, end);
  }
  
  m_filtered = HypoJetVector(begin, end);
  auto filtered_begin = m_filtered.begin();
  auto filtered_end = m_filtered.end();
  
  for (const auto& cptr : m_conditions) {
    filtered_end = std::partition(filtered_begin,
				  filtered_end,
				  FilterPred(cptr, collector));
  }
  
  return std::make_pair(filtered_begin, filtered_end);
}

std::string ConditionFilter::toString() const {
  std::stringstream ss;
  const void* address = static_cast<const void*>(this);
  ss << "ConditionFilter: (" << address << ")\n"
     << "Conditions [" << m_conditions.size() << "]:\n";
  for (const auto& c : m_conditions) {
     ss << " " << c->toString() << "\n\n";
   }

  return ss.str();
}


std::ostream& operator<<(std::ostream& os, const ConditionFilter& cf){
  os << cf.toString();
  return os;
}
