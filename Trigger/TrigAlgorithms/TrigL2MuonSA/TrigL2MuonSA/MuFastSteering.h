/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef  TRIGL2MUONSA_MUFASTSTEERING_H
#define  TRIGL2MUONSA_MUFASTSTEERING_H

#include "GaudiKernel/Bootstrap.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrigInterfaces/FexAlgo.h"
#include "TrigTimeAlgs/ITrigTimerSvc.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigL2MuonSA/MuFastDataPreparator.h"
#include "TrigL2MuonSA/MuFastPatternFinder.h"
#include "TrigL2MuonSA/MuFastStationFitter.h"
#include "TrigL2MuonSA/MuFastTrackFitter.h"
#include "TrigL2MuonSA/MuFastTrackExtrapolator.h"
#include "TrigL2MuonSA/RecMuonRoIUtils.h"
#include "TrigL2MuonSA/MuCalStreamerTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "TrigL2MuonSA/CscSegmentMaker.h"
#include "TrigL2MuonSA/CscRegUtils.h"

//adding a part of DataHandle for AthenaMT
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"    
#include "StoreGate/WriteHandleKey.h"   
#include "TrigMuonEvent/MuonFeature.h"
#include "TrigMuonEvent/MuonFeatureDetails.h"

#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "xAODTrigger/TrigCompositeContainer.h"

//using namespace TrigL2MuonSA;

class IRegSelSvc;
class IJobOptionsSvc;
class Incident;

enum ECRegions{ Bulk, WeakBFieldA, WeakBFieldB };

