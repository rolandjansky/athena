/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigElectronAuxContainerCnv.cxx

// System include(s):
#include <exception>

// Local include(s):
#include "xAODTrigElectronAuxContainerCnv.h"
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"

xAODTrigElectronAuxContainerCnv::xAODTrigElectronAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODTrigElectronAuxContainerCnvBase( svcLoc ) {

}

xAOD::TrigElectronAuxContainer*
xAODTrigElectronAuxContainerCnv::
createPersistent( xAOD::TrigElectronAuxContainer* trans ) {

   // Create a copy of the container:
   return SG::copyThinned (*trans, IThinningSvc::instance());
}

xAOD::TrigElectronAuxContainer* xAODTrigElectronAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "173425E6-51BB-4015-B960-3F7F83F26B9E" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::TrigElectronAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrigElectronAuxContainer found" );
   return 0;
}
