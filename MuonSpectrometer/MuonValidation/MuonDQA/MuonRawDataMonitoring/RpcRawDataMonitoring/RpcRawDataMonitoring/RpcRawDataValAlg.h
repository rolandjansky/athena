/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include <sstream>
#include <string.h>
#include <vector>
#include <map>

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "MuonDQAUtils/MuonDQAHistMap.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/NTuple.h"

#include "DataModel/DataLink.h"
#include "RPCcablingInterface/IRPCcablingServerSvc.h"


#include <TError.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TF1.h>
#include <inttypes.h> 

#include <sstream>
 
//#include "MuonCondInterface/IRPCConditionsSvc.h"

class TFile;
class RpcIdHelper;
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
  std::string type ;
  
  // Private function to add the clusters to the ntuple
  StatusCode addClusters(std::string clusterContainerName);  
  
  StoreGateSvc* m_eventStore;
  
  ActiveStoreSvc* m_activeStore;


  //ServiceHandle<IRPCConditionsSvc> m_pSummarySvc;
  
  

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
 
  /* void bookRPCSummaryHistograms( int, std::vector<std::string>::const_iterator &m_iter ) ; */
  void bookRPCSummaryHistograms( int, const std::string & m_quantity ) ;
  
  MuonDQAHistMap m_stationHists;

  int m_sector;
  int m_side;
  int rpc_eventstotal;
  std::string hardware_name                                  ;
  std::vector<Identifier>*    m_chambersId		     ;
  std::vector<std::string>    hardware_name_list             ;  
  std::vector<std::string>    layer_name_list                ; 
  std::vector<std::string>    layer_name_list_panel          ; 
  std::vector<int>            layer_name_bin_list            ; 
  std::vector<int>            layer_name_bin_list_panel      ;
  std::vector<std::string>    layervslayer_name_list         ;
  std::vector<std::string>    layerPhivsEta_name_list        ;
  std::vector<std::string>    layerPhivsEtaSector_name_list  ;
  // std::vector<std::string> cm_time_list                   ;
  // std::vector<int>         ch16_index_list                ;
  int                         StationNameViewIndex[100][2]   ;
  float                       StationNameSectorSize[100]     ;
  float                       StationPivotSectorSize[100]    ;
  
  int shiftphiatlas ;
   
  std::vector<Identifier>* m_padsId;
  std::map<std::string,int> hitsperchamber_map;
  
  bool histo_flag;  
   
  const MuonGM::MuonDetectorManager* m_muonMgr;
  const RpcIdHelper* m_rpcIdHelper;
  
  const IRPCcablingSvc* m_cabling;
   

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
  bool Clus_Retr  			;
       
  int m_StationEta       		;
  int m_StationPhi       		;
  int m_lastEvent        		;
  int m_cosmicStation    		;
  std::string sector_num 		;
 
  // Define variables in the Ntuple:
  int m_nColl;  		    // number of collection in the container
  int m_nPrd ;			    // number of prd in the collection
  int m_nTrig;
  int m_nClus;                      // number of clusters

  int m_nPrd_BA	;
  int m_nPrd_BC ;
  
  int ndbl ;
  
  enum {enumBA_LowPt, enumBC_LowPt, enumBA_HighPt, enumBC_HighPt};
  std::vector<TH2I*> rpc2DEtaStationTriggerHits_Side_Pt      ; 
  std::vector<TH2F*> rpc2DEtaStationTriggerHits_Side_Pt_norm ; 
  int   nEtaStatFired_BA_LowPt	       ;
  int   nEtaStatFired_BC_LowPt	       ;
  int   nEtaStatFired_BA_HighPt	       ;
  int   nEtaStatFired_BC_HighPt	       ; 
   
  std::vector<TH1I*> rpcNumberEtaStatFired_Side_Pt      ;
  bool  m_doTrigEvol		       ;
  bool  m_doLumiPlot		       ;
  bool  m_doTriggerHits                ;
  int   minStatTrEvol                  ;
  int   nTrigEtaStat_BA_LowPt	       ;
  int   nTrigEtaStat_BC_LowPt	       ;
  int   nTrigEtaStat_BA_HighPt	       ;
  int   nTrigEtaStat_BC_HighPt	       ; 
  
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
  std::vector<TH2I*> rpc2DPanelHits ; 
  std::vector<TH2I*> rpc1DvsLBPanelHits ;
  enum {enum_Phi_TrigTowerLowPt_BA ,enum_Phi_TrigTowerHighPt_BA, enum_Eta_TrigTowerLowPt_BA ,enum_Eta_TrigTowerHighPt_BA ,
        enum_Phi_TrigTowerLowPt_BC ,enum_Phi_TrigTowerHighPt_BC, enum_Eta_TrigTowerLowPt_BC ,enum_Eta_TrigTowerHighPt_BC };    
  std::vector<TH2I*> rpc1DvsLBTrigTowerHits ;  
  
  TH1* rpcEtaTime	   ;
  TH1* rpcPhiTime	   ;
  TH1* rpcevents	   ;
  TH1* rpctime		   ;
    
  
  TH1* rpcclusters	   ;
  TH1* rpcCSEta 	   ;
  TH1* rpcCSPhi 	   ;

  TH1* rpcCSEta_BA 	   ;
  TH1* rpcCSPhi_BA 	   ;
  TH1* rpcCSEta_BC 	   ;
  TH1* rpcCSPhi_BC 	   ;

  
  
  TH2* rpcPhivsEtaAtlasPivot0   	       ;
  TH2* rpcPhivsEtaAtlasPivot1   	       ;
  TH2* rpcPhivsEtaAtlasLowPt0   	       ;
  TH2* rpcPhivsEtaAtlasLowPt1   	       ;
  TH2* rpcPhivsEtaAtlasHighPt0  	       ;
  TH2* rpcPhivsEtaAtlasHighPt1  	       ;
  TH2* rpcPhivsEtaAtlasLowPt_TriggerOut        ;
  TH2* rpcPhivsEtaAtlasHighPt_TriggerFromLowPt ;
  TH2* rpcPhivsEtaAtlasHighPt_TriggerOut       ;
  
  TH2* rpcPhivsEtaAtlasPivot0_PhivsZ	       ;
  TH2* rpcPhivsEtaAtlasPivot1_PhivsZ	       ;
  TH2* rpcPhivsEtaAtlasLowPt0_PhivsZ	       ;
  TH2* rpcPhivsEtaAtlasLowPt1_PhivsZ	       ;
  TH2* rpcPhivsEtaAtlasHighPt0_PhivsZ	       ;
  TH2* rpcPhivsEtaAtlasHighPt1_PhivsZ	       ;
  TH2* rpcPhivsEtaAtlasLowPt_TriggerOut_PhivsZ        ;
  TH2* rpcPhivsEtaAtlasHighPt_TriggerFromLowPt_PhivsZ ;
  TH2* rpcPhivsEtaAtlasHighPt_TriggerOut_PhivsZ       ;
  
    
  TH2* rpc2DEtaStation                	    ;
  TH2* rpc2DEtaStationGap1           	    ;
  TH2* rpc2DEtaStationGap2           	    ;
  TH2* rpc2DEtaStationTriggerHits    	    ;
  
  TH2* GlobalHitsPerRPCMiddle        	    ;
  TH2* GlobalHitsPerRPCOuter         	    ;
  
  TH2* EtavsPhi_TriggeredMuons_LowPt        ;
  TH2* EtavsPhi_TriggeredMuons_HighPt        ;
  TH2* EtavsPhi_TriggeredMuons_Pt1          ;
  TH2* EtavsPhi_TriggeredMuons_Pt2          ;
  TH2* EtavsPhi_TriggeredMuons_Pt3          ;
  TH2* EtavsPhi_TriggeredMuons_Pt4          ;
  TH2* EtavsPhi_TriggeredMuons_Pt5          ;
  TH2* EtavsPhi_TriggeredMuons_Pt6          ;
  
  TH1* RPC_Threshold_Eta             	    ;	  
  TH1* RPC_Threshold_Phi             	    ;
 
  TH2* RPC_TriggerRoad                      ;
  TH2* RPC_TriggerRoad_Large_Eta      ;
  TH2* RPC_TriggerRoad_Large_Phi      ;
  TH2* RPC_TriggerRoad_Small_Eta      ;
  TH2* RPC_TriggerRoad_Small_Phi      ;
    
  
  TH1* TotalNumber_of_RPC_hits_per_events_BA;
  TH1* TotalNumber_of_RPC_hits_per_events_BC;
  TH1* rpctime_BA			    ;
  TH1* rpctime_BC			    ;
  TH1* rpctime_LPt_BA			;
  TH1* rpctime_LPt_BC			;
  TH1* rpctime_HPt_BA			;
  TH1* rpctime_HPt_BC			;


  TH1* rpctime_Sector1_BA		    ;
  TH1* rpctime_Sector2_BA		    ;
  TH1* rpctime_Sector3_BA		    ;
  TH1* rpctime_Sector4_BA		    ;
  TH1* rpctime_Sector5_BA		    ;
  TH1* rpctime_Sector6_BA		    ;
  TH1* rpctime_Sector7_BA		    ;
  TH1* rpctime_Sector8_BA		    ;
  TH1* rpctime_Sector9_BA		    ;
  TH1* rpctime_Sector10_BA		    ;
  TH1* rpctime_Sector11_BA		    ;
  TH1* rpctime_Sector12_BA		    ;
  TH1* rpctime_Sector13_BA		    ;
  TH1* rpctime_Sector14_BA		    ;
  TH1* rpctime_Sector15_BA		    ;
  TH1* rpctime_Sector16_BA		    ;

  TH1* rpctime_Sector1_BC		    ;
  TH1* rpctime_Sector2_BC		    ;
  TH1* rpctime_Sector3_BC		    ;
  TH1* rpctime_Sector4_BC		    ;
  TH1* rpctime_Sector5_BC		    ;
  TH1* rpctime_Sector6_BC		    ;
  TH1* rpctime_Sector7_BC		    ;
  TH1* rpctime_Sector8_BC		    ;
  TH1* rpctime_Sector9_BC		    ;
  TH1* rpctime_Sector10_BC		    ;
  TH1* rpctime_Sector11_BC		    ;
  TH1* rpctime_Sector12_BC		    ;
  TH1* rpctime_Sector13_BC		    ;
  TH1* rpctime_Sector14_BC		    ;
  TH1* rpctime_Sector15_BC		    ;
  TH1* rpctime_Sector16_BC		    ;

  
  TH1* rpcTriggerHitsPerEvents_Eta_LowPt    ;  
  TH1* rpcTriggerHitsPerEvents_Phi_LowPt    ;  
  TH1* rpcTriggerHitsPerEvents_Eta_HighPt   ;  
  TH1* rpcTriggerHitsPerEvents_Phi_HighPt   ;
  
  TH1* rpc1DStationNameHitsSideA            ;
  TH1* rpc1DStationNameTriggerHitsSideA     ;
  TH1* rpc1DStationNameHitsSideC            ;
  TH1* rpc1DStationNameTriggerHitsSideC     ;
  
  
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

  int Phi_Rpc_Track;
  int layertype    ;
  int planetype    ;

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
  float Impact_B_IP, polar             ;
  float pseudoeta, anglephi            ;
  float rhoxy, rho, costh, sinth, pseta;
  float polarx, polary                 ; 
  //int   NPhiStrip, NEtaStrip         ;
  float phipitch, etapitch             ;
  int   Layer	                       ;
  int   phistripN, etastripN           ; 
  int   foundonehiteta, foundonehitphi ;  
  float stripontrack, residuals        ;
  float TrackType                      ;
  float small_large                    ;
  float hitstripphi                    ;
  float hitstripeta                    ; 
  
 
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
  
  int NetaPanelsTot	                ;
  int ShiftEtaPanelsTot                 ;
  int NetaPanelsTotSideA                ;
  int NetaPanelsTotSideC                ;
  int rpcpanel_dbindex                  ; 
  int rpctower_dbindex                  ; 
  
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
  
  int NTrigger_Eta_LowPt                ;  
  int NTrigger_Phi_LowPt                ;  
  int NTrigger_Eta_HighPt               ;  
  int NTrigger_Phi_HighPt               ;
  
  // cool
  void  bookRPCCoolHistograms(std::vector<std::string>::const_iterator &m_iter, int, int, std::string m_layer);
  bool m_doCoolDB		       ;
  char histName_char[100]	       ;
  std::string histName   	       ;
  std::vector<std::string> DB_list     ;
  
  TH1* rpcCool_StripProfile            ;
  TH1* rpcCool_PanelIdHist             ;
  
  
  
};

#endif
 


 
 
