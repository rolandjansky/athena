/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * eflowSubtractor.cxx
 *
 *  Created on: Feb 25, 2015
 *      Author: zhangrui
 */

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"
#include "eflowRec/eflowRecTrack.h"
#include "eflowRec/eflowRecCluster.h"
#include "eflowRec/eflowTrackClusterLink.h"
#include "eflowRec/eflowCaloObject.h"
#include "eflowRec/eflowRingSubtractionManager.h"
#include "eflowRec/eflowCellSubtractionFacilitator.h"
#include "eflowRec/eflowLayerIntegrator.h"
#include "eflowRec/eflowEEtaBinnedParameters.h"
#include "eflowRec/eflowSubtractor.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"


namespace eflowSubtract {

void Subtractor::subtractTracksFromClusters(eflowRecTrack* efRecTrack, std::vector<xAOD::CaloCluster*> clusterSubtractionList) {

    /* Make ordered cell list */
    /* (Invokes newCluster() on orderedCells, than adds all the cells in tracksClus) */
    eflowCellList orderedCells;
    makeOrderedCellList(efRecTrack->getTrackCaloPoints(), clusterSubtractionList, orderedCells);

    /* Get the cellSubtractionManager from the eflowCaloObject */
    eflowRingSubtractionManager& ranking = efRecTrack->getCellSubtractionManager();

    eflowCellSubtractionFacilitator facilitator;
    facilitator.subtractCells(ranking, efRecTrack->getTrack()->e(), clusterSubtractionList, orderedCells);

    orderedCells.eraseList();

}

void Subtractor::makeOrderedCellList(const eflowTrackCaloPoints& trackCalo, const std::vector<xAOD::CaloCluster*>& clusters, eflowCellList& orderedCells) {
  orderedCells.setNewExtrapolatedTrack(trackCalo);
  std::vector<xAOD::CaloCluster*>::const_iterator  itMatchedCluster = clusters.begin();
  std::vector<xAOD::CaloCluster*>::const_iterator endMatchedCluster = clusters.end();
  for (int countMatchedClusters = 0; itMatchedCluster != endMatchedCluster ; ++itMatchedCluster, ++countMatchedClusters){
    xAOD::CaloCluster *thisCluster = *itMatchedCluster;

    const CaloClusterCellLink* theCellLink = thisCluster->getCellLinks();
    CaloClusterCellLink::const_iterator firstCell = theCellLink->begin();
    CaloClusterCellLink::const_iterator lastCell = theCellLink->end();

    /* Loop over cells in cluster */
    for (; firstCell != lastCell; firstCell++) {
      std::pair<CaloCell*,int> myPair(const_cast<CaloCell*>(*firstCell), countMatchedClusters);
      orderedCells.addCell(myPair);
    }
  }
}


void Subtractor::annihilateClusters(std::vector<xAOD::CaloCluster*>& clusters) {
  std::vector<xAOD::CaloCluster*>::iterator  itCluster = clusters.begin();
  std::vector<xAOD::CaloCluster*>::iterator endCluster = clusters.end();
  for (; itCluster != endCluster; ++itCluster) {
    annihilateCluster(*itCluster);
  }
}

void Subtractor::annihilateCluster(xAOD::CaloCluster* cluster) {

  const CaloClusterCellLink* theCellLink = cluster->getCellLinks();
  CaloClusterCellLink* theCellLink_nonConst = const_cast<CaloClusterCellLink*>(theCellLink);

  CaloClusterCellLink::iterator theFirstCell = theCellLink_nonConst->begin();
  CaloClusterCellLink::iterator theLastCell = theCellLink_nonConst->end();

  for (; theFirstCell != theLastCell; ++theFirstCell) theCellLink_nonConst->removeCell(theFirstCell);

  cluster->setE(0.0);
  cluster->setRawE(0.0);
  CaloClusterKineHelper::calculateKine(cluster,true, true);

}

} //namespace eflowSubtract
