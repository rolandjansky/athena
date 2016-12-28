/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTRTTRACKEXTENSIONTOOL_TRIGTRT_TRACKEXTENSIONGEOMETRY_H
#define TRIGTRTTRACKEXTENSIONTOOL_TRIGTRT_TRACKEXTENSIONGEOMETRY_H

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
  void addTR_HitWeight(double,bool);
  void addTRT_HitWeight(double,bool);
  void addDetectionWeight(double);
  void addCrossedLayer();
  void addCrossedStraw(double);
  double getTRT_DriftTime_Sum();
  double getTR_DriftTime_Sum();
  double getTRT_NoDriftTime_Sum();
  double getTR_NoDriftTime_Sum();
  double getCrossedStraws();
  int getCrossedLayers();
  double getDetectionWeight();
  void addTRT_Hit(const InDet::TRT_DriftCircle*);
  std::vector<const InDet::TRT_DriftCircle*>& getTRT_Hits();
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
  void setLocalCoordinate(double);
  double localCoordinate();
  void setDC(const InDet::TRT_DriftCircle*);
  const InDet::TRT_DriftCircle* getDC();
 private:
  double m_loc;
  const InDet::TRT_DriftCircle* m_pDC;
};

class TrigTRT_TrackExtensionLayer
{
 public:
  TrigTRT_TrackExtensionLayer(int,double[10],int,ToolHandle<TrigMagneticFieldTool>,const TRT_ID*,TrigTRT_BasePDAF*);
  ~TrigTRT_TrackExtensionLayer();
  void addElement(TrigTRT_DetElement*);
  void dump(FILE*);
  TrigTRT_DetElementPoint* findRoadPoint(double*);
  std::vector<TrigTRT_Straw*>* getVectorOfStraws(int,const double[3],Trk::TrkTrackState*);
  Trk::TrkPlanarSurface* createSurface(int);
  bool isInAcceptance(Trk::TrkTrackState*);
  const TRT_ID* getTRT_Helper();
  TrigTRT_BasePDAF* getUpdator();
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
  void addLayer(TrigTRT_TrackExtensionLayer*);
  void setType(int);
  void dump(FILE*);
  void report();
  bool isInAcceptance(double[6]);
  void setBoundaries(double,double,double,double);
  void collectDetElements(TrigTRT_DetElementRoad*,double[6]);
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
  TrigTRT_DetElementRoad* buildTRT_Road(double[6]);
  void dump(char fileName[]);
 private:
  TrigTRT_TrackExtensionRegion* m_pRegions[4];
  TrigTRT_BasePDAF* m_barrelUpdator;
  TrigTRT_BasePDAF* m_endcapUpdator;
};


#endif
