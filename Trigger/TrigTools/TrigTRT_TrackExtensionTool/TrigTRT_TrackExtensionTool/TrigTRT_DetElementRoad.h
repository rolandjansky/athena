/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_TRT_DETELROAD_H__
#define __TRIG_TRT_DETELROAD_H__

#include<stdio.h>
#include<stdlib.h>
#include "InDetPrepRawData/TRT_DriftCircleCollection.h"

class IdentifierHash;
class TrigTRT_TrackExtensionLayer;
class TrigTRT_Info;

namespace Trk {
  class TrkTrackState;
  class TrkPlanarSurface;
}

namespace InDetDD {
  class TRT_BaseElement;
}

class TrigTRT_DetElement
{
 public:
  TrigTRT_DetElement(IdentifierHash,int,const InDetDD::TRT_BaseElement*);
  ~TrigTRT_DetElement(){};
  void m_addDC_Collection(const InDet::TRT_DriftCircleCollection*);
  const InDet::TRT_DriftCircleCollection* m_getDC_Collection();
  IdentifierHash m_getHashId();
  int m_getSectorId();
  void m_dump(FILE*);
  void m_report();
  const InDetDD::TRT_BaseElement* m_getDetectorElement();
 private:
  IdentifierHash m_hashId;
  const InDet::TRT_DriftCircleCollection* m_pDC;  
  int m_sectorId;
  const InDetDD::TRT_BaseElement* m_pDE;
};

class TrigTRT_DetElementPoint
{
 public:
  TrigTRT_DetElementPoint(double[3],int,TrigTRT_TrackExtensionLayer*);
  ~TrigTRT_DetElementPoint(){};
  void m_addDetectorElement(TrigTRT_DetElement*);
  TrigTRT_DetElement* m_detectorElement();
  void m_report();
  void m_updateTrackState(Trk::TrkTrackState*,TrigTRT_Info*,Trk::TrkTrackState* pTS=NULL);
  Trk::TrkPlanarSurface* m_createSurface();
  bool m_hasHits();
  int m_getType();
 private:
  double m_Coordinates[3],m_distance;
  TrigTRT_DetElement* m_pElement;
  int m_type;  
  TrigTRT_TrackExtensionLayer* m_pTRT_Layer;
};

class TrigTRT_DetElementRoad
{
 public:
  TrigTRT_DetElementRoad();
  ~TrigTRT_DetElementRoad();
  void m_addPoint(TrigTRT_DetElementPoint*);
  std::vector<TrigTRT_DetElementPoint*>* m_roadPoints();
  void m_collectDetectorElements(std::list<TrigTRT_DetElement*>*);
  void m_report();
 private:
  std::vector<TrigTRT_DetElementPoint*> m_vpPoints;
};



#endif
