/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCMMJETHITSAUXCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCMMJETHITSAUXCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"
#include "xAODTrigL1Calo/CMMJetHitsAuxContainer.h"

typedef T_AthenaPoolAuxContainerCnv<xAOD::CMMJetHitsAuxContainer>
  xAODCMMJetHitsAuxContainerCnv;
  
#endif 
