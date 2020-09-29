/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
  
  StoreGateSvc* m_eventStore;
  
  ActiveStoreSvc* m_activeStore;

  std::string m_muonTracksName;
  std::string m_msVertexCollection;
  std::string m_muonExtrapTracksName;
  std::string m_innerTracksName;
  // Trigger items
  bool m_useTrigger; 
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
  
  ServiceHandle< LVL1::RecMuonRoiSvc > m_rpcRoiSvc;
  ServiceHandle< LVL1::RecMuonRoiSvc > m_tgcRoiSvc;
  ToolHandle<Trig::TrigDecisionTool>   m_trigDec  ;
  
  StringProperty  m_triggerChainGroupRegExp   ;
  StringProperty  m_triggerChainGroupRegNot   ;
  BooleanProperty m_selectTriggerChainGroup   ;
  BooleanProperty m_deselectTriggerChainGroup ;
  const Trig::ChainGroup*      m_chainGroupSelect ;  
  const Trig::ChainGroup*      m_chainGroupVeto ;  
  
  StatusCode RPC_ROI_Mapping();
  
  double m_EtaROImin[2][32][32];
  double m_EtaROImax[2][32][32];
  double m_PhiROImin[2][32][32];
  double m_PhiROImax[2][32][32];
  
  MuonDQAHistMap m_stationHists;

  int m_sector;
  int m_side;
  int m_rpc_eventstotal;
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
  int  m_rpcreducetimenbins		;
  
  int  m_lv1Thres_0		        ;
  int  m_lv1Thres_1		        ;
  int  m_lv1Thres_2		        ;
  
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
  
  int m_nClus;                      // number of clusters

  TH1* m_rpczxSurfaceView    ;   
  TH1* m_rpcxSurfaceView     ; 
  TH1* m_rpczSurfaceView     ;
  TH1* m_f_rpczxSurfaceView  ;
  
  TH1* m_rpctrack_phivseta   ;  
  TH1* m_rpctrack_eta        ;  
  TH1* m_rpctrack_phi        ;
    
  TH1* m_rpctrack_bvseta     ;  
  TH1* m_rpctrack_b          ;  
  
  TH1* m_rpctrack_bVTXy0     ;
  TH1* m_rpctrack_bVTXx0     ;
  TH1* m_rpctrack_bVTXz0     ;
   
  
  TH1* m_rpcmergepointdistance          ;
  TH1* m_f_rpcmergepointdistance        ;
  TH1* m_rpcchi2dof		      ;
  TH1* m_rpcetavsphichi2dof	      ;
  TH1* m_f_rpcchi2dof		      ;
  TH1* m_rpcNtracks		      ;
  TH1* m_f_rpcNtracks		      ;
  TH1* m_rpcPointPerTracks	      ;
  TH1* m_f_rpcPointPerTracks	      ;
  TH1* m_rpcTimeTrackRes		      ;
  TH1* m_rpcTimeTrackRMS		      ;
  TH1* m_rpcPhiResidual		      ;
  TH1* m_rpcEtaResidual		      ;
  TH1* m_f_rpcPhiResidual 	      ;
  TH1* m_f_rpcEtaResidual 	      ;
  TH1* m_rpcTrackType		      ;
  TH1* m_f_rpcTrackType		      ;
 
  TH1* m_ResidualVsCS		      ;
 
  TH2* m_rpcSectorLayerTrackProj		        ;
  TH2* m_rpcSectorLayerResponse		        ;
  TH2* m_rpcSectorLayerResponseOR  	        ;
  TH2* m_rpcSectorLayerNoiseCorr                  ;
  TH2* m_rpcSectorLayerNoiseNotCorr               ;
  TH1* m_rpcCS_HitOnTrack 		        ;
  TH1* m_rpcCS_NoiseCorr			        ;
  TH1* m_rpcCS_NoiseNotCorr		        ;
  
  TH2* m_rpcCS_angleLong		        ;
  TH2* m_rpcCS_angleTrasv		        ;
  TH2* m_rpcCS_EtavsPhi                         ; 
  
  //Trigger efficiency plots
  TH1* m_hMEtracks  ;
  
  
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
	   
  int  m_SummaryHist_Size		        ;
  int  m_SummaryHist_Idx                        ;	   
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

  TH1* m_LayerTrackProjection		   ;
  TH1* m_LayerHitOnTrack		   ;
  TH1* m_LayerEff  			   ;
 
  int     m_MinEntries	    ; 
  float m_resThEff		       ;
  
  int   m_HPtPointForHPteff	       ;
  int   m_HPtPointForLPteff	       ;
  int   m_HPtPointForTracks	       ;
};


#endif
