#ifndef XAODPFLOW_TRACKCALOCLUSTERAUXCONTAINER_H
#define XAODPFLOW_TRACKCALOCLUSTERAUXCONTAINER_H
 
// Local include(s):
#include "xAODPFlow/versions/TrackCaloClusterAuxContainer_v1.h"
 
namespace xAOD {
   /// Definition of the current TrackCaloCluster auxiliary container
   ///
   /// All reconstruction code should attach the typedefed auxiliary
   /// container to the xAOD::TrackCaloCluster, so it will be easy to change
   /// the container type as we get new I/O technologies for these
   /// objects.
   ///
   typedef TrackCaloClusterAuxContainer_v1 TrackCaloClusterAuxContainer;
}

// Set up a CLID and StoreGate inheritance for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::TrackCaloClusterAuxContainer, 1284131157, 1 )
 
#endif // XAODPFLOW_TRACKCALOCLUSTERAUXCONTAINER_H
