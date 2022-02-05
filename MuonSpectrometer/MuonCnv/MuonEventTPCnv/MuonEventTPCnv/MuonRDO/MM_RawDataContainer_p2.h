/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_RAWDATACONTAINER_P2_H
#define MM_RAWDATACONTAINER_P2_H

#include <vector>
#include "MM_RawDataCollection_p2.h"

namespace Muon {
  class MM_RawDataContainer_p2
     : public std::vector< Muon::MM_RawDataCollection_p2>
  {
  public:
    // Default constructor
    MM_RawDataContainer_p2 () {}
  };
}

#endif
