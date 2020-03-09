/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODRODHEADERAUXCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODRODHEADERAUXCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"
#include "xAODTrigL1Calo/RODHeaderAuxContainer.h"
#include "xAODRODHeaderAuxContainerCnv_v1.h"

typedef T_AthenaPoolAuxContainerCnv<xAOD::RODHeaderAuxContainer,
                                    xAODRODHeaderAuxContainerCnv_v1>
  xAODRODHeaderAuxContainerCnv;
  
#endif
