// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODPARTICLEEVENT_PARTICLECONTAINERFWD_H
#define XAODPARTICLEEVENT_PARTICLECONTAINERFWD_H

// This is a forward declaration for the latest particle container type.


// EDM include(s):
#include "AthContainers/DataVector.h"
#include "xAODBase/IParticleContainer.h"

// Local include(s):
#include "xAODParticleEvent/ParticleFwd.h"

// Declare that this is a vector of IParticle-derived objects
DATAVECTOR_BASE_FWD( xAOD::Particle, xAOD::IParticle );
namespace xAOD {
  typedef DataVector< Particle > ParticleContainer;
}

#endif // XAODPARTICLEEVENT_PARTICLECONTAINERFWD_H
