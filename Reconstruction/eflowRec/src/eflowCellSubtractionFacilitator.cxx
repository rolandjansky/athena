/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Athena Headers
#include "eflowRec/eflowCellSubtractionFacilitator.h"
#include "eflowRec/eflowCellPosition.h"

#include "CaloEvent/CaloCell.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"

//C++ Headers
#include <map>

eflowCellSubtractionFacilitator::eflowCellSubtractionFacilitator(): m_annFlag(false) { }

double eflowCellSubtractionFacilitator::subtractCells(eflowCellSubtractionManager& cellSubtractionManager, double trackEnergy, xAOD::CaloCluster* tracksCluster, eflowCellList& orderedCells) {
  return subtractCells(cellSubtractionManager, trackEnergy, std::vector<xAOD::CaloCluster*> (1,tracksCluster), orderedCells);
}

void eflowCellSubtractionFacilitator::updateClusterKinematics(std::vector<xAOD::CaloCluster*>& tracksClusters) {
  std::vector<xAOD::CaloCluster*>::iterator itClus = tracksClusters.begin();
  std::vector<xAOD::CaloCluster*>::iterator endCluster = tracksClusters.end();
  for (; itClus != endCluster; itClus++) {
    updateClusterKinematics(*itClus);
  }
}

void eflowCellSubtractionFacilitator::updateClusterKinematics(xAOD::CaloCluster* theCluster) {
  float oldEnergy = theCluster->e();
  CaloClusterKineHelper::calculateKine(theCluster, true, true);
  if (0.0 != oldEnergy) {
    float energyAdjustment = theCluster->e() / oldEnergy;
    theCluster->setRawE(theCluster->rawE() * energyAdjustment);
    theCluster->setRawEta(theCluster->eta());
    theCluster->setRawPhi(theCluster->phi());
  }
}

