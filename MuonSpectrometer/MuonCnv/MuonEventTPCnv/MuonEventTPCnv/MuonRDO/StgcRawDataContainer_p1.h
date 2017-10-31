/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGCRAWDATACONTAINER_P1_H
#define STGCRAWDATACONTAINER_P1_H

#include <vector>
#include "StgcRawDataCollection_p1.h"

namespace Muon {
  class StgcRawDataContainer_p1
     : public std::vector< Muon::StgcRawDataCollection_p1>
  {
  public:
    // Default constructor
    StgcRawDataContainer_p1 () {}
  };
}

#endif
