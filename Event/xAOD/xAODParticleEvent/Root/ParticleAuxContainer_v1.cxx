/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODParticleEvent/versions/ParticleAuxContainer_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {

  ParticleAuxContainer_v1::ParticleAuxContainer_v1()
  : AuxContainerBase() {

    // iParticle
    AUX_VARIABLE( px );
    AUX_VARIABLE( py );
    AUX_VARIABLE( pz );
    AUX_VARIABLE( e );

    AUX_VARIABLE( charge );
    AUX_VARIABLE( pdgId );

  }

} // namespace xAOD
