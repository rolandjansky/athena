/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCPMROICONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCPMROICONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"
#include "xAODTrigL1Calo/CPMRoIContainer.h"

typedef T_AthenaPoolxAODCnv<xAOD::CPMRoIContainer>
  xAODCPMRoIContainerCnv;
  
#endif   