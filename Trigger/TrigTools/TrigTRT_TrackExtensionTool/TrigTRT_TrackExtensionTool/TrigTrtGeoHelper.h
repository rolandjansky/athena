/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIGTRTGEOHELPER_H__
#define __TRIGTRTGEOHELPER_H__

namespace Trk {	
  class TrkTrtPdafNode;
  class TrkBaseNode;
  class TrkTrackState;
}

class DataVector;

namespace InDet {	
  class TRT_DriftCircle;
}

namespace InDetDD {
  class TRT_DetectorManager;
}

typedef class TrigTrtRoadPoint
{
 public:
  TrigTrtRoadPoint();
  ~TrigTrtRoadPoint();
  TrkTrtPdafNode* m_createFilteringNode();
 private:
  std::vector<class TrigTrtDetectorElement*> m_vpDetEl;
  double x,y,z,path;
} TRIG_TRT_RP,*pTRIG_TRT_RP;

typedef class TrigTrtRoad
{
 public:
  TrigTrtRoad();
  ~TrigTrtRoad();
  void m_createFilteringNodes(std::vector<Trk::TrkBaseNode>&);
  void m_collectDetectorElements(std::list<int>);
  void m_sortPoints();
 private:
  std::vector<pTRIG_TRT_RP> m_vpPoints;
} TRIG_TRT_ROAD, *pTRIG_TRT_ROAD;

typedef class TrigTrtDetectorElement
{
 public:
  TrigTrtDetectorElement();
  ~TrigTrtDetectorElement();
  void m_setDC(DataVector<InDet::TRT_DriftCircle>*);
  DataVector<InDet::TRT_DriftCircle>* m_getDC();
 private:
  DataVector<InDet::TRT_DriftCircle>* m_vpDC;
} TRIG_TRT_DE, *pTRIG_TRT_DE;

typedef class TrigTrtLayer
{
 public:
  TrigTrtLayer();
  ~TrigTrtLayer();
  pTRIG_TRT_RP m_findRoadPoint(double[6]);
 private:
  int m_type;
  double m_minBound,m_maxBound,m_refCoord;
  double m_Phi0,m_deltaPhi,m_PhiWidth;
  std::vector<pTRIG_TRT_DE> m_vpDE;
} TRIG_TRT_LAYER, *pTRIG_TRT_LAYER;

typedef class TrigTrtRegion
{
 public:
  TrigTrtRegion(const InDetDD::TRT_DetectorManager*,int);
  ~TrigTrtRegion();
  bool m_isInAcceptance(double[6]);
  pTRIG_TRT_ROAD m_createRoad(double[6]);
 private:
  std::vector<pTRIG_TRT_LAYER> m_vpLayers;
} TRIG_TRT_LAYER, *pTRIG_TRT_LAYER;

typedef class TrigTrtGeo
{
 public:
  TrigTrtGeo(const InDetDD::TRT_DetectorManager*);
  ~TrigTrtGeo();
  pTRIG_TRT_ROAD m_createRoad(Trk::TrkTrackState*);
 private:
  pTRIG_TRT_REGION m_pRegions[4];
} TRIG_TRT_GEO, *pTRIG_TRT_GEO;


#endif
