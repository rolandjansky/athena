/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCMMETSUMSCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCMMETSUMSCONTAINERCNV_H


#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"
#include "xAODTrigL1Calo/CMMEtSumsContainer.h"

typedef T_AthenaPoolxAODCnv<xAOD::CMMEtSumsContainer>
  xAODCMMEtSumsContainerCnv;

#endif 
