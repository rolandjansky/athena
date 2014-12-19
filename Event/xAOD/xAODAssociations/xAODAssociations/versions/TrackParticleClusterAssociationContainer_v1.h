/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODASSOCIATIONS_VERSIONS_TRACKPARTICLECLUSTERASSOCIATIONCONTAINER_V1_H
#define XAODASSOCIATIONS_VERSIONS_TRACKPARTICLECLUSTERASSOCIATIONCONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODAssociations/versions/TrackParticleClusterAssociation_v1.h"

namespace xAOD {
   /// The container is a simple typedef for now
   typedef DataVector< xAOD::TrackParticleClusterAssociation_v1 > TrackParticleClusterAssociationContainer_v1;
}

#endif // XAODASSOCIATIONS_VERSIONS_TRACKPARTICLECLUSTERASSOCIATIONCONTAINER_V1_H
