/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGC_RAWDATACONTAINER_P2_H
#define STGC_RAWDATACONTAINER_P2_H

#include <vector>
#include "STGC_RawDataCollection_p2.h"

namespace Muon {
  class STGC_RawDataContainer_p2
     : public std::vector< Muon::STGC_RawDataCollection_p2>
  {
  public:
    // Default constructor
    STGC_RawDataContainer_p2 () {}
  };
}

#endif
