// -*- C++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRUTH_VERSIONS_TRUTHPARTICLECONTAINER_V1_H
#define XAODTRUTH_VERSIONS_TRUTHPARTICLECONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"
#include "xAODBase/IParticleContainer.h"

// Local include(s):
#include "xAODTruth/versions/TruthParticle_v1.h"

namespace xAOD {
   // Alias
   typedef DataVector< TruthParticle_v1 > TruthParticleContainer_v1;
}

#endif // XAODTRUTH_VERSIONS_TRUTHPARTICLECONTAINER_V1_H
