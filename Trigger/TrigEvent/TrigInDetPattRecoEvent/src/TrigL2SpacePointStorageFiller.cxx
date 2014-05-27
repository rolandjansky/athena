/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_SPACEPOINT_STORAGE_FILLER_CXX__
#define __TRIG_L2_SPACEPOINT_STORAGE_FILLER_CXX__

#include "TrigInDetPattRecoEvent/TrigL2SpacePointStorageFiller.h"
#include "TrigInDetEvent/TrigSiSpacePointBase.h"
#include "TrigInDetPattRecoEvent/TrigInDetRoad.h"
#include "TrigInDetPattRecoEvent/TrigSpacePointStorage.h"
#include <algorithm>
#include <iostream>



void TrigL2SpacePointStorageFiller::fillSpacePointStorage(TrigSpacePointStorage& spacePointStorage,
							  const std::vector<TrigSiSpacePointBase>& spacePointVector,
							  const std::vector<TrigInDetRoad>& roadVector) {
  for (std::vector<TrigSiSpacePointBase>::const_iterator spacePoint = spacePointVector.begin(); spacePoint != spacePointVector.end(); spacePoint++) {
    for (std::vector<TrigInDetRoad>::const_iterator road = roadVector.begin(); road != roadVector.end(); road++) {
      TrigInDetRoad::roadLayer spLayer;
      if (isCompatibleWithRoad(*spacePoint, *road, spLayer)) {
				spacePointStorage.fill(road->roadId(), road->roadSubId(), *spacePoint, spLayer);
      }
    }
  }
  return;
}

void TrigL2SpacePointStorageFiller::fillSpacePointStorage(TrigSpacePointStorage& spacePointStorage,
							  const std::vector< std::vector<const TrigSiSpacePointBase*> >& spacePointLayerVector,
							  const std::vector<TrigInDetRoad>& roadVector,
							  const TrigInDetRoad::roadLayer& whichLayer) {
  
  std::set<unsigned int> roadLayers;
  std::vector<const TrigSiSpacePointBase*> spacePointVector;
  //std::cout << "SMH: roadVector.size(): " << roadVector.size() << std::endl;
  for (auto roadIter = roadVector.begin(); roadIter != roadVector.end(); roadIter++) {
    TrigInDetRoad road = *roadIter;
    roadLayers = road.layer(whichLayer);
    //std::cout << "SMH: roadLayers.size(): " << roadLayers.size() <<std::endl;
    for (auto layerIter = roadLayers.begin(); layerIter != roadLayers.end(); layerIter++) {
      spacePointVector = spacePointLayerVector[*layerIter];
      //std::cout << "SMH: spacePointVector.size(): " << spacePointVector.size() <<std::endl;
      for (auto spacePointIter = spacePointVector.begin(); spacePointIter != spacePointVector.end(); spacePointIter++) {
	//	if (isGeometricallyCompatible(**spacePointIter, road)) {
	  spacePointStorage.fill(road.roadId(), road.roadSubId(), **spacePointIter, whichLayer);
	  //	}
      }
    }
  }
  return;
}


bool TrigL2SpacePointStorageFiller::spacePointLessEtaReverse(const TrigSiSpacePointBase* spacePointLeft, const float& etaRight) {
  return (spacePointLeft->eta() < etaRight);
}

bool TrigL2SpacePointStorageFiller::spacePointLessEta(const float& etaLeft, const TrigSiSpacePointBase* spacePointRight) {
  return (spacePointRight->eta() < etaLeft);
}



bool TrigL2SpacePointStorageFiller::greaterEta(const TrigSiSpacePointBase& spacePointLeft, const TrigSiSpacePointBase& spacePointRight) {
  return (spacePointLeft.eta() > spacePointRight.eta());
}

bool TrigL2SpacePointStorageFiller::greaterPhi(const TrigSiSpacePointBase& spacePointLeft, const TrigSiSpacePointBase& spacePointRight) {
  return (spacePointLeft.phi() > spacePointRight.phi());
}

bool TrigL2SpacePointStorageFiller::lessEta(const TrigSiSpacePointBase& spacePointLeft, const TrigSiSpacePointBase& spacePointRight) {
  return (spacePointLeft.eta() < spacePointRight.eta());
}

