/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODHIEventShapeContainerCnv.cxx 693514 2015-09-07 15:51:04Z krasznaa $

// System include(s):
#include <exception>

// Local include(s):
#include "xAODHIEventShapeContainerCnv.h"
#include "xAODHIEventShapeContainerCnv_v1.h"

namespace {

   /// Helper function to set up the object's link to its auxiliary store
   void setStoreLink( SG::AuxVectorBase* vec, const std::string& key ) {

      // The link to set up:
      DataLink< SG::IConstAuxStore > link( key + "Aux." );

      // Give it to the object:
      vec->setStore( link );

      return;
   }

} // private namespace

xAODHIEventShapeContainerCnv::
xAODHIEventShapeContainerCnv( ISvcLocator* svcLoc )
   : xAODHIEventShapeContainerCnvBase( svcLoc ), m_key() {

}

StatusCode xAODHIEventShapeContainerCnv::createObj( IOpaqueAddress* pAddr,
                                                    DataObject*& pObj ) {

   // Get the key of the object that we'll be creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of xAOD::HIEventShape: " << m_key );

   // Let the base class do its thing now:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::HIEventShapeContainer*
xAODHIEventShapeContainerCnv::
createPersistent( xAOD::HIEventShapeContainer* trans ) {

   // Create a view copy of the container:
   xAOD::HIEventShapeContainer* result =
      new xAOD::HIEventShapeContainer( trans->begin(), trans->end(),
                                       SG::VIEW_ELEMENTS );

   // Return the new container:
   return result;
}

xAOD::HIEventShapeContainer* xAODHIEventShapeContainerCnv::createTransient() {

   // The known ID(s) for the object:
   static const pool::Guid v1_guid( "FA2C3CB2-71A6-11E4-BB30-02163E010E29" );
   static const pool::Guid v2_guid( "386D4EAA-358E-42CB-B35C-126D69D1331C" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v2_guid ) ) {

      // It's the latest version, read it without further fuss:
      xAOD::HIEventShapeContainer* c =
         poolReadObject< xAOD::HIEventShapeContainer >();
      setStoreLink( c, m_key );
      return c;

   } else if( compareClassGuid( v1_guid ) ) {

      // The v1 converter:
      static xAODHIEventShapeContainerCnv_v1 converter;

      // Read in the v1 version:
      std::unique_ptr< xAOD::HIEventShapeContainer_v1 >
         old( poolReadObject< xAOD::HIEventShapeContainer_v1 >() );

      // Create and return the converted object:
      xAOD::HIEventShapeContainer* c =
         converter.createTransient( old.get(), msg() );
      setStoreLink( c, m_key );
      return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::HIEventShapeContainer found" );
   return 0;
}
