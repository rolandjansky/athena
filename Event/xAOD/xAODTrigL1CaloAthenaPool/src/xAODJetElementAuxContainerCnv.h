/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODJETELEMENTAUXCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODJETELEMENTAUXCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"
#include "xAODTrigL1Calo/JetElementAuxContainer.h"

typedef T_AthenaPoolAuxContainerCnv<xAOD::JetElementAuxContainer>
  xAODJetElementAuxContainerCnv;
  
#endif 
