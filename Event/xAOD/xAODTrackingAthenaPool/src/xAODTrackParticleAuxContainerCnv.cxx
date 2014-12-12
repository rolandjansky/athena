/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrackParticleAuxContainerCnv.cxx 635799 2014-12-12 22:08:09Z ssnyder $

// System include(s):
#include <exception>

// ROOT include(s):
#include <TClass.h>

// Athena/Gaudi include(s):
#include "AthContainers/tools/copyThinned.h"
#include "AthenaKernel/IThinningSvc.h"
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "xAODTrackParticleAuxContainerCnv.h"
#include "xAODTrackParticleAuxContainerCnv_v1.h"

// EDM include(s):
// #include "xAODTracking/versions/TrackParticleContainer_v1.h"


xAODTrackParticleAuxContainerCnv::
xAODTrackParticleAuxContainerCnv( ISvcLocator* svcLoc )
   : xAODTrackParticleAuxContainerCnvBase( svcLoc ) {

}

xAOD::TrackParticleAuxContainer*
xAODTrackParticleAuxContainerCnv::
createPersistent( xAOD::TrackParticleAuxContainer* trans ) {

   // Load the necessary ROOT class(es):
   static bool dictLoaded = false;
   if( ! dictLoaded ) {
      static const char* NAME =
         "std::vector<ElementLink<DataVector<Trk::Track> > >";
      if( ! TClass::GetClass( NAME ) ) {
         ATH_MSG_ERROR( "Couldn't load dictionary for type: " << NAME );
      }
      dictLoaded = true;
   }

   // Create a copy of the container:
   return SG::copyThinned (*trans, IThinningSvc::instance());
}

xAOD::TrackParticleAuxContainer*
xAODTrackParticleAuxContainerCnv::createTransient() {

  // The known ID(s) for this container:
  static const pool::Guid v1_guid( "C3B01EA0-CA87-4C96-967F-E0F9A75BD370" );
  static const pool::Guid v2_guid( "53031BE5-2156-41E8-B70C-41A1C0572FC5" );

  // Check which version of the container we're reading:
  if ( compareClassGuid( v2_guid ) ){
    // It's the latest version, read it directly:
    xAOD::TrackParticleAuxContainer* blah = poolReadObject< xAOD::TrackParticleAuxContainer >(); 
    return blah;
  } else if( compareClassGuid( v1_guid ) ) {
    
    // The v1 converter: 
    static xAODTrackParticleAuxContainerCnv_v1 converter; 
 
    // Read in the v1 version: 
    std::unique_ptr< xAOD::TrackParticleAuxContainer_v1 > 
      old( poolReadObject< xAOD::TrackParticleAuxContainer_v1 >() ); 
 
    // Return the converted object: 
    return converter.createTransient( old.get(), msg() );     
  }

  // If we didn't recognise the ID:
  throw std::runtime_error( "Unsupported version of "
    "xAOD::TrackParticleAuxContainer found" );
  return 0;
}
