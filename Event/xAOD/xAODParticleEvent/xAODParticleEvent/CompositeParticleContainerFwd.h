// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef XAODPARTICLEEVENT_COMPOSITEPARTICLECONTAINERFWD_H
#define XAODPARTICLEEVENT_COMPOSITEPARTICLECONTAINERFWD_H

// This is a forward declaration for the latest composite particle container type.


// EDM include(s):
#include "AthContainers/DataVector.h"
#include "xAODBase/IParticleContainer.h"

// Local include(s):
#include "xAODParticleEvent/CompositeParticleFwd.h"

// Declare that this is a vector of IParticle-derived objects
DATAVECTOR_BASE_FWD( xAOD::CompositeParticle, xAOD::IParticle );
namespace xAOD {
  typedef DataVector< CompositeParticle > CompositeParticleContainer;
}

#endif // XAODPARTICLEEVENT_COMPOSITEPARTICLECONTAINERFWD_H
