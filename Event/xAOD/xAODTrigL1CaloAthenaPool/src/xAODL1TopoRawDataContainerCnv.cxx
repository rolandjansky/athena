/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODL1TopoRawDataContainerCnv.h"

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


xAODL1TopoRawDataContainerCnv::xAODL1TopoRawDataContainerCnv( ISvcLocator* svcLoc )
   : xAODL1TopoRawDataContainerCnvBase( svcLoc ) {
}

/**
* This function needs to be re-implemented in order to figure out the StoreGate
* key of the container that's being created. After that's done, it lets the
* base class do its normal task.
*/

StatusCode xAODL1TopoRawDataContainerCnv::createObj( IOpaqueAddress* pAddr, DataObject*& pObj ) {

  // Get the key of the container that we'll be creating:
  m_key = *( pAddr->par() + 1 );

  // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::L1TopoRawDataContainer*
xAODL1TopoRawDataContainerCnv::
createPersistent( xAOD::L1TopoRawDataContainer* trans ) {

   // Create a view copy of the container:
   xAOD::L1TopoRawDataContainer* result =
      new xAOD::L1TopoRawDataContainer( trans->begin(), trans->end(),
                                      SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::L1TopoRawDataContainer::iterator itr = result->begin();
   xAOD::L1TopoRawDataContainer::iterator end = result->end();
   for( ; itr != end; ++itr ) {
      toPersistent( *itr );
   }

   // Return the new container:
   return result;
}

xAOD::L1TopoRawDataContainer* xAODL1TopoRawDataContainerCnv::createTransient() {
   // The known ID(s) for this container:
   static pool::Guid v1_guid( "59FD769F-86CA-4636-8AB0-61EB3E0482EA" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
     xAOD::L1TopoRawDataContainer* c = poolReadObject< xAOD::L1TopoRawDataContainer >();
     setStoreLink( c , m_key );
     return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::L1TopoRawDataContainer found" );
   return 0;
}

void xAODL1TopoRawDataContainerCnv::
toPersistent( xAOD::L1TopoRawData* /*cluster*/ ) const {

   return;
}
