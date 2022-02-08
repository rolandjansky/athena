/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/*
 * eflowSubtractor.cxx
 *
 *  Created on: Feb 25, 2015
 *      Author: zhangrui
 */

#include "eflowRec/eflowSubtractor.h"
#include "eflowRec/eflowCaloObject.h"
#include "eflowRec/eflowCellSubtractionFacilitator.h"
#include "eflowRec/eflowEEtaBinnedParameters.h"
#include "eflowRec/eflowLayerIntegrator.h"
#include "eflowRec/eflowRecCluster.h"
#include "eflowRec/eflowRecTrack.h"
#include "eflowRec/eflowRingSubtractionManager.h"
#include "eflowRec/eflowTrackClusterLink.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"

namespace eflowSubtract {

void
Subtractor::subtractTracksFromClusters(
  eflowRecTrack* efRecTrack,
  std::vector<std::pair<xAOD::CaloCluster*, bool>>& clusterSubtractionList,
  const bool& debugToggle)
{

  /* Make ordered cell list */
  /* (Invokes newCluster() on orderedCells, than adds all the cells in
   * tracksClus) */
  eflowCellList orderedCells;
  makeOrderedCellList(
    efRecTrack->getTrackCaloPoints(), clusterSubtractionList, orderedCells);

  /* Get the cellSubtractionManager from the eflowCaloObject */
  eflowRingSubtractionManager& ranking =
    efRecTrack->getCellSubtractionManager();

  eflowCellSubtractionFacilitator facilitator;
  if (debugToggle)
    facilitator.setLevel(MSG::DEBUG);
  facilitator.subtractCells(
    ranking, efRecTrack->getTrack()->e(), clusterSubtractionList, orderedCells);

  orderedCells.eraseList();
}

void
Subtractor::makeOrderedCellList(
  const eflowTrackCaloPoints& trackCalo,
  const std::vector<std::pair<xAOD::CaloCluster*, bool>>& clusters,
  eflowCellList& orderedCells)
{
  orderedCells.setNewExtrapolatedTrack(trackCalo);

  unsigned int countMatchedClusters = 0;
  for (auto thisPair : clusters) {

    xAOD::CaloCluster* thisCluster = thisPair.first;

    const CaloClusterCellLink* theCellLink = thisCluster->getCellLinks();
    CaloClusterCellLink::const_iterator firstCell = theCellLink->begin();
    CaloClusterCellLink::const_iterator lastCell = theCellLink->end();

    /* Loop over cells in cluster */
    for (; firstCell != lastCell; firstCell++) {
      std::pair<const CaloCell*, int> myPair((*firstCell),
                                       countMatchedClusters);
      orderedCells.addCell(myPair);
    }
    countMatchedClusters++;
  }
}

void
Subtractor::annihilateClusters(
  std::vector<std::pair<xAOD::CaloCluster*, bool>>& clusters)
{

  for (auto& thisPair : clusters) {
    annihilateCluster(thisPair.first);
    // mark subtraction status as true
    thisPair.second = true;
  }
}

void
Subtractor::annihilateCluster(xAOD::CaloCluster* cluster)
{

  CaloClusterCellLink* theCellLink = cluster->getOwnCellLinks();

  CaloClusterCellLink::iterator theFirstCell = theCellLink->begin();
  CaloClusterCellLink::iterator theLastCell = theCellLink->end();

  for (; theFirstCell != theLastCell; ++theFirstCell)
    theCellLink->removeCell(theFirstCell);

  cluster->setE(0.0);
  cluster->setRawE(0.0);
  CaloClusterKineHelper::calculateKine(cluster, true, true);
}

} // namespace eflowSubtract
