// -*- C++ -*-

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRUTH_VERSIONS_TRUTHPILEUPEVENTCONTAINER_V1_H
#define XAODTRUTH_VERSIONS_TRUTHPILEUPEVENTCONTAINER_V1_H

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTruth/versions/TruthPileupEvent_v1.h"


namespace xAOD {
   /// Alias declaring the container type
   typedef DataVector< TruthPileupEvent_v1 > TruthPileupEventContainer_v1;
}

#endif // XAODTRUTH_VERSIONS_TRUTHPILEUPEVENTCONTAINER_V1_H
