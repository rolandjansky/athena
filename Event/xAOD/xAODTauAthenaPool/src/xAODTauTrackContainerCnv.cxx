/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// System include(s):
#include <exception>
#include <memory>

// Gaudi/Athena include(s):
#include "GaudiKernel/IOpaqueAddress.h"

// Local include(s):
#include "xAODTauTrackContainerCnv.h"

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

xAODTauTrackContainerCnv::xAODTauTrackContainerCnv( ISvcLocator* svcLoc )
   : xAODTauTrackContainerCnvBase( svcLoc ) {

}

StatusCode xAODTauTrackContainerCnv::createObj( IOpaqueAddress* pAddr,
                                              DataObject*& pObj ) {

   // Get the key of the container that we'll be creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of xAOD::TauTrackContainer: " << m_key );

   // Let the base class do its thing now:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::TauTrackContainer*
xAODTauTrackContainerCnv::
createPersistent( xAOD::TauTrackContainer* trans ) {

   // Create a view copy of the container:
   xAOD::TauTrackContainer* result =
      new xAOD::TauTrackContainer( trans->begin(), trans->end(),
                                 SG::VIEW_ELEMENTS );

   // Return the new container:
   return result;
}

xAOD::TauTrackContainer* xAODTauTrackContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "A45C1CC2-E6BE-4D99-BB49-3CE80AACEDDA" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {

      // It's the latest version, read it directly:
      xAOD::TauTrackContainer* c =
         poolReadObject< xAOD::TauTrackContainer >();
      setStoreLink( c, m_key );
      return c;
   } 

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TauTrackContainer found" );
   return 0;
}
