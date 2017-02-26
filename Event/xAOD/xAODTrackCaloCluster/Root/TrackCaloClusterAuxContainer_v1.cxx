// $Id: TrackCaloClusterAuxContainer_v1.cxx $

// Local include(s):
#include "xAODTrackCaloCluster/versions/TrackCaloClusterAuxContainer_v1.h"
 
namespace xAOD {
 
   TrackCaloClusterAuxContainer_v1::TrackCaloClusterAuxContainer_v1()
      : AuxContainerBase() {
	
      }
   
   void TrackCaloClusterAuxContainer_v1::dump() const {
     std::cout<<" Dumping TrackCaloClusterAuxContainer_v1"<<std::endl;
   }
   
} // namespace xAOD