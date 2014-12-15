/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_NEUTRALPARTICLE_H
#define XAODTRACKING_NEUTRALPARTICLE_H

// Local include(s):
#include "versions/NeutralParticle_v1.h"

namespace xAOD {
  /// Reference the current persistent version:
  typedef NeutralParticle_v1 NeutralParticle;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::NeutralParticle, 33702537, 1 )

#endif // XAODTRACKING_NEUTRALPARTICLE_H
