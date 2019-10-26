// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigRingerRingsAuxContainerCnv.h 707600 2015-11-12 19:55:33Z krasznaa $
#ifndef XAODTRIGRINGERATHENAPOOL_XAODTRIGRINGERRINGSAUXCONTAINERCNV_H
#define XAODTRIGRINGERATHENAPOOL_XAODTRIGRINGERRINGSAUXCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolAuxContainerCnv.h"

// EDM include(s):
#include "xAODTrigRinger/TrigRingerRingsAuxContainer.h"
#include "xAODTrigRingerRingsAuxContainerCnv_v1.h"

/// Base class for the converter
typedef T_AthenaPoolAuxContainerCnv< xAOD::TrigRingerRingsAuxContainer,
                                     xAODTrigRingerRingsAuxContainerCnv_v1 >
   xAODTrigRingerRingsAuxContainerCnv;


#endif // XAODTRIGRINGERATHENAPOOL_XAODTRIGRINGERRINGSAUXCONTAINERCNV_H
