/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>

#include "./RangeFilter.h"
#include "./HypoJetPreds.h"  // HypoJetPtGreater


RangeFilter::RangeFilter(std::size_t begin, std::size_t end):
  m_begin(begin), m_end(end) {
  if (begin > end) {throw std::out_of_range("RangeFilter begin > end");}
  m_njets = end - begin;
}


std::pair<HypoJetCIter, HypoJetCIter>
RangeFilter::filter (const HypoJetCIter& begin,
			 const HypoJetCIter& end,
			 const std::unique_ptr<ITrigJetHypoInfoCollector>&) {

  m_filtered.clear();
  if (m_njets > (end - begin)) {// too few jets
    return std::make_pair(m_filtered.begin(), m_filtered.end());
  }

  m_filtered.assign(begin, end);

  std::partial_sort(m_filtered.begin(),
		    m_filtered.begin() + m_end,
		    m_filtered.end(),
		    HypoJetPtGreater());
  
  return std::make_pair(m_filtered.begin() + m_begin,
			m_filtered.begin() + m_end);
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
