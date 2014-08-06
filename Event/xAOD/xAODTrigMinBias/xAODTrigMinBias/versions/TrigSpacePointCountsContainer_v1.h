/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef xAODTrigMinBias_TrigSpacePointCountsCONTAINER_V1_H
#define xAODTrigMinBias_TrigSpacePointCountsCONTAINER_V1_H

#include "AthContainers/DataVector.h"
#include "xAODTrigMinBias/versions/TrigSpacePointCounts_v1.h"

namespace xAOD {
  typedef DataVector<xAOD::TrigSpacePointCounts_v1> TrigSpacePointCountsContainer_v1;

}

// Setup a CLID for the container... not sure what this is for.
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigSpacePointCountsContainer_v1 , 1208170683 , 1 )

#endif

#endif
