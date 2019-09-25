/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODRODHEADERCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODRODHEADERCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"
#include "xAODTrigL1Calo/RODHeaderContainer.h"
#include "xAODRODHeaderContainerCnv_v1.h"

typedef T_AthenaPoolxAODCnv<xAOD::RODHeaderContainer,
                            xAODRODHeaderContainerCnv_v1>
  xAODRODHeaderContainerCnv;
  
#endif   
