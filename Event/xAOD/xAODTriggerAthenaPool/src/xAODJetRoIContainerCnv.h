/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file JetRoIContainerCnv.h
 */

#ifndef JetRoIContainerCnv_H
#define JetRoIContainerCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"
#include "xAODTrigger/JetRoIContainer.h"
#include "xAODJetRoIContainerCnv_v1.h"

typedef T_AthenaPoolxAODCnv< xAOD::JetRoIContainer,
                             xAODJetRoIContainerCnv_v1 >
   xAODJetRoIContainerCnv;


#endif

