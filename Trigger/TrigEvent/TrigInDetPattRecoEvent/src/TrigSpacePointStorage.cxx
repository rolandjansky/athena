/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_SPACEPOINT_STORAGE_CXX__
#define __TRIG_SPACEPOINT_STORAGE_CXX__
#include "TrigInDetPattRecoEvent/TrigSpacePointStorage.h"
#include <iostream>

void TrigSpacePointStorage::fill(const unsigned int& roadId, const unsigned int& roadSubId, const TrigSiSpacePointBase& spacePoint, 
				 const TrigInDetRoad::roadLayer& spLayer) {
  //std::cout << "SMH: In TrigSpacePointStorage::fill() spLayer: " << spLayer << std::endl;
  //std::cout << "SMH: roadId: " << roadId << std::endl;
  std::pair<unsigned int, unsigned int> roadFullId = std::make_pair(roadId, roadSubId);
  if (spLayer == TrigInDetRoad::firstRoadLayer){
    m_spacePointRoadMap[roadFullId].m_spacePointFirstLayerVector.push_back(&spacePoint);
    //std::cout << "SMH: In TrigSpacePointStorage::fill() FirstLayer: " << std::endl;
    //std::cout << "SMH: FirstLayerVector.size(): " << m_spacePointRoadMap[roadFullId].m_spacePointFirstLayerVector.size() << std::endl;
    //std::cout << "SMH: SecondLayerVector.size(): " << m_spacePointRoadMap[roadFullId].m_spacePointSecondLayerVector.size() << std::endl;
    //std::cout << "SMH: ThirdLayerVector.size(): " << m_spacePointRoadMap[roadFullId].m_spacePointThirdLayerVector.size() << std::endl;
  }
  else if (spLayer == TrigInDetRoad::secondRoadLayer){
    m_spacePointRoadMap[roadFullId].m_spacePointSecondLayerVector.push_back(&spacePoint);
    //std::cout << "SMH: In TrigSpacePointStorage::fill() SecondLayer: " << std::endl;
    //std::cout << "SMH: FirstLayerVector.size(): " << m_spacePointRoadMap[roadFullId].m_spacePointFirstLayerVector.size() << std::endl;
    //std::cout << "SMH: SecondLayerVector.size(): " << m_spacePointRoadMap[roadFullId].m_spacePointSecondLayerVector.size() << std::endl;
    //std::cout << "SMH: ThirdLayerVector.size(): " << m_spacePointRoadMap[roadFullId].m_spacePointThirdLayerVector.size() << std::endl;
  }
  else if (spLayer == TrigInDetRoad::thirdRoadLayer) {
    m_spacePointRoadMap[roadFullId].m_spacePointThirdLayerVector.push_back(&spacePoint);
    //std::cout << "SMH: In TrigSpacePointStorage::fill() ThirdLayer: " << std::endl;
    //std::cout << "SMH: FirstLayerVector.size(): " << m_spacePointRoadMap[roadFullId].m_spacePointFirstLayerVector.size() << std::endl;
    //std::cout << "SMH: SecondLayerVector.size(): " << m_spacePointRoadMap[roadFullId].m_spacePointSecondLayerVector.size() << std::endl;
    //std::cout << "SMH: ThirdLayerVector.size(): " << m_spacePointRoadMap[roadFullId].m_spacePointThirdLayerVector.size() << std::endl;
  }
}

const std::vector<const TrigSiSpacePointBase*>& TrigSpacePointStorage::spacePointVector(const unsigned int& roadId, 
											const unsigned int& roadSubId, const TrigInDetRoad::roadLayer& layer) const {
  std::pair<unsigned int, unsigned int> roadFullId = std::make_pair(roadId, roadSubId);
  auto spacePointRoadMapIter = m_spacePointRoadMap.find(roadFullId);
  if (spacePointRoadMapIter != m_spacePointRoadMap.end()) {
    if (layer == TrigInDetRoad::firstRoadLayer){
      //return &(spacePointRoadMapIter->second.m_spacePointFirstLayerVector);
      return (m_spacePointRoadMap.at(roadFullId).m_spacePointFirstLayerVector);
    }
    else if (layer == TrigInDetRoad::secondRoadLayer){
      //return &(spacePointRoadMapIter->second.m_spacePointSecondLayerVector);
      return (m_spacePointRoadMap.at(roadFullId).m_spacePointSecondLayerVector);
    }
    else if (layer == TrigInDetRoad::thirdRoadLayer){
      //return &(spacePointRoadMapIter->second.m_spacePointThirdLayerVector);
      return (m_spacePointRoadMap.at(roadFullId).m_spacePointThirdLayerVector);
    }
  }
  return m_empty;
}

const std::vector<const TrigSiSpacePointBase*>& TrigSpacePointStorage::firstLayerSpacePointVector(const unsigned int& roadId, const unsigned int& roadSubId) const {
	return spacePointVector(roadId, roadSubId, TrigInDetRoad::firstRoadLayer);
}
const std::vector<const TrigSiSpacePointBase*>& TrigSpacePointStorage::secondLayerSpacePointVector(const unsigned int& roadId, const unsigned int& roadSubId) const {
	return spacePointVector(roadId, roadSubId, TrigInDetRoad::secondRoadLayer);
}
const std::vector<const TrigSiSpacePointBase*>& TrigSpacePointStorage::thirdLayerSpacePointVector(const unsigned int& roadId, const unsigned int& roadSubId) const {
	return spacePointVector(roadId, roadSubId, TrigInDetRoad::thirdRoadLayer);
}

#endif
