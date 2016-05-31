/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/NTuple.h"
#include "DataModel/DataLink.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"


#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "MuonDQAUtils/MuonDQAHistMap.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"

#include "RPCcablingInterface/IRPCcablingServerSvc.h"


#include "xAODTracking/TrackParticle.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/Muon.h"
#include "xAODMuon/MuonSegment.h"
#include "xAODMuon/MuonSegmentContainer.h"
#include "xAODEventInfo/EventInfo.h"

#include "TrigT1Interfaces/RecMuonRoiSvc.h"

#include "TrigDecisionTool/TrigDecisionTool.h" 

#include <TError.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TF1.h>
#include <inttypes.h> 
#include <sstream>
#include <string.h>
#include <vector>
#include <map>

#include "TROOT.h"
 
class TFile;
class RpcIdHelper;
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
  std::string type ;
  
  // Private function to add the clusters to the ntuple
  StatusCode addClusters(std::string clusterContainerName);  
  
  StoreGateSvc* m_eventStore;
  
  ActiveStoreSvc* m_activeStore;

  std::string m_muonsName;
  std::string m_muonSegmentsName;
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
 
  /* void bookRPCSummaryHistograms( int, std::vector<std::string>::const_iterator &m_iter ) ; */
  void bookRPCSummaryHistograms( int, const std::string & m_quantity )      ;
  void bookRPCSummaryFinalHistograms( int, const std::string & m_quantity ) ;
  void bookRPCLayerRadiographyHistograms( int m_isec, std::string m_layer_name );
  void bookRPCCoolHistograms_NotNorm(std::vector<std::string>::const_iterator &m_iter , int , int , std::string m_layer );
  void bookRPCCoolHistograms(std::vector<std::string>::const_iterator &m_iter , int , int , std::string m_layer );
  
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
  
  double EtaROImin[2][32][32];
  double EtaROImax[2][32][32];
  double PhiROImin[2][32][32];
  double PhiROImax[2][32][32];
  
  MuonDQAHistMap m_stationHists;

  int m_sector;
  int m_side;
  int rpc_eventstotal;
  std::string hardware_name                               ;
  std::vector<Identifier>* m_chambersId			  ;
  std::vector<std::string> hardware_name_list             ;  
  std::vector<std::string> layer_name_list                ; 
  std::vector<std::string> layer_name_list_panel          ; 
  std::vector<int>         layer_name_bin_list            ; 
  std::vector<int>         layer_name_bin_list_panel      ;
  std::vector<std::string> layervslayer_name_list         ;
  std::vector<std::string> layerPhivsEta_name_list        ;
  std::vector<std::string> layerPhivsEtaSector_name_list  ;
  // std::vector<std::string> cm_time_list                ;
  // std::vector<int>         ch16_index_list             ;
  
  int shiftphiatlas ;
   
  std::vector<Identifier>* m_padsId;
  std::map<std::string,int> hitsperchamber_map;
  
  bool histo_flag;  
   
  const MuonGM::MuonDetectorManager* m_muonMgr;
  const RpcIdHelper* m_rpcIdHelper;
  
  const IRPCcablingSvc* m_cabling;
  
  
  const RpcSectorLogicContainer* sectorLogicContainer; 
  
  //Declare Properties  
  std::string m_chamberName		;
  std::string m_StationSize		;
  std::string m_key_rpc			;
  std::string m_key_trig		;
  bool m_doClusters			;
  std::string m_clusterContainerName	;
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
  bool Clus_Retr  			;
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
  std::string sector_num 		;
  
  float  m_rpc_readout_window           ;
  double m_MuonDeltaRMatching           ;       
  bool   m_requireMuonCombinedTight     ;
  bool   m_StandAloneMatchedWithTrack   ;
  
  int m_nClus;                      // number of clusters

  
  int ndbl ;

  
  TH1* rpczxSurfaceView    ;   
  TH1* rpcxSurfaceView     ; 
  TH1* rpczSurfaceView     ;
  TH1* f_rpczxSurfaceView  ;
  
  TH1* rpctrack_phivseta   ;  
  TH1* rpctrack_eta        ;  
  TH1* rpctrack_phi        ;
    
  TH1* rpctrack_bvseta     ;  
  TH1* rpctrack_b          ;  
  
  TH1* rpctrack_bVTXy0     ;
  TH1* rpctrack_bVTXx0     ;
  TH1* rpctrack_bVTXz0     ;
   
  
  TH1* rpcmergepointdistance          ;
  TH1* f_rpcmergepointdistance        ;
  TH1* rpcchi2dof		      ;
  TH1* rpcetavsphichi2dof	      ;
  TH1* f_rpcchi2dof		      ;
  TH1* rpcNtracks		      ;
  TH1* f_rpcNtracks		      ;
  TH1* rpcPointPerTracks	      ;
  TH1* f_rpcPointPerTracks	      ;
  TH1* rpcTimeTrackRes		      ;
  TH1* rpcTimeTrackRMS		      ;
  TH1* rpcPhiResidual		      ;
  TH1* rpcEtaResidual		      ;
  TH1* f_rpcPhiResidual 	      ;
  TH1* f_rpcEtaResidual 	      ;
  TH1* rpcTrackType		      ;
  TH1* f_rpcTrackType		      ;
 
  TH1* ResidualVsCS		      ;
 
  TH2* rpcSectorLayerTrackProj		        ;
  TH2* rpcSectorLayerResponse		        ;
  TH2* rpcSectorLayerResponseOR  	        ;
  TH2* rpcSectorLayerNoiseCorr                  ;
  TH2* rpcSectorLayerNoiseNotCorr               ;
  TH1* rpcCS_HitOnTrack 		        ;
  TH1* rpcCS_NoiseCorr			        ;
  TH1* rpcCS_NoiseNotCorr		        ;
  
  TH2* m_rpcCS_angleLong		        ;
  TH2* m_rpcCS_angleTrasv		        ;
  TH2* m_rpcCS_EtavsPhi                         ; 
  
  //Trigger efficiency plots
  TH1* hMEtracks  ;
  
  
  std::vector<TH1F*> hRPCPhiEtaCoinThr       ;
  std::vector<TH1F*> hRPCPadThr              ;
  std::vector<TH1F*> hRPCMuctpiThr           ;
  std::vector<TH1F*> hRPCPhiEtaCoinThr_eff   ;
  std::vector<TH1F*> hRPCPadThr_eff          ;
  std::vector<TH1F*> hRPCMuctpiThr_eff       ;
   
  std::vector<double> etaminpad   ;
  std::vector<double> etamaxpad   ;
  std::vector<double> phiminpad   ;
  std::vector<double> phimaxpad   ;
  std::vector<int>    thresholdpad;
  
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
  
  TH1* SummaryTrackProj	      			; //0
  TH1* SummaryHitOnTrack	      		; //1
  TH1* SummaryHitOnTrack_withCrossStrip		; //2
  TH1* SummaryEfficiency			; //5
  TH1* SummaryGapEfficiency  			; //6
  TH1* SummaryNoiseTot_NotNorm			; //3
  TH1* SummaryNoiseCorr_NotNorm   		; //4
  TH1* SummaryCS_NotNorm     			; //7
  TH1* SummaryCS_square				; //8
  TH1* SummaryCS_entries			; //9
  TH1* SummaryRes_CS1_NotNorm     		; //10
  TH1* SummaryRes_CS1_square			; //11
  TH1* SummaryRes_CS1_entries			; //12
  TH1* SummaryRes_CS2_NotNorm     		; //13
  TH1* SummaryRes_CS2_square			; //14
  TH1* SummaryRes_CS2_entries			; //15
  TH1* SummaryRes_CSmore2_NotNorm    		; //16
  TH1* SummaryRes_CSmore2_square		; //17
  TH1* SummaryRes_CSmore2_entries		; //18
  TH1* SummaryOccupancy_NotNorm      		; //19
  TH1* SummaryTime_NotNorm			; //20
  TH1* SummaryTime_square		        ; //21
  TH1* SummaryCS1_entries     		        ; //22
  TH1* SummaryCS2_entries     		        ; //23 adjust m_SummaryHist_Size

  std::vector<TH1F*> m_sumTmpHist               ;
  TH1* SummaryNoiseCorr   			; //0
  TH1* SummaryNoiseTot   			; //1
  TH1* SummaryRes_CS1      			; //2
  TH1* SummaryRes_CS2     			; //3
  TH1* SummaryRes_CSmore2     			; //4
  TH1* SummaryOccupancy	      		        ; //5
  TH1* SummaryCS      				; //6
  TH1* SummaryTime			        ; //7
  

  TH1* SummaryEffDistriPerSector		;  
  TH1* SummaryGapEffDistriPerSector		;
  TH1* SummaryNoiseCorrDistriPerSector		;
  TH1* SummaryNoiseTotDistriPerSector		;
  TH1* SummaryCSDistriPerSector		        ;
  TH1* SummaryRes_CS1DistriPerSector		;
  TH1* SummaryRes_CS2DistriPerSector		;
  TH1* SummaryRes_CSmore2DistriPerSector	;
  TH1* SummaryRes_CS1rmsDistriPerSector   	;
  TH1* SummaryRes_CS2rmsDistriPerSector         ;
  TH1* SummaryRes_CSmore2rmsDistriPerSector	;
  TH1* SummaryOccupancyDistriPerSector		;
  TH1* SummaryTimeDistriPerSector		;
    
  
  TH1*StripTimeDistributionRPCBA	        ;   
  TH1*StripTimeDistributionRPCBC	        ; 
  
  TH1* RPCBA_layerTrackProj	      		;
  TH1* RPCBA_layerHitOnTrack	      		;
  TH1* RPCBA_layerHitOnTrack_withCrossStrip	;
  TH1* RPCBA_layerNoiseTot   			;
  TH1* RPCBA_layerNoiseTot_s			;
  TH1* RPCBA_layerNoiseCorr   			;
  TH1* RPCBA_layerNoiseCorr_s   		;
  TH1* RPCBA_layerEfficiency			;
  TH1* RPCBA_layerGapEfficiency  		;
  TH1* RPCBA_layerCS      			;
  TH1* RPCBA_layerCS_s     			;
  TH1* RPCBA_layerCS_square			;
  TH1* RPCBA_layerCS_entries			;
  TH1* RPCBA_layerRes_CS1      			;
  TH1* RPCBA_layerRes_CS1_s     		;
  TH1* RPCBA_layerRes_CS1_square		;
  TH1* RPCBA_layerRes_CS1_entries		;
  TH1* RPCBA_layerRes_CS2     			;
  TH1* RPCBA_layerRes_CS2_s     		;
  TH1* RPCBA_layerRes_CS2_square		;
  TH1* RPCBA_layerRes_CS2_entries		;
  TH1* RPCBA_layerRes_CSmore2     		;
  TH1* RPCBA_layerRes_CSmore2_s    		;
  TH1* RPCBA_layerRes_CSmore2_square		;
  TH1* RPCBA_layerRes_CSmore2_entries		;
  TH1* RPCBA_layerOccupancy	      		;
  TH1* RPCBA_layerOccupancy_s      		;
  TH1* RPCBA_layerTime			        ;
  TH1* RPCBA_layerTime_s			;
  TH1* RPCBA_layerTime_square		        ;
    
  TH1* RPCBC_layerTrackProj	      		;
  TH1* RPCBC_layerHitOnTrack	      		;
  TH1* RPCBC_layerHitOnTrack_withCrossStrip	;
  TH1* RPCBC_layerNoiseTot   			;
  TH1* RPCBC_layerNoiseTot_s			;
  TH1* RPCBC_layerNoiseCorr   			;
  TH1* RPCBC_layerNoiseCorr_s   		;
  TH1* RPCBC_layerEfficiency			;
  TH1* RPCBC_layerGapEfficiency  		;
  TH1* RPCBC_layerCS      			;
  TH1* RPCBC_layerCS_s     			;
  TH1* RPCBC_layerCS_square			;
  TH1* RPCBC_layerCS_entries			;
  TH1* RPCBC_layerRes_CS1      			;
  TH1* RPCBC_layerRes_CS1_s     		;
  TH1* RPCBC_layerRes_CS1_square		;
  TH1* RPCBC_layerRes_CS1_entries		;
  TH1* RPCBC_layerRes_CS2     			;
  TH1* RPCBC_layerRes_CS2_s     		;
  TH1* RPCBC_layerRes_CS2_square		;
  TH1* RPCBC_layerRes_CS2_entries		;
  TH1* RPCBC_layerRes_CSmore2     		;
  TH1* RPCBC_layerRes_CSmore2_s    		;
  TH1* RPCBC_layerRes_CSmore2_square		;
  TH1* RPCBC_layerRes_CSmore2_entries		;
  TH1* RPCBC_layerOccupancy	      		;
  TH1* RPCBC_layerOccupancy_s      		;
  TH1* RPCBC_layerTime			        ;
  TH1* RPCBC_layerTime_s			;
  TH1* RPCBC_layerTime_square		        ;
  
  enum {enumAvEff, enumAvGapEff, enumAvNoiseCorr, enumAvNoiseTot, 
        enumAvCS, enumAvRes_CS1, enumAvRes_CS2, enumAvRes_CSmore2,
        enumAvRes_CS1rms, enumAvRes_CS2rms, enumAvRes_CSmore2rms,   
	enumAvOccupancy, enumAvTime }; 
  std::vector<TH1F*> rpcAverageSide_C       ;
  std::vector<TH1F*> rpcAverageSide_A       ; 

  TH1* LayerTrackProjection		   ;
  TH1* LayerHitOnTrack			   ;
  TH1* LayerEff  			   ;
 
  TH1*  rpcCool_TrackProj	           ;
  TH1*  rpcCool_HitOnTrack  		   ;
  TH1*  rpcCool_HitOnTrack_withCrossStrip  ;
  TH1*  rpcCool_NoiseCorr_NotNorm	   ;
  TH1*  rpcCool_NoiseTot_NotNorm 	   ;
  TH1*  rpcCool_Res_CS1_NotNorm  	   ;
  TH1*  rpcCool_Res_CS1_entries		   ;
  TH1*  rpcCool_Res_CS1_square		   ;
  TH1*  rpcCool_Res_CS2_NotNorm  	   ;
  TH1*  rpcCool_Res_CS2_entries		   ;
  TH1*  rpcCool_Res_CS2_square		   ;
  TH1*  rpcCool_Res_CSmore2_NotNorm	   ;
  TH1*  rpcCool_Res_CSmore2_entries 	   ;
  TH1*  rpcCool_Res_CSmore2_square  	   ;
  TH1*  rpcCool_Occupancy_NotNorm	   ;
  TH1*  rpcCool_CS_NotNorm		   ;
  TH1*  rpcCool_CS_entries  		   ;
  TH1*  rpcCool_CS_square		   ;
  TH1*  rpcCool_Time_NotNorm		   ;
  TH1*  rpcCool_Time_square 		   ; 
	  
  // Global Variables declaration

  //string variable
  std::string layeronly_name            ;
  std::string layer_name                ;
  std::string layervslayer_name         ;
  std::string layerPhivsEta_name        ;
  std::string layerPhivsEtaSector_name  ;
  std::string sector_name               ;
  std::string layer0_name               ;
  std::string layer1_name               ;
  std::string layer2_name               ;
  std::string layertodraw1_name         ;
  std::string layertodraw2_name         ;
  std::string layer_name_panel          ;
  std::string sector_dphi_layer         ;
  
  int 	  irpc_station    ;   
  int 	  irpc_eta        ;      
  int 	  irpc_phi        ;      
  int 	  irpc_doublr     ;   
  int 	  irpc_doublz     ;  
  int 	  irpc_doublphi   ; 
  int 	  irpc_gasgap     ;   
  int 	  irpc_measphi    ; 

   	  
  double  irpctime	    ;  			  
  int 	  irpcstationPhi    ;	  
  int	  irpcstationName   ; 	  
  int	  irpcstationEta    ; 
  int 	  irpcdoubletR	    ;	  
  int	  irpcdoubletZ	    ; 
  int	  irpcdoubletPhi    ;
  int	  irpcgasGap	    ; 
  int	  irpcmeasuresPhi   ; 
  int	  irpcstrip	    ;  
  int     irpctriggerInfo   ;
  //double  irpctriggerInfo   ; 
  double  irpcambiguityFlag ; 
  int     irpcthreshold     ;
  int     m_threshold       ;
  
  
  int     irpcstationPhi_prep  ; 
  int     irpcstationName_prep ;
  int     irpcstationEta_prep  ;
  int     irpcdoubletR_prep    ;   
  int     irpcdoubletPhi_prep  ;
  int     irpcmeasuresPhi_prep ;
  
  int     irpcstationNameC     ;
    	  
  double  irpctimeII	       ;  			  
  int 	  irpcstationPhiII     ;	
  int	  irpcstationNameII    ;		
  int	  irpcstationEtaII     ; 
  int 	  irpcdoubletRII       ;	  
  int	  irpcdoubletZII       ; 
  int	  irpcdoubletPhiII     ;
  int	  irpcgasGapII	       ; 
  int	  irpcmeasuresPhiII    ; 
  int	  irpcstripII	       ; 
  double  irpctriggerInfoII    ; 
  double  irpcambiguityFlagII  ;
  
  double  x_atl,      y_atl,      z_atl      ;
  double  x_atl_II,   y_atl_II,   z_atl_II   ;
  double  x_atlas,    y_atlas,    z_atlas    ;
  double  x_atl_prep, y_atl_prep, z_atl_prep ;
  double  phi_atlas                   	     ;
  double  eta_atlas                   	     ;
  double  impactParam                        ;
  
  int     irpc_clus_station  ;
  int     irpc_clus_eta      ;    
  int     irpc_clus_phi      ;    
  int     irpc_clus_doublr   ; 
  int     irpc_clus_doublz   ; 
  int     irpc_clus_doublphi ;
  int     irpc_clus_gasgap   ; 
  int     irpc_clus_measphi  ;
  int     irpc_clus_size     ;   
  double  irpc_clus_time     ;   
  double  irpc_clus_posx     ;   
  double  irpc_clus_posy     ;   
  double  irpc_clus_posz     ;    
  double  irpc_clus_poseta   ;   
  double  irpc_clus_posphi   ;   
   
  int     irpc_clus_stationII  ;
  int     irpc_clus_etaII      ;    
  int     irpc_clus_phiII      ;    
  int     irpc_clus_doublrII   ; 
  int     irpc_clus_doublzII   ; 
  int     irpc_clus_doublphiII ;
  int     irpc_clus_gasgapII   ; 
  int     irpc_clus_measphiII  ;
  int     irpc_clus_sizeII     ;   
  double  irpc_clus_timeII     ;   
  double  irpc_clus_posxII     ;   
  double  irpc_clus_posyII     ;   
  double  irpc_clus_poszII     ;     
  double  irpc_clus_posetaII   ;   
  double  irpc_clus_posphiII   ;

  int strip                 ; 
  
  int NphiStrips            ;
  int NetaStrips            ;
  int NetaStripsTot         ;
  int NetaStripsTotSideA    ;
  int NetaStripsTotSideC    ;
  int EtaStripSign          ;
  int ShiftStrips           ;
  int ShiftEtaStrips        ;
  int ShiftEtaStripsTot     ;
  int ShiftPhiStrips        ;
  int Nbin                  ;
  int SectorLogic           ;
  int Side                  ;
  int PanelIndex	    ;
  int Settore	            ;
  int PlaneTipo             ;
  
  int shift_pos             ;
  
  int NbinII                ;
  
  int     MinEntries	    ; 
  double  irpc_posx	    ;   
  double  irpc_posy	    ;   
  double  irpc_posz	    ;	     

  int  stripetaatlas      ;
  int  stripphisector     ;
  int  shiftstripphiatlas ;
  int  stripphiatlas      ;
  
  int N_Rpc_Clusters3D              	;
  std::vector<int> LayerType        	;
  std::vector<int> PlaneType        	;
  std::vector<int> SmallLarge       	;
  std::vector<int> Rpc_Phi_3D       	;
  std::vector<int> Rpc_Station_3D   	;
  std::vector<int> Rpc_Eta_3D       	;
  std::vector<int> Rpc_DBLr_3D      	;
  std::vector<int> Rpc_DBLphi_3D    	;
  std::vector<int> Rpc_DBLz_3D      	;
  std::vector<int> Rpc_GasGap_3D    	;
  std::vector<int> Rpc_SL_3D        	;
  std::vector<int> Rpc_Side_3D      	;
  std::vector<int> Rpc_avEta_3D         ;
  std::vector<int> Rpc_avPhi_3D         ;
  std::vector<double> Rpc_x_3D      	;
  std::vector<double> Rpc_y_3D      	;
  std::vector<double> Rpc_z_3D      	;
  std::vector<double> Rpc_eta_3D      	;
  std::vector<double> Rpc_phi_3D      	;
  std::vector<double> Rpc_t_3D      	;
  std::vector<Identifier> Rpc_id_eta_3D ;
  std::vector<Identifier> Rpc_id_phi_3D ;
  std::vector<Amg::Vector3D > Rpc_Point ;
  std::vector<int   > Rpc_Matched_mu    ;
  
  double incAngle  ;
  
  int Phi_Rpc_Track;
  int layertype    ;
  int planetype    ;

  
  int N_RpcTrack ;
  std::vector<Amg::Vector3D > RpcTrack_Vect3DCos  ;
  std::vector<Amg::Vector3D >  RpcTrack_Poin3RLin  ;
  std::vector<float>       RpcTrack_chi2dofeta ;
  std::vector<float>       RpcTrack_chi2dofphi ;
  std::vector<float>       RpcTrack_chi2dof    ;
              

  std::string SideSector    ;
  int  side                 ; 
  int  sector               ;
  char sector_char[1000]    ;
  char dblZ_char[100]       ;
  char dblPhi_char[100]     ;
  
  float av_strip            ;
  float avstripeta          ;
  float avstripphi          ;
  float avstrip             ;
  
  int N_Rpc_Tracks          ;  
  int Phi_Rpc_Tracks        ;
  int PointperTrack	    ;
  int linkedtrack           ;
  int lookforthirdII        ;
  int thirdlayertypeII      ;
  int thirdlayerHPt         ;
  int *Rpc_track            ;
  
  
  float cosmerge 	    ;
  
  float x0Phi 	    ;
  float xyPhi 	    ;
  float z0Eta 	    ;
  float zyEta 	    ;
   
  int	NplaneSmall0, NplaneSmall1, NplaneSmall2  ; 
  int	NplaneLarge0, NplaneLarge1, NplaneLarge2  ;
  
  
  float xav , x2av, yav, y2av, zav, z2av, xyav, xzav, zyav ;
  float deltax, deltaz, deltay	       ;
  float projx , projz , projy	       ;
  float chi2dof, chi2dofeta, chi2dofphi;
  float res, residual2		       ;
  int   mineta, maxeta, minphi, maxphi ;
  float tav , t2av, trms , tres        ;
  float cosyx                          ;
  float xsurface, zsurface 	       ;
  float xPy0, zPy0 	               ;
  float xPz0, yPz0 	               ;
  float zPx0, yPx0 	               ;
  float Impact_B_IP                    ;
  float pseudoeta, anglephi            ;
  float rhoxy, rho, costh, sinth, pseta;
  //int   NPhiStrip, NEtaStrip         ;
  float phipitch, etapitch             ;
  int   Layer	                       ;
  int   phistripN, etastripN           ; 
  int   foundonehiteta, foundonehitphi ;   
  int   foundEtaPhi                    ;
  int   foundEtaOrPhi                  ;
  int   PanelIndexeta ,  PanelIndexphi ;   
  int   StripIndexeta ,  StripIndexphi ;
  float stripontrack, residuals        ;
  float TrackType                      ;
  float small_large                    ;
  float hitstripphi                    ;
  float hitstripeta                    ; 
  float m_resThEff		       ;
  
  int   m_HPtPointForHPteff	       ;
  int   m_HPtPointForLPteff	       ;
  int   m_HPtPointForTracks	       ;
  	       
  int   res_th                         ;
  int   n_eta_station                  ;
  //   int   i_sector		       ; 
  //   int   i_side		       ;
  //   int   i_sectorLogic	       ;
  //   int   i_padId 		       ;
  //   int   i_status		       ;
  //   int   i_errorCode	       ;
  //   int   i_cmaId 		       ;
  //   int   i_fel1Id		       ;
  //   int   i_febcId		       ;
  //   int   i_crc		       ;
    
  //   int   match_in_spacefound       ;
  //   int   N_16ch                    ;
  //   char  ch16group[100]            ;
  
  
  
  int   nEtaStatFired_BA_LowPt	       ;
  int   nEtaStatFired_BC_LowPt	       ;
  int   nEtaStatFired_BA_HighPt	       ;
  int   nEtaStatFired_BC_HighPt	       ;
  //int   nTrigEtaStatVect_BA[8][16*3]   ;
  //int   nTrigEtaStatVect_BC[7][16*3]   ;
  bool  m_doTrigEvol		       ;
  int   minStatTrEvol                  ;
  int   nTrigEtaStat_BA_LowPt	       ;
  int   nTrigEtaStat_BC_LowPt	       ;
  int   nTrigEtaStat_BA_HighPt	       ;
  int   nTrigEtaStat_BC_HighPt	       ;
     
  
  char histName_char[100]	       	;
  std::string histName   	       	;
  std::vector<std::string> DB_list     	;
  std::vector<std::string> DB_NotNorm_list  	;
  
   
  int istatPhi                         	;
  int iName                            	; 
  int ir                               	;
  int NTotStrips                       	;
  int NTotStripsSideC, NTotStripsSideA 	;
  int NTotStripsEtaSideC		;
  int NTotStripsEtaSideA		;
  int NTotStripsPhiSideC		;
  int NTotStripsPhiSideA		;
  int NEtaTotStripsSideC               	;
  int NEtaTotStripsSideA               	;
  int NPhiTotStrips                    	;
  int iStripEtaA    			;
  int iStripPhiA			;
  int iStripEtaC			;
  int iStripPhiC			;
  int panelId				;
  int panelLyIndex			;
  int panelGasGap			;
  
  int Npanel, NSecStrips               	;
  int strip_dbindex		       	;
  int ShiftPhiTot_db                   	;
  int NphiStripsTotSideA               	;
  int NphiStripsTotSideC               	;
  int ShiftPhi_db                      	;
  int Nphi_Zstrips                     	;
  
  int rpc2DEtaStatBinX_BA	       	;	
  int rpc2DEtaStatBinX_BC	       	;
  int rpc2DEtaStatBinY		       	;
  int etaStatShiftX_BA		       	;
  int etaStatShiftX_BC		       	;
  
  int binz			       	;
  int binminz			       	;
  int binmaxz			       	;
  int binx			       	;
  int binminx			       	;
  int binmaxx			       	;
  
  int panelBin                          ;
  int indexplane                        ;
  
  float RPCLyHitOnTr		        ;
  float RPCLyTrkPrj		        ;
  float RPCLyEff  		        ;
  float RPCLyEff_err		        ;
  float PanelVal  		        ;
  float PanelVal_err		        ;
  float PanelVal_entries  	        ;
  float PanelVal_square		        ;
  float PanelHitOnTrack		        ;
  float PanelTrackProj		        ;
  float PanelHitOnTrack_withCrossStrip  ; 
  
  float invgasgaparea		  	;  
  float invstriparea		  	;
  float distance		  	;
  
  int isNoise                           ;
  int isNoiseCorr                       ;
  float isNoisePhiStrip                 ;
  
  float norm1                           ;
  float norm2                           ;
  
  float norm3DGaprho                    ;
  float Vect3DCosrho                    ;   
};

#endif
 


 
 
