/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODL1TOPORAWDATAAUXCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODL1TOPORAWDATAAUXCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"
#include "xAODTrigL1Calo/L1TopoRawDataAuxContainer.h"

typedef T_AthenaPoolAuxContainerCnv<xAOD::L1TopoRawDataAuxContainer>
  xAODL1TopoRawDataAuxContainerCnv;
  
#endif 
