/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODL2StandAloneMuonContainerCnv.cxx 592615 2014-04-11 14:36:58Z krasznaa $

// System include(s):
#include <exception>

// Local include(s):
#include "xAODL2StandAloneMuonContainerCnv.h"

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

xAODL2StandAloneMuonContainerCnv::
xAODL2StandAloneMuonContainerCnv( ISvcLocator* svcLoc )
   : xAODL2StandAloneMuonContainerCnvBase( svcLoc ) {

}

StatusCode xAODL2StandAloneMuonContainerCnv::createObj( IOpaqueAddress* pAddr,
                                                        DataObject*& pObj ) {

   // Get the key of the container that we're creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of the xAOD::L2StandAloneMuonContainer: " << m_key );

   // Let the base class do its thing:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::L2StandAloneMuonContainer*
xAODL2StandAloneMuonContainerCnv::
createPersistent( xAOD::L2StandAloneMuonContainer* trans ) {

   // Create a view copy of the container:
   xAOD::L2StandAloneMuonContainer* result =
      new xAOD::L2StandAloneMuonContainer( trans->begin(), trans->end(),
					  SG::VIEW_ELEMENTS );

   // Return the new container:
   return result;
}

xAOD::L2StandAloneMuonContainer*
xAODL2StandAloneMuonContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static pool::Guid v1_guid( "645BDBC3-44EE-486B-8783-96F93FA2550B" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      xAOD::L2StandAloneMuonContainer* c =
         poolReadObject< xAOD::L2StandAloneMuonContainer >();
      setStoreLink( c, m_key );
      return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::L2StandAloneMuonContainer found" );
   return 0;
}
