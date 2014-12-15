/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_TRACKPARTICLECONTAINER_H
#define XAODTRACKING_TRACKPARTICLECONTAINER_H
 
// Local include(s):
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/versions/TrackParticleContainer_v1.h"
 
namespace xAOD {
   /// Definition of the current "TrackParticle container version"
   typedef TrackParticleContainer_v1 TrackParticleContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrackParticleContainer, 1287425431, 1 )

#endif // XAODTRACKING_TRACKPARTICLECONTAINER_H
