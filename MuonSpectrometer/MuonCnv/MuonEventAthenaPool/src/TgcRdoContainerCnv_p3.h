/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TGCRDOCONTAINERCNV_P3_H
#define MUON_TGCRDOCONTAINERCNV_P3_H

/*

Transient/Persistent converter for a container of collections of TGC,
Author: Zvi Tarem
        CERN, August, 2006

*/

#include "MuonRdoContainerTPCnv.h"
#include "MuonEventAthenaPool/TgcRdoContainer_p3.h"
#include "TgcRdoCnv_p3.h"
#include "MuonRDO/TgcRdoContainer.h"

typedef T_AthenaPoolTPCnvIDCont< TgcRdoContainer, TgcRdoContainer_p3, TgcRdoCnv_p3>  TgcRdoContainerCnv_p3;

#endif

