/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODPFLOW_TRACKCALOCLUSTER_H
#define XAODPFLOW_TRACKCALOCLUSTER_H

// Local include(s):
#include "xAODPFlow/versions/TrackCaloCluster_v1.h"

namespace xAOD {
  /// Reference the current persistent version:
  typedef TrackCaloCluster_v1 TrackCaloCluster;
}

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrackCaloCluster, 138661214, 1 )

#endif // XAODPFLOW_TRACKCALOCLUSTER_H