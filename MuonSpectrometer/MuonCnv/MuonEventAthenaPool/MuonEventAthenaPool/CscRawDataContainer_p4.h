/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// NOTE: This is an exact copy of CscRawDataContainer_p3, since from p3 to p4 only the meaning of the m_hashId changed:
// Until p3, the m_hashId stores the identifier hash which encodes the geometrical position of the CSC
// From p4 onwards, the m_hashId stores the position of the CSC identifier in the vector of identifiers

#ifndef MUON_CSCCRAWDATACONTAINER_P4_H
#define MUON_CSCCRAWDATACONTAINER_P4_H

/*

Persistent represenation of a container of collections of CSC Raw Hits,
Author: Marcin Nowak
        CERN, December, 2005

*/

#include "MuonEventAthenaPool/CscRawDataCollection_p4.h"


class CscRawDataContainer_p4
   : public std::vector<CscRawDataCollection_p4>
{
public:
  CscRawDataContainer_p4() : std::vector<CscRawDataCollection_p4>()  {}

  friend class CscRawDataContainerCnv_p4;
};

#endif
