/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_STGC_RAWDATACONTAINERCNV_P1_H
#define MUON_STGC_RAWDATACONTAINERCNV_P1_H

/*

Transient/Persistent converter for a container of collections of TGC,
Author: Zvi Tarem
        CERN, August, 2006

*/

#include "MuonRdoContainerTPCnv.h"
#include "MuonEventAthenaPool/sTGC_RawDataCollectionCnv_p1.h"
#include "TgcRdoCnv_p1.h"
#include "MuonRDO/sTGC_RawDataContainer.h"

typedef T_AthenaPoolTPCnvIDCont< sTGC_RawDataContainer, sTGC_RawDataContainer_p3, sTGC_RawDataCollectionCnv_p1>  sTGC_RawDataContainerCnv_p1;

#endif

