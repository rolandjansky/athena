/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>

#include "./PassThroughFilter.h"
#include "./ITrigJetHypoInfoCollector.h"


HypoJetVector
PassThroughFilter::filter (const HypoJetVector& jv,
			   const std::unique_ptr<ITrigJetHypoInfoCollector>& col) const{

  if (col) {
    col->collect("PassTroughFilter", "");
  }

  return jv;
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
