/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_IN_DET_ROAD_H__
#define __TRIG_IN_DET_ROAD_H__

#include<cmath>
#include<set>
#include<algorithm>
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

//class TrigRoiDescriptor; TODO: forward declaration?

typedef class TrigInDetRoad {

  friend class TrigInDetRoadMakerTool;
  //friend class TrigInDetSpacePointStorage;

public:
 TrigInDetRoad() = delete; //to prevent creation w/o initialization
 TrigInDetRoad(unsigned int id, float pt, float d0) : m_roadId(id), m_minPt(pt), m_maxD0(d0),
    m_eta(0.0), m_etaMin(-5.0), m_etaMax(5.0), m_phi(0.0), m_phiMin(-M_PI), 
    m_phiMax(M_PI), m_z0(0.0), m_z0Min(-168.0), m_z0Max(168.0) {};
 TrigInDetRoad(const TrigInDetRoad& r) : m_roadId(r.m_roadId), m_roadSubId(r.m_roadSubId), m_minPt(r.m_minPt), 
    m_maxD0(r.m_maxD0), 
    m_eta(r.m_eta), m_etaMin(r.m_etaMin), m_etaMax(r.m_etaMax), 
    m_phi(r.m_phi), m_phiMin(r.m_phiMin), m_phiMax(r.m_phiMax), 
    m_z0(r.m_z0), m_z0Min(r.m_z0Min), m_z0Max(r.m_z0Max), 
    m_firstLayer(r.m_firstLayer), 
    m_secondLayer(r.m_secondLayer), 
    m_thirdLayer(r.m_thirdLayer) {};
  TrigInDetRoad& operator = (const TrigInDetRoad& r) {
    m_roadId = r.m_roadId;
    m_roadSubId = r.m_roadSubId;
    m_minPt = r.m_minPt; 
    m_maxD0 = r.m_maxD0; 
    m_eta = r.m_eta;
    m_etaMin = r.m_etaMin; 
    m_etaMax = r.m_etaMax; 
    m_phi = r.m_phi;
    m_phiMin = r.m_phiMin;
    m_phiMax = r.m_phiMax; 
    m_z0 = r.m_z0;
    m_z0Min = r.m_z0Min;
    m_z0Max = r.m_z0Max; 

    m_firstLayer = r.m_firstLayer;
    m_secondLayer = r.m_secondLayer;
    m_thirdLayer = r.m_thirdLayer;

    return *this;
  }

	bool operator < (const TrigInDetRoad& rhs) const {
		if (this->m_roadId < rhs.m_roadId) {
			return true;
		}
		else if (this->m_roadId == rhs.m_roadId) {
			return this->m_roadSubId < rhs.m_roadSubId;
		}
		else {
			return false;
		}
	}

	enum roadLayer {
		firstRoadLayer,
		secondRoadLayer,
		thirdRoadLayer
	};
  unsigned int roadId()    const {return m_roadId;}
  unsigned int roadSubId() const {return m_roadSubId;}
  float minPt() const {return m_minPt;}
  float axD0() const {return m_maxD0;}
  float eta() const {return m_eta;}
  float etaMin() const {return m_etaMin;}
  float etaMax() const {return m_etaMax;}
  float phi() const {return m_phi;}
  float phiMin() const {return m_phiMin;}
  float phiMax() const {return m_phiMax;}
  float z0() const {return m_z0;}
	float z0Min() const {return m_z0Min;}
  float z0Max() const {return m_z0Max;}

  const std::set<unsigned int>& firstLayer()  const {return m_firstLayer;}
  const std::set<unsigned int>& secondLayer() const {return m_secondLayer;}
  const std::set<unsigned int>& thirdLayer()  const {return m_thirdLayer;}

  const std::set<unsigned int>& layer(const roadLayer& layer)  const {
		switch(layer) {
			case firstRoadLayer:
				return m_firstLayer;
				break;
			case secondRoadLayer:
				return m_secondLayer;
				break;
			case thirdRoadLayer:
				return m_thirdLayer;
				break;
			default:
				return m_thirdLayer;
				break;
		}
	}
protected:
  unsigned int m_roadId;
  unsigned int m_roadSubId;
  // Kinematic boundaries
  float m_minPt;
  float m_maxD0;
  // Geometric boundaries
  float m_eta, m_etaMin, m_etaMax;    // eta boundaries
  float m_phi, m_phiMin, m_phiMax;    // phi boundaries
  float m_z0, m_z0Min, m_z0Max;       // z0 boundaries
  std::set<unsigned int> m_firstLayer;
  std::set<unsigned int> m_secondLayer;
  std::set<unsigned int> m_thirdLayer;
} TRIG_ID_ROAD;

#endif
