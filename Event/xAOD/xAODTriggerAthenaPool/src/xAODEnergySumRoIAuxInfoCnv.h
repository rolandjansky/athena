/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file EnergySumRoIAuxInfoCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef EnergySumRoIAuxInfoCnv_H
#define EnergySumRoIAuxInfoCnv_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"

// EDM include(s):
#include "xAODTrigger/EnergySumRoIAuxInfo.h"
#include "xAODEnergySumRoIAuxInfoCnv_v1.h"

/// Base class for the converter
typedef T_AthenaPoolAuxContainerCnv< xAOD::EnergySumRoIAuxInfo,
                                     xAODEnergySumRoIAuxInfoCnv_v1 >
   xAODEnergySumRoIAuxInfoCnv;


#endif

