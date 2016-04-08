/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIGTRTTRACKEXTENSIONTOOL_H__
#define __TRIGTRTTRACKEXTENSIONTOOL_H__

#include "GaudiKernel/AlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrigInDetToolInterfaces/ITrigTRT_TrackExtensionTool.h"
#include "TrigInDetToolInterfaces/ITrigDkfTrackMakerTool.h"

#include "InDetIdentifier/TRT_ID.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "TrigTRT_TrackExtensionTool/TrigTRT_DetElementRoadTool.h"
#include "TrigInDetToolInterfaces/ITrigTRT_DriftCircleProvider.h"
//#include "RegionSelector/IRegSelSvc.h"

class TrigTimer;
class ITrigRegionSelector;
class IROBDataProviderSvc;
class IRegSelSvc;

namespace Trk {	
  class TrkBaseNode;             
  class IMagneticFieldTool;
}
class TrigTRT_Trajectory;
class TrigMagneticFieldTool;
class ITrigInDetBremDetectionTool;

class TrigTRT_TrackExtensionTool: public AthAlgTool, virtual public ITrigTRT_TrackExtensionTool
{
 public:
  TrigTRT_TrackExtensionTool( const std::string&, const std::string&, const IInterface* );
  virtual ~TrigTRT_TrackExtensionTool();
  virtual StatusCode initialize();
  virtual StatusCode finalize  ();
  StatusCode propagate(TrigInDetTrackCollection*);

  const std::vector<int>* fillTRT_DataErrors();

  inline double getTRTTotalTime(void) const { return m_trtDataProvider->getTRTTotalTime();}
  inline double getTRTRobProvTime(void) const { return m_trtDataProvider->getTRTRobProvTime();}
  inline double getTRTTrtCnvTime(void) const { return m_trtDataProvider->getTRTTrtCnvTime();}
  inline double getTRT_DataProviderTime(void) const { return m_trtDataProviderTime;}
  inline double getTRT_RobPreloaderTime(void) const { return m_trtRobPreloaderTime;}
  inline double getTRT_RoadBuilderTime(void) const { return m_trtRoadBuilderTime;}
  inline double getTRT_PDAF_TrackingTime(void) const { return m_trtPDAF_TrackingTime;}

private:

  Trk::TrkTrackState* m_extrapolate(Trk::TrkTrackState*, 
				    Trk::TrkPlanarSurface*,
				    Trk::TrkPlanarSurface*,
				    double[5][5],
				    double&);
  void m_matrixInversion5x5(double a[5][5]);

  void m_deleteTrajectories();
  void m_deleteNodes();

  int m_preloadROBs(TrigInDetTrackCollection*);
  void m_getPhiRange(double, double, double&, double&);
  void m_getEtaRange(double, double, double&, double&);
  int  m_outputLevel;
  double m_DChi2;
  double m_minPt;
  double m_roiPhiSize,m_roiEtaSize;
  bool m_doMultScatt;
  bool m_doBremm;
  bool m_offlineClusters;
  std::string m_trtContName;

  const TRT_ID* m_trtId;
//  const InDetDD::TRT_DetectorManager* m_TRT_Manager;

  std::vector<Trk::TrkBaseNode*> m_vpTrkNodes;
  std::vector<TrigTRT_Trajectory*> m_vpTrajectories;

  ServiceHandle<IRegSelSvc> m_regionSelector;

  ToolHandle<ITrigTRT_DriftCircleProviderTool> m_trtDataProvider;

  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;
  ToolHandle<TrigTRT_DetElementRoadTool> m_trtRoadBuilder;
  ToolHandle<TrigMagneticFieldTool> m_trigFieldTool;
  ToolHandle<ITrigInDetBremDetectionTool> m_trigBremTool;
  ToolHandle<ITrigDkfTrackMakerTool> m_trackMaker;
  std::vector<int> m_trtDataErrors;

  std::vector<const ROBF*> m_vROBF;

#define TRIGTRTEXT_NTIMERS 8
  TrigTimer* m_timer[TRIGTRTEXT_NTIMERS];
  bool m_timers;
  double m_trtDataProviderTime;
  double m_trtRobPreloaderTime;
  double m_trtRoadBuilderTime;
  double m_trtPDAF_TrackingTime;
};

#endif

