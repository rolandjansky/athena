/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTrackParticleAuxContainerCnv.cxx 609811 2014-08-01 15:47:00Z ssnyder $

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

   // Check if there's a thinning service:
   IThinningSvc* thinSvc = IThinningSvc::instance();

   // Need to change createPersistent in converter base to take a const
   // pointer as a return.
   return SG::copyThinned (*trans, thinSvc);
}

xAOD::TrackParticleAuxContainer*
xAODTrackParticleAuxContainerCnv::createTransient() {

   // The known ID(s) for this container:
   static const pool::Guid v1_guid( "C3B01EA0-CA87-4C96-967F-E0F9A75BD370" );

   // Check which version of the container we're reading:
   if( compareClassGuid( v1_guid ) ) {
      // It's the latest version, read it directly:
      return poolReadObject< xAOD::TrackParticleAuxContainer >();
   }

   // If we didn't recognise the ID:
   throw std::runtime_error( "Unsupported version of "
                             "xAOD::TrackParticleAuxContainer found" );
   return 0;
}
