/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODJEMEtSumsContainerCnv.h"

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


xAODJEMEtSumsContainerCnv::xAODJEMEtSumsContainerCnv( ISvcLocator* svcLoc )
   : xAODJEMEtSumsContainerCnvBase( svcLoc ) {
}

/**
* This function needs to be re-implemented in order to figure out the StoreGate
* key of the container that's being created. After that's done, it lets the
* base class do its normal task.
*/

StatusCode xAODJEMEtSumsContainerCnv::createObj( IOpaqueAddress* pAddr, DataObject*& pObj ) {

  // Get the key of the container that we'll be creating:
  m_key = *( pAddr->par() + 1 );

  // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::JEMEtSumsContainer*
xAODJEMEtSumsContainerCnv::
createPersistent( xAOD::JEMEtSumsContainer* trans ) {

   // Create a view copy of the container:
   xAOD::JEMEtSumsContainer* result =
      new xAOD::JEMEtSumsContainer( trans->begin(), trans->end(),
                                      SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::JEMEtSumsContainer::iterator itr = result->begin();
   xAOD::JEMEtSumsContainer::iterator end = result->end();
   for( ; itr != end; ++itr ) {
      toPersistent( *itr );
   }

   // Return the new container:
   return result;
}

xAOD::JEMEtSumsContainer* xAODJEMEtSumsContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "DDB6D476-D1E2-490E-A81B-8D4FC65F1D81" );
   static const pool::Guid v2_guid( "9EE079C0-5737-4710-A81D-278D97F01E50" );
   
   // Check if we're reading the most up to date type:  
   if( compareClassGuid( v2_guid ) ) {
     xAOD::JEMEtSumsContainer* c = poolReadObject< xAOD::JEMEtSumsContainer >();
     setStoreLink( c , m_key );
     return c;
   }
   if( compareClassGuid( v1_guid ) ) {
    throw std::runtime_error( "Version 1 of xAOD::JEMEtSumsContainer found - bail out for now" );
    return 0;    
   }    

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::JEMEtSumsContainer found" );
   return 0;
}

void xAODJEMEtSumsContainerCnv::
toPersistent( xAOD::JEMEtSums* /*cluster*/ ) const {

   return;
}
