// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODRingerRingsContainerCnv.h
#ifndef XAODRINGERSATHENAPOOL_XAODTRIGRINGERRINGSCONTAINERCNV_H
#define XAODRINGERSATHENAPOOL_XAODTRIGRINGERRINGSCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"

// EDM include(s):
#include "xAODTrigRinger/TrigRingerRingsContainer.h"
#include "xAODTrigRingerRingsContainerCnv_v1.h"

/// Type definition for the converter's base
typedef T_AthenaPoolxAODCnv< xAOD::TrigRingerRingsContainer,
                             xAODTrigRingerRingsContainerCnv_v1 >
  xAODTrigRingerRingsContainerCnv;


#endif // XAODRINGERSATHENAPOOL_XAODTRIGRINGERRINGSCONTAINERCNV_H
