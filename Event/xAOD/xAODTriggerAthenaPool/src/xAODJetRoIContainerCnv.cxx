/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODJetRoIContainerCnv.h"
#include "xAODJetRoIContainerCnv_v1.h"

xAODJetRoIContainerCnv::xAODJetRoIContainerCnv( ISvcLocator* svcLoc )
   : xAODJetRoIContainerCnvBase( svcLoc ) {
}

xAOD::JetRoIContainer*
xAODJetRoIContainerCnv::
createPersistent( xAOD::JetRoIContainer* trans ) {
   // Create a view copy of the container:
   xAOD::JetRoIContainer* result =
      new xAOD::JetRoIContainer( trans->begin(), trans->end(),
                                      SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::JetRoIContainer::iterator itr = result->begin();
   xAOD::JetRoIContainer::iterator end = result->end();
   for( ; itr != end; ++itr ) {
      toPersistent( *itr );
   }

   // Return the new container:
   return result;
}

xAOD::JetRoIContainer* xAODJetRoIContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static pool::Guid v1_guid( "ED39F230-5E9A-11E3-9563-02163E00A725" );
   static pool::Guid v2_guid( "8F9A76C9-2083-4774-A0EE-FB585930B664" );   

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v2_guid ) ) {
      return poolReadObject< xAOD::JetRoIContainer >();
   }
   if( compareClassGuid( v1_guid ) ) {
      // The v1 converter:
      static xAODJetRoIContainerCnv_v1 converter;

      // Read in the v1 version:
      std::unique_ptr< xAOD::JetRoIContainer_v1 >
         old( poolReadObject< xAOD::JetRoIContainer_v1 >() );

      // Return the converted object:
      xAOD::JetRoIContainer* c =
         converter.createTransient( old.get(), msg() );
      //setStoreLink( c, m_key );
      return c;
   }   

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::JetRoIContainer found" );
   return 0;
}

void xAODJetRoIContainerCnv::
toPersistent( xAOD::JetRoI* /*cluster*/ ) const {

   return;
}
