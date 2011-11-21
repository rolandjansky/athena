/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_RAWDATACONTAINER_P1_H
#define LUCID_RAWDATACONTAINER_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"
#include "LUCID_RawData_p1.h"
#include <vector>
#include <string>

class LUCID_RawDataContainer_p1: public std::vector<LUCID_RawData_p1> {
  
 public:
  
  LUCID_RawDataContainer_p1() {};
};

#endif
