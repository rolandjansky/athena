#include "eflowRec/PFSubtractionStatusSetter.h"
#include "eflowRec/eflowCaloObject.h"
#include "eflowRec/eflowRecCluster.h"
#include "eflowRec/eflowTrackClusterLink.h"

void PFSubtractionStatusSetter::markSubtractionStatus(const std::vector<std::pair<xAOD::CaloCluster*, bool> >& clusterList, eflowCaloObject& thisEflowCaloObject){
  //An eflowCaloObject may have one cluster and N tracks, and then one would have N eflowTrackClusterLink* for each track-cluster pair
  //Hence there can be more entries in the track cluster link list due to duplication

  const std::vector<std::pair<eflowTrackClusterLink*, bool> >& matchedTrackList = thisEflowCaloObject.efRecLink();

  for (auto thisClusterPair : clusterList){
    xAOD::CaloCluster* thisCluster = thisClusterPair.first;
    unsigned int counter = 0;
    for (auto& thisTrackClusterLinkPair : matchedTrackList){
      //if the subtraction status is already true, then no need to update it
      if (true == thisTrackClusterLinkPair.second) {
	counter++;
	continue;
      }
      //eflowTrackCluster link returns an eflowRecCluster pointer, which in turn returns an xAOD:;CaloCluster* pointer
      xAOD::CaloCluster* thisMatchedTrackCluster = (thisTrackClusterLinkPair.first)->getCluster()->getCluster();
      //Now we can do a floating point comparison of the energy to check which cluster we have
      if (fabs(thisCluster->e() - thisMatchedTrackCluster->e()) < 0.0001){
	if (true == thisClusterPair.second) thisEflowCaloObject.setTrackClusterLinkSubtractionStatus(counter, true);
      }//if have a match of the cluster
      counter++;
    }//loop on track cluster link pairs
    
  }//loop on cluster pair list
}

