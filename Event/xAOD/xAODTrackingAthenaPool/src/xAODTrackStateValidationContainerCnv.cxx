/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODTrackStateValidationContainerCnv.h"

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

xAODTrackStateValidationContainerCnv::xAODTrackStateValidationContainerCnv( ISvcLocator* svcLoc )
   : xAODTrackStateValidationContainerCnvBase( svcLoc ) {
}

xAOD::TrackStateValidationContainer*
xAODTrackStateValidationContainerCnv::
createPersistent( xAOD::TrackStateValidationContainer* trans ) {

   // Create a view copy of the container:
   xAOD::TrackStateValidationContainer* result =
      new xAOD::TrackStateValidationContainer( trans->begin(), trans->end(),
                                      SG::VIEW_ELEMENTS );

   // Return the new container:
   return result;
}

/**
 * This function needs to be re-implemented in order to figure out the StoreGate
 * key of the container that's being created. After that's done, it lets the
 * base class do its normal task.
 */
StatusCode xAODTrackStateValidationContainerCnv::createObj( IOpaqueAddress* pAddr,
                                                   DataObject*& pObj ) {

   // Get the key of the container that we'll be creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of xAOD::TrackStateValidationContainer: " << m_key );

   // Let the base class do its thing now:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::TrackStateValidationContainer* xAODTrackStateValidationContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static pool::Guid v1_guid( "149664B4-C7A7-4373-B5BA-D8D2DB3E65B5" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      xAOD::TrackStateValidationContainer* c = poolReadObject< xAOD::TrackStateValidationContainer >();
      setStoreLink( c, m_key );
      return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrackStateValidationContainer found" );
   return 0;
}
