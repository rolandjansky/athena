/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*
 * eflowCaloObjectMaker.cxx
 *
 *  Created on: Apr 16, 2015
 *      Author: zhangrui
 */

#include "eflowRec/eflowCaloObjectMaker.h"

#include "eflowRec/eflowRecCluster.h"
#include "eflowRec/eflowRecTrack.h"
#include "eflowRec/eflowTrackClusterLink.h"
#include "eflowRec/eflowCaloObject.h"

#include <algorithm>
#include <iostream>
#include <cmath>
#include <vector>


unsigned int eflowCaloObjectMaker::makeTrkCluCaloObjects(eflowRecTrackContainer* eflowTrackContainer, eflowRecClusterContainer* eflowClusterContainer, eflowCaloObjectContainer* caloObjectContainer) {
  std::vector<eflowRecTrack*> tracksToRecover;
  std::vector<eflowRecCluster*> clustersToConsider;
  for (unsigned int iTrack=0; iTrack<eflowTrackContainer->size(); ++iTrack) {
    eflowRecTrack *thisEflowRecTrack = static_cast<eflowRecTrack*>(eflowTrackContainer->at(iTrack));
    tracksToRecover.push_back(thisEflowRecTrack);
  }
  for (unsigned int iCluster = 0; iCluster < eflowClusterContainer->size(); ++iCluster) {
    eflowRecCluster* thisEFRecCluster = eflowClusterContainer->at(iCluster);
    clustersToConsider.push_back(thisEFRecCluster);
  }

  return makeTrkCluCaloObjects(tracksToRecover, clustersToConsider, caloObjectContainer);

}

unsigned int eflowCaloObjectMaker::makeTrkCluCaloObjects(std::vector<eflowRecTrack*>& tracksToRecover, std::vector<eflowRecCluster*>& clustersToConsider, eflowCaloObjectContainer* caloObjectContainer) {
  unsigned int result(0);

  /* Create all eflowCaloObjects that contain only eflowRecTracks and cache eflowRecTracks matched with cluster */
  std::vector<eflowRecTrack*> tracksToConsider;  tracksToConsider.clear();
  unsigned int nTrack = tracksToRecover.size();
  for (unsigned int iTrack=0; iTrack<nTrack; ++iTrack) {
    eflowRecTrack *thisEflowRecTrack = static_cast<eflowRecTrack*>(tracksToRecover.at(iTrack));
    if(thisEflowRecTrack->getClusterMatches().empty()) {
      std::unique_ptr<eflowCaloObject> calob = std::make_unique<eflowCaloObject>();
      calob->addTrack(thisEflowRecTrack);
      caloObjectContainer->push_back(std::move(calob));
      result++;
    } else {
      tracksToConsider.push_back(thisEflowRecTrack);
    }
  }

  /* Create all eflowCaloObjects that contain only eflowRecCluster*/
  unsigned int nClusters = clustersToConsider.size();
  for (unsigned int iCluster = 0; iCluster < nClusters; ++iCluster) {
    eflowRecCluster* thisEFRecCluster = clustersToConsider.at(iCluster);
    if(thisEFRecCluster->getTrackMatches().empty()) {
      std::unique_ptr<eflowCaloObject> thisEflowCaloObject = std::make_unique<eflowCaloObject>();
      thisEflowCaloObject->addCluster(thisEFRecCluster);
      caloObjectContainer->push_back(std::move(thisEflowCaloObject));
      result++;
    }
  }

  // It is possible an event has no tracks to consider 
  if (!tracksToConsider.empty()){

    /* one loop creates one CaloObject */
    do {
      /* temporary vector for next loop */
      std::vector<eflowRecTrack*> trackForNextLoop; trackForNextLoop.clear();
      std::vector<eflowRecCluster*> clusterForNextLoop; clusterForNextLoop.clear();
      
      /* track/cluster vector for CaloObject */
      std::vector<eflowRecTrack*> trackList; trackList.clear();
      std::vector<eflowRecCluster*> clusterList; clusterList.clear();
      
      /* initiallize trackForNextLoop by the first track in tracksToConsider */
      trackForNextLoop.push_back(tracksToConsider.at(0));
      trackList.insert(trackList.end(), trackForNextLoop.begin(), trackForNextLoop.end());

      unsigned int time(0);
      /* iteratively searching tracks and clusters those belong to one CaloObject */
      do {
	clusterForNextLoop = uniqCluster(trackForNextLoop, clusterList);
	if(clusterForNextLoop.empty()) break;
	clusterList.insert(clusterList.end(), clusterForNextLoop.begin(), clusterForNextLoop.end());
	trackForNextLoop = uniqTrack(clusterForNextLoop, trackList);
	if (trackForNextLoop.empty()) break;
	trackList.insert(trackList.end(), trackForNextLoop.begin(), trackForNextLoop.end());
      } while (++time);
      
      /* store this eflowCaloObject */
      std::unique_ptr<eflowCaloObject> thisEflowCaloObject = std::make_unique<eflowCaloObject>();
      thisEflowCaloObject->addTracks(trackList);
      
      thisEflowCaloObject->addClusters(clusterList);
      for(std::vector<eflowRecCluster*>::const_iterator itr_cluster = clusterList.begin(); itr_cluster != clusterList.end(); ++itr_cluster) {
	thisEflowCaloObject->addTrackClusterLinks((*itr_cluster)->getTrackMatches());
      }
      caloObjectContainer->push_back(std::move(thisEflowCaloObject));
      
      /* update tracksToConsider */
      updateTracksToConsider(tracksToConsider, trackList);
      if(tracksToConsider.empty()) {
	++result;
	break;
      }

    } while (++result);
  }//If tracksToCosider is not empty


  return result;
}

