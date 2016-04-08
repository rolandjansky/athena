/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODEventInfoContainerCnv.cxx 682544 2015-07-13 13:53:25Z krasznaa $

// System include(s):
#include <exception>

// Local include(s):
#include "xAODEventInfoContainerCnv.h"

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

xAODEventInfoContainerCnv::xAODEventInfoContainerCnv( ISvcLocator* svcLoc )
   : xAODEventInfoContainerCnvBase( svcLoc ), m_key() {

}

StatusCode xAODEventInfoContainerCnv::createObj( IOpaqueAddress* pAddr,
                                                 DataObject*& pObj ) {

   // Get the key of the object that we'll be creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of xAOD::EventInfo: " << m_key );

   // Let the base class do its thing now:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::EventInfoContainer*
xAODEventInfoContainerCnv::createPersistent( xAOD::EventInfoContainer* trans ) {

   // Create a view copy of the container:
   xAOD::EventInfoContainer* result =
      new xAOD::EventInfoContainer( trans->begin(), trans->end(),
                                    SG::VIEW_ELEMENTS );

   // Prepare all objects to be written out:
   xAOD::EventInfoContainer::iterator itr = result->begin();
   xAOD::EventInfoContainer::iterator end = result->end();
   for( ; itr != end; ++itr ) {
      ( *itr )->toPersistent();
   }

   // Return the new container:
   return result;
}

xAOD::EventInfoContainer* xAODEventInfoContainerCnv::createTransient() {

   // The known ID(s) for the object:
   static const pool::Guid v1_guid( "8F061263-D744-4D72-9377-1573FE21CDCE" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      xAOD::EventInfoContainer* ei =
         poolReadObject< xAOD::EventInfoContainer >();
      setStoreLink( ei, m_key );
      return ei;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of xAOD::EventInfoContainer"
                             " found" );
   return 0;
}
