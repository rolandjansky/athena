/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Athena Headers
#include "eflowRec/eflowCellSubtractionFacilitator.h"
//#include "eflowRec/eflowCellPosition.h"

#include "CaloEvent/CaloCell.h"
#include "xAODCaloEvent/CaloClusterKineHelper.h"

//C++ Headers
#include <map>

eflowCellSubtractionFacilitator::eflowCellSubtractionFacilitator(): m_annFlag(false) { }

double eflowCellSubtractionFacilitator::subtractCells(eflowRingSubtractionManager& cellSubtractionManager, double trackEnergy, xAOD::CaloCluster* tracksCluster, eflowCellList& orderedCells) {
  std::vector<std::pair<xAOD::CaloCluster*, bool> > localClusterBoolPairVec(1,std::pair(tracksCluster,false));
  return subtractCells(cellSubtractionManager, trackEnergy, localClusterBoolPairVec, orderedCells);
}

void eflowCellSubtractionFacilitator::updateClusterKinematics(std::vector<std::pair<xAOD::CaloCluster*, bool> >& tracksClusters) {
  for (auto thisPair : tracksClusters) updateClusterKinematics(thisPair.first);
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

double eflowCellSubtractionFacilitator::getTotalEnergy(std::vector<std::pair<xAOD::CaloCluster*, bool> > tracksClusters) {
  double eClustersOld = 0;
  /* Summed energy of all clusters before subtraction */
  for (auto thisPair : tracksClusters) eClustersOld += (thisPair.first)->e();
  return eClustersOld;
}

double eflowCellSubtractionFacilitator::getRingsEnergy(const std::vector<std::pair<xAOD::CaloCluster*, bool> >& tracksClusters, CellIt beginRing, CellIt endRing) {
  /* Get total energy of ringes from beginRing to endRing */

  double eRing(0.0);
  for (CellIt it = beginRing; it != endRing; ++it) {
    /* Loop over Rings */
    for (auto thisCell : it->second){
      /* Loop over Cells */
      std::pair<CaloCell*, int> thisPair = thisCell;
      const xAOD::CaloCluster* clus = tracksClusters[thisPair.second].first;
      CaloClusterCellLink::iterator theIterator = this->getCellIterator(clus, thisPair.first);
      double cellWeight = theIterator.weight();
      eRing += thisPair.first->energy() * cellWeight;
    }
  }

  return eRing;
}

void eflowCellSubtractionFacilitator::annihilateClusters(std::vector<std::pair<xAOD::CaloCluster*, bool> >& tracksClusters) {
  for (auto& thisPair : tracksClusters){
    xAOD::CaloCluster* thisCluster = thisPair.first;
    const CaloClusterCellLink* theCellLink = thisCluster->getCellLinks();
    CaloClusterCellLink* theCellLink_nonConst = const_cast<CaloClusterCellLink*>(theCellLink);
    CaloClusterCellLink::iterator theFirstCell = theCellLink_nonConst->begin();
    CaloClusterCellLink::iterator theLastCell = theCellLink_nonConst->end();
    for (; theFirstCell != theLastCell; ++theFirstCell) thisCluster->removeCell(*theFirstCell);
    thisCluster->setCalE(0.0);
    thisCluster->setRawE(0.0);
    //set the subtracted status to true
    thisPair.second = true;
  }
  setAnnFlag();
}

void eflowCellSubtractionFacilitator::subtractPartialRings(std::vector<std::pair<xAOD::CaloCluster*, bool> >& tracksClusters, CellIt beginRing, CellIt endRing,
    double targetRingEnergy, double eRings) {
  for (CellIt itRing = beginRing; itRing != endRing; ++itRing) {
    /* Loop over Rings */
    for (auto thisCell : itRing->second){
      /* Loop over Cells */
      std::pair<CaloCell*, int> thisPair = thisCell;
      xAOD::CaloCluster* cluster = tracksClusters[thisPair.second].first;
      //flag this cluster as having had subtraction applied to it
      tracksClusters[thisPair.second].second = true;
      CaloCell* cell = thisPair.first;
      CaloClusterCellLink::iterator theIterator = this->getCellIterator(cluster, cell);
      double oldCellWeight = theIterator.weight();
      const double newCellWeight = oldCellWeight * targetRingEnergy / eRings;
      theIterator.reweight(newCellWeight);
    }
  }
}

void eflowCellSubtractionFacilitator::subtractFullRings(std::vector<std::pair<xAOD::CaloCluster*, bool> >& tracksClusters, CellIt beginRing, CellIt endRing) {
  /* Subtract full ring */

  for (CellIt itRing = beginRing; itRing != endRing; ++itRing) {
    /* Loop over Rings */
    for (auto thisCell : itRing->second){
      /* Loop over Cells */
      std::pair<CaloCell*, int> thisPair = thisCell;
      xAOD::CaloCluster* cluster = tracksClusters[thisPair.second].first;
      //flag this cluster as having had subtraction applied to it
      tracksClusters[thisPair.second].second = true;
      cluster->removeCell(thisPair.first);
    }
  }
}

bool eflowCellSubtractionFacilitator::subtractRings(
    eflowRingSubtractionManager& cellSubtractionManager,
    const std::pair<eflowCaloENUM, short>& ring, double & eSubtracted, const double eExpect,
    eflowCellList& orderedCells, std::vector<std::pair<xAOD::CaloCluster*, bool> >& tracksClusters) {
  /* Subtract energy from ring, return TRUE if the whole expected energy is subtracted */

  const eflowCaloENUM subtLayer = ring.first;
  const short ringNo = ring.second;

  const double r1 = ringNo * cellSubtractionManager.ringThickness(subtLayer);
  const double r2 = (ringNo + 1) * cellSubtractionManager.ringThickness(subtLayer);

  CellIt beginRing = orderedCells.getLowerBound(subtLayer, r1);
  CellIt endRing = orderedCells.getLowerBound(subtLayer, r2);

  /* Get total energy of Rings from beginRing to endRing */
  double eRings = getRingsEnergy(tracksClusters, beginRing, endRing);

  if (eSubtracted + eRings > eExpect) {
    /* Subtract partial ring */

    /* Target ring energy is ring energy minus the energy that still needs to be subtracted */
    double targetRingEnergy = eRings - (eExpect - eSubtracted);
    subtractPartialRings(tracksClusters, beginRing, endRing, targetRingEnergy, eRings);
    eSubtracted = eExpect;

    /* Update the cluster four-momenta having done the subtraction */
    updateClusterKinematics(tracksClusters);

    return true;

  } else {
    /* Subtract full ring */

    subtractFullRings(tracksClusters, beginRing, endRing);
    orderedCells.deleteFromList(beginRing, endRing);
    eSubtracted += eRings;

    /* If no energy left */
    double eClustersOld = getTotalEnergy(tracksClusters);
    if (fabs(eClustersOld - eSubtracted) < 1.0e-6 * eClustersOld) {
      /* Annihilate clusters, clear orderedCells, and update subtracted cluster kinematics */
      annihilateClusters(tracksClusters);
      orderedCells.eraseList();
      updateClusterKinematics(tracksClusters);

      return true;
    }

    return false;
  }
}

bool eflowCellSubtractionFacilitator::subtractCaloCell(
    double & eSubtracted, const double eExpect, xAOD::CaloCluster* cluster,
    CaloCell* cell) {


  CaloClusterCellLink::iterator theIterator = this->getCellIterator(cluster, cell);
  double oldCellWeight = theIterator.weight();
  double oldCellEnergy = cell->energy() * oldCellWeight;

  if (oldCellEnergy != 0. && eSubtracted + oldCellEnergy > eExpect) {
    /* Target cell energy is cell energy minus the energy that still needs to be subtracted */
    double targetCellEnergy = oldCellEnergy - (eExpect - eSubtracted);

    double newCellWeight = oldCellWeight * targetCellEnergy / oldCellEnergy;
    theIterator.reweight(newCellWeight);

    eSubtracted = eExpect;

    /* Update the cluster four-momenta having done the subtraction */
    updateClusterKinematics(cluster);

    return true;

  } else {
    cluster->removeCell(cell);
    eSubtracted += oldCellEnergy;

    /* Update the clusters having done the subtraction */
    updateClusterKinematics(cluster);
    return false;
  }
}

bool eflowCellSubtractionFacilitator::subtractReorderedCells(std::vector<std::pair<xAOD::CaloCluster*, bool> >& tracksClusters, double eSubtracted, const double eExpect,
    eflowCellList& reorderedCells) {
  CellIt itCellPosition = reorderedCells.begin();
  CellIt endCellPosition = reorderedCells.end();
  while (itCellPosition != endCellPosition) {
    /* Loop cells */
    std::vector<std::pair<CaloCell*, int> >::iterator itEntry = itCellPosition->second.begin();
    std::vector<std::pair<CaloCell*, int> >::iterator endEntry = itCellPosition->second.end();
    for (; itEntry != endEntry; ++itEntry) {
      const std::pair<CaloCell*, int> thisPair = *itEntry;
      xAOD::CaloCluster* cluster = tracksClusters[thisPair.second].first;
      //flag this cluster as having had subtraction applied to it
      tracksClusters[thisPair.second].second = true;
      CaloCell* cell = thisPair.first;
      bool isFinished = subtractCaloCell(eSubtracted, eExpect, cluster, cell);
      if (isFinished) return true;
    }
    /* Erase the CellPosition from the cell list */
    CellIt tmp = itCellPosition;
    itCellPosition++;
    reorderedCells.deleteFromList(tmp);
  }
  return false;
}

double eflowCellSubtractionFacilitator::subtractCells(eflowRingSubtractionManager& cellSubtractionManager, double trackEnergy, std::vector<std::pair<xAOD::CaloCluster*, bool> >& tracksClusters, eflowCellList& orderedCells) {

  const double eExpect = cellSubtractionManager.fudgeMean() * trackEnergy;
  const double sigmaEExpect = cellSubtractionManager.fudgeStdDev() * trackEnergy;

  double eSubtracted = 0.0;

  /*
   * Ring subtraction
   */
  std::map<double, RingId>::const_iterator ringIt = cellSubtractionManager.rankBegin();
  std::map<double, RingId>::const_iterator ringEnd = cellSubtractionManager.rankEnd();
  for (; ringIt != ringEnd; ++ringIt) {
    bool isFinished = subtractRings(cellSubtractionManager, ringIt->second, eSubtracted, eExpect, orderedCells, tracksClusters);
    if(isFinished) { return sigmaEExpect; }
  }

  /* Update the cluster four-momenta */
  updateClusterKinematics(tracksClusters);

  if (!(orderedCells.mapSize() > 0 && eSubtracted < eExpect)) { return sigmaEExpect; }

  /*
   * Cell subtraction
   */
  /* Increasing dR (not by layer) for cell subtraction */
  orderedCells.reorderWithoutLayers();

  bool isFinished = subtractReorderedCells(tracksClusters, eSubtracted, eExpect, orderedCells);
  if(isFinished) { return sigmaEExpect; }

  /* Update the cluster four-momenta */
  updateClusterKinematics(tracksClusters);

  return sigmaEExpect;

}

CaloClusterCellLink::iterator eflowCellSubtractionFacilitator::getCellIterator(const xAOD::CaloCluster* thisCluster, const CaloCell* thisCell){

  //SLOW! Can we move to directly work with iterators in future? 

  //We have to use non-const iterators so that we are allowed to modify the cell weights
  CaloClusterCellLink* theCells = const_cast<CaloClusterCellLink*>(thisCluster->getCellLinks());

  CaloClusterCellLink::iterator itCell=theCells->begin();
  CaloClusterCellLink::iterator endCell=theCells->end();
  for(; itCell != endCell; ++itCell) {
    const CaloCell* pCell=(*itCell);
    if (pCell==thisCell) { //Pointer comparison!
      return itCell;
    }
  }

  //if no match is found then return end of container
  return endCell;
  
}
