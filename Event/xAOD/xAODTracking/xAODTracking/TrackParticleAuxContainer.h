/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_TRACKPARTICLEAUXCONTAINER_H
#define XAODTRACKING_TRACKPARTICLEAUXCONTAINER_H
 
// Local include(s):
#include "xAODTracking/versions/TrackParticleAuxContainer_v3.h"
 
namespace xAOD {
   /// Definition of the current TrackParticle auxiliary container
   ///
   /// All reconstruction code should attach the typedefed auxiliary
   /// container to the xAOD::TrackParticleContainer, so it will be easy to change
   /// the container type as we get new I/O technologies for these
   /// objects.
   ///
   typedef TrackParticleAuxContainer_v3 TrackParticleAuxContainer;
}

// Set up a CLID and StoreGate inheritance for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrackParticleAuxContainer, 1209269198, 1 )


#endif // XAODTRACKING_TRACKPARTICLEAUXCONTAINER_H
