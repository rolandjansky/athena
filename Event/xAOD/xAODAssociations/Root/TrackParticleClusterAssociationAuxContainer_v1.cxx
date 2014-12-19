/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "xAODAssociations/versions/TrackParticleClusterAssociationAuxContainer_v1.h"
 
namespace xAOD {
 
  TrackParticleClusterAssociationAuxContainer_v1::TrackParticleClusterAssociationAuxContainer_v1()
    : AuxContainerBase() {

    AUX_VARIABLE( trackParticleLink );
#ifndef XAOD_ANALYSIS
    AUX_VARIABLE( caloClusterLinks );
#endif

  }

} // namespace xAOD
