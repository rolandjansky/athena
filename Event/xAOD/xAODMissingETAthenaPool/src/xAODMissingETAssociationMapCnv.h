// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODMissingETAssociationMapCnv.h 795699 2017-02-05 23:26:05Z khoo $
#ifndef XAODMISSINGETATHENAPOOL_XAODMISSINGETASSOCIATIONMAPCNV_H
#define XAODMISSINGETATHENAPOOL_XAODMISSINGETASSOCIATIONMAPCNV_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODMissingET/MissingETAssociation.h"
#include "xAODMissingET/MissingETAssociationMap.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::MissingETAssociationMap,
                               xAOD::MissingETAssociationMap >
   xAODMissingETAssociationMapCnvBase;

/**
 *  @short POOL converter for the xAOD::MissingETAssociationMap class
 *
 *         Simple converter class making the xAOD::MissingETAssociationMap
 *         class known to POOL.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 795699 $
 * $Date: 2017-02-06 00:26:05 +0100 (Mon, 06 Feb 2017) $
 */
class xAODMissingETAssociationMapCnv : public xAODMissingETAssociationMapCnvBase {

   // Declare the factory as our friend:
   friend class CnvFactory< xAODMissingETAssociationMapCnv >;

public:
   /// Converter constructor
   xAODMissingETAssociationMapCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

   /// Function preparing the container to be written out
   virtual xAOD::MissingETAssociationMap* createPersistent( xAOD::MissingETAssociationMap* trans );
   /// Function reading in the persistent object
   virtual xAOD::MissingETAssociationMap* createTransient();

private:
   /// Function preparing a met object for persistence
   // Should be done by the aux container
   // void toPersistent( xAOD::MissingETAssociation* metComp ) const;

   /// StoreGate key of the container just being created
   std::string m_key;

}; // class xAODMissingETAssociationMapCnv

#endif // XAODMISSINGETATHENAPOOL_XAODMISSINGETASSOCIATIONMAPCNV_H
