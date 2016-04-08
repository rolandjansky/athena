/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODBTaggingTrigAuxContainerCnv.cxx 705820 2015-11-04 14:54:05Z krasznaa $

// System include(s):
#include <exception>

// ROOT include(s):
#include <TClass.h>

// Gaudi/Athena include(s):
#include "AthenaKernel/IThinningSvc.h"
#include "AthContainers/tools/copyThinned.h"

// EDM include(s):
#include "xAODBTagging/versions/BTaggingAuxContainer_v1.h"

// Local include(s):
#include "xAODBTaggingTrigAuxContainerCnv.h"
#include "xAODBTaggingTrigAuxContainerOfflineCnv_v1.h"

xAODBTaggingTrigAuxContainerCnv::
xAODBTaggingTrigAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODBTaggingTrigAuxContainerCnvBase( svcLoc ) {

}

xAOD::BTaggingTrigAuxContainer*
xAODBTaggingTrigAuxContainerCnv::
createPersistent( xAOD::BTaggingTrigAuxContainer* trans ) {

  // Make sure that the dictionary for the Athena-only dynamic variable
  // is loaded:
  static const char* trackParticleType =
    "std::vector<std::vector<ElementLink<DataVector<xAOD::TrackParticle_v1> > > >";
  static bool dictLoaded = false;
  if( ! dictLoaded ) {
    TClass* cl = TClass::GetClass( trackParticleType );
    if( ( ! cl ) || ( ! cl->IsLoaded() ) ) {
      ATH_MSG_ERROR( "Couldn't load the dictionary for \""
                     << trackParticleType << "\"" );
    } else {
      dictLoaded = true;
    }
  }

  // Create a copy of the container:
  return SG::copyThinned (*trans, IThinningSvc::instance());
}

xAOD::BTaggingTrigAuxContainer*
xAODBTaggingTrigAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   const pool::Guid v1_guid( "1B738105-2F3E-4DCB-9DAE-DA3A4964486E" );
   const pool::Guid
      v1_offline_guid( "5E1973D2-D860-4BB1-B8EF-C9AD8E6C66A2" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {

      // It's the latest version, read it directly:
      return poolReadObject< xAOD::BTaggingTrigAuxContainer >();

   } else if( compareClassGuid( v1_offline_guid ) ) {

      // The converter:
      static xAODBTaggingTrigAuxContainerOfflineCnv_v1 converter;

      // Read in the offline v1 version:
      std::unique_ptr< xAOD::BTaggingAuxContainer_v1 >
         old( poolReadObject< xAOD::BTaggingAuxContainer_v1 >() );

      // Return the converter object:
      return converter.createTransient( old.get(), msg() );
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::BTaggingTrigAuxContainer found" );
   return 0;
}
