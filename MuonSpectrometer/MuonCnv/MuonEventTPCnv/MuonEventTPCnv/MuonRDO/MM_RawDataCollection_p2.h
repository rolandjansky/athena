/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_RAWDATACOLLECTION_P2_H
#define MM_RAWDATACOLLECTION_P2_H

#include <vector>
#include "MM_RawData_p2.h"

namespace Muon {
  class MM_RawDataCollection_p2
     : public std::vector< Muon::MM_RawData_p2>
  {
  public:
    // Default constructor
    MM_RawDataCollection_p2 () : m_idHash(0) {}
    unsigned int m_idHash; 
  };
}

#endif
