/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCMXETSUMSAUXCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCMXETSUMSAUXCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"
#include "xAODTrigL1Calo/CMXEtSumsAuxContainer.h"

typedef T_AthenaPoolAuxContainerCnv<xAOD::CMXEtSumsAuxContainer>
  xAODCMXEtSumsAuxContainerCnv;
  
#endif 
