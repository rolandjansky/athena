/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTruthPileupEventAuxContainerCnv.cxx 622196 2014-10-16 16:10:41Z krasznaa $

// System include(s):
#include <exception>

// Athena/Gaudi include(s):
#include "AthenaKernel/IThinningSvc.h"
#include "AthContainers/tools/copyThinned.h"

// Local include(s):
#include "xAODTruthPileupEventAuxContainerCnv.h"

xAODTruthPileupEventAuxContainerCnv::
xAODTruthPileupEventAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODTruthPileupEventAuxContainerCnvBase( svcLoc ) {

}

xAOD::TruthPileupEventAuxContainer*
xAODTruthPileupEventAuxContainerCnv::
createPersistent( xAOD::TruthPileupEventAuxContainer* trans ) {

   // Access the thinning svc, if thinning is defined for this object:
   IThinningSvc* thinSvc = IThinningSvc::instance();

   // Create the persistent object using the helper function from AthContainers:
   return SG::copyThinned( *trans, thinSvc );
}

xAOD::TruthPileupEventAuxContainer*
xAODTruthPileupEventAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "9E9DD653-247C-4D5E-B14C-538EADEA6CD2" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::TruthPileupEventAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TruthPileupEventAuxContainer found" );
   return 0;
}
