/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_NEUTRALPARTICLEAUXCONTAINER_H
#define XAODTRACKING_NEUTRALPARTICLEAUXCONTAINER_H
 
// Local include(s):
#include "xAODTracking/versions/NeutralParticleAuxContainer_v1.h"
 
namespace xAOD {
   /// Definition of the current NeutralParticle auxiliary container
   ///
   /// All reconstruction code should attach the typedefed auxiliary
   /// container to the xAOD::NeutralParticleContainer, so it will be easy to change
   /// the container type as we get new I/O technologies for these
   /// objects.
   ///
   typedef NeutralParticleAuxContainer_v1 NeutralParticleAuxContainer;
}
 
#endif // XAODTRACKING_NEUTRALPARTICLEAUXCONTAINER_H
