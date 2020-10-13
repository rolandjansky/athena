/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Package : RpcRawDataValAlg
// Authors:  N. Benekos(Illinois) - M. Bianco(INFN-Lecce) - G. Chiodini(INFN-Lecce) - A. Guida (INFN-Lecce) 
// Sept. 2007
// RPC Cluster Monitoring added by G. Chiodini(INFN-Lecce)
// March 2008
//
// DESCRIPTION:
// Subject: RPC-->Offline Muon Data Quality
// 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
         
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"
  
#include "MuonReadoutGeometry/RpcReadoutSet.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h"  
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "RPCcablingInterface/IRPCcablingServerSvc.h"

#include "MuonRDO/RpcFiredChannel.h"
#include "MuonRDO/RpcCoinMatrix.h"
#include "MuonRDO/RpcPad.h"
#include "MuonRDO/RpcPadContainer.h"

#include "MuonDigitContainer/RpcDigitContainer.h"
 
#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include "MuonTrigCoinData/RpcCoinDataContainer.h"

#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonDQAUtils/MuonCosmicSetup.h"
#include "MuonDQAUtils/MuonDQAHistMap.h" 
 
#include "RpcRawDataMonitoring/RpcRawDataValAlg.h"
#include "AthenaMonitoring/AthenaMonManager.h"
  
#include "xAODEventInfo/EventInfo.h" 
#include "RpcRawDataMonitoring/RpcGlobalUtilities.h"  
    
#include <fstream> 
#include <sstream>
#include <iostream>     

using namespace std;

static const   int maxColl	      =  1200;
static const   int maxPRD 	      = 50000;
static const   int maxClus	      =  1000;
static const   int timeminrange	      =	 -200;
static const   int timemaxrange	      =	  200;
static const   int timeNbin	      =	  128;
static const   int nstripfiducial     =     0;
static const   int nstripfiduceff     =     3;
static const   int MergePointDistance =    50;
static const   int EtaStationSpan     =     2;
static const   int DoublePhiSpan      =     1;
static const float Chi2dofCut         =     1;

/////////////////////////////////////////////////////////////////////////////

RpcRawDataValAlg::RpcRawDataValAlg( const std::string & type, const std::string & name, const IInterface* parent )
  :ManagedMonitorToolBase( type, name, parent )
  //,m_pSummarySvc("RPCCondSummarySvc", name)
{
  // Declare the properties 
  declareProperty("DoRpcEsd",            m_doRpcESD		= false	); 
  declareProperty("CheckCabling",        m_checkCabling		= false	);
  declareProperty("RpcFile",             m_rpcfile		= false	);    
  declareProperty("RpcChamberHist",      m_rpcchamberhist	= false	);     
  declareProperty("RpcSectorHist",       m_rpcsectorhist	= false	);  
  declareProperty("RpcReduceNbins",      m_rpcreducenbins	= 8	);	   
  declareProperty("RpcReduceNbinsStrip", m_rpcreducenbinsstrip	= 8	);	   
  declareProperty("ChamberName",         m_chamberName		= "XXX"	);
  declareProperty("StationSize",         m_StationSize		= "XXX"	);
  declareProperty("StationEta",          m_StationEta		= -100	);
  declareProperty("StationPhi",          m_StationPhi		= -100	);
  declareProperty("LastEvent",           m_lastEvent		= 0	);
  declareProperty("Sector",              m_sector		= 0	); 
  declareProperty("CosmicStation",       m_cosmicStation	= 0	);
  declareProperty("Side",                m_side			= 0	); 
  declareProperty("Clusters",            m_doClusters		= true	);			
  declareProperty("doTrigEvol",		 m_doTrigEvol		= false	); // historical plot of trigger hits		
  declareProperty("doLumiPlot",	         m_doLumiPlot		= false	); 		
  declareProperty("doTriggerHits",	 m_doTriggerHits	= false	); 
  declareProperty("minStatTrEvol",	 minStatTrEvol		= 300	);  
  declareProperty("lv1Thres_0",		 m_lv1Thres_0		= 99	);
  declareProperty("lv1Thres_1",		 m_lv1Thres_1		= 1	);
  declareProperty("lv1Thres_2",		 m_lv1Thres_2		= 2	);
  declareProperty("lv1Thres_3",		 m_lv1Thres_3		= 3	);
  declareProperty("doCoolDB",		 m_doCoolDB		= true	);
  declareProperty("ClusterContainer",    m_clusterContainerName = "rpcClusters"		);
  declareProperty("RpcPrepDataContainer",m_key_rpc		= "RPC_Measurements"	);
  declareProperty("RPCTriggerContainer", m_key_trig		= "RPC_triggerHits"	);
  declareProperty("MinimunEntries",      MinEntries		= 10	);    // min entries required for summary plot 
  declareProperty("LB_Nbins"      ,      m_LB_Nbins		= 300	);     
  declareProperty("LBmax"         ,      m_LBmax		= 1500	);
  m_padsId     = 0;
  m_chambersId = 0;
} 
                            
