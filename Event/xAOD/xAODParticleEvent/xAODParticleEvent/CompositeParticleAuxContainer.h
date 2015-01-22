// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CompositeParticleAuxContainer.h 637086 2014-12-19 16:02:19Z kkoeneke $
#ifndef XAODPARTICLEEVENT_COMPOSITEPARTICLEAUXCONTAINER_H
#define XAODPARTICLEEVENT_COMPOSITEPARTICLEAUXCONTAINER_H

// Local include(s):
#include "xAODParticleEvent/versions/CompositeParticleAuxContainer_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
   /// Define the latest version of the CompositeParticleAuxContainer class
   typedef CompositeParticleAuxContainer_v1 CompositeParticleAuxContainer;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CompositeParticleAuxContainer, 1205246726, 1 )

#endif // XAODPARTICLEEVENT_COMPOSITEPARTICLEAUXCONTAINER_H
