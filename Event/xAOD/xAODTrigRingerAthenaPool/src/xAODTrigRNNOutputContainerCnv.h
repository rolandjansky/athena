// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigRNNOutputContainerCnv.h 707590 2015-11-12 19:09:03Z krasznaa $
#ifndef XAODTRIGRINGERATHENAPOOL_XAODTRIGRNNOUTPUTCONTAINERCNV_H
#define XAODTRIGRINGERATHENAPOOL_XAODTRIGRNNOUTPUTCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"

// EDM include(s):
#include "xAODTrigRinger/TrigRNNOutputContainer.h"
#include "xAODTrigRNNOutputContainerCnv_v1.h"

/// Type definition for the converter's base
typedef T_AthenaPoolxAODCnv< xAOD::TrigRNNOutputContainer,
                             xAODTrigRNNOutputContainerCnv_v1 >
   xAODTrigRNNOutputContainerCnv;


#endif // XAODTRIGRINGERATHENAPOOL_XAODTRIGRNNOUTPUTCONTAINERCNV_H
