/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODParticleEvent/versions/CompositeParticleAuxContainer_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {

  CompositeParticleAuxContainer_v1::CompositeParticleAuxContainer_v1()
  : AuxContainerBase() {

    // iParticle
    AUX_VARIABLE( px );
    AUX_VARIABLE( py );
    AUX_VARIABLE( pz );
    AUX_VARIABLE( e );

    AUX_VARIABLE( charge );
    AUX_VARIABLE( pdgId );

    AUX_VARIABLE( partLinks );
    AUX_VARIABLE( otherPartLinks );
    AUX_VARIABLE( missingETLink );

  }

} // namespace xAOD
