/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Package : RpcRawDataMonitoring
// Author:   N. Benekos(Illinois) - M. Bianco(INFN-Lecce) - G. Chiodini(INFN-Lecce) - A. Guida (INFN-Lecce)
// Sept. 2007
//
// DESCRIPTION:
// Subject: RPC-->Offline Muon Data Quality
////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef RpcRawDataValAlg_H
#define RpcRawDataValAlg_H

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonDQAUtils/MuonDQAHistMap.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonTrigCoinData/RpcCoinDataContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "StoreGate/ReadHandleKey.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"

#include <string>
#include <vector>
#include <map>
#include <TH2F.h>
#include <TH2I.h>
#include <TH1I.h>
#include <inttypes.h>

class IRPCcablingSvc;
 
template <class ConcreteAlgorithm> class AlgFactory;

/////////////////////////////////////////////////////////////////////////////

class RpcRawDataValAlg: public ManagedMonitorToolBase {
 
 public:

  RpcRawDataValAlg ( const std::string & type, const std::string & name, const IInterface* parent );
  virtual ~RpcRawDataValAlg();
  StatusCode initialize(); 
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();
  StatusCode finalize();
  
 private:
      
  bool m_first = true;   //!< flag to make sure that overflow message is only printed once 
  std::string m_type ;
  
  // Private function to add the clusters to the ntuple
  StatusCode addClusters(std::string clusterContainerName);  
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfo{this,"EventInfo","EventInfo","event info"};
  SG::ReadHandleKey<Muon::RpcPrepDataContainer> m_key_rpc{this,"RpcPrepDataContainer","RPC_Measurements","RPC PRDs"};  
  SG::ReadHandleKey<Muon::RpcCoinDataContainer> m_key_trig{this,"RPCTriggerContainer","RPC_triggerHits","RPC trigger hits"};
  SG::ReadHandleKey<Muon::RpcPrepDataContainer> m_clusterContainerName{this,"ClusterContainer","rpcClusters","RPC clusters"};

  std::string m_generic_path_rpcmonitoring ;
  
  //Function for histogram booking and parameterd for fitting
  void bookRPCChamberStats();
  void bookRPCLayerHistograms(std::string, std::string,  std::string, int, int, int);
  void bookRPCLayerHistogramsPanel(std::string, std::string);
  void bookRPCLayervsTimeHistograms( std::string hardware_name,  std::string layer_name, 
				     int bin,  int binmin ,  int binmax);
  void bookRPCLayerPhiAmbiHistograms(std::string, std::string, std::string, int, int, int);
  void bookRPCLayervsLayerHistograms(std::string, std::string, std::string, std::string, int, int, int, int, int ,int);
  void bookRPCLayerPhivsEtaHistograms(std::string, std::string, int, int, int, int, int, int);
  void bookRPCLayerPhivsEtaSectorHistograms(std::string, std::string, int, int, int, int, int, int);
 
  void bookRPCSummaryHistograms( int, const std::string & m_quantity ) ;
  
  MuonDQAHistMap m_stationHists;

  int m_sector;
  int m_side;
  int m_rpc_eventstotal;
  std::string m_hardware_name                                ;
  std::vector<Identifier>*    m_chambersId		     ;
  std::vector<std::string>    m_layer_name_list              ; 
  std::vector<std::string>    m_layer_name_list_panel        ; 
  std::vector<int>            m_layer_name_bin_list_panel    ;
  std::vector<std::string>    m_layervslayer_name_list       ;
  std::vector<std::string>    m_layerPhivsEta_name_list      ;
  std::vector<std::string>    m_layerPhivsEtaSector_name_list;
  int                         m_StationNameViewIndex[100][2] ;
  float                       m_StationNameSectorSize[100]   ;
  float                       m_StationPivotSectorSize[100]  ;
  
  int m_shiftphiatlas ;
   
  std::vector<Identifier>* m_padsId;
  std::map<std::string,int> m_hitsperchamber_map;
  
  // MuonDetectorManager from the conditions store
  SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey {this, "DetectorManagerKey", 
      "MuonDetectorManager", 
      "Key of input MuonDetectorManager condition data"};    

  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  
  const IRPCcablingSvc* m_cabling;
   

