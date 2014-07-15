/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigPhotonContainerCnv.cxx

// System include(s):
#include <exception>

// Local include(s):
#include "xAODTrigPhotonContainerCnv.h"

namespace {
  // Helper function setting up the container's link to its auxiliary store
  void setStoreLink( SG::AuxVectorBase* cont, const std::string& key ) {
    // The link to set up:
    DataLink< SG::IConstAuxStore > link( key + "Aux." );
   
    // Give it to the container:
    cont->setStore( link );

    return;
  }   
} // private namespace 

xAODTrigPhotonContainerCnv::xAODTrigPhotonContainerCnv( ISvcLocator* svcLoc )
   : xAODTrigPhotonContainerCnvBase( svcLoc ) {

}

StatusCode xAODTrigPhotonContainerCnv::createObj( IOpaqueAddress* pAddr,
                                           DataObject*& pObj ) {
 
  // Get the key of the container that we'll be creating:
  m_key = *( pAddr->par() + 1 );
  ATH_MSG_VERBOSE( "Key of xAOD::xAODTrigPhotonContainer: " << m_key );
 
  // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::TrigPhotonContainer*
xAODTrigPhotonContainerCnv::
createPersistent( xAOD::TrigPhotonContainer* trans ) {

   // Create a view copy of the container:
   xAOD::TrigPhotonContainer* result =
      new xAOD::TrigPhotonContainer( trans->begin(), trans->end(),
					  SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::TrigPhotonContainer::iterator itr = result->begin();
   xAOD::TrigPhotonContainer::iterator end = result->end();
   for( ; itr != end; ++itr ) {
      toPersistent( *itr );
   }

   // Return the new container:
   return result;
}

xAOD::TrigPhotonContainer* xAODTrigPhotonContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static pool::Guid v1_guid( "DA2CDAF5-B0E8-4502-89A3-E342DFA9C250" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      xAOD::TrigPhotonContainer* c = poolReadObject< xAOD::TrigPhotonContainer >();
      setStoreLink(c, m_key );
      return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrigPhotonContainer found" );
   return 0;
}

void xAODTrigPhotonContainerCnv::
toPersistent( xAOD::TrigPhoton* /*cluster*/ ) const {

   return;
}
