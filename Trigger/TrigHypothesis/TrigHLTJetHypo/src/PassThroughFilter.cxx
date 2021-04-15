/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>

#include "./PassThroughFilter.h"
#include "./ITrigJetHypoInfoCollector.h"


std::pair<HypoJetCIter, HypoJetCIter>
PassThroughFilter::filter (const HypoJetCIter& begin,
			   const HypoJetCIter& end,
			   const std::unique_ptr<ITrigJetHypoInfoCollector>& col) {

  if (col) {
    col->collect("PassTroughFilter", "");
  }
  
  return std::make_pair(begin, end);
}

std::string PassThroughFilter::toString() const {
  std::stringstream ss;
  const void* address = static_cast<const void*>(this);
  ss << "PassThroughFilter: (" << address << ")\n";
  return ss.str();
}


std::ostream& operator<<(std::ostream& os, const PassThroughFilter& cf){
  os << cf.toString();
  return os;
}
