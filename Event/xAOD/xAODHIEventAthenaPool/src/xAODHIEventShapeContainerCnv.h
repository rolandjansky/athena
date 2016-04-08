// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODHIEventShapeContainerCnv.h 693514 2015-09-07 15:51:04Z krasznaa $
#ifndef XAODHIEVENTATHENAPOOL_XAODHIEVENTSHAPECONTAINERCNV_H
#define XAODHIEVENTATHENAPOOL_XAODHIEVENTSHAPECONTAINERCNV_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODHIEvent/HIEventShapeContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::HIEventShapeContainer,
                               xAOD::HIEventShapeContainer >
   xAODHIEventShapeContainerCnvBase;

/**
 *  @short POOL converter for the xAOD::HIEventShapeContainer class
 *
 *         A custom POOL converter is only needed to make sure that the
 *         HIEventShapeContainer gets connected to its auxiliary store correctly
 *         in case it is read in using a DataLink/ElementLink from an input
 *         file.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 693514 $
 * $Date: 2015-09-07 17:51:04 +0200 (Mon, 07 Sep 2015) $
 */
class xAODHIEventShapeContainerCnv : public xAODHIEventShapeContainerCnvBase {

   // Declare the factory as a friend of this class:
   friend class CnvFactory< xAODHIEventShapeContainerCnv >;

public:
   /// Converter constructor
   xAODHIEventShapeContainerCnv( ISvcLocator* svcLoc );

   /// Re-implemented function in order to get access to the SG key
   virtual StatusCode createObj( IOpaqueAddress* pAddr, DataObject*& pObj );

   /// Function preparing the object to be written out
   virtual xAOD::HIEventShapeContainer*
   createPersistent( xAOD::HIEventShapeContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::HIEventShapeContainer* createTransient();

private:
   /// StoreGate key of the container being read
   std::string m_key;

}; // class xAODHIEventShapeContainerCnv

#endif // XAODHIEVENTATHENAPOOL_XAODHIEVENTSHAPECONTAINERCNV_H
