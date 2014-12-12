/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// xAODL2CombinedMuonContainerCnv.cxx giagu

// System include(s):
#include <exception>

// Local include(s):
#include "xAODL2CombinedMuonContainerCnv.h"

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

xAODL2CombinedMuonContainerCnv::
xAODL2CombinedMuonContainerCnv( ISvcLocator* svcLoc )
   : xAODL2CombinedMuonContainerCnvBase( svcLoc ) {

}

StatusCode xAODL2CombinedMuonContainerCnv::createObj( IOpaqueAddress* pAddr,
                                                        DataObject*& pObj ) {

   // Get the key of the container that we're creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of the xAOD::L2CombinedMuonContainer: " << m_key );

   // Let the base class do its thing:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::L2CombinedMuonContainer*
xAODL2CombinedMuonContainerCnv::
createPersistent( xAOD::L2CombinedMuonContainer* trans ) {

   // Create a view copy of the container:
   xAOD::L2CombinedMuonContainer* result =
      new xAOD::L2CombinedMuonContainer( trans->begin(), trans->end(),
					  SG::VIEW_ELEMENTS );

   // Return the new container:
   return result;
}

xAOD::L2CombinedMuonContainer*
xAODL2CombinedMuonContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static pool::Guid v1_guid( "91ECD1FA-3B9F-4F80-BBD1-698DCC7C9477" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      xAOD::L2CombinedMuonContainer* c =
         poolReadObject< xAOD::L2CombinedMuonContainer >();
      setStoreLink( c, m_key );
      return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::L2CombinedMuonContainer found" );
   return 0;
}
