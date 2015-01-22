// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ParticleContainer.h 637086 2014-12-19 16:02:19Z kkoeneke $
#ifndef XAODPARTICLEEVENT_PARTICLECONTAINER_H
#define XAODPARTICLEEVENT_PARTICLECONTAINER_H

// Local include(s):
#include "xAODParticleEvent/versions/ParticleContainer_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
   /// Define the latest version of the particle class
   typedef ParticleContainer_v1 ParticleContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::ParticleContainer, 1141859363, 1 )

#endif // XAODPARTICLEEVENT_PARTICLECONTAINER_H
