/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// System include(s):
#include <exception>
#include <iostream>

// Gaudi/Athena include(s):
#include "GaudiKernel/IOpaqueAddress.h"

// Local include(s):
#include "xAODTruthEventContainerCnv.h"

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


xAODTruthEventContainerCnv::xAODTruthEventContainerCnv( ISvcLocator* svcLoc )
   : xAODTruthEventContainerCnvBase( svcLoc ) {

}

/**
* This function needs to be re-implemented in order to figure out the StoreGate
* key of the container that's being created. After that's done, it lets the
* base class do its normal task.
*/
StatusCode xAODTruthEventContainerCnv::createObj( IOpaqueAddress* pAddr,
                                                   DataObject*& pObj ) {

   // Get the key of the container that we'll be creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of xAOD::TruthEventContainer: " << m_key );

   // Let the base class do its thing now:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::TruthEventContainer*
xAODTruthEventContainerCnv::createPersistent( xAOD::TruthEventContainer* trans ) {

   // Create a view copy of the container:
   xAOD::TruthEventContainer* result =
      new xAOD::TruthEventContainer( trans->begin(), trans->end(),
                              SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::TruthEventContainer::iterator itr = result->begin();
   xAOD::TruthEventContainer::iterator end = result->end();
   for( ; itr != end; ++itr ) {
      toPersistent( *itr );
   }

   // Return the new container:
   return result;
}

xAOD::TruthEventContainer* xAODTruthEventContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static pool::Guid v1_guid( "6290F297-F529-40EE-9FE5-1C577678306D" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
	xAOD::TruthEventContainer* c = poolReadObject< xAOD::TruthEventContainer >(); 
 	setStoreLink( c, m_key ); 
 	return c; 
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TruthEventContainer found" );
   return 0;
}

void xAODTruthEventContainerCnv::toPersistent( xAOD::TruthEvent* truthEvent ) const {

   // Tell the object to prepare all its smart pointers for persistification:
   truthEvent->toPersistent();

   return;
}
