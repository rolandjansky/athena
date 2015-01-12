/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTauJetAuxContainerCnv.cxx 635819 2014-12-13 04:01:46Z ssnyder $

// System include(s):
#include <exception>
#include <memory>

// EDM include(s):
#include "xAODTau/versions/TauJetAuxContainer_v1.h"

// Local include(s):
#include "xAODTauJetAuxContainerCnv.h"
#include "xAODTauJetAuxContainerCnv_v1.h"
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

xAOD::TauJetAuxContainer* xAODTauJetAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "EA3CE9A0-18D8-49FD-B978-62857D8D8FD0" );
   static const pool::Guid v2_guid( "2853B3D8-136E-444D-AB48-24B1A0E13083" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v2_guid ) ) {

      // It's the latest version, read it directly:
      return poolReadObject< xAOD::TauJetAuxContainer >();

   } else if( compareClassGuid( v1_guid ) ) {

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
