/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * @file JetRoIAuxContainerCnv.h
 * @brief Generated header file which defines a typedef for templated converter class
 * @author RD Schaffer <R.D. Schaffer@cern.ch>
 */

#ifndef JetRoIAuxContainerCnv_H
#define JetRoIAuxContainerCnv_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"

// EDM include(s):
#include "xAODTrigger/JetRoIAuxContainer.h"
#include "xAODJetRoIAuxContainerCnv_v1.h"

/// Base class for the converter
typedef T_AthenaPoolAuxContainerCnv< xAOD::JetRoIAuxContainer,
                                     xAODJetRoIAuxContainerCnv_v1 >
   xAODJetRoIAuxContainerCnv;


#endif

