/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_VERSIONS_MEASUREMENTSTATEONSURFACECONTAINER_V1_H
#define XAODTRACKING_VERSIONS_MEASUREMENTSTATEONSURFACECONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"

// Local include(s):
#include "xAODTracking/versions/MeasurementStateOnSurface_v1.h"

namespace xAOD {
   /// The container is a simple typedef for now
   typedef DataVector< xAOD::MeasurementStateOnSurface_v1 > MeasurementStateOnSurfaceContainer_v1;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::MeasurementStateOnSurfaceContainer_v1 , 1204230301 , 1 )

#endif // XAODTRACKING_VERSIONS_MEASUREMENTSTATEONSURFACECONTAINER_V1_H
