/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODPFLOW_TRACKCALOCLUSTERCONTAINER_H
#define XAODPFLOW_TRACKCALOCLUSTERCONTAINER_H
 
// Local include(s):
#include "xAODPFlow/TrackCaloCluster.h"
#include "xAODPFlow/versions/TrackCaloClusterContainer_v1.h"
 
namespace xAOD {
   /// Definition of the current "TrackCaloCluster container version"
   typedef TrackCaloClusterContainer_v1 TrackCaloClusterContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrackCaloClusterContainer, 1138314170, 1 )
 
#endif // XAODPFLOW_TRACKCALOCLUSTERCONTAINER_H