/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>

#include "./RangeFilter.h"
#include "./HypoJetPreds.h"  // HypoJetPtGreater


RangeFilter::RangeFilter(std::size_t begin, std::size_t end):
  m_begin(begin), m_end(end) {
  if (begin > end) {throw std::out_of_range("RangeFilter begin > end");}
  m_nToSort = end;
}


HypoJetVector
RangeFilter::filter(const HypoJetVector& jv,
		    const std::unique_ptr<ITrigJetHypoInfoCollector>&) const {

  if (m_nToSort > jv.size()) {// too few jets
    return HypoJetVector();
  }

  auto filtered = HypoJetVector(jv.cbegin(), jv.cend());

  std::partial_sort(filtered.begin(),
		    filtered.begin() + m_end,
		    filtered.end(),
		    HypoJetPtGreater());

  return HypoJetVector(filtered.begin() + m_begin,
		       filtered.begin() + m_end);
}

std::string RangeFilter::toString() const {
  std::stringstream ss;
  const void* address = static_cast<const void*>(this);
  ss << "RangeFilter: (" << address << ") "
     << "begin "<< m_begin << " end " << m_end << '\n';
  return ss.str();
}


std::ostream& operator<<(std::ostream& os, const RangeFilter& cf){
  os << cf.toString();
  return os;
}
