// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ParticleAuxContainer.h 637086 2014-12-19 16:02:19Z kkoeneke $
#ifndef XAODPARTICLEEVENT_PARTICLEAUXCONTAINER_H
#define XAODPARTICLEEVENT_PARTICLEAUXCONTAINER_H

// Local include(s):
#include "xAODParticleEvent/versions/ParticleAuxContainer_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
   /// Define the latest version of the ParticleAuxContainer class
   typedef ParticleAuxContainer_v1 ParticleAuxContainer;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::ParticleAuxContainer, 1331351342, 1 )

#endif // XAODPARTICLEEVENT_PARTICLEAUXCONTAINER_H
