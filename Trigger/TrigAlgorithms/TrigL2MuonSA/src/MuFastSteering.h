/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

#include "MuFastDataPreparator.h"
#include "MuFastPatternFinder.h"
#include "MuFastStationFitter.h"
#include "MuFastTrackFitter.h"
#include "MuFastTrackExtrapolator.h"
#include "RecMuonRoIUtils.h"
#include "MuCalStreamerTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "CscSegmentMaker.h"
#include "CscRegUtils.h"

//adding a part of DataHandle for AthenaMT
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"    
#include "StoreGate/WriteHandleKey.h"   

#include "xAODEventInfo/EventInfo.h"
#include "xAODTrigMuon/L2StandAloneMuonContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "AthenaMonitoringKernel/GenericMonitoringTool.h"

class IRegSelSvc;
class Incident;
class MsgStream;

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
  
  /** hltStop() */
  HLT::ErrorCode hltStop();
  
  /** hltInitialize() */
  HLT::ErrorCode hltInitialize();
  /** hltFinalize() */
  HLT::ErrorCode hltFinalize();
  /** hltExecute(), main code of the algorithm */
  HLT::ErrorCode hltExecute(const HLT::TriggerElement* /*inputTE*/, 
			    HLT::TriggerElement* outputTE);

  /** execute(), main code of the algorithm for AthenaMT*/
  StatusCode execute();

  /** findMuonSignature(), includes reconstract algorithms **/
  /** this function can be called from both execute() and hltExecute() **/
  StatusCode findMuonSignature(const std::vector<const TrigRoiDescriptor*>&	roi,
			       const std::vector<const LVL1::RecMuonRoI*>& 	muonRoIs,
                               DataVector<xAOD::L2StandAloneMuon>& 		outputTracks,
			       TrigRoiDescriptorCollection&	 		outputID,
			       TrigRoiDescriptorCollection&	 		outputMS,
			       DataVector<xAOD::TrigComposite>&			outputComposite );

  int L2MuonAlgoMap(const std::string& name);

  // handler for "UpdateAfterFork" actions
  void handle(const Incident& incident);
  
  using HLT::FexAlgo::prepareRobRequests;
  virtual HLT::ErrorCode prepareRobRequests(const HLT::TriggerElement* inputTE );

 protected:
  
  /**
     Called at the end of the algorithm processing to set the steering
     navigation properly
  */
  bool updateOutputObjects(const LVL1::RecMuonRoI*                        roi,
                           const TrigRoiDescriptor*                       roids,
                           const TrigL2MuonSA::MuonRoad&                  muonRoad,
                           const TrigL2MuonSA::MdtRegion&                 mdtRegion,
                           const TrigL2MuonSA::RpcHits&                   rpcHits,
                           const TrigL2MuonSA::TgcHits&                   tgcHits,
                           const TrigL2MuonSA::RpcFitResult&              rpcFitResult,
                           const TrigL2MuonSA::TgcFitResult&              tgcFitResult,
                           const TrigL2MuonSA::MdtHits&                   mdtHits,
                           const TrigL2MuonSA::CscHits&                   cscHits,
                           const std::vector<TrigL2MuonSA::TrackPattern>& trackPatterns,
			   DataVector<xAOD::L2StandAloneMuon>&	          outputTracks,
			   TrigRoiDescriptorCollection&  	          outputID,
			   TrigRoiDescriptorCollection&   	          outputMS);
 
  bool storeMuonSA(const LVL1::RecMuonRoI*             roi,
                   const TrigRoiDescriptor*            roids,
               	   const TrigL2MuonSA::MuonRoad&       muonRoad,
               	   const TrigL2MuonSA::MdtRegion&      mdtRegion,
               	   const TrigL2MuonSA::RpcHits&        rpcHits,
               	   const TrigL2MuonSA::TgcHits&        tgcHits,
               	   const TrigL2MuonSA::RpcFitResult&   rpcFitResult,
               	   const TrigL2MuonSA::TgcFitResult&   tgcFitResult,
               	   const TrigL2MuonSA::MdtHits&        mdtHits,
               	   const TrigL2MuonSA::CscHits&        cscHits,
               	   const TrigL2MuonSA::TrackPattern&   pattern,
                   DataVector<xAOD::L2StandAloneMuon>& outputTracks);

  bool storeMSRoiDescriptor(const TrigRoiDescriptor*                  roids,
		            const TrigL2MuonSA::TrackPattern&         pattern,
                            const DataVector<xAOD::L2StandAloneMuon>& outputTracks,
		            TrigRoiDescriptorCollection&	      outputMS);


  bool storeIDRoiDescriptor(const TrigRoiDescriptor*                  roids,
		            const TrigL2MuonSA::TrackPattern&         pattern,
                            const DataVector<xAOD::L2StandAloneMuon>& outputTracks,
		            TrigRoiDescriptorCollection&	      outputID);

  /**
     Update monitoring variables
  */
  StatusCode updateMonitor(const LVL1::RecMuonRoI*                  roi,
			   const TrigL2MuonSA::MdtHits&             mdtHits,
                           std::vector<TrigL2MuonSA::TrackPattern>& trackPatterns );
 protected:
  
  // Services
  
  /** Timers */
  ServiceHandle<ITrigTimerSvc> m_timerSvc;
  std::vector<TrigTimer*> m_timingTimers;

  ServiceHandle<IRegSelSvc> m_regionSelector;
  
  // Tools
  ToolHandle<TrigL2MuonSA::MuFastDataPreparator>     m_dataPreparator {
	this, "DataPreparator", "TrigL2MuonSA::MuFastDataPreparator", "data preparator" };
  ToolHandle<TrigL2MuonSA::MuFastPatternFinder>      m_patternFinder {
	this, "PatternFinder", "TrigL2MuonSA::MuFastPatternFinder", "pattern finder" };
  ToolHandle<TrigL2MuonSA::MuFastStationFitter>      m_stationFitter {
	this, "StationFitter", "TrigL2MuonSA::MuFastStationFitter", "station fitter" };
  ToolHandle<TrigL2MuonSA::MuFastTrackFitter>        m_trackFitter {
	this, "TrackFitter", "TrigL2MuonSA::MuFastTrackFitter", "track fitter" };
  ToolHandle<TrigL2MuonSA::MuFastTrackExtrapolator>  m_trackExtrapolator {
	this, "TrackExtrapolator", "TrigL2MuonSA::MuFastTrackExtrapolator", "track extrapolator" };

  /** Handle to MuonBackExtrapolator tool */
  ToolHandle<ITrigMuonBackExtrapolator> m_backExtrapolatorTool {
	this, "BackExtrapolator", "TrigMuonBackExtrapolator", "public tool for back extrapolating the muon tracks to the IV" };
  
  // calibration streamer tool
  ToolHandle<TrigL2MuonSA::MuCalStreamerTool> m_calStreamer {
	this, "CalibrationStreamer", "TrigL2MuonSA::MuCalStreamerTool", "" };

  // Utils
  TrigL2MuonSA::RecMuonRoIUtils  m_recMuonRoIUtils;

  //Tools for CSC
  ToolHandle<TrigL2MuonSA::CscSegmentMaker> m_cscsegmaker {
	this, "CscSegmentMaker", "TrigL2MuonSA::CscSegmentMaker", "" };


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
  
  // Property
  Gaudi::Property< float > m_scaleRoadBarrelInner { this, "Scale_Road_BarrelInner", 1, "" };
  Gaudi::Property< float > m_scaleRoadBarrelMiddle { this, "Scale_Road_BarrelMiddle", 1, "" };
  Gaudi::Property< float > m_scaleRoadBarrelOuter { this, "Scale_Road_BarrelOuter", 1, "" };
  
  Gaudi::Property< bool > m_use_timer { this, "Timing", false, "" };
  Gaudi::Property< bool > m_use_mcLUT { this, "UseLUTForMC", true, ""};
  Gaudi::Property< bool > m_use_new_segmentfit { this, "USE_NEW_SEGMENTFIT", true, ""};
  Gaudi::Property< bool > m_use_rpc { this, "USE_RPC", true, ""};
  Gaudi::Property< bool > m_use_mdtcsm { this, "USE_MDTCSM", false, ""};
  Gaudi::Property< bool > m_use_RoIBasedDataAccess_MDT { this, "USE_ROIBASEDACCESS_MDT", true, ""};
  Gaudi::Property< bool > m_use_RoIBasedDataAccess_RPC { this, "USE_ROIBASEDACCESS_RPC", true, ""};
  Gaudi::Property< bool > m_use_RoIBasedDataAccess_TGC { this, "USE_ROIBASEDACCESS_TGC", true, ""};
  Gaudi::Property< bool > m_use_RoIBasedDataAccess_CSC { this, "USE_ROIBASEDACCESS_CSC", true, ""};
  Gaudi::Property< bool > m_doCalStream { this, "DoCalibrationStream", false, ""};
  Gaudi::Property< bool > m_calDataScouting { this, "MuonCalDataScouting", false, ""};
  Gaudi::Property< bool > m_rpcErrToDebugStream { this, "RpcErrToDebugStream", false, ""};
  Gaudi::Property< bool > m_use_endcapInnerFromBarrel { this, "UseEndcapInnerFromBarrel", false, ""};
  
  Gaudi::Property< int > m_esd_rpc_size { this, "ESD_RPC_size", 100, "" };
  Gaudi::Property< int > m_esd_tgc_size { this, "ESD_TGC_size", 50, "" };
  Gaudi::Property< int > m_esd_mdt_size { this, "ESD_MDT_size", 100, "" };
  Gaudi::Property< int > m_esd_csc_size { this, "ESD_CSC_size", 100, "" };
  
  Gaudi::Property< double > m_rWidth_RPC_Failed { this, "R_WIDTH_RPC_FAILED", 400, "" };
  Gaudi::Property< double > m_rWidth_TGC_Failed { this, "R_WIDTH_TGC_FAILED", 200, "" };

  Gaudi::Property< double > m_winPt { this, "WinPt", 4.0, "" };

  Gaudi::Property< bool > m_topoRoad { this, "topoRoad", false, "create road in barrel not to highly overlap surrounding L1 RoIs" };
  Gaudi::Property< float > m_dPhisurrRoI { this, "dPhisurrRoI", 99, "phi range to find surrounding L1 RoIs" };
  Gaudi::Property< float > m_dEtasurrRoI { this, "dEtasurrRoI", 99, "eta range to find surrounding L1 RoIs" };

  float getRoiSizeForID(bool isEta, const xAOD::L2StandAloneMuon* muonSA);

  Gaudi::Property< bool > m_allowOksConfig { this, "AllowOksConfig", true, ""};
  Gaudi::Property< std::string > m_calBufferName { this, "MuonCalBufferName", "/tmp/testOutput", ""};
  Gaudi::Property< int > m_calBufferSize { this, "MuonCalBufferSize", 1024*1024, ""};

  // Enable to fill FS RoI for ID (cosmic run)
  Gaudi::Property< bool > m_fill_FSIDRoI { this, "FILL_FSIDRoI", false, "Fill FS RoI for ID (will be used in cosmic run)"};

  //adding a part of DataHandle for AthenaMT
  //ReadHandle xAOD::EventInfo
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{
	this, "EventInfo", "EventInfo", "Name of the xAOD::EventInfo object"};

  //ReadHandle MURoIs
  SG::ReadHandleKey<TrigRoiDescriptorCollection> m_roiCollectionKey{
	this, "MuRoIs", "HLT_MURoIs", "Name of the input data from L1Decoder"};

  //ReadHandle RecMuonRoIs
  SG::ReadHandleKey<DataVector<LVL1::RecMuonRoI>> m_recRoiCollectionKey{
	this, "RecMuonRoI", "HLT_RecMURoIs", "Name of the input data on LVL1::RecMuonRoI produced by L1Decoder"};

  //WriteHandle <xAOD::L2StandAloneMuonContainer>
  SG::WriteHandleKey<xAOD::L2StandAloneMuonContainer> m_muFastContainerKey{
	this, "MuonL2SAInfo", "MuonL2SAInfo", "Name of the output data on xAOD::L2StandAloneMuonContainer"};

  //WriteHandle <xAOD::L2StandAloneMuonContainer>
  SG::WriteHandleKey<xAOD::TrigCompositeContainer> m_muCompositeContainerKey{
	this, "MuonCalibrationStream", "MuonCalibrationStream", "Name of the decisions object attached by MuFastSteering"};

  //WriteHandle <TrigRoiDescriptor> for ID
  SG::WriteHandleKey<TrigRoiDescriptorCollection> m_muIdContainerKey{
	this, "forID", "forID", "Name of the output data for Inner Detector"};

  //WriteHandle <TrigRoiDescriptor> for MS
  SG::WriteHandleKey<TrigRoiDescriptorCollection> m_muMsContainerKey{
	this, "forMS", "forMS", "Name of the output data for MS"};

  // Monitor system
  ToolHandle< GenericMonitoringTool > m_monTool { this, "MonTool", "", "Monitoring tool" };
};

#endif // MUFASTSTEERING_H
