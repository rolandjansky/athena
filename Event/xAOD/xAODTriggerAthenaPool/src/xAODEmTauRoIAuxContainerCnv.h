/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file EmTauRoIAuxContainerCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef EmTauRoIAuxContainerCnv_H
#define EmTauRoIAuxContainerCnv_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"

// EDM include(s):
#include "xAODTrigger/EmTauRoIAuxContainer.h"
#include "xAODEmTauRoIAuxContainerCnv_v1.h"

/// Base class for the converter
typedef T_AthenaPoolAuxContainerCnv< xAOD::EmTauRoIAuxContainer,
                                     xAODEmTauRoIAuxContainerCnv_v1 >
   xAODEmTauRoIAuxContainerCnv;

#endif

