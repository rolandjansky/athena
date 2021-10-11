/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TGCL1RDOCONTAINERCNV_P1_H
#define MUON_TGCL1RDOCONTAINERCNV_P1_H

#include "MuonRdoContainerTPCnv.h"
#include "MuonEventAthenaPool/TgcL1RdoContainer_p1.h"
#include "TgcL1RdoCnv_p1.h"
#include "MuonRDO/TgcL1RdoContainer.h"

typedef T_AthenaPoolTPCnvIDCont< TgcL1RdoContainer, TgcL1RdoContainer_p1, TgcL1RdoCnv_p1>  TgcL1RdoContainerCnv_p1;

#endif

