// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODMissingETComponentMapCnv.h 586838 2014-03-08 15:10:34Z khoo $
#ifndef XAODMISSINGETATHENAPOOL_XAODMISSINGETCOMPONENTMAPCNV_H
#define XAODMISSINGETATHENAPOOL_XAODMISSINGETCOMPONENTMAPCNV_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODMissingET/MissingETComponent.h"
#include "xAODMissingET/MissingETComponentMap.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::MissingETComponentMap,
                               xAOD::MissingETComponentMap >
   xAODMissingETComponentMapCnvBase;

/**
 *  @short POOL converter for the xAOD::MissingETComponentMap class
 *
 *         Simple converter class making the xAOD::MissingETComponentMap
 *         class known to POOL.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 586838 $
 * $Date: 2014-03-08 16:10:34 +0100 (Sat, 08 Mar 2014) $
 */
class xAODMissingETComponentMapCnv : public xAODMissingETComponentMapCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODMissingETComponentMapCnv >;

public:
   /// Converter constructor
   xAODMissingETComponentMapCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

   /// Function preparing the container to be written out
   virtual xAOD::MissingETComponentMap* createPersistent( xAOD::MissingETComponentMap* trans );
   /// Function reading in the persistent object
   virtual xAOD::MissingETComponentMap* createTransient();

private:
   /// Function preparing a met object for persistence
   void toPersistent( xAOD::MissingETComponent* metComp ) const;

   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODMissingETComponentMapCnv

#endif // XAODMISSINGETATHENAPOOL_XAODMISSINGETCOMPONENTMAPCNV_H
