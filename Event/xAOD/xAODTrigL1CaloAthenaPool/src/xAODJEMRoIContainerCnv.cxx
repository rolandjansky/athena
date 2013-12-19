/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODJEMRoIContainerCnv.h"

xAODJEMRoIContainerCnv::xAODJEMRoIContainerCnv( ISvcLocator* svcLoc )
   : xAODJEMRoIContainerCnvBase( svcLoc ) {
}

xAOD::JEMRoIContainer*
xAODJEMRoIContainerCnv::
createPersistent( xAOD::JEMRoIContainer* trans ) {

   // Create a view copy of the container:
   xAOD::JEMRoIContainer* result =
      new xAOD::JEMRoIContainer( trans->begin(), trans->end(),
                                      SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::JEMRoIContainer::iterator itr = result->begin();
   xAOD::JEMRoIContainer::iterator end = result->end();
   for( ; itr != end; ++itr ) {
      toPersistent( *itr );
   }

   // Return the new container:
   return result;
}

xAOD::JEMRoIContainer* xAODJEMRoIContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static pool::Guid v1_guid( "52384828-A57D-477D-A8C8-597C78D02686" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      return poolReadObject< xAOD::JEMRoIContainer >();
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::JEMRoIContainer found" );
   return 0;
}

void xAODJEMRoIContainerCnv::
toPersistent( xAOD::JEMRoI* /*cluster*/ ) const {

   return;
}
