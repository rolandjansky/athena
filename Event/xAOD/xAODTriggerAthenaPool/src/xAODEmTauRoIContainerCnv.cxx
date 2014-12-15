/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODEmTauRoIContainerCnv.h"
#include "xAODEmTauRoIContainerCnv_v1.h"

xAODEmTauRoIContainerCnv::xAODEmTauRoIContainerCnv( ISvcLocator* svcLoc )
   : xAODEmTauRoIContainerCnvBase( svcLoc ) {
}

xAOD::EmTauRoIContainer*
xAODEmTauRoIContainerCnv::
createPersistent( xAOD::EmTauRoIContainer* trans ) {
   // Create a view copy of the container:
   xAOD::EmTauRoIContainer* result =
      new xAOD::EmTauRoIContainer( trans->begin(), trans->end(),
                                      SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::EmTauRoIContainer::iterator itr = result->begin();
   xAOD::EmTauRoIContainer::iterator end = result->end();
   for( ; itr != end; ++itr ) {
      toPersistent( *itr );
   }

   // Return the new container:
   return result;
}

xAOD::EmTauRoIContainer* xAODEmTauRoIContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static pool::Guid v1_guid( "101CD1EE-5EA2-11E3-895D-02163E00A743" );
   static pool::Guid v2_guid( "6AB862C4-31E6-4F66-AAE8-56BA01E350F2" );   

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v2_guid ) ) {
      return poolReadObject< xAOD::EmTauRoIContainer >();
   }
   if( compareClassGuid( v1_guid ) ) {
      // The v1 converter:
      static xAODEmTauRoIContainerCnv_v1 converter;

      // Read in the v1 version:
      std::unique_ptr< xAOD::EmTauRoIContainer_v1 >
         old( poolReadObject< xAOD::EmTauRoIContainer_v1 >() );

      // Return the converted object:
      xAOD::EmTauRoIContainer* c =
         converter.createTransient( old.get(), msg() );
      //setStoreLink( c, m_key );
      return c;
   }   

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::EmTauRoIContainer found" );
   return 0;
}

void xAODEmTauRoIContainerCnv::
toPersistent( xAOD::EmTauRoI* /*cluster*/ ) const {

   return;
}
