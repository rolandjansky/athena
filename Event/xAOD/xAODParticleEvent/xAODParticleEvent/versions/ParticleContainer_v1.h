// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ParticleContainer_v1.h 608484 2014-07-24 17:10:54Z kkoeneke $
#ifndef XAODPARTICLEEVENT_VERSIONS_PARTICLECONTAINER_V1_H
#define XAODPARTICLEEVENT_VERSIONS_PARTICLECONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"
#include "xAODBase/IParticleContainer.h"

// Local include(s):
#include "xAODParticleEvent/versions/Particle_v1.h"
#include "xAODParticleEvent/ParticleContainerFwd.h"

// To complete the DATAVECTOR_BASE macro:
template struct DataVector_detail::DVLEltBaseInit< xAOD::Particle_v1 >;

namespace xAOD {
   /// Define the particle container as a simple DataVector
   typedef DataVector< Particle_v1 > ParticleContainer_v1;
}

// Set up a CLID for the container:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::ParticleContainer_v1, 1141859363, 1 )
#endif // not XAOD_STANDALONE

#endif // XAODPARTICLEEVENT_VERSIONS_PARTICLECONTAINER_V1_H
