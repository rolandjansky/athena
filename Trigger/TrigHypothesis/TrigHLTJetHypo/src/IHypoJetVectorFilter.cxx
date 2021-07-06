/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "./IHypoJetVectorFilter.h"

std::ostream& operator<<(std::ostream& os, const IHypoJetVectorFilter& filter) {
  os << filter.toString();
  return os;
}

