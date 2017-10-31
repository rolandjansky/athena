/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGC_RAWDATACOLLECTION_P1_H
#define STGC_RAWDATACOLLECTION_P1_H

#include <vector>
#include "STGC_RawData_p1.h"

namespace Muon {
  class STGC_RawDataCollection_p1
     : public std::vector< Muon::STGC_RawData_p1>
  {
  public:
    // Default constructor
    STGC_RawDataCollection_p1 () : m_idHash(0) {}
  private:
    unsigned int m_idHash; 
  };
}

#endif
