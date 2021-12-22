/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Package : RPCStandaloneTracksMon
// Author:   N. Benekos(Illinois) - M. Bianco(INFN-Lecce) - G. Chiodini(INFN-Lecce) - A. Guida (INFN-Lecce)
// April 2009
//
// DESCRIPTION:
// Subject: RPC-->Offline Muon Data Quality
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef RPCStandaloneTracksMon_H
#define RPCStandaloneTracksMon_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrigT1Interfaces/ITrigT1MuonRecRoiTool.h"
#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "MuonDQAUtils/MuonDQAHistMap.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonSegment.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODEventInfo/EventInfo.h"

#include "TrigT1Interfaces/RecMuonRoiSvc.h"
#include "TrigT1Result/MuCTPI_RDO.h"
#include "TrigDecisionTool/TrigDecisionTool.h" 
#include "MuonRDO/RpcSectorLogicContainer.h"
#include "MuonTrigCoinData/RpcCoinDataContainer.h"

#include "StoreGate/ReadHandleKey.h"

#include <TH1F.h>
#include <TH2.h>
#include <inttypes.h> 
#include <sstream>
#include <string.h>
#include <vector>
#include <map>

#include "TROOT.h"
 
template <class ConcreteAlgorithm> class AlgFactory;

/////////////////////////////////////////////////////////////////////////////

class RPCStandaloneTracksMon: public ManagedMonitorToolBase {
 
 public:

  RPCStandaloneTracksMon ( const std::string & type, const std::string & name, const IInterface* parent );
  virtual ~RPCStandaloneTracksMon();
  StatusCode initialize(); 
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();
  StatusCode finalize();
 
 private:
      
  bool m_first;   //!< flag to make sure that overflow message is only printed once   
  std::string m_type ;
  
  // Private function to add the clusters to the ntuple
  StatusCode addClusters(std::string clusterContainerName);  
  
  StoreGateSvc* m_eventStore = nullptr;
  
  ActiveStoreSvc* m_activeStore = nullptr;

  std::string m_muonTracksName;
  std::string m_msVertexCollection;
  std::string m_muonExtrapTracksName;
  std::string m_innerTracksName;
  // Trigger items
  bool m_useTrigger = false; 
  std::string m_MuonTriggerChainName;
  std::vector<std::string> m_muon_triggers; 
  std::string m_generic_path_rpcmonitoring ;
  
  //Function for histogram booking and parameterd for fitting

  void bookRPCTracksLayerHistograms(std::string, std::string,  std::string, int, int, int);
  void bookHistogramsSummaryDistribution(   );
 
  /* void bookRPCSummaryHistograms( int, std::vector<std::string>::const_iterator &iter ) ; */
  void bookRPCSummaryHistograms( int, const std::string & quantity )      ;
  void bookRPCSummaryFinalHistograms( int, const std::string & quantity ) ;
  void bookRPCLayerRadiographyHistograms( int isec, std::string layer_name );
  void bookRPCCoolHistograms_NotNorm(std::vector<std::string>::const_iterator &iter , int , int , std::string layer );
  void bookRPCCoolHistograms(std::vector<std::string>::const_iterator &iter , int , int , std::string layer );
  
  ToolHandle< LVL1::ITrigT1MuonRecRoiTool > m_rpcRoiTool{ this, "RPCRecRoiTool", "LVL1::TrigT1RPCRecRoiTool/TrigT1RPCRecRoiTool", "RPC Rec Roi Tool"};
  ToolHandle< LVL1::ITrigT1MuonRecRoiTool > m_tgcRoiTool{ this, "TGCRecRoiTool", "LVL1::TrigT1TGCRecRoiTool/TrigT1TGCRecRoiTool", "TGC Rec Roi Tool"};

  ToolHandle<Trig::TrigDecisionTool>   m_trigDec  ;
  
  StringProperty  m_triggerChainGroupRegExp   ;
  StringProperty  m_triggerChainGroupRegNot   ;
  BooleanProperty m_selectTriggerChainGroup   ;
  BooleanProperty m_deselectTriggerChainGroup ;
  const Trig::ChainGroup*      m_chainGroupSelect ;  
  const Trig::ChainGroup*      m_chainGroupVeto ;  
  
  StatusCode RPC_ROI_Mapping();
  
  double m_EtaROImin[2][32][32]{};
  double m_EtaROImax[2][32][32]{};
  double m_PhiROImin[2][32][32]{};
  double m_PhiROImax[2][32][32]{};
  
  MuonDQAHistMap m_stationHists;

