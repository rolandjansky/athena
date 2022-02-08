/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODINDETMEASUREMENT_PIXELCLUSTER_H
#define XAODINDETMEASUREMENT_PIXELCLUSTER_H

#include "xAODInDetMeasurement/versions/PixelCluster_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
    /// Define the version of the pixel cluster class
    typedef PixelCluster_v1 PixelCluster;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::PixelCluster, 229103880, 1 )

#endif // XAODINDETMEASUREMENT_PIXELCLUSTER_H

