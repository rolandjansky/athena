/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODTRACKING_VERSIONS_TRACKSTATECONTAINER_V1_H
#define XAODTRACKING_VERSIONS_TRACKSTATECONTAINER_V1_H

#include "AthContainers/DataVector.h"

#include "xAODTracking/versions/TrackState_v1.h"
namespace xAOD {
    typedef DataVector<xAOD::TrackState_v1> TrackStateContainer_v1;
}

#endif