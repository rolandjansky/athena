// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODHIEventShapeContainerCnv.h 594317 2014-04-25 17:36:58Z krasznaa $
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
 * $Revision: 594317 $
 * $Date: 2014-04-25 19:36:58 +0200 (Fri, 25 Apr 2014) $
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

#endif // XAODEVENTINFOATHENAPOOL_XAODEVENTINFOCONTAINERCNV_H
