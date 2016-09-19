/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODCaloClusterContainerCnv.cxx 757270 2016-06-23 13:52:41Z krasznaa $

// System include(s):
#include <exception>

// Gaudi/Athena include(s):
#include "GaudiKernel/IOpaqueAddress.h"

// Local include(s):
#include "xAODCaloClusterContainerCnv.h"

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

xAODCaloClusterContainerCnv::xAODCaloClusterContainerCnv( ISvcLocator* svcLoc )
  : xAODCaloClusterContainerCnvBase( svcLoc ) {

}

/**
 * This function needs to be re-implemented in order to figure out the StoreGate
 * key of the container that's being created. After that's done, it lets the
 * base class do its normal task.
 */
StatusCode xAODCaloClusterContainerCnv::createObj( IOpaqueAddress* pAddr,
                                                   DataObject*& pObj ) {

   // Get the key of the container that we'll be creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of xAOD::CaloClusterContainer: " << m_key );

   // Let the base class do its thing now:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::CaloClusterContainer*
xAODCaloClusterContainerCnv::
createPersistent( xAOD::CaloClusterContainer* trans ) {

   // Create a view copy of the container:
   xAOD::CaloClusterContainer* result =
      new xAOD::CaloClusterContainer( trans->begin(), trans->end(),
                                      SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::CaloClusterContainer::iterator itr = result->begin();
   xAOD::CaloClusterContainer::iterator end = result->end();
   for( ; itr != end; ++itr ) {
      toPersistent( *itr );
   }

   // Return the new container:
   return result;
}

xAOD::CaloClusterContainer* xAODCaloClusterContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static pool::Guid v1_guid( "24997BCA-3F6A-4530-8826-822EE9FC3C08" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      xAOD::CaloClusterContainer* c =
         poolReadObject< xAOD::CaloClusterContainer >();
      setStoreLink( c, m_key );
      return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::CaloClusterContainer found" );
   return 0;
}

void xAODCaloClusterContainerCnv::
toPersistent( xAOD::CaloCluster* cluster ) const {

   cluster->toPersistent();
   return;
}
