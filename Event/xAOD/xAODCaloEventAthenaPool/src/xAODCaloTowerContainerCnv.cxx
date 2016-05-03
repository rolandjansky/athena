/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODCaloTowerContainerCnv.cxx 744625 2016-05-03 18:42:58Z ssnyder $

// System include(s):
#include <exception>

// Gaudi/Athena include(s):
#include "GaudiKernel/IOpaqueAddress.h"

// Local include(s):
#include "xAODCaloTowerContainerCnv.h"


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

xAODCaloTowerContainerCnv::xAODCaloTowerContainerCnv( ISvcLocator* svcLoc )
  : xAODCaloTowerContainerCnvBase( svcLoc ) {}


/**
 * This function needs to be re-implemented in order to figure out the StoreGate
 * key of the container that's being created. After that's done, it lets the
 * base class do its normal task.
 */
StatusCode xAODCaloTowerContainerCnv::createObj( IOpaqueAddress* pAddr,
                                                   DataObject*& pObj ) {

   // Get the key of the container that we'll be creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of xAOD::CaloTowerContainer: " << m_key );

   // Let the base class do its thing now:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::CaloTowerContainer*
xAODCaloTowerContainerCnv::
createPersistent( xAOD::CaloTowerContainer* trans ) {

  // Return a view copy of the container:
  return new xAOD::CaloTowerContainer(*trans);
}

xAOD::CaloTowerContainer* xAODCaloTowerContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static pool::Guid v1_guid( "EEA02A0F-98D3-464D-BAF1-1C944A700B8A" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      xAOD::CaloTowerContainer* c =
         poolReadObject< xAOD::CaloTowerContainer >();
      setStoreLink( c, m_key );
      return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::CaloTowerContainer found" );
   return 0;
}