RpcRawDataValAlg::~RpcRawDataValAlg()
{
  // fixes fot Memory leak
  if (m_padsId) { 
    delete m_padsId;
    m_padsId = 0; 
  }
  if (m_chambersId) { 
    delete m_chambersId;
    m_chambersId = 0; 
  } 
  ATH_MSG_INFO (  " deleting RpcRawDataValAlg " );
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode RpcRawDataValAlg::initialize(){


  ATH_MSG_INFO (  "in initializing RpcRawDataValAlg" );

  ATH_MSG_DEBUG (  "******************" );
  ATH_MSG_DEBUG (  "doRpcESD: " << m_doRpcESD );
  ATH_MSG_DEBUG (  "RpcFile: "<< m_rpcfile );
  
  StatusCode sc;

  // Store Gate store
  sc = serviceLocator()->service("StoreGateSvc", m_eventStore);
  if (sc != StatusCode::SUCCESS ) {
    ATH_MSG_ERROR (  " Cannot get StoreGateSvc " );
    return sc ;
  }
  // retrieve the active store
  sc = serviceLocator()->service("ActiveStoreSvc", m_activeStore);
  if (sc != StatusCode::SUCCESS ) {
    ATH_MSG_ERROR (  " Cannot get ActiveStoreSvc " );
    return sc ;
  }

  // Initialize the IdHelper
  StoreGateSvc* detStore = 0;
  sc = service("DetectorStore", detStore);
  if (sc.isFailure()) {
    ATH_MSG_FATAL(  "DetectorStore service not found !" );
    return StatusCode::FAILURE;
  }   
  
  // Retrieve the MuonDetectorManager  
  sc = detStore->retrieve(m_muonMgr);
  if (sc.isFailure()) {
    ATH_MSG_FATAL(  "Cannot get MuonDetectorManager from detector store" );
    return StatusCode::FAILURE;
  }  
  else {
    ATH_MSG_DEBUG (  " Found the MuonDetectorManager from detector store. " );
  }

  sc = detStore->retrieve(m_rpcIdHelper,"RPCIDHELPER");
  if (sc.isFailure())
    {
      ATH_MSG_ERROR (  "Can't retrieve RpcIdHelper" );
      return sc;
    }	 
    
  // get RPC cablingSvc
  const IRPCcablingServerSvc* RpcCabGet = 0;
  sc = service("RPCcablingServerSvc", RpcCabGet);
  if (sc.isFailure()) {
    ATH_MSG_WARNING (  "Could not get RPCcablingServerSvc !" );
    return StatusCode::FAILURE;
  }
 
  sc = RpcCabGet->giveCabling(m_cabling);
  if (sc.isFailure()) {
    ATH_MSG_WARNING (  "Could not get RPCcablingSvc from the Server !" );
    m_cabling = 0;
    return StatusCode::FAILURE;
  } else { ATH_MSG_DEBUG (  " Found the RPCcablingSvc. " );    }
  /*
    sc = m_pSummarySvc.retrieve();
    if (StatusCode::SUCCESS not_eq sc) {
    ATH_MSG_WARNING (  "Could not retrieve the summary service" );
    }
  */
  hardware_name_list.push_back("XXX");
  
  ManagedMonitorToolBase::initialize().ignore();  //  Ignore the checking code; 
  
 
  rpc_eventstotal=0;  
  
  // Clear Muon Monitoring Histograms 
  rpc2DEtaStationTriggerHits_Side_Pt.clear();
  rpcNumberEtaStatFired_Side_Pt.clear();
  rpc2DPanelHits        .clear(); 
  rpc1DvsLBPanelHits    .clear(); 
  rpc1DvsLBTrigTowerHits.clear();
  

  //2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG,53=BME  
  StationNameViewIndex[ 2][0]= 0;
  StationNameViewIndex[ 2][1]= 1;
  StationNameViewIndex[ 3][0]= 2;
  StationNameViewIndex[ 3][1]= 3;
  StationNameViewIndex[ 4][0]= 8;
  StationNameViewIndex[ 4][1]= 9;
  StationNameViewIndex[ 5][0]= 6;
  StationNameViewIndex[ 5][1]= 7;
  StationNameViewIndex[ 8][0]= 4;
  StationNameViewIndex[ 8][1]= 5;
  StationNameViewIndex[ 9][0]=10;
  StationNameViewIndex[ 9][1]=11;
  StationNameViewIndex[10][0]=10;
  StationNameViewIndex[10][1]=11;
  StationNameViewIndex[53][0]= 2;
  StationNameViewIndex[53][1]= 2; 
  
  //size in cm2 =zmax in cm * layers * # phi strips * 3 cm pitch
  StationNameSectorSize[ 2]=  965*4*6*128*3;
  StationNameSectorSize[ 3]=  945*4*6* 96*3;
  StationNameSectorSize[ 4]= 1235*2*6*160*3;
  StationNameSectorSize[ 5]= 1285*2*6*128*3;
  StationNameSectorSize[ 8]=  685*4*2* 96*3;
  StationNameSectorSize[ 9]= 1235*4*2*128*3;
  StationNameSectorSize[10]= 1235*4*2*128*3;
  StationNameSectorSize[53]=  965*4*6*128*3;
  
  StationPivotSectorSize[ 2]=  965*6*128*3;
  StationPivotSectorSize[ 3]=  945*6* 96*3;
  StationPivotSectorSize[ 4]= 1235*6*160*3;
  StationPivotSectorSize[ 5]= 1285*6*128*3;
  StationPivotSectorSize[ 8]=  685*2* 96*3;
  StationPivotSectorSize[ 9]= 1235*2*128*3;
  StationPivotSectorSize[10]= 1235*2*128*3;
  StationPivotSectorSize[53]=  965*6*128*3;
 
 
 
 
   Nphi_Zstrips = 0 ;
   NetaPanelsTot = 0 ;
   ShiftEtaPanelsTot = 0 ;
   NetaPanelsTotSideA = 0 ;
   NetaPanelsTotSideC = 0 ;
   rpcpanel_dbindex = 0 ;
   rpctower_dbindex = 0 ;
   rpc2DEtaStatBinX_BA = 0 ;
   rpc2DEtaStatBinX_BC = 0 ;
   rpc2DEtaStatBinY = 0 ;
   etaStatShiftX_BA = 0 ;
   etaStatShiftX_BC = 0 ;
   binz = 0 ;
   binminz = 0 ;
   binmaxz = 0 ;
   binx = 0 ;
   binminx = 0 ;
   binmaxx = 0 ;
   panelBin = 0 ;
   indexplane = 0 ;
   NTrigger_Eta_LowPt = 0 ;
   NTrigger_Phi_LowPt = 0 ;
   NTrigger_Eta_HighPt = 0 ;
   NTrigger_Phi_HighPt = 0 ;
   rpcCool_StripProfile = 0 ;
   rpcCool_PanelIdHist = 0 ;
 
 
   m_first = true ;
  
  
  return StatusCode::SUCCESS;
}

/*----------------------------------------------------------------------------------*/
StatusCode RpcRawDataValAlg::fillHistograms()
/*----------------------------------------------------------------------------------*/
{
  StatusCode sc = StatusCode::SUCCESS;
  
  
  ATH_MSG_DEBUG (  "RpcRawDataValAlg::RPC RawData Monitoring Histograms being filled" );
  if( m_doRpcESD==true ) { if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online ) {  
    
      //get lumiblock for analysis
       
      int lumiblock = -1 ;
	
      const DataHandle<xAOD::EventInfo> eventInfo;
      StatusCode sc = m_eventStore->retrieve( eventInfo );
      if (sc.isFailure()) {
	ATH_MSG_INFO ( "no event info" );
	return StatusCode::SUCCESS;
      }
      else {
	  
	lumiblock = eventInfo->lumiBlock()  ;
	  
	ATH_MSG_DEBUG ( "event LB " << lumiblock ); 
	      
      }
              
      float AverageLuminosityWeight = 1;
      
  
    
    
      const Muon::RpcPrepDataContainer* rpc_container;
      sc = (*m_activeStore)->retrieve(rpc_container, m_key_rpc);
      if (sc.isFailure()) {
	ATH_MSG_ERROR (  " Cannot retrieve RpcPrepDataContainer " << m_key_rpc );
	return sc;
      }

      ATH_MSG_DEBUG ( "****** rpc->size() : " << rpc_container->size()) ;  
    
      Muon::RpcPrepDataContainer::const_iterator containerIt;
   
      m_nColl = 0;
      m_nPrd = 0;
      m_nPrd_BA =0; m_nPrd_BC =0;
     
      m_nTrig = 0;
     
    
      type="RPC";


      // recall general histos  
      m_generic_path_rpcmonitoring = "Muon/MuonRawDataMonitoring/RPC";
      MonGroup rpcprd_shift( this, m_generic_path_rpcmonitoring+"/Overview", run, ATTRIB_UNMANAGED )    ;
      MonGroup rpcprd_expert( this, m_generic_path_rpcmonitoring+"/Overview", run, ATTRIB_UNMANAGED )   ;
      MonGroup rpc_dqmf_global( this, m_generic_path_rpcmonitoring + "/GLOBAL", run, ATTRIB_UNMANAGED )  ;
      MonGroup rpcprd_dq_BA( this, m_generic_path_rpcmonitoring + "/RPCBA", run, ATTRIB_UNMANAGED  )     ;
      MonGroup rpcprd_dq_BC( this, m_generic_path_rpcmonitoring + "/RPCBC", run, ATTRIB_UNMANAGED )     ;
      MonGroup rpcprd_dq_Panel( this, m_generic_path_rpcmonitoring + "/GLOBAL", run, ATTRIB_UNMANAGED )     ;
      MonGroup rpcTrigRoad ( this, m_generic_path_rpcmonitoring + "/TriggerRoad", run, ATTRIB_UNMANAGED );
      MonGroup rpcCoolDb( this, m_generic_path_rpcmonitoring+"/CoolDB", run, ATTRIB_UNMANAGED )         ;
    
       
      sc = rpcprd_shift.getHist(rpctime, "Time_Distribution") ;
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register rpctime hist to MonGroup" );
          
      sc = rpcprd_shift.getHist(rpcevents,"Number_of_RPC_hits_per_event");		
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register rpcevents hist to MonGroup" );
        
      sc = rpcprd_expert.getHist(rpcEtaTime,"Eta_Time_Distribution");		
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register rpcEtaTime hist to MonGroup" );
     
      sc = rpcprd_expert.getHist(rpcPhiTime,"Phi_Time_Distribution");				
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register rpcPhiTime hist to MonGroup" );
        
      sc = rpc_dqmf_global.getHist(rpc2DEtaStation,"rpc2DEtaStation");		
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register rpc2DEtaStation hist to MonGroup" );
    
      sc = rpcprd_expert.getHist(rpc2DEtaStationGap1,"rpc2DEtaStationGap1");		
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register rpc2DEtaStationGap1 hist to MonGroup" ); 
    
      sc = rpcprd_expert.getHist(rpc2DEtaStationGap2,"rpc2DEtaStationGap2");		
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register rpc2DEtaStationGap2 hist to MonGroup" );
    
      sc = rpcprd_shift.getHist(rpc2DEtaStationTriggerHits,"rpc2DEtaStationTriggerHits");		
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register rpc2DEtaStationLowPtTriggerHits hist to MonGroup" );  
  
      sc = rpc_dqmf_global.getHist(GlobalHitsPerRPCMiddle, "GlobalHitsPerRPCMiddle" );
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get GlobalHitsPerRPCMiddle hist" );
     
      sc = rpc_dqmf_global.getHist(GlobalHitsPerRPCOuter, "GlobalHitsPerRPCOuter" );
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get GlobalHitsPerRPCOuter hist"  );
  
      sc = rpc_dqmf_global.getHist(EtavsPhi_TriggeredMuons_LowPt, "EtavsPhi_TriggeredMuons_LowPt");
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get EtavsPhi_TriggeredMuons_LowPt hist"  );

      sc = rpc_dqmf_global.getHist(EtavsPhi_TriggeredMuons_HighPt, "EtavsPhi_TriggeredMuons_HighPt");
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get EtavsPhi_TriggeredMuons_HighPt hist"  );
    
      sc = rpc_dqmf_global.getHist(EtavsPhi_TriggeredMuons_Pt1, "EtavsPhi_TriggeredMuons_Pt1");
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get EtavsPhi_TriggeredMuons_Pt1 hist"  );
  
      sc = rpc_dqmf_global.getHist(EtavsPhi_TriggeredMuons_Pt2, "EtavsPhi_TriggeredMuons_Pt2");
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get EtavsPhi_TriggeredMuons_Pt2 hist"  );
    
      sc = rpc_dqmf_global.getHist(EtavsPhi_TriggeredMuons_Pt3, "EtavsPhi_TriggeredMuons_Pt3");
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get EtavsPhi_TriggeredMuons_Pt3 hist"  );
    
      sc = rpc_dqmf_global.getHist(EtavsPhi_TriggeredMuons_Pt4, "EtavsPhi_TriggeredMuons_Pt4");
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get EtavsPhi_TriggeredMuons_Pt4 hist"  );
    
      sc = rpc_dqmf_global.getHist(EtavsPhi_TriggeredMuons_Pt5, "EtavsPhi_TriggeredMuons_Pt5");
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get EtavsPhi_TriggeredMuons_Pt5 hist"  );
    
      sc = rpc_dqmf_global.getHist(EtavsPhi_TriggeredMuons_Pt6, "EtavsPhi_TriggeredMuons_Pt6");
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get EtavsPhi_TriggeredMuons_Pt6 hist"  );
    
      sc = rpcprd_expert.getHist(rpcPhivsEtaAtlasPivot0,"AtlasPivot0");	    
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register AtlasPivot0 hist to MonGroup" );
  
      sc = rpcprd_expert.getHist(rpcPhivsEtaAtlasPivot1,"AtlasPivot1");	    
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register AtlasPivot1 hist to MonGroup" );
  
      sc = rpcprd_expert.getHist(rpcPhivsEtaAtlasLowPt0,"AtlasLowPt0");	    
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register AtlasLowPt0 hist to MonGroup" );
  
      sc = rpcprd_expert.getHist(rpcPhivsEtaAtlasLowPt1,"AtlasLowPt1");	    
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register AtlasLowPt1 hist to MonGroup" );
  
      sc = rpcprd_expert.getHist(rpcPhivsEtaAtlasHighPt0,"AtlasHighPt0");	    
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register AtlasHighPt0 hist to MonGroup" );
   
      sc = rpcprd_expert.getHist(rpcPhivsEtaAtlasHighPt1,"AtlasHighPt1");	    
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register AtlasHighPt1 hist to MonGroup" );
  
      sc = rpcprd_expert.getHist(rpcPhivsEtaAtlasLowPt_TriggerOut,"AtlasLowPt_TriggerOut");		    
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register AtlasLowPt_TriggerOut hist to MonGroup" );
     
      sc = rpcprd_expert.getHist(rpcPhivsEtaAtlasHighPt_TriggerFromLowPt,"AtlasHighPt_TriggerFromLowPt");	    
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register AtlasHighPt_TriggerFromLowPt hist to MonGroup" );
  
      sc = rpcprd_expert.getHist(rpcPhivsEtaAtlasHighPt_TriggerOut,"AtlasHighPt_TriggerOut"); 	    
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register AtlasHighPt_TriggerOut hist to MonGroup" );	
  
      sc = rpcprd_expert.getHist(rpcPhivsEtaAtlasPivot0_PhivsZ,"AtlasPivot0_PhivsZ");	    
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register AtlasPivot0_PhivsZ hist to MonGroup" );
  
      sc = rpcprd_expert.getHist(rpcPhivsEtaAtlasPivot1_PhivsZ,"AtlasPivot1_PhivsZ");	    
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register AtlasPivot1_PhivsZ hist to MonGroup" );
  
      sc = rpcprd_expert.getHist(rpcPhivsEtaAtlasLowPt0_PhivsZ,"AtlasLowPt0_PhivsZ");	    
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register AtlasLowPt0_PhivsZ hist to MonGroup" );
  
      sc = rpcprd_expert.getHist(rpcPhivsEtaAtlasLowPt1_PhivsZ,"AtlasLowPt1_PhivsZ");	    
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register AtlasLowPt1_PhivsZ hist to MonGroup" );
  
      sc = rpcprd_expert.getHist(rpcPhivsEtaAtlasHighPt0_PhivsZ,"AtlasHighPt0_PhivsZ");	    
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register AtlasHighPt0_PhivsZ hist to MonGroup" );
    
      sc = rpcprd_expert.getHist(rpcPhivsEtaAtlasHighPt1_PhivsZ,"AtlasHighPt1_PhivsZ");	    
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register AtlasHighPt1 hist to MonGroup" );
    
      sc = rpcprd_expert.getHist(rpcPhivsEtaAtlasLowPt_TriggerOut_PhivsZ,"AtlasLowPt_TriggerOut_PhivsZ");		    
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register AtlasLowPt_TriggerOut_PhivsZ hist to MonGroup" );
    
      sc = rpcprd_expert.getHist(rpcPhivsEtaAtlasHighPt_TriggerFromLowPt_PhivsZ,"AtlasHighPt_TriggerFromLowPt_PhivsZ");	    
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register AtlasHighPt_TriggerFromLowPt hist to MonGroup" );
      
      sc = rpcprd_expert.getHist(rpcPhivsEtaAtlasHighPt_TriggerOut_PhivsZ,"AtlasHighPt_TriggerOut_PhivsZ"); 	    
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register AtlasHighPt_TriggerOut_PhivsZ hist to MonGroup" );	
     
      sc = rpcprd_dq_BA.getHist(TotalNumber_of_RPC_hits_per_events_BA, "TotalNumber_of_RPC_hits_per_events_BA") ;
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get TotalNumber_of_RPC_hits_per_events_BA hist " );
    
      sc = rpcprd_dq_BC.getHist(TotalNumber_of_RPC_hits_per_events_BC, "TotalNumber_of_RPC_hits_per_events_BC") ;
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get TotalNumber_of_RPC_hits_per_events_BC hist ");
    
      sc = rpcprd_dq_BA.getHist(rpcCSEta_BA, "rpcCSEta_BA" );
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get rpcCSEta_BA hist " );

      sc = rpcprd_dq_BC.getHist(rpcCSEta_BC, "rpcCSEta_BC" );
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get rpcCSEta_BC hist " );

      sc = rpcprd_dq_BA.getHist(rpcCSPhi_BA, "rpcCSPhi_BA" );
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get rpcCSPhi_BA hist " );

      sc = rpcprd_dq_BC.getHist(rpcCSPhi_BC, "rpcCSPhi_BC" );
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get rpcCSPhi_BC hist " );

      sc = rpcprd_dq_BA.getHist(rpctime_LPt_BA, "rpctime_LPt_BA" );
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get rpctime_LPt_BA hist " );
    
      sc = rpcprd_dq_BC.getHist(rpctime_LPt_BC, "rpctime_LPt_BC" );
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get rpctime_LPt_BC hist " );

      sc = rpcprd_dq_BA.getHist(rpctime_HPt_BA, "rpctime_HPt_BA" );
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get rpctime_HPt_BA hist " );
    
      sc = rpcprd_dq_BC.getHist(rpctime_HPt_BC, "rpctime_HPt_BC" );
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get rpctime_HPt_BC hist " );   
    
      sc = rpcprd_dq_BA.getHist(rpctime_BA, "rpctime_BA" );
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get rpctime_BA hist " );
    
      sc = rpcprd_dq_BC.getHist(rpctime_BC, "rpctime_BC" );
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get rpctime_BC hist " );
 
      sc = rpcprd_dq_BA.getHist(rpctime_Sector1_BA, "rpctime_Sector1_BA" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpctime_Sector1_BA hist "  );
    
      sc = rpcprd_dq_BA.getHist(rpctime_Sector2_BA, "rpctime_Sector2_BA" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpctime_Sector2_BA hist "  );
   
      sc = rpcprd_dq_BA.getHist(rpctime_Sector3_BA, "rpctime_Sector3_BA" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpctime_Sector3_BA hist "  );
    
      sc = rpcprd_dq_BA.getHist(rpctime_Sector4_BA, "rpctime_Sector4_BA" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpctime_Sector4_BA hist "  );
    
      sc = rpcprd_dq_BA.getHist(rpctime_Sector5_BA, "rpctime_Sector5_BA" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpctime_Sector5_BA hist "  );
    
      sc = rpcprd_dq_BA.getHist(rpctime_Sector6_BA, "rpctime_Sector6_BA" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpctime_Sector6_BA hist "  );
    
      sc = rpcprd_dq_BA.getHist(rpctime_Sector7_BA, "rpctime_Sector7_BA" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpctime_Sector7_BA hist "  );
    
      sc = rpcprd_dq_BA.getHist(rpctime_Sector8_BA, "rpctime_Sector8_BA" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpctime_Sector8_BA hist "  );
    
      sc = rpcprd_dq_BA.getHist(rpctime_Sector9_BA, "rpctime_Sector9_BA" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpctime_Sector9_BA hist "  );
    
      sc = rpcprd_dq_BA.getHist(rpctime_Sector10_BA, "rpctime_Sector10_BA" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpctime_Sector10_BA hist "  );
    
      sc = rpcprd_dq_BA.getHist(rpctime_Sector11_BA, "rpctime_Sector11_BA" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpctime_Sector11_BA hist "  );
    
      sc = rpcprd_dq_BA.getHist(rpctime_Sector12_BA, "rpctime_Sector12_BA" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpctime_Sector12_BA hist "  );
    
      sc = rpcprd_dq_BA.getHist(rpctime_Sector13_BA, "rpctime_Sector13_BA" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpctime_Sector13_BA hist "  );
    
      sc = rpcprd_dq_BA.getHist(rpctime_Sector14_BA, "rpctime_Sector14_BA" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpctime_Sector14_BA hist "  );
    
      sc = rpcprd_dq_BA.getHist(rpctime_Sector15_BA, "rpctime_Sector15_BA" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpctime_Sector16_BA hist "  );
    
      sc = rpcprd_dq_BA.getHist(rpctime_Sector16_BA, "rpctime_Sector16_BA" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpctime_Sector2_BA hist "  );
            
      sc = rpcprd_dq_BA.getHist(rpc1DStationNameHitsSideA, "rpc1DStationNameHitsSideA" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpc1DStationNameHitsSideA hist "  );
            
      sc = rpcprd_dq_BA.getHist(rpc1DStationNameTriggerHitsSideA, "rpc1DStationNameTriggerHitsSideA" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpc1DStationNameTriggerHitsSideA hist "  );

      sc = rpcprd_dq_BC.getHist(rpctime_Sector1_BC, "rpctime_Sector1_BC" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpctime_Sector1_BC hist "  );
    
      sc = rpcprd_dq_BC.getHist(rpctime_Sector2_BC, "rpctime_Sector2_BC" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpctime_Sector2_BC hist "  );
   
      sc = rpcprd_dq_BC.getHist(rpctime_Sector3_BC, "rpctime_Sector3_BC" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpctime_Sector3_BC hist "  );
    
      sc = rpcprd_dq_BC.getHist(rpctime_Sector4_BC, "rpctime_Sector4_BC" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpctime_Sector4_BC hist "  );
    
      sc = rpcprd_dq_BC.getHist(rpctime_Sector5_BC, "rpctime_Sector5_BC" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpctime_Sector5_BC hist "  );
    
      sc = rpcprd_dq_BC.getHist(rpctime_Sector6_BC, "rpctime_Sector6_BC" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpctime_Sector6_BC hist "  );
    
      sc = rpcprd_dq_BC.getHist(rpctime_Sector7_BC, "rpctime_Sector7_BC" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpctime_Sector7_BC hist "  );
    
      sc = rpcprd_dq_BC.getHist(rpctime_Sector8_BC, "rpctime_Sector8_BC" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpctime_Sector8_BC hist "  );
    
      sc = rpcprd_dq_BC.getHist(rpctime_Sector9_BC, "rpctime_Sector9_BC" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpctime_Sector9_BC hist "  );
    
      sc = rpcprd_dq_BC.getHist(rpctime_Sector10_BC, "rpctime_Sector10_BC" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpctime_Sector10_BC hist "  );
    
      sc = rpcprd_dq_BC.getHist(rpctime_Sector11_BC, "rpctime_Sector11_BC" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpctime_Sector11_BC hist "  );
    
      sc = rpcprd_dq_BC.getHist(rpctime_Sector12_BC, "rpctime_Sector12_BC" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpctime_Sector12_BC hist "  );
    
      sc = rpcprd_dq_BC.getHist(rpctime_Sector13_BC, "rpctime_Sector13_BC" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpctime_Sector13_BC hist "  );
    
      sc = rpcprd_dq_BC.getHist(rpctime_Sector14_BC, "rpctime_Sector14_BC" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpctime_Sector14_BC hist "  );
    
      sc = rpcprd_dq_BC.getHist(rpctime_Sector15_BC, "rpctime_Sector15_BC" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpctime_Sector15_BC hist "  );
    
      sc = rpcprd_dq_BC.getHist(rpctime_Sector16_BC, "rpctime_Sector16_BC" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpctime_Sector16_BC hist "  );
            
      sc = rpcprd_dq_BC.getHist(rpc1DStationNameHitsSideC, "rpc1DStationNameHitsSideC" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpc1DStationNameHitsSideC hist "  );
            
      sc = rpcprd_dq_BC.getHist(rpc1DStationNameTriggerHitsSideC, "rpc1DStationNameTriggerHitsSideC" );
      if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get rpc1DStationNameTriggerHitsSideC hist "  );
    
      // trigger road
    
    
      sc = rpcTrigRoad.getHist( RPC_TriggerRoad, "RPC_TriggerRoad" );
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get RPC_TriggerRoad hist " );
    
      sc = rpcTrigRoad.getHist( RPC_TriggerRoad_Large_Eta, "RPC_TriggerRoad_Large_Eta" );
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get RPC_TriggerRoad_Large_Eta hist " );
    
      sc = rpcTrigRoad.getHist( RPC_TriggerRoad_Large_Phi, "RPC_TriggerRoad_Large_Phi" );
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get RPC_TriggerRoad_LowPt_Large_Phi hist " );
      /*
	sc = rpcTrigRoad.getHist( RPC_TriggerRoad_HighPt_Large_Eta, "RPC_TriggerRoad_HighPt_Large_Eta" );
	if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get RPC_TriggerRoad_HighPt_Large_Eta hist " );
    
	sc = rpcTrigRoad.getHist( RPC_TriggerRoad_HighPt_Large_Phi, "RPC_TriggerRoad_HighPt_Large_Phi" );
	if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get RPC_TriggerRoad_HighPt_Large_Phi hist " );
      */
      sc = rpcTrigRoad.getHist( RPC_TriggerRoad_Small_Eta, "RPC_TriggerRoad_Small_Eta" );
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get RPC_TriggerRoad_Small_Eta hist " );
   
      sc = rpcTrigRoad.getHist( RPC_TriggerRoad_Small_Phi, "RPC_TriggerRoad_Small_Phi" );
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get RPC_TriggerRoad_Small_Phi hist " );
      /*
	sc = rpcTrigRoad.getHist( RPC_TriggerRoad_HighPt_Small_Eta, "RPC_TriggerRoad_HighPt_Small_Eta" );
	if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get RPC_TriggerRoad_HighPt_Small_Eta hist " );
         
	sc = rpcTrigRoad.getHist( RPC_TriggerRoad_HighPt_Small_Phi, "RPC_TriggerRoad_HighPt_Small_Phi" );
	if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get RPC_TriggerRoad_HighPt_Small_Phi hist " );
      */
      // threshold
      sc = rpcTrigRoad.getHist( RPC_Threshold_Eta, "RPC_Threshold_Eta");
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get RPC_Threshold_Eta hist " );
     
      sc = rpcTrigRoad.getHist( RPC_Threshold_Phi, "RPC_Threshold_Phi");
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get RPC_Threshold_Phi hist " );
         
      // lumiblock histos
      if(m_doLumiPlot){
       
       MonGroup rpcTrig_lumi_block ( this, m_generic_path_rpcmonitoring + "/lumiblock", run, ATTRIB_UNMANAGED );
 
       sc = rpcTrig_lumi_block.getHist( rpcTriggerHitsPerEvents_Eta_LowPt, "rpcTriggerHitsPerEvents_Eta_LowPt");
       if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get rpcTriggerHitsPerEvents_Eta_LowPt hist " ); 
     
       sc = rpcTrig_lumi_block.getHist( rpcTriggerHitsPerEvents_Phi_LowPt, "rpcTriggerHitsPerEvents_Phi_LowPt");
       if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get rpcTriggerHitsPerEvents_Phi_LowPt hist " );    
        
       sc = rpcTrig_lumi_block.getHist( rpcTriggerHitsPerEvents_Eta_HighPt, "rpcTriggerHitsPerEvents_Eta_HighPt");
       if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get rpcTriggerHitsPerEvents_Eta_HighPt hist " ); 
 	  
       sc = rpcTrig_lumi_block.getHist( rpcTriggerHitsPerEvents_Phi_HighPt, "rpcTriggerHitsPerEvents_Phi_HighPt");
       if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get rpcTriggerHitsPerEvents_Phi_HighPt hist " );  
      }   
 
 
      for (containerIt = rpc_container->begin() ; containerIt != rpc_container->end() ; ++containerIt) 
	{ 
	 
	  for (Muon::RpcPrepDataCollection::const_iterator rpcCollection = (*containerIt)->begin(); rpcCollection!=(*containerIt)->end(); ++rpcCollection)
	    {
	      if (m_nPrd<maxPRD) {
		Identifier prdcoll_id = (*rpcCollection)->identify();
		irpcstationPhi     =   int(m_rpcIdHelper->stationPhi(prdcoll_id))  ;	      
		irpcstationName    =   int(m_rpcIdHelper->stationName(prdcoll_id)) ;	      
		irpcstationEta     =   int(m_rpcIdHelper->stationEta(prdcoll_id))  ;		      
		irpcdoubletR       =   int(m_rpcIdHelper->doubletR(prdcoll_id))	 ;
		irpcdoubletZ       =   int(m_rpcIdHelper->doubletZ(prdcoll_id))	 ;
		irpcdoubletPhi	 =   int(m_rpcIdHelper->doubletPhi(prdcoll_id))  ;
		irpcgasGap  	 =   int(m_rpcIdHelper->gasGap(prdcoll_id))	 ;
		irpcmeasuresPhi	 =   int(m_rpcIdHelper->measuresPhi(prdcoll_id)) ;
		irpcstrip		 =   int(m_rpcIdHelper->strip(prdcoll_id))	 ;
            
		irpctime		 =   double((*rpcCollection)->time())	         ;		 
		irpctriggerInfo	 =   int((*rpcCollection)->triggerInfo   ())     ; // double		   
		irpcambiguityFlag	 =   double((*rpcCollection)->ambiguityFlag ())  ;		 
		// irpcthreshold	 =   double((*rpcCollection)->threshold ())  ;		 
		
		// std::cout << "irpcthreshold rpcCollection   " << irpcthreshold <<  "\n";		  
		// m_threshold: internal threshold 
		const MuonGM::RpcReadoutElement* descriptor_Atl = m_muonMgr->getRpcReadoutElement( prdcoll_id );
		x_atl = descriptor_Atl ->stripPos(prdcoll_id ).x() ;
		y_atl = descriptor_Atl ->stripPos(prdcoll_id ).y() ;
		z_atl = descriptor_Atl ->stripPos(prdcoll_id ).z() ;
	    		  
		//get chamber hardware name
		hardware_name=convertChamberName(irpcstationName,irpcstationEta,irpcstationPhi,type) ;
            
	    		  
		//get information from geomodel to book and fill rpc histos with the right max strip number
		std::vector<int>   rpcstripshift = RpcGM::RpcStripShift(m_muonMgr,m_rpcIdHelper, prdcoll_id, irpctriggerInfo)  ;
		
		 
		
		
		NphiStrips         =  rpcstripshift[0] ;
		ShiftPhiStrips     =  rpcstripshift[1] ;
		NetaStrips	       =  rpcstripshift[2] ;
		ShiftEtaStrips     =  rpcstripshift[3] ;
		ShiftStrips	       =  rpcstripshift[4] ;
		NetaStripsTot      =  rpcstripshift[5] ;
		NetaStripsTotSideA =  rpcstripshift[6] ;
		NetaStripsTotSideC =  rpcstripshift[7] ;
		ShiftEtaStripsTot  =  rpcstripshift[8] ;
		Nbin               =  rpcstripshift[9] ;
		EtaStripSign       =  rpcstripshift[10];
		PanelIndex         =  rpcstripshift[13];
		Settore            =  rpcstripshift[14];
		PlaneTipo          =  rpcstripshift[15];
		strip_dbindex      =  rpcstripshift[16];
		NetaPanelsTot	 =  rpcstripshift[19];  
		ShiftEtaPanelsTot  =  rpcstripshift[20];
		NetaPanelsTotSideA =  rpcstripshift[21];
		NetaPanelsTotSideC =  rpcstripshift[22];
		rpcpanel_dbindex   =  rpcstripshift[23];
		rpctower_dbindex   =  rpcstripshift[24];
		shiftstripphiatlas =  rpcstripshift[25];
 
		//get name for titles and labels 
		std::vector<std::string>   rpclayersectorsidename = RpcGM::RpcLayerSectorSideName(m_rpcIdHelper,prdcoll_id, irpctriggerInfo)  ;  
		layer_name               = rpclayersectorsidename[0] ;
		layertodraw1_name        = rpclayersectorsidename[1] ;
		layertodraw2_name        = rpclayersectorsidename[2] ;
		layervslayer_name        = rpclayersectorsidename[3] ;
		layer0_name	             = rpclayersectorsidename[4] ;
		layer1_name	             = rpclayersectorsidename[5] ;
		layer2_name	             = rpclayersectorsidename[6] ;
		layerPhivsEta_name       = rpclayersectorsidename[7] ;
		layerPhivsEtaSector_name = rpclayersectorsidename[8] ;
		sector_name              = rpclayersectorsidename[9] ;
		layeronly_name           = rpclayersectorsidename[10];
		layer_name_panel         = rpclayersectorsidename[11];
		sector_dphi_layer        = rpclayersectorsidename[12];
	      
	    

            
		// fill general histograms
	      
		rpc2DEtaStation->Fill(irpcstationEta, Settore-1 + PlaneTipo*16);
		if(irpcgasGap==1) rpc2DEtaStationGap1	->Fill(irpcstationEta, Settore-1 + PlaneTipo*16 	);
		if(irpcgasGap==2) rpc2DEtaStationGap2	->Fill(irpcstationEta, Settore-1 + PlaneTipo*16 	);
		
		int NPanel_sign = ShiftEtaPanelsTot ;
		if(irpcstationEta<0)NPanel_sign=-NPanel_sign ;
		
		if(PlaneTipo<2  ) GlobalHitsPerRPCMiddle->Fill(NPanel_sign , Settore-1+(irpcdoubletPhi-1)*0.5 ); 
		if(PlaneTipo==2 ) GlobalHitsPerRPCOuter ->Fill(NPanel_sign , Settore-1+(irpcdoubletPhi-1)*0.5 );
	      

		//dq histo from Mauro 
		//enum_Eta(Phi)_LowPt0_BA(BC)
		if(PlaneTipo==0&&irpcgasGap==1){
		  if(irpcmeasuresPhi==0){	 
		    if(EtaStripSign>0){
		      rpc2DPanelHits[enum_Eta_LowPt0_BA]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Eta_LowPt0_BA]->Fill(lumiblock, float(rpcpanel_dbindex) + float(irpcdoubletPhi-1)*0.5-1     );
		    }
		    else{
		      rpc2DPanelHits[enum_Eta_LowPt0_BC]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1); 
		      rpc1DvsLBPanelHits[enum_Eta_LowPt0_BC]->Fill(lumiblock, float(rpcpanel_dbindex) + float(irpcdoubletPhi-1)*0.5-1    );
		    }
		  }
		  else{	 
		    if(EtaStripSign>0){
		      rpc2DPanelHits[enum_Phi_LowPt0_BA]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Phi_LowPt0_BA]->Fill(lumiblock, float(rpcpanel_dbindex) + float(irpcdoubletPhi-1)*0.5-1    );
		    }
		    else{
		      rpc2DPanelHits[enum_Phi_LowPt0_BC]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Phi_LowPt0_BC]->Fill(lumiblock, float(rpcpanel_dbindex) + float(irpcdoubletPhi-1)*0.5-1    );
		    }
		  }
		}
		//enum_Eta(Phi)_LowPt1_BA(BC)
		if(PlaneTipo==0&&irpcgasGap==2){
		  if(irpcmeasuresPhi==0){	 
		    if(EtaStripSign>0){
		      rpc2DPanelHits[enum_Eta_LowPt1_BA]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Eta_LowPt1_BA]->Fill(lumiblock, float(rpcpanel_dbindex) + float(irpcdoubletPhi-1)*0.5-1    );
		    }
		    else{
		      rpc2DPanelHits[enum_Eta_LowPt1_BC]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Eta_LowPt1_BC]->Fill(lumiblock, float(rpcpanel_dbindex) + float(irpcdoubletPhi-1)*0.5-1    );
		    }
		  }
		  else{	 
		    if(EtaStripSign>0){
		      rpc2DPanelHits[enum_Phi_LowPt1_BA]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Phi_LowPt1_BA]->Fill(lumiblock, float(rpcpanel_dbindex) + float(irpcdoubletPhi-1)*0.5-1  );
		    }
		    else{
		      rpc2DPanelHits[enum_Phi_LowPt1_BC]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Phi_LowPt1_BC]->Fill(lumiblock, float(rpcpanel_dbindex) + float(irpcdoubletPhi-1)*0.5-1  );
		    }
		  }
		}
		//enum_Eta(Phi)_Pivot0_BA(BC)
		if(PlaneTipo==1&&irpcgasGap==1){
		  if(irpcmeasuresPhi==0){	 
		    if(EtaStripSign>0){
		      rpc2DPanelHits[enum_Eta_Pivot0_BA]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Eta_Pivot0_BA]->Fill(lumiblock, float(rpcpanel_dbindex) + float(irpcdoubletPhi-1)*0.5-1    );
		    }
		    else{
		      rpc2DPanelHits[enum_Eta_Pivot0_BC]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Eta_Pivot0_BC]->Fill(lumiblock, float(rpcpanel_dbindex) + float(irpcdoubletPhi-1)*0.5-1    );
		    }
		  }
		  else{	 
		    if(EtaStripSign>0){
		      rpc2DPanelHits[enum_Phi_Pivot0_BA]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Phi_Pivot0_BA]->Fill(lumiblock, float(rpcpanel_dbindex) + float(irpcdoubletPhi-1)*0.5-1    );
		    }
		    else{
		      rpc2DPanelHits[enum_Phi_Pivot0_BC]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Phi_Pivot0_BC]->Fill(lumiblock, float(rpcpanel_dbindex) + float(irpcdoubletPhi-1)*0.5-1    );
		    }
		  }
		}
		//enum_Eta(Phi)_Pivot1_BA(BC)
		if(PlaneTipo==1&&irpcgasGap==2){
		  if(irpcmeasuresPhi==0){	 
		    if(EtaStripSign>0){
		      rpc2DPanelHits[enum_Eta_Pivot1_BA]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Eta_Pivot1_BA]->Fill(lumiblock, float(rpcpanel_dbindex) + float(irpcdoubletPhi-1)*0.5-1    );
		    }
		    else{
		      rpc2DPanelHits[enum_Eta_Pivot1_BC]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Eta_Pivot1_BC]->Fill(lumiblock, float(rpcpanel_dbindex) + float(irpcdoubletPhi-1)*0.5-1    );
		    }
		  }
		  else{	 
		    if(EtaStripSign>0){
		      rpc2DPanelHits[enum_Phi_Pivot1_BA]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Phi_Pivot1_BA]->Fill(lumiblock, float(rpcpanel_dbindex) + float(irpcdoubletPhi-1)*0.5-1    );
		    }
		    else{
		      rpc2DPanelHits[enum_Phi_Pivot1_BC]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Phi_Pivot1_BC]->Fill(lumiblock, float(rpcpanel_dbindex) + float(irpcdoubletPhi-1)*0.5-1    );
		    }
		  }
		}
		//enum_Eta(Phi)_HighPt0_BA(BC)
		if(PlaneTipo==2&&irpcgasGap==1){
		  if(irpcmeasuresPhi==0){	 
		    if(EtaStripSign>0){
		      rpc2DPanelHits[enum_Eta_HighPt0_BA]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Eta_HighPt0_BA]->Fill(lumiblock, float(rpcpanel_dbindex) + float(irpcdoubletPhi-1)*0.5-1    );
		    }
		    else{
		      rpc2DPanelHits[enum_Eta_HighPt0_BC]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Eta_HighPt0_BC]->Fill(lumiblock, float(rpcpanel_dbindex) + float(irpcdoubletPhi-1)*0.5-1    );
		    }
		  }
		  else{	 
		    if(EtaStripSign>0){
		      rpc2DPanelHits[enum_Phi_HighPt0_BA]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Phi_HighPt0_BA]->Fill(lumiblock, float(rpcpanel_dbindex) + float(irpcdoubletPhi-1)*0.5-1    );
		    }
		    else{
		      rpc2DPanelHits[enum_Phi_HighPt0_BC]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Phi_HighPt0_BC]->Fill(lumiblock, float(rpcpanel_dbindex) + float(irpcdoubletPhi-1)*0.5-1    );
		    }
		  }
		}
		//enum_Eta(Phi)_HighPt1_BA(BC)
		if(PlaneTipo==2&&irpcgasGap==2){
		  if(irpcmeasuresPhi==0){	 
		    if(EtaStripSign>0){
		      rpc2DPanelHits[enum_Eta_HighPt1_BA]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Eta_HighPt1_BA]->Fill(lumiblock, float(rpcpanel_dbindex) + float(irpcdoubletPhi-1)*0.5-1    );
		    }
		    else{
		      rpc2DPanelHits[enum_Eta_HighPt1_BC]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Eta_HighPt1_BC]->Fill(lumiblock, float(rpcpanel_dbindex) + float(irpcdoubletPhi-1)*0.5-1    );
		    }
		  }
		  else{	 
		    if(EtaStripSign>0){
		      rpc2DPanelHits[enum_Phi_HighPt1_BA]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Phi_HighPt1_BA]->Fill(lumiblock, float(rpcpanel_dbindex) + float(irpcdoubletPhi-1)*0.5-1    );
		    }
		    else{
		      rpc2DPanelHits[enum_Phi_HighPt1_BC]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Phi_HighPt1_BC]->Fill(lumiblock, float(rpcpanel_dbindex) + float(irpcdoubletPhi-1)*0.5-1    );
		    }
		  }
		}
                

		///////SHIFT 1D HISTOS
		 
	        if(EtaStripSign>=0){
		   rpc1DStationNameHitsSideA       ->Fill(StationNameViewIndex[irpcstationName][irpcmeasuresPhi]  ,1./StationNameSectorSize [irpcstationName]*AverageLuminosityWeight);
		} 
	        else{
		   rpc1DStationNameHitsSideC       ->Fill(StationNameViewIndex[irpcstationName][irpcmeasuresPhi]  ,1./StationNameSectorSize [irpcstationName]*AverageLuminosityWeight);
		}
		  
	      
	            
		//fill eta phi view time histo distribution 
		if(irpcmeasuresPhi==0){	 
		  rpcEtaTime->Fill(irpctime);  
		}
		else{ rpcPhiTime->Fill(irpctime); }
		  
		// shift time distribution histogram 
		rpctime -> Fill(irpctime);

		if (irpcstationEta>=0) { rpctime_BA->Fill(irpctime); }
		else		     { rpctime_BC->Fill(irpctime); }


	      
		if (irpcstationEta<0&&Settore==1 ){ rpctime_Sector1_BC->Fill(irpctime); }
		if (irpcstationEta<0&&Settore==2 ){ rpctime_Sector2_BC->Fill(irpctime); }
		if (irpcstationEta<0&&Settore==3 ){ rpctime_Sector3_BC->Fill(irpctime); }
		if (irpcstationEta<0&&Settore==4 ){ rpctime_Sector4_BC->Fill(irpctime); }
		if (irpcstationEta<0&&Settore==5 ){ rpctime_Sector5_BC->Fill(irpctime); }
		if (irpcstationEta<0&&Settore==6 ){ rpctime_Sector6_BC->Fill(irpctime); }
		if (irpcstationEta<0&&Settore==7 ){ rpctime_Sector7_BC->Fill(irpctime); }
		if (irpcstationEta<0&&Settore==8 ){ rpctime_Sector8_BC->Fill(irpctime); }
		if (irpcstationEta<0&&Settore==9 ){ rpctime_Sector9_BC->Fill(irpctime); }
		if (irpcstationEta<0&&Settore==10){ rpctime_Sector10_BC->Fill(irpctime); }
		if (irpcstationEta<0&&Settore==11){ rpctime_Sector11_BC->Fill(irpctime); }
		if (irpcstationEta<0&&Settore==12){ rpctime_Sector12_BC->Fill(irpctime); }
		if (irpcstationEta<0&&Settore==13){ rpctime_Sector13_BC->Fill(irpctime); }
		if (irpcstationEta<0&&Settore==14){ rpctime_Sector14_BC->Fill(irpctime); }
		if (irpcstationEta<0&&Settore==15){ rpctime_Sector15_BC->Fill(irpctime); }
		if (irpcstationEta<0&&Settore==16){ rpctime_Sector16_BC->Fill(irpctime); }
	      
	      
		if (irpcstationEta>=0&&Settore==1 ){ rpctime_Sector1_BA->Fill(irpctime); }
		if (irpcstationEta>=0&&Settore==2 ){ rpctime_Sector2_BA->Fill(irpctime); }
		if (irpcstationEta>=0&&Settore==3 ){ rpctime_Sector3_BA->Fill(irpctime); }
		if (irpcstationEta>=0&&Settore==4 ){ rpctime_Sector4_BA->Fill(irpctime); }
		if (irpcstationEta>=0&&Settore==5 ){ rpctime_Sector5_BA->Fill(irpctime); }
		if (irpcstationEta>=0&&Settore==6 ){ rpctime_Sector6_BA->Fill(irpctime); }
		if (irpcstationEta>=0&&Settore==7 ){ rpctime_Sector7_BA->Fill(irpctime); }
		if (irpcstationEta>=0&&Settore==8 ){ rpctime_Sector8_BA->Fill(irpctime); }
		if (irpcstationEta>=0&&Settore==9 ){ rpctime_Sector9_BA->Fill(irpctime); }
		if (irpcstationEta>=0&&Settore==10){ rpctime_Sector10_BA->Fill(irpctime); }
		if (irpcstationEta>=0&&Settore==11){ rpctime_Sector11_BA->Fill(irpctime); }
		if (irpcstationEta>=0&&Settore==12){ rpctime_Sector12_BA->Fill(irpctime); }
		if (irpcstationEta>=0&&Settore==13){ rpctime_Sector13_BA->Fill(irpctime); }
		if (irpcstationEta>=0&&Settore==14){ rpctime_Sector14_BA->Fill(irpctime); }
		if (irpcstationEta>=0&&Settore==15){ rpctime_Sector15_BA->Fill(irpctime); }
		if (irpcstationEta>=0&&Settore==16){ rpctime_Sector16_BA->Fill(irpctime); } 


	   
		// cool strip profile
		if ( m_doCoolDB ) {
		  sc = rpcCoolDb.getHist( rpcCool_StripProfile, sector_dphi_layer+"_Profile" ) ;
		  if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get " << sector_dphi_layer << "_Profile hist" );
  
		  if ( rpcCool_StripProfile ) {
		    if ( irpcmeasuresPhi==0 ) {
		      rpcCool_StripProfile->Fill( strip_dbindex );
		    }
		    else {
		      if ( irpcambiguityFlag==1 ) {
			rpcCool_StripProfile->Fill( strip_dbindex );
			// removed logical-OR and wired-OR of RPC Phi strips
		      }
		    }
		  }
		}
	      
		if(m_rpcchamberhist){
		
		  ATH_MSG_DEBUG (  "RPC Hardware chamber name in the selected area : " << hardware_name );	
 
		  histo_flag=true;
		  for (std::vector<std::string>::const_iterator iter=layer_name_list.begin(); iter!=layer_name_list.end(); iter++){
		    if ( (hardware_name+layer_name)==*iter){histo_flag=false;}
		  }
		  if (histo_flag){ 
		    layer_name_list.push_back(hardware_name+layer_name); 
		    bookRPCLayerHistograms(hardware_name, layer_name, layer0_name, Nbin, 0 , Nbin);
		    bookRPCLayervsTimeHistograms(hardware_name, layer_name, Nbin, 0 , Nbin);
		    if(irpcmeasuresPhi==1)bookRPCLayerPhiAmbiHistograms(hardware_name, layer_name, layer0_name, Nbin, 0 , Nbin);
		  }
	      
		  histo_flag=true ;
		  for (std::vector<std::string>::const_iterator iter=layer_name_list_panel.begin(); iter!=layer_name_list_panel.end(); iter++){
		    if ( (hardware_name+layer_name_panel)==*iter){histo_flag=false;}
		  }
		  if (histo_flag){ 
		    layer_name_list_panel.push_back(hardware_name+layer_name_panel)  ; 
		    layer_name_bin_list_panel.push_back( PanelIndex )                ; 
		    bookRPCLayerHistogramsPanel(hardware_name, layer_name_panel)     ; 
		  }
		
		  const MuonDQAHistList& hists1 = m_stationHists.getList( hardware_name + "/Profiles/" + layer_name);       	  	  	  	  	
		  TH1* rpcstriplayer = hists1.getH1( hardware_name + "_" + layer_name + "_strip");
		  
		  const MuonDQAHistList& hists2 = m_stationHists.getList( hardware_name + "/ProfilesvsTime/" + layer_name);       	  	  	  	  	
		  TH2* rpcstripvstimelayer = hists2.getH2( hardware_name + "_" + layer_name + "_stripvstime");
		  
		  const MuonDQAHistList& hists3 = m_stationHists.getList( hardware_name + "/Phi_profiles_ambiguity_resolved/" + layer_name);
		  TH1* rpcstripPhiAmbilayer = hists3.getH1(hardware_name + "_" + layer_name +"_Ambiguity_resolved_phi_strip");
		    
		  if (rpcstriplayer) {rpcstriplayer->Fill( float(irpcstrip  + ShiftStrips)  -0.5 );}
		  else {ATH_MSG_DEBUG (  "rpcstriplayer not in hist list!" );}
            
		  if (rpcstripvstimelayer) {rpcstripvstimelayer->Fill( float(irpcstrip  + ShiftStrips)  -0.5 , irpctime );}
		  else {ATH_MSG_DEBUG (  "rpcstripvstimelayer not in hist list!" );}

		  if(irpcmeasuresPhi==1&&irpcambiguityFlag==1){
		    if (rpcstripPhiAmbilayer) {rpcstripPhiAmbilayer->Fill( float(irpcstrip  + ShiftStrips)  -0.5 );}
		    else {ATH_MSG_DEBUG (  "rpcstripPhiAmbilayer not in hist list!" );}  
		  }
                                        
      
		}//end if on m_rpcchamberhist || ESD
		else{
		  histo_flag=true ;
		  for (std::vector<std::string>::const_iterator iter=layer_name_list_panel.begin(); iter!=layer_name_list_panel.end(); iter++){
		    if ( (hardware_name+layer_name_panel)==*iter){histo_flag=false;}
		  }
		  if (histo_flag){ 
		    layer_name_list_panel.push_back(hardware_name+layer_name_panel)  ; 
		    layer_name_bin_list_panel.push_back( PanelIndex )                ; 
		  }
		}
	
		////////////// Start Loop on the second prd ///////////////////////////////
		for (Muon::RpcPrepDataCollection::const_iterator rpcCollectionII=(*containerIt)->begin();
		     rpcCollectionII!=(*containerIt)->end(); ++rpcCollectionII)	  	   	 	   
		  { 
		    Identifier prdcoll_id_II = (*rpcCollectionII)->identify(); 
		  
		    irpcstationPhiII       =   int(m_rpcIdHelper->stationPhi(prdcoll_id_II))  ;		
		    irpcstationNameII      =   int(m_rpcIdHelper->stationName(prdcoll_id_II)) ;		
		    irpcstationEtaII       =   int(m_rpcIdHelper->stationEta(prdcoll_id_II))  ; 			
		    irpcdoubletRII         =   int(m_rpcIdHelper->doubletR(prdcoll_id_II))    ;	  	
		    irpcdoubletZII         =   int(m_rpcIdHelper->doubletZ(prdcoll_id_II))    ;
		    irpcdoubletPhiII       =   int(m_rpcIdHelper->doubletPhi(prdcoll_id_II))  ;
		    irpcgasGapII           =   int(m_rpcIdHelper->gasGap(prdcoll_id_II))      ;
		    irpcmeasuresPhiII      =   int(m_rpcIdHelper->measuresPhi(prdcoll_id_II)) ;
		    irpcstripII            =   int(m_rpcIdHelper->strip(prdcoll_id_II))       ;  		  
		    irpctimeII             =   double((*rpcCollectionII)->time())             ;		               		  
		    irpctriggerInfoII      =   int((*rpcCollectionII)->triggerInfo() )        ; // double		
		    irpcambiguityFlagII    =   double((*rpcCollectionII)->ambiguityFlag ())   ;
		
		  
		    const MuonGM::RpcReadoutElement* descriptor_Atl_II = m_muonMgr->getRpcReadoutElement( prdcoll_id_II );
		    x_atl_II = descriptor_Atl_II ->stripPos(prdcoll_id_II ).x() ;
		    y_atl_II = descriptor_Atl_II ->stripPos(prdcoll_id_II ).y() ;
		    z_atl_II = descriptor_Atl_II ->stripPos(prdcoll_id_II ).z() ;
		  
		    //get information from geomodel to book and fill rpc histos with the right max strip number
		    std::vector<int>   rpcstripshiftII = RpcGM::RpcStripShift(m_muonMgr,m_rpcIdHelper, prdcoll_id, irpctriggerInfo)  ;
		    NbinII               =  rpcstripshiftII[9];
		    
		    if(irpcmeasuresPhi==1&&irpcmeasuresPhiII==0){
		      if(irpcstationPhi==irpcstationPhiII&&irpcstationName==irpcstationNameII&&irpcstationEta==irpcstationEtaII&&
			 irpcdoubletR==irpcdoubletRII&&irpcdoubletZ==irpcdoubletZII&&irpcdoubletPhi==irpcdoubletPhiII&&irpcgasGap==irpcgasGapII){
			      
			if(m_rpcchamberhist ){   
			  //if(1 == 0){
			  histo_flag=true;
			  for (std::vector<std::string>::const_iterator iter=layerPhivsEta_name_list.begin(); iter!=layerPhivsEta_name_list.end(); iter++){
			    if ( (hardware_name+layerPhivsEta_name)==*iter){histo_flag=false;}
			  }
			  if (histo_flag){ 
			    layerPhivsEta_name_list.push_back(hardware_name+layerPhivsEta_name); 
			    bookRPCLayerPhivsEtaHistograms(hardware_name, layerPhivsEta_name, NetaStrips, 0 , NetaStrips, NphiStrips, 0 , NphiStrips);
			  }	              
			  const MuonDQAHistList& hists4 = m_stationHists.getList( hardware_name + "/PhivsEta/" + layerPhivsEta_name);       	  	  	  	  	
			  TH2* rpcstriplayerPhivsEta = hists4.getH2(hardware_name + "_" + layerPhivsEta_name );    
			  if (rpcstriplayerPhivsEta) {rpcstriplayerPhivsEta->Fill( float(irpcstripII + ShiftEtaStrips)  -0.5,  float(irpcstrip + ShiftPhiStrips)  -0.5 );}
			  else {ATH_MSG_DEBUG (  "rpcstriplayerPhivsEta not in hist list!" );}
			}//end if on m_rpcchamberhist or ESD	      
		     
			//Sector  
			stripetaatlas  =  ( irpcstripII + ShiftEtaStripsTot )*EtaStripSign ;	     
			if ( stripetaatlas>0 )  stripetaatlas-- ;  
			stripphisector =   irpcstrip + ShiftPhiStrips                      ;
		          
			if(m_rpcsectorhist){ 	      
			  histo_flag=true;
			  for (std::vector<std::string>::const_iterator iter=layerPhivsEtaSector_name_list.begin(); iter!=layerPhivsEtaSector_name_list.end(); iter++){
			    if ( (sector_name+layerPhivsEtaSector_name)==*iter){histo_flag=false;}
			  }
			  if (histo_flag){ 
			    layerPhivsEtaSector_name_list.push_back(sector_name+layerPhivsEtaSector_name); 
			    bookRPCLayerPhivsEtaSectorHistograms(sector_name, layerPhivsEtaSector_name, NetaStripsTot, -NetaStripsTotSideC, NetaStripsTotSideA, NphiStrips, 0 , NphiStrips);
			  }
			  const MuonDQAHistList& hists5 = m_stationHists.getList( sector_name + "/PhivsEta/" + layerPhivsEtaSector_name);
			  TH2* rpcstriplayerPhivsEtaSector = hists5.getH2(layerPhivsEtaSector_name ); 
			  if (rpcstriplayerPhivsEtaSector) {
		     	  
			    rpcstriplayerPhivsEtaSector->Fill( stripetaatlas, stripphisector-1 );	
		    
			  }
			  else {ATH_MSG_DEBUG (  "rpcstriplayerPhivsEtaSector not in hist list!" );}
			}
		      
			stripphiatlas = stripphisector + shiftstripphiatlas ;
			
			x_atlas = x_atl     ;
			y_atlas = y_atl     ;
			z_atlas = z_atl_II  ;
			
			if ( x_atlas > 0 ) { 
			  phi_atlas = atan ( y_atlas / x_atlas ); 
			}
			else if ( x_atlas == 0 ){ 
			  if (y_atlas > 0) { 
			    phi_atlas = CLHEP::pi/2 ;
			  }
			  else 
			    { 
			      phi_atlas = -CLHEP::pi/2 ;
			    } 
			}
			else{
			  if (y_atlas > 0) { 
			    phi_atlas = atan ( y_atlas / x_atlas ) + CLHEP::pi ; 
			  }
			  else 
			    { 
			      phi_atlas = -CLHEP::pi + atan ( y_atlas / x_atlas ) ;
			    }  
			 
			}
			
			// pseudorapidity
			if ( z_atlas!=0  ) {
			  eta_atlas = -log( abs( tan( 0.5 * atan(sqrt(pow(x_atlas,2.)+pow(y_atlas,2.))/ z_atlas )) ));
		        }
			else{
			  eta_atlas = 0 ;
		        }
			if ( irpcstationEta<0 ) { eta_atlas = -eta_atlas; }
			
		                            
			if(layeronly_name=="Pivot0" ) {
			  rpcPhivsEtaAtlasPivot0        ->Fill( stripetaatlas , stripphiatlas -1 );
			  rpcPhivsEtaAtlasPivot0_PhivsZ ->Fill( z_atlas, phi_atlas )           ;
			  // coord cilindriche: 1->phi, 2->z
			  //rpcPhivsEtaAtlasPivot0_PhivsZ ->Fill( phi_atlas, z_atlas ); 
			}
			if(layeronly_name=="Pivot1" ) { 
			  rpcPhivsEtaAtlasPivot1        ->Fill( stripetaatlas , stripphiatlas -1 );	
			  rpcPhivsEtaAtlasPivot1_PhivsZ ->Fill( z_atlas, phi_atlas )           ;   
			}
			if(layeronly_name=="LowPt0"  	         ) { 
			  rpcPhivsEtaAtlasLowPt0        ->Fill( stripetaatlas , stripphiatlas -1 );
			  rpcPhivsEtaAtlasLowPt0_PhivsZ ->Fill( z_atlas, phi_atlas )           ;
			}
			if(layeronly_name=="LowPt1"  	         ) { 
			  rpcPhivsEtaAtlasLowPt1        ->Fill( stripetaatlas , stripphiatlas -1 );
			  rpcPhivsEtaAtlasLowPt1_PhivsZ ->Fill( z_atlas, phi_atlas )           ;
			}
			if(layeronly_name=="HighPt0" 	         ) { 
			  rpcPhivsEtaAtlasHighPt0        ->Fill( stripetaatlas , stripphiatlas -1 );
			  rpcPhivsEtaAtlasHighPt0_PhivsZ ->Fill( z_atlas, phi_atlas )           ;
			}
			if(layeronly_name=="HighPt1" 	         ) { 
			  rpcPhivsEtaAtlasHighPt1        ->Fill( stripetaatlas , stripphiatlas -1 );	    
			  rpcPhivsEtaAtlasHighPt1_PhivsZ ->Fill( z_atlas, phi_atlas )           ;
			}
			if(layeronly_name=="LowPt_TriggerOut"       ){ 
		
			  rpcPhivsEtaAtlasLowPt_TriggerOut        ->Fill( stripetaatlas , stripphiatlas-1 );
			  rpcPhivsEtaAtlasLowPt_TriggerOut_PhivsZ ->Fill( z_atlas, phi_atlas )           ;
			}
			if(layeronly_name=="HighPt_TriggerFromLowPt") { 
			  rpcPhivsEtaAtlasHighPt_TriggerFromLowPt        ->Fill( stripetaatlas , stripphiatlas-1 );
			  rpcPhivsEtaAtlasHighPt_TriggerFromLowPt_PhivsZ ->Fill( z_atlas, phi_atlas )           ;
			}
			if(layeronly_name=="HighPt_TriggerOut"      ) { 
			  rpcPhivsEtaAtlasHighPt_TriggerOut        ->Fill( stripetaatlas , stripphiatlas-1 );
			  rpcPhivsEtaAtlasHighPt_TriggerOut_PhivsZ ->Fill( z_atlas, phi_atlas )           ;
			}
		    
		  
		  		  
		      }//same gasgap

	    	      
		    }//phi and eta
		     
		      	 	      
		    //////////same chamber, plane, phi-phi, eta-eta
		    if(m_rpcchamberhist ){ 
		      //if(1 == 0){     
		      if(irpcmeasuresPhi==irpcmeasuresPhiII){
		    		  
			if(  irpcgasGap==(irpcgasGapII+1)      && irpcstationName==irpcstationNameII  ) {
			  if(irpcstationPhi==irpcstationPhiII&&irpcstationEta==irpcstationEtaII&&
			     irpcdoubletR==irpcdoubletRII&&irpcdoubletZ==irpcdoubletZII&&irpcdoubletPhi==irpcdoubletPhiII){
		    		  
			    histo_flag=true;
			    for (std::vector<std::string>::const_iterator iter=layervslayer_name_list.begin(); iter!=layervslayer_name_list.end(); iter++){
			      if ( (hardware_name+layervslayer_name)==*iter){histo_flag=false;}
			    }
			    if (histo_flag){ 
			      layervslayer_name_list.push_back(hardware_name+layervslayer_name); 
			      bookRPCLayervsLayerHistograms(hardware_name, layervslayer_name, layer1_name, layer2_name, Nbin, 0 , Nbin, Nbin, 0, Nbin);
			    }

			    const MuonDQAHistList& hists6 = m_stationHists.getList( hardware_name +"/Layer2vsLayer1/"+layervslayer_name);  
			    TH2* rpcstriplayervslayer = hists6.getH2(hardware_name + "_" + layervslayer_name );  
	 		   
			    if (rpcstriplayervslayer) {rpcstriplayervslayer->Fill( irpcstripII + ShiftStrips , irpcstrip + ShiftStrips );}
			    else {ATH_MSG_DEBUG (  "rpcstriplayervslayer not in hist list!" );}
		  
			  }//same chamber
			}//same plane
		      }//phi-phi or eta-eta		  
		    }//end if on m_rpcchamberhist or ESD  

		  }     ////////////// End Loop on the second prd
	      
		 
		  ++m_nPrd;
		  if (irpcstationEta>=0){ ++m_nPrd_BA ; } 
		  else                  { ++m_nPrd_BC ; }
		 
	
		ATH_MSG_DEBUG (  " RPC PrepRawData has" << m_nPrd <<  "PRD number " );
		map<string,int>::iterator iter_hitsperchamber = hitsperchamber_map.find(hardware_name);
		if ( iter_hitsperchamber  == hitsperchamber_map.end() ){ 
		  hitsperchamber_map.insert( make_pair( hardware_name,1 ) );  
		} else {iter_hitsperchamber->second+=1;}	
		//}//chamber name selection
	      }
	
	      else {
		
		// check if index not out of range  
		if (m_first == true) {
		  ATH_MSG_DEBUG (  "More than " << maxPRD << " RPC PrepRawData" );
		  m_first = false;
		  ATH_MSG_VERBOSE(  "More than " << maxPRD << " RPC PrepRawData" );
		}
 		
	      }
		
	    }/// if (containerIt size)    
	}/// for chambers iterator 
    
      ++rpc_eventstotal;
    
      rpcevents->Fill(m_nPrd);    
    
      TotalNumber_of_RPC_hits_per_events_BA->Fill(m_nPrd_BA) ;
      TotalNumber_of_RPC_hits_per_events_BC->Fill(m_nPrd_BC) ;
  
  
      // loop on trigger hits
     if(m_doTriggerHits){  
         
      NTrigger_Eta_LowPt	= 0	 ;  
      NTrigger_Phi_LowPt	= 0	 ;  
      NTrigger_Eta_HighPt = 0 	 ;  
      NTrigger_Phi_HighPt = 0      ;
    
      const Muon::RpcCoinDataContainer* rpc_trigcontainer;
    
      sc = (*m_activeStore)->retrieve(rpc_trigcontainer, m_key_trig );
      if (sc.isFailure()) {
	ATH_MSG_ERROR (  " Cannot retrieve RPC trigger Hits container " << m_key_trig );
	return sc;
      }
      Muon::RpcCoinDataContainer::const_iterator trigcontainerIt;
      for ( trigcontainerIt = rpc_trigcontainer->begin(); trigcontainerIt != rpc_trigcontainer->end(); ++trigcontainerIt ) 
	{
	  for ( Muon::RpcCoinDataCollection::const_iterator rpcCoinCollection = (*trigcontainerIt)->begin();
		rpcCoinCollection!=(*trigcontainerIt)->end();
		++rpcCoinCollection )
	    {
	      if ( m_nTrig < maxPRD ) {
		Identifier prdcoll_id = (*rpcCoinCollection)->identify();
		irpcstationPhi     =   int(m_rpcIdHelper->stationPhi(prdcoll_id))  ;  	  
		irpcstationName    =   int(m_rpcIdHelper->stationName(prdcoll_id)) ;  	  
		irpcstationEta     =   int(m_rpcIdHelper->stationEta(prdcoll_id))  ;  		  
		irpcdoubletR       =   int(m_rpcIdHelper->doubletR(prdcoll_id))    ;
		irpcdoubletZ       =   int(m_rpcIdHelper->doubletZ(prdcoll_id))    ;
		irpcdoubletPhi     =   int(m_rpcIdHelper->doubletPhi(prdcoll_id))  ;
		irpcgasGap	 =   int(m_rpcIdHelper->gasGap(prdcoll_id))      ;
		irpcmeasuresPhi    =   int(m_rpcIdHelper->measuresPhi(prdcoll_id)) ;
		irpcstrip	         =   int(m_rpcIdHelper->strip(prdcoll_id))       ;
        
		irpctime	     =   double((*rpcCoinCollection)->time())		     ;  	     
		// irpctriggerInfo	     =   int((*rpcCoinCollection)->triggerInfo   ())	 ; // double		       
		irpctriggerInfo    =   int ( ((*rpcCoinCollection)->isLowPtCoin())*6 + 
					     ((*rpcCoinCollection)->isLowPtInputToHighPtCm())*100 + 
					     ((*rpcCoinCollection)->isHighPtCoin())*106  );
		irpcambiguityFlag  =   double((*rpcCoinCollection)->ambiguityFlag());     
		irpcthreshold      =   int((*rpcCoinCollection)->threshold() )      ;
	
	
		//std::cout << "irpcthreshold rpcCoinCollection   " << irpcthreshold <<  "\n";		  
		// m_threshold: internal threshold 
		// set in joboptions the correspondence between m_threshold and lvl1 thresholds
		if ( irpcthreshold == m_lv1Thres_0 )  m_threshold = 0 ; // no threshold assigned
		if ( irpcthreshold == m_lv1Thres_1 )  m_threshold = 1 ;
		if ( irpcthreshold == m_lv1Thres_2 )  m_threshold = 2 ;
		if ( irpcthreshold == m_lv1Thres_3 )  m_threshold = 3 ;
              
		/*switch (irpcthreshold ) {
		  case m_lv1Thres_0 : m_threshold = 0 ; break ;
		  case m_lv1Thres_1 : m_threshold = 1 ; break ;
		  case m_lv1Thres_2 : m_threshold = 2 ; break ;
		  }  */
		if ( irpcmeasuresPhi==0 ){ 
		  if ( irpctriggerInfo==106 ) { RPC_Threshold_Eta->Fill( m_threshold+4 ) ; }
		  else if ( irpctriggerInfo==6 ) { RPC_Threshold_Eta->Fill( m_threshold   ) ; }
		}
		else { 
		  if ( irpctriggerInfo==106 ) { RPC_Threshold_Phi->Fill( m_threshold+4 ) ; } 
		  else if ( irpctriggerInfo==6 ) { RPC_Threshold_Phi->Fill( m_threshold   ) ; } 
		}
                      
		const MuonGM::RpcReadoutElement* descriptor_Atl = m_muonMgr->getRpcReadoutElement( prdcoll_id );
		x_atl = descriptor_Atl ->stripPos(prdcoll_id ).x() ;
		y_atl = descriptor_Atl ->stripPos(prdcoll_id ).y() ;
		z_atl = descriptor_Atl ->stripPos(prdcoll_id ).z() ;
                                
		Amg::Vector3D TrigHit2DEta(z_atl,sqrt(x_atl*x_atl + y_atl*y_atl), 0 );	  
		Amg::Vector3D TrigHit2DPhi(x_atl,y_atl, 0 );	  
		Amg::Vector3D TrigHit2D(0,0,0); 
		TrigHit2D = TrigHit2DEta ;
		if(irpcmeasuresPhi==1) TrigHit2D = TrigHit2DPhi ;
       
		//get chamber hardware name
		hardware_name=convertChamberName(irpcstationName,irpcstationEta,irpcstationPhi,type) ;
       
        
		//get information from geomodel to book and fill rpc histos with the right max strip number
		std::vector<int>   rpcstripshift = RpcGM::RpcStripShift(m_muonMgr,m_rpcIdHelper, prdcoll_id, irpctriggerInfo)  ;
		NphiStrips	       =  rpcstripshift[0] ;
		ShiftPhiStrips     =  rpcstripshift[1] ;
		NetaStrips	       =  rpcstripshift[2] ;
		ShiftEtaStrips     =  rpcstripshift[3] ;
		ShiftStrips	       =  rpcstripshift[4] ;
		NetaStripsTot      =  rpcstripshift[5] ;
		NetaStripsTotSideA =  rpcstripshift[6] ;
		NetaStripsTotSideC =  rpcstripshift[7] ;
		ShiftEtaStripsTot  =  rpcstripshift[8] ;
		Nbin  	       =  rpcstripshift[9] ;
		EtaStripSign       =  rpcstripshift[10];
		PanelIndex	       =  rpcstripshift[13];
		Settore	       =  rpcstripshift[14];
		PlaneTipo	       =  rpcstripshift[15];
		strip_dbindex      =  rpcstripshift[16];
		NetaPanelsTot	 =  rpcstripshift[19];  
		ShiftEtaPanelsTot  =  rpcstripshift[20];
		NetaPanelsTotSideA =  rpcstripshift[21];
		NetaPanelsTotSideC =  rpcstripshift[22];
		rpcpanel_dbindex   =  rpcstripshift[23];
		rpctower_dbindex   =  rpcstripshift[24];
		shiftstripphiatlas =  rpcstripshift[25];

 
		//get name for titles and labels 
		std::vector<std::string>   rpclayersectorsidename = RpcGM::RpcLayerSectorSideName(m_rpcIdHelper,prdcoll_id, irpctriggerInfo)  ;  
		layer_name		   = rpclayersectorsidename[0] ;
		layertodraw1_name	   = rpclayersectorsidename[1] ;
		layertodraw2_name	   = rpclayersectorsidename[2] ;
		layervslayer_name	   = rpclayersectorsidename[3] ;
		layer0_name		   = rpclayersectorsidename[4] ;
		layer1_name		   = rpclayersectorsidename[5] ;
		layer2_name		   = rpclayersectorsidename[6] ;
		layerPhivsEta_name	   = rpclayersectorsidename[7] ;
		layerPhivsEtaSector_name = rpclayersectorsidename[8] ;
		sector_name		   = rpclayersectorsidename[9] ;
		layeronly_name	   = rpclayersectorsidename[10];
		layer_name_panel	   = rpclayersectorsidename[11];
		sector_dphi_layer	   = rpclayersectorsidename[12];
	      
		int irpcstationNameC1 = irpcstationName ;
		int irpcstationNameC2 = irpcstationName ;
                
		if(irpctriggerInfo==106){
		  irpcstationNameC1 += 2 ;
		  irpcstationNameC2 += 2 ;
		  if(irpcstationName==8)	{
		    irpcstationNameC1 =  9 ;
		    irpcstationNameC2 = 10 ;
		  }
		}
		
		///////SHIFT 1D HISTOS
		 
	        if(EtaStripSign>=0){
		   if(irpctriggerInfo==  6)rpc1DStationNameTriggerHitsSideA->Fill(StationNameViewIndex[irpcstationName][irpcmeasuresPhi]  ,1./StationPivotSectorSize[irpcstationName]*AverageLuminosityWeight);		  
		   if(irpctriggerInfo==106)rpc1DStationNameTriggerHitsSideA->Fill(StationNameViewIndex[irpcstationName][irpcmeasuresPhi]+6,1./StationPivotSectorSize[irpcstationName]*AverageLuminosityWeight);
		} 
	        else{
		   if(irpctriggerInfo==  6)rpc1DStationNameTriggerHitsSideC->Fill(StationNameViewIndex[irpcstationName][irpcmeasuresPhi]  ,1./StationPivotSectorSize[irpcstationName]*AverageLuminosityWeight);		  
		   if(irpctriggerInfo==106)rpc1DStationNameTriggerHitsSideC->Fill(StationNameViewIndex[irpcstationName][irpcmeasuresPhi]+6,1./StationPivotSectorSize[irpcstationName]*AverageLuminosityWeight);
		}
		
		// fill trigger road
		// remember:
		// in sectors 12 and 14 HighPt plane has name = 9 or 10
          
 
		if(irpctriggerInfo==6||irpctriggerInfo==106){
	      
		  Muon::RpcPrepDataContainer::const_iterator collPrep ;
		  for ( collPrep = rpc_container->begin(); collPrep != rpc_container->end(); collPrep++ ) {
             
		    for ( Muon::RpcPrepDataCollection::const_iterator it=(*collPrep)->begin(); it!=(*collPrep)->end(); it++) {
               
		      Identifier prdConf_id   =   (*it)->identify();
		      irpcstationPhi_prep     =   int(m_rpcIdHelper->stationPhi (prdConf_id ))  ;   
		      irpcstationName_prep    =   int(m_rpcIdHelper->stationName(prdConf_id ))  ;   
		      irpcstationEta_prep     =   int(m_rpcIdHelper->stationEta (prdConf_id ))  ;	    
		      irpcdoubletR_prep       =   int(m_rpcIdHelper->doubletR   (prdConf_id ))  ;
		      irpcdoubletPhi_prep     =   int(m_rpcIdHelper->doubletPhi (prdConf_id ))  ;
		      irpcmeasuresPhi_prep    =   int(m_rpcIdHelper->measuresPhi(prdConf_id ))  ;
        
		      if ( irpcmeasuresPhi_prep != irpcmeasuresPhi ) continue;
		      if ( irpcstationPhi_prep  != irpcstationPhi  ) continue;
		      if ( irpcdoubletPhi_prep  != irpcdoubletPhi  ) continue;
		      if ( irpcstationName_prep != irpcstationNameC1 && irpcstationName_prep != irpcstationNameC2 ) continue;
		      if ( irpctriggerInfo==  6 && (irpcdoubletR_prep != irpcdoubletR  - 1 ) ) continue;
		      if ( irpctriggerInfo==106 && (irpcdoubletR_prep != irpcdoubletR  - 1 ) ) continue;
		      if (abs(irpcstationEta_prep-irpcstationEta)>1) continue;
        
		      //get information from geomodel to book and fill rpc histos with the right max strip number
	          
		      //		  std::cout << "elementID trig  " << irpcstrip << " " << irpcstationName <<" "<< irpcstationEta <<" "<< irpcstationPhi <<" "<< irpcdoubletR  << " " << irpctriggerInfo << "\n";		  
        
		      const MuonGM::RpcReadoutElement* descriptor_Atl_prep = m_muonMgr->getRpcReadoutElement( prdConf_id );
		      x_atl_prep = descriptor_Atl_prep ->stripPos(prdConf_id ).x() ;
		      y_atl_prep = descriptor_Atl_prep ->stripPos(prdConf_id ).y() ;
		      z_atl_prep = descriptor_Atl_prep ->stripPos(prdConf_id ).z() ;
        
		      Amg::Vector3D PrepHit2DEta( z_atl_prep, sqrt(x_atl_prep*x_atl_prep + y_atl_prep*y_atl_prep), 0 );  	  
		      Amg::Vector3D PrepHit2DPhi( x_atl_prep, y_atl_prep , 0 );  	  
		      Amg::Vector3D PrepHit2D   (0,0,0); 
		      PrepHit2D  = PrepHit2DEta ;
		      if(irpcmeasuresPhi_prep==1) PrepHit2D = PrepHit2DPhi ;		  
          
		      Amg::Vector3D SegVector2D   (0,0,0) ;
		      Amg::Vector3D SegPoint2D    (0,0,0) ;
		      Amg::Vector3D ImpactVector2D(0,0,0) ;
          
		      SegVector2D =  PrepHit2D - TrigHit2D ; 
		      SegPoint2D  =  TrigHit2D  	       ;

		      ImpactVector2D = SegPoint2D.cross(SegVector2D);	       
              
		      if(SegVector2D.mag()!=0)ImpactVector2D = ImpactVector2D/ SegVector2D.mag();      
        
		      // impactParam = ImpactVector2D.z();
		      impactParam = ImpactVector2D.mag() ;
		  
		      if ( irpcmeasuresPhi==0 ) {
			if ( ImpactVector2D.z() <0 ) impactParam = -impactParam ;
		      }
		      else { 
			if ( ImpactVector2D.z() <0 ) impactParam = -impactParam ;
		      }
              	  
		      RPC_TriggerRoad->Fill( impactParam, m_threshold );
	      
		      if ( irpctriggerInfo==6 && irpcstationEta%2 == 0 && irpcmeasuresPhi==0 ) {
			RPC_TriggerRoad_Large_Eta->Fill( impactParam, m_threshold );
		      }
		      if ( irpctriggerInfo==6 && irpcstationEta%2 == 0 && irpcmeasuresPhi==1 ) {
			RPC_TriggerRoad_Large_Phi->Fill( impactParam, m_threshold );
		      }
		      if ( irpctriggerInfo==6 && irpcstationEta%2 == 1 && irpcmeasuresPhi==0 ) {
			RPC_TriggerRoad_Small_Eta->Fill( impactParam, m_threshold );
		      }
		      if ( irpctriggerInfo==6 && irpcstationEta%2 == 1 && irpcmeasuresPhi==1 ) {
			RPC_TriggerRoad_Small_Phi->Fill( impactParam, m_threshold );
		      }     
		      if ( irpctriggerInfo >6 && irpcstationEta%2 == 0 && irpcmeasuresPhi==0 ) {
			RPC_TriggerRoad_Large_Eta->Fill( impactParam, m_threshold + 4 );
		      }
		      if ( irpctriggerInfo >6 && irpcstationEta%2 == 0 && irpcmeasuresPhi==1 ) {
			RPC_TriggerRoad_Large_Phi->Fill( impactParam, m_threshold + 4);
		      }
		      if ( irpctriggerInfo >6 && irpcstationEta%2 == 1 && irpcmeasuresPhi==0 ) {
			RPC_TriggerRoad_Small_Eta->Fill( impactParam, m_threshold + 4);
		      }
		      if ( irpctriggerInfo >6 && irpcstationEta%2 == 1 && irpcmeasuresPhi==1 ) {
			RPC_TriggerRoad_Small_Phi->Fill( impactParam, m_threshold + 4);
		      }     
                
		    }
		  }//end loop collPrep
		}//end if
 
		// fill general histograms
		if(irpctriggerInfo==6){
		  if(irpcmeasuresPhi==0) {NTrigger_Eta_LowPt ++ ;}
		  else {NTrigger_Phi_LowPt ++ ;}	
		  if(irpcmeasuresPhi==0){	 
		    if(EtaStripSign>0){
		      rpc2DPanelHits[enum_Eta_LowPt]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Eta_LowPt]->Fill(lumiblock, float(rpcpanel_dbindex)  + float(irpcdoubletPhi-1)*0.5-1     );
		      rpc2DPanelHits[enum_Eta_LowPt_BA]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Eta_LowPt_BA]->Fill(lumiblock, float(rpcpanel_dbindex)  + float(irpcdoubletPhi-1)*0.5-1     );
		     
		      rpc1DvsLBTrigTowerHits[enum_Eta_TrigTowerLowPt_BA]->Fill(lumiblock, float(rpctower_dbindex) + float(irpcdoubletPhi-1)*0.5-1      );
			
		      rpctime_LPt_BA->Fill(irpctime);
		    }
		    else{
		      rpc2DPanelHits[enum_Eta_LowPt]->Fill(-ShiftEtaPanelsTot-1, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Eta_LowPt]->Fill(lumiblock, -float(rpcpanel_dbindex) - float(irpcdoubletPhi-1)*0.5 + 0.5    );
		      rpc2DPanelHits[enum_Eta_LowPt_BC]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Eta_LowPt_BC]->Fill(lumiblock, float(rpcpanel_dbindex)  + float(irpcdoubletPhi-1)*0.5-1     );
		     
		      rpc1DvsLBTrigTowerHits[enum_Eta_TrigTowerLowPt_BC]->Fill(lumiblock, float(rpctower_dbindex) + float(irpcdoubletPhi-1)*0.5-1      );
			
	
		      rpctime_LPt_BC->Fill(irpctime);
		    }
		  }
		  else{	 
		    if(EtaStripSign>0){
		      rpc2DPanelHits[enum_Phi_LowPt]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Phi_LowPt]->Fill(lumiblock, float(rpcpanel_dbindex)  + float(irpcdoubletPhi-1)*0.5-1     );
		      rpc2DPanelHits[enum_Phi_LowPt_BA]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Phi_LowPt_BA]->Fill(lumiblock, float(rpcpanel_dbindex)  + float(irpcdoubletPhi-1)*0.5-1     );
		     
		      rpc1DvsLBTrigTowerHits[enum_Phi_TrigTowerLowPt_BA]->Fill(lumiblock, float(rpctower_dbindex) + float(irpcdoubletPhi-1)*0.5-1      );
		     
		      rpctime_LPt_BA->Fill(irpctime);
		    }
		    else{
		      rpc2DPanelHits[enum_Phi_LowPt]->Fill(-ShiftEtaPanelsTot-1, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Phi_LowPt]->Fill(lumiblock, -float(rpcpanel_dbindex) - float(irpcdoubletPhi-1)*0.5  + 0.5    );
		      rpc2DPanelHits[enum_Phi_LowPt_BC]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Phi_LowPt_BC]->Fill(lumiblock, float(rpcpanel_dbindex)  + float(irpcdoubletPhi-1)*0.5-1     );
		     
		      rpc1DvsLBTrigTowerHits[enum_Phi_TrigTowerLowPt_BC]->Fill(lumiblock, float(rpctower_dbindex) + float(irpcdoubletPhi-1)*0.5-1      );
		     
		      rpctime_LPt_BC->Fill(irpctime);
		    }
		  }
		  rpc2DEtaStationTriggerHits->Fill(irpcstationEta, Settore );
		  if ( irpcstationEta<0 ) { 
		    rpc2DEtaStationTriggerHits_Side_Pt[enumBC_LowPt] ->Fill(irpcstationEta, Settore-1 + 0.5*(irpcdoubletPhi-1) ); 
		  }
		  else {		      
		    rpc2DEtaStationTriggerHits_Side_Pt[enumBA_LowPt] ->Fill(irpcstationEta, Settore-1 + 0.5*(irpcdoubletPhi-1) ); 
		  }
		}
		else if(irpctriggerInfo==100){
		  rpc2DEtaStationTriggerHits->Fill(irpcstationEta, Settore-1 + 16 );
		  // if ( irpcstationEta<0 ) { rpc2DEtaStationTriggerHits_BC ->Fill(irpcstationEta, Settore-1 + 16 );}
		  // else {		      rpc2DEtaStationTriggerHits_BA ->Fill(irpcstationEta, Settore-1 + 16 );}
		} 
		else if(irpctriggerInfo==106){
		  if(irpcmeasuresPhi==0) {NTrigger_Eta_HighPt ++;}	 
		  else {NTrigger_Phi_HighPt ++;}
		  if(irpcmeasuresPhi==0){	 
		    if(EtaStripSign>0){
		      rpc2DPanelHits[enum_Eta_HighPt]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Eta_HighPt]->Fill(lumiblock, float(rpcpanel_dbindex)  + float(irpcdoubletPhi-1)*0.5-1    );
		      rpc2DPanelHits[enum_Eta_HighPt_BA]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Eta_HighPt_BA]->Fill(lumiblock, float(rpcpanel_dbindex)  + float(irpcdoubletPhi-1)*0.5-1     );
		     
		      rpc1DvsLBTrigTowerHits[enum_Eta_TrigTowerHighPt_BA]->Fill(lumiblock, float(rpctower_dbindex) + float(irpcdoubletPhi-1)*0.5-1      );
		     
		      rpctime_HPt_BA->Fill(irpctime);
		    }
		    else{
		      rpc2DPanelHits[enum_Eta_HighPt]->Fill(-ShiftEtaPanelsTot-1, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Eta_HighPt]->Fill(lumiblock, -float(rpcpanel_dbindex) - float(irpcdoubletPhi-1)*0.5  + 0.5    );
		      rpc2DPanelHits[enum_Eta_HighPt_BC]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Eta_HighPt_BC]->Fill(lumiblock, float(rpcpanel_dbindex)  + float(irpcdoubletPhi-1)*0.5-1     );
		     
		      rpc1DvsLBTrigTowerHits[enum_Eta_TrigTowerHighPt_BC]->Fill(lumiblock, float(rpctower_dbindex) + float(irpcdoubletPhi-1)*0.5-1      );
		     
		      rpctime_HPt_BC->Fill(irpctime);
		    }
		  }
		  else{	 
		    if(EtaStripSign>0){
		      rpc2DPanelHits[enum_Phi_HighPt]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Phi_HighPt]->Fill(lumiblock, float(rpcpanel_dbindex)  + float(irpcdoubletPhi-1)*0.5-1  );
		      rpc2DPanelHits[enum_Phi_HighPt_BA]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Phi_HighPt_BA]->Fill(lumiblock, float(rpcpanel_dbindex)  + float(irpcdoubletPhi-1)*0.5-1     );
		     
		      rpc1DvsLBTrigTowerHits[enum_Phi_TrigTowerHighPt_BA]->Fill(lumiblock, float(rpctower_dbindex) + float(irpcdoubletPhi-1)*0.5-1      );
		     
		      rpctime_HPt_BA->Fill(irpctime);
		    }
		    else{
		      rpc2DPanelHits[enum_Phi_HighPt]->Fill(-ShiftEtaPanelsTot-1, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Phi_HighPt]->Fill(lumiblock, -float(rpcpanel_dbindex) - float(irpcdoubletPhi-1)*0.5   + 0.5    );
		      rpc2DPanelHits[enum_Phi_HighPt_BC]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		      rpc1DvsLBPanelHits[enum_Phi_HighPt_BC]->Fill(lumiblock, float(rpcpanel_dbindex)  + float(irpcdoubletPhi-1)*0.5-1     );
		      
		     
		      rpc1DvsLBTrigTowerHits[enum_Phi_TrigTowerHighPt_BC]->Fill(lumiblock, float(rpctower_dbindex) + float(irpcdoubletPhi-1)*0.5-1      );
		     
		      rpctime_HPt_BC->Fill(irpctime);
		    }
		  }
	 
		  rpc2DEtaStationTriggerHits->Fill(irpcstationEta, Settore-1 + 32 );
		  if ( irpcstationEta<0 ) { rpc2DEtaStationTriggerHits_Side_Pt[enumBC_HighPt] ->Fill(irpcstationEta, Settore-1 + 0.5*(irpcdoubletPhi-1) );}
		  else {		      rpc2DEtaStationTriggerHits_Side_Pt[enumBA_HighPt] ->Fill(irpcstationEta, Settore-1 + 0.5*(irpcdoubletPhi-1) );}
		}
          
		if(m_rpcchamberhist){
		  ATH_MSG_DEBUG (  "RPC Hardware chamber name in the selected area : " << hardware_name );	    
 
		  histo_flag=true;
		  for (std::vector<std::string>::const_iterator iter=layer_name_list.begin(); iter!=layer_name_list.end(); iter++){
		    if ( (hardware_name+layer_name)==*iter){histo_flag=false;}
		  }
		  if (histo_flag){ 
		    layer_name_list.push_back(hardware_name+layer_name); 
		    bookRPCLayerHistograms(hardware_name, layer_name, layer0_name, Nbin, 0 , Nbin);
		  }
          
		  histo_flag=true ;
		  for (std::vector<std::string>::const_iterator iter=layer_name_list_panel.begin(); iter!=layer_name_list_panel.end(); iter++){
		    if ( (hardware_name+layer_name_panel)==*iter){histo_flag=false;}
		  }
		  if (histo_flag){ 
		    layer_name_list_panel.push_back(hardware_name+layer_name_panel)  ; 
		    layer_name_bin_list_panel.push_back( PanelIndex ) 	       ; 
		    bookRPCLayerHistogramsPanel(hardware_name, layer_name_panel)     ; 
		  }
                                   
		  const MuonDQAHistList& hists1 = m_stationHists.getList( hardware_name + "/Profiles/" + layer_name); 					    
		  TH1* rpcstriplayer = hists1.getH1( hardware_name + "_" + layer_name + "_strip");

                    
		  if (rpcstriplayer) {rpcstriplayer->Fill( float(irpcstrip  + ShiftStrips)  -0.5 );}
		  else {ATH_MSG_DEBUG (  "rpcstriplayer not in hist list!" );}


		}//end if on m_rpcchamberhist || ESD
		else{
		  histo_flag=true ;
		  for (std::vector<std::string>::const_iterator iter=layer_name_list_panel.begin(); iter!=layer_name_list_panel.end(); iter++){
		    if ( (hardware_name+layer_name_panel)==*iter){histo_flag=false;}
		  }
		  if (histo_flag){ 
		    layer_name_list_panel.push_back(hardware_name+layer_name_panel)  ; 
		    layer_name_bin_list_panel.push_back( PanelIndex ) 	       ; 
		  }
        
		}
                 
		////////////// Start Loop on the second coin ///////////////////////////////
		for (Muon::RpcCoinDataCollection::const_iterator rpcCoinCollectionII=(*trigcontainerIt)->begin();
		     rpcCoinCollectionII!=(*trigcontainerIt)->end(); ++rpcCoinCollectionII)				       
		  {
	
		    Identifier prdcoll_id_II = (*rpcCoinCollectionII)->identify(); 
             	     
		    irpcstationPhiII	    =	int(m_rpcIdHelper->stationPhi( prdcoll_id_II))   ; 	
		    irpcstationNameII         =	int(m_rpcIdHelper->stationName(prdcoll_id_II))   ;     
		    irpcstationEtaII	    =	int(m_rpcIdHelper->stationEta( prdcoll_id_II))   ; 	       
		    irpcdoubletRII	    =	int(m_rpcIdHelper->doubletR(   prdcoll_id_II))   ;  
		    irpcdoubletZII	    =	int(m_rpcIdHelper->doubletZ(   prdcoll_id_II))   ;
		    irpcdoubletPhiII	    =	int(m_rpcIdHelper->doubletPhi( prdcoll_id_II))   ;
		    irpcgasGapII	            =	int(m_rpcIdHelper->gasGap(     prdcoll_id_II))   ;
		    irpcmeasuresPhiII         =	int(m_rpcIdHelper->measuresPhi(prdcoll_id_II))   ;
		    irpcstripII	            =	int(m_rpcIdHelper->strip(      prdcoll_id_II))   ; 	 
		    irpctimeII 	            =	double((*rpcCoinCollectionII)->time())  	 ;					   
		    //irpctriggerInfoII         =	int((*rpcCoinCollectionII)->triggerInfo() )	 ; // double		 
		    irpctriggerInfoII	    =   int ( ((*rpcCoinCollection)->isLowPtCoin())*6 + 
					              ((*rpcCoinCollection)->isLowPtInputToHighPtCm())*100 + 
					              ((*rpcCoinCollection)->isHighPtCoin())*106  );
		    irpcambiguityFlagII       =	double((*rpcCoinCollectionII)->ambiguityFlag ()) ;
               
             				   
		    const MuonGM::RpcReadoutElement* descriptor_Atl_II = m_muonMgr->getRpcReadoutElement( prdcoll_id_II );
		    x_atl_II = descriptor_Atl_II ->stripPos(prdcoll_id_II ).x() ;
		    y_atl_II = descriptor_Atl_II ->stripPos(prdcoll_id_II ).y() ;
		    z_atl_II = descriptor_Atl_II ->stripPos(prdcoll_id_II ).z() ;
             	   
		    //get information from geomodel to book and fill rpc histos with the right max strip number
		    std::vector<int>	rpcstripshiftII = RpcGM::RpcStripShift(m_muonMgr,m_rpcIdHelper, prdcoll_id, irpctriggerInfo)  ;
		    NbinII  =  rpcstripshiftII[9];
             	    
		    if(irpcmeasuresPhi==1&&irpcmeasuresPhiII==0&&irpctriggerInfo==irpctriggerInfoII){
		      if(irpcstationPhi==irpcstationPhiII&&irpcstationName==irpcstationNameII&&irpcstationEta==irpcstationEtaII&&
			 irpcdoubletR==irpcdoubletRII&&irpcdoubletZ==irpcdoubletZII&&irpcdoubletPhi==irpcdoubletPhiII&&irpcgasGap==irpcgasGapII){
             	   
			if(m_rpcchamberhist ){   
			  //if(1 == 0){
			  histo_flag=true;
			  for (std::vector<std::string>::const_iterator iter=layerPhivsEta_name_list.begin(); iter!=layerPhivsEta_name_list.end(); iter++){
			    if ( (hardware_name+layerPhivsEta_name)==*iter){histo_flag=false;}
			  }
			  if (histo_flag){ 
			    layerPhivsEta_name_list.push_back(hardware_name+layerPhivsEta_name); 
			    bookRPCLayerPhivsEtaHistograms(hardware_name, layerPhivsEta_name, NetaStrips, 0 , NetaStrips, NphiStrips, 0 , NphiStrips);
			  }
			  const MuonDQAHistList& hists4 = m_stationHists.getList( hardware_name + "/PhivsEta/" + layerPhivsEta_name);  					   
			  TH2* rpcstriplayerPhivsEta = hists4.getH2(hardware_name + "_" + layerPhivsEta_name );    
			  if (rpcstriplayerPhivsEta) {rpcstriplayerPhivsEta->Fill( float(irpcstripII + ShiftEtaStrips)  -0.5,  float(irpcstrip + ShiftPhiStrips)  -0.5 );}
			  else {ATH_MSG_DEBUG (  "rpcstriplayerPhivsEta not in hist list!" );}
			}//end if on m_rpcchamberhist or ESD		 
             	   
			//Sector
             	   
			stripetaatlas  =  ( irpcstripII + ShiftEtaStripsTot )*EtaStripSign ; 	
			if ( stripetaatlas >0 ) stripetaatlas-- ;
			stripphisector =   irpcstrip + ShiftPhiStrips		      ;
			if(m_rpcsectorhist){ 		 
			  histo_flag=true;
			  for (std::vector<std::string>::const_iterator iter=layerPhivsEtaSector_name_list.begin(); iter!=layerPhivsEtaSector_name_list.end(); iter++){
			    if ( (sector_name+layerPhivsEtaSector_name)==*iter){histo_flag=false;}
			  }
			  if (histo_flag){ 
			    layerPhivsEtaSector_name_list.push_back(sector_name+layerPhivsEtaSector_name); 
			    bookRPCLayerPhivsEtaSectorHistograms(sector_name, layerPhivsEtaSector_name, NetaStripsTot, -NetaStripsTotSideC, NetaStripsTotSideA, NphiStrips, 0 , NphiStrips);
			  }		 
			  const MuonDQAHistList& hists5 = m_stationHists.getList( sector_name + "/PhivsEta/" + layerPhivsEtaSector_name);
			  TH2* rpcstriplayerPhivsEtaSector = hists5.getH2(layerPhivsEtaSector_name ); 
			  if (rpcstriplayerPhivsEtaSector) {
               
			    rpcstriplayerPhivsEtaSector->Fill( stripetaatlas, stripphisector-1 );    
			  }
			  else {ATH_MSG_DEBUG (  "rpcstriplayerPhivsEtaSector not in hist list!" );} 
			}
			 
			stripphiatlas = stripphisector + shiftstripphiatlas ;
               
			x_atlas = x_atl     ;
			y_atlas = y_atl     ;
			z_atlas = z_atl_II  ;
	
			if ( x_atlas > 0 ) { 
			  phi_atlas = atan ( y_atlas / x_atlas ); 
			}
			else if ( x_atlas == 0 ){ 
			  if (y_atlas > 0) { 
			    phi_atlas = CLHEP::pi/2 ;
			  }
			  else 
			    { 
			      phi_atlas = -CLHEP::pi/2 ;
			    }
			}
			else{
			  if (y_atlas > 0) { 
			    phi_atlas = atan ( y_atlas / x_atlas ) + CLHEP::pi ; 
			  }  
			  else 
			    { 
			      phi_atlas = -CLHEP::pi + atan ( y_atlas / x_atlas ) ;
			    }
             	      
	        	}
             	   
			// pseudorapidity
			if ( z_atlas!=0  ) {
			  eta_atlas = -log( abs( tan( 0.5 * atan(sqrt(pow(x_atlas,2.)+pow(y_atlas,2.))/ z_atlas )) ));
			}
			else{
			  eta_atlas = 0 ;
	        	}
			if ( irpcstationEta<0 ) { eta_atlas = -eta_atlas; }
        
			if(layeronly_name=="LowPt_TriggerOut"       ){
			  rpcPhivsEtaAtlasLowPt_TriggerOut	     ->Fill( stripetaatlas , stripphiatlas-1 );
			  rpcPhivsEtaAtlasLowPt_TriggerOut_PhivsZ ->Fill( z_atlas, phi_atlas )	    ;
	        	}
			if(layeronly_name=="HighPt_TriggerFromLowPt") { 
			  rpcPhivsEtaAtlasHighPt_TriggerFromLowPt	    ->Fill( stripetaatlas , stripphiatlas-1 );
			  rpcPhivsEtaAtlasHighPt_TriggerFromLowPt_PhivsZ ->Fill( z_atlas, phi_atlas )	   ;
	        	}
			if(layeronly_name=="HighPt_TriggerOut"      ) { 
			  rpcPhivsEtaAtlasHighPt_TriggerOut        ->Fill( stripetaatlas , stripphiatlas-1 );
			  rpcPhivsEtaAtlasHighPt_TriggerOut_PhivsZ ->Fill( z_atlas, phi_atlas )	     ;
			}
	  
			if ( irpctriggerInfo==6  ) {
			  if ( x_atlas!=0 &&  y_atlas!=0 ) { EtavsPhi_TriggeredMuons_LowPt->Fill( eta_atlas, phi_atlas ); }
			}
			if ( irpctriggerInfo==106  ) {
			  if ( x_atlas!=0 &&  y_atlas!=0 ) { EtavsPhi_TriggeredMuons_HighPt->Fill( eta_atlas, phi_atlas ); }
			}
			if ( irpctriggerInfo==6 && m_threshold==1) {
			  if ( x_atlas!=0 &&  y_atlas!=0 ) { EtavsPhi_TriggeredMuons_Pt1->Fill( eta_atlas, phi_atlas ); }
			}
			if ( irpctriggerInfo==6 && m_threshold==2) {
			  if ( x_atlas!=0 &&  y_atlas!=0 ) { EtavsPhi_TriggeredMuons_Pt2->Fill( eta_atlas, phi_atlas ); }
			}
			if ( irpctriggerInfo==6 && m_threshold==3) {
			  if ( x_atlas!=0 &&  y_atlas!=0 ) { EtavsPhi_TriggeredMuons_Pt3->Fill( eta_atlas, phi_atlas ); }
			}
			if ( irpctriggerInfo==106 && m_threshold==1) {
			  if ( x_atlas!=0 &&  y_atlas!=0 ) { EtavsPhi_TriggeredMuons_Pt4->Fill( eta_atlas, phi_atlas ); }
			}
			if ( irpctriggerInfo==106 && m_threshold==2) {
			  if ( x_atlas!=0 &&  y_atlas!=0 ) { EtavsPhi_TriggeredMuons_Pt5->Fill( eta_atlas, phi_atlas ); }
			}
			if ( irpctriggerInfo==106 && m_threshold==3) {
			  if ( x_atlas!=0 &&  y_atlas!=0 ) { EtavsPhi_TriggeredMuons_Pt6->Fill( eta_atlas, phi_atlas ); }
			}
	                         
		        //Data Quality Plot Alex Tuna
		        if(irpcstationEta>=0){
		         if( irpctriggerInfo==6){
		          rpc2DPanelHits[enum_EtaPhi_LowPt_BA]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		          rpc1DvsLBPanelHits[enum_EtaPhi_LowPt_BA]->Fill(lumiblock, float(rpcpanel_dbindex)  + float(irpcdoubletPhi-1)*0.5-1	 ); 
		         }
		         else if( irpctriggerInfo==106){
		          rpc2DPanelHits[enum_EtaPhi_HighPt_BA]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		          rpc1DvsLBPanelHits[enum_EtaPhi_HighPt_BA]->Fill(lumiblock, float(rpcpanel_dbindex)  + float(irpcdoubletPhi-1)*0.5-1	  );
		         }
		        }
		        else{
		         if( irpctriggerInfo==6){
		           rpc2DPanelHits[enum_EtaPhi_LowPt_BC]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		           rpc1DvsLBPanelHits[enum_EtaPhi_LowPt_BC]->Fill(lumiblock, float(rpcpanel_dbindex)  + float(irpcdoubletPhi-1)*0.5-1	  );
		         }
		         else if( irpctriggerInfo==106){
		          rpc2DPanelHits[enum_EtaPhi_HighPt_BC]->Fill(ShiftEtaPanelsTot, Settore + 0.5*(irpcdoubletPhi-1)-1);
		          rpc1DvsLBPanelHits[enum_EtaPhi_HighPt_BC]->Fill(lumiblock, float(rpcpanel_dbindex)  + float(irpcdoubletPhi-1)*0.5-1	  );
		         }
		        }
			  
		      }//same gasgap
		    }//phi and eta
    
		    //////////same chamber, plane, phi-phi, eta-eta
		    if(m_rpcchamberhist ){ 

		      if(irpcmeasuresPhi==irpcmeasuresPhiII){
             	
			if(	( irpctriggerInfo==100 && (irpcstationName==irpcstationNameII+2) && irpctriggerInfoII == 6     ) || 
				( irpctriggerInfo==106 && (irpcstationName==irpcstationNameII  ) && irpctriggerInfoII == 0     ) || 
				( irpctriggerInfo==  6 && (irpcstationName==irpcstationNameII  ) && irpctriggerInfoII == 0  &&
				  irpcdoubletRII ==  1 )       
				){
			  if(irpcstationPhi==irpcstationPhiII&&irpcstationEta==irpcstationEtaII&&
			     irpcdoubletR==irpcdoubletRII&&irpcdoubletZ==irpcdoubletZII&&irpcdoubletPhi==irpcdoubletPhiII){
             			 
			    histo_flag=true;
			    for (std::vector<std::string>::const_iterator iter=layervslayer_name_list.begin(); iter!=layervslayer_name_list.end(); iter++){
			      if ( (hardware_name+layervslayer_name)==*iter){histo_flag=false;}
			    }
			    if (histo_flag){ 
			      layervslayer_name_list.push_back(hardware_name+layervslayer_name); 
			      bookRPCLayervsLayerHistograms(hardware_name, layervslayer_name, layer1_name, layer2_name, Nbin, 0 , Nbin, Nbin, 0, Nbin);
			    }  
    
			    const MuonDQAHistList& hists6 = m_stationHists.getList( hardware_name +"/Layer2vsLayer1/"+layervslayer_name);  
			    TH2* rpcstriplayervslayer = hists6.getH2(hardware_name + "_" + layervslayer_name );  
 
			    if (rpcstriplayervslayer) {rpcstriplayervslayer->Fill( irpcstripII + ShiftStrips , irpcstrip + ShiftStrips );}
			    else {ATH_MSG_DEBUG (  "rpcstriplayervslayer not in hist list!" );}
  
			  }//same chamber
			}//same plane
		      }//phi-phi or eta-eta		     
		    } //end if on m_rpcchamberhist or ESD  
 
		  }  ////////////// End Loop on the second coin
            
		m_nTrig++ ;
    
	      }
	    }  
	} // end loop on trigger hits
      } //end if
      
      
      
      if(m_doLumiPlot){
       rpcTriggerHitsPerEvents_Eta_LowPt  ->  Fill (NTrigger_Eta_LowPt )  ;  
       rpcTriggerHitsPerEvents_Phi_LowPt  ->  Fill (NTrigger_Phi_LowPt )  ;  
       rpcTriggerHitsPerEvents_Eta_HighPt ->  Fill (NTrigger_Eta_HighPt)  ;  
       rpcTriggerHitsPerEvents_Phi_HighPt ->  Fill (NTrigger_Phi_HighPt)  ;
      }
      // begin cluster monitoring
      const Muon::RpcPrepDataContainer* rpc_clusterContainer;
      if(m_eventStore->contains<Muon::RpcPrepDataContainer>(m_clusterContainerName)){
	sc = m_eventStore->retrieve(rpc_clusterContainer, m_clusterContainerName);
	if ( sc.isSuccess() ) {
	  Clus_Retr = true ; 
	}
	else {
	  ATH_MSG_DEBUG (  " RpcRawDataValAlg :: Cannot retrieve the RPC cluster container " );
	  Clus_Retr = false;
	  return sc;
	}
      }  
      else { Clus_Retr = false;}
   
      if (m_doClusters && Clus_Retr )
	{  
	  ATH_MSG_DEBUG (  "Start RPC Cluster Monitoring" );
  
	  // RPC clusters histograms

	  m_nClus=0;
    
  
	  sc = rpcprd_shift.getHist(rpcclusters,"Number_of_RPC_clusters_per_event");		
	  if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register rpcclusters hist to MonGroup" );

	  sc = rpcprd_expert.getHist(rpcCSEta,"Eta_ClusterSize_Distribution");		
	  if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register rpcCSEta hist to MonGroup" );
    
	  sc = rpcprd_expert.getHist(rpcCSPhi,"Phi_ClusterSize_Distribution");		
	  if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register rpcCSPhi hist to MonGroup" );

	  ATH_MSG_DEBUG (  "Filling the RPCRawData Monitoring with clusters informations " );
   
  

	  Muon::RpcPrepDataContainer::const_iterator it = rpc_clusterContainer->begin();
    
	  // Access by Collection
  
	  for ( ; it != rpc_clusterContainer->end() ; ++it ) {

	    const Muon::RpcPrepDataCollection* clusterCollection = *it;
    
	    if (clusterCollection->size()>0) {
      
	      ATH_MSG_DEBUG (  "New Cluster collection" );

	      for (Muon::RpcPrepDataCollection::const_iterator rpcCollection = clusterCollection->begin(); 
		   rpcCollection != clusterCollection->end(); ++rpcCollection) {
	
		Identifier prd_id = (*rpcCollection)->identify();
	
		ATH_MSG_DEBUG (  "Adding a new cluster " );
	     
		irpc_clus_size     =  ((*rpcCollection)->rdoList()).size();
		irpc_clus_time     =  (*rpcCollection)->time()            ;
		irpc_clus_station  =  m_rpcIdHelper->stationName(prd_id)  ;
		irpc_clus_eta      =  m_rpcIdHelper->stationEta(prd_id)   ;
		irpc_clus_phi      =  m_rpcIdHelper->stationPhi(prd_id)   ;
		irpc_clus_doublr   =  m_rpcIdHelper->doubletR(prd_id)     ;
		irpc_clus_doublz   =  m_rpcIdHelper->doubletZ(prd_id)     ;
		irpc_clus_doublphi =  m_rpcIdHelper->doubletPhi(prd_id)   ;
		irpc_clus_gasgap   =  m_rpcIdHelper->gasGap(prd_id)       ;
		irpc_clus_measphi  =  m_rpcIdHelper->measuresPhi(prd_id)  ;

		// get the cluster position
		const MuonGM::RpcReadoutElement* descriptor = m_muonMgr->getRpcReadoutElement(prd_id);
	      
		const Amg::Vector3D stripPosC = descriptor->stripPos(prd_id);
	      
		irpc_clus_posx = stripPosC.x() ;
		irpc_clus_posy = stripPosC.y() ;
		irpc_clus_posz = stripPosC.z() ;
	
	
		if(irpc_clus_measphi==0){
		  rpcCSEta->Fill( irpc_clus_size);
		  if(irpc_clus_eta>=0){
		    rpcCSEta_BA->Fill( irpc_clus_size);
		  }
		  else{
		    rpcCSEta_BC->Fill( irpc_clus_size);		 
		  }		
		}
		else{
		  rpcCSPhi->Fill( irpc_clus_size);
		  if(irpc_clus_eta>=0){
		    rpcCSPhi_BA->Fill( irpc_clus_size);
		  }
		  else{
		    rpcCSPhi_BC->Fill( irpc_clus_size);		 
		  }
		}


		//cluster profiles
		hardware_name=convertChamberName(irpc_clus_station,irpc_clus_eta,irpc_clus_phi,type) ;
	
  
		//get information from geomodel to book and fill rpc histos with the right max strip number
		std::vector<int>   rpcstripshift = RpcGM::RpcStripShift(m_muonMgr,m_rpcIdHelper, prd_id, 0)  ;
	    		  
		NphiStrips  	 =  rpcstripshift[ 0]  ;
		ShiftPhiStrips	 =  rpcstripshift[ 1]  ;
		NetaStrips  	 =  rpcstripshift[ 2]  ;
		ShiftEtaStrips	 =  rpcstripshift[ 3]  ;
		ShiftStrips 	 =  rpcstripshift[ 4]  ;
		NetaStripsTot	 =  rpcstripshift[ 5]  ;
		NetaStripsTotSideA =  rpcstripshift[ 6]  ;
		NetaStripsTotSideC =  rpcstripshift[ 7]  ;
		ShiftEtaStripsTot	 =  rpcstripshift[ 8]  ;
		Nbin		 =  rpcstripshift[ 9]  ;
		EtaStripSign	 =  rpcstripshift[10]  ;
		SectorLogic 	 =  rpcstripshift[11]  ;
		Side		 =  rpcstripshift[12]  ;
		PanelIndex  	 =  rpcstripshift[13]  ;
		strip_dbindex      =  rpcstripshift[16]  ;
 
		//get name for titles and labels
		std::vector<std::string>   rpclayersectorsidename = RpcGM::RpcLayerSectorSideName(m_rpcIdHelper,prd_id, 0)  ;
  	       
		layer_name	       = rpclayersectorsidename[ 0]  ;
		layertodraw1_name	       = rpclayersectorsidename[ 1]  ;
		layertodraw2_name	       = rpclayersectorsidename[ 2]  ;
		layervslayer_name	       = rpclayersectorsidename[ 3]  ;
		layer0_name 	       = rpclayersectorsidename[ 4]  ;
		layer1_name 	       = rpclayersectorsidename[ 5]  ;
		layer2_name 	       = rpclayersectorsidename[ 6]  ;
		layerPhivsEta_name       = rpclayersectorsidename[ 7]  ;
		layerPhivsEtaSector_name = rpclayersectorsidename[ 8]  ;
		sector_name 	       = rpclayersectorsidename[ 9]  ;
		layeronly_name	       = rpclayersectorsidename[10]  ;
		layer_name_panel         = rpclayersectorsidename[11]  ;	
		sector_dphi_layer        = rpclayersectorsidename[12]  ;
	      
	      
		av_strip = 0 ;
		for(int i=0; i!=irpc_clus_size ; i++){
		  Identifier id = ((*rpcCollection)->rdoList())[i] ;
		  strip = int(m_rpcIdHelper->strip(id))            ;
		  strip +=  ShiftStrips                            ;
		  av_strip += float(strip)                         ;
		}
		if( irpc_clus_size != 0 ) av_strip = av_strip / irpc_clus_size ;
		
		if(m_rpcchamberhist ){

		  const MuonDQAHistList& hists_ly1 = m_stationHists.getList( hardware_name + "/Profiles/" + layer_name);       	  	  	  	  	
		  TH2* rpcclustersizelayer    = hists_ly1.getH2( hardware_name + "_" + layer_name + "_clustersize"   );      	  	  	  	  	
		  TH1* rpcclusterlayer        = hists_ly1.getH1( hardware_name + "_" + layer_name + "_cluster"       );		
	 
		  const MuonDQAHistList& hists_ly2 = m_stationHists.getList( hardware_name + "/Panels/" + layer_name_panel)  ;
		  TH1* rpcclustersizedislayer = hists_ly2.getH1( hardware_name + "_" + layer_name_panel + "_CSdistribution") ;
		  
		  avstrip = 0 ;		
		  if (rpcclustersizelayer) {
		    rpcclustersizedislayer->Fill(irpc_clus_size);
		    for(int i=0; i!=irpc_clus_size ; i++){
		      Identifier id = ((*rpcCollection)->rdoList())[i]   ;
		      strip = int(m_rpcIdHelper->strip(id))              ;
		      strip +=  ShiftStrips                              ;
		      if(rpcclustersizelayer)rpcclustersizelayer->Fill( strip,  irpc_clus_size );
		      avstrip += float(strip);
		    } 
		
		  }
		  else {
		    ATH_MSG_DEBUG (  "rpcclustersizelayer not in hist list!" );
		  }	
	      
		  if (rpcclusterlayer) {
		    if(irpc_clus_size != 0)avstrip = avstrip / irpc_clus_size ;
		    rpcclusterlayer->Fill( avstrip );
		  }   
		  else {
		    ATH_MSG_DEBUG (  "rpcclusterlayer not in hist list!" );
		  }	
		}//end if on m_rpcchamberhist or ESD	
		
	        
		++m_nClus;
	
		//second loop on clusters begin only if previous was phi strips
		if(irpc_clus_measphi==1){
		  for (Muon::RpcPrepDataCollection::const_iterator rpcCollectionII = clusterCollection->begin(); 
		       rpcCollectionII != clusterCollection->end(); ++rpcCollectionII) {
	   	    
		    Identifier prd_idII = (*rpcCollectionII)->identify();
	     
		    irpc_clus_sizeII     = ((*rpcCollectionII)->rdoList()).size();
		    irpc_clus_timeII     =  (*rpcCollectionII)->time()           ;
		    irpc_clus_stationII  =  m_rpcIdHelper->stationName(prd_idII) ;
		    irpc_clus_etaII      =  m_rpcIdHelper->stationEta(prd_idII)  ;
		    irpc_clus_phiII      =  m_rpcIdHelper->stationPhi(prd_idII)  ;
		    irpc_clus_doublrII   =  m_rpcIdHelper->doubletR(prd_idII)    ;
		    irpc_clus_doublzII   =  m_rpcIdHelper->doubletZ(prd_idII)    ;
		    irpc_clus_doublphiII =  m_rpcIdHelper->doubletPhi(prd_idII)  ;
		    irpc_clus_gasgapII   =  m_rpcIdHelper->gasGap(prd_idII)      ; 
		    irpc_clus_measphiII  =  m_rpcIdHelper->measuresPhi(prd_idII) ;
	   
		    if(irpc_clus_measphi  == irpc_clus_measphiII )continue;
		    if(irpc_clus_station  != irpc_clus_stationII )continue;
		    if(irpc_clus_eta      != irpc_clus_etaII     )continue;
		    if(irpc_clus_phi      != irpc_clus_phiII     )continue;
		    if(irpc_clus_doublr   != irpc_clus_doublrII  )continue;
		    if(irpc_clus_doublz   != irpc_clus_doublzII  )continue;
		    if(irpc_clus_doublphi != irpc_clus_doublphiII)continue;
		    if(irpc_clus_gasgap   != irpc_clus_gasgapII  )continue;
	   
		    //evaluate average strip
		    avstripeta = 0       ;
		    avstripphi = av_strip ; 
		    ShiftEtaStripsTot = RpcGM::RpcStripShift(m_muonMgr,m_rpcIdHelper, prd_idII, 0)[8]  ;  // angelo 07 oct 2009
		    EtaStripSign      = RpcGM::RpcStripShift(m_muonMgr,m_rpcIdHelper, prd_idII, 0)[10] ;  // angelo 07 oct 2009

		    for(int i=0; i!=irpc_clus_sizeII ; i++){
		      Identifier id = ((*rpcCollectionII)->rdoList())[i]             ;
		      avstripeta += float(m_rpcIdHelper->strip(id))/irpc_clus_sizeII ;
		    }
	   
		    avstripeta += float(ShiftEtaStripsTot)       ;
		    avstripeta  = avstripeta*float(EtaStripSign) ;
	   
		    // get the cluster position
		    const MuonGM::RpcReadoutElement* descriptorII = 
		      m_muonMgr->getRpcReadoutElement(prd_idII);
	
		    const Amg::Vector3D stripPosCII = descriptorII->stripPos(prd_idII);
	          
		    irpc_clus_posxII = stripPosCII.x() ;
		    irpc_clus_posyII = stripPosCII.y() ;
		    irpc_clus_poszII = stripPosCII.z() ;
	   
		    const MuonDQAHistList& hists8 = m_stationHists.getList( sector_name + "/PhivsEta/" + layerPhivsEtaSector_name);       	  	  	  	  	
		    TH2* rpcclusterlayerPhivsEtaSector = hists8.getH2( layerPhivsEtaSector_name + "_cluster" ); 
	   
		    if (rpcclusterlayerPhivsEtaSector) {
		      rpcclusterlayerPhivsEtaSector->Fill( avstripeta , avstripphi );
		      layertype = 0 ;
		      if(layeronly_name=="Pivot0"	){
			layertype = 3 ;
			//rpcPhivsEtaAtlasPivot0->Fill( avstripeta , avstripphi + shiftphiatlas );
		      }
		      if(layeronly_name=="Pivot1"	){
			layertype = 4 ;
			//rpcPhivsEtaAtlasPivot1->Fill( avstripeta , avstripphi + shiftphiatlas );
		      }
		      if(layeronly_name=="LowPt0"	){
			layertype = 1 ;
			//rpcPhivsEtaAtlasLowPt0->Fill( avstripeta , avstripphi + shiftphiatlas );
		      }
		      if(layeronly_name=="LowPt1"	){
			layertype = 2 ;
			//rpcPhivsEtaAtlasLowPt1->Fill( avstripeta , avstripphi + shiftphiatlas );
		      }
		      if(layeronly_name=="HighPt0"  ){
			layertype = 5 ;
			//rpcPhivsEtaAtlasHighPt0->Fill( avstripeta , avstripphi + shiftphiatlas );
		      }
		      if(layeronly_name=="HighPt1"  ){
			layertype = 6 ;
			//rpcPhivsEtaAtlasHighPt1->Fill( avstripeta , avstripphi + shiftphiatlas );
		      }	   
		      //if(layeronly_name=="Pivot0"                 )rpcPhivsEtaAtlasPivot0                 ->Fill( avstripeta , avstripphi + shiftphiatlas );
		      //if(layeronly_name=="Pivot1"                 )rpcPhivsEtaAtlasPivot1                 ->Fill( avstripeta , avstripphi + shiftphiatlas );    
		      //if(layeronly_name=="LowPt0"                 )rpcPhivsEtaAtlasLowPt0                 ->Fill( avstripeta , avstripphi + shiftphiatlas );
		      //if(layeronly_name=="LowPt1"                 )rpcPhivsEtaAtlasLowPt1                 ->Fill( avstripeta , avstripphi + shiftphiatlas );
		      //if(layeronly_name=="HighPt0"                )rpcPhivsEtaAtlasHighPt0                ->Fill( avstripeta , avstripphi + shiftphiatlas );
		      //if(layeronly_name=="HighPt1"                )rpcPhivsEtaAtlasHighPt1                ->Fill( avstripeta , avstripphi + shiftphiatlas );	    
		      //if(layeronly_name=="LowPt_TriggerOut"       )rpcPhivsEtaAtlasLowPt_TriggerOut       ->Fill( avstripeta , avstripphi + shiftphiatlas );
		      //if(layeronly_name=="HighPt_TriggerFromLowPt")rpcPhivsEtaAtlasHighPt_TriggerFromLowPt->Fill( avstripeta , avstripphi + shiftphiatlas );
		      //if(layeronly_name=="HighPt_TriggerOut"      )rpcPhivsEtaAtlasHighPt_TriggerOut      ->Fill( avstripeta , avstripphi + shiftphiatlas );
	     
		    }
		    else {ATH_MSG_DEBUG (  "rpcclusterlayerPhivsEtaSector not in hist list!" );}
 		
		  		 
	 
		  } // for loop on RpcPrepDataCollection
		} //second loop on clusters end	
	

	      }//end clusters collection
	    }//end if size
	  }//end clusters container
  
	  rpcclusters->Fill( m_nClus  );
      
	} // END IF (m_doClusters)  

	
      

    }}//m_doRpcESD // AthenaMonManager::tier0 || AthenaMonManager::tier0ESD        
  return sc; // statuscode check 
  
}


