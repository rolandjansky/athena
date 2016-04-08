// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODHIEventShapeAuxContainerCnv.h 693658 2015-09-08 10:00:53Z krasznaa $
#ifndef XAODHIEVENTATHENAPOOL_XAODHIEVENTSHAPEAUXCONTAINERCNV_H
#define XAODHIEVENTATHENAPOOL_XAODHIEVENTSHAPEAUXCONTAINERCNV_H

// System include(s):
#include <string>

// Gaudi/Athena include(s):
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// EDM include(s):
#include "xAODHIEvent/HIEventShapeAuxContainer.h"

/// Type definition for the converter's base
typedef T_AthenaPoolCustomCnv< xAOD::HIEventShapeAuxContainer,
                               xAOD::HIEventShapeAuxContainer >
   xAODHIEventShapeAuxContainerCnvBase;

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
 * $Revision: 693658 $
 * $Date: 2015-09-08 12:00:53 +0200 (Tue, 08 Sep 2015) $
 */
class xAODHIEventShapeAuxContainerCnv :
   public xAODHIEventShapeAuxContainerCnvBase {

   // Declare the factory as a friend of this class:
   friend class CnvFactory< xAODHIEventShapeAuxContainerCnv >;

public:
   /// Converter constructor
   xAODHIEventShapeAuxContainerCnv( ISvcLocator* svcLoc );

   /// Function preparing the object to be written out
   virtual xAOD::HIEventShapeAuxContainer*
   createPersistent( xAOD::HIEventShapeAuxContainer* trans );
   /// Function reading in the persistent object
   virtual xAOD::HIEventShapeAuxContainer* createTransient();

}; // class xAODHIEventShapeAuxContainerCnv

#endif // XAODHIEVENTATHENAPOOL_XAODHIEVENTSHAPEAUXCONTAINERCNV_H
