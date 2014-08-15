/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRKVALHISTUTILS_TRKVALHISTUTILITIES
#define TRKVALHISTUTILS_TRKVALHISTUTILITIES

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackingPrimitives.h"

namespace Trk{

// int is used for info because it refers to SummaryTpe and MuonSummaryType at the same time
uint8_t RetrieveHitInfo(const xAOD::TrackParticle& trkprt, const xAOD::SummaryType& info);
}

#endif

