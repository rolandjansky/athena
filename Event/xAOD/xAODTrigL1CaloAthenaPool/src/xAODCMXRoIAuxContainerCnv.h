/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCMXROIAUXCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCMXROIAUXCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"
#include "xAODTrigL1Calo/CMXRoIAuxContainer.h"

typedef T_AthenaPoolAuxContainerCnv<xAOD::CMXRoIAuxContainer>
  xAODCMXRoIAuxContainerCnv;
  
#endif 