  //Declare Properties  
  std::string m_chamberName		;
  std::string m_StationSize		;
  bool m_doClusters			;
  bool m_checkCabling			;
  bool m_rpcfile			;   
  bool m_rpcchamberhist			;    
  bool m_rpcsectorhist			;
  
  int  m_rpcreducenbins			;
  int  m_rpcreducenbinsstrip	        ;
  int  m_rpcreducetimenbins		;
  
  int  m_lv1Thres_0		        ;
  int  m_lv1Thres_1		        ;
  int  m_lv1Thres_2		        ;
  int  m_lv1Thres_3		        ;
  
  int  m_LB_Nbins		        ;
  int  m_LBmax 		                ; 
  
  bool m_doRpcESD 			;
       
  int m_StationEta       		;
  int m_StationPhi       		;
  int m_lastEvent        		;
  int m_cosmicStation    		;
 
  // Define variables in the Ntuple:
  int m_nColl;  		    // number of collection in the container
  int m_nPrd ;			    // number of prd in the collection
  int m_nTrig;
  int m_nClus;                      // number of clusters

  int m_nPrd_BA	;
  int m_nPrd_BC ;
  
  enum {enumBA_LowPt, enumBC_LowPt, enumBA_HighPt, enumBC_HighPt};
  std::vector<TH2I*> m_rpc2DEtaStationTriggerHits_Side_Pt      ; 
  std::vector<TH2F*> m_rpc2DEtaStationTriggerHits_Side_Pt_norm ; 
  int   m_nEtaStatFired_BA_LowPt	       ;
  int   m_nEtaStatFired_BC_LowPt	       ;
  int   m_nEtaStatFired_BA_HighPt	       ;
  int   m_nEtaStatFired_BC_HighPt	       ; 
   
  std::vector<TH1I*> m_rpcNumberEtaStatFired_Side_Pt      ;
  bool  m_doTrigEvol		       ;
  bool  m_doLumiPlot		       ;
  bool  m_doTriggerHits                ;
  int   m_minStatTrEvol                  ;
  int   m_nTrigEtaStat_BA_LowPt	       ;
  int   m_nTrigEtaStat_BC_LowPt	       ;
  int   m_nTrigEtaStat_BA_HighPt	       ;
  int   m_nTrigEtaStat_BC_HighPt	       ; 
  
  enum {enum_Phi_LowPt        ,enum_Phi_HighPt       ,enum_Eta_LowPt       ,enum_Eta_HighPt       ,
        enum_Phi_LowPt_BA     ,enum_Phi_HighPt_BA    ,enum_Eta_LowPt_BA    ,enum_Eta_HighPt_BA    ,
        enum_Phi_LowPt_BC     ,enum_Phi_HighPt_BC    ,enum_Eta_LowPt_BC    ,enum_Eta_HighPt_BC    ,
        enum_EtaPhi_LowPt_BA  ,enum_EtaPhi_HighPt_BA ,enum_EtaPhi_LowPt_BC ,enum_EtaPhi_HighPt_BC ,
        enum_Phi_LowPt0_BA    ,enum_Phi_LowPt1_BA    ,enum_Eta_LowPt0_BA   ,enum_Eta_LowPt1_BA    ,
        enum_Phi_Pivot0_BA    ,enum_Phi_Pivot1_BA    ,enum_Eta_Pivot0_BA   ,enum_Eta_Pivot1_BA    ,
        enum_Phi_HighPt0_BA   ,enum_Phi_HighPt1_BA   ,enum_Eta_HighPt0_BA  ,enum_Eta_HighPt1_BA   ,
        enum_Phi_LowPt0_BC    ,enum_Phi_LowPt1_BC    ,enum_Eta_LowPt0_BC   ,enum_Eta_LowPt1_BC    ,
        enum_Phi_Pivot0_BC    ,enum_Phi_Pivot1_BC    ,enum_Eta_Pivot0_BC   ,enum_Eta_Pivot1_BC    ,
        enum_Phi_HighPt0_BC   ,enum_Phi_HighPt1_BC   ,enum_Eta_HighPt0_BC  ,enum_Eta_HighPt1_BC}  ;
  std::vector<TH2I*> m_rpc2DPanelHits ; 
  std::vector<TH2I*> m_rpc1DvsLBPanelHits ;
  enum {enum_Phi_TrigTowerLowPt_BA ,enum_Phi_TrigTowerHighPt_BA, enum_Eta_TrigTowerLowPt_BA ,enum_Eta_TrigTowerHighPt_BA ,
        enum_Phi_TrigTowerLowPt_BC ,enum_Phi_TrigTowerHighPt_BC, enum_Eta_TrigTowerLowPt_BC ,enum_Eta_TrigTowerHighPt_BC };    
  std::vector<TH2I*> m_rpc1DvsLBTrigTowerHits ;  
  
