/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_TRT_COMBINED_EXTENSION_TOOL_H__
#define __TRIG_TRT_COMBINED_EXTENSION_TOOL_H__

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
#include "TrigInDetTrackFitter/ITrigL2FastExtrapolationTool.h"
#include "TrigInDetTrackFitter/ITrigL2TrackFittingTool.h"

//#include "RegionSelector/IRegSelSvc.h"
#include "TrkExInterfaces/IExtrapolator.h"

class TrigTimer;
class ITrigRegionSelector;
class IROBDataProviderSvc;
class IRegSelSvc;

namespace Trk {	
  class TrkBaseNode;             
  class TrkTrackState;      
  class TrkPlanarSurface;
  class IMagneticFieldTool;
  class IRIO_OnTrackCreator;
  class IExtrapolator;
}

class TrigTRT_DetElementRoad;
class TrigTRT_Info;
class TrigMagneticFieldTool;
class TrigTRT_Trajectory;


class TrigTRT_CombinedExtensionTool: public AthAlgTool, virtual public ITrigTRT_TrackExtensionTool
{
 public:
  TrigTRT_CombinedExtensionTool( const std::string&, const std::string&, const IInterface* );
  virtual ~TrigTRT_CombinedExtensionTool();
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

  Trk::TrkTrackState* m_extrapolateOffline(Trk::TrkTrackState*,
	  Trk::TrkPlanarSurface*,Trk::TrkPlanarSurface*,int,bool createTargetSurface=false);

  TrigTRT_Trajectory* m_createTRT_Trajectory(TrigInDetTrack*);
  bool m_refitTRT_Trajectory(TrigTRT_Trajectory*);
  bool m_runPDAF_Tracking(TrigTRT_Trajectory*);
  bool m_runBackwardFilter(TrigTRT_Trajectory*);
  bool m_updatePerigee(TrigTRT_Trajectory*);

  void m_updateFilteringNode(Trk::TrkBaseNode*, Trk::TrkTrackState*);
  void m_deleteTrajectories();

  int m_preloadROBs(TrigInDetTrackCollection*);
  void m_getPhiRange(double, double, double&, double&);
  void m_getEtaRange(double, double, double&, double&);
  int  m_outputLevel;
  double m_DChi2;
  double m_roiPhiSize,m_roiEtaSize;
  bool m_doMultScatt;
  bool m_doBremm;
  bool m_recalibrate;
  double m_momentumThreshold;
  std::string m_trtContName;
  
  const TRT_ID* m_trtId;

  std::vector<TrigTRT_Trajectory*> m_vpTrajectories;

  ServiceHandle<IRegSelSvc> m_regionSelector;

  ToolHandle<ITrigTRT_DriftCircleProviderTool> m_trtDataProvider;

  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;
  ToolHandle<TrigTRT_DetElementRoadTool> m_trtRoadBuilder;
  ToolHandle<TrigMagneticFieldTool> m_trigFieldTool;
  ToolHandle<ITrigDkfTrackMakerTool> m_trackMaker;
  ToolHandle<Trk::IRIO_OnTrackCreator>       m_ROTcreator;
  ToolHandle<Trk::IExtrapolator> m_extrapolator;
  ToolHandle<ITrigL2TrackFittingTool> m_lowPtFitter;
  ToolHandle<ITrigL2FastExtrapolationTool> m_fastExtrapolator;
  
  std::vector<int> m_trtDataErrors;

  std::vector<const ROBF*> m_vROBF;

#define TRIGCOMBTOOL_NTIMERS 8
  TrigTimer* m_timer[TRIGCOMBTOOL_NTIMERS];
  bool m_timers;
  double m_trtDataProviderTime;
  double m_trtRobPreloaderTime;
  double m_trtRoadBuilderTime;
  double m_trtPDAF_TrackingTime;
};

#endif

