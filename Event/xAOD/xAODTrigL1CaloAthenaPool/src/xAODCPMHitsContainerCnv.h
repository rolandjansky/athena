/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCPMHITSCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCPMHITSCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"
#include "xAODTrigL1Calo/CPMHitsContainer.h"

typedef T_AthenaPoolxAODCnv<xAOD::CPMHitsContainer>
  xAODCPMHitsContainerCnv;
  
#endif   
