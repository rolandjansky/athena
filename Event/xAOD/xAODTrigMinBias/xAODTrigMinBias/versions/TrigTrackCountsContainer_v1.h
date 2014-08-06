/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef xAODTrigMinBias_TrigTrackCountsCONTAINER_V1_H
#define xAODTrigMinBias_TrigTrackCountsCONTAINER_V1_H

#include "AthContainers/DataVector.h"
#include "xAODTrigMinBias/versions/TrigTrackCounts_v1.h"

namespace xAOD {
  typedef DataVector<xAOD::TrigTrackCounts_v1> TrigTrackCountsContainer_v1;

}

// Setup a CLID for the container... not sure what this is for.
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigTrackCountsContainer_v1 , 1327042225 , 1 )

#endif

#endif
