// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CompositeParticleContainer.h 637086 2014-12-19 16:02:19Z kkoeneke $
#ifndef XAODPARTICLEEVENT_COMPOSITEPARTICLECONTAINER_H
#define XAODPARTICLEEVENT_COMPOSITEPARTICLECONTAINER_H

// Local include(s):
#include "xAODParticleEvent/versions/CompositeParticleContainer_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
   /// Define the latest version of the CompositeParticle class
   typedef CompositeParticleContainer_v1 CompositeParticleContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CompositeParticleContainer, 1213067199, 1 )

#endif // XAODPARTICLEEVENT_COMPOSITEPARTICLECONTAINER_H
