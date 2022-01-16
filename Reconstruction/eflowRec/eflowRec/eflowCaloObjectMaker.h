/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/*
 * eflowCaloObjectMaker.h
 *
 *  Created on: Apr 16, 2015
 *      Author: zhangrui
 */

#ifndef EFLOWCALOOBJECTMAKER_H_
#define EFLOWCALOOBJECTMAKER_H_

#include <vector>

class eflowCaloObjectContainer;
class eflowRecTrackContainer;
class eflowRecClusterContainer;
class eflowRecCluster;
class eflowRecTrack;

/**
This class creates eflowCaloObjects and adds them into the eflowCaloObjectContainer.
*/
class eflowCaloObjectMaker {

public:

  eflowCaloObjectMaker() { }

  static unsigned int makeTrkCluCaloObjects(eflowRecTrackContainer* eflowTrackContainer, eflowRecClusterContainer* eflowClusterContainer, eflowCaloObjectContainer* caloObjectContainer);
  static unsigned int makeTrkCluCaloObjects(std::vector<eflowRecTrack*>& tracksToRecover, std::vector<eflowRecCluster*>& clustersToConsider, eflowCaloObjectContainer* caloObjectContainer);

private:

  static std::vector<eflowRecCluster*> uniqCluster(const std::vector<eflowRecTrack*>& trackForNextLoop, const std::vector<eflowRecCluster*>& clusterList);
  static std::vector<eflowRecTrack*> uniqTrack(const std::vector<eflowRecCluster*>& clusterForNextLoop, const std::vector<eflowRecTrack*>& trackList);
  static void updateTracksToConsider(std::vector<eflowRecTrack*>& total, const std::vector<eflowRecTrack*>& part);


};

#endif /* EFLOWCALOOBJECTMAKER_H_ */
