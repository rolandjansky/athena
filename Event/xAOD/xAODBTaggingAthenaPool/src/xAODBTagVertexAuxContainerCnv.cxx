/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODBTaggingAuxContainerCnv.cxx 566967 2013-10-24 13:24:31Z krasznaa $

// System include(s):
#include <exception>

// ROOT include(s):
#include <TClass.h>

// Local include(s):
#include "xAODBTagVertexAuxContainerCnv.h"

#include "xAODTracking/TrackParticleContainer.h"
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"


xAODBTagVertexAuxContainerCnv::xAODBTagVertexAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODBTagVertexAuxContainerCnvBase( svcLoc ) {

}

xAOD::BTagVertexAuxContainer*
xAODBTagVertexAuxContainerCnv::
createPersistent( xAOD::BTagVertexAuxContainer* trans ) {
  
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

xAOD::BTagVertexAuxContainer* xAODBTagVertexAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
  const pool::Guid v1_guid( "09CD44BA-0F40-4FDB-BB30-2F4226FF3E18" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::BTagVertexAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::BTagVertexAuxContainer found" );
   return 0;
}
