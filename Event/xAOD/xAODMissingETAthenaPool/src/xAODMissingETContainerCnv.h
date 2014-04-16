// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODMissingETContainerCnv.h 586838 2014-03-08 15:10:34Z khoo $
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
 * $Revision: 586838 $
 * $Date: 2014-03-08 16:10:34 +0100 (Sat, 08 Mar 2014) $
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
   /// Function preparing a met object for persistence
   void toPersistent( xAOD::MissingET* met ) const;

   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODMissingETContainerCnv

#endif // XAODMISSINGETATHENAPOOL_XAODMISSINGETCONTAINERCNV_H
