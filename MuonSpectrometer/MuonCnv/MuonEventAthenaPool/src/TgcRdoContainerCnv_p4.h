/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_TGCRDOCONTAINERCNV_P4_H
#define MUON_TGCRDOCONTAINERCNV_P4_H

#include "MuonRdoContainerTPCnv.h"
#include "MuonEventAthenaPool/TgcRdoContainer_p4.h"
#include "TgcRdoCnv_p4.h"
#include "MuonRDO/TgcRdoContainer.h"

typedef T_AthenaPoolTPCnvIDCont< TgcRdoContainer, TgcRdoContainer_p4, TgcRdoCnv_p4>  TgcRdoContainerCnv_p4;

#endif

