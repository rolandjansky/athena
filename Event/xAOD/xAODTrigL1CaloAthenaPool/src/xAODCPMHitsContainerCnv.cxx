/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODCPMHitsContainerCnv.h"

xAODCPMHitsContainerCnv::xAODCPMHitsContainerCnv( ISvcLocator* svcLoc )
   : xAODCPMHitsContainerCnvBase( svcLoc ) {
}

xAOD::CPMHitsContainer*
xAODCPMHitsContainerCnv::
createPersistent( xAOD::CPMHitsContainer* trans ) {

   // Create a view copy of the container:
   xAOD::CPMHitsContainer* result =
      new xAOD::CPMHitsContainer( trans->begin(), trans->end(),
                                      SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::CPMHitsContainer::iterator itr = result->begin();
   xAOD::CPMHitsContainer::iterator end = result->end();
   for( ; itr != end; ++itr ) {
      toPersistent( *itr );
   }

   // Return the new container:
   return result;
}

xAOD::CPMHitsContainer* xAODCPMHitsContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static pool::Guid v1_guid( "3D15DF3F-E014-42E3-9614-37E4CD535F9C" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      return poolReadObject< xAOD::CPMHitsContainer >();
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::CPMHitsContainer found" );
   return 0;
}

void xAODCPMHitsContainerCnv::
toPersistent( xAOD::CPMHits* /*cluster*/ ) const {

   return;
}