  TH1* m_rpcEtaTime	   ;
  TH1* m_rpcPhiTime	   ;
  TH1* m_rpcevents	   ;
  TH1* m_rpctime		   ;
    
  
  TH1* m_rpcclusters	   ;
  TH1* m_rpcCSEta 	   ;
  TH1* m_rpcCSPhi 	   ;

  TH1* m_rpcCSEta_BA 	   ;
  TH1* m_rpcCSPhi_BA 	   ;
  TH1* m_rpcCSEta_BC 	   ;
  TH1* m_rpcCSPhi_BC 	   ;

  
  
  TH2* m_rpcPhivsEtaAtlasPivot0   	       ;
  TH2* m_rpcPhivsEtaAtlasPivot1   	       ;
  TH2* m_rpcPhivsEtaAtlasLowPt0   	       ;
  TH2* m_rpcPhivsEtaAtlasLowPt1   	       ;
  TH2* m_rpcPhivsEtaAtlasHighPt0  	       ;
  TH2* m_rpcPhivsEtaAtlasHighPt1  	       ;
  TH2* m_rpcPhivsEtaAtlasLowPt_TriggerOut        ;
  TH2* m_rpcPhivsEtaAtlasHighPt_TriggerFromLowPt ;
  TH2* m_rpcPhivsEtaAtlasHighPt_TriggerOut       ;
  
  TH2* m_rpcPhivsEtaAtlasPivot0_PhivsZ	       ;
  TH2* m_rpcPhivsEtaAtlasPivot1_PhivsZ	       ;
  TH2* m_rpcPhivsEtaAtlasLowPt0_PhivsZ	       ;
  TH2* m_rpcPhivsEtaAtlasLowPt1_PhivsZ	       ;
  TH2* m_rpcPhivsEtaAtlasHighPt0_PhivsZ	       ;
  TH2* m_rpcPhivsEtaAtlasHighPt1_PhivsZ	       ;
  TH2* m_rpcPhivsEtaAtlasLowPt_TriggerOut_PhivsZ        ;
  TH2* m_rpcPhivsEtaAtlasHighPt_TriggerFromLowPt_PhivsZ ;
  TH2* m_rpcPhivsEtaAtlasHighPt_TriggerOut_PhivsZ       ;
  
    
  TH2* m_rpc2DEtaStation                	    ;
  TH2* m_rpc2DEtaStationGap1           	    ;
  TH2* m_rpc2DEtaStationGap2           	    ;
  TH2* m_rpc2DEtaStationTriggerHits    	    ;
  
  TH2* m_GlobalHitsPerRPCMiddle        	    ;
  TH2* m_GlobalHitsPerRPCOuter         	    ;

  TH2* m_EtavsPhi_TriggeredMuons_LowPt        ;
  TH2* m_EtavsPhi_TriggeredMuons_HighPt        ;
  TH2* m_EtavsPhi_TriggeredMuons_Pt1          ;
  TH2* m_EtavsPhi_TriggeredMuons_Pt2          ;
  TH2* m_EtavsPhi_TriggeredMuons_Pt3          ;
  TH2* m_EtavsPhi_TriggeredMuons_Pt4          ;
  TH2* m_EtavsPhi_TriggeredMuons_Pt5          ;
  TH2* m_EtavsPhi_TriggeredMuons_Pt6          ;
  
  TH1* m_RPC_Threshold_Eta             	    ;	  
  TH1* m_RPC_Threshold_Phi             	    ;
 
