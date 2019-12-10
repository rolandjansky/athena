// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODMissingETContainerCnv.h 795699 2017-02-05 23:26:05Z khoo $
#ifndef XAODMISSINGETATHENAPOOL_XAODMISSINGETCONTAINERCNV_H
#define XAODMISSINGETATHENAPOOL_XAODMISSINGETCONTAINERCNV_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolxAODCnv.h"

// EDM include(s):
#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolxAODCnv< xAOD::MissingETContainer >
   xAODMissingETContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::MissingETContainer class
 *
 * We need to extend the default version to handle maintaining the name hashing
 * in the interface class.
 */
class xAODMissingETContainerCnv : public xAODMissingETContainerCnvBase
{
public:
   using xAODMissingETContainerCnvBase::xAODMissingETContainerCnvBase;

   /// Function reading in the persistent object
   virtual xAOD::MissingETContainer* createTransientWithKey (const std::string& key) override;
}; // class xAODMissingETContainerCnv


#endif // XAODMISSINGETATHENAPOOL_XAODMISSINGETCONTAINERCNV_H
