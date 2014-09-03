/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_CSCCRAWDATACONTAINERCNV_P2_H
#define MUON_CSCCRAWDATACONTAINERCNV_P2_H

/*

Transient/Persistent converter for a container of collections of CSC Raw Hits,
Author: Marcin Nowak
        CERN, January, 2006

*/
#include "MuonRDO/CscRawDataContainer.h"
#include "MuonEventAthenaPool/CscRawDataContainer_p2.h"
#include "CscRawDataCollectionCnv_p2.h"

class CscRawDataContainerCnv_p2
    : public T_AthenaPoolTPCnvIDCont< CscRawDataContainer, CscRawDataContainer_p2, CscRawDataCollectionCnv_p2>
{
public:
  CscRawDataContainerCnv_p2() {}

};


#endif




