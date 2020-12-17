//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

#ifndef XAOD_ANALYSIS
#   include "../TrackCollectionCnvTool.h"
#   include "../RecTrackParticleContainerCnvTool.h"
#   include "../TrackParticleCnvAlg.h"
#   include "../VertexCnvAlg.h"
#endif // NOT XAOD_ANALYSIS
#include "../TrackParticleCompressorTool.h"

#ifndef XAOD_ANALYSIS
   DECLARE_COMPONENT( xAODMaker::RecTrackParticleContainerCnvTool )
   DECLARE_COMPONENT( xAODMaker::TrackCollectionCnvTool )
   DECLARE_COMPONENT( xAODMaker::TrackParticleCnvAlg )
   DECLARE_COMPONENT( xAODMaker::VertexCnvAlg )
#endif // NOT XAOD_ANALYSIS
DECLARE_COMPONENT( xAODMaker::TrackParticleCompressorTool )
