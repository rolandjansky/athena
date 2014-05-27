/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_SPACEPOINT_STORAGE_FILLER_H__
#define __TRIG_L2_SPACEPOINT_STORAGE_FILLER_H__
#include <map>
#include <vector>
#include <TrigInDetPattRecoEvent/TrigInDetRoad.h>

class TrigSiSpacePointBase;
class TrigSpacePointStorage;

class TrigL2SpacePointStorageFiller {
 public:
	void fillSpacePointStorage(TrigSpacePointStorage& spacePointStorage,
																														const std::vector<TrigSiSpacePointBase>& spacePointVector,
																														const std::vector<TrigInDetRoad>& roadVector);

	void fillSpacePointStorage(TrigSpacePointStorage& spacePointStorage,
																													const std::vector< std::vector<const TrigSiSpacePointBase*> >& spacePointLayerVector,
																													const std::vector<TrigInDetRoad>& roadVector,
																													const TrigInDetRoad::roadLayer& layer);
	static bool isCompatibleWithRoad (const TrigSiSpacePointBase& spacePoint, const TrigInDetRoad& road, TrigInDetRoad::roadLayer& spLayer);
	static bool isGeometricallyCompatible (const TrigSiSpacePointBase& spacePoint, const TrigInDetRoad& road);
	static bool isPhiCompatible (const TrigSiSpacePointBase& spacePoint, const TrigInDetRoad& road);
	static bool greaterEta(const TrigSiSpacePointBase& spacePointLeft, const TrigSiSpacePointBase& spacePointRight);
	static bool greaterPhi(const TrigSiSpacePointBase& spacePointLeft, const TrigSiSpacePointBase& spacePointRight);
	static bool lessEta(const TrigSiSpacePointBase& spacePointLeft, const TrigSiSpacePointBase& spacePointRight);
	static bool lessPhi(const TrigSiSpacePointBase& spacePointLeft, const TrigSiSpacePointBase& spacePointRight);
	static bool spacePointLessEtaReverse(const TrigSiSpacePointBase* spacePointLeft, const float& etaRight);
	static bool spacePointLessEta(const float& etaLeft, const TrigSiSpacePointBase* spacePointRight);
};

#endif
