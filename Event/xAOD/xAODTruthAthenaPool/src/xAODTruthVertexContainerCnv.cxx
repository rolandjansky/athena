/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTruthVertexContainerCnv.cxx 582241 2014-02-07 17:28:39Z jcatmore $

// System include(s):
#include <exception>
#include <iostream>

// Gaudi/Athena include(s):
#include "GaudiKernel/IOpaqueAddress.h"

// Local include(s):
#include "xAODTruthVertexContainerCnv.h"

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
   

xAODTruthVertexContainerCnv::xAODTruthVertexContainerCnv( ISvcLocator* svcLoc )
   : xAODTruthVertexContainerCnvBase( svcLoc ) {

}

/**
 * * This function needs to be re-implemented in order to figure out the StoreGate
 * * key of the container that's being created. After that's done, it lets the
 * * base class do its normal task.
 * */
StatusCode xAODTruthVertexContainerCnv::createObj( IOpaqueAddress* pAddr,
                                                   DataObject*& pObj ) {

   // Get the key of the container that we'll be creating:
    m_key = *( pAddr->par() + 1 );
    ATH_MSG_VERBOSE( "Key of xAOD::TruthVertexContainer: " << m_key );
   
   // Let the base class do its thing now:
   return AthenaPoolConverter::createObj( pAddr, pObj ); 
}

xAOD::TruthVertexContainer*
xAODTruthVertexContainerCnv::createPersistent( xAOD::TruthVertexContainer* trans ) {

   // Create a view copy of the container:
   xAOD::TruthVertexContainer* result =
      new xAOD::TruthVertexContainer( trans->begin(), trans->end(),
                              SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::TruthVertexContainer::iterator itr = result->begin();
   xAOD::TruthVertexContainer::iterator end = result->end();
   for( ; itr != end; ++itr ) {
      toPersistent( *itr );
   }

   // Return the new container:
   return result;
}

xAOD::TruthVertexContainer* xAODTruthVertexContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static pool::Guid v1_guid( "5FBAE0AB-09F7-4B6C-B066-0A003FC38ECF" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
	xAOD::TruthVertexContainer* c =
	poolReadObject< xAOD::TruthVertexContainer >();
	setStoreLink( c, m_key );
	return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TruthVertexContainer found" );
   return 0;
}

void xAODTruthVertexContainerCnv::toPersistent( xAOD::TruthVertex* truthVertex ) const {

   // Tell the object to prepare all its smart pointers for persistification:
   truthVertex->toPersistent();

   return;
}
