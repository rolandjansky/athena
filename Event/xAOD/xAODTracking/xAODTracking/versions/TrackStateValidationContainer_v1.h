/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_VERSIONS_TRACKSTATEVALIDATIONCONTAINER_V1_H
#define XAODTRACKING_VERSIONS_TRACKSTATEVALIDATIONCONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTracking/versions/TrackStateValidation_v1.h"

namespace xAOD {
   /// The container is a simple typedef for now
   typedef DataVector< xAOD::TrackStateValidation_v1 > TrackStateValidationContainer_v1;
}

#endif // XAODTRACKING_VERSIONS_TRACKSTATEVALIDATIONCONTAINER_V1_H
