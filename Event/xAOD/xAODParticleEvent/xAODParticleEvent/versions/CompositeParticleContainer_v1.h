// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODPARTICLEEVENT_VERSIONS_COMPOSITEPARTICLECONTAINER_V1_H
#define XAODPARTICLEEVENT_VERSIONS_COMPOSITEPARTICLECONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODParticleEvent/versions/CompositeParticle_v1.h"

namespace xAOD {
   /// Define the composite particle container as a simple DataVector
   typedef DataVector< CompositeParticle_v1 > CompositeParticleContainer_v1;
}

#endif // XAODPARTICLEEVENT_VERSIONS_COMPOSITEPARTICLECONTAINER_V1_H
