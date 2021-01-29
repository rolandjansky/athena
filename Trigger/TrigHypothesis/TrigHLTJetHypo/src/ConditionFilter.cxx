/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>

#include "./ConditionFilter.h"

//ConditionFilter::ConditionFilter(ConditionPtrs& conditions):
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

HypoJetVector
ConditionFilter::filter (const HypoJetCIter& begin,
			 const HypoJetCIter& end,
			 const std::unique_ptr<ITrigJetHypoInfoCollector>& collector) const {

  HypoJetVector in (begin, end);
  auto new_end = in.end();
  
  for (const auto& cptr : m_conditions) {
    new_end = std::partition(in.begin(),
			 new_end,
			 FilterPred(cptr, collector));
  }
  
  return HypoJetVector(in.begin(), new_end);
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

HypoJetVector
ConditionFilter::filter (const HypoJetVector& jv,
			 const std::unique_ptr<ITrigJetHypoInfoCollector>& col) const {
  return filter(jv.cbegin(), jv.cend(), col);
}

std::ostream& operator<<(std::ostream& os, const ConditionFilter& cf){
  os << cf.toString();
  return os;
}
