/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODJEMETSUMSCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODJEMETSUMSCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"
#include "xAODTrigL1Calo/JEMEtSumsContainer.h"

typedef T_AthenaPoolxAODCnv<xAOD::JEMEtSumsContainer>
  xAODJEMEtSumsContainerCnv;
  
#endif   