  TH2* m_RPC_TriggerRoad                      ;
  TH2* m_RPC_TriggerRoad_Large_Eta      ;
  TH2* m_RPC_TriggerRoad_Large_Phi      ;
  TH2* m_RPC_TriggerRoad_Small_Eta      ;
  TH2* m_RPC_TriggerRoad_Small_Phi      ;
    
  
  TH1* m_TotalNumber_of_RPC_hits_per_events_BA;
  TH1* m_TotalNumber_of_RPC_hits_per_events_BC;
  TH1* m_rpctime_BA			    ;
  TH1* m_rpctime_BC			    ;
  TH1* m_rpctime_LPt_BA			;
  TH1* m_rpctime_LPt_BC			;
  TH1* m_rpctime_HPt_BA			;
  TH1* m_rpctime_HPt_BC			;


  TH1* m_rpctime_Sector1_BA		    ;
  TH1* m_rpctime_Sector2_BA		    ;
  TH1* m_rpctime_Sector3_BA		    ;
  TH1* m_rpctime_Sector4_BA		    ;
  TH1* m_rpctime_Sector5_BA		    ;
  TH1* m_rpctime_Sector6_BA		    ;
  TH1* m_rpctime_Sector7_BA		    ;
  TH1* m_rpctime_Sector8_BA		    ;
  TH1* m_rpctime_Sector9_BA		    ;
  TH1* m_rpctime_Sector10_BA		    ;
  TH1* m_rpctime_Sector11_BA		    ;
  TH1* m_rpctime_Sector12_BA		    ;
  TH1* m_rpctime_Sector13_BA		    ;
  TH1* m_rpctime_Sector14_BA		    ;
  TH1* m_rpctime_Sector15_BA		    ;
  TH1* m_rpctime_Sector16_BA		    ;

  TH1* m_rpctime_Sector1_BC		    ;
  TH1* m_rpctime_Sector2_BC		    ;
  TH1* m_rpctime_Sector3_BC		    ;
  TH1* m_rpctime_Sector4_BC		    ;
  TH1* m_rpctime_Sector5_BC		    ;
  TH1* m_rpctime_Sector6_BC		    ;
  TH1* m_rpctime_Sector7_BC		    ;
  TH1* m_rpctime_Sector8_BC		    ;
  TH1* m_rpctime_Sector9_BC		    ;
  TH1* m_rpctime_Sector10_BC		    ;
  TH1* m_rpctime_Sector11_BC		    ;
  TH1* m_rpctime_Sector12_BC		    ;
  TH1* m_rpctime_Sector13_BC		    ;
  TH1* m_rpctime_Sector14_BC		    ;
  TH1* m_rpctime_Sector15_BC		    ;
  TH1* m_rpctime_Sector16_BC		    ;

  
  TH1* m_rpcTriggerHitsPerEvents_Eta_LowPt    ;  
  TH1* m_rpcTriggerHitsPerEvents_Phi_LowPt    ;  
  TH1* m_rpcTriggerHitsPerEvents_Eta_HighPt   ;  
  TH1* m_rpcTriggerHitsPerEvents_Phi_HighPt   ;
  
  TH1* m_rpc1DStationNameHitsSideA            ;
  TH1* m_rpc1DStationNameTriggerHitsSideA     ;
  TH1* m_rpc1DStationNameHitsSideC            ;
  TH1* m_rpc1DStationNameTriggerHitsSideC     ;
  
  
  // Global Variables declaration

  //string variable
  std::string m_layeronly_name            ;
  std::string m_layer_name                ;
  std::string m_layervslayer_name         ;
  std::string m_layerPhivsEta_name        ;
  std::string m_layerPhivsEtaSector_name  ;
  std::string m_sector_name               ;
  std::string m_layer0_name               ;
  std::string m_layer1_name               ;
  std::string m_layer2_name               ;
  std::string m_layertodraw1_name         ;
  std::string m_layertodraw2_name         ;
  std::string m_layer_name_panel          ;
  std::string m_sector_dphi_layer         ;
  
  int     m_threshold       ;
  
  int     m_MinEntries	    ; 

  // cool
  void  bookRPCCoolHistograms(std::vector<std::string>::const_iterator &m_iter, int, int, std::string m_layer);
  bool m_doCoolDB		       ;
  std::vector<std::string> m_DB_list   ;
  
  TH1* m_rpcCool_StripProfile          ;
  TH1* m_rpcCool_PanelIdHist           ;
  
  
  
};

#endif
 


 
 
