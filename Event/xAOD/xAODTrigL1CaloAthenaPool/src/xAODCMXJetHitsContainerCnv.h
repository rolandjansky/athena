/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCMXJETHITSCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCMXJETHITSCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"
#include "xAODTrigL1Calo/CMXJetHitsContainer.h"

typedef T_AthenaPoolxAODCnv<xAOD::CMXJetHitsContainer>
  xAODCMXJetHitsContainerCnv;
  
#endif   
