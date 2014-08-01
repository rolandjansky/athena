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

eflowCellSubtractionFacilitator::eflowCellSubtractionFacilitator() {
  //initialise data members
  m_annFlag = false;
}

double eflowCellSubtractionFacilitator::subtractCells(eflowCellSubtractionManager& cellSubtractionManager, double trackEnergy, xAOD::CaloCluster* tracksCluster, eflowCellList& orderedCells) {
  return subtractCells(cellSubtractionManager, trackEnergy, std::vector<xAOD::CaloCluster*> (1,tracksCluster), orderedCells);
}

double eflowCellSubtractionFacilitator::subtractCells(eflowCellSubtractionManager& cellSubtractionManager, double trackEnergy, std::vector<xAOD::CaloCluster*> tracksClusters, eflowCellList& orderedCells) {

  int debug = 0;

  std::map<double,RingId>::const_iterator ringIt = cellSubtractionManager.rankBegin();
  std::map<double,RingId>::const_iterator ringEnd = cellSubtractionManager.rankEnd();

  const double eExpect = cellSubtractionManager.fudgeMean() * trackEnergy;
  const double sigmaEExpect = cellSubtractionManager.fudgeStdDev() * trackEnergy;

  if (1 == debug) { std::cout << "So eExpect is " << eExpect << std::endl; }

  std::vector<xAOD::CaloCluster*>::iterator firstClus = tracksClusters.begin();
  std::vector<xAOD::CaloCluster*>::iterator lastClus = tracksClusters.end();

  double eClusOld = 0;

  for (; firstClus != lastClus; firstClus++) {
    eClusOld += (*firstClus)->e();
  }

  double eSubt = 0.0;

  for (ringIt = cellSubtractionManager.rankBegin(); ringIt != ringEnd; ++ringIt){

    const std::pair<eflowCaloENUM,short>& ring = ringIt->second;

    const eflowCaloENUM subtLayer = ring.first;
    const short ringNo = ring.second;

    const double r1 = ringNo * cellSubtractionManager.ringThickness(subtLayer);
    const double r2 = (ringNo + 1) * cellSubtractionManager.ringThickness(subtLayer);

    CellIt begin = orderedCells.getLowerBound(subtLayer, r1);
    CellIt end = orderedCells.getLowerBound(subtLayer, r2);


    /*  * * *  Do Subtraction * * * */

    /* Find total energy in ring */
    double eRing = 0.0;
    for (CellIt it = begin; it != end; ++it) {
      std::vector<std::pair<CaloCell*,int> >::iterator   itEntry = it->second.begin();
      std::vector<std::pair<CaloCell*,int> >::iterator lastEntry = it->second.end();
      for (; itEntry != lastEntry; itEntry++){
        std::pair<CaloCell*,int> thisPair = *itEntry;

        const xAOD::CaloCluster* clus = tracksClusters[thisPair.second];
        CaloClusterCellLink::iterator theIterator = this->getCellIterator(clus,thisPair.first);	
        double cellWeight = theIterator.weight();
	
        eRing += thisPair.first->energy()*cellWeight;
        if (1 == debug) std::cout << "added cell with e " << thisPair.first->energy()*cellWeight << " from cluster with e " << clus->e() << " to ring " << std::endl;
      }//loop over all pairs in vector
    }//loop over CellIt

    // subtract energy from ring

    if (1 == debug) std::cout << "ring energy is " << eRing << std::endl;
    if (1 == debug) std::cout << "eSubt is and eExpect is " << eSubt << " and " << eExpect << std::endl;

    if (eSubt + eRing > eExpect){  // don't need to subtract whole ring

      for (CellIt it = begin; it != end; ++it){

        std::vector<std::pair<CaloCell*,int> >::iterator   itEntry = it->second.begin();
        std::vector<std::pair<CaloCell*,int> >::iterator lastEntry = it->second.end();
        for (; itEntry != lastEntry; itEntry++){
          std::pair<CaloCell*,int> thisPair = *itEntry;

	  xAOD::CaloCluster* cluster = tracksClusters[thisPair.second];
          CaloCell* cell = thisPair.first;
          CaloClusterCellLink::iterator theIterator = this->getCellIterator(cluster,cell);	
          double oldCellWeight = theIterator.weight();
          const double newCellWeight = oldCellWeight * ( 1.0 - (eExpect - eSubt) / eRing);
          theIterator.reweight(newCellWeight);
        } //loop over all pairs in vector
      } //loop over CellIt

      eSubt = eExpect;

      std::vector<xAOD::CaloCluster*>::iterator itClus = tracksClusters.begin();
      
      for (; itClus != lastClus; itClus++){
	xAOD::CaloCluster* theCluster = *itClus;
	float oldEnergy = theCluster->e();
	CaloClusterKineHelper::calculateKine(theCluster,true, true);
	if (0.0 != oldEnergy){
	  float energyAdjustment = theCluster->e()/oldEnergy;
	  theCluster->setRawE( theCluster->rawE()*energyAdjustment );
	  theCluster->setRawEta( theCluster->eta() );
	  theCluster->setRawPhi( theCluster->phi() );
	}
      }

      return sigmaEExpect;  // finished subtraction

    } else {  // subtract whole ring
      if (1 == debug) std::cout << " subtract full ring " << std::endl;

      for (CellIt it = begin; it != end; ++it) {

        std::vector<std::pair<CaloCell*,int> >::iterator   itEntry = it->second.begin();
        std::vector<std::pair<CaloCell*,int> >::iterator lastEntry = it->second.end();
        for (; itEntry != lastEntry; itEntry++){
          std::pair<CaloCell*,int> thisPair = *itEntry;

	  xAOD::CaloCluster* cluster = tracksClusters[thisPair.second];
          cluster->removeCell(thisPair.first);
        }//loop over all pairs in vector
      }//loop over CellIt
      orderedCells.deleteFromList(begin, end);
      eSubt += eRing;
    }

    if (1 == debug) { std::cout << " now esubt is " << eSubt << std::endl; }

    if ( fabs(eClusOld - eSubt) < 1.0e-6 * eClusOld )  {
      //annihilate clusters

      std::vector<xAOD::CaloCluster*>::iterator itClus = tracksClusters.begin();

      for (; itClus != lastClus; itClus++){

        const CaloClusterCellLink* theCellLink = (*itClus)->getCellLinks();
	CaloClusterCellLink* theCellLink_nonConst = const_cast<CaloClusterCellLink*>(theCellLink);
	CaloClusterCellLink::iterator theFirstCell = theCellLink_nonConst->begin();
	CaloClusterCellLink::iterator theLastCell = theCellLink_nonConst->end();
	
	for (; theFirstCell != theLastCell; ++theFirstCell){
          (*itClus)->removeCell(*theFirstCell);
        }
        (*itClus)->setCalE(0.0);
        (*itClus)->setRawE(0.0);

        orderedCells.eraseList();
        m_annFlag = true;

      }//clus loop

      itClus = tracksClusters.begin();

      for (; itClus != lastClus; itClus++){
	xAOD::CaloCluster* theCluster = *itClus;
	float oldEnergy = theCluster->e();
	CaloClusterKineHelper::calculateKine(theCluster,true, true);
	if (0.0 != oldEnergy){
	  float energyAdjustment = theCluster->e()/oldEnergy;
	  theCluster->setRawE( theCluster->rawE()*energyAdjustment );
	  theCluster->setRawEta( theCluster->eta() );
	  theCluster->setRawPhi( theCluster->phi() );
	}
      }

      return sigmaEExpect;
    }//if no energy left
  }//end of loop on cell rings in order

  //update the clusters having done the subtraction
  std::vector<xAOD::CaloCluster*>::iterator itClus = tracksClusters.begin();

  for (; itClus != lastClus; itClus++){
    xAOD::CaloCluster* theCluster = *itClus;
    float oldEnergy = theCluster->e();
    CaloClusterKineHelper::calculateKine(theCluster,true, true);
    if (0.0 != oldEnergy){
      float energyAdjustment = theCluster->e()/oldEnergy;
      theCluster->setRawE( theCluster->rawE()*energyAdjustment );
      theCluster->setRawEta( theCluster->eta() );
      theCluster->setRawPhi( theCluster->phi() );
    }
  }

  if (1 == debug) std::cout << " here esubt and eExpect are finally " << eSubt << " and " << eExpect << std::endl;

  if ( orderedCells.mapSize() > 0 && eSubt < eExpect ) {

    orderedCells.reorderWithoutLayers();  // just want to go through in order of increasing dR (not by layer)

    CellIt end = orderedCells.end();
    for (CellIt it = orderedCells.begin(); it != end;){

      std::vector<std::pair<CaloCell*,int> >::iterator   itEntry = it->second.begin();
      std::vector<std::pair<CaloCell*,int> >::iterator lastEntry = it->second.end();
      /* Don't need a for loop here - "incrementation" is done implicitly when erasing a pair from the vector */


      while (itEntry != lastEntry){

        std::pair<CaloCell*,int> thisPair = *itEntry;

	xAOD::CaloCluster* cluster = tracksClusters[thisPair.second];
        CaloCell* cell = thisPair.first;

        CaloClusterCellLink::iterator theIterator = this->getCellIterator(cluster,cell);	
        double oldCellWeight = theIterator.weight();

        double oldCellEnergy = cell->energy()*oldCellWeight;

        if (eSubt + oldCellEnergy > eExpect && oldCellEnergy!=0.) {

          double temp = eExpect - eSubt;
       
          double newCellWeight = oldCellWeight * ((oldCellEnergy - temp)/oldCellEnergy);
          theIterator.reweight(newCellWeight);

          //cell->setEnergy(oldCellEnergy - temp);
          eSubt = eExpect;

          //update the clusters having done the subtraction
	  itClus = tracksClusters.begin();

	  for (; itClus != lastClus; itClus++){
	    xAOD::CaloCluster* theCluster = *itClus;
	    float oldEnergy = theCluster->e();
	    CaloClusterKineHelper::calculateKine(theCluster,true, true);
	    if (0.0 != oldEnergy){
	      float energyAdjustment = theCluster->e()/oldEnergy;
	      theCluster->setRawE( theCluster->rawE()*energyAdjustment );
	      theCluster->setRawEta( theCluster->eta() );
	      theCluster->setRawPhi( theCluster->phi() );
	    }
	  }

          return sigmaEExpect;

        } else {

          double oldClusterEnergy = cluster->e();
          cluster->removeCell(cell);
          //update the clusters having done the subtraction
	  CaloClusterKineHelper::calculateKine(cluster,true, true);
          double newClusterEnergy = cluster->e();
          if (0.0 != oldClusterEnergy) {
            cluster->setRawE(cluster->rawE()*newClusterEnergy/oldClusterEnergy);
	    cluster->setRawEta( cluster->eta() );
	    cluster->setRawPhi( cluster->phi() );
          }
          eSubt += oldCellEnergy;
          itEntry = it->second.erase(itEntry);
          lastEntry = it->second.end(); // gets invalidated by the erase method otherwise, resulting in an endless loop
        }

      }//loop over all pairs in vector

      //check whether cluster has been killed
      if (0 == it->second.size()){
        CellIt eraseIt = it;
        ++it;
        orderedCells.deleteFromList(eraseIt);
      } else {
        ++it;
      }
    }//Cell It Loop
  }

  itClus = tracksClusters.begin();

  for (; itClus != lastClus; itClus++){
    xAOD::CaloCluster* theCluster = *itClus;
    float oldEnergy = theCluster->e();
    CaloClusterKineHelper::calculateKine(theCluster,true, true);
    if (0.0 != oldEnergy){
      float energyAdjustment = theCluster->e()/oldEnergy;
      theCluster->setRawE( theCluster->rawE()*energyAdjustment );
      theCluster->setRawEta( theCluster->eta() );
      theCluster->setRawPhi( theCluster->phi() );
    }
  }

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
