/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODINDETMEASUREMENT_STRIPCLUSTER_H
#define XAODINDETMEASUREMENT_STRIPCLUSTER_H

#include "xAODInDetMeasurement/versions/StripCluster_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
    /// Define the version of the strip cluster class
    typedef StripCluster_v1 StripCluster;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::StripCluster, 176991126, 1 )

#endif // XAODINDETMEASUREMENT_STRIPCLUSTER_H

