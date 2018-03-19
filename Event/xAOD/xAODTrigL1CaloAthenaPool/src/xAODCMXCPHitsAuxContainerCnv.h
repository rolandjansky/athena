/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCMXCPHITSAUXCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCMXCPHITSAUXCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"
#include "xAODTrigL1Calo/CMXCPHitsAuxContainer.h"

typedef T_AthenaPoolAuxContainerCnv<xAOD::CMXCPHitsAuxContainer>
  xAODCMXCPHitsAuxContainerCnv;
  
#endif 
