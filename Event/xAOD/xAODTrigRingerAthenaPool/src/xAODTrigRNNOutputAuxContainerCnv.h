// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigRNNOutputAuxContainerCnv.h 707590 2015-11-12 19:09:03Z krasznaa $
#ifndef XAODTRIGRINGERATHENAPOOL_XAODRNNOUTPUTAUXCONTAINERCNV_H
#define XAODTRIGRINGERATHENAPOOL_XAODRNNOUTPUTAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"

// EDM include(s):
#include "xAODTrigRinger/TrigRNNOutputAuxContainer.h"
#include "xAODTrigRNNOutputAuxContainerCnv_v1.h"

/// Base class for the converter
typedef T_AthenaPoolAuxContainerCnv< xAOD::TrigRNNOutputAuxContainer,
                                     xAODTrigRNNOutputAuxContainerCnv_v1 >
   xAODTrigRNNOutputAuxContainerCnv;


#endif // XAODTRIGRINGERATHENAPOOL_XAODRNNOUTPUTAUXCONTAINERCNV_H
