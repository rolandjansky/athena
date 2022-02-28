/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGC_RAWDATACOLLECTION_P2_H
#define STGC_RAWDATACOLLECTION_P2_H

#include <vector>
#include "STGC_RawData_p2.h"

namespace Muon {
  class STGC_RawDataCollection_p2
     : public std::vector< Muon::STGC_RawData_p2>
  {
  public:
    // Default constructor
    STGC_RawDataCollection_p2 () : m_idHash(0) {}
    unsigned int m_idHash; 
  };
}

#endif
