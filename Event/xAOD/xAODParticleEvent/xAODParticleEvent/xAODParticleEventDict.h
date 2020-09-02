// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODPARTICLEEVENT_XAODPARTICLEEVENTDICT_H
#define XAODPARTICLEEVENT_XAODPARTICLEEVENTDICT_H

// Local include(s).
#include "xAODParticleEvent/Particle.h"
#include "xAODParticleEvent/ParticleContainer.h"
#include "xAODParticleEvent/ParticleAuxContainer.h"
#include "xAODParticleEvent/versions/Particle_v1.h"
#include "xAODParticleEvent/versions/ParticleContainer_v1.h"
#include "xAODParticleEvent/versions/ParticleAuxContainer_v1.h"

#include "xAODParticleEvent/CompositeParticle.h"
#include "xAODParticleEvent/CompositeParticleContainer.h"
#include "xAODParticleEvent/CompositeParticleAuxContainer.h"
#include "xAODParticleEvent/versions/CompositeParticle_v1.h"
#include "xAODParticleEvent/versions/CompositeParticleContainer_v1.h"
#include "xAODParticleEvent/versions/CompositeParticleAuxContainer_v1.h"

// EDM include(s).
#include "xAODCore/tools/DictHelpers.h"

// Instantiate all necessary types for the dictionary.
namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODPARTICLEEVENT {
      // Types for which dictionaries are built.
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, ParticleContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD,
                                           CompositeParticleContainer_v1 );
      // Type(s) needed for the dictionary generation to succeed.
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, IParticleContainer );
   };
}

#endif // XAODPARTICLEEVENT_XAODPARTICLEEVENTDICT_H
