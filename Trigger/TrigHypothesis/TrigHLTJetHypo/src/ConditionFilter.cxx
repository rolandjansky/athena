/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>

#include "./ConditionFilter.h"

ConditionFilter::ConditionFilter(ConditionPtrs&& conditions):
  m_conditions(std::move(conditions)) {
}


struct FilterPred{
  
  FilterPred(const ConditionPtr& cptr,
	     const std::unique_ptr<ITrigJetHypoInfoCollector>& collector):
    m_cptr(cptr), m_collector(collector) {
  }

  bool operator() (pHypoJet pjet) {
    auto hjv = HypoJetVector{pjet};
    return m_cptr->isSatisfied(hjv, m_collector);
  }
  
  const ConditionPtr& m_cptr;
  const std::unique_ptr<ITrigJetHypoInfoCollector>& m_collector;
};

HypoJetVector
ConditionFilter::filter (const HypoJetVector& jv,
			 const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const {
  
  
  if (m_conditions.empty()) {
    return jv;
  }
  
  auto filtered = HypoJetVector(jv.cbegin(), jv.cend());
  auto filtered_begin = filtered.begin();
  auto filtered_end = filtered.end();
  
  for (const auto& cptr : m_conditions) {
    filtered_end = std::partition(filtered_begin,
				  filtered_end,
				  FilterPred(cptr, collector));
  }

  filtered.resize(filtered_end - filtered_begin);

  return filtered;
}

std::string ConditionFilter::toString() const {
  std::stringstream ss;
  const void* address = static_cast<const void*>(this);
  ss << "ConditionFilter: (" << address << ")\n"
     << "Conditions [" << m_conditions.size() << "]:\n";
  for (const auto& c : m_conditions) {
    if (c) {
      ss << " " << c->toString() << "\n\n";
    }
   }

  return ss.str();
}


std::ostream& operator<<(std::ostream& os, const ConditionFilter& cf){
  os << cf.toString();
  return os;
}
