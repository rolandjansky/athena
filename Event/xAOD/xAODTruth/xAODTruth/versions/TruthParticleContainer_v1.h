// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthParticleContainer_v1.h 613511 2014-08-26 15:37:45Z krasznaa $
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

// Declare a CLID for the class for Athena:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TruthParticleContainer_v1, 1237340765, 1 )

#endif // XAODTRUTH_VERSIONS_TRUTHPARTICLECONTAINER_V1_H
