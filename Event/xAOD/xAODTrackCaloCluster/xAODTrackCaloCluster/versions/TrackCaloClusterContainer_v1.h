#ifndef XAODTRACKCALOCLUSTER_VERSIONS_TRACKCALOCLUSTERCONTAINER_V1_H
#define XAODTRACKCALOCLUSTER_VERSIONS_TRACKCALOCLUSTERCONTAINER_V1_H
 
#include <stdint.h>
 
// Core include(s):
#include "AthContainers/DataVector.h"
 
// Local include(s):
#include "xAODTrackCaloCluster/versions/TrackCaloCluster_v1.h"
 
namespace xAOD {
   /// The container is a simple typedef for now
   typedef DataVector< xAOD::TrackCaloCluster_v1 > TrackCaloClusterContainer_v1;
}
 
#endif // XAODTRACKCALOCLUSTER_VERSIONS_TRACKCALOCLUSTERCONTAINER_V1_H