/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigElectronContainerCnv.cxx

// System include(s):
#include <exception>

// Local include(s):
#include "xAODTrigElectronContainerCnv.h"

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


xAODTrigElectronContainerCnv::xAODTrigElectronContainerCnv( ISvcLocator* svcLoc )
   : xAODTrigElectronContainerCnvBase( svcLoc ) {

}

StatusCode xAODTrigElectronContainerCnv::createObj( IOpaqueAddress* pAddr,
                                           DataObject*& pObj ) {

  // Get the key of the container that we'll be creating:
  m_key = *( pAddr->par() + 1 );
  ATH_MSG_VERBOSE( "Key of xAOD::xAODTrigElectronContainer: " << m_key );
  //
  // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::TrigElectronContainer*
xAODTrigElectronContainerCnv::
createPersistent( xAOD::TrigElectronContainer* trans ) {

   // Create a view copy of the container:
   xAOD::TrigElectronContainer* result =
      new xAOD::TrigElectronContainer( trans->begin(), trans->end(),
					  SG::VIEW_ELEMENTS );

   // Prepare the objects to be written out:
   xAOD::TrigElectronContainer::iterator itr = result->begin();
   xAOD::TrigElectronContainer::iterator end = result->end();
   for( ; itr != end; ++itr ) {
      toPersistent( *itr );
   }

   // Return the new container:
   return result;
}

xAOD::TrigElectronContainer* xAODTrigElectronContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static pool::Guid v1_guid( "3492BB27-3ED8-45E3-9A5B-7266949CEDA9" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      xAOD::TrigElectronContainer* c = poolReadObject< xAOD::TrigElectronContainer >();
      setStoreLink(c, m_key );
      return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrigElectronContainer found" );
   return 0;
}

void xAODTrigElectronContainerCnv::
toPersistent( xAOD::TrigElectron* /*cluster*/ ) const {

   return;
}
