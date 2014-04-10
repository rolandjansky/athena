/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigEMClusterContainerCnv.cxx

// System include(s):
#include <exception>

// Local include(s):
#include "xAODTrigEMClusterContainerCnv.h"

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

xAODTrigEMClusterContainerCnv::
xAODTrigEMClusterContainerCnv( ISvcLocator* svcLoc )
   : xAODTrigEMClusterContainerCnvBase( svcLoc ) {

}

StatusCode xAODTrigEMClusterContainerCnv::createObj( IOpaqueAddress* pAddr,
                                                     DataObject*& pObj ) {

   // Get the key of the container that we're creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of the xAOD::TrigEMClusterContainer: " << m_key );

   // Let the base class do its thing:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::TrigEMClusterContainer*
xAODTrigEMClusterContainerCnv::
createPersistent( xAOD::TrigEMClusterContainer* trans ) {

   // Create a view copy of the container:
   xAOD::TrigEMClusterContainer* result =
      new xAOD::TrigEMClusterContainer( trans->begin(), trans->end(),
                                        SG::VIEW_ELEMENTS );

   // Return the new container:
   return result;
}

xAOD::TrigEMClusterContainer* xAODTrigEMClusterContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static pool::Guid v1_guid( "B37C59EC-1A1E-467C-BFCE-52EA08694C60" );

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      xAOD::TrigEMClusterContainer* c =
         poolReadObject< xAOD::TrigEMClusterContainer >();
      setStoreLink( c, m_key );
      return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrigEMClusterContainer found" );
   return 0;
}
