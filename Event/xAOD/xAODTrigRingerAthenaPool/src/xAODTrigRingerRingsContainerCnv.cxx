/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODRingerRingsContainerCnv.cxx

// System include(s):
#include <exception>

// Local include(s):
#include "xAODTrigRingerRingsContainerCnv.h"
#include "xAODTrigRingerRingsContainerCnv_v1.h"

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

xAODTrigRingerRingsContainerCnv::xAODTrigRingerRingsContainerCnv( ISvcLocator* svcLoc ): xAODTrigRingerRingsContainerCnvBase( svcLoc ) {

}

StatusCode xAODTrigRingerRingsContainerCnv::createObj( IOpaqueAddress* pAddr, DataObject*& pObj ) {

   // Get the key of the container that we're creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of the xAOD::TrigRingerRingsContainer: " << m_key );

   // Let the base class do its thing:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::TrigRingerRingsContainer* xAODTrigRingerRingsContainerCnv::createPersistent( xAOD::TrigRingerRingsContainer* trans ) {

   // Create a view copy of the container:
   xAOD::TrigRingerRingsContainer* result = new xAOD::TrigRingerRingsContainer( trans->begin(), trans->end(), SG::VIEW_ELEMENTS );

   // Return the new container:
   return result;
}

xAOD::TrigRingerRingsContainer* xAODTrigRingerRingsContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static pool::Guid v1_guid( "AEF63C18-1B19-4861-A909-FCAF11CFBFCE" );
   static pool::Guid v2_guid( "96FADA09-FADA-4437-AC96-B02CAA5DE776" );


   // Check if we're reading the most up to date type:
   if( compareClassGuid( v2_guid ) ) {
      xAOD::TrigRingerRingsContainer* c = poolReadObject< xAOD::TrigRingerRingsContainer >();
      setStoreLink( c, m_key );
      return c;
   }

   if( compareClassGuid( v1_guid) ){
     
      static xAODTrigRingerRingsContainerCnv_v1 converter;
      // Read in the v1 version:
      std::unique_ptr< xAOD::TrigRingerRingsContainer_v1 > old( poolReadObject< xAOD::TrigRingerRingsContainer_v1 >() );
      // Return the converted object:
      xAOD::TrigRingerRingsContainer* c = converter.createTransient( old.get(), msg() );
      setStoreLink( c, m_key );
      return c;
   }

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrigRingerRingsContainer found" );
   return 0;
}








