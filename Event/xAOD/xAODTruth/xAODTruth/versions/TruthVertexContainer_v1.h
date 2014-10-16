// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TruthVertexContainer_v1.h 622193 2014-10-16 16:08:34Z krasznaa $
#ifndef XAODTRUTH_VERSIONS_TRUTHVERTEXCONTAINER_V1_H
#define XAODTRUTH_VERSIONS_TRUTHVERTEXCONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTruth/versions/TruthVertex_v1.h"
#include "xAODTruth/TruthVertexContainerFwd.h" // Only as long as this is the most recent version...

namespace xAOD {
   // Alias
   typedef DataVector< TruthVertex_v1 > TruthVertexContainer_v1;
}

#endif // XAODTRUTH_VERSIONS_TRUTHVERTEXCONTAINER_V1_H
