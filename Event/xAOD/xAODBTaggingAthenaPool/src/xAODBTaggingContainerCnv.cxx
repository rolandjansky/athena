/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODBTaggingContainerCnv.cxx 581167 2014-02-03 10:45:56Z krasznaa $

// System include(s):
#include <exception>

// Local include(s):
#include "xAODBTaggingContainerCnv.h"

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

xAODBTaggingContainerCnv::xAODBTaggingContainerCnv( ISvcLocator* svcLoc )
   : xAODBTaggingContainerCnvBase( svcLoc ) {

}

/**
 * This function needs to be re-implemented in order to figure out the StoreGate
 * key of the container that's being created. After that's done, it lets the
 * base class do its normal task.
 */
StatusCode xAODBTaggingContainerCnv::createObj( IOpaqueAddress* pAddr,
                                           DataObject*& pObj ) {

   // Get the key of the container that we'll be creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of xAOD::BTaggingContainer: " << m_key );

   // Let the base class do its thing now:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::BTaggingContainer*
xAODBTaggingContainerCnv::createPersistent( xAOD::BTaggingContainer* trans ) {

   // Create a view copy of the container:
   xAOD::BTaggingContainer* result =
      new xAOD::BTaggingContainer( trans->begin(), trans->end(),
                              SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::BTaggingContainer::iterator itr = result->begin();
   xAOD::BTaggingContainer::iterator end = result->end();
   for( ; itr != end; ++itr ) {
      toPersistent( *itr );
   }

   // Return the new container:
   return result;
}

xAOD::BTaggingContainer* xAODBTaggingContainerCnv::createTransient() {

   // The known ID(s) for this container:
  const pool::Guid v1_guid( "5D19F171-E07E-4F65-BC1F-FC4F4099E9F1" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      xAOD::BTaggingContainer* c = poolReadObject< xAOD::BTaggingContainer >();
      setStoreLink( c, m_key );
      return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::BTaggingContainer found" );
   return 0;
}

void xAODBTaggingContainerCnv::toPersistent( xAOD::BTagging* BTagging ) const {

   // Tell the object to prepare all its smart pointers for persistification:
   BTagging->toPersistent();

   return;
}