bool TrigL2SpacePointStorageFiller::lessPhi(const TrigSiSpacePointBase& spacePointLeft, const TrigSiSpacePointBase& spacePointRight) {
  return (spacePointLeft.phi() < spacePointRight.phi());
}

bool TrigL2SpacePointStorageFiller::isGeometricallyCompatible(const TrigSiSpacePointBase& spacePoint, const TrigInDetRoad& road) {
  if (spacePoint.eta() > road.etaMax()) return false;
  if (spacePoint.eta() < road.etaMin()) return false;
  if (spacePoint.phi() > road.phiMax()) return false;
  if (spacePoint.phi() < road.phiMin()) return false;
  return true;
}

bool TrigL2SpacePointStorageFiller::isPhiCompatible(const TrigSiSpacePointBase& spacePoint, const TrigInDetRoad& road) {
  if (spacePoint.phi() > road.phiMax()) return false;
  if (spacePoint.phi() < road.phiMin()) return false;
  return true;
}


bool TrigL2SpacePointStorageFiller::isCompatibleWithRoad (const TrigSiSpacePointBase& spacePoint, const TrigInDetRoad& road, TrigInDetRoad::roadLayer& spLayer) {
	//std::cout << "SMH: Layer compatibility" << std::endl;
	//std::cout << "SMH: Road first layer size:" << road.firstLayer().size() << std::endl;
	//std::cout << "SMH: Road second layer size:" << road.secondLayer().size() << std::endl;
	//std::cout << "SMH: Road third layer size:" << road.thirdLayer().size() << std::endl;
	//std::cout << "SMH: sp layerId:" << spacePoint.layer() << std::endl;
	//std::cout << "SMH: *(road.firstLayer().find(spacePoint.layer())): " << *(road.firstLayer().find(spacePoint.layer())) << std::endl; 
	//std::cout << "SMH: &(*(road.firstLayer().find(spacePoint.layer()))) " << &(*(road.firstLayer().find(spacePoint.layer()))) << std::endl;
	//std::cout << "SMH: &(*(road.firstLayer().end()))" << &(*(road.firstLayer().end())) << std::endl;
	//for (std::set<unsigned int>::const_iterator firstLayerIter = road.firstLayer().begin(); firstLayerIter != road.firstLayer().end(); firstLayerIter++) {
	//	std::cout << "SMH: Road first layer " << " :" << *firstLayerIter << std::endl;
	//}
	//for (std::set<unsigned int>::const_iterator secondLayerIter = road.secondLayer().begin(); secondLayerIter != road.secondLayer().end(); secondLayerIter++) {
	//	std::cout << "SMH: Road second layer " << " :" << *secondLayerIter << std::endl;
	//}

	//for (std::set<unsigned int>::const_iterator thirdLayerIter = road.thirdLayer().begin(); thirdLayerIter != road.thirdLayer().end(); thirdLayerIter++) {
	//	std::cout << "SMH: Road third layer " << " :" << *thirdLayerIter << std::endl;
	//}
	//Check geometric compatibility
	if (spacePoint.phi() > road.phiMax()) return false;
	if (spacePoint.phi() < road.phiMin()) return false;
	if (spacePoint.eta() > road.etaMax()) return false;
	if (spacePoint.eta() < road.etaMin()) return false;
	
	
	//Check layer compatibility
	if (road.thirdLayer().find(spacePoint.layer()) != road.thirdLayer().end()){
		spLayer = TrigInDetRoad::thirdRoadLayer;
		//std::cout << "SMH: Spacepoint in third layer" << std::endl;
	}
	else if (road.secondLayer().find(spacePoint.layer()) != road.secondLayer().end()){
		spLayer = TrigInDetRoad::secondRoadLayer;
		//std::cout << "SMH: Spacepoint in second layer" << std::endl;
	}
	else if (road.firstLayer().find(spacePoint.layer()) != road.firstLayer().end()){
		spLayer = TrigInDetRoad::firstRoadLayer;
		//std::cout << "SMH: Spacepoint in first layer" << std::endl;
	}
	else {
		//std::cout << "SMH: Spacepoint in no layer" << std::endl;
		return false;
	}
	
	return true;
}
#endif
