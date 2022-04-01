/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGC_RAWDATACOLLECTION_P3_H
#define STGC_RAWDATACOLLECTION_P3_H

#include <vector>
#include "STGC_RawData_p3.h"

namespace Muon {
  class STGC_RawDataCollection_p3
     : public std::vector< Muon::STGC_RawData_p3>
  {
  public:
    // Default constructor
    STGC_RawDataCollection_p3 () : m_idHash(0) {}
    unsigned int m_idHash; 
  };
}

#endif
