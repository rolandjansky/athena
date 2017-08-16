// $Id: xAODTrackCaloClusterContainerCnv.cxx $

// System include(s):
#include <exception>

// Local include(s):
#include "xAODTrackCaloClusterContainerCnv.h"

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

xAODTrackCaloClusterContainerCnv::xAODTrackCaloClusterContainerCnv( ISvcLocator* svcLoc )
   : xAODTrackCaloClusterContainerCnvBase( svcLoc ) {
}

xAOD::TrackCaloClusterContainer*
xAODTrackCaloClusterContainerCnv::
createPersistent( xAOD::TrackCaloClusterContainer* trans ) {

   // Create a view copy of the container:
   return
      new xAOD::TrackCaloClusterContainer( trans->begin(), trans->end(),
                                           SG::VIEW_ELEMENTS );
}

/**
 * This function needs to be re-implemented in order to figure out the StoreGate
 * key of the container that's being created. After that's done, it lets the
 * base class do its normal task.
 */
StatusCode xAODTrackCaloClusterContainerCnv::createObj( IOpaqueAddress* pAddr,
                                                   DataObject*& pObj ) {

   // Get the key of the container that we'll be creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of xAOD::TrackCaloClusterContainer: " << m_key );

   // Let the base class do its thing now:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::TrackCaloClusterContainer* xAODTrackCaloClusterContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static pool::Guid v1_guid( "84C52180-1AC0-4877-BAD3-B5C7A71F9125" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      xAOD::TrackCaloClusterContainer* c = poolReadObject< xAOD::TrackCaloClusterContainer >();
      setStoreLink( c, m_key );
      return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of xAOD::TrackCaloClusterContainer found" );
   return 0;
}
