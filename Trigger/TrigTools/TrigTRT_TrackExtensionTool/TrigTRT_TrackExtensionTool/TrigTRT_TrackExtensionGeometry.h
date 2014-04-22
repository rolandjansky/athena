/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_TRT_GEOMETRY_H__
#define __TRIG_TRT_GEOMETRY_H__

#include<stdio.h>
#include<stdlib.h>

#include<GaudiKernel/ToolHandle.h>

class TrigTRT_DetElement;
class TrigTRT_DetElementRoad;
class TrigTRT_DetElementPoint;
class TrigMagneticFieldTool;
class TrigTRT_BasePDAF;
class TRT_ID;


namespace InDetDD {
  class TRT_DetectorManager;
}

namespace InDet {
  class TRT_DriftCircle;
}

namespace Trk {
  class TrkPlanarSurface;
  class TrkTrackState;
}


class TrigTRT_Info
{
 public:
  TrigTRT_Info();
  ~TrigTRT_Info(){m_driftCircles.clear();}
  void m_addTR_HitWeight(double,bool);
  void m_addTRT_HitWeight(double,bool);
  void m_addDetectionWeight(double);
  void m_addCrossedLayer();
  void m_addCrossedStraw(double);
  double m_getTRT_DriftTime_Sum();
  double m_getTR_DriftTime_Sum();
  double m_getTRT_NoDriftTime_Sum();
  double m_getTR_NoDriftTime_Sum();
  double m_getCrossedStraws();
  int m_getCrossedLayers();
  double m_getDetectionWeight();
  void m_addTRT_Hit(const InDet::TRT_DriftCircle*);
  std::vector<const InDet::TRT_DriftCircle*>& m_getTRT_Hits();
 private:
  double m_trHitsDT,m_trtHitsDT;
  double m_trHitsNoDT,m_trtHitsNoDT;
  int m_nCrossedLayers;
  double m_crossedStraws;
  double m_detectionSum;
  std::vector<const InDet::TRT_DriftCircle*> m_driftCircles;
};

class TrigTRT_Straw
{
 public:
  TrigTRT_Straw();
  TrigTRT_Straw(class TrigTRT_Straw*);
  ~TrigTRT_Straw(){};
  void m_setLocalCoordinate(double);
  double m_localCoordinate();
  void m_setDC(const InDet::TRT_DriftCircle*);
  const InDet::TRT_DriftCircle* m_getDC();
 private:
  double m_loc;
  const InDet::TRT_DriftCircle* m_pDC;
};

class TrigTRT_TrackExtensionLayer
{
 public:
  TrigTRT_TrackExtensionLayer(int,double[10],int,ToolHandle<TrigMagneticFieldTool>,const TRT_ID*,TrigTRT_BasePDAF*);
  ~TrigTRT_TrackExtensionLayer();
  void m_addElement(TrigTRT_DetElement*);
  void m_dump(FILE*);
  TrigTRT_DetElementPoint* m_findRoadPoint(double*);
  std::vector<TrigTRT_Straw*>* m_getVectorOfStraws(int,const double[3],Trk::TrkTrackState*);
  Trk::TrkPlanarSurface* m_createSurface(int);
  bool m_isInAcceptance(Trk::TrkTrackState*);
  const TRT_ID* m_getTRT_Helper();
  TrigTRT_BasePDAF* m_getUpdator();
 private:
  int m_type,m_nStraws;
  double m_minBound,m_maxBound,m_refCoord;
  double m_phi0,m_phiMax,m_dphi;
  double m_xFirst,m_xLast,m_yFirst,m_yLast;
  double m_phiPitch;
  std::vector<TrigTRT_DetElement*> m_vpElements;
  ToolHandle<TrigMagneticFieldTool> m_pMagTool;
  const TRT_ID* m_trtHelper;
  std::vector<TrigTRT_Straw*> m_vpStraws;
  TrigTRT_BasePDAF* m_pdafUpdator;
};

class TrigTRT_TrackExtensionRegion
{
 public:
  TrigTRT_TrackExtensionRegion(int);
  ~TrigTRT_TrackExtensionRegion();
  void m_addLayer(TrigTRT_TrackExtensionLayer*);
  void m_setType(int);
  void m_dump(FILE*);
  void m_report();
  bool m_isInAcceptance(double[6]);
  void m_setBoundaries(double,double,double,double);
  void m_collectDetElements(TrigTRT_DetElementRoad*,double[6]);
 private:
  std::vector<TrigTRT_TrackExtensionLayer*> m_vpLayers;
  int m_type;
  double m_minZ,m_maxZ,m_minR,m_maxR;
};


class TrigTRT_TrackExtensionGeometry
{
 public:
  TrigTRT_TrackExtensionGeometry(const InDetDD::TRT_DetectorManager*,
				 ToolHandle<TrigMagneticFieldTool>,TrigTRT_BasePDAF*,TrigTRT_BasePDAF*,const TRT_ID*);
  ~TrigTRT_TrackExtensionGeometry();
  TrigTRT_DetElementRoad* m_buildTRT_Road(double[6]);
  void m_dump(char fileName[]);
 private:
  TrigTRT_TrackExtensionRegion* m_pRegions[4];
  TrigTRT_BasePDAF* m_barrelUpdator;
  TrigTRT_BasePDAF* m_endcapUpdator;
};


#endif
