/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// xAODL2IsoMuonAuxContainerCnv.cxx giagu

// System include(s):
#include <exception>

// Local include(s):
#include "xAODL2IsoMuonAuxContainerCnv.h"
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"

xAODL2IsoMuonAuxContainerCnv::
xAODL2IsoMuonAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODL2IsoMuonAuxContainerCnvBase( svcLoc ) {

}

xAOD::L2IsoMuonAuxContainer*
xAODL2IsoMuonAuxContainerCnv::
createPersistent( xAOD::L2IsoMuonAuxContainer* trans ) {

   // Create a copy of the container:
   return SG::copyThinned (*trans, IThinningSvc::instance());
}

xAOD::L2IsoMuonAuxContainer*
xAODL2IsoMuonAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "09310901-41D9-49FB-A131-5AC73A9EA455" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::L2IsoMuonAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::L2IsoMuonAuxContainer found" );
   return 0;
}
