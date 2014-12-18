/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigPhotonAuxContainerCnv.cxx

// System include(s):
#include <exception>

// Local include(s):
#include "xAODTrigPhotonAuxContainerCnv.h"
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"

xAODTrigPhotonAuxContainerCnv::xAODTrigPhotonAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODTrigPhotonAuxContainerCnvBase( svcLoc ) {

}

xAOD::TrigPhotonAuxContainer*
xAODTrigPhotonAuxContainerCnv::
createPersistent( xAOD::TrigPhotonAuxContainer* trans ) {

   // Create a copy of the container:
   return SG::copyThinned (*trans, IThinningSvc::instance());
}

xAOD::TrigPhotonAuxContainer* xAODTrigPhotonAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "36B8B9E3-8F5E-4356-9315-EF4FA6959624" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::TrigPhotonAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrigPhotonAuxContainer found" );
   return 0;
}
