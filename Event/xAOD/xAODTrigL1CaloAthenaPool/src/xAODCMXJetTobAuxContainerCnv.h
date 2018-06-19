/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCMXJETTOBAUXCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCMXJETTOBAUXCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"
#include "xAODTrigL1Calo/CMXJetTobAuxContainer.h"

typedef T_AthenaPoolAuxContainerCnv<xAOD::CMXJetTobAuxContainer>
  xAODCMXJetTobAuxContainerCnv;
  
#endif 
