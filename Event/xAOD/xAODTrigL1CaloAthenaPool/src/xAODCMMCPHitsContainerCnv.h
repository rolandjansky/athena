/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRIGL1CALOATHENAPOOL_XAODCMMCPHITSCONTAINERCNV_H
#define XAODTRIGL1CALOATHENAPOOL_XAODCMMCPHITSCONTAINERCNV_H


#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"
#include "xAODTrigL1Calo/CMMCPHitsContainer.h"

typedef T_AthenaPoolxAODCnv<xAOD::CMMCPHitsContainer>
  xAODCMMCPHitsContainerCnv;

#endif 

