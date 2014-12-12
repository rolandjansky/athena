/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrackStateValidationAuxContainerCnv.cxx $

// System include(s):
#include <exception>

// Local include(s):
#include "xAODTrackStateValidationAuxContainerCnv.h"
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"

xAODTrackStateValidationAuxContainerCnv::
xAODTrackStateValidationAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODTrackStateValidationAuxContainerCnvBase( svcLoc ) {

}

xAOD::TrackStateValidationAuxContainer*
xAODTrackStateValidationAuxContainerCnv::
createPersistent( xAOD::TrackStateValidationAuxContainer* trans ) {

   // Create a copy of the container:
   return SG::copyThinned (*trans, IThinningSvc::instance());
}

xAOD::TrackStateValidationAuxContainer*
xAODTrackStateValidationAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "797432BD-DBEE-4C08-BF8E-E0556434A3F3" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::TrackStateValidationAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrackStateValidationAuxContainer found" );
   return 0;
}
