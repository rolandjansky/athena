/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_NEUTRALPARTICLECONTAINER_H
#define XAODTRACKING_NEUTRALPARTICLECONTAINER_H
 
// Local include(s):
#include "xAODTracking/NeutralParticle.h"
#include "xAODTracking/versions/NeutralParticleContainer_v1.h"
 
namespace xAOD {
   /// Definition of the current "NeutralParticle container version"
   typedef NeutralParticleContainer_v1 NeutralParticleContainer;
}
 
// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::NeutralParticleContainer, 1293931985, 1 )

#endif // XAODTRACKING_NEUTRALPARTICLECONTAINER_H
