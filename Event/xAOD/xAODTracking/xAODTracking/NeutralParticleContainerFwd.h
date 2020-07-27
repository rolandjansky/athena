// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_NEUTRALPARTICLECONTAINERFWD_H
#define XAODTRACKING_NEUTRALPARTICLECONTAINERFWD_H

// This is a forward declaration for the latest neutral particle container type.

// EDM include(s):
#include "AthContainers/DataVector.h"
#include "xAODBase/IParticleContainer.h"

// Local include(s):
#include "xAODTracking/NeutralParticleFwd.h"

// Declare that this is a vector of IParticle-derived objects
DATAVECTOR_BASE_FWD( xAOD::NeutralParticle, xAOD::IParticle );
namespace xAOD {
   /// Provide a forward declaration for NeutralParticleContainer
   typedef DataVector< NeutralParticle> NeutralParticleContainer;
}

#endif // XAODTRACKING_NEUTRALPARTICLECONTAINERFWD_H
