/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*#ifndef XAODTRIGL1CALOATHENAPOOL_XAODTRIGGERTOWERCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODTRIGGERTOWERCONTAINERCNV_H*/

#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"

typedef T_AthenaPoolxAODCnv<xAOD::TriggerTowerContainer>
  xAODTriggerTowerContainerCnv;
  
/*#endif   */