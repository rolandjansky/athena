/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODINDETMEASUREMENT_VERSION_PIXELCLUSTERCONTAINER_V1_H
#define XAODINDETMEASUREMENT_VERSION_PIXELCLUSTERCONTAINER_V1_H

// Core include(s):
#include "AthContainers/DataVector.h"
#include "xAODInDetMeasurement/versions/PixelCluster_v1.h"

namespace xAOD {
    /// The container is a simple typedef for now
    typedef DataVector< xAOD::PixelCluster_v1 > PixelClusterContainer_v1;
}

#endif // XAODINDETMEASUREMENT_VERSION_PIXELCLUSTERCONTAINER_V1_H
