/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODINDETMEASUREMENT_PIXELCLUSTERCONTAINER_H
#define XAODINDETMEASUREMENT_PIXELCLUSTERCONTAINER_H

#include "xAODInDetMeasurement/PixelCluster.h"
#include "xAODInDetMeasurement/versions/PixelClusterContainer_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
    /// Define the version of the pixel cluster container
    typedef PixelClusterContainer_v1 PixelClusterContainer;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::PixelClusterContainer, 1318540388, 1 )

#endif // XAODINDETMEASUREMENT_PIXELCLUSTERCONTAINER_H

