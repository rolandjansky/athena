/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):
#include "xAODCore/AddDVProxy.h"

// Local include(s):
#include "xAODTracking/versions/TrackParticleContainer_v1.h"
#include "xAODTracking/versions/NeutralParticleContainer_v1.h"
#include "xAODTracking/versions/VertexContainer_v1.h"
#include "xAODTracking/versions/PrepRawDataContainer_v1.h"
#include "xAODTracking/versions/MeasurementStateOnSurfaceContainer_v1.h"

// Set up the collection proxies:
ADD_NS_DV_PROXY( xAOD, TrackParticleContainer_v1 );
ADD_NS_DV_PROXY( xAOD, NeutralParticleContainer_v1 );
ADD_NS_DV_PROXY( xAOD, VertexContainer_v1 );
ADD_NS_DV_PROXY( xAOD, PrepRawDataContainer_v1 );
ADD_NS_DV_PROXY( xAOD, MeasurementStateOnSurfaceContainer_v1 );