  int m_sector;
  int m_side;
  int m_rpc_eventstotal = 0;
  std::vector<Identifier>* m_chambersId			  ;
  std::vector<std::string> m_hardware_name_list           ;  
  std::vector<std::string> m_layer_name_list              ; 
  std::vector<std::string> m_layer_name_list_panel        ; 
  std::vector<int>         m_layer_name_bin_list_panel    ;
  
  std::vector<Identifier>* m_padsId;
  
  // MuonDetectorManager from the conditions store
  SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey {this, "DetectorManagerKey", 
      "MuonDetectorManager", 
      "Key of input MuonDetectorManager condition data"};    

  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  
  //Declare Properties  
  SG::ReadHandleKey<Muon::RpcPrepDataContainer> m_key_rpc{this,"RpcPrepDataContainer","RPC_Measurements","RPC PRDs"};
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo{this,"EventInfo","EventInfo","event info"};
  SG::ReadHandleKey<Muon::RpcPrepDataContainer> m_clusterContainerName{this,"ClusterContainer","rpcClusters","RPC clusters"};
  SG::ReadHandleKey<MuCTPI_RDO> m_muCTPI_RDO_key{this,"muCTPI_RDO_key","MUCTPI_RDO","muCTPI RDO"};
  SG::ReadHandleKey<RpcSectorLogicContainer> m_sectorLogicContainerKey{this,"RPCSec","RPC_SECTORLOGIC","RPC sector logic"};
  SG::ReadHandleKey<Muon::RpcCoinDataContainer> m_rpc_coin_key{this,"RPCCoinKey","RPC_triggerHits","RPC coin container"};
  SG::ReadHandleKey<xAOD::MuonContainer> m_muonsName{this,"MuonCollection","Muons","muons"};
  std::string m_chamberName		;
  std::string m_StationSize		;
  std::string m_key_trig		;
  bool m_doClusters			;
  bool m_checkCabling			;
  bool m_rpcfile			;   
  bool m_rpcchamberhist			; 
  bool m_doRadiography		        ;
  bool m_do_rpctracks			;
  // bool m_do_CmTracksTime        	;
  int  m_rpcreducenbins			;
  int  m_rpcreducenbinsstrip	        ;
  int  m_rpcreducetimenbins = 0		;
  
  int  m_lv1Thres_0 = 0		        ;
  int  m_lv1Thres_1 = 0		        ;
  int  m_lv1Thres_2 = 0		        ;
  
  bool m_doRpcESD 			;
  bool m_doCoolDB		        ;
  
  bool m_selectLB 			;
  int  m_minSelLB 			;
  int  m_maxSelLB 			;
  
  bool m_trOnlySmall       		;
  bool m_trOnlyLarge       		;
  
      
  int m_StationEta       		;
  int m_StationPhi       		;
  int m_lastEvent        		;
  int m_cosmicStation    		;
  
  float  m_rpc_readout_window           ;
  double m_MuonDeltaRMatching           ;       
  bool   m_requireMuonCombinedTight     ;
  bool   m_StandAloneMatchedWithTrack   ;

  bool   m_isMC                         ;
  
  int m_nClus = 0;                      // number of clusters

  TH1* m_rpczxSurfaceView = nullptr    ;   
  TH1* m_rpcxSurfaceView = nullptr     ; 
  TH1* m_rpczSurfaceView = nullptr     ;
  TH1* m_f_rpczxSurfaceView = nullptr  ;
  
  TH1* m_rpctrack_phivseta = nullptr   ;  
  TH1* m_rpctrack_eta = nullptr        ;  
  TH1* m_rpctrack_phi = nullptr        ;
    
  TH1* m_rpctrack_bvseta = nullptr     ;  
  TH1* m_rpctrack_b = nullptr          ;  
  
  TH1* m_rpctrack_bVTXy0 = nullptr     ;
  TH1* m_rpctrack_bVTXx0 = nullptr     ;
  TH1* m_rpctrack_bVTXz0 = nullptr     ;
   
  
  TH1* m_rpcmergepointdistance = nullptr          ;
  TH1* m_f_rpcmergepointdistance = nullptr        ;
  TH1* m_rpcchi2dof = nullptr		      ;
  TH1* m_rpcetavsphichi2dof = nullptr	      ;
  TH1* m_f_rpcchi2dof = nullptr		      ;
  TH1* m_rpcNtracks = nullptr		      ;
  TH1* m_f_rpcNtracks = nullptr		      ;
  TH1* m_rpcPointPerTracks = nullptr	      ;
  TH1* m_f_rpcPointPerTracks = nullptr	      ;
  TH1* m_rpcTimeTrackRes = nullptr		      ;
  TH1* m_rpcTimeTrackRMS = nullptr		      ;
  TH1* m_rpcPhiResidual = nullptr		      ;
  TH1* m_rpcEtaResidual = nullptr		      ;
  TH1* m_f_rpcPhiResidual = nullptr 	      ;
  TH1* m_f_rpcEtaResidual = nullptr 	      ;
  TH1* m_rpcTrackType = nullptr		      ;
  TH1* m_f_rpcTrackType = nullptr		      ;
 
