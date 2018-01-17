/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGC_RAWDATACONTAINER_P1_H
#define STGC_RAWDATACONTAINER_P1_H

#include <vector>
#include "STGC_RawDataCollection_p1.h"

namespace Muon {
  class STGC_RawDataContainer_p1
     : public std::vector< Muon::STGC_RawDataCollection_p1>
  {
  public:
    // Default constructor
    STGC_RawDataContainer_p1 () {}
  };
}

#endif
