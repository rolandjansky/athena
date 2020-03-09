/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODMUONATHENAPOOL_XAODMUONAUXCONTAINERCNV_H
#define XAODMUONATHENAPOOL_XAODMUONAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"

#include "xAODMuon/MuonAuxContainer.h"
#include "xAODMuonAuxContainerCnv_v1.h"
#include "xAODMuonAuxContainerCnv_v2.h"
#include "xAODMuonAuxContainerCnv_v3.h"
#include "xAODMuonAuxContainerCnv_v4.h"


typedef T_AthenaPoolAuxContainerCnv< xAOD::MuonAuxContainer,
                                     xAODMuonAuxContainerCnv_v4,
                                     xAODMuonAuxContainerCnv_v3,
                                     xAODMuonAuxContainerCnv_v2,
                                     xAODMuonAuxContainerCnv_v1>
   xAODMuonAuxContainerCnv;


#endif // XAODMUONATHENAPOOL_XAODMUONAUXCONTAINERCNV_H
