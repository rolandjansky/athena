/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTruthVertexAuxContainerCnv.cxx 621073 2014-10-10 10:42:51Z krasznaa $

// System include(s):
#include <exception>

// Athena/Gaudi include(s):
#include "AthenaKernel/IThinningSvc.h"
#include "AthContainers/tools/copyThinned.h"

// Local include(s):
#include "xAODTruthVertexAuxContainerCnv.h"

xAODTruthVertexAuxContainerCnv::
xAODTruthVertexAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODTruthVertexAuxContainerCnvBase( svcLoc ) {

}

xAOD::TruthVertexAuxContainer*
xAODTruthVertexAuxContainerCnv::
createPersistent( xAOD::TruthVertexAuxContainer* trans ) {

   // Access the thinning svc, if thinning is defined for this object:
   IThinningSvc* thinSvc = IThinningSvc::instance();

   // Create the persistent object using the helper function from AthContainers:
   return SG::copyThinned( *trans, thinSvc );
}

xAOD::TruthVertexAuxContainer*
xAODTruthVertexAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "B6BD3B02-C411-4EB9-903F-5B099D3B1A3E" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::TruthVertexAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TruthVertexAuxContainer found" );
   return 0;
}
