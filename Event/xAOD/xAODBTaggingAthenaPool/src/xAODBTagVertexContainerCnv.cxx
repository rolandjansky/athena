/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODBTagVertexContainerCnv.cxx 581167 2014-02-03 10:45:56Z $

// System include(s):
#include <exception>

// Local include(s):
#include "xAODBTagVertexContainerCnv.h"

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

xAODBTagVertexContainerCnv::xAODBTagVertexContainerCnv( ISvcLocator* svcLoc )
   : xAODBTagVertexContainerCnvBase( svcLoc ) {

}

/**
 * This function needs to be re-implemented in order to figure out the StoreGate
 * key of the container that's being created. After that's done, it lets the
 * base class do its normal task.
 */
StatusCode xAODBTagVertexContainerCnv::createObj( IOpaqueAddress* pAddr,
                                           DataObject*& pObj ) {

   // Get the key of the container that we'll be creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of xAOD::BTagVertexContainer: " << m_key );

   // Let the base class do its thing now:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::BTagVertexContainer*
xAODBTagVertexContainerCnv::createPersistent( xAOD::BTagVertexContainer* trans ) {

   // Create a view copy of the container:
   xAOD::BTagVertexContainer* result =
      new xAOD::BTagVertexContainer( trans->begin(), trans->end(),
                              SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::BTagVertexContainer::iterator itr = result->begin();
   xAOD::BTagVertexContainer::iterator end = result->end();
   for( ; itr != end; ++itr ) {
      toPersistent( *itr );
   }

   // Return the new container:
   return result;
}

xAOD::BTagVertexContainer* xAODBTagVertexContainerCnv::createTransient() {

   // The known ID(s) for this container:
  const pool::Guid v1_guid( "E225A9EC-9782-43F7-B7BF-1B215187C11C" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      xAOD::BTagVertexContainer* c = poolReadObject< xAOD::BTagVertexContainer >();
      setStoreLink( c, m_key );
      return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::BTagVertexContainer found" );
   return 0;
}

void xAODBTagVertexContainerCnv::toPersistent( xAOD::BTagVertex* BTagVertex ) const {

   // Tell the object to prepare all its smart pointers for persistification:
   BTagVertex->toPersistent();

   return;
}
