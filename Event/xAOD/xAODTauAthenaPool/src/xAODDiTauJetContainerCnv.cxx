/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODDiTauJetContainerCnv.cxx 631921 2015-09-23 23:30:59Z dkirchme $

// System include(s):
#include <exception>
#include <memory>

// Gaudi/Athena include(s):
#include "GaudiKernel/IOpaqueAddress.h"

// Local include(s):
#include "xAODDiTauJetContainerCnv.h"

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

xAODDiTauJetContainerCnv::xAODDiTauJetContainerCnv( ISvcLocator* svcLoc )
   : xAODDiTauJetContainerCnvBase( svcLoc ) {

}

StatusCode xAODDiTauJetContainerCnv::createObj( IOpaqueAddress* pAddr,
                                              DataObject*& pObj ) {

   // Get the key of the container that we'll be creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of xAOD::DiTauJetContainer: " << m_key );

   // Let the base class do its thing now:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::DiTauJetContainer*
xAODDiTauJetContainerCnv::
createPersistent( xAOD::DiTauJetContainer* trans ) {

   // Create a view copy of the container:
   xAOD::DiTauJetContainer* result =
      new xAOD::DiTauJetContainer( trans->begin(), trans->end(),
                                 SG::VIEW_ELEMENTS );

   // Return the new container:
   return result;
}

xAOD::DiTauJetContainer* xAODDiTauJetContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "0AD18017-0347-4F89-976E-6DA3149DA622" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {

      // It's the latest version, read it directly:
      xAOD::DiTauJetContainer* c =
         poolReadObject< xAOD::DiTauJetContainer >();
      setStoreLink( c, m_key );
      return c;
   } 

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::DiTauJetContainer found" );
   return 0;
}