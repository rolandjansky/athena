/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/*
 * eflowSubtractor.h
 *
 *  Created on: Feb 25, 2015
 *      Author: zhangrui
 */

#ifndef EFLOWSUBTRACTOR_H_
#define EFLOWSUBTRACTOR_H_

#include "eflowRec/eflowCellList.h"
#include "eflowRec/eflowCellSubtractionFacilitator.h"
#include "eflowRec/eflowRingSubtractionManager.h"
#include "xAODCaloEvent/CaloCluster.h"
#include <vector>

class eflowRecTrack;
class eflowRecCluster;
class eflowTrackCaloPoints;
class eflowLayerIntegrator;
class eflowEEtaBinnedParameters;

namespace eflowSubtract {

/**
Used in eflowCellLevelSubtractionTool and eflowRecoverSplitShowersTool to
perform the removal of calorimeter cells from clusters. Makes use of
eflowCellList, eflowRingSubtractionManager and eflowCellSubtractionFacilitator.
*/
class Subtractor
{

public:
  Subtractor() = default;

  ~Subtractor() {}

  void subtractTracksFromClusters(
    eflowRecTrack* efRecTrack,
    std::vector<std::pair<xAOD::CaloCluster*, bool>>& clusterSubtractionList) const;

  static void annihilateClusters(
    std::vector<std::pair<xAOD::CaloCluster*, bool>>& clusters);
  
  static void annihilateCluster(xAOD::CaloCluster* cluster);
  
  static void makeOrderedCellList(
    const eflowTrackCaloPoints& trackCalo,
    const std::vector<std::pair<xAOD::CaloCluster*, bool>>& clusters,
    eflowCellList& orderedCells);

  eflowCellSubtractionFacilitator m_facilitator{};
};
}

#endif /* EFLOWSUBTRACTOR_H_ */
