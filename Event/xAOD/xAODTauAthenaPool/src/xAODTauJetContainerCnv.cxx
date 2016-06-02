/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTauJetContainerCnv.cxx 749546 2016-05-25 01:31:32Z griffith $

// System include(s):
#include <exception>
#include <memory>

// Gaudi/Athena include(s):
#include "GaudiKernel/IOpaqueAddress.h"

// EDM include(s):
#include "xAODTau/versions/TauJetContainer_v1.h"
#include "xAODTau/versions/TauJetContainer_v2.h"

// Local include(s):
#include "xAODTauJetContainerCnv.h"
#include "xAODTauJetContainerCnv_v1.h"
#include "xAODTauJetContainerCnv_v2.h"

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

xAODTauJetContainerCnv::xAODTauJetContainerCnv( ISvcLocator* svcLoc )
   : xAODTauJetContainerCnvBase( svcLoc ) {

}

/**
 * This function needs to be re-implemented in order to figure out the StoreGate
 * key of the container that's being created. After that's done, it lets the
 * base class do its normal task.
 */
StatusCode xAODTauJetContainerCnv::createObj( IOpaqueAddress* pAddr,
                                              DataObject*& pObj ) {

   // Get the key of the container that we'll be creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of xAOD::TauJetContainer: " << m_key );

   // Let the base class do its thing now:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::TauJetContainer*
xAODTauJetContainerCnv::
createPersistent( xAOD::TauJetContainer* trans ) {

   // Create a view copy of the container:
   xAOD::TauJetContainer* result =
      new xAOD::TauJetContainer( trans->begin(), trans->end(),
                                 SG::VIEW_ELEMENTS );

   // Return the new container:
   return result;
}

xAOD::TauJetContainer* xAODTauJetContainerCnv::createTransient() {


   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "93CCE680-47C0-11E3-997C-02163E00A614" );
   static const pool::Guid v2_guid( "AACF5DF5-2D1A-4678-9188-756C27314E2F" );
   static const pool::Guid v3_guid( "9A1207C5-E25F-4974-A9D6-A51DB37F09F9" );

   // Check which version of the container we're reading:
   if (compareClassGuid( v3_guid ) ){
      // It's the latest version, read it directly:
      xAOD::TauJetContainer* c =
         poolReadObject< xAOD::TauJetContainer >();
      setStoreLink( c, m_key );
      return c;
   }   else if( compareClassGuid( v2_guid ) ) {

      // It's the latest version, read it directly:
      // The v1 converter:
      static xAODTauJetContainerCnv_v2 converter;

      // Read in the v2 version:
      std::unique_ptr< xAOD::TauJetContainer_v2 >
         old( poolReadObject< xAOD::TauJetContainer_v2 >() );

      // Return the converted object:
      xAOD::TauJetContainer* c =
         converter.createTransient( old.get(), msg() );
      setStoreLink( c, m_key );
      return c;
   } else if( compareClassGuid( v1_guid ) ) {

      // The v1 converter:
      static xAODTauJetContainerCnv_v1 converter;

      // Read in the v1 version:
      std::unique_ptr< xAOD::TauJetContainer_v1 >
         old( poolReadObject< xAOD::TauJetContainer_v1 >() );

      // Return the converted object:
      xAOD::TauJetContainer* c =
         converter.createTransient( old.get(), msg() );
      setStoreLink( c, m_key );
      return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TauJetContainer found" );
   return 0;
}
