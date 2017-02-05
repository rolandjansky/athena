// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODMissingETContainerCnv.h 795699 2017-02-05 23:26:05Z khoo $
#ifndef XAODMISSINGETATHENAPOOL_XAODMISSINGETCONTAINERCNV_H
#define XAODMISSINGETATHENAPOOL_XAODMISSINGETCONTAINERCNV_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODMissingET/MissingET.h"
#include "xAODMissingET/MissingETContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::MissingETContainer,
                               xAOD::MissingETContainer >
   xAODMissingETContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::MissingETContainer class
 *
 *         Simple converter class making the xAOD::MissingETContainer
 *         class known to POOL.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 795699 $
 * $Date: 2017-02-06 00:26:05 +0100 (Mon, 06 Feb 2017) $
 */
class xAODMissingETContainerCnv : public xAODMissingETContainerCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODMissingETContainerCnv >;

public:
   /// Converter constructor
   xAODMissingETContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

   /// Function preparing the container to be written out
   virtual xAOD::MissingETContainer* createPersistent( xAOD::MissingETContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::MissingETContainer* createTransient();

private:
   // /// Function preparing a met object for persistence
   // void toPersistent( xAOD::MissingET* met ) const;

   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODMissingETContainerCnv

#endif // XAODMISSINGETATHENAPOOL_XAODMISSINGETCONTAINERCNV_H
