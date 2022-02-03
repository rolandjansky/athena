/*                                                                                                                                                                                                                                          
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration                                                                                                                                                                   
*/


#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

#include "AthenaKernel/getMessageSvc.h"

#include "eflowRec/PFSubtractionStatusSetter.h"
#include "eflowRec/eflowCaloObject.h"
#include "eflowRec/eflowRecCluster.h"
#include "eflowRec/eflowTrackClusterLink.h"

#include <numeric>

PFSubtractionStatusSetter::PFSubtractionStatusSetter() : AthMessaging(Gaudi::svcLocator()->service<IMessageSvc>("MessageSvc"),
                  "PFSubtractionStatusSetter"){}

void PFSubtractionStatusSetter::markSubtractionStatus(const std::vector<std::pair<xAOD::CaloCluster*, bool> >& clusterList, std::vector<std::pair<float,float> >& clusterSubtractedEnergyRatios, eflowCaloObject& thisEflowCaloObject, unsigned int trackIndex){
  //An eflowCaloObject may have one cluster and N tracks, and then one would have N eflowTrackClusterLink* for each track-cluster pair
  //Hence there can be more entries in the track cluster link list due to duplication

  ATH_MSG_DEBUG("Executing markSubtractionStatus and have clusterList of size " << clusterList.size() << ", clusterSubtractedEnergyRatios of size " << clusterSubtractedEnergyRatios.size() << " and trackIndex of " << trackIndex);  

  const std::vector<std::pair<eflowTrackClusterLink*, std::pair<float,float> > >& matchedTrackList = thisEflowCaloObject.efRecLink();

  unsigned int clusCounter = 0;
  for (auto thisClusterPair : clusterList){
    xAOD::CaloCluster* thisCluster = thisClusterPair.first;
    ATH_MSG_DEBUG("Cluster with e and eta " << thisCluster->e() << " and " << thisCluster->eta());
    unsigned int counter = 0;
    for (const auto & thisTrackClusterLinkPair : matchedTrackList){

      if (trackIndex != thisTrackClusterLinkPair.first->getTrack()->getTrack()->index()){
        counter++;
        continue;
      }

      ATH_MSG_DEBUG("Track with e and eta " << thisTrackClusterLinkPair.first->getTrack()->getTrack()->e() << " and " << thisTrackClusterLinkPair.first->getTrack()->getTrack()->eta());

      //if the subtraction status is already true, then no need to update it
      if (!std::isnan(thisTrackClusterLinkPair.second.first)) {
    	  counter++;
	      continue;
      }

      ATH_MSG_DEBUG("Not already subtracted");

      //eflowTrackCluster link returns an eflowRecCluster pointer, which in turn returns an xAOD:;CaloCluster* pointer
      xAOD::CaloCluster* thisMatchedTrackCluster = (thisTrackClusterLinkPair.first)->getCluster()->getCluster();
      //Now we can do a pointer comparison of the two clusters to see if we have a match
      ATH_MSG_DEBUG("Comparing clusters with energies " << thisCluster->e() << " and " << thisMatchedTrackCluster->e());
      if (thisCluster == thisMatchedTrackCluster){      
        ATH_MSG_DEBUG("Passed cluster energy check");
	      if (thisClusterPair.second && !std::isnan(clusterSubtractedEnergyRatios[clusCounter].first)) {
          ATH_MSG_DEBUG("Will set ratio to be " << clusterSubtractedEnergyRatios[clusCounter].first);
          thisEflowCaloObject.setTrackClusterLinkSubtractionStatus(counter, clusterSubtractedEnergyRatios[clusCounter]);        
        }
      }//if have a match of the cluster
      counter++;
    }//loop on track cluster link pairs
    clusCounter++;
  }//loop on cluster pair list

}

void PFSubtractionStatusSetter::markAllTracksAnnihStatus(eflowCaloObject& thisEflowCaloObject){

  ATH_MSG_DEBUG("Executing markAllTracksAnnihStatus");

  const std::vector<std::pair<eflowTrackClusterLink*, std::pair<float,float> > >& matchedTrackList = thisEflowCaloObject.efRecLink();
  
  auto accumulateTrackEnergy = [](float sum, std::pair<eflowTrackClusterLink*, std::pair<float,float> > thisPair){ return sum + thisPair.first->getTrack()->getTrack()->e();};
  float totalTrackE = std::accumulate(matchedTrackList.begin(),matchedTrackList.end(),0.0,accumulateTrackEnergy);

  unsigned int counter = 0;
  for (const auto & thisTrackClusterLinkPair : matchedTrackList){
    ATH_MSG_DEBUG("Track with e and eta " << thisTrackClusterLinkPair.first->getTrack()->getTrack()->e() << " and " << thisTrackClusterLinkPair.first->getTrack()->getTrack()->eta());
    ATH_MSG_DEBUG("will annihilate cluster with e and eta " << thisTrackClusterLinkPair.first->getCluster()->getCluster()->e() << " and " << thisTrackClusterLinkPair.first->getCluster()->getCluster()->eta());
    float trackEFraction = thisTrackClusterLinkPair.first->getTrack()->getTrack()->e()/totalTrackE;
    //We set the subtracted energy to the cluster energy multiplied by the fraction of total matched track energy belonging to this track
    //whilst the ratio of new to old enegry is set to 0.0
    thisEflowCaloObject.setTrackClusterLinkSubtractionStatus(counter, std::pair(0.0,trackEFraction*thisTrackClusterLinkPair.first->getCluster()->getCluster()->e()));  
    counter++;
  }
  
}

