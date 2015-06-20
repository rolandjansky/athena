/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////////////////////
// Package : RpcRawDataMonitoring
// Author:   N. Benekos(Illinois) - M. Bianco(INFN-Lecce) - G. Chiodini(INFN-Lecce)
// Sept. 2007
//
// DESCRIPTION:
// Subject: RPCLV1-->Offline Muon Data Quality
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef RpcLv1RawDataValAlg_H
#define RpcLv1RawDataValAlg_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/NTuple.h"

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "MuonDQAUtils/MuonDQAHistMap.h"

#include "DataModel/DataLink.h"
#include "RPCcablingInterface/IRPCcablingServerSvc.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutSet.h"
#include "MuonGeoModel/MYSQL.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include "MuonRDO/RpcFiredChannel.h"
#include "MuonRDO/RpcCoinMatrix.h"
#include "MuonRDO/RpcPad.h"
#include "MuonRDO/RpcPadContainer.h"
#include "MuonRDO/RpcSectorLogicContainer.h"

#include "RpcRawDataMonitoring/RpcGlobalUtilities.h"

#include <sstream>
#include <string.h>
#include <vector>
#include <map>
 
class TFile;
class RpcIdHelper;
template <class ConcreteAlgorithm> class AlgFactory;
/////////////////////////////////////////////////////////////////////////////

class RpcLv1RawDataValAlg: public ManagedMonitorToolBase {
 
 public:

  RpcLv1RawDataValAlg ( const std::string & type, const std::string & name, const IInterface* parent );
  virtual ~RpcLv1RawDataValAlg();
  StatusCode initialize(); 
 
  virtual StatusCode bookHistogramsRecurrent();
  virtual StatusCode fillHistograms();
  virtual StatusCode procHistograms();
  StatusCode finalize();

 private:
    
  // Private function to add the clusters to the ntuple
  // StatusCode addClusters(std::string clusterContainerName);  
  
  // Functions declaration
  
  //Function for histogram booking and parameterd for fitting
  StatusCode  bookRPCLV1cmatimevschHistograms(std::string m_sectorlogic_name, std::string m_tower_name, std::string m_cma_name);    
  StatusCode bookRPCLV1TriggerRoadHistograms(std::string m_sectorlogic_name, std::string m_tower_name, std::string m_cma_name, std::string m_thr_name);
  StatusCode bookRPCLV1ProfilesHistograms(int m_i_sector, std::string m_sectorlogic_name, std::string m_cma_name,  int m_i_ijk, std::string m_ijk_name) ;  

   
  MuonDQAHistMap m_stationHists;


  StoreGateSvc* m_eventStore;
  
  ActiveStoreSvc* m_activeStore;
  
  StatusCode StoreTriggerType();
  int GetTriggerType() { return trigtype; }
  int trigtype;
  
  int m_sector;
  int m_side;
  int rpc_eventstotal;
  int rpc_event_inarea;
  //std::string m_clusterContainerName;

  std::vector<Identifier>* m_rpcchambersId;
  std::vector<std::string> sectorlogic_list                ;
  std::vector<std::string> sectorlogicTowerCma_name_list   ;
  std::vector<std::string> sectorlogicTowerCma_name_list2  ;
  std::vector<std::string> profile_list                    ;
  
  
  std::vector<Identifier>* m_padsId;
  std::map<std::string,int> hitsperchamber_map;
  
  bool in_area              ;
  bool histo_flag           ;  
  bool m_doClusters         ;
  bool m_checkCabling       ;
  bool m_rpclv1file         ;  
  bool m_rpclv1hist         ; 
  bool m_rpclv1prof         ;
  int  m_rpclv1reducenbins  ;
     
  const MuonGM::MuonDetectorManager* m_muonMgr;
  const RpcIdHelper* m_rpcIdHelper;
  
  const IRPCcablingSvc* m_cabling;
    
  //Declare Properties
  
  // cool
  void  bookRPCCoolHistograms(std::vector<std::string>::const_iterator &m_iter, int, int, std::string m_layer);
  bool m_doCoolDB		       ;
  char histName_char[100]	       ;
  std::string histName   	       ;
  std::vector<std::string> DB_list     ;
  
  TH1* rpcCool_StripProfile            ;
  TH1* rpcCool_PanelIdHist             ;
  
  std::string m_chamberName;
  std::string m_StationSize;
  //std::string m_key_rpc; 
  int m_StationEta;
  int m_StationPhi;
  int m_lastEvent;
  int m_cosmicStation;
 
  // Define variables in the Ntuple:
  int m_nColl;  		    // number of collection in the container
  int m_nPrd;			    // number of prd in the collection
  int m_nClus;                      // number of clusters 

