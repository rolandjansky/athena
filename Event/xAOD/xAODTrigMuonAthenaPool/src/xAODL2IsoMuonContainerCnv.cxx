/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// xAODL2IsoMuonContainerCnv.cxx giagu

// System include(s):
#include <exception>

// Local include(s):
#include "xAODL2IsoMuonContainerCnv.h"

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

xAODL2IsoMuonContainerCnv::
xAODL2IsoMuonContainerCnv( ISvcLocator* svcLoc )
   : xAODL2IsoMuonContainerCnvBase( svcLoc ) {

}

StatusCode xAODL2IsoMuonContainerCnv::createObj( IOpaqueAddress* pAddr,
                                                        DataObject*& pObj ) {

   // Get the key of the container that we're creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of the xAOD::L2IsoMuonContainer: " << m_key );

   // Let the base class do its thing:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::L2IsoMuonContainer*
xAODL2IsoMuonContainerCnv::
createPersistent( xAOD::L2IsoMuonContainer* trans ) {

   // Create a view copy of the container:
   xAOD::L2IsoMuonContainer* result =
      new xAOD::L2IsoMuonContainer( trans->begin(), trans->end(),
					  SG::VIEW_ELEMENTS );

   // Return the new container:
   return result;
}

xAOD::L2IsoMuonContainer*
xAODL2IsoMuonContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static pool::Guid v1_guid( "D3AD2358-29B9-48D1-B181-89D4DA39B5CC" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      xAOD::L2IsoMuonContainer* c =
         poolReadObject< xAOD::L2IsoMuonContainer >();
      setStoreLink( c, m_key );
      return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::L2IsoMuonContainer found" );
   return 0;
}
