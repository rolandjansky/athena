/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODVertexContainerCnv.cxx 625403 2014-10-31 13:39:56Z nstyles $

// System include(s):
#include <exception>

// xAOD include(s):
#include "xAODTracking/TrackParticleContainer.h"

// Local include(s):
#include "xAODVertexContainerCnv.h"

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

xAODVertexContainerCnv::xAODVertexContainerCnv( ISvcLocator* svcLoc )
   : xAODVertexContainerCnvBase( svcLoc ) {
}

xAOD::VertexContainer*
xAODVertexContainerCnv::
createPersistent( xAOD::VertexContainer* trans ) {

   // Create a view copy of the container:
   xAOD::VertexContainer* result =
      new xAOD::VertexContainer( trans->begin(), trans->end(),
                                      SG::VIEW_ELEMENTS );

   // Return the new container:
   return result;
}

/**
 * This function needs to be re-implemented in order to figure out the StoreGate
 * key of the container that's being created. After that's done, it lets the
 * base class do its normal task.
 */
StatusCode xAODVertexContainerCnv::createObj( IOpaqueAddress* pAddr,
                                                   DataObject*& pObj ) {

   // Get the key of the container that we'll be creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of xAOD::VertexContainer: " << m_key );

   // Let the base class do its thing now:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::VertexContainer* xAODVertexContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static pool::Guid v1_guid( "00B6DBA1-0E0D-4FDA-BFEF-F08EEFFE4BA0" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      xAOD::VertexContainer* c = poolReadObject< xAOD::VertexContainer >();
      setStoreLink( c, m_key );
      return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::VertexContainer found" );
   return 0;
}