/*----------------------------------------------------------------------------------*/
StatusCode RpcRawDataValAlg::bookHistogramsRecurrent()
/*----------------------------------------------------------------------------------*/
{

  ATH_MSG_DEBUG (  "RPC RawData Monitoring Histograms being booked" );
 
  StatusCode sc = StatusCode::SUCCESS; 
  if( m_doRpcESD==true ) {if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online ) {       
      //declare a group of histograms
      std::string m_generic_path_rpcmonitoring = "Muon/MuonRawDataMonitoring/RPC";
      MonGroup rpcprd_shift( this, m_generic_path_rpcmonitoring+"/Overview", run, ATTRIB_UNMANAGED );
      MonGroup rpcprd_expert( this, m_generic_path_rpcmonitoring+"/Overview", run, ATTRIB_UNMANAGED )     ;
      MonGroup rpc_dqmf_global( this, m_generic_path_rpcmonitoring + "/GLOBAL", run, ATTRIB_UNMANAGED )    ;
      MonGroup rpcprd_dq_BA( this, m_generic_path_rpcmonitoring + "/RPCBA", run, ATTRIB_UNMANAGED  )       ;
      MonGroup rpcprd_dq_BC( this, m_generic_path_rpcmonitoring + "/RPCBC", run, ATTRIB_UNMANAGED )        ;
      MonGroup rpcprd_dq_Panel( this, m_generic_path_rpcmonitoring + "/GLOBAL", run, ATTRIB_UNMANAGED )    ;
      MonGroup rpcprd_dq_BA_Panel( this, m_generic_path_rpcmonitoring + "/RPCBA", run, ATTRIB_UNMANAGED )    ;
      MonGroup rpcprd_dq_BC_Panel( this, m_generic_path_rpcmonitoring + "/RPCBC", run, ATTRIB_UNMANAGED )    ;
      MonGroup rpcprd_dq_BA_TrigTower( this, m_generic_path_rpcmonitoring + "/RPCBA", run, ATTRIB_UNMANAGED )    ;
      MonGroup rpcprd_dq_BC_TrigTower( this, m_generic_path_rpcmonitoring + "/RPCBC", run, ATTRIB_UNMANAGED )    ;
      MonGroup rpcTrigRoad ( this, m_generic_path_rpcmonitoring + "/TriggerRoad", run, ATTRIB_UNMANAGED )  ;
    
      if(newLumiBlockFlag() && m_doLumiPlot){
	
	MonGroup rpcTrig_lumi_block ( this, m_generic_path_rpcmonitoring + "/lumiblock", lumiBlock, ATTRIB_UNMANAGED )  ;
 	
	// Relative Luminosity with Trigger hits   
	TH1 *rpcTriggerHitsPerEvents_Eta_LowPt = new TH1I("rpcTriggerHitsPerEvents_Eta_LowPt","rpcTriggerHitsPerEvents_Eta_LowPt", 20, 0, 20);
	sc=rpcTrig_lumi_block.regHist(rpcTriggerHitsPerEvents_Eta_LowPt);
	rpcTriggerHitsPerEvents_Eta_LowPt->SetFillColor(42) ;
	rpcTriggerHitsPerEvents_Eta_LowPt->GetXaxis()->SetTitle("Number of Trigger Hits");
	rpcTriggerHitsPerEvents_Eta_LowPt->GetYaxis()->SetTitle("Counts"  );  
	
	TH1 *rpcTriggerHitsPerEvents_Phi_LowPt = new TH1I("rpcTriggerHitsPerEvents_Phi_LowPt","rpcTriggerHitsPerEvents_Phi_LowPt", 20, 0, 20);
	sc=rpcTrig_lumi_block.regHist(rpcTriggerHitsPerEvents_Phi_LowPt);
	rpcTriggerHitsPerEvents_Phi_LowPt->SetFillColor(42) ;
	rpcTriggerHitsPerEvents_Phi_LowPt->GetXaxis()->SetTitle("Number of Trigger Hits");
	rpcTriggerHitsPerEvents_Phi_LowPt->GetYaxis()->SetTitle("Counts"  );   
	
	TH1 *rpcTriggerHitsPerEvents_Eta_HighPt = new TH1I("rpcTriggerHitsPerEvents_Eta_HighPt","rpcTriggerHitsPerEvents_Eta_HighPt", 20, 0, 20);
	sc=rpcTrig_lumi_block.regHist(rpcTriggerHitsPerEvents_Eta_HighPt);
	rpcTriggerHitsPerEvents_Eta_HighPt->SetFillColor(42) ;
	rpcTriggerHitsPerEvents_Eta_HighPt->GetXaxis()->SetTitle("Number of Trigger Hits");
	rpcTriggerHitsPerEvents_Eta_HighPt->GetYaxis()->SetTitle("Counts"  );  
	
	TH1 *rpcTriggerHitsPerEvents_Phi_HighPt = new TH1I("rpcTriggerHitsPerEvents_Phi_HighPt","rpcTriggerHitsPerEvents_Phi_HighPt", 20, 0, 20);
	sc=rpcTrig_lumi_block.regHist(rpcTriggerHitsPerEvents_Phi_HighPt);
	rpcTriggerHitsPerEvents_Phi_HighPt->SetFillColor(42) ;
	rpcTriggerHitsPerEvents_Phi_HighPt->GetXaxis()->SetTitle("Number of Trigger Hits");
	rpcTriggerHitsPerEvents_Phi_HighPt->GetYaxis()->SetTitle("Counts"  ); 
	  	  
	  
      }
      if(newRunFlag())
	{      
	  ATH_MSG_INFO (  "RPC RawData Monitoring : begin of run" );
	  	  
	  
	  std::string m_generic_path_rpcevents = m_generic_path_rpcmonitoring+"/Overview";
	  std::string m_rpcevents_title = "Number_of_RPC_hits_per_event";
	  const char* m_rpcevents_title_char = m_rpcevents_title.c_str();  

	  TH1 *rpcevents=new TH1I(m_rpcevents_title_char,m_rpcevents_title_char,300,-0.5,299.5);	    
	  sc=rpcprd_shift.regHist(rpcevents) ;  
	  if(sc.isFailure())
	    { ATH_MSG_FATAL(  "rpcevents Failed to register histogram " );       
	      return sc;
	    }
	  rpcevents->SetFillColor(42);
	  rpcevents->GetXaxis()->SetTitle("[counts]");
	  rpcevents->GetYaxis()->SetTitle("Number of Hits/Events"); 
	
	  ATH_MSG_DEBUG (  "INSIDE bookHistograms : " << rpcevents << m_generic_path_rpcevents.c_str() );
	  //ATH_MSG_DEBUG (  "SHIFT : " << shift );
	  ATH_MSG_DEBUG (  "RUN : " << run );
	  ATH_MSG_DEBUG (  "Booked bookrpcevents successfully" );       
	
	
	
	  std::string m_generic_path_rpcclusters = m_generic_path_rpcmonitoring+"/Overview";
	  std:: string m_rpcclusters_title = "Number_of_RPC_clusters_per_event";
	  const char* m_rpcclusters_title_char = m_rpcclusters_title.c_str();  
	
	  TH1 *rpcclusters=new TH1I(m_rpcclusters_title_char,m_rpcclusters_title_char,300,-0.5,299.5);	  
	  sc=rpcprd_shift.regHist(rpcclusters) ;  
	  if(sc.isFailure())
	    { ATH_MSG_FATAL(  "rpcclusters Failed to register histogram " );       
	      return sc;
	    }
	  rpcclusters->SetFillColor(42);
	  rpcclusters->GetXaxis()->SetTitle("[counts]");
	  rpcclusters->GetYaxis()->SetTitle("Number of Clusters/Events"); 
	 
	  ATH_MSG_DEBUG (  "INSIDE bookHistograms : " << rpcclusters << m_generic_path_rpcclusters.c_str() );
	 // ATH_MSG_DEBUG (  "SHIFT : " << shift );
	  ATH_MSG_DEBUG (  "RUN : " << run );
	  ATH_MSG_DEBUG (  "Booked bookrpcclusters successfully" );      

	  //Eta time
	  std::string m_generic_path_rpcEtaTime = m_generic_path_rpcmonitoring+"/Overview";
	  const char* m_rpcEtaTime_title_char = "Eta_Time_Distribution";
	  TH1 *rpcEtaTime=new TH1I(m_rpcEtaTime_title_char,m_rpcEtaTime_title_char, timeNbin, timeminrange, timemaxrange);  	       
	  sc=rpcprd_expert.regHist(rpcEtaTime) ;  
	  if(sc.isFailure())
	    { ATH_MSG_FATAL(  "rpc_EtaTime Failed to register histogram " );       
	      return sc;
	    }
	  rpcEtaTime->SetFillColor(42);
	  rpcEtaTime->GetXaxis()->SetTitle("Time Eta View [ns]");
	  rpcEtaTime->GetYaxis()->SetTitle("Counts/(3.125ns)");
     
	  //Phi time
	  std::string m_generic_path_rpcPhiTime = m_generic_path_rpcmonitoring+"/Overview";
	  const char* m_rpcPhiTime_title_char = "Phi_Time_Distribution";
	  TH1 *rpcPhiTime=new TH1I(m_rpcPhiTime_title_char,m_rpcPhiTime_title_char,timeNbin, timeminrange, timemaxrange);
	  sc=rpcprd_expert.regHist(rpcPhiTime) ;  
	  if(sc.isFailure())
	    { ATH_MSG_FATAL(  "rpc_PhiTime Failed to register histogram " );       
	      return sc;
	    }
	  rpcPhiTime->SetFillColor(42);
	  rpcPhiTime->GetXaxis()->SetTitle("Time Phi View [ns]");
	  rpcPhiTime->GetYaxis()->SetTitle("Counts/(3.125ns)");
     
     
	  //CS Eta
	  std::string m_generic_path_rpcCSEta = m_generic_path_rpcmonitoring+"/Overview";
	  const char* m_rpcCSEta_title_char = "Eta_ClusterSize_Distribution";
	  TH1 *rpcCSEta=new TH1I(m_rpcCSEta_title_char,m_rpcCSEta_title_char,32, -0.5, 31.5);
	  sc=rpcprd_expert.regHist(rpcCSEta) ;  
	  if(sc.isFailure())
	    { ATH_MSG_FATAL(  "rpcCSEta Failed to register histogram " );	    
	      return sc;
	    }
	  rpcCSEta->SetFillColor(42);
	  rpcCSEta->GetXaxis()->SetTitle("Cluster Size Eta view");
	  rpcCSEta->GetYaxis()->SetTitle("Counts");      
     
     
	  //CS Phi
	  std::string m_generic_path_rpcCSPhi = m_generic_path_rpcmonitoring+"/Overview";
	  const char* m_rpcCSPhi_title_char = "Phi_ClusterSize_Distribution";
	  TH1 *rpcCSPhi=new TH1I(m_rpcCSPhi_title_char,m_rpcCSPhi_title_char, 32, -0.5, 31.5);
	  sc=rpcprd_expert.regHist(rpcCSPhi) ;  
	  if(sc.isFailure())
	    { ATH_MSG_FATAL(  "rpcCSPhi Failed to register histogram " );	    
	      return sc;
	    }
	  rpcCSPhi->SetFillColor(42);
	  rpcCSPhi->GetXaxis()->SetTitle("Cluster Size Phi view");
	  rpcCSPhi->GetYaxis()->SetTitle("Counts");      
     
     
	  //RPC2D station
 
	  //
	  TH2 *rpc2DEtaStationGap1=new TH2I("rpc2DEtaStationGap1","rpc2DEtaStationGap1", 13, -6, 7,  16*3, 0, 16*3); 
	  sc=rpcprd_expert.regHist(rpc2DEtaStationGap1) ; 
	  rpc2DEtaStationGap1->SetFillColor(42);  
	  rpc2DEtaStationGap1->SetMarkerColor(1);  
	  rpc2DEtaStationGap1->SetMarkerStyle(21);
	  rpc2DEtaStationGap1->SetOption("COLZ");    
	  rpc2DEtaStationGap1->SetMarkerSize(0.2);
	  rpc2DEtaStationGap1->GetXaxis()->SetTitle("<--- SIDE C      Rpc Eta Station     SIDE A --->");
	  rpc2DEtaStationGap1->GetXaxis()->SetTitleSize(0.03) ;
	  rpc2DEtaStationGap1->GetYaxis()->SetTitle("Rpc Sector + 16 * (LPt=0,Piv=1,HPt=2) GasGap==1");
        
	  //
	  TH2 *rpc2DEtaStationGap2=new TH2I("rpc2DEtaStationGap2","rpc2DEtaStationGap2", 13, -6, 7,  16*3, 0, 16*3); 
	  sc=rpcprd_expert.regHist(rpc2DEtaStationGap2) ; 
	  rpc2DEtaStationGap2->SetFillColor(42);  
	  rpc2DEtaStationGap2->SetMarkerColor(1);  
	  rpc2DEtaStationGap2->SetMarkerStyle(21);
	  rpc2DEtaStationGap2->SetOption("COLZ");    
	  rpc2DEtaStationGap2->SetMarkerSize(0.2);
	  rpc2DEtaStationGap2->GetXaxis()->SetTitle("<--- SIDE C      Rpc Eta Station     SIDE A --->");
	  rpc2DEtaStationGap2->GetXaxis()->SetTitleSize(0.03) ;
	  rpc2DEtaStationGap2->GetYaxis()->SetTitle("Rpc Sector + 16 * (LPt=0,Piv=1,HPt=2) GasGap==2");
	 
     
	  // station trigger hits
	  // bin x: 16 * 3 = 16 sectors * 3 ( LowPt, LowPtToHighPt and HighPt trigger )
	  TH2 *rpc2DEtaStationTriggerHits=new TH2I("rpc2DEtaStationTriggerHits","rpc2DEtaStationTriggerHits", 13, -6, 7,  16*3, 0, 16*3); 
	  sc=rpcprd_shift.regHist(rpc2DEtaStationTriggerHits) ; 
	  rpc2DEtaStationTriggerHits->SetOption("COLZ");    
	  rpc2DEtaStationTriggerHits->SetMarkerSize(0.2);
	  rpc2DEtaStationTriggerHits->GetXaxis()->SetTitle("<--- SIDE C      Rpc Eta Station     SIDE A --->");
	  rpc2DEtaStationTriggerHits->GetYaxis()->SetTitle("Rpc Sector + 16 * (LPtTrigger=0,LPtToHPt trigger=1,HPtTrigger=2)");
	  rpc2DEtaStationTriggerHits->GetYaxis()->SetTitleSize(0.02);
	  rpc2DEtaStationTriggerHits->GetYaxis()->SetTitleOffset(2);
          
	  // station trigger hits SIDE A LowPt  ( includes stationEta 0 )
	  rpc2DEtaStationTriggerHits_Side_Pt.push_back( new TH2I("rpc2DEtaStationTriggerHits_BA_LowPt","rpc2DEtaStationTriggerHits_BA_LowPt", 8, 0, 8,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BA.regHist(rpc2DEtaStationTriggerHits_Side_Pt.back()) ; 
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->SetOption("COLZ") ;    
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->SetMarkerSize(0.2);
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetXaxis()->SetTitle("<--- IP      Rpc Eta Station       EC A --->");
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetTitle("") ;	  
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 1-1,"A01 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 2-1,"A02 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 3-1,"A03 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 4-1,"A04 ROside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 5-1,"A05 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 6-1,"A06 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 7-1,"A07 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 8-1,"A08 ROside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 9-1,"A09 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*10-1,"A10 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*11-1,"A11 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*12-1,"A12 ROside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*13-1,"A13 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*14-1,"A14 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*15-1,"A15 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*16-1,"A16 ROside") ;
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 1  ,"A01 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 2  ,"A02 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 3  ,"A03 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 4  ,"A04 HVside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 5  ,"A05 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 6  ,"A06 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 7  ,"A07 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 8  ,"A08 HVside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 9  ,"A09 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*10  ,"A10 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*11  ,"A11 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*12  ,"A12 HVside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*13  ,"A13 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*14  ,"A14 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*15  ,"A15 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*16  ,"A16 HVside") ;


	  // station trigger hits SIDE C LowPt
	  rpc2DEtaStationTriggerHits_Side_Pt.push_back( new TH2I("rpc2DEtaStationTriggerHits_BC_LowPt","rpc2DEtaStationTriggerHits_BC_LowPt", 7, -7, 0,  16*2, 0, 16) ); 
	  sc=rpcprd_dq_BC.regHist(rpc2DEtaStationTriggerHits_Side_Pt.back()) ; 
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->SetOption("COLZ") ;    
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->SetMarkerSize(0.2);
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetXaxis()->SetTitle("<--- EC C      Rpc Eta Station       IP --->");
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetTitle("");  
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 1-1,"C01 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 2-1,"C02 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 3-1,"C03 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 4-1,"C04 ROside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 5-1,"C05 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 6-1,"C06 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 7-1,"C07 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 8-1,"C08 ROside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 9-1,"C09 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*10-1,"C10 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*11-1,"C11 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*12-1,"C12 ROside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*13-1,"C13 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*14-1,"C14 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*15-1,"C15 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*16-1,"C16 ROside") ;
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 1  ,"C01 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 2  ,"C02 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 3  ,"C03 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 4  ,"C04 HVside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 5  ,"C05 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 6  ,"C06 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 7  ,"C07 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 8  ,"C08 HVside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 9  ,"C09 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*10  ,"C10 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*11  ,"C11 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*12  ,"C12 HVside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*13  ,"C13 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*14  ,"C14 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*15  ,"C15 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*16  ,"C16 HVside") ;

   
	  // station trigger hits SIDE A HighPt  ( includes stationEta 0 )
	  rpc2DEtaStationTriggerHits_Side_Pt.push_back( new  TH2I("rpc2DEtaStationTriggerHits_BA_HighPt","rpc2DEtaStationTriggerHits_BA_HighPt", 8, 0, 8,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BA.regHist(rpc2DEtaStationTriggerHits_Side_Pt.back()) ; 
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->SetOption("COLZ") ;    
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->SetMarkerSize(0.2);
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetXaxis()->SetTitle("<--- IP      Rpc Eta Station       EC A --->");
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetTitle("") ;	  
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 1-1,"A01 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 2-1,"A02 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 3-1,"A03 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 4-1,"A04 ROside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 5-1,"A05 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 6-1,"A06 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 7-1,"A07 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 8-1,"A08 ROside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 9-1,"A09 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*10-1,"A10 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*11-1,"A11 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*12-1,"A12 ROside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*13-1,"A13 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*14-1,"A14 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*15-1,"A15 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*16-1,"A16 ROside") ;
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 1  ,"A01 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 2  ,"A02 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 3  ,"A03 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 4  ,"A04 HVside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 5  ,"A05 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 6  ,"A06 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 7  ,"A07 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 8  ,"A08 HVside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 9  ,"A09 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*10  ,"A10 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*11  ,"A11 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*12  ,"A12 HVside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*13  ,"A13 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*14  ,"A14 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*15  ,"A15 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*16  ,"A16 HVside") ;

     
	  // station trigger hits SIDE C HighPt
	  rpc2DEtaStationTriggerHits_Side_Pt.push_back( new TH2I("rpc2DEtaStationTriggerHits_BC_HighPt","rpc2DEtaStationTriggerHits_BC_HighPt", 7, -7, 0,  16*2, 0, 16) ); 
	  sc=rpcprd_dq_BC.regHist(rpc2DEtaStationTriggerHits_Side_Pt.back()) ; 
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->SetOption("COLZ") ;    
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->SetMarkerSize(0.2);
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetXaxis()->SetTitle("<--- EC C      Rpc Eta Station       IP --->");
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetTitle("");  
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 1-1,"C01 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 2-1,"C02 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 3-1,"C03 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 4-1,"C04 ROside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 5-1,"C05 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 6-1,"C06 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 7-1,"C07 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 8-1,"C08 ROside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 9-1,"C09 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*10-1,"C10 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*11-1,"C11 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*12-1,"C12 ROside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*13-1,"C13 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*14-1,"C14 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*15-1,"C15 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*16-1,"C16 ROside") ;
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 1  ,"C01 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 2  ,"C02 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 3  ,"C03 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 4  ,"C04 HVside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 5  ,"C05 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 6  ,"C06 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 7  ,"C07 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 8  ,"C08 HVside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2* 9  ,"C09 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*10  ,"C10 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*11  ,"C11 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*12  ,"C12 HVside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*13  ,"C13 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*14  ,"C14 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*15  ,"C15 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt.back()->GetYaxis()->SetBinLabel(2*16  ,"C16 HVside") ;

       
           
	  // station trigger hits SIDE A LowPt  ( includes stationEta 0 )
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.push_back( new TH2F("rpc2DEtaStationTriggerHits_BA_LowPt_norm","rpc2DEtaStationTriggerHits_BA_LowPt_norm", 8, 0, 8,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BA.regHist(rpc2DEtaStationTriggerHits_Side_Pt_norm.back()) ; 
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->SetOption("COLZ") ;    
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->SetMarkerSize(0.2);
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetXaxis()->SetTitle("<--- IP      Rpc Eta Station       EC A --->");
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetTitle("") ;	  
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 1-1,"A01 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 2-1,"A02 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 3-1,"A03 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 4-1,"A04 ROside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 5-1,"A05 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 6-1,"A06 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 7-1,"A07 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 8-1,"A08 ROside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 9-1,"A09 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*10-1,"A10 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*11-1,"A11 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*12-1,"A12 ROside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*13-1,"A13 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*14-1,"A14 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*15-1,"A15 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*16-1,"A16 ROside") ;
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 1  ,"A01 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 2  ,"A02 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 3  ,"A03 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 4  ,"A04 HVside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 5  ,"A05 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 6  ,"A06 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 7  ,"A07 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 8  ,"A08 HVside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 9  ,"A09 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*10  ,"A10 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*11  ,"A11 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*12  ,"A12 HVside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*13  ,"A13 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*14  ,"A14 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*15  ,"A15 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*16  ,"A16 HVside") ;

	  // station trigger hits SIDE C LowPt
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.push_back( new TH2F("rpc2DEtaStationTriggerHits_BC_LowPt_norm","rpc2DEtaStationTriggerHits_BC_LowPt_norm", 7, -7, 0,  16*2, 0, 16) ); 
	  sc=rpcprd_dq_BC.regHist(rpc2DEtaStationTriggerHits_Side_Pt_norm.back()) ; 
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->SetOption("COLZ") ;    
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->SetMarkerSize(0.2);
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetXaxis()->SetTitle("<--- EC C      Rpc Eta Station       IP --->");
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetTitle("");	  
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 1-1,"C01 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 2-1,"C02 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 3-1,"C03 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 4-1,"C04 ROside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 5-1,"C05 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 6-1,"C06 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 7-1,"C07 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 8-1,"C08 ROside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 9-1,"C09 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*10-1,"C10 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*11-1,"C11 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*12-1,"C12 ROside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*13-1,"C13 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*14-1,"C14 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*15-1,"C15 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*16-1,"C16 ROside") ;
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 1  ,"C01 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 2  ,"C02 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 3  ,"C03 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 4  ,"C04 HVside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 5  ,"C05 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 6  ,"C06 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 7  ,"C07 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 8  ,"C08 HVside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 9  ,"C09 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*10  ,"C10 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*11  ,"C11 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*12  ,"C12 HVside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*13  ,"C13 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*14  ,"C14 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*15  ,"C15 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*16  ,"C16 HVside") ;

   
	  // station trigger hits SIDE A HighPt  ( includes stationEta 0 )
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.push_back( new  TH2F("rpc2DEtaStationTriggerHits_BA_HighPt_norm","rpc2DEtaStationTriggerHits_BA_HighPt_norm", 8, 0, 8,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BA.regHist(rpc2DEtaStationTriggerHits_Side_Pt_norm.back()) ; 
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->SetOption("COLZ") ;    
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->SetMarkerSize(0.2);
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetXaxis()->SetTitle("<--- IP      Rpc Eta Station       EC A --->");
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetTitle("") ;	  
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 1-1,"A01 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 2-1,"A02 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 3-1,"A03 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 4-1,"A04 ROside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 5-1,"A05 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 6-1,"A06 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 7-1,"A07 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 8-1,"A08 ROside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 9-1,"A09 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*10-1,"A10 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*11-1,"A11 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*12-1,"A12 ROside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*13-1,"A13 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*14-1,"A14 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*15-1,"A15 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*16-1,"A16 ROside") ;
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 1  ,"A01 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 2  ,"A02 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 3  ,"A03 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 4  ,"A04 HVside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 5  ,"A05 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 6  ,"A06 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 7  ,"A07 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 8  ,"A08 HVside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 9  ,"A09 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*10  ,"A10 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*11  ,"A11 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*12  ,"A12 HVside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*13  ,"A13 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*14  ,"A14 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*15  ,"A15 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*16  ,"A16 HVside") ;

     
	  // station trigger hits SIDE C HighPt
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.push_back( new TH2F("rpc2DEtaStationTriggerHits_BC_HighPt_norm","rpc2DEtaStationTriggerHits_BC_HighPt_norm", 7, -7, 0,  16*2, 0, 16) ); 
	  sc=rpcprd_dq_BC.regHist(rpc2DEtaStationTriggerHits_Side_Pt_norm.back()) ; 
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->SetOption("COLZ") ;    
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->SetMarkerSize(0.2);
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetXaxis()->SetTitle("<--- EC C      Rpc Eta Station       IP --->");
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetTitle("");	  
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 1-1,"C01 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 2-1,"C02 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 3-1,"C03 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 4-1,"C04 ROside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 5-1,"C05 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 6-1,"C06 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 7-1,"C07 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 8-1,"C08 ROside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 9-1,"C09 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*10-1,"C10 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*11-1,"C11 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*12-1,"C12 ROside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*13-1,"C13 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*14-1,"C14 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*15-1,"C15 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*16-1,"C16 ROside") ;
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 1  ,"C01 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 2  ,"C02 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 3  ,"C03 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 4  ,"C04 HVside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 5  ,"C05 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 6  ,"C06 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 7  ,"C07 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 8  ,"C08 HVside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2* 9  ,"C09 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*10  ,"C10 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*11  ,"C11 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*12  ,"C12 HVside") ; 
	  rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*13  ,"C13 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*14  ,"C14 HVside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*15  ,"C15 ROside") ; rpc2DEtaStationTriggerHits_Side_Pt_norm.back()->GetYaxis()->SetBinLabel(2*16  ,"C16 HVside") ;

        
	  //DQ from Mauro
	 
	  // 2D panels Phi trigger hits LowPt 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DPhiPanelTriggerHits_LowPt","rpc2DPhiPanelTriggerHits_LowPt", 26, -13, 13,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("<--- Side C      Rpc Phi Panel       Side A --->");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"A01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"A02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"A03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"A04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"A05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"A06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"A07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"A08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"A09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"A10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"A11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"A12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"A13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"A14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"A15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"A16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"A01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"A02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"A03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"A04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"A05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"A06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"A07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"A08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"A09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"A10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"A11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"A12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"A13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"A14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"A15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"A16 HVside") ;

	
	  // 2D panels Phi trigger hits HighPt 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DPhiPanelTriggerHits_HighPt","rpc2DPhiPanelTriggerHits_HighPt", 26, -13, 13,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("<--- Side C      Rpc Phi Panel       Side A --->");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"A01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"A02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"A03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"A04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"A05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"A06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"A07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"A08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"A09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"A10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"A11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"A12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"A13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"A14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"A15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"A16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"A01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"A02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"A03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"A04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"A05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"A06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"A07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"A08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"A09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"A10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"A11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"A12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"A13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"A14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"A15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"A16 HVside") ;

	
	  // 2D panels Eta trigger hits LowPt 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DEtaPanelTriggerHits_LowPt","rpc2DEtaPanelTriggerHits_LowPt", 26, -13, 13,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("<--- Side C      Rpc Eta Panel       Side A --->");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"A01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"A02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"A03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"A04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"A05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"A06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"A07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"A08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"A09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"A10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"A11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"A12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"A13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"A14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"A15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"A16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"A01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"A02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"A03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"A04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"A05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"A06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"A07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"A08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"A09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"A10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"A11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"A12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"A13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"A14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"A15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"A16 HVside") ;

	
	  // 2D panels Eta trigger hits HighPt 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DEtaPanelTriggerHits_HighPt","rpc2DEtaPanelTriggerHits_HighPt", 26, -13, 13,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("<--- Side C      Rpc Eta Panel       Side A --->");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"A01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"A02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"A03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"A04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"A05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"A06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"A07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"A08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"A09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"A10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"A11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"A12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"A13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"A14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"A15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"A16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"A01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"A02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"A03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"A04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"A05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"A06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"A07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"A08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"A09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"A10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"A11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"A12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"A13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"A14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"A15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"A16 HVside") ;

	  // 2D panels Phi trigger hits LowPt_BA 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DPhiPanelTriggerHits_LowPt_BA","rpc2DPhiPanelTriggerHits_LowPt_BA", 13, 0, 13,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BA_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Phi Panel  Side A");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"A01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"A02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"A03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"A04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"A05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"A06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"A07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"A08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"A09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"A10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"A11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"A12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"A13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"A14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"A15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"A16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"A01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"A02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"A03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"A04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"A05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"A06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"A07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"A08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"A09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"A10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"A11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"A12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"A13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"A14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"A15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"A16 HVside") ;

	
	  // 2D panels Phi trigger hits HighPt_BA 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DPhiPanelTriggerHits_HighPt_BA","rpc2DPhiPanelTriggerHits_HighPt_BA", 13, 0, 13,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BA_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Phi Panel Side A");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"A01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"A02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"A03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"A04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"A05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"A06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"A07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"A08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"A09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"A10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"A11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"A12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"A13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"A14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"A15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"A16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"A01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"A02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"A03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"A04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"A05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"A06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"A07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"A08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"A09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"A10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"A11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"A12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"A13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"A14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"A15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"A16 HVside") ;

	
	  // 2D panels Eta trigger hits LowPt_BA 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DEtaPanelTriggerHits_LowPt_BA","rpc2DEtaPanelTriggerHits_LowPt_BA", 13, 0, 13,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BA_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Eta Panel Side A");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"A01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"A02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"A03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"A04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"A05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"A06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"A07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"A08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"A09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"A10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"A11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"A12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"A13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"A14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"A15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"A16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"A01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"A02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"A03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"A04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"A05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"A06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"A07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"A08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"A09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"A10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"A11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"A12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"A13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"A14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"A15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"A16 HVside") ;

	
	  // 2D panels Eta trigger hits HighPt_BA 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DEtaPanelTriggerHits_HighPt_BA","rpc2DEtaPanelTriggerHits_HighPt_BA", 13, 0, 13,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BA_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Eta Panel Side A");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"A01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"A02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"A03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"A04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"A05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"A06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"A07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"A08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"A09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"A10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"A11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"A12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"A13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"A14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"A15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"A16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"A01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"A02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"A03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"A04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"A05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"A06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"A07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"A08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"A09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"A10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"A11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"A12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"A13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"A14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"A15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"A16 HVside") ;
          
	  // 2D panels Phi trigger hits LowPt_BC 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DPhiPanelTriggerHits_LowPt_BC","rpc2DPhiPanelTriggerHits_LowPt_BC", 13, 0, 13,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BC_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Phi Panel Side C");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"C01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"C02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"C03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"C04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"C05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"C06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"C07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"C08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"C09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"C10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"C11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"C12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"C13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"C14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"C15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"C16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"C01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"C02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"C03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"C04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"C05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"C06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"C07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"C08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"C09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"C10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"C11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"C12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"C13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"C14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"C15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"C16 HVside") ;

	
	  // 2D panels Phi trigger hits HighPt_BC 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DPhiPanelTriggerHits_HighPt_BC","rpc2DPhiPanelTriggerHits_HighPt_BC", 13, 0, 13,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BC_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Phi Panel Side C");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"C01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"C02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"C03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"C04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"C05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"C06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"C07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"C08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"C09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"C10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"C11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"C12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"C13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"C14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"C15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"C16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"C01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"C02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"C03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"C04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"C05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"C06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"C07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"C08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"C09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"C10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"C11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"C12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"C13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"C14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"C15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"C16 HVside") ;

	
	  // 2D panels Eta trigger hits LowPt_BC 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DEtaPanelTriggerHits_LowPt_BC","rpc2DEtaPanelTriggerHits_LowPt_BC", 13, 0, 13,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BC_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Phi Panel Side C");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"C01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"C02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"C03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"C04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"C05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"C06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"C07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"C08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"C09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"C10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"C11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"C12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"C13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"C14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"C15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"C16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"C01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"C02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"C03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"C04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"C05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"C06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"C07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"C08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"C09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"C10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"C11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"C12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"C13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"C14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"C15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"C16 HVside") ;

	
	  // 2D panels Eta trigger hits HighPt_BC 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DEtaPanelTriggerHits_HighPt_BC","rpc2DEtaPanelTriggerHits_HighPt_BC", 13, 0, 13,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BC_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Phi Panel Side C");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"C01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"C02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"C03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"C04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"C05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"C06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"C07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"C08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"C09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"C10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"C11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"C12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"C13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"C14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"C15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"C16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"C01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"C02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"C03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"C04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"C05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"C06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"C07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"C08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"C09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"C10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"C11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"C12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"C13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"C14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"C15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"C16 HVside") ;
          
          //eta-phi trigger
	  // 2D panels PhiEndEta trigger hits LowPt_BA 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DPhiEndEtaPanelTriggerHits_LowPt_BA","rpc2DPhiEndEtaPanelTriggerHits_LowPt_BA", 13, 0, 13,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BA_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Phi&Phi Panel Side C");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"A01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"A02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"A03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"A04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"A05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"A06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"A07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"A08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"A09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"A10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"A11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"A12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"A13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"A14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"A15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"A16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"A01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"A02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"A03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"A04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"A05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"A06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"A07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"A08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"A09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"A10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"A11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"A12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"A13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"A14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"A15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"A16 HVside") ;

	
	  // 2D panels PhiEndEta trigger hits HighPt_BA 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DPhiEndEtaPanelTriggerHits_HighPt_BA","rpc2DPhiEndEtaPanelTriggerHits_HighPt_BA", 13, 0, 13,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BA_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Phi&End Panel Side A");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"A01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"A02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"A03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"A04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"A05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"A06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"A07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"A08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"A09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"A10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"A11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"A12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"A13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"A14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"A15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"A16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"A01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"A02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"A03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"A04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"A05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"A06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"A07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"A08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"A09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"A10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"A11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"A12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"A13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"A14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"A15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"A16 HVside") ;

	
	  // 2D panels PhiEndEta trigger hits LowPt_BC 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DPhiEndEtaPanelTriggerHits_LowPt_BC","rpc2DPhiEndEtaPanelTriggerHits_LowPt_BC", 13, 0, 13,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BC_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Phi&Phi Panel Side A");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"C01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"C02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"A03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"A04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"C05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"C06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"A07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"A08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"C09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"C10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"A11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"A12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"C13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"C14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"A15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"A16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"C01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"C02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"A03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"A04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"C05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"C06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"A07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"A08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"C09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"C10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"A11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"A12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"C13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"C14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"A15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"A16 HVside") ;

	
	  // 2D panels PhiEndEta trigger hits HighPt_BC 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DPhiEndEtaPanelTriggerHits_HighPt_BC","rpc2DPhiEndEtaPanelTriggerHits_HighPt_BC", 13, 0, 13,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BC_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Phi&Phi Panel Side C");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"C01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"C02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"A03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"A04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"C05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"C06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"A07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"A08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"C09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"C10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"A11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"A12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"C13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"C14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"A15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"A16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"C01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"C02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"A03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"A04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"C05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"C06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"A07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"A08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"C09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"C10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"A11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"A12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"C13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"C14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"A15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"A16 HVside") ;
	  
	  ////// End Trigger Plots
	
	  // 2D panels Phi  hits LowPt0_BA 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DPhiPanelHits_LowPt0_BA","rpc2DPhiPanelHits_LowPt0_BA", 12, 0, 12,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BA_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Phi Panel Side A");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"A01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"A02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"A03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"A04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"A05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"A06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"A07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"A08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"A09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"A10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"A11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"A12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"A13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"A14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"A15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"A16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"A01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"A02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"A03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"A04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"A05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"A06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"A07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"A08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"A09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"A10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"A11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"A12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"A13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"A14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"A15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"A16 HVside") ;

		
	  // 2D panels Phi  hits LowPt1_BA 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DPhiPanelHits_LowPt1_BA","rpc2DPhiPanelHits_LowPt1_BA", 12, 0, 12,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BA_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Phi Panel Side A");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"A01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"A02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"A03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"A04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"A05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"A06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"A07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"A08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"A09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"A10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"A11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"A12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"A13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"A14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"A15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"A16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"A01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"A02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"A03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"A04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"A05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"A06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"A07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"A08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"A09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"A10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"A11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"A12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"A13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"A14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"A15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"A16 HVside") ;

	
	  // 2D panels Eta  hits LowPt0_BA 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DEtaPanelHits_LowPt0_BA","rpc2DEtaPanelHits_LowPt0_BA", 12, 0, 12,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BA_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Eta Panel Side A");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"A01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"A02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"A03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"A04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"A05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"A06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"A07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"A08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"A09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"A10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"A11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"A12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"A13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"A14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"A15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"A16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"A01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"A02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"A03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"A04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"A05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"A06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"A07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"A08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"A09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"A10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"A11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"A12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"A13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"A14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"A15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"A16 HVside") ;

		
	  // 2D panels Eta  hits LowPt1_BA 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DEtaPanelHits_LowPt1_BA","rpc2DEtaPanelHits_LowPt1_BA", 12, 0, 12,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BA_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Eta Panel Side A");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"A01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"A02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"A03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"A04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"A05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"A06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"A07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"A08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"A09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"A10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"A11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"A12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"A13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"A14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"A15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"A16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"A01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"A02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"A03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"A04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"A05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"A06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"A07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"A08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"A09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"A10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"A11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"A12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"A13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"A14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"A15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"A16 HVside") ;


	  // 2D panels Phi  hits Pivot0_BA 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DPhiPanelHits_Pivot0_BA","rpc2DPhiPanelHits_Pivot0_BA", 13, 0, 13,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BA_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Phi Panel Side A");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"A01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"A02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"A03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"A04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"A05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"A06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"A07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"A08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"A09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"A10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"A11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"A12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"A13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"A14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"A15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"A16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"A01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"A02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"A03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"A04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"A05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"A06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"A07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"A08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"A09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"A10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"A11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"A12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"A13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"A14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"A15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"A16 HVside") ;

		
	  // 2D panels Phi  hits Pivot1_BA 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DPhiPanelHits_Pivot1_BA","rpc2DPhiPanelHits_Pivot1_BA", 13, 0, 13,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BA_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Phi Panel Side A");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"A01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"A02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"A03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"A04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"A05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"A06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"A07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"A08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"A09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"A10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"A11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"A12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"A13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"A14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"A15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"A16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"A01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"A02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"A03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"A04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"A05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"A06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"A07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"A08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"A09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"A10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"A11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"A12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"A13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"A14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"A15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"A16 HVside") ;


	  // 2D panels Eta  hits Pivot0_BA 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DEtaPanelHits_Pivot0_BA","rpc2DEtaPanelHits_Pivot0_BA", 13, 0, 13,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BA_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Eta Panel Side A");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"A01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"A02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"A03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"A04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"A05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"A06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"A07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"A08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"A09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"A10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"A11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"A12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"A13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"A14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"A15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"A16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"A01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"A02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"A03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"A04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"A05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"A06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"A07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"A08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"A09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"A10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"A11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"A12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"A13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"A14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"A15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"A16 HVside") ;

		
	  // 2D panels Eta  hits Pivot1_BA 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DEtaPanelHits_Pivot1_BA","rpc2DEtaPanelHits_Pivot1_BA", 13, 0, 13,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BA_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Eta Panel Side A");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"A01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"A02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"A03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"A04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"A05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"A06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"A07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"A08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"A09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"A10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"A11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"A12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"A13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"A14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"A15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"A16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"A01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"A02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"A03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"A04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"A05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"A06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"A07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"A08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"A09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"A10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"A11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"A12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"A13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"A14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"A15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"A16 HVside") ;

			
	  // 2D panels Phi  hits HighPt0_BA 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DPhiPanelHits_HighPt0_BA","rpc2DPhiPanelHits_HighPt0_BA", 13, 0, 13,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BA_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Phi Panel Side A");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"A01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"A02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"A03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"A04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"A05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"A06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"A07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"A08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"A09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"A10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"A11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"A12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"A13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"A14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"A15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"A16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"A01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"A02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"A03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"A04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"A05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"A06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"A07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"A08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"A09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"A10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"A11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"A12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"A13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"A14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"A15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"A16 HVside") ;

		
	  // 2D panels Phi  hits HighPt1_BA 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DPhiPanelHits_HighPt1_BA","rpc2DPhiPanelHits_HighPt1_BA", 13, 0, 13,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BA_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Phi Panel Side A");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"A01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"A02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"A03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"A04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"A05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"A06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"A07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"A08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"A09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"A10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"A11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"A12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"A13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"A14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"A15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"A16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"A01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"A02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"A03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"A04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"A05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"A06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"A07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"A08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"A09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"A10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"A11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"A12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"A13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"A14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"A15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"A16 HVside") ;
	
			
	  // 2D panels Eta  hits HighPt0_BA 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DEtaPanelHits_HighPt0_BA","rpc2DEtaPanelHits_HighPt0_BA", 13, 0, 13,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BA_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Eta Panel Side A");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"A01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"A02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"A03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"A04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"A05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"A06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"A07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"A08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"A09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"A10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"A11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"A12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"A13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"A14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"A15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"A16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"A01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"A02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"A03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"A04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"A05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"A06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"A07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"A08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"A09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"A10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"A11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"A12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"A13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"A14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"A15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"A16 HVside") ;

		
	  // 2D panels Eta  hits HighPt1_BA 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DEtaPanelHits_HighPt1_BA","rpc2DEtaPanelHits_HighPt1_BA", 13, 0, 13,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BA_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Eta Panel Side A");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"A01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"A02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"A03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"A04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"A05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"A06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"A07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"A08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"A09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"A10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"A11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"A12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"A13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"A14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"A15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"A16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"A01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"A02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"A03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"A04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"A05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"A06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"A07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"A08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"A09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"A10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"A11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"A12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"A13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"A14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"A15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"A16 HVside") ;


	  // 2D panels Phi  hits LowPt0_BC 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DPhiPanelHits_LowPt0_BC","rpc2DPhiPanelHits_LowPt0_BC", 12, 0, 12,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BC_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Phi Panel Side C");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"C01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"C02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"C03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"C04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"C05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"C06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"C07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"C08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"C09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"C10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"C11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"C12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"C13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"C14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"C15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"C16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"C01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"C02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"C03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"C04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"C05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"C06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"C07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"C08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"C09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"C10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"C11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"C12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"C13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"C14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"C15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"C16 HVside") ;

		
	  // 2D panels Phi  hits LowPt1_BC 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DPhiPanelHits_LowPt1_BC","rpc2DPhiPanelHits_LowPt1_BC", 12, 0, 12,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BC_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Phi Panel Side C");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"C01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"C02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"C03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"C04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"C05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"C06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"C07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"C08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"C09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"C10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"C11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"C12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"C13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"C14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"C15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"C16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"C01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"C02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"C03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"C04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"C05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"C06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"C07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"C08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"C09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"C10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"C11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"C12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"C13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"C14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"C15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"C16 HVside") ;

	
	  // 2D panels Eta  hits LowPt0_BC 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DEtaPanelHits_LowPt0_BC","rpc2DEtaPanelHits_LowPt0_BC", 12, 0, 12,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BC_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Eta Panel Side C");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"C01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"C02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"C03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"C04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"C05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"C06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"C07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"C08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"C09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"C10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"C11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"C12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"C13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"C14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"C15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"C16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"C01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"C02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"C03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"C04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"C05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"C06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"C07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"C08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"C09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"C10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"C11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"C12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"C13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"C14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"C15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"C16 HVside") ;

		
	  // 2D panels Eta  hits LowPt1_BC 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DEtaPanelHits_LowPt1_BC","rpc2DEtaPanelHits_LowPt1_BC", 12, 0, 12,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BC_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Eta Panel Side C");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"C01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"C02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"C03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"C04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"C05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"C06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"C07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"C08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"C09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"C10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"C11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"C12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"C13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"C14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"C15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"C16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"C01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"C02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"C03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"C04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"C05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"C06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"C07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"C08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"C09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"C10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"C11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"C12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"C13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"C14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"C15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"C16 HVside") ;


	  // 2D panels Phi  hits Pivot0_BC 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DPhiPanelHits_Pivot0_BC","rpc2DPhiPanelHits_Pivot0_BC", 13, 0, 13,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BC_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Phi Panel Side C");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"C01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"C02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"C03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"C04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"C05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"C06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"C07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"C08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"C09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"C10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"C11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"C12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"C13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"C14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"C15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"C16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"C01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"C02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"C03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"C04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"C05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"C06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"C07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"C08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"C09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"C10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"C11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"C12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"C13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"C14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"C15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"C16 HVside") ;

		
	  // 2D panels Phi  hits Pivot1_BC 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DPhiPanelHits_Pivot1_BC","rpc2DPhiPanelHits_Pivot1_BC", 13, 0, 13,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BC_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Phi Panel Side C");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"C01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"C02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"C03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"C04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"C05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"C06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"C07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"C08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"C09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"C10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"C11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"C12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"C13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"C14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"C15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"C16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"C01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"C02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"C03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"C04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"C05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"C06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"C07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"C08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"C09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"C10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"C11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"C12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"C13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"C14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"C15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"C16 HVside") ;


	  // 2D panels Eta  hits Pivot0_BC 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DEtaPanelHits_Pivot0_BC","rpc2DEtaPanelHits_Pivot0_BC", 13, 0, 13,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BC_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Eta Panel Side C");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"C01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"C02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"C03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"C04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"C05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"C06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"C07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"C08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"C09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"C10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"C11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"C12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"C13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"C14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"C15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"C16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"C01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"C02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"C03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"C04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"C05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"C06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"C07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"C08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"C09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"C10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"C11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"C12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"C13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"C14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"C15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"C16 HVside") ;

		
	  // 2D panels Eta  hits Pivot1_BC 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DEtaPanelHits_Pivot1_BC","rpc2DEtaPanelHits_Pivot1_BC", 13, 0, 13,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BC_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Eta Panel Side C");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"C01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"C02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"C03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"C04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"C05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"C06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"C07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"C08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"C09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"C10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"C11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"C12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"C13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"C14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"C15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"C16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"C01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"C02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"C03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"C04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"C05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"C06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"C07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"C08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"C09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"C10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"C11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"C12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"C13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"C14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"C15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"C16 HVside") ;

			
	  // 2D panels Phi  hits HighPt0_BC 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DPhiPanelHits_HighPt0_BC","rpc2DPhiPanelHits_HighPt0_BC", 13, 0, 13,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BC_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Phi Panel Side C");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"C01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"C02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"C03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"C04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"C05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"C06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"C07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"C08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"C09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"C10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"C11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"C12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"C13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"C14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"C15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"C16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"C01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"C02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"C03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"C04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"C05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"C06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"C07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"C08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"C09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"C10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"C11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"C12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"C13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"C14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"C15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"C16 HVside") ;

		
	  // 2D panels Phi  hits HighPt1_BC 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DPhiPanelHits_HighPt1_BC","rpc2DPhiPanelHits_HighPt1_BC", 13, 0, 13,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BC_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Phi Panel Side C");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"C01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"C02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"C03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"C04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"C05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"C06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"C07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"C08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"C09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"C10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"C11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"C12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"C13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"C14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"C15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"C16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"C01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"C02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"C03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"C04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"C05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"C06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"C07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"C08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"C09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"C10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"C11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"C12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"C13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"C14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"C15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"C16 HVside") ;
	
			
	  // 2D panels Eta  hits HighPt0_BC 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DEtaPanelHits_HighPt0_BC","rpc2DEtaPanelHits_HighPt0_BC", 13, 0, 13,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BC_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Eta Panel Side C");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"C01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"C02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"C03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"C04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"C05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"C06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"C07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"C08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"C09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"C10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"C11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"C12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"C13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"C14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"C15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"C16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"C01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"C02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"C03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"C04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"C05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"C06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"C07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"C08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"C09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"C10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"C11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"C12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"C13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"C14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"C15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"C16 HVside") ;

		
	  // 2D panels Eta  hits HighPt1_BC 
	  rpc2DPanelHits.push_back( new TH2I("rpc2DEtaPanelHits_HighPt1_BC","rpc2DEtaPanelHits_HighPt1_BC", 13, 0, 13,  16*2, 0, 16 ) ); 
	  sc=rpcprd_dq_BC_Panel.regHist(rpc2DPanelHits.back()) ; 
	  rpc2DPanelHits.back()->SetOption("COLZ") ;    
	  rpc2DPanelHits.back()->SetMarkerSize(0.2);
	  rpc2DPanelHits.back()->GetXaxis()->SetTitle("Rpc Eta Panel Side C");
	  rpc2DPanelHits.back()->GetYaxis()->SetTitle("") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1-1,"C01 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2-1,"C02 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3-1,"C03 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4-1,"C04 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5-1,"C05 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6-1,"C06 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7-1,"C07 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8-1,"C08 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9-1,"C09 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10-1,"C10 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11-1,"C11 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12-1,"C12 ROside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13-1,"C13 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14-1,"C14 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15-1,"C15 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16-1,"C16 ROside") ;
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 1  ,"C01 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 2  ,"C02 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 3  ,"C03 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 4  ,"C04 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 5  ,"C05 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 6  ,"C06 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 7  ,"C07 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 8  ,"C08 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2* 9  ,"C09 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*10  ,"C10 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*11  ,"C11 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*12  ,"C12 HVside") ; 
	  rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*13  ,"C13 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*14  ,"C14 HVside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*15  ,"C15 ROside") ; rpc2DPanelHits.back()->GetYaxis()->SetBinLabel(2*16  ,"C16 HVside") ;

          
	  //DQ vs LB from Mauro
	 
	  // 1DvsLB panels Phi trigger hits LowPt 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBPhiPanelTriggerHits_LowPt","rpc1DvsLBPhiPanelTriggerHits_LowPt", m_LB_Nbins, 0, m_LBmax,  2*(187+188), -187,188  ) ); 
	  sc=rpcprd_dq_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;
	
	  // 1DvsLB panels Phi trigger hits HighPt 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBPhiPanelTriggerHits_HighPt","rpc1DvsLBPhiPanelTriggerHits_HighPt", m_LB_Nbins, 0, m_LBmax, 2*(187+188), -187,188  ) );    
	  sc=rpcprd_dq_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;
	
	  // 1DvsLB panels Eta trigger hits LowPt 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBEtaPanelTriggerHits_LowPt","rpc1DvsLBEtaPanelTriggerHits_LowPt", m_LB_Nbins, 0, m_LBmax,  2*(187+188), -187,188  ) );   
	  sc=rpcprd_dq_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;
	
	  // 1DvsLB panels Eta trigger hits HighPt 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBEtaPanelTriggerHits_HighPt","rpc1DvsLBEtaPanelTriggerHits_HighPt", m_LB_Nbins, 0, m_LBmax,  2*(187+188), -187,188  ) );   
	  sc=rpcprd_dq_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;


	 
	  // 1DvsLB panels Phi trigger hits LowPt_BA 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBPhiPanelTriggerHits_LowPt_BA","rpc1DvsLBPhiPanelTriggerHits_LowPt_BA", m_LB_Nbins, 0, m_LBmax,  2*188, 0, 188 ) ); 
	  sc=rpcprd_dq_BA_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;
	
	  // 1DvsLB panels Phi trigger hits HighPt_BA 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBPhiPanelTriggerHits_HighPt_BA","rpc1DvsLBPhiPanelTriggerHits_HighPt_BA", m_LB_Nbins, 0, m_LBmax,  2*188, 0, 188 ) );    
	  sc=rpcprd_dq_BA_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;
	
	  // 1DvsLB panels Eta trigger hits LowPt_BA 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBEtaPanelTriggerHits_LowPt_BA","rpc1DvsLBEtaPanelTriggerHits_LowPt_BA", m_LB_Nbins, 0, m_LBmax,   2*188, 0, 188 ) );   
	  sc=rpcprd_dq_BA_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;
	
	  // 1DvsLB panels Eta trigger hits HighPt_BA 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBEtaPanelTriggerHits_HighPt_BA","rpc1DvsLBEtaPanelTriggerHits_HighPt_BA", m_LB_Nbins, 0, m_LBmax,   2*188, 0, 188 ) );   
	  sc=rpcprd_dq_BA_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;


	 
	  // 1DvsLB panels Phi trigger hits LowPt_BC 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBPhiPanelTriggerHits_LowPt_BC","rpc1DvsLBPhiPanelTriggerHits_LowPt_BC", m_LB_Nbins, 0, m_LBmax,  2*187, 0, 187 ) ); 
	  sc=rpcprd_dq_BC_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;
	
	  // 1DvsLB panels Phi trigger hits HighPt_BC 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBPhiPanelTriggerHits_HighPt_BC","rpc1DvsLBPhiPanelTriggerHits_HighPt_BC", m_LB_Nbins, 0, m_LBmax,  2*187, 0, 187 ) );      
	  sc=rpcprd_dq_BC_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;
	
	  // 1DvsLB panels Eta trigger hits LowPt_BC 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBEtaPanelTriggerHits_LowPt_BC","rpc1DvsLBEtaPanelTriggerHits_LowPt_BC", m_LB_Nbins, 0, m_LBmax,  2*187, 0, 187 ) );     
	  sc=rpcprd_dq_BC_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;
	
	  // 1DvsLB panels Eta trigger hits HighPt_BC 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBEtaPanelTriggerHits_HighPt_BC","rpc1DvsLBEtaPanelTriggerHits_HighPt_BC", m_LB_Nbins, 0, m_LBmax,  2*187, 0, 187 ) );     
	  sc=rpcprd_dq_BC_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;


	 
	  // 1DvsLB panels PhiEta trigger hits LowPt_BA 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBPhiEtaPanelTriggerHits_LowPt_BA","rpc1DvsLBPhiEtaPanelTriggerHits_LowPt_BA", m_LB_Nbins, 0, m_LBmax,  2*188, 0, 188 ) ); 
	  sc=rpcprd_dq_BA_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;
	
	  // 1DvsLB panels PhiEta trigger hits HighPt_BA 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBPhiEtaPanelTriggerHits_HighPt_BA","rpc1DvsLBPhiEtaPanelTriggerHits_HighPt_BA", m_LB_Nbins, 0, m_LBmax,  2*188, 0, 188 ) );      
	  sc=rpcprd_dq_BA_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;
	
	  // 1DvsLB panels PhiEta trigger hits LowPt_BC 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBPhiEtaPanelTriggerHits_LowPt_BC","rpc1DvsLBPhiEtaPanelTriggerHits_LowPt_BC", m_LB_Nbins, 0, m_LBmax,  2*187, 0, 187 ) );     
	  sc=rpcprd_dq_BC_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;
	
	  // 1DvsLB panels PhiEta trigger hits HighPt_BC 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBPhiEtaPanelTriggerHits_HighPt_BC","rpc1DvsLBPhiEtaPanelTriggerHits_HighPt_BC", m_LB_Nbins, 0, m_LBmax,  2*187, 0, 187 ) );     
	  sc=rpcprd_dq_BC_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;	  
//end trigger plot
	
	  // 1DvsLB panels Phi  hits LowPt0_BA 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBPhiPanelHits_LowPt0_BA","rpc1DvsLBPhiPanelHits_LowPt0_BA", m_LB_Nbins, 0, m_LBmax,  2*158, 0, 158 ) ); 
	  sc=rpcprd_dq_BA_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;
		
	  // 1DvsLB panels Phi  hits LowPt1_BA 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBPhiPanelHits_LowPt1_BA","rpc1DvsLBPhiPanelHits_LowPt1_BA", m_LB_Nbins, 0, m_LBmax,  2*158, 0, 158 ) ); 
	  sc=rpcprd_dq_BA_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;
	
	  // 1DvsLB panels Eta  hits LowPt0_BA 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBEtaPanelHits_LowPt0_BA","rpc1DvsLBEtaPanelHits_LowPt0_BA", m_LB_Nbins, 0, m_LBmax,   2*158, 0, 158 ) ); 
	  sc=rpcprd_dq_BA_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;
		
	  // 1DvsLB panels Eta  hits LowPt1_BA 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBEtaPanelHits_LowPt1_BA","rpc1DvsLBEtaPanelHits_LowPt1_BA", m_LB_Nbins, 0, m_LBmax,   2*158, 0, 158 ) ); 
	  sc=rpcprd_dq_BA_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;

	  // 1DvsLB panels Phi  hits Pivot0_BA 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBPhiPanelHits_Pivot0_BA","rpc1DvsLBPhiPanelHits_Pivot0_BA", m_LB_Nbins, 0, m_LBmax,  2*188, 0, 188 ) );  
	  sc=rpcprd_dq_BA_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;
		
	  // 1DvsLB panels Phi  hits Pivot1_BA 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBPhiPanelHits_Pivot1_BA","rpc1DvsLBPhiPanelHits_Pivot1_BA", m_LB_Nbins, 0, m_LBmax,  2*188, 0, 188 ) );  
	  sc=rpcprd_dq_BA_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;

	  // 1DvsLB panels Eta  hits Pivot0_BA 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBEtaPanelHits_Pivot0_BA","rpc1DvsLBEtaPanelHits_Pivot0_BA", m_LB_Nbins, 0, m_LBmax,   2*188, 0, 188 ) ); 
	  sc=rpcprd_dq_BA_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;
		
	  // 1DvsLB panels Eta  hits Pivot1_BA 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBEtaPanelHits_Pivot1_BA","rpc1DvsLBEtaPanelHits_Pivot1_BA", m_LB_Nbins, 0, m_LBmax,   2*188, 0, 188 ) ); 
	  sc=rpcprd_dq_BA_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;
			
	  // 1DvsLB panels Phi  hits HighPt0_BA 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBPhiPanelHits_HighPt0_BA","rpc1DvsLBPhiPanelHits_HighPt0_BA", m_LB_Nbins, 0, m_LBmax,  2*193, 0, 193 ) );  
	  sc=rpcprd_dq_BA_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;
		
	  // 1DvsLB panels Phi  hits HighPt1_BA 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBPhiPanelHits_HighPt1_BA","rpc1DvsLBPhiPanelHits_HighPt1_BA", m_LB_Nbins, 0, m_LBmax,  2*193, 0, 193 ) );  
	  sc=rpcprd_dq_BA_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;	
			
	  // 1DvsLB panels Eta  hits HighPt0_BA 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBEtaPanelHits_HighPt0_BA","rpc1DvsLBEtaPanelHits_HighPt0_BA", m_LB_Nbins, 0, m_LBmax,	2*193, 0, 193 ) ); 
	  sc=rpcprd_dq_BA_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;
		
	  // 1DvsLB panels Eta  hits HighPt1_BA 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBEtaPanelHits_HighPt1_BA","rpc1DvsLBEtaPanelHits_HighPt1_BA", m_LB_Nbins, 0, m_LBmax,	2*193, 0, 193 ) ); 
	  sc=rpcprd_dq_BA_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;

	  // 1DvsLB panels Phi  hits LowPt0_BC 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBPhiPanelHits_LowPt0_BC","rpc1DvsLBPhiPanelHits_LowPt0_BC", m_LB_Nbins, 0, m_LBmax,   2*157, 0, 157 ) ); 
	  sc=rpcprd_dq_BC_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;
		
	  // 1DvsLB panels Phi  hits LowPt1_BC 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBPhiPanelHits_LowPt1_BC","rpc1DvsLBPhiPanelHits_LowPt1_BC", m_LB_Nbins, 0, m_LBmax,   2*157, 0, 157 ) ); 
	  sc=rpcprd_dq_BC_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;
	
	  // 1DvsLB panels Eta  hits LowPt0_BC 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBEtaPanelHits_LowPt0_BC","rpc1DvsLBEtaPanelHits_LowPt0_BC", m_LB_Nbins, 0, m_LBmax,   2*157, 0, 157 ) ); 
	  sc=rpcprd_dq_BC_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;
		
	  // 1DvsLB panels Eta  hits LowPt1_BC 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBEtaPanelHits_LowPt1_BC","rpc1DvsLBEtaPanelHits_LowPt1_BC", m_LB_Nbins, 0, m_LBmax,   2*157, 0, 157 ) );  
	  sc=rpcprd_dq_BC_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;

	  // 1DvsLB panels Phi  hits Pivot0_BC 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBPhiPanelHits_Pivot0_BC","rpc1DvsLBPhiPanelHits_Pivot0_BC", m_LB_Nbins, 0, m_LBmax,   2*187, 0, 187 ) ); 
	  sc=rpcprd_dq_BC_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;
		
	  // 1DvsLB panels Phi  hits Pivot1_BC 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBPhiPanelHits_Pivot1_BC","rpc1DvsLBPhiPanelHits_Pivot1_BC", m_LB_Nbins, 0, m_LBmax,   2*187, 0, 187 ) ); 
	  sc=rpcprd_dq_BC_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;

	  // 1DvsLB panels Eta  hits Pivot0_BC 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBEtaPanelHits_Pivot0_BC","rpc1DvsLBEtaPanelHits_Pivot0_BC", m_LB_Nbins, 0, m_LBmax,   2*187, 0, 187 ) ); 
	  sc=rpcprd_dq_BC_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;
		
	  // 1DvsLB panels Eta  hits Pivot1_BC 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBEtaPanelHits_Pivot1_BC","rpc1DvsLBEtaPanelHits_Pivot1_BC", m_LB_Nbins, 0, m_LBmax,   2*187, 0, 187 ) ); 
	  sc=rpcprd_dq_BC_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;
			
	  // 1DvsLB panels Phi  hits HighPt0_BC 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBPhiPanelHits_HighPt0_BC","rpc1DvsLBPhiPanelHits_HighPt0_BC", m_LB_Nbins, 0, m_LBmax,  2*193, 0, 193 ) );  
	  sc=rpcprd_dq_BC_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;
		
	  // 1DvsLB panels Phi  hits HighPt1_BC 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBPhiPanelHits_HighPt1_BC","rpc1DvsLBPhiPanelHits_HighPt1_BC", m_LB_Nbins, 0, m_LBmax,  2*193, 0, 193 ) );  
	  sc=rpcprd_dq_BC_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;	
			
	  // 1DvsLB panels Eta  hits HighPt0_BC 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBEtaPanelHits_HighPt0_BC","rpc1DvsLBEtaPanelHits_HighPt0_BC", m_LB_Nbins, 0, m_LBmax,	2*193, 0, 193 ) ); 
	  sc=rpcprd_dq_BC_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;
		
	  // 1DvsLB panels Eta  hits HighPt1_BC 
	  rpc1DvsLBPanelHits.push_back( new TH2I("rpc1DvsLBEtaPanelHits_HighPt1_BC","rpc1DvsLBEtaPanelHits_HighPt1_BC", m_LB_Nbins, 0, m_LBmax,	2*193, 0, 193 ) ); 
	  sc=rpcprd_dq_BC_Panel.regHist(rpc1DvsLBPanelHits.back()) ; 
	  rpc1DvsLBPanelHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBPanelHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBPanelHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBPanelHits.back()->GetYaxis()->SetTitle("") ;

	  // 1DvsLB TrigTower Phi  hits LowPt_BA 
	  rpc1DvsLBTrigTowerHits.push_back( new TH2I("rpc1DvsLBPhiTrigTowerHits_LowPt_BA","rpc1DvsLBPhiTrigTowerHits_LowPt_BA", m_LB_Nbins, 0, m_LBmax,	2*108, 0, 108 ) ); 
	  sc=rpcprd_dq_BA_TrigTower.regHist(rpc1DvsLBTrigTowerHits.back()) ; 
	  rpc1DvsLBTrigTowerHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBTrigTowerHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBTrigTowerHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBTrigTowerHits.back()->GetYaxis()->SetTitle("") ; 

	  // 1DvsLB TrigTower Phi  hits HighPt_BA 
	  rpc1DvsLBTrigTowerHits.push_back( new TH2I("rpc1DvsLBPhiTrigTowerHits_HighPt_BA","rpc1DvsLBPhiTrigTowerHits_HighPt_BA", m_LB_Nbins, 0, m_LBmax,	2*108, 0, 108 ) ); 
	  sc=rpcprd_dq_BA_TrigTower.regHist(rpc1DvsLBTrigTowerHits.back()) ; 
	  rpc1DvsLBTrigTowerHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBTrigTowerHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBTrigTowerHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBTrigTowerHits.back()->GetYaxis()->SetTitle("") ; 

	  // 1DvsLB TrigTower Eta  hits LowPt_BA 
	  rpc1DvsLBTrigTowerHits.push_back( new TH2I("rpc1DvsLBEtaTrigTowerHits_LowPt_BA","rpc1DvsLBEtaTrigTowerHits_LowPt_BA", m_LB_Nbins, 0, m_LBmax,	2*108, 0, 108 ) ); 
	  sc=rpcprd_dq_BA_TrigTower.regHist(rpc1DvsLBTrigTowerHits.back()) ; 
	  rpc1DvsLBTrigTowerHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBTrigTowerHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBTrigTowerHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBTrigTowerHits.back()->GetYaxis()->SetTitle("") ; 

	  // 1DvsLB TrigTower Eta  hits HighPt_BA 
	  rpc1DvsLBTrigTowerHits.push_back( new TH2I("rpc1DvsLBEtaTrigTowerHits_HighPt_BA","rpc1DvsLBEtaTrigTowerHits_HighPt_BA", m_LB_Nbins, 0, m_LBmax,	2*108, 0, 108 ) ); 
	  sc=rpcprd_dq_BA_TrigTower.regHist(rpc1DvsLBTrigTowerHits.back()) ; 
	  rpc1DvsLBTrigTowerHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBTrigTowerHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBTrigTowerHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBTrigTowerHits.back()->GetYaxis()->SetTitle("") ; 

	  // 1DvsLB TrigTower Phi  hits LowPt_BC 
	  rpc1DvsLBTrigTowerHits.push_back( new TH2I("rpc1DvsLBPhiTrigTowerHits_LowPt_BC","rpc1DvsLBPhiTrigTowerHits_LowPt_BC", m_LB_Nbins, 0, m_LBmax,	2*108, 0, 108 ) ); 
	  sc=rpcprd_dq_BC_TrigTower.regHist(rpc1DvsLBTrigTowerHits.back()) ; 
	  rpc1DvsLBTrigTowerHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBTrigTowerHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBTrigTowerHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBTrigTowerHits.back()->GetYaxis()->SetTitle("") ; 

	  // 1DvsLB TrigTower Phi  hits HighPt_BC 
	  rpc1DvsLBTrigTowerHits.push_back( new TH2I("rpc1DvsLBPhiTrigTowerHits_HighPt_BC","rpc1DvsLBPhiTrigTowerHits_HighPt_BC", m_LB_Nbins, 0, m_LBmax,	2*108, 0, 108 ) ); 
	  sc=rpcprd_dq_BC_TrigTower.regHist(rpc1DvsLBTrigTowerHits.back()) ; 
	  rpc1DvsLBTrigTowerHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBTrigTowerHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBTrigTowerHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBTrigTowerHits.back()->GetYaxis()->SetTitle("") ; 

	  // 1DvsLB TrigTower Eta  hits LowPt_BC 
	  rpc1DvsLBTrigTowerHits.push_back( new TH2I("rpc1DvsLBEtaTrigTowerHits_LowPt_BC","rpc1DvsLBEtaTrigTowerHits_LowPt_BC", m_LB_Nbins, 0, m_LBmax,	2*108, 0, 108 ) ); 
	  sc=rpcprd_dq_BC_TrigTower.regHist(rpc1DvsLBTrigTowerHits.back()) ; 
	  rpc1DvsLBTrigTowerHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBTrigTowerHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBTrigTowerHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBTrigTowerHits.back()->GetYaxis()->SetTitle("") ; 

	  // 1DvsLB TrigTower Eta  hits HighPt_BC 
	  rpc1DvsLBTrigTowerHits.push_back( new TH2I("rpc1DvsLBEtaTrigTowerHits_HighPt_BC","rpc1DvsLBEtaTrigTowerHits_HighPt_BC", m_LB_Nbins, 0, m_LBmax,	2*108, 0, 108 ) ); 
	  sc=rpcprd_dq_BC_TrigTower.regHist(rpc1DvsLBTrigTowerHits.back()) ; 
	  rpc1DvsLBTrigTowerHits.back()->SetOption("COLZ") ;    
	  rpc1DvsLBTrigTowerHits.back()->SetMarkerSize(0.2);
	  rpc1DvsLBTrigTowerHits.back()->GetXaxis()->SetTitle("Lumiblock");
	  rpc1DvsLBTrigTowerHits.back()->GetYaxis()->SetTitle("") ; 
 
 
	   
	  //  //calculate max panels and towers  
  
	  int ismall         = 0 ;
	  char NAME[10];
  
  for(int idr = 1; idr != 2+1; idr ++ ){
  for(int iphi =1; iphi != 8+1; iphi++ ){
  for(int iname=      2; iname!=       53+1 ; iname++){
   if(iname==6||iname==7)continue;  
   if(iname>10&&iname<53)continue;  
     
  for(int ieta = -1; ieta != 1+1; ieta++ ){
     if(ieta==0)continue;

      Identifier rpcId = m_rpcIdHelper->channelID(iname, ieta, iphi, idr , 1, 1, 1, 1, 1); // last 5 arguments are: int doubletZ, int doubletPhi, int gasGap, int measuresPhi, int strip
      if (!rpcId.is_valid()) continue;
      const MuonGM::RpcReadoutElement* rpc = m_muonMgr->getRpcReadoutElement(rpcId);
      if(!rpc)continue;
     Identifier idr = rpc->identify();
     std::vector<int>   rpcstripshift = RpcGM::RpcStripShift(m_muonMgr,m_rpcIdHelper, idr, 0)  ;
		rpcpanel_dbindex   =  rpcstripshift[23];
		PlaneTipo          =  rpcstripshift[15];
		rpctower_dbindex   =  rpcstripshift[24]; 
		  if(iname== 2){
		    sprintf(NAME,"BML");
		    ismall=1;
		  }
		  if(iname== 3){
		    sprintf(NAME,"BMS");
		    ismall=2;
		  }
		  if(iname== 4){
		    sprintf(NAME,"BOL");
		    ismall=1;
		  }
		  if(iname== 5){
		    sprintf(NAME,"BOS");
		    ismall=2;
		  }
		  if(iname== 8){
		    sprintf(NAME,"BMF");
		    ismall=2;
		  }
		  if(iname== 9){
		    sprintf(NAME,"BOF");
		    ismall=2;
		  }
		  if(iname==10){
		    sprintf(NAME,"BOG");
		    ismall=2;
		  }
		  if(iname==53){
		    sprintf(NAME,"BME");
		    ismall=1;
		  }
    
		   
			char BinLabel[10];
			sprintf(BinLabel,"Sec%d%s",(iphi-1)*2+ismall,NAME);
			if(PlaneTipo==0){
			  if(ieta>=0){
			    rpc1DvsLBPanelHits[enum_Phi_LowPt0_BA]->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			    rpc1DvsLBPanelHits[enum_Phi_LowPt1_BA]->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			    rpc1DvsLBPanelHits[enum_Eta_LowPt0_BA]->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			    rpc1DvsLBPanelHits[enum_Eta_LowPt1_BA]->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			  } 
			  if(ieta<0){
			    rpc1DvsLBPanelHits[enum_Phi_LowPt0_BC]->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			    rpc1DvsLBPanelHits[enum_Phi_LowPt1_BC]->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			    rpc1DvsLBPanelHits[enum_Eta_LowPt0_BC]->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			    rpc1DvsLBPanelHits[enum_Eta_LowPt1_BC]->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			  } 
			} 
			if(PlaneTipo==1){
			  if(ieta>=0){
			    rpc1DvsLBPanelHits[enum_Phi_Pivot0_BA]->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			    rpc1DvsLBPanelHits[enum_Phi_Pivot1_BA]->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			    rpc1DvsLBPanelHits[enum_Eta_Pivot0_BA]->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			    rpc1DvsLBPanelHits[enum_Eta_Pivot1_BA]->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			    rpc1DvsLBPanelHits[enum_Phi_LowPt_BA]    ->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			    rpc1DvsLBPanelHits[enum_Phi_HighPt_BA]   ->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			    rpc1DvsLBPanelHits[enum_Eta_LowPt_BA]    ->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			    rpc1DvsLBPanelHits[enum_Eta_HighPt_BA]   ->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			    rpc1DvsLBPanelHits[enum_EtaPhi_LowPt_BA] ->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			    rpc1DvsLBPanelHits[enum_EtaPhi_HighPt_BA]->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			    rpc1DvsLBTrigTowerHits[ enum_Phi_TrigTowerLowPt_BA]->GetYaxis()->SetBinLabel(rpctower_dbindex*2,BinLabel);
			    rpc1DvsLBTrigTowerHits[ enum_Eta_TrigTowerLowPt_BA]->GetYaxis()->SetBinLabel(rpctower_dbindex*2,BinLabel);
			    rpc1DvsLBTrigTowerHits[enum_Phi_TrigTowerHighPt_BA]->GetYaxis()->SetBinLabel(rpctower_dbindex*2,BinLabel);
			    rpc1DvsLBTrigTowerHits[enum_Eta_TrigTowerHighPt_BA]->GetYaxis()->SetBinLabel(rpctower_dbindex*2,BinLabel);
			  } 
			  if(ieta<0){
			    rpc1DvsLBPanelHits[enum_Phi_Pivot0_BC]->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			    rpc1DvsLBPanelHits[enum_Phi_Pivot1_BC]->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			    rpc1DvsLBPanelHits[enum_Eta_Pivot0_BC]->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			    rpc1DvsLBPanelHits[enum_Eta_Pivot1_BC]->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			    rpc1DvsLBPanelHits[enum_Phi_LowPt_BC]    ->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			    rpc1DvsLBPanelHits[enum_Phi_HighPt_BC]   ->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			    rpc1DvsLBPanelHits[enum_Eta_LowPt_BC]    ->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			    rpc1DvsLBPanelHits[enum_Eta_HighPt_BC]   ->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			    rpc1DvsLBPanelHits[enum_EtaPhi_LowPt_BC] ->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			    rpc1DvsLBPanelHits[enum_EtaPhi_HighPt_BC]->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			    rpc1DvsLBTrigTowerHits[ enum_Phi_TrigTowerLowPt_BC]->GetYaxis()->SetBinLabel(rpctower_dbindex*2,BinLabel);
			    rpc1DvsLBTrigTowerHits[ enum_Eta_TrigTowerLowPt_BC]->GetYaxis()->SetBinLabel(rpctower_dbindex*2,BinLabel);
			    rpc1DvsLBTrigTowerHits[enum_Phi_TrigTowerHighPt_BC]->GetYaxis()->SetBinLabel(rpctower_dbindex*2,BinLabel);
			    rpc1DvsLBTrigTowerHits[enum_Eta_TrigTowerHighPt_BC]->GetYaxis()->SetBinLabel(rpctower_dbindex*2,BinLabel);
			  } 
			} 
			if(PlaneTipo==2){
			  if(ieta>=0){
			    rpc1DvsLBPanelHits[enum_Phi_HighPt0_BA]->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			    rpc1DvsLBPanelHits[enum_Phi_HighPt1_BA]->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			    rpc1DvsLBPanelHits[enum_Eta_HighPt0_BA]->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			    rpc1DvsLBPanelHits[enum_Eta_HighPt1_BA]->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			  } 
			  if(ieta<0){
			    rpc1DvsLBPanelHits[enum_Phi_HighPt0_BC]->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			    rpc1DvsLBPanelHits[enum_Phi_HighPt1_BC]->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			    rpc1DvsLBPanelHits[enum_Eta_HighPt0_BC]->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			    rpc1DvsLBPanelHits[enum_Eta_HighPt1_BC]->GetYaxis()->SetBinLabel(rpcpanel_dbindex*2,BinLabel);
			  } 
			}
		      }}}}		 
 
	
        	
	
	
	  if ( m_doTrigEvol ) {
	    char trEvLab[50];
	    snprintf(trEvLab, 50, "Number of Eta stations / %d events", minStatTrEvol );
	    rpcNumberEtaStatFired_Side_Pt.push_back(  new TH1I("rpcNumberEtaStatFired_BA_LowPt", "rpcNumberEtaStatFired_BA_LowPt", 350, 0, 350)) ;
	    sc=rpcprd_dq_BA.regHist(rpcNumberEtaStatFired_Side_Pt.back());
	    rpcNumberEtaStatFired_Side_Pt.back()->SetFillColor(42) ;
	    rpcNumberEtaStatFired_Side_Pt.back()->GetXaxis()->SetTitle(trEvLab   );
	    rpcNumberEtaStatFired_Side_Pt.back()->GetYaxis()->SetTitle("Counts"  );
	
	    rpcNumberEtaStatFired_Side_Pt.push_back(  new TH1I("rpcNumberEtaStatFired_BC_LowPt", "rpcNumberEtaStatFired_BC_LowPt", 400, 0, 400)) ;
	    sc=rpcprd_dq_BC.regHist(rpcNumberEtaStatFired_Side_Pt.back());
	    rpcNumberEtaStatFired_Side_Pt.back()->SetFillColor(42) ;
	    rpcNumberEtaStatFired_Side_Pt.back()->GetXaxis()->SetTitle(trEvLab   );
	    rpcNumberEtaStatFired_Side_Pt.back()->GetYaxis()->SetTitle("Counts"  );
	
	    rpcNumberEtaStatFired_Side_Pt.push_back(  new TH1I("rpcNumberEtaStatFired_BA_HighPt", "rpcNumberEtaStatFired_BA_HighPt", 350, 0, 350)) ;
	    sc=rpcprd_dq_BA.regHist(rpcNumberEtaStatFired_Side_Pt.back());
	    rpcNumberEtaStatFired_Side_Pt.back()->SetFillColor(42) ;
	    rpcNumberEtaStatFired_Side_Pt.back()->GetXaxis()->SetTitle(trEvLab   );
	    rpcNumberEtaStatFired_Side_Pt.back()->GetYaxis()->SetTitle("Counts"  );

	    rpcNumberEtaStatFired_Side_Pt.push_back(  new TH1I("rpcNumberEtaStatFired_BC_HighPt", "rpcNumberEtaStatFired_BC_HighPt", 400, 0, 400)) ;
	    sc=rpcprd_dq_BC.regHist(rpcNumberEtaStatFired_Side_Pt.back());
	    rpcNumberEtaStatFired_Side_Pt.back()->SetFillColor(42) ;
	    rpcNumberEtaStatFired_Side_Pt.back()->GetXaxis()->SetTitle(trEvLab   );
	    rpcNumberEtaStatFired_Side_Pt.back()->GetYaxis()->SetTitle("Counts"  );
        
	  }
	
	  // book trigger road and thresholds histogram
	  char th_lab0L[12], th_lab1L[12], th_lab2L[12], th_lab3L[12];
	  char th_lab0H[12], th_lab1H[12], th_lab2H[12], th_lab3H[12];
	  snprintf(th_lab0L, 12, "Not ass: %d", m_lv1Thres_0);
	  snprintf(th_lab1L, 12, "LowPt1: %d" , m_lv1Thres_1);
	  snprintf(th_lab2L, 12, "LowPt2: %d" , m_lv1Thres_2);
	  snprintf(th_lab3L, 12, "LowPt3: %d" , m_lv1Thres_3);
	  snprintf(th_lab0H, 12, "Not ass: %d", m_lv1Thres_0);
	  snprintf(th_lab1H, 12, "HighPt1: %d", m_lv1Thres_1);
	  snprintf(th_lab2H, 12, "HighPt2: %d", m_lv1Thres_2);
	  snprintf(th_lab3H, 12, "HighPt3: %d", m_lv1Thres_3);
	  
	  
	  TH2 * rpcTriggerRoad = new TH2I( "RPC_TriggerRoad", "RPC_TriggerRoad" , 100, -10000, 10000, 4, 0, 4);
	  sc=rpcTrigRoad.regHist(rpcTriggerRoad) ;
	  rpcTriggerRoad->SetOption("COLZ"); 
	  rpcTriggerRoad->GetXaxis()->SetTitle("position of projected point [mm] ");
	  rpcTriggerRoad->GetYaxis()->SetTitle("Threshold");
	  rpcTriggerRoad->GetYaxis()->SetBinLabel(1, "Not assigned" );
	  rpcTriggerRoad->GetYaxis()->SetBinLabel(2, "Th 1" );
	  rpcTriggerRoad->GetYaxis()->SetBinLabel(3, "Th 2" );
	  rpcTriggerRoad->GetYaxis()->SetBinLabel(4, "Th 3" );
 
	  std::vector<std::string> SmallLarge_list ;
	  std::vector<std::string> LowHighPt_list  ;
	  std::vector<std::string> EtaPhi_list     ;
	  SmallLarge_list.push_back("Small"); SmallLarge_list.push_back("Large");
	  LowHighPt_list.push_back("LowPt") ; LowHighPt_list.push_back("HighPt");
	  EtaPhi_list.push_back("Eta")      ; EtaPhi_list.push_back("Phi")      ;
	  	  
	  for (std::vector<std::string>::const_iterator it3=EtaPhi_list.begin();
	       it3!=EtaPhi_list.end(); it3++ ) {
	    std::string m_rpcThreshold_title      = "RPC_Threshold_" + *it3     ;
	    const char* m_rpcThreshold_title_char = m_rpcThreshold_title.c_str();
	    TH1 * rpcThreshold = new TH1I(m_rpcThreshold_title_char, m_rpcThreshold_title_char, 8, 0, 8);
	    sc=rpcTrigRoad.regHist(rpcThreshold);
	    rpcThreshold->SetFillColor(42) ;
	    rpcThreshold->GetXaxis()->SetTitle("Threshold" );
	    rpcThreshold->GetYaxis()->SetTitle("Counts"    );
	    rpcThreshold->GetXaxis()->SetBinLabel(1, th_lab0L );
	    rpcThreshold->GetXaxis()->SetBinLabel(2, th_lab1L );
	    rpcThreshold->GetXaxis()->SetBinLabel(3, th_lab2L );
	    rpcThreshold->GetXaxis()->SetBinLabel(4, th_lab3L );
	    rpcThreshold->GetXaxis()->SetBinLabel(5, th_lab0H );
	    rpcThreshold->GetXaxis()->SetBinLabel(6, th_lab1H );
	    rpcThreshold->GetXaxis()->SetBinLabel(7, th_lab2H );
	    rpcThreshold->GetXaxis()->SetBinLabel(8, th_lab3H );
	          
	    for (std::vector<std::string>::const_iterator it1=SmallLarge_list.begin();
		 it1!=SmallLarge_list.end(); it1++ ) {
	      std::string m_rpcTriggerRoad_title = "RPC_TriggerRoad_" + *it1 + "_" + *it3 ;
	      const char* m_rpcTriggerRoad_title_char = m_rpcTriggerRoad_title.c_str();
	      TH2 * rpcTriggerRoad = new TH2I( m_rpcTriggerRoad_title_char, m_rpcTriggerRoad_title_char, 100, -10000, 10000, 8, 0, 8);
	      sc=rpcTrigRoad.regHist(rpcTriggerRoad) ;
	      rpcTriggerRoad->SetOption("COLZ"); 
	      rpcTriggerRoad->GetXaxis()->SetTitle("position of projected point [mm] ");
	      rpcTriggerRoad->GetYaxis()->SetTitle("Threshold");
	      rpcTriggerRoad->GetYaxis()->SetBinLabel(1, th_lab0L );
	      rpcTriggerRoad->GetYaxis()->SetBinLabel(2, th_lab1L );
	      rpcTriggerRoad->GetYaxis()->SetBinLabel(3, th_lab2L );
	      rpcTriggerRoad->GetYaxis()->SetBinLabel(4, th_lab3L );
	      rpcTriggerRoad->GetYaxis()->SetBinLabel(5, th_lab0H );
	      rpcTriggerRoad->GetYaxis()->SetBinLabel(6, th_lab1H );
	      rpcTriggerRoad->GetYaxis()->SetBinLabel(7, th_lab2H );
	      rpcTriggerRoad->GetYaxis()->SetBinLabel(8, th_lab3H );
	    }
	  }
	  
	  
	
	  TH2 *rpcPhivsEtaAtlasPivot0=new TH2I("AtlasPivot0","AtlasPivot0", 2*408/m_rpcreducenbins, -408, 408, (8*(48*2+64*2)+4*16)/m_rpcreducenbins,0,8*(48*2+64*2)+4*16); 
	  sc=rpcprd_expert.regHist(rpcPhivsEtaAtlasPivot0) ; 
	  rpcPhivsEtaAtlasPivot0->SetFillColor(42);  
	  rpcPhivsEtaAtlasPivot0->SetMarkerColor(1);  
	  rpcPhivsEtaAtlasPivot0->SetMarkerStyle(21);   
	  rpcPhivsEtaAtlasPivot0->SetOption("COLZ");   
	  rpcPhivsEtaAtlasPivot0->SetMarkerSize(0.2);
	  rpcPhivsEtaAtlasPivot0->GetXaxis()->SetTitle("<--- SIDE C      Rpc Eta  strip	SIDE A --->");
	  rpcPhivsEtaAtlasPivot0->GetYaxis()->SetTitle("Rpc Phi  strip");
	  
	  TH2 *rpcPhivsEtaAtlasPivot1=new TH2I("AtlasPivot1","AtlasPivot1", 2*408/m_rpcreducenbins, -408, 408, (8*(48*2+64*2)+4*16)/m_rpcreducenbins,0,8*(48*2+64*2)+4*16); 
	  sc=rpcprd_expert.regHist(rpcPhivsEtaAtlasPivot1) ; 
	  rpcPhivsEtaAtlasPivot1->SetFillColor(42);  
	  rpcPhivsEtaAtlasPivot1->SetMarkerColor(1);  
	  rpcPhivsEtaAtlasPivot1->SetMarkerStyle(21);   
	  rpcPhivsEtaAtlasPivot1->SetOption("COLZ");    
	  rpcPhivsEtaAtlasPivot1->SetMarkerSize(0.2);
	  rpcPhivsEtaAtlasPivot1->GetXaxis()->SetTitle("<--- SIDE C      Rpc Eta  strip	SIDE A --->");
	  rpcPhivsEtaAtlasPivot1->GetYaxis()->SetTitle("Rpc Phi  strip");
	
	  TH2 *rpcPhivsEtaAtlasLowPt0=new TH2I("AtlasLowPt0","AtlasLowPt0", 2*408/m_rpcreducenbins, -408, 408, 8*(48*2+64*2)/m_rpcreducenbins,0,8*(48*2+64*2)); 
	  sc=rpcprd_expert.regHist(rpcPhivsEtaAtlasLowPt0) ; 
	  rpcPhivsEtaAtlasLowPt0->SetFillColor(42);  
	  rpcPhivsEtaAtlasLowPt0->SetMarkerColor(1);  
	  rpcPhivsEtaAtlasLowPt0->SetMarkerStyle(21);  
	  rpcPhivsEtaAtlasLowPt0->SetOption("COLZ");     
	  rpcPhivsEtaAtlasLowPt0->SetMarkerSize(0.2);
	  rpcPhivsEtaAtlasLowPt0->GetXaxis()->SetTitle("<--- SIDE C      Rpc Eta  strip	SIDE A --->");
	  rpcPhivsEtaAtlasLowPt0->GetYaxis()->SetTitle("Rpc Phi  strip");
	  
	  TH2 *rpcPhivsEtaAtlasLowPt1=new TH2I("AtlasLowPt1","AtlasLowPt1", 2*408/m_rpcreducenbins, -408, 408, 8*(48*2+64*2)/m_rpcreducenbins,0,8*(48*2+64*2)); 
	  sc=rpcprd_expert.regHist(rpcPhivsEtaAtlasLowPt1) ; 
	  rpcPhivsEtaAtlasLowPt1->SetFillColor(42);  
	  rpcPhivsEtaAtlasLowPt1->SetMarkerColor(1);  
	  rpcPhivsEtaAtlasLowPt1->SetMarkerStyle(21);  
	  rpcPhivsEtaAtlasLowPt1->SetOption("COLZ");    
	  rpcPhivsEtaAtlasLowPt1->SetMarkerSize(0.2);
	  rpcPhivsEtaAtlasLowPt1->GetXaxis()->SetTitle("<--- SIDE C      Rpc Eta  strip	SIDE A --->");
	  rpcPhivsEtaAtlasLowPt1->GetYaxis()->SetTitle("Rpc Phi  strip");
	
	  TH2 *rpcPhivsEtaAtlasHighPt0=new TH2I("AtlasHighPt0","AtlasHighPt0", 2*408/m_rpcreducenbins, -408, 408, 8*(64*2+80*2)/m_rpcreducenbins,0,8*(64*2+80*2)); 
	  sc=rpcprd_expert.regHist(rpcPhivsEtaAtlasHighPt0) ; 
	  rpcPhivsEtaAtlasHighPt0->SetFillColor(42);  
	  rpcPhivsEtaAtlasHighPt0->SetMarkerColor(1);  
	  rpcPhivsEtaAtlasHighPt0->SetMarkerStyle(21);  
	  rpcPhivsEtaAtlasHighPt0->SetOption("COLZ");    
	  rpcPhivsEtaAtlasHighPt0->SetMarkerSize(0.2);
	  rpcPhivsEtaAtlasHighPt0->GetXaxis()->SetTitle("<--- SIDE C      Rpc Eta  strip	 SIDE A --->");
	  rpcPhivsEtaAtlasHighPt0->GetYaxis()->SetTitle("Rpc Phi  strip");
	  
	  TH2 *rpcPhivsEtaAtlasHighPt1=new TH2I("AtlasHighPt1","AtlasHighPt1", 2*408/m_rpcreducenbins, -408, 408, 8*(64*2+80*2)/m_rpcreducenbins,0,8*(64*2+80*2)); 
	  sc=rpcprd_expert.regHist(rpcPhivsEtaAtlasHighPt1) ; 
	  rpcPhivsEtaAtlasHighPt1->SetFillColor(42);  
	  rpcPhivsEtaAtlasHighPt1->SetMarkerColor(1);  
	  rpcPhivsEtaAtlasHighPt1->SetMarkerStyle(21);  
	  rpcPhivsEtaAtlasHighPt1->SetOption("COLZ");    
	  rpcPhivsEtaAtlasHighPt1->SetMarkerSize(0.2);
	  rpcPhivsEtaAtlasHighPt1->GetXaxis()->SetTitle("<--- SIDE C      Rpc Eta  strip	 SIDE A --->");
	  rpcPhivsEtaAtlasHighPt1->GetYaxis()->SetTitle("Rpc Phi  strip");
	
	  
	  TH2 *rpcPhivsEtaAtlasLowPt_TriggerOut=new TH2I("AtlasLowPt_TriggerOut","AtlasLowPt_TriggerOut", 2*408/m_rpcreducenbins, -408, 408, (8*(48*2+64*2)+4*16)/m_rpcreducenbins,0,8*(48*2+64*2)+4*16); 
	  sc=rpcprd_expert.regHist(rpcPhivsEtaAtlasLowPt_TriggerOut) ; 
	  rpcPhivsEtaAtlasLowPt_TriggerOut->SetFillColor(42);  
	  rpcPhivsEtaAtlasLowPt_TriggerOut->SetMarkerColor(1);  
	  rpcPhivsEtaAtlasLowPt_TriggerOut->SetMarkerStyle(21);  
	  rpcPhivsEtaAtlasLowPt_TriggerOut->SetOption("COLZ");    
	  rpcPhivsEtaAtlasLowPt_TriggerOut->SetMarkerSize(0.2);
	  rpcPhivsEtaAtlasLowPt_TriggerOut->GetXaxis()->SetTitle("<--- SIDE C      Rpc Eta  strip	  SIDE A --->");
	  rpcPhivsEtaAtlasLowPt_TriggerOut->GetYaxis()->SetTitle("Rpc Phi  strip");
	  
	  TH2 *rpcPhivsEtaAtlasHighPt_TriggerFromLowPt=new TH2I("AtlasHighPt_TriggerFromLowPt","AtlasHighPt_TriggerFromLowPt", 2*408/m_rpcreducenbins, -408, 408, (8*(48*2+64*2)+4*16)/m_rpcreducenbins,0,8*(48*2+64*2)+4*16); 
	  sc=rpcprd_expert.regHist(rpcPhivsEtaAtlasHighPt_TriggerFromLowPt) ; 
	  rpcPhivsEtaAtlasHighPt_TriggerFromLowPt->SetFillColor(42);  
	  rpcPhivsEtaAtlasHighPt_TriggerFromLowPt->SetMarkerColor(1);  
	  rpcPhivsEtaAtlasHighPt_TriggerFromLowPt->SetMarkerStyle(21);  
	  rpcPhivsEtaAtlasHighPt_TriggerFromLowPt->SetOption("COLZ");    
	  rpcPhivsEtaAtlasHighPt_TriggerFromLowPt->SetMarkerSize(0.2);
	  rpcPhivsEtaAtlasHighPt_TriggerFromLowPt->GetXaxis()->SetTitle("<--- SIDE C      Rpc Eta  strip	 SIDE A --->");
	  rpcPhivsEtaAtlasHighPt_TriggerFromLowPt->GetYaxis()->SetTitle("Rpc Phi  strip");
	
	  TH2 *rpcPhivsEtaAtlasHighPt_TriggerOut=new TH2I("AtlasHighPt_TriggerOut","AtlasHighPt_TriggerOut", 2*408/m_rpcreducenbins, -408, 408, (8*(48*2+64*2)+4*16)/m_rpcreducenbins,0,8*(48*2+64*2)+4*16); 
	  sc=rpcprd_expert.regHist(rpcPhivsEtaAtlasHighPt_TriggerOut) ; 
	  rpcPhivsEtaAtlasHighPt_TriggerOut->SetFillColor(42);  
	  rpcPhivsEtaAtlasHighPt_TriggerOut->SetMarkerColor(1);  
	  rpcPhivsEtaAtlasHighPt_TriggerOut->SetMarkerStyle(21);
	  rpcPhivsEtaAtlasHighPt_TriggerOut->SetOption("COLZ");    
	  rpcPhivsEtaAtlasHighPt_TriggerOut->SetMarkerSize(0.2);
	  rpcPhivsEtaAtlasHighPt_TriggerOut->GetXaxis()->SetTitle("<--- SIDE C	Rpc Eta  strip	   SIDE A --->");
	  rpcPhivsEtaAtlasHighPt_TriggerOut->GetYaxis()->SetTitle("Rpc Phi  strip");
	  
	
	  //---------------------------------------------------
	  //  2-D  Atlas plot phi vs z
	  
	  TH2 *rpcPhivsEtaAtlasPivot0_PhivsZ=new TH2I("AtlasPivot0_PhivsZ","AtlasPivot0_PhivsZ", 100, -9630, 9630, 100,-CLHEP::pi,CLHEP::pi); 
	  // coord cilindriche (1-> phi, 2-> z)
	  //TH2 *rpcPhivsEtaAtlasPivot0_PhivsZ=new TH2I("AtlasPivot0_PhivsZ","AtlasPivot0_PhivsZ", 100, -CLHEP::pi, CLHEP::pi, 100,-9630,9630);
	  sc=rpcprd_expert.regHist(rpcPhivsEtaAtlasPivot0_PhivsZ) ; 
	  rpcPhivsEtaAtlasPivot0_PhivsZ->SetFillColor(42);  
	  rpcPhivsEtaAtlasPivot0_PhivsZ->SetMarkerColor(1);  
	  rpcPhivsEtaAtlasPivot0_PhivsZ->SetMarkerStyle(21);   
	  rpcPhivsEtaAtlasPivot0_PhivsZ->SetOption("COLZ");
	  //rpcPhivsEtaAtlasPivot0_PhivsZ->SetOption("LEGO2 CYL");  // coordinate cilindriche LEGO2 CYL
	  rpcPhivsEtaAtlasPivot0_PhivsZ->SetMarkerSize(0.2);
	  rpcPhivsEtaAtlasPivot0_PhivsZ->GetXaxis()->SetTitle("<--- SIDE C      Rpc Z  [mm]  SIDE A --->");
	  rpcPhivsEtaAtlasPivot0_PhivsZ->GetYaxis()->SetTitle("Rpc Phi  [rad] ");
	
	  TH2 *rpcPhivsEtaAtlasPivot1_PhivsZ=new TH2I("AtlasPivot1_PhivsZ","AtlasPivot1_PhivsZ", 100, -9630, 9630, 100,-CLHEP::pi,CLHEP::pi); 
	  sc=rpcprd_expert.regHist(rpcPhivsEtaAtlasPivot1_PhivsZ) ; 
	  rpcPhivsEtaAtlasPivot1_PhivsZ->SetFillColor(42);  
	  rpcPhivsEtaAtlasPivot1_PhivsZ->SetMarkerColor(1);  
	  rpcPhivsEtaAtlasPivot1_PhivsZ->SetMarkerStyle(21);   
	  rpcPhivsEtaAtlasPivot1_PhivsZ->SetOption("COLZ");    
	  rpcPhivsEtaAtlasPivot1_PhivsZ->SetMarkerSize(0.2);
	  rpcPhivsEtaAtlasPivot1_PhivsZ->GetXaxis()->SetTitle("<--- SIDE C      Rpc Z  [mm]       	SIDE A --->");
	  rpcPhivsEtaAtlasPivot1_PhivsZ->GetYaxis()->SetTitle("Rpc Phi  [rad]");
	  
	  TH2 *rpcPhivsEtaAtlasLowPt0_PhivsZ=new TH2I("AtlasLowPt0_PhivsZ","AtlasLowPt0_PhivsZ", 100, -9650, 9650, 100,-CLHEP::pi,CLHEP::pi); 
	  sc=rpcprd_expert.regHist(rpcPhivsEtaAtlasLowPt0_PhivsZ) ; 
	  rpcPhivsEtaAtlasLowPt0_PhivsZ->SetFillColor(42);  
	  rpcPhivsEtaAtlasLowPt0_PhivsZ->SetMarkerColor(1);  
	  rpcPhivsEtaAtlasLowPt0_PhivsZ->SetMarkerStyle(21);  
	  rpcPhivsEtaAtlasLowPt0_PhivsZ->SetOption("COLZ");     
	  rpcPhivsEtaAtlasLowPt0_PhivsZ->SetMarkerSize(0.2);
	  rpcPhivsEtaAtlasLowPt0_PhivsZ->GetXaxis()->SetTitle("<--- SIDE C      Rpc Z  [mm]       	SIDE A --->");
	  rpcPhivsEtaAtlasLowPt0_PhivsZ->GetYaxis()->SetTitle("Rpc Phi  [rad]");
	
	  TH2 *rpcPhivsEtaAtlasLowPt1_PhivsZ=new TH2I("AtlasLowPt1_PhivsZ","AtlasLowPt1_PhivsZ", 100, -9650, 9650, 100,-CLHEP::pi,CLHEP::pi); 
	  sc=rpcprd_expert.regHist(rpcPhivsEtaAtlasLowPt1_PhivsZ) ; 
	  rpcPhivsEtaAtlasLowPt1_PhivsZ->SetFillColor(42);  
	  rpcPhivsEtaAtlasLowPt1_PhivsZ->SetMarkerColor(1);  
	  rpcPhivsEtaAtlasLowPt1_PhivsZ->SetMarkerStyle(21);  
	  rpcPhivsEtaAtlasLowPt1_PhivsZ->SetOption("COLZ");    
	  rpcPhivsEtaAtlasLowPt1_PhivsZ->SetMarkerSize(0.2);
	  rpcPhivsEtaAtlasLowPt1_PhivsZ->GetXaxis()->SetTitle("<--- SIDE C      Rpc Z  [mm]       	SIDE A --->");
	  rpcPhivsEtaAtlasLowPt1_PhivsZ->GetYaxis()->SetTitle("Rpc Phi  [rad]");

	  TH2 *rpcPhivsEtaAtlasHighPt0_PhivsZ=new TH2I("AtlasHighPt0_PhivsZ","AtlasHighPt0_PhivsZ", 100, -12850, 12850, 100,-CLHEP::pi,CLHEP::pi); 
	  sc=rpcprd_expert.regHist(rpcPhivsEtaAtlasHighPt0_PhivsZ) ; 
	  rpcPhivsEtaAtlasHighPt0_PhivsZ->SetFillColor(42);  
	  rpcPhivsEtaAtlasHighPt0_PhivsZ->SetMarkerColor(1);  
	  rpcPhivsEtaAtlasHighPt0_PhivsZ->SetMarkerStyle(21);  
	  rpcPhivsEtaAtlasHighPt0_PhivsZ->SetOption("COLZ");    
	  rpcPhivsEtaAtlasHighPt0_PhivsZ->SetMarkerSize(0.2);
	  rpcPhivsEtaAtlasHighPt0_PhivsZ->GetXaxis()->SetTitle("<--- SIDE C      Rpc Z  [mm]      	 SIDE A --->");
	  rpcPhivsEtaAtlasHighPt0_PhivsZ->GetYaxis()->SetTitle("Rpc Phi  [rad]");
 	  
	  TH2 *rpcPhivsEtaAtlasHighPt1_PhivsZ=new TH2I("AtlasHighPt1_PhivsZ","AtlasHighPt1_PhivsZ", 100, -12850, 12850, 100,-CLHEP::pi,CLHEP::pi); 
	  sc=rpcprd_expert.regHist(rpcPhivsEtaAtlasHighPt1_PhivsZ) ; 
	  rpcPhivsEtaAtlasHighPt1_PhivsZ->SetFillColor(42);  
	  rpcPhivsEtaAtlasHighPt1_PhivsZ->SetMarkerColor(1);  
	  rpcPhivsEtaAtlasHighPt1_PhivsZ->SetMarkerStyle(21);  
	  rpcPhivsEtaAtlasHighPt1_PhivsZ->SetOption("COLZ");    
	  rpcPhivsEtaAtlasHighPt1_PhivsZ->SetMarkerSize(0.2);
	  rpcPhivsEtaAtlasHighPt1_PhivsZ->GetXaxis()->SetTitle("<--- SIDE C      Rpc Z  [mm]      	 SIDE A --->");
	  rpcPhivsEtaAtlasHighPt1_PhivsZ->GetYaxis()->SetTitle("Rpc Phi [rad]");
	
	  // trigger phi vs eta 
	  TH2 *rpcPhivsEtaAtlasLowPt_TriggerOut_PhivsZ=new TH2I("AtlasLowPt_TriggerOut_PhivsZ","AtlasLowPt_TriggerOut_PhivsZ", 100, -9350, 9350, 100, -CLHEP::pi,CLHEP::pi); 
	  sc=rpcprd_expert.regHist(rpcPhivsEtaAtlasLowPt_TriggerOut_PhivsZ) ; 
	  rpcPhivsEtaAtlasLowPt_TriggerOut_PhivsZ->SetFillColor(42);  
	  rpcPhivsEtaAtlasLowPt_TriggerOut_PhivsZ->SetMarkerColor(1);  
	  rpcPhivsEtaAtlasLowPt_TriggerOut_PhivsZ->SetMarkerStyle(21);  
	  rpcPhivsEtaAtlasLowPt_TriggerOut_PhivsZ->SetOption("COLZ");    
	  rpcPhivsEtaAtlasLowPt_TriggerOut_PhivsZ->SetMarkerSize(0.2);
	  rpcPhivsEtaAtlasLowPt_TriggerOut_PhivsZ->GetXaxis()->SetTitle("<--- SIDE C      Rpc Z  [mm]        	  SIDE A --->");
	  rpcPhivsEtaAtlasLowPt_TriggerOut_PhivsZ->GetYaxis()->SetTitle("Rpc Phi  [rad] ");

	  TH2 *rpcPhivsEtaAtlasHighPt_TriggerFromLowPt_PhivsZ=new	TH2I("AtlasHighPt_TriggerFromLowPt_PhivsZ","AtlasHighPt_TriggerFromLowPt_PhivsZ", 100, -9350, 9350, 100,-CLHEP::pi,CLHEP::pi); 
	  sc=rpcprd_expert.regHist(rpcPhivsEtaAtlasHighPt_TriggerFromLowPt_PhivsZ) ; 
	  rpcPhivsEtaAtlasHighPt_TriggerFromLowPt_PhivsZ->SetFillColor(42);  
	  rpcPhivsEtaAtlasHighPt_TriggerFromLowPt_PhivsZ->SetMarkerColor(1);  
	  rpcPhivsEtaAtlasHighPt_TriggerFromLowPt_PhivsZ->SetMarkerStyle(21);  
	  rpcPhivsEtaAtlasHighPt_TriggerFromLowPt_PhivsZ->SetOption("COLZ");    
	  rpcPhivsEtaAtlasHighPt_TriggerFromLowPt_PhivsZ->SetMarkerSize(0.2);
	  rpcPhivsEtaAtlasHighPt_TriggerFromLowPt_PhivsZ->GetXaxis()->SetTitle("<--- SIDE C      Rpc Z  [mm]   	 SIDE A --->");
	  rpcPhivsEtaAtlasHighPt_TriggerFromLowPt_PhivsZ->GetYaxis()->SetTitle("Rpc Phi  [rad] ");
	  
	  TH2 *rpcPhivsEtaAtlasHighPt_TriggerOut_PhivsZ=new TH2I("AtlasHighPt_TriggerOut_PhivsZ","AtlasHighPt_TriggerOut_PhivsZ", 100, -12850, 12850, 100,-CLHEP::pi,CLHEP::pi); 
	  sc=rpcprd_expert.regHist(rpcPhivsEtaAtlasHighPt_TriggerOut_PhivsZ) ; 
	  rpcPhivsEtaAtlasHighPt_TriggerOut_PhivsZ->SetFillColor(42);  
	  rpcPhivsEtaAtlasHighPt_TriggerOut_PhivsZ->SetMarkerColor(1);  
	  rpcPhivsEtaAtlasHighPt_TriggerOut_PhivsZ->SetMarkerStyle(21);
	  rpcPhivsEtaAtlasHighPt_TriggerOut_PhivsZ->SetOption("COLZ");    
	  rpcPhivsEtaAtlasHighPt_TriggerOut_PhivsZ->SetMarkerSize(0.2);
	  rpcPhivsEtaAtlasHighPt_TriggerOut_PhivsZ->GetXaxis()->SetTitle("<--- SIDE C	Rpc Z  [mm]   	   SIDE A --->");
	  rpcPhivsEtaAtlasHighPt_TriggerOut_PhivsZ->GetYaxis()->SetTitle("Rpc Phi  [rad] ");
	
	  //----------------------------------------------------------------------
	  
	

	  ///////////////////////////////
	  // RPC global
	 	  
	  TH2 * GlobalHitsPerRPCMiddle = new TH2I("GlobalHitsPerRPCMiddle","GlobalHitsPerRPCMiddle",26, -13, 13, 32, 0, 16);
	  sc = rpc_dqmf_global.regHist( GlobalHitsPerRPCMiddle );
	  GlobalHitsPerRPCMiddle->SetOption("COLZ");
	  GlobalHitsPerRPCMiddle->GetXaxis()->SetTitle("Rpc Eta Panel");
	  GlobalHitsPerRPCMiddle->GetYaxis()->SetTitle("Sectors")      ;
	
	  TH2 * GlobalHitsPerRPCOuter = new TH2I("GlobalHitsPerRPCOuter","GlobalHitsPerRPCOuter",26, -13, 13, 32, 0, 16);
	  sc = rpc_dqmf_global.regHist( GlobalHitsPerRPCOuter );
	  GlobalHitsPerRPCOuter->SetOption("COLZ");
	  GlobalHitsPerRPCOuter->GetXaxis()->SetTitle("RPC Eta Panel");
	  GlobalHitsPerRPCOuter->GetYaxis()->SetTitle("Sectors")      ;
	
	  TH2 * EtavsPhi_TriggeredMuons_LowPt = new TH2I("EtavsPhi_TriggeredMuons_LowPt", "EtavsPhi_TriggeredMuons_LowPt", 100, -1.2, 1.2, 100, -CLHEP::pi, CLHEP::pi);
	  sc = rpc_dqmf_global.regHist(EtavsPhi_TriggeredMuons_LowPt) ;
	  EtavsPhi_TriggeredMuons_LowPt->SetOption("COLZ");
	  EtavsPhi_TriggeredMuons_LowPt->GetXaxis()->SetTitle("Eta        ");
	  EtavsPhi_TriggeredMuons_LowPt->GetYaxis()->SetTitle("Phi   (rad)");

	  TH2 * EtavsPhi_TriggeredMuons_HighPt = new TH2I("EtavsPhi_TriggeredMuons_HighPt", "EtavsPhi_TriggeredMuons_HighPt", 100, -1.2, 1.2, 100, -CLHEP::pi, CLHEP::pi);
	  sc = rpc_dqmf_global.regHist(EtavsPhi_TriggeredMuons_HighPt) ;
	  EtavsPhi_TriggeredMuons_HighPt->SetOption("COLZ");
	  EtavsPhi_TriggeredMuons_HighPt->GetXaxis()->SetTitle("Eta        ");
	  EtavsPhi_TriggeredMuons_HighPt->GetYaxis()->SetTitle("Phi   (rad)");

	  TH2 * EtavsPhi_TriggeredMuons_Pt1 = new TH2I("EtavsPhi_TriggeredMuons_Pt1", "EtavsPhi_TriggeredMuons_Pt1", 100, -1.2, 1.2, 100, -CLHEP::pi, CLHEP::pi);
	  sc = rpc_dqmf_global.regHist(EtavsPhi_TriggeredMuons_Pt1) ;
	  EtavsPhi_TriggeredMuons_Pt1->SetOption("COLZ");
	  EtavsPhi_TriggeredMuons_Pt1->GetXaxis()->SetTitle("Eta        ");
	  EtavsPhi_TriggeredMuons_Pt1->GetYaxis()->SetTitle("Phi   (rad)");
 
	  TH2 * EtavsPhi_TriggeredMuons_Pt2 = new TH2I("EtavsPhi_TriggeredMuons_Pt2", "EtavsPhi_TriggeredMuons_Pt2", 100, -1.2, 1.2, 100, -CLHEP::pi, CLHEP::pi);
	  sc = rpc_dqmf_global.regHist(EtavsPhi_TriggeredMuons_Pt2) ;
	  EtavsPhi_TriggeredMuons_Pt2->SetOption("COLZ");
	  EtavsPhi_TriggeredMuons_Pt2->GetXaxis()->SetTitle("Eta        ");
	  EtavsPhi_TriggeredMuons_Pt2->GetYaxis()->SetTitle("Phi   (rad)");
	
	  TH2 * EtavsPhi_TriggeredMuons_Pt3 = new TH2I("EtavsPhi_TriggeredMuons_Pt3", "EtavsPhi_TriggeredMuons_Pt3", 100, -1.2, 1.2, 100, -CLHEP::pi, CLHEP::pi);
	  sc = rpc_dqmf_global.regHist(EtavsPhi_TriggeredMuons_Pt3) ;
	  EtavsPhi_TriggeredMuons_Pt3->SetOption("COLZ");
	  EtavsPhi_TriggeredMuons_Pt3->GetXaxis()->SetTitle("Eta        ");
	  EtavsPhi_TriggeredMuons_Pt3->GetYaxis()->SetTitle("Phi   (rad)");

	  TH2 * EtavsPhi_TriggeredMuons_Pt4 = new TH2I("EtavsPhi_TriggeredMuons_Pt4", "EtavsPhi_TriggeredMuons_Pt4", 100, -1.2, 1.2, 100, -CLHEP::pi, CLHEP::pi);
	  sc = rpc_dqmf_global.regHist(EtavsPhi_TriggeredMuons_Pt4) ;
	  EtavsPhi_TriggeredMuons_Pt4->SetOption("COLZ");
	  EtavsPhi_TriggeredMuons_Pt4->GetXaxis()->SetTitle("Eta        ");
	  EtavsPhi_TriggeredMuons_Pt4->GetYaxis()->SetTitle("Phi   (rad)");
	  EtavsPhi_TriggeredMuons_Pt4->GetYaxis()->SetTitle("Phi   (rad)");
	
	  TH2 * EtavsPhi_TriggeredMuons_Pt5 = new TH2I("EtavsPhi_TriggeredMuons_Pt5", "EtavsPhi_TriggeredMuons_Pt5", 100, -1.2, 1.2, 100, -CLHEP::pi, CLHEP::pi);
	  sc = rpc_dqmf_global.regHist(EtavsPhi_TriggeredMuons_Pt5) ;
	  EtavsPhi_TriggeredMuons_Pt5->SetOption("COLZ");
	  EtavsPhi_TriggeredMuons_Pt5->GetXaxis()->SetTitle("Eta        ");
	  EtavsPhi_TriggeredMuons_Pt5->GetYaxis()->SetTitle("Phi   (rad)");

	  TH2 * EtavsPhi_TriggeredMuons_Pt6 = new TH2I("EtavsPhi_TriggeredMuons_Pt6", "EtavsPhi_TriggeredMuons_Pt6", 100, -1.2, 1.2, 100, -CLHEP::pi, CLHEP::pi);
	  sc = rpc_dqmf_global.regHist(EtavsPhi_TriggeredMuons_Pt6) ;
	  EtavsPhi_TriggeredMuons_Pt6->SetOption("COLZ");
	  EtavsPhi_TriggeredMuons_Pt6->GetXaxis()->SetTitle("Eta        ");
	  EtavsPhi_TriggeredMuons_Pt6->GetYaxis()->SetTitle("Phi   (rad)");
	
	
	  TH1 * TotalNumber_of_RPC_hits_per_events_BA = new TH1I("TotalNumber_of_RPC_hits_per_events_BA","TotalNumber_of_RPC_hits_per_events_BA",150,-0.5,149.5);
	  sc = rpcprd_dq_BA.regHist( TotalNumber_of_RPC_hits_per_events_BA );
	  TotalNumber_of_RPC_hits_per_events_BA->SetFillColor(42);
	  TotalNumber_of_RPC_hits_per_events_BA->GetXaxis()->SetTitle("[counts]");
	  TotalNumber_of_RPC_hits_per_events_BA->GetYaxis()->SetTitle("Number of Hits/Events"); 
	 	  
	  TH1 * TotalNumber_of_RPC_hits_per_events_BC = new TH1I("TotalNumber_of_RPC_hits_per_events_BC","TotalNumber_of_RPC_hits_per_events_BC",150,-0.5,149.5);
	  sc = rpcprd_dq_BC.regHist( TotalNumber_of_RPC_hits_per_events_BC );
	  TotalNumber_of_RPC_hits_per_events_BC->SetFillColor(42);
	  TotalNumber_of_RPC_hits_per_events_BC->GetXaxis()->SetTitle("[counts]");
	  TotalNumber_of_RPC_hits_per_events_BC->GetYaxis()->SetTitle("Number of Hits/Events"); 



  
	  //CS
	  TH1 * rpcCSEta_BA = new TH1I("rpcCSEta_BA", "rpcCSEta_BA", 32, -0.5, 31.5);
	  sc = rpcprd_dq_BA.regHist( rpcCSEta_BA );
	  rpcCSEta_BA->SetFillColor(42);
	  rpcCSEta_BA->GetXaxis()->SetTitle("Cluster Size Eta view");
	  rpcCSEta_BA->GetYaxis()->SetTitle("Counts"); 

	  TH1 * rpcCSEta_BC = new TH1I("rpcCSEta_BC", "rpcCSEta_BC", 32, -0.5, 31.5);
	  sc = rpcprd_dq_BC.regHist( rpcCSEta_BC );
	  rpcCSEta_BC->SetFillColor(42);
	  rpcCSEta_BC->GetXaxis()->SetTitle("Cluster Size Eta view");
	  rpcCSEta_BC->GetYaxis()->SetTitle("Counts"); 

	  TH1 * rpcCSPhi_BA = new TH1I("rpcCSPhi_BA", "rpcCSPhi_BA", 32, -0.5, 31.5);
	  sc = rpcprd_dq_BA.regHist( rpcCSPhi_BA );
	  rpcCSPhi_BA->SetFillColor(42);
	  rpcCSPhi_BA->GetXaxis()->SetTitle("Cluster Size Phi view");
	  rpcCSPhi_BA->GetYaxis()->SetTitle("Counts"); 

	  TH1 * rpcCSPhi_BC = new TH1I("rpcCSPhi_BC", "rpcCSPhi_BC", 32, -0.5, 31.5);
	  sc = rpcprd_dq_BC.regHist( rpcCSPhi_BC );
	  rpcCSPhi_BC->SetFillColor(42);
	  rpcCSPhi_BC->GetXaxis()->SetTitle("Cluster Size Phi view");
	  rpcCSPhi_BC->GetYaxis()->SetTitle("Counts"); 

	  // RPC trigger time
	  TH1 * rpctime_LPt_BA = new TH1I("rpctime_LPt_BA", "rpctime_LPt_BA", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BA.regHist( rpctime_LPt_BA );
	  rpctime_LPt_BA->SetFillColor(42);
	  rpctime_LPt_BA->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_LPt_BA->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	 	  
	  TH1 * rpctime_LPt_BC = new TH1I("rpctime_LPt_BC", "rpctime_LPt_BC", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BC.regHist( rpctime_LPt_BC );
	  rpctime_LPt_BC->SetFillColor(42);
	  rpctime_LPt_BC->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_LPt_BC->GetYaxis()->SetTitle("Counts/(3.125ns)"); 

	  TH1 * rpctime_HPt_BA = new TH1I("rpctime_HPt_BA", "rpctime_HPt_BA", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BA.regHist( rpctime_HPt_BA );
	  rpctime_HPt_BA->SetFillColor(42);
	  rpctime_HPt_BA->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_HPt_BA->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	 	  
	  TH1 * rpctime_HPt_BC = new TH1I("rpctime_HPt_BC", "rpctime_HPt_BC", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BC.regHist( rpctime_HPt_BC );
	  rpctime_HPt_BC->SetFillColor(42);
	  rpctime_HPt_BC->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_HPt_BC->GetYaxis()->SetTitle("Counts/(3.125ns)"); 

	  // RPC hit time
	  TH1 * rpctime_BA = new TH1I("rpctime_BA", "rpctime_BA", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BA.regHist( rpctime_BA );
	  rpctime_BA->SetFillColor(42);
	  rpctime_BA->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_BA->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	 	  
	  TH1 * rpctime_BC = new TH1I("rpctime_BC", "rpctime_BC", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BC.regHist( rpctime_BC );
	  rpctime_BC->SetFillColor(42);
	  rpctime_BC->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_BC->GetYaxis()->SetTitle("Counts/(3.125ns)"); 

	  TH1 * rpctime_Sector1_BA = new TH1I("rpctime_Sector1_BA", "rpctime_Sector1_BA", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BA.regHist( rpctime_Sector1_BA );
	  rpctime_Sector1_BA->SetFillColor(42);
	  rpctime_Sector1_BA->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_Sector1_BA->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	
	  TH1 * rpctime_Sector2_BA = new TH1I("rpctime_Sector2_BA", "rpctime_Sector2_BA", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BA.regHist( rpctime_Sector2_BA );
	  rpctime_Sector2_BA->SetFillColor(42);
	  rpctime_Sector2_BA->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_Sector2_BA->GetYaxis()->SetTitle("Counts/(3.125ns)"); 

	  TH1 * rpctime_Sector3_BA = new TH1I("rpctime_Sector3_BA", "rpctime_Sector3_BA", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BA.regHist( rpctime_Sector3_BA );
	  rpctime_Sector3_BA->SetFillColor(42);
	  rpctime_Sector3_BA->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_Sector3_BA->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	
	  TH1 * rpctime_Sector4_BA = new TH1I("rpctime_Sector4_BA", "rpctime_Sector4_BA", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BA.regHist( rpctime_Sector4_BA );
	  rpctime_Sector4_BA->SetFillColor(42);
	  rpctime_Sector4_BA->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_Sector4_BA->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	
	  TH1 * rpctime_Sector5_BA = new TH1I("rpctime_Sector5_BA", "rpctime_Sector5_BA", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BA.regHist( rpctime_Sector5_BA );
	  rpctime_Sector5_BA->SetFillColor(42);
	  rpctime_Sector5_BA->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_Sector5_BA->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	
	  TH1 * rpctime_Sector6_BA = new TH1I("rpctime_Sector6_BA", "rpctime_Sector6_BA", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BA.regHist( rpctime_Sector6_BA );
	  rpctime_Sector6_BA->SetFillColor(42);
	  rpctime_Sector6_BA->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_Sector6_BA->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	
	  TH1 * rpctime_Sector7_BA = new TH1I("rpctime_Sector7_BA", "rpctime_Sector7_BA", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BA.regHist( rpctime_Sector7_BA );
	  rpctime_Sector7_BA->SetFillColor(42);
	  rpctime_Sector7_BA->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_Sector7_BA->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	
	  TH1 * rpctime_Sector8_BA = new TH1I("rpctime_Sector8_BA", "rpctime_Sector8_BA", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BA.regHist( rpctime_Sector8_BA );
	  rpctime_Sector8_BA->SetFillColor(42);
	  rpctime_Sector8_BA->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_Sector8_BA->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	
	  TH1 * rpctime_Sector9_BA = new TH1I("rpctime_Sector9_BA", "rpctime_Sector9_BA", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BA.regHist( rpctime_Sector9_BA );
	  rpctime_Sector9_BA->SetFillColor(42);
	  rpctime_Sector9_BA->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_Sector9_BA->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	
	  TH1 * rpctime_Sector10_BA = new TH1I("rpctime_Sector10_BA", "rpctime_Sector10_BA", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BA.regHist( rpctime_Sector10_BA );
	  rpctime_Sector10_BA->SetFillColor(42);
	  rpctime_Sector10_BA->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_Sector10_BA->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	
	  TH1 * rpctime_Sector11_BA = new TH1I("rpctime_Sector11_BA", "rpctime_Sector11_BA", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BA.regHist( rpctime_Sector11_BA );
	  rpctime_Sector11_BA->SetFillColor(42);
	  rpctime_Sector11_BA->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_Sector11_BA->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	
	  TH1 * rpctime_Sector12_BA = new TH1I("rpctime_Sector12_BA", "rpctime_Sector12_BA", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BA.regHist( rpctime_Sector12_BA );
	  rpctime_Sector12_BA->SetFillColor(42);
	  rpctime_Sector12_BA->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_Sector12_BA->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	
	  TH1 * rpctime_Sector13_BA = new TH1I("rpctime_Sector13_BA", "rpctime_Sector13_BA", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BA.regHist( rpctime_Sector13_BA );
	  rpctime_Sector13_BA->SetFillColor(42);
	  rpctime_Sector13_BA->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_Sector13_BA->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	
	  TH1 * rpctime_Sector14_BA = new TH1I("rpctime_Sector14_BA", "rpctime_Sector14_BA", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BA.regHist( rpctime_Sector14_BA );
	  rpctime_Sector14_BA->SetFillColor(42);
	  rpctime_Sector14_BA->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_Sector14_BA->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	
	  TH1 * rpctime_Sector15_BA = new TH1I("rpctime_Sector15_BA", "rpctime_Sector15_BA", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BA.regHist( rpctime_Sector15_BA );
	  rpctime_Sector15_BA->SetFillColor(42);
	  rpctime_Sector15_BA->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_Sector15_BA->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	
	  TH1 * rpctime_Sector16_BA = new TH1I("rpctime_Sector16_BA", "rpctime_Sector16_BA", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BA.regHist( rpctime_Sector16_BA );
	  rpctime_Sector16_BA->SetFillColor(42);
	  rpctime_Sector16_BA->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_Sector16_BA->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
		

	  TH1 * rpctime_Sector1_BC = new TH1I("rpctime_Sector1_BC", "rpctime_Sector1_BC", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BC.regHist( rpctime_Sector1_BC );
	  rpctime_Sector1_BC->SetFillColor(42);
	  rpctime_Sector1_BC->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_Sector1_BC->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	
	  TH1 * rpctime_Sector2_BC = new TH1I("rpctime_Sector2_BC", "rpctime_Sector2_BC", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BC.regHist( rpctime_Sector2_BC );
	  rpctime_Sector2_BC->SetFillColor(42);
	  rpctime_Sector2_BC->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_Sector2_BC->GetYaxis()->SetTitle("Counts/(3.125ns)"); 

	  TH1 * rpctime_Sector3_BC = new TH1I("rpctime_Sector3_BC", "rpctime_Sector3_BC", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BC.regHist( rpctime_Sector3_BC );
	  rpctime_Sector3_BC->SetFillColor(42);
	  rpctime_Sector3_BC->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_Sector3_BC->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	
	  TH1 * rpctime_Sector4_BC = new TH1I("rpctime_Sector4_BC", "rpctime_Sector4_BC", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BC.regHist( rpctime_Sector4_BC );
	  rpctime_Sector4_BC->SetFillColor(42);
	  rpctime_Sector4_BC->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_Sector4_BC->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	
	  TH1 * rpctime_Sector5_BC = new TH1I("rpctime_Sector5_BC", "rpctime_Sector5_BC", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BC.regHist( rpctime_Sector5_BC );
	  rpctime_Sector5_BC->SetFillColor(42);
	  rpctime_Sector5_BC->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_Sector5_BC->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	
	  TH1 * rpctime_Sector6_BC = new TH1I("rpctime_Sector6_BC", "rpctime_Sector6_BC", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BC.regHist( rpctime_Sector6_BC );
	  rpctime_Sector6_BC->SetFillColor(42);
	  rpctime_Sector6_BC->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_Sector6_BC->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	
	  TH1 * rpctime_Sector7_BC = new TH1I("rpctime_Sector7_BC", "rpctime_Sector7_BC", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BC.regHist( rpctime_Sector7_BC );
	  rpctime_Sector7_BC->SetFillColor(42);
	  rpctime_Sector7_BC->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_Sector7_BC->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	
	  TH1 * rpctime_Sector8_BC = new TH1I("rpctime_Sector8_BC", "rpctime_Sector8_BC", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BC.regHist( rpctime_Sector8_BC );
	  rpctime_Sector8_BC->SetFillColor(42);
	  rpctime_Sector8_BC->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_Sector8_BC->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	
	  TH1 * rpctime_Sector9_BC = new TH1I("rpctime_Sector9_BC", "rpctime_Sector9_BC", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BC.regHist( rpctime_Sector9_BC );
	  rpctime_Sector9_BC->SetFillColor(42);
	  rpctime_Sector9_BC->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_Sector9_BC->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	
	  TH1 * rpctime_Sector10_BC = new TH1I("rpctime_Sector10_BC", "rpctime_Sector10_BC", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BC.regHist( rpctime_Sector10_BC );
	  rpctime_Sector10_BC->SetFillColor(42);
	  rpctime_Sector10_BC->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_Sector10_BC->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	
	  TH1 * rpctime_Sector11_BC = new TH1I("rpctime_Sector11_BC", "rpctime_Sector11_BC", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BC.regHist( rpctime_Sector11_BC );
	  rpctime_Sector11_BC->SetFillColor(42);
	  rpctime_Sector11_BC->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_Sector11_BC->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	
	  TH1 * rpctime_Sector12_BC = new TH1I("rpctime_Sector12_BC", "rpctime_Sector12_BC", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BC.regHist( rpctime_Sector12_BC );
	  rpctime_Sector12_BC->SetFillColor(42);
	  rpctime_Sector12_BC->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_Sector12_BC->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	
	  TH1 * rpctime_Sector13_BC = new TH1I("rpctime_Sector13_BC", "rpctime_Sector13_BC", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BC.regHist( rpctime_Sector13_BC );
	  rpctime_Sector13_BC->SetFillColor(42);
	  rpctime_Sector13_BC->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_Sector13_BC->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	
	  TH1 * rpctime_Sector14_BC = new TH1I("rpctime_Sector14_BC", "rpctime_Sector14_BC", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BC.regHist( rpctime_Sector14_BC );
	  rpctime_Sector14_BC->SetFillColor(42);
	  rpctime_Sector14_BC->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_Sector14_BC->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	
	  TH1 * rpctime_Sector15_BC = new TH1I("rpctime_Sector15_BC", "rpctime_Sector15_BC", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BC.regHist( rpctime_Sector15_BC );
	  rpctime_Sector15_BC->SetFillColor(42);
	  rpctime_Sector15_BC->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_Sector15_BC->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	
	  TH1 * rpctime_Sector16_BC = new TH1I("rpctime_Sector16_BC", "rpctime_Sector16_BC", timeNbin, timeminrange, timemaxrange);
	  sc = rpcprd_dq_BC.regHist( rpctime_Sector16_BC );
	  rpctime_Sector16_BC->SetFillColor(42);
	  rpctime_Sector16_BC->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime_Sector16_BC->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	
	  //////////////////////////////
	  // DQMF shift histograms   
        
	  //
	  TH2 *rpc2DEtaStation=new TH2I("rpc2DEtaStation","rpc2DEtaStation", 13, -6, 7,  16*3, 0, 16*3); 
	  sc=rpc_dqmf_global.regHist(rpc2DEtaStation) ; 
	  rpc2DEtaStation->SetFillColor(42);  
	  rpc2DEtaStation->SetMarkerColor(1);  
	  rpc2DEtaStation->SetMarkerStyle(21);
	  rpc2DEtaStation->SetOption("COLZ");    
	  rpc2DEtaStation->SetMarkerSize(0.2);
	  rpc2DEtaStation->GetXaxis()->SetTitle("<--- SIDE C      Rpc Eta Station    SIDE A --->");
	  rpc2DEtaStation->GetYaxis()->SetTitle("Rpc Sector + 16 * (LPt=0,Piv=1,HPt=2)"); 
	
	  // distribution of hit time  
	  std::string m_generic_path_rpctime = m_generic_path_rpcmonitoring+"/GLOBAL";
	  std::string m_rpctime_title      = "Time_Distribution"          ;
	  const char* m_rpctime_title_char = m_rpctime_title.c_str();  

	  TH1 *rpctime=new TH1F(m_rpctime_title_char, m_rpctime_title_char, timeNbin, timeminrange, timemaxrange);	    
	  sc=rpcprd_shift.regHist(rpctime) ;  
	  if(sc.isFailure())
	    { ATH_MSG_FATAL(  "rpc_time_distribution Failed to register histogram " );       
	      return sc;
	    }
	  rpctime->SetFillColor(42);
	  rpctime->GetXaxis()->SetTitle("Time  [ns]");
	  rpctime->GetYaxis()->SetTitle("Counts/(3.125ns)"); 
	 
	  ATH_MSG_DEBUG (  "INSIDE bookHistograms : " << rpctime << m_rpctime_title.c_str() );
	  //ATH_MSG_DEBUG (  "SHIFT : " << shift );
	  ATH_MSG_DEBUG (  "RUN : " << run );	       
	  ATH_MSG_DEBUG (  "Booked bookrpctimedistribution successfully" );     
          
	  
	  
	  
	  //////////////////////////////
	  // DQMF 1D shift histograms   
         
	  // station trigger hits SIDE A
	  TH1 *rpc1DStationNameTriggerHitsSideA= new TH1F("rpc1DStationNameTriggerHitsSideA","rpc1DStationNameTriggerHitsSideA", 12, 0, 12); 
	  
	  rpc1DStationNameTriggerHitsSideA->GetXaxis()->SetBinLabel( 1,"BMS #eta");
	  rpc1DStationNameTriggerHitsSideA->GetXaxis()->SetBinLabel( 2,"BMS #phi");
	  rpc1DStationNameTriggerHitsSideA->GetXaxis()->SetBinLabel( 3,"BML #eta");
	  rpc1DStationNameTriggerHitsSideA->GetXaxis()->SetBinLabel( 4,"BML #phi");
	  rpc1DStationNameTriggerHitsSideA->GetXaxis()->SetBinLabel( 5,"BMF #eta");
	  rpc1DStationNameTriggerHitsSideA->GetXaxis()->SetBinLabel( 6,"BMF #phi");
	  rpc1DStationNameTriggerHitsSideA->GetXaxis()->SetBinLabel( 7,"BOS #eta");
	  rpc1DStationNameTriggerHitsSideA->GetXaxis()->SetBinLabel( 8,"BOS #phi");
	  rpc1DStationNameTriggerHitsSideA->GetXaxis()->SetBinLabel( 9,"BOL #eta");
	  rpc1DStationNameTriggerHitsSideA->GetXaxis()->SetBinLabel(10,"BOL #phi");
	  rpc1DStationNameTriggerHitsSideA->GetXaxis()->SetBinLabel(11,"BOF/G #eta");
	  rpc1DStationNameTriggerHitsSideA->GetXaxis()->SetBinLabel(12,"BOF/G #phi");
	  
	  rpc1DStationNameTriggerHitsSideA->SetFillColor(42);
	  rpc1DStationNameTriggerHitsSideA->SetTitle("RPC trigger hits surface density Side A ");
	  rpc1DStationNameTriggerHitsSideA->GetYaxis()->SetTitle("RPC trigger hits [cm-2]");
	  rpc1DStationNameTriggerHitsSideA->GetXaxis()->SetTitle("RPC Station Name");
	  
	  sc=rpcprd_dq_BA.regHist(rpc1DStationNameTriggerHitsSideA) ;  
	  if(sc.isFailure())
	    { ATH_MSG_FATAL(  "rpc1DStationNameTriggerHitsSideA Failed to register histogram " );       
	      return sc;
	    }	       
	  ATH_MSG_DEBUG (  "Booked  rpc1DStationNameTriggerHitsSideA successfully" );  
	  
  
         
	  // station hits SIDE A
	  TH1 *rpc1DStationNameHitsSideA= new TH1F("rpc1DStationNameHitsSideA","rpc1DStationNameHitsSideA", 12, 0, 12); 
	  
	  rpc1DStationNameHitsSideA->GetXaxis()->SetBinLabel( 1,"BMS #eta");
	  rpc1DStationNameHitsSideA->GetXaxis()->SetBinLabel( 2,"BMS #phi");
	  rpc1DStationNameHitsSideA->GetXaxis()->SetBinLabel( 3,"BML #eta");
	  rpc1DStationNameHitsSideA->GetXaxis()->SetBinLabel( 4,"BML #phi");
	  rpc1DStationNameHitsSideA->GetXaxis()->SetBinLabel( 5,"BMF #eta");
	  rpc1DStationNameHitsSideA->GetXaxis()->SetBinLabel( 6,"BMF #phi");
	  rpc1DStationNameHitsSideA->GetXaxis()->SetBinLabel( 7,"BOS #eta");
	  rpc1DStationNameHitsSideA->GetXaxis()->SetBinLabel( 8,"BOS #phi");
	  rpc1DStationNameHitsSideA->GetXaxis()->SetBinLabel( 9,"BOL #eta");
	  rpc1DStationNameHitsSideA->GetXaxis()->SetBinLabel(10,"BOL #phi");
	  rpc1DStationNameHitsSideA->GetXaxis()->SetBinLabel(11,"BOF/G #eta");
	  rpc1DStationNameHitsSideA->GetXaxis()->SetBinLabel(12,"BOF/G #phi");;
	  
	  rpc1DStationNameHitsSideA->SetFillColor(42);
	  rpc1DStationNameHitsSideA->SetTitle("RPC  hits surface density Side A ");
	  rpc1DStationNameHitsSideA->GetYaxis()->SetTitle("RPC hits [cm-2]");
	  rpc1DStationNameHitsSideA->GetXaxis()->SetTitle("RPC Station Name");
	  
	  sc=rpcprd_dq_BA.regHist(rpc1DStationNameHitsSideA) ;  
	  if(sc.isFailure())
	    { ATH_MSG_FATAL(  "rpc1DStationNameHitsSideA Failed to register histogram " );       
	      return sc;
	    }	       
	  ATH_MSG_DEBUG (  "Booked  rpc1DStationNameHitsSideA successfully" ); 	   
         
	  // station trigger hits SIDE C
	  TH1 *rpc1DStationNameTriggerHitsSideC= new TH1F("rpc1DStationNameTriggerHitsSideC","rpc1DStationNameTriggerHitsSideC", 12, 0, 12); 
	  
	  rpc1DStationNameTriggerHitsSideC->GetXaxis()->SetBinLabel( 1,"BMS #eta");
	  rpc1DStationNameTriggerHitsSideC->GetXaxis()->SetBinLabel( 2,"BMS #phi");
	  rpc1DStationNameTriggerHitsSideC->GetXaxis()->SetBinLabel( 3,"BML #eta");
	  rpc1DStationNameTriggerHitsSideC->GetXaxis()->SetBinLabel( 4,"BML #phi");
	  rpc1DStationNameTriggerHitsSideC->GetXaxis()->SetBinLabel( 5,"BMF #eta");
	  rpc1DStationNameTriggerHitsSideC->GetXaxis()->SetBinLabel( 6,"BMF #phi");
	  rpc1DStationNameTriggerHitsSideC->GetXaxis()->SetBinLabel( 7,"BOS #eta");
	  rpc1DStationNameTriggerHitsSideC->GetXaxis()->SetBinLabel( 8,"BOS #phi");
	  rpc1DStationNameTriggerHitsSideC->GetXaxis()->SetBinLabel( 9,"BOL #eta");
	  rpc1DStationNameTriggerHitsSideC->GetXaxis()->SetBinLabel(10,"BOL #phi");
	  rpc1DStationNameTriggerHitsSideC->GetXaxis()->SetBinLabel(11,"BOF/G #eta");
	  rpc1DStationNameTriggerHitsSideC->GetXaxis()->SetBinLabel(12,"BOF/G #phi"); 
	  
	  rpc1DStationNameTriggerHitsSideC->SetFillColor(42);
	  rpc1DStationNameTriggerHitsSideC->SetTitle("RPC trigger hits surface density Side C ");
	  rpc1DStationNameTriggerHitsSideC->GetYaxis()->SetTitle("RPC trigger hits [cm-2]");
	  rpc1DStationNameTriggerHitsSideC->GetXaxis()->SetTitle("RPC Station Name");
	  
	  sc=rpcprd_dq_BC.regHist(rpc1DStationNameTriggerHitsSideC) ;  
	  if(sc.isFailure())
	    { ATH_MSG_FATAL(  "rpc1DStationNameTriggerHitsSideC Failed to register histogram " );       
	      return sc;
	    }	       
	  ATH_MSG_DEBUG (  "Booked  rpc1DStationNameTriggerHitsSideC successfully" );  
	  
  
         
	  // station hits SIDE C
	  TH1 *rpc1DStationNameHitsSideC= new TH1F("rpc1DStationNameHitsSideC","rpc1DStationNameHitsSideC", 12, 0, 12); 
	  
	  rpc1DStationNameHitsSideC->GetXaxis()->SetBinLabel( 1,"BMS #eta");
	  rpc1DStationNameHitsSideC->GetXaxis()->SetBinLabel( 2,"BMS #phi");
	  rpc1DStationNameHitsSideC->GetXaxis()->SetBinLabel( 3,"BML #eta");
	  rpc1DStationNameHitsSideC->GetXaxis()->SetBinLabel( 4,"BML #phi");
	  rpc1DStationNameHitsSideC->GetXaxis()->SetBinLabel( 5,"BMF #eta");
	  rpc1DStationNameHitsSideC->GetXaxis()->SetBinLabel( 6,"BMF #phi");
	  rpc1DStationNameHitsSideC->GetXaxis()->SetBinLabel( 7,"BOS #eta");
	  rpc1DStationNameHitsSideC->GetXaxis()->SetBinLabel( 8,"BOS #phi");
	  rpc1DStationNameHitsSideC->GetXaxis()->SetBinLabel( 9,"BOL #eta");
	  rpc1DStationNameHitsSideC->GetXaxis()->SetBinLabel(10,"BOL #phi");
	  rpc1DStationNameHitsSideC->GetXaxis()->SetBinLabel(11,"BOF/G #eta");
	  rpc1DStationNameHitsSideC->GetXaxis()->SetBinLabel(12,"BOF/G #phi");
	  
	  rpc1DStationNameHitsSideC->SetFillColor(42);
	  rpc1DStationNameHitsSideC->SetTitle("RPC  hits surface density Side C ");
	  rpc1DStationNameHitsSideC->GetYaxis()->SetTitle("RPC hits [cm-2]");
	  rpc1DStationNameHitsSideC->GetXaxis()->SetTitle("RPC Station Name");
	  
	  sc=rpcprd_dq_BC.regHist(rpc1DStationNameHitsSideC) ;  
	  if(sc.isFailure())
	    { ATH_MSG_FATAL(  "rpc1DStationNameHitsSideC Failed to register histogram " );       
	      return sc;
	    }	       
	  ATH_MSG_DEBUG (  "Booked  rpc1DStationNameHitsSideC successfully" );  
	  ////////////////////////////////////
	
	  // cool histogram
	  // strip profile -> noise and dead strips
	  if ( m_doCoolDB ) {
	    //DB_list.push_back( "StripId" );
	    DB_list.push_back( "PanelId" );
	    DB_list.push_back( "Profile" );
	
	    for ( std::vector<std::string>::const_iterator iter=DB_list.begin(); iter!=DB_list.end(); iter++ ) {
	      for ( int isec=0; isec!=15+1; isec++ ) {
		for ( int idblPhi=0; idblPhi!=2; idblPhi ++) {
		  bookRPCCoolHistograms( iter, isec, idblPhi, "Pivot0" ) ;
		  bookRPCCoolHistograms( iter, isec, idblPhi, "Pivot1" ) ;
		  bookRPCCoolHistograms( iter, isec, idblPhi, "LowPt0" ) ;
		  bookRPCCoolHistograms( iter, isec, idblPhi, "LowPt1" ) ;
		  bookRPCCoolHistograms( iter, isec, idblPhi, "HighPt0") ;
		  bookRPCCoolHistograms( iter, isec, idblPhi, "HighPt1") ;
		}
	      }
	    }
	  } // end if (m_doCoolDB)
	
	
 
	} // end if isNewRun
    }}//m_doRpcESD // AthenaMonManager::tier0 || AthenaMonManager::tier0ESD  
  return sc;
}

void RpcRawDataValAlg::bookRPCLayerHistograms(std::string m_hardware_name, std::string m_layer_name, std::string m_layer0_name, int bin, int binmin, int binmax )
{
  //gErrorIgnoreLevel=kError;
  gErrorIgnoreLevel=kInfo;
  
  StatusCode sc = StatusCode::SUCCESS;
  		    
  if( m_doRpcESD==true ) {if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online ) {  
  
      //declare a group of histograms
      std::string m_generic_path_rpcmonitoring = "Muon/MuonRawDataMonitoring/RPC";
      MonGroup rpcprd_expert( this, m_generic_path_rpcmonitoring+"/Chambers/"+m_hardware_name+"/Profiles/", run, ATTRIB_UNMANAGED );   
      MuonDQAHistList& lst = m_stationHists.getList( m_hardware_name +"/Profiles/" + m_layer_name);
     
      //histo path for rpc strip 
      std::string m_generic_path_rpcstriplayer = m_generic_path_rpcmonitoring+"/Chambers/"+m_hardware_name+"/Profiles/";
      m_generic_path_rpcstriplayer += m_hardware_name + "_" + m_layer_name + "_strip";
      std::string m_rpcstriplayer_title = m_hardware_name + "_" + m_layer_name + "_strip";	
      const char* m_rpcstriplayer_title_char = m_rpcstriplayer_title.c_str();
		
      TH1 *rpcstriplayer=new TH1I(m_rpcstriplayer_title_char, m_rpcstriplayer_title_char, bin/m_rpcreducenbinsstrip, binmin, binmax ); 			
      lst.addHist(rpcstriplayer);  
      rpcstriplayer->SetFillColor(42); 
      rpcstriplayer->GetXaxis()->SetTitle(m_layer0_name.c_str());
      rpcstriplayer->GetYaxis()->SetTitle("Counts/Strip");

    
      ATH_MSG_DEBUG (  "INSIDE bookRPCLayerHistograms : " << rpcstriplayer << m_generic_path_rpcstriplayer.c_str() );
      //ATH_MSG_DEBUG (  "SHIFT : " << shift );
      ATH_MSG_DEBUG (  "RUN : " << run );
  
      sc = rpcprd_expert.regHist( rpcstriplayer );
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register rpcstriplayer hist to MonGroup" );
  
		
      //histo path for rpc cluster
      //size per strip
      std::string m_generic_path_rpcclustersizelayer = m_generic_path_rpcmonitoring+"/Chambers/"+m_hardware_name+"/Profiles/";
      m_generic_path_rpcclustersizelayer += m_hardware_name + "_" + m_layer_name + "_clustersize";
      std::string m_rpcclustersizelayer_title = m_hardware_name + "_" + m_layer_name + "_clustersize";	
      const char* m_rpcclustersizelayer_title_char = m_rpcclustersizelayer_title.c_str();

      TH2 *rpcclustersizelayer=new TH2I(m_rpcclustersizelayer_title_char, m_rpcclustersizelayer_title_char, bin/m_rpcreducenbins, binmin, binmax , 10, -0.5, 9.5); 			
      lst.addHist(rpcclustersizelayer);  
      rpcclustersizelayer->SetFillColor(42);  
      rpcclustersizelayer->SetOption("COLZ");  
      rpcclustersizelayer->GetXaxis()->SetTitle(m_layer0_name.c_str());
      rpcclustersizelayer->GetYaxis()->SetTitle("Cluster Size");

     
      ATH_MSG_DEBUG (  "INSIDE bookRPCLayerHistograms : " << rpcclustersizelayer << m_generic_path_rpcclustersizelayer.c_str() );
     // ATH_MSG_DEBUG (  "SHIFT : " << shift );
      ATH_MSG_DEBUG (  "RUN : " << run );    
   		     	
      sc = rpcprd_expert.regHist( rpcclustersizelayer );
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register rpcclustersizelayer hist to MonGroup" );

    
      //profile
      std::string m_generic_path_rpcclusterlayer = m_generic_path_rpcmonitoring+"/Chambers/"+m_hardware_name+"/Profiles/";  
      m_generic_path_rpcclusterlayer += m_hardware_name + "_" + m_layer_name + "_cluster";
      std::string m_rpcclusterlayer_title = m_hardware_name + "_" + m_layer_name + "_cluster";	
      const char* m_rpcclusterlayer_title_char = m_rpcclusterlayer_title.c_str();
  
      TH1 *rpcclusterlayer=new TH1I(m_rpcclusterlayer_title_char, m_rpcclusterlayer_title_char, bin/m_rpcreducenbins, binmin, binmax); 			
      lst.addHist(rpcclusterlayer);  
      rpcclusterlayer->SetFillColor(42); 
      rpcclusterlayer->GetXaxis()->SetTitle(m_layer0_name.c_str());
      rpcclusterlayer->GetYaxis()->SetTitle("Counts/strip");

     
      ATH_MSG_DEBUG (  "INSIDE bookRPCLayerHistograms : " << rpcclusterlayer << m_generic_path_rpcclusterlayer.c_str() );
     // ATH_MSG_DEBUG (  "SHIFT : " << shift );
      ATH_MSG_DEBUG (  "RUN : " << run );    
  
      sc = rpcprd_expert.regHist( rpcclusterlayer );
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register rpcclusterlayer hist to MonGroup" );
  
    }}//m_doRpcESD // AthenaMonManager::tier0 || AthenaMonManager::tier0ESD  
 
}
		
void RpcRawDataValAlg::bookRPCLayerHistogramsPanel(std::string m_hardware_name, std::string m_layer_name )
{
  //gErrorIgnoreLevel=kError;
  gErrorIgnoreLevel=kInfo;

  StatusCode sc = StatusCode::SUCCESS;
   		     	
  if( m_doRpcESD==true ) {if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online ) {    		  
      //declare a group of histograms
      std::string m_generic_path_rpcmonitoring = "Muon/MuonRawDataMonitoring/RPC";
      MonGroup rpcprd_expert( this, m_generic_path_rpcmonitoring+"/Chambers/"+m_hardware_name+"/Panels/", run, ATTRIB_UNMANAGED );   
      MuonDQAHistList& lst = m_stationHists.getList( m_hardware_name +"/Panels/" + m_layer_name);

      //histo path for rpc strip 
 
      //size distribution
      std::string m_generic_path_rpcclustersizedislayer = m_generic_path_rpcmonitoring+"/Chambers/"+m_hardware_name+"/Panels/";
      m_generic_path_rpcclustersizedislayer += m_hardware_name + "_" + m_layer_name + "_CSdistribution";
      std::string m_rpcclustersizedislayer_title = m_hardware_name + "_" + m_layer_name + "_CSdistribution";	
      const char* m_rpcclustersizedislayer_title_char = m_rpcclustersizedislayer_title.c_str();

      TH1F *rpcclustersizedislayer=new TH1F(m_rpcclustersizedislayer_title_char, m_rpcclustersizedislayer_title_char, 32, -0.5, 31.5 ); 			
      lst.addHist(rpcclustersizedislayer);  
      rpcclustersizedislayer->SetFillColor(42); 
      rpcclustersizedislayer->GetXaxis()->SetTitle(m_layer_name.c_str());
      rpcclustersizedislayer->GetYaxis()->SetTitle("Cluster Size");

     
      ATH_MSG_DEBUG (  "INSIDE bookRPCLayerHistogramsPanels : " << rpcclustersizedislayer << m_generic_path_rpcclustersizedislayer.c_str() );
      //ATH_MSG_DEBUG (  "EXPERT : " << expert );
      ATH_MSG_DEBUG (  "RUN : " << run );     

      sc = rpcprd_expert.regHist( rpcclustersizedislayer );
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register rpcclustersizedislayer hist to MonGroup" );
  
    }}//m_doRpcESD // AthenaMonManager::tier0 || AthenaMonManager::tier0ESD  
  
}
		
  
void RpcRawDataValAlg::bookRPCLayervsTimeHistograms(std::string  m_hardware_name, std::string layer_name, int bin, int binmin, int binmax)
{
  //gErrorIgnoreLevel=kError;
  gErrorIgnoreLevel=kInfo;
  
  StatusCode sc = StatusCode::SUCCESS;

  if( m_doRpcESD==true ) {if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online ) {    	  
      //declare a group of histograms
      std::string m_generic_path_rpcmonitoring = "Muon/MuonRawDataMonitoring/RPC";
      MonGroup rpcprd_expert( this, m_generic_path_rpcmonitoring+"/Chambers/"+m_hardware_name+"/ProfilesvsTime/", run, ATTRIB_UNMANAGED );   
      MuonDQAHistList& lst = m_stationHists.getList( m_hardware_name +"/ProfilesvsTime/" + layer_name);

      //histo path for rpc strip vs time 
      std::string m_generic_path_rpcstripvstimelayer = m_generic_path_rpcmonitoring+"/Chambers/"+m_hardware_name+"/ProfilesvsTime/";
      m_generic_path_rpcstripvstimelayer += m_hardware_name + "_" +  layer_name  +  "_stripvstime";
      std::string m_rpcstripvstimelayer_title = m_hardware_name + "_" + layer_name + "_stripvstime";	
      const char* m_rpcstripvstimelayer_title_char = m_rpcstripvstimelayer_title.c_str();

      TH2 *rpcstripvstimelayer=new TH2I(m_rpcstripvstimelayer_title_char,m_rpcstripvstimelayer_title_char, bin/m_rpcreducenbins, binmin, binmax,timeNbin, timeminrange, timemaxrange); 			
      lst.addHist(rpcstripvstimelayer);
      rpcstripvstimelayer->SetMarkerColor(1);  
      rpcstripvstimelayer->SetMarkerStyle(21);  
      rpcstripvstimelayer->SetOption("COLZ");   
      rpcstripvstimelayer->SetMarkerSize(0.2);
      rpcstripvstimelayer->GetXaxis()->SetTitle("Rpc strip");
      rpcstripvstimelayer->GetYaxis()->SetTitle("Time[ns]");  

    
      ATH_MSG_DEBUG (  "INSIDE bookRPCLayerHistograms : " << rpcstripvstimelayer << m_generic_path_rpcstripvstimelayer.c_str() );
     // ATH_MSG_DEBUG (  "EXPERT : " << expert );
      ATH_MSG_DEBUG (  "RUN : " << run ); 
		
      sc = rpcprd_expert.regHist( rpcstripvstimelayer );  
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register rpcstripvstimelayer hist to MonGroup" );

    }}//m_doRpcESD // AthenaMonManager::tier0 || AthenaMonManager::tier0ESD  
}

void RpcRawDataValAlg::bookRPCLayerPhiAmbiHistograms(std::string m_hardware_name, std::string m_layer_name, std::string m_layer0_name, int bin, int binmin, int binmax )
{
  // gErrorIgnoreLevel=kError;
  gErrorIgnoreLevel=kInfo;

  StatusCode sc = StatusCode::SUCCESS;

  if( m_doRpcESD==true ) {if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online ) {    		    
      //declare a group of histograms
      std::string m_generic_path_rpcmonitoring = "Muon/MuonRawDataMonitoring/RPC";
      MonGroup rpcprd_expert( this, m_generic_path_rpcmonitoring+"/Chambers/"+m_hardware_name+"/Phi_profiles_ambiguity_resolved/", run, ATTRIB_UNMANAGED );   
      MuonDQAHistList& lst = m_stationHists.getList( m_hardware_name +"/Phi_profiles_ambiguity_resolved/" + m_layer_name);
  
      //histo path for phi rpc strip with ambiguity removal
      std::string m_generic_path_rpcstripPhiAmbilayer = m_generic_path_rpcmonitoring+"/Chambers/"+m_hardware_name+"/phi_profiles_ambiguity_resolved";
      m_generic_path_rpcstripPhiAmbilayer += m_hardware_name + "_" + m_layer_name + "_Ambiguity_resolved_phi_strip";
      std::string m_rpcstripPhiAmbilayer_title = m_hardware_name + "_" + m_layer_name +"_Ambiguity_resolved_phi_strip";	
      const char* m_rpcstripPhiAmbilayer_title_char = m_rpcstripPhiAmbilayer_title.c_str();
   		    
      TH1 *rpcstripPhiAmbilayer=new TH1I(m_rpcstripPhiAmbilayer_title_char,m_rpcstripPhiAmbilayer_title_char, bin/m_rpcreducenbinsstrip, binmin, binmax );   
      lst.addHist(rpcstripPhiAmbilayer);
      rpcstripPhiAmbilayer->SetFillColor(42); 
      rpcstripPhiAmbilayer->GetXaxis()->SetTitle(m_layer0_name.c_str());
      rpcstripPhiAmbilayer->GetYaxis()->SetTitle("Counts/Strip");
  
     
      ATH_MSG_DEBUG (  "INSIDE bookRPCLayerPhiAmbiHistograms : " << rpcstripPhiAmbilayer << m_generic_path_rpcstripPhiAmbilayer.c_str() );
      //ATH_MSG_DEBUG (  "EXPERT : " << expert );
      ATH_MSG_DEBUG (  "RUN : " << run );

      sc = rpcprd_expert.regHist( rpcstripPhiAmbilayer ); 
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register rpcstripPhiAmbilayer hist to MonGroup" );
   		     	
    }}//m_doRpcESD // AthenaMonManager::tier0 || AthenaMonManager::tier0ESD  
  
}

void RpcRawDataValAlg::bookRPCLayerPhivsEtaHistograms(std::string m_hardware_name, std::string m_layerPhivsEta_name, int binz, int binminz, int binmaxz, int binx, int binminx, int binmaxx )
{
  //gErrorIgnoreLevel=kError;
  gErrorIgnoreLevel=kInfo;

  
  StatusCode sc = StatusCode::SUCCESS;
  if ( binmaxx==64  ) { binmaxx=96 ; } //exception for SU2 / SU3 chambers
		
  if( m_doRpcESD==true ) {if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online ) {  
    
      //declare a group of histograms
      std::string m_generic_path_rpcmonitoring = "Muon/MuonRawDataMonitoring/RPC";
      MonGroup rpcprd_expert( this, m_generic_path_rpcmonitoring+"/Chambers/"+m_hardware_name+"/PhivsEta/", run, ATTRIB_UNMANAGED );   
      MuonDQAHistList& lst = m_stationHists.getList( m_hardware_name + "/PhivsEta/" + m_layerPhivsEta_name);

      //histo path for rpc strip 
      std::string m_generic_path_rpcstriplayerPhivsEta = m_generic_path_rpcmonitoring+"/Chambers/"+ m_hardware_name + "/PhivsEta/";
      m_generic_path_rpcstriplayerPhivsEta += m_hardware_name +  "_" + m_layerPhivsEta_name;
      std::string m_rpcstriplayerPhivsEta_title = m_hardware_name + "_" + m_layerPhivsEta_name;	
      const char* m_rpcstriplayerPhivsEta_title_char = m_rpcstriplayerPhivsEta_title.c_str();
   		     	
      TH2 *rpcstriplayerPhivsEta=new TH2I(m_rpcstriplayerPhivsEta_title_char,m_rpcstriplayerPhivsEta_title_char, binz/m_rpcreducenbins, binminz, binmaxz, binx/m_rpcreducenbins, binminx, binmaxx); 		
  
      lst.addHist(rpcstriplayerPhivsEta);
      rpcstriplayerPhivsEta->SetFillColor(42);  
      rpcstriplayerPhivsEta->SetMarkerColor(1);  
      rpcstriplayerPhivsEta->SetMarkerStyle(21);  
      rpcstriplayerPhivsEta->SetOption("COLZ");    
      rpcstriplayerPhivsEta->SetMarkerSize(0.2);
      rpcstriplayerPhivsEta->GetXaxis()->SetTitle("<--- IP          Rpc Eta strip     EC --->"     );
      rpcstriplayerPhivsEta->GetYaxis()->SetTitle("<--- HV side     Rpc Phi strip     RO side --->");
     
      ATH_MSG_DEBUG (  "INSIDE bookRPCLayerPhivsEtaHistograms : " << rpcstriplayerPhivsEta << m_generic_path_rpcstriplayerPhivsEta.c_str() );
     // ATH_MSG_DEBUG (  "EXPERT : " << expert );
      ATH_MSG_DEBUG (  "RUN : " << run );
	   
      sc  = rpcprd_expert.regHist( rpcstriplayerPhivsEta ); 
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register rpcstriplayerPhivsEta hist to MonGroup" );

    }}//m_doRpcESD // AthenaMonManager::tier0 || AthenaMonManager::tier0ESD    
}
   		     	

//void RpcRawDataValAlg::bookRPCLayerPhivsEtaSectorHistograms(std::string m_hardware_name,std::string m_sector_name, std::string m_layerPhivsEtaSector_name, int binz, int binminz, int binmaxz, int binx, int binminx, int binmaxx )
void RpcRawDataValAlg::bookRPCLayerPhivsEtaSectorHistograms(std::string m_sector_name, std::string m_layerPhivsEtaSector_name, int binz, int binminz, int binmaxz, int binx, int binminx, int binmaxx )
{
  //  gErrorIgnoreLevel=kError;
  gErrorIgnoreLevel=kInfo;
  if ( binmaxx==64  ) { binmaxx=96 ; } //exception for SU2 / SU3 chambers
  if ( binmaxx==112 ) { binmaxx=128; } //exception for BML7 chambers

  StatusCode sc = StatusCode::SUCCESS;

  if( m_doRpcESD==true ) {if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online ) { 
      //declare a group of histograms
      std::string m_generic_path_rpcmonitoring = "Muon/MuonRawDataMonitoring/RPC";
      MonGroup rpcprd_expert( this, m_generic_path_rpcmonitoring+"/Sectors/"+m_sector_name + "/PhivsEta/" , run, ATTRIB_UNMANAGED );   
      MuonDQAHistList& lst = m_stationHists.getList( m_sector_name + "/PhivsEta/" + m_layerPhivsEtaSector_name); 
 
      //histo path for rpc strip 
      std::string m_generic_path_rpcstriplayerPhivsEtaSector = m_generic_path_rpcmonitoring+"/Sectors/"+m_sector_name+"/PhivsEta/";
      m_generic_path_rpcstriplayerPhivsEtaSector += m_layerPhivsEtaSector_name;
      std::string m_rpcstriplayerPhivsEtaSector_title = m_layerPhivsEtaSector_name; 	
      const char* m_rpcstriplayerPhivsEtaSector_title_char = m_rpcstriplayerPhivsEtaSector_title.c_str();
 
      TH2 *rpcstriplayerPhivsEtaSector=new TH2I(m_rpcstriplayerPhivsEtaSector_title_char,m_rpcstriplayerPhivsEtaSector_title_char, binz/m_rpcreducenbins, binminz, binmaxz, binx/m_rpcreducenbins, binminx, binmaxx); 
      lst.addHist(rpcstriplayerPhivsEtaSector);
      rpcstriplayerPhivsEtaSector->SetFillColor(42);  
      rpcstriplayerPhivsEtaSector->SetMarkerColor(1);  
      rpcstriplayerPhivsEtaSector->SetMarkerStyle(21);  
      rpcstriplayerPhivsEtaSector->SetOption("COLZ");    
      rpcstriplayerPhivsEtaSector->SetMarkerSize(0.2);
      rpcstriplayerPhivsEtaSector->GetXaxis()->SetTitle("<--- SIDE C      Rpc Eta strip      SIDE A --->");
      //rpcstriplayerPhivsEtaSector->GetYaxis()->SetTitle( m_layer0_name );
      rpcstriplayerPhivsEtaSector->GetYaxis()->SetTitle("<--- HV side     Rpc Phi strip     RO side --->");
      
      ATH_MSG_DEBUG (  "INSIDE bookRPCLayerPhivsEtaSectorHistograms : " << rpcstriplayerPhivsEtaSector << m_generic_path_rpcstriplayerPhivsEtaSector.c_str() );
     // ATH_MSG_DEBUG (  "EXPERT : " << expert );
      ATH_MSG_DEBUG (  "RUN : " << run );
    
      sc = rpcprd_expert.regHist( rpcstriplayerPhivsEtaSector ); 
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register rpcstriplayerPhivsEtaSector hist to MonGroup" );
 
 
      //histo path for rpc cluster 
      if ( m_layerPhivsEtaSector_name.find("Trigger", 0) == string::npos ) { 
	std::string m_generic_path_rpcclusterlayerPhivsEtaSector = m_generic_path_rpcmonitoring+"/Sectors/"+m_sector_name+"/PhivsEta/";
	m_generic_path_rpcclusterlayerPhivsEtaSector += m_layerPhivsEtaSector_name + "_cluster"  ;
	std::string m_rpcclusterlayerPhivsEtaSector_title = m_layerPhivsEtaSector_name + "_cluster"   ; 	
	const char* m_rpcclusterlayerPhivsEtaSector_title_char = m_rpcclusterlayerPhivsEtaSector_title.c_str();
    
	TH2 *rpcclusterlayerPhivsEtaSector=new TH2I(m_rpcclusterlayerPhivsEtaSector_title_char,m_rpcclusterlayerPhivsEtaSector_title_char, binz/m_rpcreducenbins, binminz, binmaxz, binx/m_rpcreducenbins, binminx, binmaxx); 
	lst.addHist(rpcclusterlayerPhivsEtaSector);
	rpcclusterlayerPhivsEtaSector->SetFillColor(42);  
	rpcclusterlayerPhivsEtaSector->SetMarkerColor(1);  
	rpcclusterlayerPhivsEtaSector->SetMarkerStyle(21);  
	rpcclusterlayerPhivsEtaSector->SetOption("COLZ");    
	rpcclusterlayerPhivsEtaSector->SetMarkerSize(0.2);
	rpcclusterlayerPhivsEtaSector->GetXaxis()->SetTitle("<--- SIDE C      Rpc Eta strip      SIDE A --->");
	rpcclusterlayerPhivsEtaSector->GetYaxis()->SetTitle("<--- HV side     Rpc Phi strip     RO side --->");
    
       
        ATH_MSG_DEBUG (  "INSIDE bookRPCLayerPhivsEtaSectorHistograms : " << rpcclusterlayerPhivsEtaSector << m_generic_path_rpcclusterlayerPhivsEtaSector.c_str() );
       // ATH_MSG_DEBUG (  "EXPERT : " << expert );
        ATH_MSG_DEBUG (  "RUN : " << run );
    
	sc = rpcprd_expert.regHist( rpcclusterlayerPhivsEtaSector ); 
	if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register rpcclusterlayerPhivsEtaSector hist to MonGroup" );
      }
    
    }}//m_doRpcESD // AthenaMonManager::tier0 || AthenaMonManager::tier0ESD  
   
}
 
    
void RpcRawDataValAlg::bookRPCLayervsLayerHistograms(std::string m_hardware_name, std::string m_layervslayer_name, 
                                                     std::string   m_layer1_name, std::string m_layer2_name, 
						     int binx, int binminx, int binmaxx, int biny, int binminy, int binmaxy)
{
  // gErrorIgnoreLevel=kError;
  gErrorIgnoreLevel=kInfo;
    
  StatusCode sc = StatusCode::SUCCESS;
    
  if( m_doRpcESD==true ) {if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online ) {    	    
      //declare a group of histograms
      std::string m_generic_path_rpcmonitoring = "Muon/MuonRawDataMonitoring/RPC";
      MonGroup rpcprd_expert( this, m_generic_path_rpcmonitoring+"/Chambers/"+m_hardware_name+"/Layer2vsLayer1/", run, ATTRIB_UNMANAGED );   
      MuonDQAHistList& lst = m_stationHists.getList( m_hardware_name +"/Layer2vsLayer1/"+m_layervslayer_name);
   
      //histo path for rpc strip 
      std::string m_generic_path_rpcstriplayervslayer = m_generic_path_rpcmonitoring+"/Chambers/"+m_hardware_name+"/Layer2vsLayer1/";
      m_generic_path_rpcstriplayervslayer += m_hardware_name + "_" + m_layervslayer_name   ;
      std::string m_rpcstriplayervslayer_title = m_hardware_name + "_" + m_layervslayer_name    ; 	
      const char* m_rpcstriplayervslayer_title_char = m_rpcstriplayervslayer_title.c_str();
 
      TH2 *rpcstriplayervslayer=new TH2I(m_rpcstriplayervslayer_title_char,m_rpcstriplayervslayer_title_char, 
					 binx/m_rpcreducenbins, binminx, binmaxx, biny/m_rpcreducenbins, binminy, binmaxy );	
      lst.addHist(rpcstriplayervslayer);
      rpcstriplayervslayer->SetFillColor(42);   
      rpcstriplayervslayer->SetMarkerColor(1);     
      rpcstriplayervslayer->SetMarkerStyle(21);  
      rpcstriplayervslayer->SetOption("COLZ"); 
      rpcstriplayervslayer->SetMarkerSize(0.2);
      rpcstriplayervslayer->GetXaxis()->SetTitle(m_layer1_name.c_str() );
      rpcstriplayervslayer->GetYaxis()->SetTitle(m_layer2_name.c_str() );
    
     
      ATH_MSG_DEBUG (  "INSIDE bookRPCLayervsLayerHistograms : " << rpcstriplayervslayer << m_generic_path_rpcstriplayervslayer.c_str() );
    //  ATH_MSG_DEBUG (  "EXPERT : " << expert );
      ATH_MSG_DEBUG (  "RUN : " << run );
    
      sc = rpcprd_expert.regHist( rpcstriplayervslayer ); 
      if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register rpcstriplayervslayer hist to MonGroup" );
    
    }}//m_doRpcESD // AthenaMonManager::tier0 || AthenaMonManager::tier0ESD  
  
}

void RpcRawDataValAlg::bookRPCCoolHistograms( std::vector<std::string>::const_iterator & m_iter, int m_isec, int m_idblPhi,
					      std::string m_layer ) 
{
  StatusCode sc = StatusCode::SUCCESS ;
  
  std::string m_generic_path_rpcmonitoring = "Muon/MuonRawDataMonitoring/RPC";
  MonGroup rpcCoolDb( this, m_generic_path_rpcmonitoring+"/CoolDB", run, ATTRIB_UNMANAGED );
  
  sprintf(histName_char,"Sector%.2d_%s_dblPhi%d", m_isec+1, m_layer.c_str(), m_idblPhi+1) ;
  // example: Sector01_Pivot0_dblPhi1_StripId
  
  histName  = histName_char  ;
  histName += "_"            ;
  histName += *m_iter        ;  //histName += m_coolQuantity ;
  istatPhi  = int( m_isec/2) ;
  iName     = 0              ;
  int ig        = 0          ;
  int iNameMax  = 0          ;
  
  //BML7(dr=1) is associated to LowPt and not Pivot
  if ( m_isec<11 ||  m_isec>13) {
    // if ( m_layer.find("Pivot",0) )
    if ( m_layer == "Pivot0" || m_layer == "Pivot1" )   {
      iName = 2 + (m_isec%2 ) ;
      ir    = 2 	      ;		
      ig    = atoi( (m_layer.substr(5,1)).c_str() ) ;
    }
    if ( m_layer == "LowPt0" || m_layer == "LowPt1" )   {
      iName = 2 + (m_isec%2 ) ;
      ir    = 1 	      ;
      ig    = atoi( (m_layer.substr(5,1)).c_str() ) ;
    }
    if ( m_layer == "HighPt0" || m_layer == "HighPt1" ) {
      iName = 4 + (m_isec%2 ) ;
      ir    = 1 	      ;
      ig    = atoi( (m_layer.substr(6,1)).c_str() ) ; 
    }
    iNameMax  =  iName         ;
  }  
  else if ( m_isec==12) {
    // if ( m_layer.find("Pivot",0) )
    if ( m_layer == "Pivot0" || m_layer == "Pivot1" )   {
      iName     =  1      ;
      iNameMax  =  2      ; 
      ir    = 2 	      ;		
      ig    = atoi( (m_layer.substr(5,1)).c_str() ) ;
    }
    if ( m_layer == "LowPt0" || m_layer == "LowPt1" )   {
      iName     =  1      ;
      iNameMax  =  2      ; 
      ir    = 1 	      ;
      ig    = atoi( (m_layer.substr(5,1)).c_str() ) ;
    }
    if ( m_layer == "HighPt0" || m_layer == "HighPt1" ) {
      iName = 4               ;
      ir    = 1 	      ;
      ig    = atoi( (m_layer.substr(6,1)).c_str() ) ; 
      iNameMax  =  iName      ; 
    }
     
  }
  else {
    if ( m_layer == "Pivot0" || m_layer == "Pivot1" )   {
      iName     =   8  ;
      iNameMax  =  10  ;
      ir    = 2 ;   
      ig    = atoi( (m_layer.substr(5,1)).c_str() ) ;   
    }
    if ( m_layer == "LowPt0" || m_layer == "LowPt1" )   {
      iName = 8 ;
      iNameMax  =  iName         ;
      ir    = 1 ;
      ig    = atoi( (m_layer.substr(5,1)).c_str() ) ;
    }
    if ( m_layer == "HighPt0" || m_layer == "HighPt1" ) {
      iName = 9 ; // or 10 ;
      iNameMax=10          ;
      ir    = 1 ; // doubletR=2 -> upgrade of Atlas
      ig    = atoi( (m_layer.substr(6,1)).c_str() ) ; 
    }
  } // end sectors 12 and 14
  
  int NTotStripsSideA = 1;
  int NTotStripsSideC = 1;     
 
  int kName = iName ;
  if(kName==1)kName=53;//BMLE

  Identifier rpcId = m_rpcIdHelper->channelID(kName, 1 , istatPhi+1, ir, 1, m_idblPhi+1, 1, 1, 1); // last 3 arguments are: int gasGap, int measuresPhi, int strip
  if (!rpcId.is_valid()) {
    ATH_MSG_WARNING("Could not get valid Identifier for stationName="<<kName<<", eta=1, phi="<<istatPhi+1<<", doubletR="<<ir<<", doubletZ="<<1<<", doubletPhi="<<m_idblPhi+1);
  }
  Identifier rpcId_c = m_rpcIdHelper->channelID(kName, -1 , istatPhi+1, ir, 1, m_idblPhi+1, 1, 1, 1); // last 3 arguments are: int gasGap, int measuresPhi, int strip
  if (!rpcId_c.is_valid()) {
    ATH_MSG_WARNING("Could not get valid Identifier for stationName="<<kName<<", eta=-1, phi="<<istatPhi+1<<", doubletR="<<ir<<", doubletZ="<<1<<", doubletPhi="<<m_idblPhi+1);
  }
  const MuonGM::RpcReadoutElement* rpc   = m_muonMgr->getRpcReadoutElement(rpcId);   
  const MuonGM::RpcReadoutElement* rpc_c = m_muonMgr->getRpcReadoutElement(rpcId_c); 

  if(rpc){
    Identifier idr = rpc->identify();
    std::vector<int>   rpcstripshift = RpcGM::RpcStripShift(m_muonMgr,m_rpcIdHelper, idr, 0)  ;
    NTotStripsSideA = rpcstripshift[6]+rpcstripshift[17];
    Identifier idr_c = rpc_c->identify();
    std::vector<int>   rpcstripshift_c = RpcGM::RpcStripShift(m_muonMgr,m_rpcIdHelper, idr_c, 0)  ;
    NTotStripsSideC = rpcstripshift_c[7]+rpcstripshift_c[18];
   
  } 
   
  TH1 *rpcCoolHisto = new TH1F(histName.c_str(), histName.c_str(), NTotStripsSideC+NTotStripsSideA, -NTotStripsSideC, NTotStripsSideA );

  sc=rpcCoolDb.regHist(rpcCoolHisto) ;
  if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register " << histName << "hist to MonGroup" );
  rpcCoolHisto->GetXaxis()->SetTitle("strip");  
  
  
  
  // Fill strip Id histogram
  if ( (histName.find("PanelId", 0)) != string::npos ) {
  
    sc = rpcCoolDb.getHist( rpcCool_PanelIdHist, histName.c_str() );
    if( sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get "<< histName << " hist" );
    rpcCool_PanelIdHist->GetYaxis()->SetTitle("strip Id");
    rpcCool_PanelIdHist->SetBit(TH1::kIsAverage)         ;
    int rpcElemPhiStrip   ;
    int rpcElemEtaStrip   ;
    int coolStripIndex =0 ;
    
      
    for (int ieta=0; ieta!=17; ieta++) {
      //if((ieta-8)!=6)continue;//932
      for ( int iNameF=iName; iNameF!= iNameMax+1 ; iNameF++ ) {
        int kNameF = iNameF;
	if(kNameF==1)kNameF=53;//BMLE
    	for (int iz=0; iz!=3; iz++ ) {
	  int irc = ir ;	
	  if(abs(ieta-8)==7&&ir==1&&kNameF==2)continue;	
	  if(m_isec==12&&abs(ieta-8)==6&&ir==1&&kNameF==2)continue;
	  if(abs(ieta-8)==7&&ir==2&&kNameF==2)irc=1; 
	  if(m_isec==12&&abs(ieta-8)==6&&ir==2&&kNameF==2)irc=1;

        Identifier id = m_rpcIdHelper->channelID(kNameF, ieta-8, istatPhi+1, irc, iz+1, m_idblPhi+1, 1, 1, 1); // last 3 arguments are: int gasGap, int measuresPhi, int strip
        if (!id.is_valid()) {
          ATH_MSG_WARNING("Could not get valid Identifier for stationName="<<kNameF<<", eta="<<ieta-8<<", phi="<<istatPhi+1<<", doubletR="<<irc<<", doubletZ="<<iz+1<<", doubletPhi="<<m_idblPhi+1);
          continue;
        }
        const MuonGM::RpcReadoutElement* rpc = m_muonMgr->getRpcReadoutElement(id);  
        if(!rpc) continue;
	  
    	  if  ( iz+1 != rpc->getDoubletZ() ) { 
    	    continue ;
    	  }
    	  Identifier idr = m_rpcIdHelper->parentID( rpc->identify() );
    	  rpcElemPhiStrip = int (rpc->NphiStrips() ) ;
    	  rpcElemEtaStrip = int (rpc->NetaStrips() ) ;
	  
    	  for ( int istripEta=0; istripEta!=rpcElemEtaStrip; istripEta++ ) {
    	    Identifier strip_id  =  m_rpcIdHelper->channelID(idr, iz+1, m_idblPhi+1, ig+1, 0, istripEta+1) ;
    	    Identifier panel_id  =  m_rpcIdHelper->panelID( strip_id ) ;
    	    if( strip_id == 0 ) continue;
    	    coolStripIndex = (RpcGM::RpcStripShift(m_muonMgr,m_rpcIdHelper, strip_id, 0)).at(16);
    	    rpcCool_PanelIdHist->Fill(coolStripIndex, panel_id.get_identifier32().get_compact()) ;
        }
    	  for ( int istripPhi=0; istripPhi!=rpcElemPhiStrip; istripPhi++ ) {
    	    Identifier strip_id  =  m_rpcIdHelper->channelID(idr, iz+1, m_idblPhi+1, ig+1, 1, istripPhi+1) ;				     
    	    Identifier panel_id  =  m_rpcIdHelper->panelID( strip_id ) ;
    	    if( strip_id == 0 ) continue;
          coolStripIndex = (RpcGM::RpcStripShift(m_muonMgr,m_rpcIdHelper, strip_id, 0)).at(16);
    	    rpcCool_PanelIdHist->Fill(coolStripIndex, panel_id.get_identifier32().get_compact() );
        }
        } // end loop on doubletZ
      }
    }  // end loop on stationeta
  
  } // end fill cool histograms with panelId 
  
  
}



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode RpcRawDataValAlg::procHistograms()
{
  StatusCode sc = StatusCode::SUCCESS;
   
  ATH_MSG_DEBUG (  "********Reached Last Event in RpcRawDataValAlg !!!" );	  
  ATH_MSG_DEBUG (  "RpcRawDataValAlg finalize()" ); 	  
   
  if( m_doRpcESD==true ) {if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online ) {    
    
      if(endOfRunFlag()){        
	if ( m_doTrigEvol && (rpc_eventstotal > minStatTrEvol) ) {    
 
	  rpc2DEtaStatBinX_BA 	= int ( rpc2DEtaStationTriggerHits_Side_Pt[enumBA_LowPt]->GetNbinsX() );
	  rpc2DEtaStatBinX_BC 	= int ( rpc2DEtaStationTriggerHits_Side_Pt[enumBC_LowPt]->GetNbinsX() ); 
	  rpc2DEtaStatBinY    	= int ( rpc2DEtaStationTriggerHits_Side_Pt[enumBA_LowPt]->GetNbinsY() );
	  etaStatShiftX_BA    	= -1;
	  etaStatShiftX_BC    	= -8;
	  nEtaStatFired_BA_LowPt  =  0; 
	  nEtaStatFired_BA_HighPt =  0;
	  nEtaStatFired_BC_LowPt  =  0;
	  nEtaStatFired_BC_HighPt =  0;
       
       
	  for (int iy=1; iy!= rpc2DEtaStatBinY+1; iy++ ) {
	    for (int ix=1; ix!=rpc2DEtaStatBinX_BA+1; ix++ ) {
	      nTrigEtaStat_BA_LowPt  = int ( rpc2DEtaStationTriggerHits_Side_Pt[enumBA_LowPt] ->GetBinContent(ix, iy ) );
	      nTrigEtaStat_BA_HighPt = int ( rpc2DEtaStationTriggerHits_Side_Pt[enumBA_HighPt]->GetBinContent(ix, iy ) );
	      if ( nTrigEtaStat_BA_LowPt >0 ) nEtaStatFired_BA_LowPt++  ;
	      if ( nTrigEtaStat_BA_HighPt>0 ) nEtaStatFired_BA_HighPt++ ;
	    }
	    for (int ix=1; ix!=rpc2DEtaStatBinX_BC+1; ix++ ) {
	      nTrigEtaStat_BC_LowPt  = int ( rpc2DEtaStationTriggerHits_Side_Pt[enumBC_LowPt] ->GetBinContent(ix, iy ) );
	      nTrigEtaStat_BC_HighPt = int ( rpc2DEtaStationTriggerHits_Side_Pt[enumBC_HighPt]->GetBinContent(ix, iy ) );
	      if ( nTrigEtaStat_BC_LowPt >0 ) nEtaStatFired_BC_LowPt++ ;
	      if ( nTrigEtaStat_BC_HighPt>0 ) nEtaStatFired_BC_HighPt++ ;
	    }
	  }
	  rpcNumberEtaStatFired_Side_Pt[enumBA_LowPt]  -> Fill( nEtaStatFired_BA_LowPt  ) ; 
	  rpcNumberEtaStatFired_Side_Pt[enumBC_LowPt]  -> Fill( nEtaStatFired_BC_LowPt  ) ;
	  rpcNumberEtaStatFired_Side_Pt[enumBA_HighPt] -> Fill( nEtaStatFired_BA_HighPt ) ;
	  rpcNumberEtaStatFired_Side_Pt[enumBC_HighPt] -> Fill( nEtaStatFired_BC_HighPt ) ;
     	
	}  
      
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	/**Writes hits per harware chamber in a txt file*/
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *   
	std::ofstream myfile;
	if(m_rpcfile){
	  myfile.open ("rpccosmic.txt",ios::out); 
	  myfile << "-------- Counts per Chamber Statistics--------\n";} //only if m_rpcfile==true

    
	if(m_rpcfile){
	  myfile << "----Total events / Events in selected area----\n";
	  // myfile << rpc_eventstotal << "     /     " << rpc_event_inarea << "\n";
	  myfile.close();}  //only if m_rpcfile==true
      
	  

	//To review with Angelo Guida    
	std::vector<int>::const_iterator iter_bin=layer_name_bin_list_panel.begin() ;
	for (std::vector<std::string>::const_iterator iter=layer_name_list_panel.begin(); iter!=layer_name_list_panel.end(); iter++) 
	  {
	    int pos = *iter_bin;
	    iter_bin++ ;
	    if(pos>1000)continue; //skip fake trigger panels
      
	    std::string name = *iter          ;      
	    std::string list_name  = *iter    ;
	    std::string panel_name = *iter    ;
	    list_name.insert(7, "/Panels/")	  ; 
	    panel_name.insert(7, "_")         ;
	    sector_num  = name.substr(5, 2)   ;
	    sector_name = "Sector"+sector_num ;
                  
	  }
    
          //Normalize for integrated luminosity
	  
  
      } // end if isEndOfRun
  
    }}//m_doRpcESD // AthenaMonManager::tier0 || AthenaMonManager::tier0ESD  
  
  return sc;
}
 

 
//======================================================================================//
/**  finalize */
//======================================================================================//
StatusCode RpcRawDataValAlg::finalize() 
{ 

  StatusCode sc = ManagedMonitorToolBase::finalize();
  if(!sc.isSuccess()) return sc;


  ATH_MSG_DEBUG ( "RpcRawDataValAlg::finalize() " );
  
  // Clear Muon Monitoring Histograms 
  rpc2DEtaStationTriggerHits_Side_Pt.clear();
  rpcNumberEtaStatFired_Side_Pt.clear();
  
  return sc;
}
