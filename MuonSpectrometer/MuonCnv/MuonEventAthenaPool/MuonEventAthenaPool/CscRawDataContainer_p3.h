/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_CSCCRAWDATACONTAINER_P3_H
#define MUON_CSCCRAWDATACONTAINER_P3_H

/*

Persistent represenation of a container of collections of CSC Raw Hits,
Author: Marcin Nowak
        CERN, December, 2005

*/

#include "MuonEventAthenaPool/CscRawDataCollection_p3.h"


class CscRawDataContainer_p3
   : public std::vector<CscRawDataCollection_p3>
{
public:
  // Default constructor 
  CscRawDataContainer_p3() : std::vector<CscRawDataCollection_p3>()  {}

  friend class CscRawDataContainerCnv_p3;
};


#endif




