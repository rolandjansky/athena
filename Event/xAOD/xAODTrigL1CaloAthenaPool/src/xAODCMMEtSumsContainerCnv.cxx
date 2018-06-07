/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODCMMEtSumsContainerCnv.h"

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


xAODCMMEtSumsContainerCnv::xAODCMMEtSumsContainerCnv( ISvcLocator* svcLoc )
   : xAODCMMEtSumsContainerCnvBase( svcLoc ) {
}

/**
* This function needs to be re-implemented in order to figure out the StoreGate
* key of the container that's being created. After that's done, it lets the
* base class do its normal task.
*/

StatusCode xAODCMMEtSumsContainerCnv::createObj( IOpaqueAddress* pAddr, DataObject*& pObj ) {

  // Get the key of the container that we'll be creating:
  m_key = *( pAddr->par() + 1 );

  // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::CMMEtSumsContainer*
xAODCMMEtSumsContainerCnv::
createPersistent( xAOD::CMMEtSumsContainer* trans ) {

   // Create a view copy of the container:
   xAOD::CMMEtSumsContainer* result =
      new xAOD::CMMEtSumsContainer( trans->begin(), trans->end(),
                                      SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::CMMEtSumsContainer::iterator itr = result->begin();
   xAOD::CMMEtSumsContainer::iterator end = result->end();
   for( ; itr != end; ++itr ) {
      toPersistent( *itr );
   }

   // Return the new container:
   return result;
}

xAOD::CMMEtSumsContainer* xAODCMMEtSumsContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "186A613D-0C91-4880-9B09-B41C28B17B53" );   

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      xAOD::CMMEtSumsContainer* c = poolReadObject< xAOD::CMMEtSumsContainer >();
      setStoreLink( c , m_key );
      return c;
   }
   if( compareClassGuid( v1_guid ) ) {
    throw std::runtime_error( "Version 1 of xAOD::CMMEtSumsContainer found - bail out for now" );
    return 0;    
   }   

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::CMMEtSumsContainer found" );
   return 0;
}

void xAODCMMEtSumsContainerCnv::
toPersistent( xAOD::CMMEtSums* /*cluster*/ ) const {

   return;
}
