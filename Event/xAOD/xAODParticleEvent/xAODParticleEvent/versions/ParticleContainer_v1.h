// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODPARTICLEEVENT_VERSIONS_PARTICLECONTAINER_V1_H
#define XAODPARTICLEEVENT_VERSIONS_PARTICLECONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODParticleEvent/versions/Particle_v1.h"

namespace xAOD {
   /// Define the particle container as a simple DataVector
   typedef DataVector< Particle_v1 > ParticleContainer_v1;
}

#endif // XAODPARTICLEEVENT_VERSIONS_PARTICLECONTAINER_V1_H
