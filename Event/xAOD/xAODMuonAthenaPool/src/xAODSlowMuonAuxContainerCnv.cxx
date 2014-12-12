/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODSlowMuonAuxContainerCnv.h"
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"

xAODSlowMuonAuxContainerCnv::
xAODSlowMuonAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODSlowMuonAuxContainerCnvBase( svcLoc ) {

}

xAOD::SlowMuonAuxContainer*
xAODSlowMuonAuxContainerCnv::
createPersistent( xAOD::SlowMuonAuxContainer* trans ) {

   // Create a copy of the container:
   return SG::copyThinned (*trans, IThinningSvc::instance());
}

xAOD::SlowMuonAuxContainer*
xAODSlowMuonAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
  static const pool::Guid v1_guid( "B0B4F66B-C261-4403-AB96-D71249A9CDEC");

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::SlowMuonAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::SlowMuonAuxContainer found" );
   return 0;
}
