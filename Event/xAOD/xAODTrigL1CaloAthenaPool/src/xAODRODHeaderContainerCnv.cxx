/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODRODHeaderContainerCnv.h"

xAODRODHeaderContainerCnv::xAODRODHeaderContainerCnv( ISvcLocator* svcLoc )
   : xAODRODHeaderContainerCnvBase( svcLoc ) {
}

xAOD::RODHeaderContainer*
xAODRODHeaderContainerCnv::
createPersistent( xAOD::RODHeaderContainer* trans ) {

   // Create a view copy of the container:
   xAOD::RODHeaderContainer* result =
      new xAOD::RODHeaderContainer( trans->begin(), trans->end(),
                                      SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::RODHeaderContainer::iterator itr = result->begin();
   xAOD::RODHeaderContainer::iterator end = result->end();
   for( ; itr != end; ++itr ) {
      toPersistent( *itr );
   }

   // Return the new container:
   return result;
}

xAOD::RODHeaderContainer* xAODRODHeaderContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static pool::Guid v1_guid( "AA0924D2-23E2-47B3-8C7D-9D3B104B8990" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      return poolReadObject< xAOD::RODHeaderContainer >();
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::RODHeaderContainer found" );
   return 0;
}

void xAODRODHeaderContainerCnv::
toPersistent( xAOD::RODHeader* /*cluster*/ ) const {

   return;
}
