/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCMXCPHITSCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCMXCPHITSCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"
#include "xAODTrigL1Calo/CMXCPHitsContainer.h"

typedef T_AthenaPoolxAODCnv<xAOD::CMXCPHitsContainer>
  xAODCMXCPHitsContainerCnv;
  
#endif   
