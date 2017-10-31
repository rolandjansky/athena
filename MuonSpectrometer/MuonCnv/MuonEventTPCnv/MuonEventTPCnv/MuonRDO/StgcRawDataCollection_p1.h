/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGCRAWDATACOLLECTION_P1_H
#define STGCRAWDATACOLLECTION_P1_H

#include <vector>
#include "StgcRawData_p1.h"

namespace Muon {
  class StgcRawDataCollection_p1
     : public std::vector< Muon::StgcRawData_p1>
  {
  public:
    // Default constructor
    StgcRawDataCollection_p1 () : m_idHash(0) {}
  private:
    unsigned int m_idHash; 
  };
}

#endif
