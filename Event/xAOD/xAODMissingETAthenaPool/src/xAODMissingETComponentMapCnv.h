// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODMissingETComponentMapCnv.h 795699 2017-02-05 23:26:05Z khoo $
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
 * $Revision: 795699 $
 * $Date: 2017-02-06 00:26:05 +0100 (Mon, 06 Feb 2017) $
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
   // /// Function preparing a met object for persistence
   // void toPersistent( xAOD::MissingETComponent* metComp ) const;

   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODMissingETComponentMapCnv

#endif // XAODMISSINGETATHENAPOOL_XAODMISSINGETCOMPONENTMAPCNV_H
