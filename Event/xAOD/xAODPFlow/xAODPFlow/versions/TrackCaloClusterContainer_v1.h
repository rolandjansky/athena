#ifndef XAODPFLOW_VERSIONS_TRACKCALOCLUSTERCONTAINER_V1_H
#define XAODPFLOW_VERSIONS_TRACKCALOCLUSTERCONTAINER_V1_H
 
#include <stdint.h>
 
// Core include(s):
#include "AthContainers/DataVector.h"
#include "xAODBase/IParticleContainer.h"
 
// Local include(s):
#include "xAODPFlow/versions/TrackCaloCluster_v1.h"

// Declare IParticle as a base class of CaloCluster_v1:
DATAVECTOR_BASE( xAOD::TrackCaloCluster_v1, xAOD::IParticle );
 
namespace xAOD {
   /// The container is a simple typedef for now
   typedef DataVector< xAOD::TrackCaloCluster_v1 > TrackCaloClusterContainer_v1;
}
 
#endif // XAODPFLOW_VERSIONS_TRACKCALOCLUSTERCONTAINER_V1_H