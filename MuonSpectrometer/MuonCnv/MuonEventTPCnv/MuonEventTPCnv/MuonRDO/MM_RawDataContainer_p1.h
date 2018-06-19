/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_RAWDATACONTAINER_P1_H
#define MM_RAWDATACONTAINER_P1_H

#include <vector>
#include "MM_RawDataCollection_p1.h"

namespace Muon {
  class MM_RawDataContainer_p1
     : public std::vector< Muon::MM_RawDataCollection_p1>
  {
  public:
    // Default constructor
    MM_RawDataContainer_p1 () {}
  };
}

#endif
