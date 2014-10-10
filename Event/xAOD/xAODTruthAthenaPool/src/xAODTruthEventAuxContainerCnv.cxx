/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTruthEventAuxContainerCnv.cxx 621073 2014-10-10 10:42:51Z krasznaa $

// System include(s):
#include <exception>

// Athena/Gaudi include(s):
#include "AthenaKernel/IThinningSvc.h"
#include "AthContainers/tools/copyThinned.h"

// Local include(s):
#include "xAODTruthEventAuxContainerCnv.h"

xAODTruthEventAuxContainerCnv::
xAODTruthEventAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODTruthEventAuxContainerCnvBase( svcLoc ) {

}

xAOD::TruthEventAuxContainer*
xAODTruthEventAuxContainerCnv::
createPersistent( xAOD::TruthEventAuxContainer* trans ) {

   // Access the thinning svc, if thinning is defined for this object:
   IThinningSvc* thinSvc = IThinningSvc::instance();

   // Create the persistent object using the helper function from AthContainers:
   return SG::copyThinned( *trans, thinSvc );
}

xAOD::TruthEventAuxContainer*
xAODTruthEventAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "1B945EFD-4F7D-4BDD-9FB1-6FB975315961" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::TruthEventAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TruthEventAuxContainer found" );
   return 0;
}