double eflowCellSubtractionFacilitator::subtractCells(eflowCellSubtractionManager& cellSubtractionManager, double trackEnergy, std::vector<xAOD::CaloCluster*> tracksClusters, eflowCellList& orderedCells) {

  int debug = 0;

  const double eExpect = cellSubtractionManager.fudgeMean() * trackEnergy;
  const double sigmaEExpect = cellSubtractionManager.fudgeStdDev() * trackEnergy;

  if (1 == debug) {
    std::cout << "So eExpect is " << eExpect << std::endl;
  }

  /* Summed energy of all clusters before subtraction */
  std::vector<xAOD::CaloCluster*>::iterator itCluster = tracksClusters.begin();
  std::vector<xAOD::CaloCluster*>::iterator endCluster = tracksClusters.end();
  double eClustersOld = 0;
  for (; itCluster != endCluster; itCluster++) {
    eClustersOld += (*itCluster)->e();
  }

  double eSubtracted = 0.0;

  std::map<double, RingId>::const_iterator ringIt = cellSubtractionManager.rankBegin();
  std::map<double, RingId>::const_iterator ringEnd = cellSubtractionManager.rankEnd();
  for (ringIt = cellSubtractionManager.rankBegin(); ringIt != ringEnd; ++ringIt) {

    const std::pair<eflowCaloENUM, short>& ring = ringIt->second;
    const eflowCaloENUM subtLayer = ring.first;
    const short ringNo = ring.second;

    const double r1 = ringNo * cellSubtractionManager.ringThickness(subtLayer);
    const double r2 = (ringNo + 1) * cellSubtractionManager.ringThickness(subtLayer);

    CellIt beginRing = orderedCells.getLowerBound(subtLayer, r1);
    CellIt endRing = orderedCells.getLowerBound(subtLayer, r2);

    /*  * * *  Do Subtraction * * * */

    /* Find total energy in ring */
    double eRing = 0.0;
    for (CellIt it = beginRing; it != endRing; ++it) {
      std::vector<std::pair<CaloCell*, int> >::iterator itEntry = it->second.begin();
      std::vector<std::pair<CaloCell*, int> >::iterator lastEntry = it->second.end();
      for (; itEntry != lastEntry; itEntry++) {
        std::pair<CaloCell*, int> thisPair = *itEntry;

        const xAOD::CaloCluster* clus = tracksClusters[thisPair.second];
        CaloClusterCellLink::iterator theIterator = this->getCellIterator(clus, thisPair.first);
        double cellWeight = theIterator.weight();

        eRing += thisPair.first->energy() * cellWeight;
        if (1 == debug)
          std::cout << "added cell with e " << thisPair.first->energy() * cellWeight
                    << " from cluster with e " << clus->e() << " to ring " << std::endl;
      } //loop over all pairs in vector
    } //loop over CellIt

    // subtract energy from ring

    if (1 == debug) std::cout << "ring energy is " << eRing << std::endl;
    if (1 == debug)
      std::cout << "eSubt is and eExpect is " << eSubtracted << " and " << eExpect << std::endl;

    if (eSubtracted + eRing > eExpect) {  // don't need to subtract whole ring
      /* Target ring energy is ring energy minus the energy that still needs to be subtracted */
      double targetRingEnergy = eRing - (eExpect - eSubtracted);

      for (CellIt it = beginRing; it != endRing; ++it) {

        std::vector<std::pair<CaloCell*, int> >::iterator itEntry = it->second.begin();
        std::vector<std::pair<CaloCell*, int> >::iterator lastEntry = it->second.end();
        for (; itEntry != lastEntry; itEntry++) {
          std::pair<CaloCell*, int> thisPair = *itEntry;

          xAOD::CaloCluster* cluster = tracksClusters[thisPair.second];
          CaloCell* cell = thisPair.first;
          CaloClusterCellLink::iterator theIterator = this->getCellIterator(cluster, cell);
          double oldCellWeight = theIterator.weight();
          const double newCellWeight = oldCellWeight * targetRingEnergy / eRing;

          theIterator.reweight(newCellWeight);
        } //loop over all pairs in vector
      } //loop over CellIt

      eSubtracted = eExpect;

      /* Update the cluster four-momenta having done the subtraction */
      updateClusterKinematics(tracksClusters);

      return sigmaEExpect;  // finished subtraction

    } else {  // subtract whole ring
      if (1 == debug) std::cout << " subtract full ring " << std::endl;

      for (CellIt it = beginRing; it != endRing; ++it) {

        std::vector<std::pair<CaloCell*, int> >::iterator itEntry = it->second.begin();
        std::vector<std::pair<CaloCell*, int> >::iterator lastEntry = it->second.end();
        for (; itEntry != lastEntry; itEntry++) {
          std::pair<CaloCell*, int> thisPair = *itEntry;

          xAOD::CaloCluster* cluster = tracksClusters[thisPair.second];
          cluster->removeCell(thisPair.first);
        }  //loop over all pairs in vector
      }  //loop over CellIt
      orderedCells.deleteFromList(beginRing, endRing);
      eSubtracted += eRing;
    }

    if (1 == debug) {
      std::cout << " now esubt is " << eSubtracted << std::endl;
    }

    if (fabs(eClustersOld - eSubtracted) < 1.0e-6 * eClustersOld) {
      //annihilate clusters

      std::vector<xAOD::CaloCluster*>::iterator itClus = tracksClusters.begin();

      for (; itClus != endCluster; itClus++) {

        const CaloClusterCellLink* theCellLink = (*itClus)->getCellLinks();
        CaloClusterCellLink* theCellLink_nonConst = const_cast<CaloClusterCellLink*>(theCellLink);
        CaloClusterCellLink::iterator theFirstCell = theCellLink_nonConst->begin();
        CaloClusterCellLink::iterator theLastCell = theCellLink_nonConst->end();

        for (; theFirstCell != theLastCell; ++theFirstCell) {
          (*itClus)->removeCell(*theFirstCell);
        }
        (*itClus)->setCalE(0.0);
        (*itClus)->setRawE(0.0);

        orderedCells.eraseList();
        m_annFlag = true;

      }  //clus loop

      itClus = tracksClusters.begin();

      /* Update the cluster four-momenta having done the subtraction */
      updateClusterKinematics(tracksClusters);

      return sigmaEExpect;
    }  //if no energy left
  }  //end of loop on cell rings in order

  /* Update the cluster four-momenta having done the subtraction */
  updateClusterKinematics(tracksClusters);

  if (1 == debug) {
    std::cout << " here esubt and eExpect are finally " << eSubtracted << " and " << eExpect << std::endl;
  }

  if (orderedCells.mapSize() > 0 && eSubtracted < eExpect) {

    orderedCells.reorderWithoutLayers(); // just want to go through in order of increasing dR (not by layer)

    CellIt endCells = orderedCells.end();
    for (CellIt itCellPosition = orderedCells.begin(); itCellPosition != endCells;) {

      std::vector<std::pair<CaloCell*, int> >::iterator itEntry = itCellPosition->second.begin();
      for (; itEntry != itCellPosition->second.end(); ++itEntry) {
        std::pair<CaloCell*, int> thisPair = *itEntry;

        xAOD::CaloCluster* cluster = tracksClusters[thisPair.second];
        CaloCell* cell = thisPair.first;

        CaloClusterCellLink::iterator theIterator = this->getCellIterator(cluster, cell);
        double oldCellWeight = theIterator.weight();
        double oldCellEnergy = cell->energy() * oldCellWeight;

        if (eSubtracted + oldCellEnergy > eExpect && oldCellEnergy != 0.) {
          /* Target cell energy is cell energy minus the energy that still needs to be subtracted */
          double targetCellEnergy = oldCellEnergy - (eExpect - eSubtracted);

          double newCellWeight = oldCellWeight * targetCellEnergy / oldCellEnergy;
          theIterator.reweight(newCellWeight);

          eSubtracted = eExpect;

          /* Update the cluster four-momenta having done the subtraction */
          updateClusterKinematics(tracksClusters);

          return sigmaEExpect;

        } else {
          cluster->removeCell(cell);
          eSubtracted += cluster->e();

          //update the clusters having done the subtraction
          updateClusterKinematics(cluster);
        }

      } //loop over all pairs in vector

      /* Erase the CellPosition from the cell list */
      CellIt eraseIt = itCellPosition;
      ++itCellPosition;
      orderedCells.deleteFromList(eraseIt);

    } //Cell It Loop
  }

  /* Update the cluster four-momenta having done the subtraction */
  updateClusterKinematics(tracksClusters);

  return sigmaEExpect;

}

CaloClusterCellLink::iterator eflowCellSubtractionFacilitator::getCellIterator(const xAOD::CaloCluster* thisCluster, const CaloCell* thisCell){

  //SLOW! Can we move to directly work with iterators in future? 

  //We have to use non-const iterators so that we are allowed to modify the cell weights
  const CaloClusterCellLink* theCellLink = thisCluster->getCellLinks();
  CaloClusterCellLink* theNonConstCellLink = const_cast<CaloClusterCellLink*>(theCellLink);

  CaloClusterCellLink::iterator it=theNonConstCellLink->begin();
  CaloClusterCellLink::iterator it_e=theNonConstCellLink->end();
  for(;it!=it_e;++it) {
    const CaloCell* pCell=(*it);
    if (pCell==thisCell) { //Pointer comparison!
      return it;
    }
  }

  //if no match is found then return end of container
  return it_e;
  
}
