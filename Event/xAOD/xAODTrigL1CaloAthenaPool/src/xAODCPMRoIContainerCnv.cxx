/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODCPMRoIContainerCnv.h"

xAODCPMRoIContainerCnv::xAODCPMRoIContainerCnv( ISvcLocator* svcLoc )
   : xAODCPMRoIContainerCnvBase( svcLoc ) {
}

xAOD::CPMRoIContainer*
xAODCPMRoIContainerCnv::
createPersistent( xAOD::CPMRoIContainer* trans ) {

   // Create a view copy of the container:
   xAOD::CPMRoIContainer* result =
      new xAOD::CPMRoIContainer( trans->begin(), trans->end(),
                                      SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::CPMRoIContainer::iterator itr = result->begin();
   xAOD::CPMRoIContainer::iterator end = result->end();
   for( ; itr != end; ++itr ) {
      toPersistent( *itr );
   }

   // Return the new container:
   return result;
}

xAOD::CPMRoIContainer* xAODCPMRoIContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static pool::Guid v1_guid( "4B48CC07-6B22-46EC-9BD5-51379664B9BC" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      return poolReadObject< xAOD::CPMRoIContainer >();
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::CPMRoIContainer found" );
   return 0;
}

void xAODCPMRoIContainerCnv::
toPersistent( xAOD::CPMRoI* /*cluster*/ ) const {

   return;
}
