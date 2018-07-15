/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTauJetAuxContainerCnv.cxx 800296 2017-03-10 18:16:40Z griffith $

// System include(s):
#include <exception>
#include <memory>
#include <iostream>

// EDM include(s):
#include "xAODTau/versions/TauJetAuxContainer_v1.h"

// Local include(s):
#include "xAODTauJetAuxContainerCnv.h"
#include "xAODTauJetAuxContainerCnv_v1.h"
#include "xAODTauJetAuxContainerCnv_v2.h"
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"

xAODTauJetAuxContainerCnv::xAODTauJetAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODTauJetAuxContainerCnvBase( svcLoc ) {

}

xAOD::TauJetAuxContainer*
xAODTauJetAuxContainerCnv::
createPersistent( xAOD::TauJetAuxContainer* trans ) {

   // Create a copy of the container:
   return SG::copyThinned (*trans, IThinningSvc::instance());
}

StatusCode xAODTauJetAuxContainerCnv::
createObj( IOpaqueAddress* pAddr, DataObject*& pObj ){

   // Get the key of the container that we'll be creating:
   m_key = *( pAddr->par() + 1 );
   ATH_MSG_VERBOSE( "Key of xAOD::TauJetAuxContainer: " << m_key );

  // Let the base class do its thing now:
  return AthenaPoolConverter::createObj( pAddr, pObj );  
}

xAOD::TauJetAuxContainer* xAODTauJetAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "EA3CE9A0-18D8-49FD-B978-62857D8D8FD0" );
   static const pool::Guid v2_guid( "2853B3D8-136E-444D-AB48-24B1A0E13083" );
   static const pool::Guid v3_guid( "77AA6800-DDAD-44E8-AD90-003D48082A94" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v3_guid ) ) {

      // It's the latest version, read it directly:
      return poolReadObject< xAOD::TauJetAuxContainer >();

   } else if(compareClassGuid( v2_guid )) {
      // The v2 converter:
     static xAODTauJetAuxContainerCnv_v2 converter;
     converter.setKey(m_key);
     //      std::cout << "Converting TauJet_v2 --> v3 not fully supported" << std::endl;

      // Read in the v2 object:
      std::unique_ptr< xAOD::TauJetAuxContainer_v2 >
         old( poolReadObject< xAOD::TauJetAuxContainer_v2 >() );

      // Return the converted object:
      return converter.createTransient( old.get(), msg() );
   }
   else if( compareClassGuid( v1_guid ) ) {
  
      // The v1 converter:
      static xAODTauJetAuxContainerCnv_v1 converter;

      // Read in the v1 object:
      std::unique_ptr< xAOD::TauJetAuxContainer_v1 >
         old( poolReadObject< xAOD::TauJetAuxContainer_v1 >() );

      // Return the converted object:
      return converter.createTransient( old.get(), msg() );
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TauJetAuxContainer found" );
   return 0;
}
