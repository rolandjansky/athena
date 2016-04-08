/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigMissingETContainerCnv.cxx 592512 2014-04-11 09:38:00Z krasznaa $

// System include(s):
#include <exception>

// Local include(s):
#include "xAODTrigMissingETContainerCnv.h"

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

xAODTrigMissingETContainerCnv::xAODTrigMissingETContainerCnv( ISvcLocator* svcLoc )
   : xAODTrigMissingETContainerCnvBase( svcLoc ) {

}

StatusCode xAODTrigMissingETContainerCnv::createObj( IOpaqueAddress* pAddr,
                                                     DataObject*& pObj ) {

   // Get the key of the container that we're creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of the xAOD::TrigCaloClusterContainer: " << m_key );

   // Let the base class do its thing:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::TrigMissingETContainer*
xAODTrigMissingETContainerCnv::
createPersistent( xAOD::TrigMissingETContainer* trans ) {

   // Create a view copy of the container:
   xAOD::TrigMissingETContainer* result =
      new xAOD::TrigMissingETContainer( trans->begin(), trans->end(),
                                        SG::VIEW_ELEMENTS );

   // Return the new container:
   return result;
}

xAOD::TrigMissingETContainer* xAODTrigMissingETContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static pool::Guid v1_guid( "3EA0678D-5DCA-4BA8-BB29-775CB9A246AC" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      xAOD::TrigMissingETContainer* c =
         poolReadObject< xAOD::TrigMissingETContainer >();
      setStoreLink( c, m_key );
      return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrigMissingETContainer found" );
   return 0;
}
