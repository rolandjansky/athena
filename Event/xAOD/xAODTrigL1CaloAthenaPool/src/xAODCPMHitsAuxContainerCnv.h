/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCPMHITSAUXCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCPMHITSAUXCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"
#include "xAODTrigL1Calo/CPMHitsAuxContainer.h"

typedef T_AthenaPoolAuxContainerCnv<xAOD::CPMHitsAuxContainer>
  xAODCPMHitsAuxContainerCnv;
  
#endif