std::vector<eflowRecCluster*> eflowCaloObjectMaker::uniqCluster(const std::vector<eflowRecTrack*> trackForNextLoop, const std::vector<eflowRecCluster*> clusterList) {
  std::vector<eflowRecCluster*> result; result.clear();
  std::vector<int> allClusterId; allClusterId.clear();
  for(std::vector<eflowRecTrack*>::const_iterator itr_track = trackForNextLoop.begin(); itr_track != trackForNextLoop.end(); ++itr_track) {
    std::vector<eflowTrackClusterLink*> allLinks = (*itr_track)->getClusterMatches();
    for(std::vector<eflowTrackClusterLink*>::const_iterator itr_link = allLinks.begin(); itr_link != allLinks.end(); ++itr_link) {
      int id = (*itr_link)->getCluster()->getClusterId();
      bool notInList(true);
      for (std::vector<eflowRecCluster*>::const_iterator itr_cluster = clusterList.begin(); itr_cluster != clusterList.end(); ++itr_cluster) {
        if (id == (*itr_cluster)->getClusterId()) {
          notInList = false;
          break;
        }
      }
      if ((find(allClusterId.begin(), allClusterId.end(), id) == allClusterId.end()) && notInList) {
        allClusterId.push_back(id);
        result.push_back((*itr_link)->getCluster());
      } else {
        continue;
      }
    }
  }
  return result;
}

std::vector<eflowRecTrack*> eflowCaloObjectMaker::uniqTrack(const std::vector<eflowRecCluster*> clusterForNextLoop, const std::vector<eflowRecTrack*> trackList) {
  std::vector<eflowRecTrack*> result; result.clear();
  std::vector<int> allTrackId; allTrackId.clear();
  for(std::vector<eflowRecCluster*>::const_iterator itr_cluster = clusterForNextLoop.begin(); itr_cluster != clusterForNextLoop.end(); ++itr_cluster) {
    std::vector<eflowTrackClusterLink*> allLinks = (*itr_cluster)->getTrackMatches();
    for(std::vector<eflowTrackClusterLink*>::const_iterator itr_link = allLinks.begin(); itr_link != allLinks.end(); ++itr_link) {
      int id = (*itr_link)->getTrack()->getTrackId();
      bool notInList(true);
      for (std::vector<eflowRecTrack*>::const_iterator itr_track = trackList.begin(); itr_track != trackList.end(); ++itr_track) {
        if (id == (*itr_track)->getTrackId()) {
          notInList = false;
          break;
        }
      }
      if ((find(allTrackId.begin(), allTrackId.end(), id) == allTrackId.end()) && notInList) {
        allTrackId.push_back(id);
        result.push_back((*itr_link)->getTrack());
      } else {
        continue;
      }
    }
  }
  return result;
}

void eflowCaloObjectMaker::updateTracksToConsider(std::vector<eflowRecTrack*>& total, const std::vector<eflowRecTrack*> part) {
  unsigned int nPart = part.size();
  for(unsigned int i=0; i<nPart; ++i) {
    std::vector<eflowRecTrack*>::iterator itr = total.begin();
    while (itr != total.end()) {
      if ((*itr)->getTrackId() == part.at(i)->getTrackId()) { total.erase(itr); break; }
      ++itr;
    }
  }
}