class MuFastSteering : public HLT::FexAlgo,
  virtual public IIncidentListener
{
 public:
  enum {
    ITIMER_DATA_PREPARATOR=0,
    ITIMER_PATTERN_FINDER,
    ITIMER_STATION_FITTER,
    ITIMER_TRACK_FITTER,
    ITIMER_TRACK_EXTRAPOLATOR,
    ITIMER_CALIBRATION_STREAMER,
    ITIMER_TOTAL_PROCESSING
  };
  
 public:
  
  /** Constructor */
  MuFastSteering(const std::string& name, ISvcLocator* svc);
  ~MuFastSteering();
  
  /** hltBeginRun() */
  HLT::ErrorCode hltBeginRun();
  /** hltEndRun() */
  HLT::ErrorCode hltEndRun();
  
  /** hltInitialize() */
  HLT::ErrorCode hltInitialize();
  /** hltFinalize() */
  HLT::ErrorCode hltFinalize();
  /** hltExecute(), main code of the algorithm */
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, 
			    HLT::TriggerElement* outputTE);

  //adding a part of DataHandle for AthenaMT
  /** execute(), main code of the algorithm for AthenaMT*/
  StatusCode execute();

  int L2MuonAlgoMap(const std::string& name);
  
  /** A function which clears internal data for a new event */
  void clearEvent();

  // handler for "UpdateAfterFork" actions
  void handle(const Incident& incident);
  
  using HLT::FexAlgo::prepareRobRequests;
  virtual HLT::ErrorCode prepareRobRequests(const HLT::TriggerElement* inputTE );

 protected:
  
  /**
     Called at the end of the algorithm processing to set the steering
     navigation properly
  */
  bool updateOutputTE(HLT::TriggerElement*                     outputTE,
		      const HLT::TriggerElement*               inputTE,
		      const LVL1::RecMuonRoI*                  roi,
		      const TrigRoiDescriptor*                 roids,
		      const TrigL2MuonSA::MuonRoad&            muonRoad,
		      const TrigL2MuonSA::MdtRegion&           mdtRegion,
		      const TrigL2MuonSA::RpcHits&             rpcHits,
		      const TrigL2MuonSA::TgcHits&             /*tgcHits*/,
		      const TrigL2MuonSA::RpcFitResult&        rpcFitResult,
		      const TrigL2MuonSA::TgcFitResult&        tgcFitResult,
		      const TrigL2MuonSA::MdtHits&             mdtHits,
		      const TrigL2MuonSA::CscHits&             cscHits,
		      std::vector<TrigL2MuonSA::TrackPattern>& m_trackPatterns);

  /**
     Update monitoring variables
  */
  StatusCode updateMonitor(const LVL1::RecMuonRoI*                  roi,
			   const TrigL2MuonSA::MdtHits&             mdtHits,
                           std::vector<TrigL2MuonSA::TrackPattern>& trackPatterns);
  
 protected:
  
  // Services
  /** A service handle to StoreGate */
  ServiceHandle<StoreGateSvc> m_storeGate;
  
  /** Timers */
  ITrigTimerSvc* m_timerSvc;
  std::vector<TrigTimer*> m_timers;

  IRegSelSvc*        m_regionSelector;
  
  // Tools
  ToolHandle<TrigL2MuonSA::MuFastDataPreparator>     m_dataPreparator;
  ToolHandle<TrigL2MuonSA::MuFastPatternFinder>      m_patternFinder;
  ToolHandle<TrigL2MuonSA::MuFastStationFitter>      m_stationFitter;
  ToolHandle<TrigL2MuonSA::MuFastTrackFitter>        m_trackFitter;
  ToolHandle<TrigL2MuonSA::MuFastTrackExtrapolator>  m_trackExtrapolator;

  /** Handle to MuonBackExtrapolator tool */
  ToolHandle<ITrigMuonBackExtrapolator> m_backExtrapolatorTool;
  
  // calibration streamer tool
  ToolHandle<TrigL2MuonSA::MuCalStreamerTool>        m_calStreamer;

  // Utils
  TrigL2MuonSA::RecMuonRoIUtils  m_recMuonRoIUtils;

  //Tools for CSC
  ToolHandle<TrigL2MuonSA::CscSegmentMaker> m_cscsegmaker;


 private:
  TrigL2MuonSA::RpcHits      m_rpcHits;
  TrigL2MuonSA::TgcHits      m_tgcHits;
  TrigL2MuonSA::MdtRegion    m_mdtRegion;
  TrigL2MuonSA::MuonRoad     m_muonRoad;
  TrigL2MuonSA::RpcFitResult m_rpcFitResult;
  TrigL2MuonSA::TgcFitResult m_tgcFitResult;
  TrigL2MuonSA::MdtHits      m_mdtHits_normal;
  TrigL2MuonSA::MdtHits      m_mdtHits_overlap;
  TrigL2MuonSA::CscHits      m_cscHits;
  
  float m_scaleRoadBarrelInner;
  float m_scaleRoadBarrelMiddle;
  float m_scaleRoadBarrelOuter;
  
  BooleanProperty  m_use_timer;
  BooleanProperty  m_use_mcLUT;
  BooleanProperty  m_use_new_segmentfit;
  BooleanProperty  m_use_rpc;
  BooleanProperty  m_use_mdtcsm;
  BooleanProperty  m_use_RoIBasedDataAccess_MDT;
  BooleanProperty  m_use_RoIBasedDataAccess_RPC;
  BooleanProperty  m_use_RoIBasedDataAccess_TGC;
  BooleanProperty  m_use_RoIBasedDataAccess_CSC;
  BooleanProperty  m_doCalStream;
  BooleanProperty  m_calDataScouting;
  BooleanProperty  m_rpcErrToDebugStream;
  BooleanProperty  m_use_endcapInnerFromBarrel;
  
  IntegerProperty m_esd_rpc_size;
  IntegerProperty m_esd_tgc_size;
  IntegerProperty m_esd_mdt_size;      
  IntegerProperty m_esd_csc_size;      
  
  DoubleProperty m_rWidth_RPC_Failed;
  DoubleProperty m_rWidth_TGC_Failed;

  DoubleProperty m_winPt;

  //adding a part of DataHandle for AthenaMT
  //ReadHandle L1MURoIs
  SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey;    
  SG::ReadHandle<TrigRoiDescriptorCollection> m_roiCollection;		

  //WriteHandle MuonFeature 
  SG::WriteHandleKey<MuonFeature> m_muFeContainerKey;			
  SG::WriteHandle<MuonFeature> m_muFeContainer;  			

  //WriteHandle MuonFeatureDetails
  SG::WriteHandleKey<MuonFeatureDetails> m_muFeDeContainerKey;		
  SG::WriteHandle<MuonFeatureDetails> m_muFeDeContainer;  		

  //Test Value to confirm to record with WriteHandle
  MuonFeatureDetails m_muFeDeTestValue;

  //Monitored variables
  float m_inner_mdt_hits;
  float m_middle_mdt_hits;
  float m_outer_mdt_hits;  
  std::vector<float> m_fit_residuals;
  std::vector<float> m_res_inner;
  std::vector<float> m_res_middle;
  std::vector<float> m_res_outer;
  float m_efficiency;
  float m_sag_inverse;
  float m_sagitta;
  float m_address;
  float m_absolute_pt;
  float m_track_pt;
  std::vector<float> m_track_eta;
  std::vector<float> m_track_phi;
  std::vector<float> m_failed_eta;
  std::vector<float> m_failed_phi;

  ECRegions whichECRegion(const float eta, const float phi) const;
  float getRoiSizeForID(bool isEta, const xAOD::L2StandAloneMuon* muonSA);

  // calibration streamer properties
  IJobOptionsSvc*       m_jobOptionsSvc;
  BooleanProperty m_allowOksConfig; 
  StringProperty  m_calBufferName;
  int m_calBufferSize;
  xAOD::TrigCompositeContainer* m_trigCompositeContainer;
  xAOD::TrigCompositeAuxContainer m_trigCompositeAuxContainer;


 
};

#endif // MUFASTSTEERING_H
