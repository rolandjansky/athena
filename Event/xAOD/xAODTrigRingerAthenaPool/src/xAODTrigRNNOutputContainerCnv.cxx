/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigRNNOutputContainerCnv.cxx 707590 2015-11-12 19:09:03Z krasznaa $

// System include(s):
#include <exception>
#include <memory>

// EDM include(s):
#include "xAODTrigRinger/versions/TrigRNNOutputContainer_v1.h"

// Local include(s):
#include "xAODTrigRNNOutputContainerCnv.h"
#include "xAODTrigRNNOutputContainerCnv_v1.h"

namespace {

   /// Helper function setting up the container's link to its auxiliary store
   void setStoreLink( SG::AuxVectorBase* cont, const std::string& key ) {

      // The link to set up:
      DataLink< SG::IConstAuxStore > link( key + "Aux." );

      // Give it to the container:
      cont->setStore( link );

      return;
   }

} // private namespace

xAODTrigRNNOutputContainerCnv::
xAODTrigRNNOutputContainerCnv( ISvcLocator* svcLoc )
   : xAODTrigRNNOutputContainerCnvBase( svcLoc ) {

}

StatusCode xAODTrigRNNOutputContainerCnv::
createObj( IOpaqueAddress* pAddr, DataObject*& pObj ) {

   // Get the key of the container that we're creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of the xAOD::TrigRNNOutputContainer: " << m_key );

   // Let the base class do its thing:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::TrigRNNOutputContainer*
xAODTrigRNNOutputContainerCnv::
createPersistent( xAOD::TrigRNNOutputContainer* trans ) {

   // Create a view copy of the container:
   xAOD::TrigRNNOutputContainer* result =
      new xAOD::TrigRNNOutputContainer( trans->begin(), trans->end(),
                                        SG::VIEW_ELEMENTS );

   // Return the new container:
   return result;
}

xAOD::TrigRNNOutputContainer* xAODTrigRNNOutputContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static pool::Guid v1_guid( "E96CE49C-6A88-47A0-8DA0-F1D42E0813C8" );
   static pool::Guid v2_guid( "6410C90B-57BE-48A9-9A96-A203E4EF90F1" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v2_guid ) ) {

      xAOD::TrigRNNOutputContainer* c =
         poolReadObject< xAOD::TrigRNNOutputContainer >();
      setStoreLink( c, m_key );
      return c;

   } else if( compareClassGuid( v1_guid ) ) {

      // The v1 converter:
      static xAODTrigRNNOutputContainerCnv_v1 converter;

      // Read in the v1 object:
      std::unique_ptr< xAOD::TrigRNNOutputContainer_v1 >
         old( poolReadObject< xAOD::TrigRNNOutputContainer_v1 >() );

      // Return the converted object:
      xAOD::TrigRNNOutputContainer *c =
         converter.createTransient( old.get(), msg() );   
      setStoreLink( c, m_key );
      return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrigRNNOutputContainer found" );
   return 0;
}
