/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_CSCCRAWDATACONTAINERCNV_P3_H
#define MUON_CSCCRAWDATACONTAINERCNV_P3_H


#include "MuonRDO/CscRawDataContainer.h"
#include "MuonEventAthenaPool/CscRawDataContainer_p3.h"
#include "CscRawDataCollectionCnv_p3.h"

/**
Transient/Persistent converter for a container of collections of CSC Raw Hits,
*/
class CscRawDataContainerCnv_p3
    : public T_AthenaPoolTPCnvIDCont< CscRawDataContainer, CscRawDataContainer_p3, CscRawDataCollectionCnv_p3>
{
public:
  CscRawDataContainerCnv_p3() {}

};


#endif




