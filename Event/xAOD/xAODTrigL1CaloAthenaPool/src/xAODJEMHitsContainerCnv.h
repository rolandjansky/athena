/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODJEMHITSCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODJEMHITSCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"
#include "xAODTrigL1Calo/JEMHitsContainer.h"

typedef T_AthenaPoolxAODCnv<xAOD::JEMHitsContainer>
  xAODJEMHitsContainerCnv;
  
#endif   
