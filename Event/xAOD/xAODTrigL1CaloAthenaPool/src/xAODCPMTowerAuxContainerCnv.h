/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODTRIGGERTOWERAUXCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODTRIGGERTOWERAUXCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"
#include "xAODTrigL1Calo/CPMTowerAuxContainer.h"

typedef T_AthenaPoolAuxContainerCnv<xAOD::CPMTowerAuxContainer>
  xAODCPMTowerAuxContainerCnv;
  
#endif 
