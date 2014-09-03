/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_CSCCRAWDATACONTAINER_P2_H
#define MUON_CSCCRAWDATACONTAINER_P2_H

/*

Persistent represenation of a container of collections of CSC Raw Hits,
Author: Marcin Nowak
        CERN, December, 2005

*/

#include "MuonEventAthenaPool/CscRawDataCollection_p2.h"


class CscRawDataContainer_p2
   : public std::vector<CscRawDataCollection_p2>
{
public:
  // Default constructor 
  CscRawDataContainer_p2() : std::vector<CscRawDataCollection_p2>()  {}

  friend class CscRawDataContainerCnv_p2;
};


#endif




