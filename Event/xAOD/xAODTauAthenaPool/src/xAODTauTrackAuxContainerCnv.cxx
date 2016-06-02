/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>
#include <memory>

// Local include(s):
#include "xAODTauTrackAuxContainerCnv.h"
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"

xAODTauTrackAuxContainerCnv::xAODTauTrackAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODTauTrackAuxContainerCnvBase( svcLoc ) {

}

xAOD::TauTrackAuxContainer*
xAODTauTrackAuxContainerCnv::
createPersistent( xAOD::TauTrackAuxContainer* trans ) {

   // Create a copy of the container:
   return SG::copyThinned (*trans, IThinningSvc::instance());
}

xAOD::TauTrackAuxContainer* xAODTauTrackAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "E36C4BCF-0180-461F-8767-46768A7F3573" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {

      // It's the latest version, read it directly:
      return poolReadObject< xAOD::TauTrackAuxContainer >();

   } 
   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TauTrackAuxContainer found" );
   return 0;
}
