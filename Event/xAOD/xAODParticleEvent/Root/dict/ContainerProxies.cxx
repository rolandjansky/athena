/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ContainerProxies.cxx 599909 2014-06-02 14:25:44Z kkoeneke $

// EDM include(s):
#include "xAODCore/AddDVProxy.h"

// Local include(s):
#include "xAODParticleEvent/versions/ParticleContainer_v1.h"
#include "xAODParticleEvent/versions/CompositeParticleContainer_v1.h"
// #include "xAODParticleEvent/versions/IParticleLinkContainer_v1.h"

// Set up the collection proxies:
ADD_NS_DV_PROXY( xAOD, ParticleContainer_v1 );
ADD_NS_DV_PROXY( xAOD, CompositeParticleContainer_v1 );
// ADD_NS_DV_PROXY( xAOD, IParticleLinkContainer_v1 );
