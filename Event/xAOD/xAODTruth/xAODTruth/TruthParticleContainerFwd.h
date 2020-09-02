// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRUTH_TRUTHPARTICLECONTAINERFWD_H
#define XAODTRUTH_TRUTHPARTICLECONTAINERFWD_H

// EDM include(s):
#include "AthContainers/DataVector.h"
#include "xAODBase/IParticleContainer.h"

// Local include(s):
#include "xAODTruth/TruthParticleFwd.h"

// Declare that this is a vector of IParticle-derived objects
DATAVECTOR_BASE_FWD( xAOD::TruthParticle, xAOD::IParticle );

namespace xAOD {
   /// Provide a forward declaration for TruthParticleContainer
   typedef DataVector< TruthParticle > TruthParticleContainer;
}

#endif // XAODTRUTH_TRUTHPARTICLECONTAINERFWD_H
