/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTRTTRACKEXTENSIONTOOL_TRIGTRT_DETELEMENTROAD_H
#define TRIGTRTTRACKEXTENSIONTOOL_TRIGTRT_DETELEMENTROAD_H

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
  void addDC_Collection(const InDet::TRT_DriftCircleCollection*);
  const InDet::TRT_DriftCircleCollection* getDC_Collection();
  IdentifierHash getHashId();
  int getSectorId();
  void dump(FILE*);
  void report();
  const InDetDD::TRT_BaseElement* getDetectorElement();
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
  void addDetectorElement(TrigTRT_DetElement*);
  TrigTRT_DetElement* detectorElement();
  void report();
  void updateTrackState(Trk::TrkTrackState*,TrigTRT_Info*,Trk::TrkTrackState* pTS=NULL);
  Trk::TrkPlanarSurface* createSurface();
  bool hasHits();
  int getType();
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
  void addPoint(TrigTRT_DetElementPoint*);
  std::vector<TrigTRT_DetElementPoint*>* roadPoints();
  void collectDetectorElements(std::list<TrigTRT_DetElement*>*);
  void report();
 private:
  std::vector<TrigTRT_DetElementPoint*> m_vpPoints;
};



#endif
