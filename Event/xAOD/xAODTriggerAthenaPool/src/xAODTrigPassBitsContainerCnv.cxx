/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrigPassBitsContainerCnv.cxx 755818 2016-06-17 14:51:59Z krasznaa $

// System include(s):
#include <memory>
#include <stdexcept>

// Local include(s):
#include "xAODTrigPassBitsContainerCnv.h"

xAODTrigPassBitsContainerCnv::
xAODTrigPassBitsContainerCnv( ISvcLocator* svcLoc )
   : xAODTrigPassBitsContainerCnvBase( svcLoc ) {

}

StatusCode xAODTrigPassBitsContainerCnv::createObj( IOpaqueAddress* pAddr,
                                                    DataObject*& pObj ) {

   // Get the name of the container:
   m_key = *( pAddr->par() + 1 );

   // Let the base class do its thing:
   return AthenaPoolConverter::createObj( pAddr, pObj );
}

xAOD::TrigPassBitsContainer* xAODTrigPassBitsContainerCnv::
createPersistent( xAOD::TrigPassBitsContainer* trans ) {

   // Create a view copy of the container:
   xAOD::TrigPassBitsContainer* result =
      new xAOD::TrigPassBitsContainer( trans->begin(), trans->end(),
                                       SG::VIEW_ELEMENTS );

   // Return it:
   return result;
}

xAOD::TrigPassBitsContainer* xAODTrigPassBitsContainerCnv::createTransient() {

   // The known ID(s) of the container:
   static const pool::Guid v1_guid( "7B5DF7FF-2043-414E-BEDC-2BB0AB518AF7" );

   // Check whether we're reading the most up to date type:
   if( compareClassGuid( v1_guid ) ) {
      // Get the object:
      std::unique_ptr< xAOD::TrigPassBitsContainer >
         ondisk( poolReadObject< xAOD::TrigPassBitsContainer >() );
      // Create a copy of it:
      xAOD::TrigPassBitsContainer* result = new xAOD::TrigPassBitsContainer();
      for( size_t i = 0; i < ondisk->size(); ++i ) {
         result->push_back( new xAOD::TrigPassBits() );
      }
      // Set the store link on the copy:
      DataLink< SG::IConstAuxStore > link( m_key + "Aux." );
      result->setStore( link );
      // Return the new container:
      return result;
   }

   // Apparently the input type is unknown:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrigPassBitsContainer found" );
   return 0;
}
