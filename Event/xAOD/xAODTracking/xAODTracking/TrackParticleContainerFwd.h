// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_TRACKPARTICLECONTAINERFWD_H
#define XAODTRACKING_TRACKPARTICLECONTAINERFWD_H

// This is a forward declaration for the latest track particle container type.

// EDM include(s):
#include "AthContainers/DataVector.h"
#include "xAODBase/IParticleContainer.h"


// Local include(s):
#include "xAODTracking/TrackParticleFwd.h"


// Declare that this is a vector of IParticle-derived objects
DATAVECTOR_BASE_FWD( xAOD::TrackParticle, xAOD::IParticle );
namespace xAOD {
   /// Provide a forward declaration for TrackParticleContainer
   typedef DataVector< TrackParticle> TrackParticleContainer;
}

#endif // XAODTRACKING_TRACKPARTICLECONTAINERFWD_H
