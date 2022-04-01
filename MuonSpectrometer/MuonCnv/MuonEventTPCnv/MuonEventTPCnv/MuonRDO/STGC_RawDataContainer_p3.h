/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGC_RAWDATACONTAINER_P3_H
#define STGC_RAWDATACONTAINER_P3_H

#include <vector>
#include "STGC_RawDataCollection_p3.h"

namespace Muon {
  class STGC_RawDataContainer_p3
     : public std::vector< Muon::STGC_RawDataCollection_p3>
  {
  public:
    // Default constructor
    STGC_RawDataContainer_p3 () {}
  };
}

#endif
