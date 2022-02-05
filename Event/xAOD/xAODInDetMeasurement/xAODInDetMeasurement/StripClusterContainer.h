/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODINDETMEASUREMENT_STRIPCLUSTERCONTAINER_H
#define XAODINDETMEASUREMENT_STRIPCLUSTERCONTAINER_H

#include "xAODInDetMeasurement/StripCluster.h"
#include "xAODInDetMeasurement/versions/StripClusterContainer_v1.h"

/// Namespace holding all the xAOD EDM classes
namespace xAOD {
    /// Define the version of the strip cluster container
    typedef StripClusterContainer_v1 StripClusterContainer;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::StripClusterContainer, 1323510650, 1 )

#endif // XAODINDETMEASUREMENT_STRIPCLUSTERCONTAINER_H

