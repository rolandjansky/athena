/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_VERSIONS_TRACKPARAMETERSCONTAINER_V1_H
#define XAODTRACKING_VERSIONS_TRACKPARAMETERSCONTAINER_V1_H

#include "AthContainers/DataVector.h"

#include "xAODTracking/versions/TrackParameters_v1.h"
namespace xAOD {
    typedef DataVector<xAOD::TrackParameters_v1> TrackParametersContainer_v1;
}


#endif