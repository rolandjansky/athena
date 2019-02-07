/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <exception>
#include <memory>
#include <iostream>

// EDM include(s):
#include "xAODTrigger/versions/TrigCompositeAuxContainer_v1.h"

// Local include(s):
#include "xAODTrigCompositeAuxContainerCnv.h"
#include "xAODTrigCompositeAuxContainerCnv_v1.h"
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"

xAODTrigCompositeAuxContainerCnv::xAODTrigCompositeAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODTrigCompositeAuxContainerCnvBase( svcLoc ) {

}

xAOD::TrigCompositeAuxContainer*
xAODTrigCompositeAuxContainerCnv::
createPersistent( xAOD::TrigCompositeAuxContainer* trans ) {

   // Create a copy of the container:
   return SG::copyThinned (*trans, IThinningSvc::instance());
}

StatusCode xAODTrigCompositeAuxContainerCnv::
createObj( IOpaqueAddress* pAddr, DataObject*& pObj ){

   // Get the key of the container that we'll be creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of xAOD::TrigCompositeAuxContainer: " << m_key );

  // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj );  
}

xAOD::TrigCompositeAuxContainer* xAODTrigCompositeAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "171EB8B8-A777-47D9-94A9-33B2482E2AAF" );
   static const pool::Guid v2_guid( "3174A8A0-7EA2-436D-93FC-0D058BE99B09" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v2_guid ) ) {

      // It's the latest version, read it directly:
      return poolReadObject< xAOD::TrigCompositeAuxContainer >();

   }
   else if( compareClassGuid( v1_guid ) ) {
  
      // The v1 converter:
      static xAODTrigCompositeAuxContainerCnv_v1 converter;

      // Read in the v1 object:
      std::unique_ptr< xAOD::TrigCompositeAuxContainer_v1 >
         old( poolReadObject< xAOD::TrigCompositeAuxContainer_v1 >() );

      // Return the converted object:
      return converter.createTransient( old.get(), msg() );
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrigCompositeAuxContainer found" );
   return 0;
}