  TH1* rpclv1_hitperEvent                 ;  
  TH1* rpclv1triggerlowpt_etastat	  ;
  TH1* rpclv1triggerlowpt_phistat	  ;
  TH2* rpclv1triggerlowpt_etaphistat	  ;
  TH1* rpclv1triggerhighpt_etastat	  ;
  TH1* rpclv1triggerhighpt_phistat	  ;
  TH2* rpclv1triggerhighpt_etaphistat	  ;
  TH1* rpclv1sectorlogicstat		  ;
  TH1* rpclv1towervslogicstat		  ;
  TH2* rpclv1cmalogicstat		          ;
  TH2* rpclv1ROI_LowPt                    ;
  TH2* rpclv1ROI_HighPt                   ;
  TH1* rpclv1Trigger_cond_LowPt_phi	;
  TH1* rpclv1Trigger_cond_LowPt_eta       ;
  TH1* rpclv1Trigger_cond_HighPt_phi      ;
  TH1* rpclv1Trigger_cond_HighPt_eta      ;
  
  TH1* rpclv1_BCid_per_TriggerType        ;
  
  TH1* rpclv1_BCid_per_TriggerCorr	;
  TH2* rpclv1_TriggerLy_vs_SL 		;
  
  TH2* rpclv1_TriggerCond_vs_SL           ;
  TH2* rpclv1_BCid_vs_SL		        ;
  TH2* rpclv1_LPt_BCid_vs_SL		;
  TH2* rpclv1_HPt_BCid_vs_SL		;
	    
  // Global Variables declaration
	    
  //string variables
  
  std::string sectorlogic_name          ;
  std::string tower_name                ;
  std::string cma_name                  ;
  std::string thr_name                  ;
  std::string ijk_name                  ;
  std::string cma_name_p                ;
  std::string ijk_name_p                ;
  char sectorlogic_name_char[1000]      ;
  char tower_name_char[1000]		;
  char cma_name_char[1000]		;
  char thr_name_char[1000]		;
  char ijk_name_char[1000]              ;
  char cma_name_p_char[1000]		;
  char ijk_name_p_char[1000]            ;
   
  
  int  i_sector       ;	   // pad sector Id
  int  i_padId        ;    // pad Id
  int  i_status       ;	   // status
  int  i_errorCode    ;	   // error code
  int  i_cmaId        ;    // CMA Id
  int  i_fel1Id       ;	   // fel1Id
  int  i_febcId       ;	   // febcId
  int  i_crc          ;	   // crc
  int  i_bcId         ;    // bc Id
  int  i_time         ;    // time
  int  i_ijk          ;	   // ijk code
  int  i_channel      ;	   // fired channel
  int  i_overlap      ;	   // overlap
  int  i_threshold    ;	   // threshold
					
  int  i_thr          ;
  int  thrskip        ;
					    
  int  NtriggerLowPtPhi        ;
  int  NtriggerLowPtEta        ;
  int  NtriggerHighPtPhi       ;
  int  NtriggerHighPtEta       ;
  int  ijk_trigger	       ;
  int  Layertrigger            ;	
  int  shift_pad               ;
  int  shift_cm                ;
  int  shift_ijk               ;
  int  sec_large               ;
  
 
  int  m_nPads           ;
  int  m_nCMA            ;
  int  m_nFiredChannels  ;
    
  int  i_triggertimelowpt   ;	 
  int  i_triggertimehighpt  ;  
  int  n_triggerlowpt       ;  
  int  n_triggerlowpt_eta   ;
  int  n_triggerlowpt_phi   ;
  int  n_triggerhighpt      ;
  int  n_triggerhighpt_eta  ;
  int  n_triggerhighpt_phi  ;
  int  n_trigLow_ly         ;
  int  n_trigHigh_ly        ;
  
  int  IJKtriggerLowPtPhi   ;
  int  IJKtriggerLowPtEta   ;
  int  IJKtriggerHighPtPhi  ;
  int  IJKtriggerHighPtEta  ;
     
   
  int istatPhi                         	;
  int iName                            	; 
  int ir                               	;
  
  

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
  
  std::string SideSector    ;
  int  side                 ; 
  int  sector               ;
  char sector_char[1000]    ;
  char dblZ_char[100]       ;
  char dblPhi_char[100]     ;
  
  const RpcPadContainer* rpcRDO ;  
  const RpcPad*          rdoColl;
  
  //sector logic stuff
  int m_SL_nSectors     ;
  int m_SL_nTriggerHits ;
  const RpcSectorLogicContainer* sectorLogicContainer;
  
  int N_pad, N_bin_profile  ;
  std::string cm_ch_label   ;
  std::string pad_ch_label  ;
  
  TH2* TriggerCondition_vs_CM ;
  
  // logical OR
  // PhiOr[sector logic][cm][ijk][channel]
  int PhiOr[64][4][4][32] ;
  TH2* rpclv1_logicalOR_LowPt0 ;
  TH2* rpclv1_logicalOR_LowPt1 ;
  TH2* rpclv1_logicalOR_HighPt0;
  TH2* rpclv1_logicalOR_HighPt1;
 
    
};

#endif 


 
