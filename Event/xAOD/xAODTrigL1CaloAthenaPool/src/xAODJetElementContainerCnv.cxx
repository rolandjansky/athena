/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODJetElementContainerCnv.h"

xAODJetElementContainerCnv::xAODJetElementContainerCnv( ISvcLocator* svcLoc )
   : xAODJetElementContainerCnvBase( svcLoc ) {
}

xAOD::JetElementContainer*
xAODJetElementContainerCnv::
createPersistent( xAOD::JetElementContainer* trans ) {

   // Create a view copy of the container:
   xAOD::JetElementContainer* result =
      new xAOD::JetElementContainer( trans->begin(), trans->end(),
                                      SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::JetElementContainer::iterator itr = result->begin();
   xAOD::JetElementContainer::iterator end = result->end();
   for( ; itr != end; ++itr ) {
      toPersistent( *itr );
   }

   // Return the new container:
   return result;
}

xAOD::JetElementContainer* xAODJetElementContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static pool::Guid v1_guid( "B1103844-7187-4305-B20A-748591A54145" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      return poolReadObject< xAOD::JetElementContainer >();
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::JetElementContainer found" );
   return 0;
}

void xAODJetElementContainerCnv::
toPersistent( xAOD::JetElement* /*cluster*/ ) const {

   return;
}
