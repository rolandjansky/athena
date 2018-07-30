/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_RAWDATACONTAINER_H
#define LUCID_RAWDATACONTAINER_H

#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "LUCID_RawEvent/LUCID_RawData.h"

class LUCID_RawDataContainer : public DataVector<LUCID_RawData> {
  
 public:
  
  LUCID_RawDataContainer() {}
  virtual ~LUCID_RawDataContainer() {}
};

#ifndef __CINT__
CLASS_DEF(LUCID_RawDataContainer, 2592, 1)
#endif

#endif // LUCID_RAWDATACONTAINER_H
