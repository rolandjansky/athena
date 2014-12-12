/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// xAODL2CombinedMuonAuxContainerCnv.cxx giagu

// System include(s):
#include <exception>

// Local include(s):
#include "xAODL2CombinedMuonAuxContainerCnv.h"
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"

xAODL2CombinedMuonAuxContainerCnv::
xAODL2CombinedMuonAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODL2CombinedMuonAuxContainerCnvBase( svcLoc ) {

}

xAOD::L2CombinedMuonAuxContainer*
xAODL2CombinedMuonAuxContainerCnv::
createPersistent( xAOD::L2CombinedMuonAuxContainer* trans ) {

   // Create a copy of the container:
   return SG::copyThinned (*trans, IThinningSvc::instance());
}

xAOD::L2CombinedMuonAuxContainer*
xAODL2CombinedMuonAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "0A7A1CB4-A5D3-4447-BE1A-F4DA14D5891B" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::L2CombinedMuonAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::L2CombinedMuonAuxContainer found" );
   return 0;
}
