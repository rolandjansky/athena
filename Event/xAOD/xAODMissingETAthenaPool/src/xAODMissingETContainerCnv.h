// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODMissingETContainerCnv.h 795699 2017-02-05 23:26:05Z khoo $
#ifndef XAODMISSINGETATHENAPOOL_XAODMISSINGETCONTAINERCNV_H
#define XAODMISSINGETATHENAPOOL_XAODMISSINGETCONTAINERCNV_H

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"

// EDM include(s):
#include "xAODMissingET/MissingETContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolxAODCnv< xAOD::MissingETContainer >
   xAODMissingETContainerCnv;

#endif // XAODMISSINGETATHENAPOOL_XAODMISSINGETCONTAINERCNV_H
