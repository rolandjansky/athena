/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_CSCCRAWDATACONTAINERCNV_P1_H
#define MUON_CSCCRAWDATACONTAINERCNV_P1_H

/*

Transient/Persistent converter for a container of collections of CSC Raw Hits,
Author: Marcin Nowak
        CERN, January, 2006

*/
#include "MuonRDO/CscRawDataContainer.h"
#include "MuonEventAthenaPool/CscRawDataContainer_p1.h"
#include "CscRawDataCollectionCnv_p1.h"

class CscRawDataContainerCnv_p1
    : public T_AthenaPoolTPCnvIDCont< CscRawDataContainer, CscRawDataContainer_p1, CscRawDataCollectionCnv_p1>
{
public:
  CscRawDataContainerCnv_p1() {}

};


#endif




