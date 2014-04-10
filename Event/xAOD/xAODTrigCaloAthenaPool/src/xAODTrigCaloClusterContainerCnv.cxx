/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigCaloClusterContainerCnv.cxx 592328 2014-04-10 10:58:05Z krasznaa $

// System include(s):
#include <exception>

// Local include(s):
#include "xAODTrigCaloClusterContainerCnv.h"

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

xAODTrigCaloClusterContainerCnv::
xAODTrigCaloClusterContainerCnv( ISvcLocator* svcLoc )
   : xAODTrigCaloClusterContainerCnvBase( svcLoc ) {

}

StatusCode xAODTrigCaloClusterContainerCnv::createObj( IOpaqueAddress* pAddr,
                                                       DataObject*& pObj ) {

   // Get the key of the container that we're creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of the xAOD::TrigCaloClusterContainer: " << m_key );

   // Let the base class do its thing:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::TrigCaloClusterContainer*
xAODTrigCaloClusterContainerCnv::
createPersistent( xAOD::TrigCaloClusterContainer* trans ) {

   // Create a view copy of the container:
   xAOD::TrigCaloClusterContainer* result =
      new xAOD::TrigCaloClusterContainer( trans->begin(), trans->end(),
                                          SG::VIEW_ELEMENTS );

   // Return the new container:
   return result;
}

xAOD::TrigCaloClusterContainer*
xAODTrigCaloClusterContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static pool::Guid v1_guid( "0C7F7869-C67A-4E4A-8793-358F8B32DFEA" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      xAOD::TrigCaloClusterContainer* c =
         poolReadObject< xAOD::TrigCaloClusterContainer >();
      setStoreLink( c, m_key );
      return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrigCaloClusterContainer found" );
   return 0;
}
