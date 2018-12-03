/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODL1TOPORAWDATACONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODL1TOPORAWDATACONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"
#include "xAODTrigL1Calo/L1TopoRawDataContainer.h"

typedef T_AthenaPoolxAODCnv<xAOD::L1TopoRawDataContainer>
  xAODL1TopoRawDataContainerCnv;
  
#endif   
