/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TGCRDOCONTAINERCNV_P2_H
#define MUON_TGCRDOCONTAINERCNV_P2_H

/*

Transient/Persistent converter for a container of collections of TGC,
Author: Zvi Tarem
        CERN, August, 2006

*/

#include "MuonRdoContainerTPCnv.h"
#include "MuonEventAthenaPool/TgcRdoContainer_p2.h"
#include "TgcRdoCnv_p2.h"
#include "MuonRDO/TgcRdoContainer.h"

typedef T_AthenaPoolTPCnvIDCont< TgcRdoContainer, TgcRdoContainer_p2, TgcRdoCnv_p2>  TgcRdoContainerCnv_p2;

#endif

