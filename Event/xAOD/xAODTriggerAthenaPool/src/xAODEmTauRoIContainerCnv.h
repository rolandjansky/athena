/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file EmTauRoIContainerCnv.h
 */

#ifndef EmTauRoIContainerCnv_H
#define EmTauRoIContainerCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "xAODEmTauRoIContainerCnv_v1.h"

typedef T_AthenaPoolxAODCnv< xAOD::EmTauRoIContainer,
                             xAODEmTauRoIContainerCnv_v1 >
   xAODEmTauRoIContainerCnv;


#endif