  TH1* m_ResidualVsCS = nullptr		      ;
 
  TH2* m_rpcSectorLayerTrackProj = nullptr		        ;
  TH2* m_rpcSectorLayerResponse = nullptr		        ;
  TH2* m_rpcSectorLayerResponseOR = nullptr  	        ;
  TH2* m_rpcSectorLayerNoiseCorr = nullptr                  ;
  TH2* m_rpcSectorLayerNoiseNotCorr = nullptr               ;
  TH1* m_rpcCS_HitOnTrack = nullptr 		        ;
  TH1* m_rpcCS_NoiseCorr = nullptr			        ;
  TH1* m_rpcCS_NoiseNotCorr = nullptr		        ;
  
  TH2* m_rpcCS_angleLong = nullptr		        ;
  TH2* m_rpcCS_angleTrasv = nullptr		        ;
  TH2* m_rpcCS_EtavsPhi = nullptr                         ; 
  
  //Trigger efficiency plots
  TH1* m_hMEtracks = nullptr  ;
  
  
  std::vector<TH1F*> m_hRPCPhiEtaCoinThr       ;
  std::vector<TH1F*> m_hRPCPadThr              ;
  std::vector<TH1F*> m_hRPCMuctpiThr           ;
  std::vector<TH1F*> m_hRPCPhiEtaCoinThr_eff   ;
  std::vector<TH1F*> m_hRPCPadThr_eff          ;
  std::vector<TH1F*> m_hRPCMuctpiThr_eff       ;
   
  struct muctpi_rdo {
        float eta;
        float phi;
        short int source;
        short int hemisphere;
        short int bcid;
        short int sectorID;
        short int thrNumber;
        short int RoINumber;
        short int overlapFlags;
    };
    
  
  enum {enumSumTrackProj, enumSumHitOnTrack, enumSumHitOnTrack_withCrossStrip, enumSumEfficiency, enumSumGapEfficiency,
        enumSumNoiseTot_NotNorm, enumSumNoiseCorr_NotNorm,
	enumSumCS_NotNorm, enumSumCS_square, enumSumCS_entries,
	enumSumRes_CS1_NotNorm, enumSumRes_CS1_square, enumSumRes_CS1_entries,
	enumSumRes_CS2_NotNorm, enumSumRes_CS2_square, enumSumRes_CS2_entries,
        enumSumRes_CSmore2_NotNorm, enumSumRes_CSmore2_square, enumSumRes_CSmore2_entries,
	enumSumOccupancy_NotNorm, enumSumTime_NotNorm, enumSumTime_square, enumSumCS1_entries, enumSumCS2_entries};
	   
  int  m_SummaryHist_Size = 0		        ;
  int  m_SummaryHist_Idx = 0                        ;	   
  std::vector<TH1F*> m_SummaryHist		;
  std::vector<TH1F*> m_SummaryHistRPCBA 	;
  std::vector<TH1F*> m_SummaryHistRPCBC 	;
  
  std::vector<TH1F*> m_sumTmpHist               ;
  
   enum {enumAvEff, enumAvGapEff, enumAvNoiseCorr, enumAvNoiseTot, 
        enumAvCS, enumAvRes_CS1, enumAvRes_CS2, enumAvRes_CSmore2,
        enumAvRes_CS1rms, enumAvRes_CS2rms, enumAvRes_CSmore2rms,   
	enumAvOccupancy, enumAvTime }; 
  std::vector<TH1F*> m_rpcAverageSide_C       ;
  std::vector<TH1F*> m_rpcAverageSide_A       ; 

  TH1* m_LayerTrackProjection = nullptr		   ;
  TH1* m_LayerHitOnTrack = nullptr		   ;
  TH1* m_LayerEff = nullptr  			   ;
 
  int     m_MinEntries	    ; 
  float m_resThEff		       ;
  
  int   m_HPtPointForHPteff	       ;
  int   m_HPtPointForLPteff	       ;
  int   m_HPtPointForTracks	       ;
};


#endif
