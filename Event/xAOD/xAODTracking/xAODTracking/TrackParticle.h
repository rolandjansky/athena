/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_TRACKPARTICLE_H
#define XAODTRACKING_TRACKPARTICLE_H

// Local include(s):
#include "versions/TrackParticle_v1.h"

namespace xAOD {
  /// Reference the current persistent version:
  typedef TrackParticle_v1 TrackParticle;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrackParticle, 159016719, 1 )

#endif // XAODTRACKING_TRACKPARTICLE_H
