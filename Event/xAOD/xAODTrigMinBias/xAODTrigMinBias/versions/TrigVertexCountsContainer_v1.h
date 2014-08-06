/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef xAODTrigMinBias_TrigVertexCountsCONTAINER_V1_H
#define xAODTrigMinBias_TrigVertexCountsCONTAINER_V1_H

#include "AthContainers/DataVector.h"
#include "xAODTrigMinBias/versions/TrigVertexCounts_v1.h"

namespace xAOD {
  typedef DataVector<xAOD::TrigVertexCounts_v1> TrigVertexCountsContainer_v1;

}

// Setup a CLID for the container... not sure what this is for.
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigVertexCountsContainer_v1 , 1126179625, 1 )

#endif

#endif
