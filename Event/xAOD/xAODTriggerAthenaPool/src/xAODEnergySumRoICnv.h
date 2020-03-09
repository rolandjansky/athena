/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file EnergySumRoICnv.h
 */

#ifndef EnergySumRoICnv_H
#define EnergySumRoICnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"
#include "xAODTrigger/EnergySumRoI.h"
#include "xAODEnergySumRoICnv_v1.h"

typedef T_AthenaPoolxAODCnv< xAOD::EnergySumRoI,
                             xAODEnergySumRoICnv_v1 >
   xAODEnergySumRoICnv;


#endif

