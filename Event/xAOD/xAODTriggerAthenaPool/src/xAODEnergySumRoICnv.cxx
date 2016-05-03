/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>

// Local include(s):
#include "xAODEnergySumRoICnv.h"
#include "xAODEnergySumRoICnv_v1.h"

xAODEnergySumRoICnv::xAODEnergySumRoICnv( ISvcLocator* svcLoc )
   : xAODEnergySumRoICnvBase( svcLoc ) {
}

xAOD::EnergySumRoI*
xAODEnergySumRoICnv::
createPersistent( xAOD::EnergySumRoI* trans ) {
   // Create a view copy of the object:
   xAOD::EnergySumRoI* result = 
      new xAOD::EnergySumRoI( *trans );

   // Prepare the objects to be written out:
   toPersistent( result );

   // Return the new container:
   return result;
}

xAOD::EnergySumRoI* xAODEnergySumRoICnv::createTransient() {
   // The known ID(s) for this container:
   static pool::Guid v1_guid( "727CB19A-5EB8-11E3-81B0-02163E00A743" );
   static pool::Guid v2_guid( "3F26401F-25D9-4CCA-B883-57EBCEE8D299" );   

   // Check if we're reading the most up to date type:
   if( compareClassGuid( v2_guid ) ) {
      return poolReadObject< xAOD::EnergySumRoI >();
   }
   if( compareClassGuid( v1_guid ) ) {
      // The v1 converter:
      static xAODEnergySumRoICnv_v1 converter;

      // Read in the v1 version:
      //std::unique_ptr< xAOD::EnergySumRoI_v1 >
      //    old( poolReadObject< xAOD::EnergySumRoI_v1 >() );
      xAOD::EnergySumRoI_v1* old = poolReadObject< xAOD::EnergySumRoI_v1 >();
      // Return the converted object:
      xAOD::EnergySumRoI* c =
         converter.createTransient( old, msg() );
      
      DataLink< SG::IConstAuxStore> link ("LVL1EnergySumRoIsAux.");
      c->setStore(link);
      //setStoreLink( c, m_key );
      return c;
   }   

   // If we didn't recognise the ID, let's complain:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::EnergySumRoI found" );
   return 0;
}

void xAODEnergySumRoICnv::
toPersistent( xAOD::EnergySumRoI* /*roi*/ ) const {

   return;
}
