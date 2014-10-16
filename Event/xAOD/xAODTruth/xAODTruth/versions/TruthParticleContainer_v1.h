// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthParticleContainer_v1.h 622193 2014-10-16 16:08:34Z krasznaa $
#ifndef XAODTRUTH_VERSIONS_TRUTHPARTICLECONTAINER_V1_H
#define XAODTRUTH_VERSIONS_TRUTHPARTICLECONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"
#include "xAODBase/IParticleContainer.h"

// Local include(s):
#include "xAODTruth/versions/TruthParticle_v1.h"
#include "xAODTruth/TruthParticleContainerFwd.h" // Only as long as this is the most recent version...

namespace xAOD {
   // Alias
   typedef DataVector< TruthParticle_v1 > TruthParticleContainer_v1;
}

#endif // XAODTRUTH_VERSIONS_TRUTHPARTICLECONTAINER_V1_H
