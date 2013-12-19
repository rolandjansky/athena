/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODCMMJetHitsContainerCnv.h"

xAODCMMJetHitsContainerCnv::xAODCMMJetHitsContainerCnv( ISvcLocator* svcLoc )
   : xAODCMMJetHitsContainerCnvBase( svcLoc ) {
}

xAOD::CMMJetHitsContainer*
xAODCMMJetHitsContainerCnv::
createPersistent( xAOD::CMMJetHitsContainer* trans ) {

   // Create a view copy of the container:
   xAOD::CMMJetHitsContainer* result =
      new xAOD::CMMJetHitsContainer( trans->begin(), trans->end(),
                                      SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::CMMJetHitsContainer::iterator itr = result->begin();
   xAOD::CMMJetHitsContainer::iterator end = result->end();
   for( ; itr != end; ++itr ) {
      toPersistent( *itr );
   }

   // Return the new container:
   return result;
}

xAOD::CMMJetHitsContainer* xAODCMMJetHitsContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static pool::Guid v1_guid( "43B6130D-4C3F-4E22-9FE2-680E6DFA149F" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      return poolReadObject< xAOD::CMMJetHitsContainer >();
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::CMMJetHitsContainer found" );
   return 0;
}

void xAODCMMJetHitsContainerCnv::
toPersistent( xAOD::CMMJetHits* /*cluster*/ ) const {

   return;
}
