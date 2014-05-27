/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_SPACE_POINT_STORAGE_H__
#define __TRIG_SPACE_POINT_STORAGE_H__
#include <map>
#include <utility>
#include <vector>
#include "TrigInDetPattRecoEvent/TrigInDetRoad.h"

class TrigSiSpacePointBase;

struct TrigL2TripletSpacePointVector {
  std::vector<const TrigSiSpacePointBase*> m_spacePointFirstLayerVector;
  std::vector<const TrigSiSpacePointBase*> m_spacePointSecondLayerVector;
  std::vector<const TrigSiSpacePointBase*> m_spacePointThirdLayerVector;
};

class TrigSpacePointStorage {
 public:
 TrigSpacePointStorage() : m_empty(std::vector<const TrigSiSpacePointBase*>(0)) {};
  void fill(const unsigned int& roadId, const unsigned int& roadSubId, const TrigSiSpacePointBase& spacePoint, const TrigInDetRoad::roadLayer& spLayer);
  
  const std::vector<const TrigSiSpacePointBase*>& spacePointVector(const unsigned int& roadId, const unsigned int& roadSubId, const TrigInDetRoad::roadLayer& layer) const;
  const std::vector<const TrigSiSpacePointBase*>& firstLayerSpacePointVector(const unsigned int& roadId, const unsigned int& roadSubId) const;
  const std::vector<const TrigSiSpacePointBase*>& secondLayerSpacePointVector(const unsigned int& roadId, const unsigned int& roadSubId) const;
  const std::vector<const TrigSiSpacePointBase*>& thirdLayerSpacePointVector(const unsigned int& roadId, const unsigned int& roadSubId) const;
 private:
  std::map<std::pair<unsigned int, unsigned int>, TrigL2TripletSpacePointVector> m_spacePointRoadMap;
  std::vector<const TrigSiSpacePointBase*> m_empty;
};

#endif
