/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Package : RPCStandaloneTracksMon
// Authors:  N. Benekos(Illinois) - M. Bianco(INFN-Lecce) - G. Chiodini(INFN-Lecce) - A. Guida (INFN-Lecce) 
// April 2009
//
// DESCRIPTION:
// Subject: RPC-->Offline Muon Data Quality
// 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
           
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
 
#include "MuonReadoutGeometry/RpcReadoutSet.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h"  
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "RPCcablingInterface/IRPCcablingServerSvc.h"

#include "MuonRDO/RpcFiredChannel.h"
#include "MuonRDO/RpcCoinMatrix.h"
#include "MuonRDO/RpcPad.h"
#include "MuonRDO/RpcPadContainer.h"
#include "MuonRDO/RpcSectorLogicContainer.h"
 
#include "TrigT1Result/MuCTPI_RDO.h"
#include "TrigT1Result/MuCTPI_DataWord_Decoder.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "MuonDigitContainer/RpcDigitContainer.h"
 
#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include "MuonTrigCoinData/RpcCoinDataContainer.h"

#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonDQAUtils/MuonCosmicSetup.h"
#include "MuonDQAUtils/MuonDQAHistMap.h" 


#include "MuonIdHelpers/MuonStationIndex.h"
#include "xAODEventInfo/EventInfo.h"

#include "TrkMultiComponentStateOnSurface/MultiComponentStateOnSurface.h"
#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"
#include "TrkEventPrimitives/ResidualPull.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkTrack/TrackCollection.h"
 
 
#include "RpcRawDataMonitoring/RPCStandaloneTracksMon.h"
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
static const   int nstripfiducial     =     2;
static const   int nstripfiduceff     =    80;
static const   int MergePointDistance =   100;
static const   int EtaStationSpan     =     2;
static const   int DoublePhiSpan      =     1;
static const   int maxCSres           =     8;
static const float Chi2dofCut         =     1;

/////////////////////////////////////////////////////////////////////////////

RPCStandaloneTracksMon::RPCStandaloneTracksMon( const std::string & type, const std::string & name, const IInterface* parent )
  :ManagedMonitorToolBase( type, name, parent ),
   m_first(true), 
   m_rpcRoiSvc( "LVL1RPC::RPCRecRoiSvc", name ),
   m_tgcRoiSvc( "LVL1TGC::TGCRecRoiSvc", name ),
   m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool")
{
  // Declare the properties 
  declareProperty("DoRpcEsd",            m_doRpcESD		= false	); 
  declareProperty("CheckCabling",        m_checkCabling		= false	);
  declareProperty("RpcFile",             m_rpcfile		= false	);    
  declareProperty("RpcChamberHist",      m_rpcchamberhist	= false	);  
  declareProperty("RPCStandaloneTracks", m_do_rpctracks		= true  );
  declareProperty("RpcReduceNbins",      m_rpcreducenbins	= 8	);	   
  declareProperty("RpcReduceNbinsStrip", m_rpcreducenbinsstrip	= 8	);	   
  declareProperty("ResThEff",		 m_resThEff		= 61	);
  declareProperty("ChamberName",         m_chamberName		= "XXX"	);
  declareProperty("StationSize",         m_StationSize		= "XXX"	);
  declareProperty("StationEta",          m_StationEta		= -100	);
  declareProperty("StationPhi",          m_StationPhi		= -100	);
  declareProperty("LastEvent",           m_lastEvent		= 0	);
  declareProperty("Sector",              m_sector		= 0	); 
  declareProperty("CosmicStation",       m_cosmicStation	= 0	);
  declareProperty("Side",                m_side			= 0	); 
  declareProperty("Clusters",            m_doClusters		= true	);			
  declareProperty("ClusterContainer",    m_clusterContainerName = "rpcClusters"		);
  declareProperty("RpcPrepDataContainer",m_key_rpc		= "RPC_Measurements"	);
  declareProperty("RPCTriggerContainer", m_key_trig		= "RPC_triggerHits"	);
  declareProperty("MinimunEntries",      MinEntries		= 10	);    // min entries required for summary plot 
  declareProperty("rpc_readout_window",  m_rpc_readout_window	= 0.2   ); // rpc readout window in 10^(-6) s for noise evaluation 
  declareProperty("doRadiography",       m_doRadiography        = true  ); 
  declareProperty("doCoolDB",	         m_doCoolDB		= false	);
  declareProperty("selectLB",	         m_selectLB		= false	);
  declareProperty("minSelLB",		 m_minSelLB             = 1	);
  declareProperty("maxSelLB",            m_maxSelLB             = 9999	);
  declareProperty("trackOnlySmall",	 m_trOnlySmall 		= false	);
  declareProperty("trackOnlyLarge",	 m_trOnlyLarge 		= false	);
  declareProperty("HPtPointForHPteff",	 m_HPtPointForHPteff 	= true	);
  declareProperty("HPtPointForLPteff",	 m_HPtPointForLPteff 	= true	);
  declareProperty("HPtPointForTracks",	 m_HPtPointForTracks 	= false	);

  declareProperty("UseTriggerVector",	  m_useTrigger);
  declareProperty("MuonTriggerChainName", m_MuonTriggerChainName);
  declareProperty("Muon_Trigger_Items",   m_muon_triggers);
  declareProperty("TriggerDecisionTool",  m_trigDecTool);
  
  declareProperty("MuonCollection",		     m_muonsName	    = "Muons");
  declareProperty("MuonSegmentCollection",	     m_muonSegmentsName     = "MuonSegments");
  declareProperty("MuonTrackCollection",	     m_muonTracksName	    = "MuonSpectrometerTrackParticles");
  declareProperty("MuonExtrapolatedTrackCollection", m_muonExtrapTracksName = "ExtrapolatedMuonTrackParticles");
  declareProperty("InDetTrackParticles",	     m_innerTracksName      = "InDetTrackParticles");
  declareProperty("MSVertexCollection",              m_msVertexCollection   = "MSDisplacedVertex");
  
  
  
  declareProperty("MuonDeltaRMatching"          , m_MuonDeltaRMatching         =   0.15 ); 
  declareProperty("requireMuonCombinedTight"    , m_requireMuonCombinedTight   = false  );
  declareProperty("StandAloneMatchedWithTrack"  , m_StandAloneMatchedWithTrack = true   );
  
  declareProperty( "selectTriggerChainGroup"    , m_selectTriggerChainGroup    = false     );
  declareProperty( "deSelectTriggerChainGroup"  , m_deselectTriggerChainGroup  = false     );
  declareProperty( "triggerChainGroupRegNot"    , m_triggerChainGroupRegNot    ="HLT_j.*" ); 
  declareProperty( "triggerChainGroupRegExp"    , m_triggerChainGroupRegExp    ="HLT_mu.*"  );//".*" all triggers//"HLT_mu.*" all EF muon triggers//"HLT_.*" all EF triggers//"L2_.*"  all L2 triggers//"L1_.*"  all L1 triggers 

  
  m_padsId     = 0;
  m_chambersId = 0;

  m_chainGroupSelect=NULL;
  m_chainGroupVeto=NULL;


} 
                            
RPCStandaloneTracksMon::~RPCStandaloneTracksMon()
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
  ATH_MSG_INFO ( " deleting RPCStandaloneTracksMon " );
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode RPCStandaloneTracksMon::initialize(){

      
  ATH_MSG_INFO ( "in initializing RPCStandaloneTracksMon" );

  ATH_MSG_DEBUG ( "******************" );
  ATH_MSG_DEBUG ( "doRpcESD: " << m_doRpcESD );
  ATH_MSG_DEBUG ( "RpcFile: "<< m_rpcfile );
  
  StatusCode sc;

  // Store Gate store
  sc = serviceLocator()->service("StoreGateSvc", m_eventStore);
  if (sc != StatusCode::SUCCESS ) {
    ATH_MSG_ERROR ( " Cannot get StoreGateSvc " );
    return sc ;
  }
  // retrieve the active store
  sc = serviceLocator()->service("ActiveStoreSvc", m_activeStore);
  if (sc != StatusCode::SUCCESS ) {
    ATH_MSG_ERROR ( " Cannot get ActiveStoreSvc " );
    return sc ;
  }

  // Initialize the IdHelper
  StoreGateSvc* detStore = 0;
  sc = service("DetectorStore", detStore);
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "DetectorStore service not found !" );
    return StatusCode::FAILURE;
  }   
  
  // Retrieve the MuonDetectorManager  
  sc = detStore->retrieve(m_muonMgr);
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "Cannot get MuonDetectorManager from detector store" );
    return StatusCode::FAILURE;
  }  
  else {
    ATH_MSG_DEBUG ( " Found the MuonDetectorManager from detector store. " );
  }

  sc = detStore->retrieve(m_rpcIdHelper,"RPCIDHELPER");
  if (sc.isFailure())
    {
      ATH_MSG_ERROR ( "Can't retrieve RpcIdHelper" );
      return sc;
    }	 
    
  // get RPC cablingSvc
  const IRPCcablingServerSvc* RpcCabGet = 0;
  sc = service("RPCcablingServerSvc", RpcCabGet);
  if (sc.isFailure()) {
    ATH_MSG_WARNING ( "Could not get RPCcablingServerSvc !" );
    return StatusCode::FAILURE;
  }
 
  sc = RpcCabGet->giveCabling(m_cabling);
  if (sc.isFailure()) {
    ATH_MSG_WARNING ( "Could not get RPCcablingSvc from the Server !" );
    m_cabling = 0;
    return StatusCode::FAILURE;
  } else { ATH_MSG_DEBUG ( " Found the RPCcablingSvc. " );    }

  hardware_name_list.push_back("XXX");
  
  ManagedMonitorToolBase::initialize().ignore();  //  Ignore the checking code;
      
  sc = RPC_ROI_Mapping();
  if (sc.isFailure()) {
    ATH_MSG_WARNING ( "Cannot retrieve PC_ROI_Mapping()" );	
    return StatusCode::FAILURE;
  } else { ATH_MSG_DEBUG ( "RPC_ROI_Mapping() done. " );    }
  
  
  
  if ( m_selectTriggerChainGroup || m_deselectTriggerChainGroup ) {
     sc = m_trigDec.retrieve();
     if ( sc.isFailure() ){
         ATH_MSG_WARNING ("Can't get handle on TrigDecisionTool");
     } else {
         ATH_MSG_DEBUG ("Got handle on TrigDecisionTool");
     }
  }     
 
  rpc_eventstotal=0;
  
  // Clear Muon Monitoring Histograms 
  m_SummaryHist.clear();
  m_SummaryHistRPCBA.clear();
  m_SummaryHistRPCBC.clear();
  m_sumTmpHist.clear();
  rpcAverageSide_A.clear(); 
  rpcAverageSide_C.clear();  
  
  hRPCPhiEtaCoinThr.clear()	 ;
  hRPCPadThr.clear()		 ;
  hRPCMuctpiThr.clear()  	 ;
  
  hRPCPhiEtaCoinThr_eff.clear()	 ;
  hRPCPadThr_eff.clear()		 ;
  hRPCMuctpiThr_eff.clear()  	 ;
  
  etaminpad.clear(); 
  etamaxpad.clear(); 
  phiminpad.clear(); 
  phimaxpad.clear();
  thresholdpad.clear();
   
  binx = 0 ;
  binminx = 0 ;
  binmaxx = 0 ;
  panelBin = 0 ;
  indexplane = 0 ;
  RPCLyHitOnTr = 0 ;
  RPCLyTrkPrj = 0 ;
  RPCLyEff = 0 ;
  RPCLyEff_err = 0 ;
  PanelVal = 0 ;
  PanelVal_err = 0 ;
  PanelVal_entries = 0 ;
  PanelVal_square = 0 ;
  PanelHitOnTrack = 0 ;
  PanelTrackProj = 0 ;
  PanelHitOnTrack_withCrossStrip = 0 ;
  invgasgaparea = 0 ;
  invstriparea = 0 ;
  distance = 0 ;
  isNoise = 0 ;
  isNoiseCorr = 0 ;
  isNoisePhiStrip = 0 ;
  norm1 = 0 ;
  norm2 = 0 ;
  norm3DGaprho = 0 ;
  Vect3DCosrho = 0 ;
  
  sectorLogicContainer = 0 ;
	 
  return StatusCode::SUCCESS;
}

/*----------------------------------------------------------------------------------*/
StatusCode RPCStandaloneTracksMon::fillHistograms()
/*----------------------------------------------------------------------------------*/
{
  StatusCode sc = StatusCode::SUCCESS; 
  
  ATH_MSG_DEBUG ( "RPCStandaloneTracksMon::RPCStandaloneTracksMon Histograms being filled" );
  if( m_doRpcESD==true ) { if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online ) {  
          
  // TRIGGER SELECTION BASED ON CHAIN GROUP
		 
  if (m_selectTriggerChainGroup || m_deselectTriggerChainGroup){
    std::string triggerChainGroupRegExp = m_triggerChainGroupRegExp;
    std::string triggerChainGroupRegNot = m_triggerChainGroupRegNot;
    m_chainGroupVeto   = m_trigDec->getChainGroup(triggerChainGroupRegNot.c_str());
    std::vector<std::string> vec_list = m_chainGroupVeto->getListOfTriggers();
    ATH_MSG_DEBUG(" List of Triggers to be Vetoed "<<vec_list);
    m_chainGroupSelect = m_trigDec->getChainGroup(triggerChainGroupRegExp.c_str());
    vec_list = m_chainGroupSelect->getListOfTriggers();
    ATH_MSG_DEBUG(" List of Triggers to be Selected "<<vec_list);
    if (m_chainGroupSelect) 
      {
	if (!m_chainGroupSelect->isPassed()) 
	  {
	    ATH_MSG_DEBUG(" not passed ....  skip ");
	    return sc;
	  }
	else 
	  {
	    ATH_MSG_DEBUG(" passed ....  keep event ");
	  }
      }
    if (m_chainGroupVeto) 
      {
	if (m_chainGroupVeto->isPassed()) 
	  {
	    ATH_MSG_DEBUG(" a trigger to be vetoed is passed ....  skip ");
	    return sc;
	  }
	else 
	  {
	    ATH_MSG_DEBUG(" passed ....  keep event ");
	  }
      }
  }
    
   //Muon tracks
     
//       // retrieve containers
//       const xAOD::MuonSegmentContainer*     MuonSegments = evtStore()->retrieve< const xAOD::MuonSegmentContainer >        (m_muonSegmentsName);       
//       const xAOD::TrackParticleContainer*   tracksMS     = evtStore()->retrieve< const xAOD::TrackParticleContainer >        (m_muonTracksName);      
         const xAOD::MuonContainer*	       Muons        = evtStore()->retrieve< const xAOD::MuonContainer >                      (m_muonsName);
	 if (!Muons)   {
	   ATH_MSG_WARNING ("Couldn't retrieve Muons container with key: " << m_muonsName);
	   return StatusCode::SUCCESS;
	 } 
	 ATH_MSG_DEBUG ("Muon container with key: " << m_muonsName<<" found");
//       const xAOD::VertexContainer*	       MSVertices   = evtStore()->retrieve< const xAOD::VertexContainer >           (m_msVertexCollection);
//       const xAOD::TrackParticleContainer*   METracks     = evtStore()->retrieve< const xAOD::TrackParticleContainer >( m_muonExtrapTracksName );
//       const xAOD::TrackParticleContainer*   IDTracks     = evtStore()->retrieve< const xAOD::TrackParticleContainer >     ( m_innerTracksName );
  
      
      
      const Muon::RpcPrepDataContainer* rpc_container;
      sc = (*m_activeStore)->retrieve(rpc_container, m_key_rpc);
      if (sc.isFailure()) {
	ATH_MSG_ERROR ( " Cannot retrieve RpcPrepDataContainer " << m_key_rpc );
	return sc;
      }
      ATH_MSG_DEBUG ( "RpcPrepDataContainer " << m_key_rpc <<" found");

      

      const DataHandle<xAOD::EventInfo> eventInfo;
      sc = m_eventStore->retrieve( eventInfo );
      if (sc.isFailure()) {
	ATH_MSG_DEBUG ( "no event info" );
	return StatusCode::SUCCESS;
      }
      else {
	ATH_MSG_DEBUG ( "yes event info" );
      }

            
      //int RunNumber = eventInfo->runNumber();
      //long int EventNumber = eventInfo->eventNumber();
      long int BCID   =  eventInfo->       bcid()  ;
      int lumiBlock   =  eventInfo->  lumiBlock()  ; 
      
      //select lumiblock for analysis
      if ( m_selectLB ) {	 
	  // skip events outside Lb range selected in jobOpt
	  if ( lumiBlock<m_minSelLB || lumiBlock>m_maxSelLB ) 
	    { 
	      ATH_MSG_INFO ( "event LB " << lumiBlock << " outside range " <<  m_minSelLB <<" "<<m_maxSelLB 
			     << "   -> skip event " ); 
	      return sc ;
	    }  	 
      }
    
    
      // check if data or MC
      int isMC=0;
      if (eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION ) ){
       isMC=1;
       ATH_MSG_INFO( "SIMULATION");
      }
      //   return StatusCode::SUCCESS; // stop this algorithms execute() for this event, here only interested in MC
        
      ATH_MSG_DEBUG ("****** rpc->size() : " << rpc_container->size());  
    
      Muon::RpcPrepDataContainer::const_iterator containerIt;
   
    
      type="RPC";


      // recall general histos  
      m_generic_path_rpcmonitoring = "Muon/MuonRawDataMonitoring/RPCStandAloneTrackMon"	 ;
      MonGroup rpcprd_shift         ( this, m_generic_path_rpcmonitoring +"/Overview"         , run, ATTRIB_UNMANAGED  );
      MonGroup rpcprd_expert        ( this, m_generic_path_rpcmonitoring +"/Overview"         , run, ATTRIB_UNMANAGED  );
      MonGroup rpcprd_expert_sum    ( this, m_generic_path_rpcmonitoring +"/Summary"          , run, ATTRIB_UNMANAGED  )	;
      MonGroup rpc_dqmf_global      ( this, m_generic_path_rpcmonitoring +"/GLOBAL"	      , run, ATTRIB_UNMANAGED  );
      MonGroup rpcprd_dq_BA         ( this, m_generic_path_rpcmonitoring +"/RPCBA"	      , run, ATTRIB_UNMANAGED  );
      MonGroup rpcprd_dq_BC         ( this, m_generic_path_rpcmonitoring +"/RPCBC"	      , run, ATTRIB_UNMANAGED  );
      MonGroup rpc_radiography      ( this, m_generic_path_rpcmonitoring +"/RPCRadiography"   , run, ATTRIB_UNMANAGED  );
      MonGroup rpcCoolDb            ( this, m_generic_path_rpcmonitoring +"/CoolDB"           , run, ATTRIB_UNMANAGED  );
      MonGroup rpc_triggerefficiency( this, m_generic_path_rpcmonitoring +"/TriggerEfficiency", run, ATTRIB_UNMANAGED  );
       
      // begin cluster monitoring
      const Muon::RpcPrepDataContainer* rpc_clusterContainer;
      if(m_eventStore->contains<Muon::RpcPrepDataContainer>(m_clusterContainerName)){
	sc = m_eventStore->retrieve(rpc_clusterContainer, m_clusterContainerName);
	if ( sc.isSuccess() ) {
	  Clus_Retr = true ; 
	}
	else {
	  ATH_MSG_DEBUG ( " RPCStandaloneTracksMon :: Cannot retrieve the RPC cluster container " );
	  Clus_Retr = false;
	  return sc;
	}
      }  
      else { Clus_Retr = false;}
        
      if (m_doClusters && Clus_Retr )
	{  
	  ATH_MSG_DEBUG ( "Start RPC Cluster Monitoring" );
    
	  // RPC clusters histograms
    
	  m_nClus=0;
    
	  N_Rpc_Clusters3D = 0     ;  
	  LayerType.clear()        ;
	  PlaneType.clear()        ;
	  SmallLarge.clear()       ;
	  Rpc_Phi_3D.clear()       ;
	  Rpc_Station_3D.clear()   ;
	  Rpc_Eta_3D.clear()       ;
	  Rpc_DBLr_3D.clear()      ;
	  Rpc_DBLphi_3D.clear()    ;
	  Rpc_DBLz_3D.clear()      ;
	  Rpc_GasGap_3D.clear()    ;
	  Rpc_SL_3D.clear()        ;
	  Rpc_Side_3D.clear()      ;
	  Rpc_x_3D.clear()         ;
	  Rpc_y_3D.clear()         ;
	  Rpc_z_3D.clear()         ;
	  Rpc_eta_3D.clear()       ;
	  Rpc_phi_3D.clear()       ;
	  Rpc_t_3D.clear()         ;
	  Rpc_avEta_3D.clear()     ;
	  Rpc_avPhi_3D.clear()	   ;
	  Rpc_id_eta_3D.clear()    ;
	  Rpc_id_phi_3D.clear()    ;
	  Rpc_Point.clear()        ;
	  Rpc_Matched_mu.clear()   ;
    
	  sc = rpcprd_shift.getHist(rpctrack_phivseta,"rpctrack_phivseta");		
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpctrack_phivseta hist to MonGroup" ); 
    
	  sc = rpcprd_shift.getHist(rpctrack_phi,"rpctrack_phi");		
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpctrack_phi hist to MonGroup" ); 
    
	  sc = rpcprd_shift.getHist(rpctrack_eta,"rpctrack_eta");		
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpctrack_eta hist to MonGroup" );
           
	  sc = rpc_dqmf_global.getHist(rpczxSurfaceView,"rpczxSurfaceView");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpczxSurfaceView hist to MonGroup" ); 
  
	  sc = rpcprd_shift.getHist(rpcxSurfaceView,"rpcxSurfaceView");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpcxSurfaceView hist to MonGroup" );  
     
	  sc = rpcprd_shift.getHist(rpczSurfaceView,"rpczSurfaceView");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpczSurfaceView hist to MonGroup" ); 
  
	  sc = rpcprd_shift.getHist(f_rpczxSurfaceView,"f_rpczxSurfaceView");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register f_rpczxSurfaceView hist to MonGroup" );
  
	  sc = rpcprd_shift.getHist(rpctrack_bvseta,"rpctrack_bvseta");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpctrack_bvseta hist to MonGroup" );   
  
	  sc = rpcprd_shift.getHist(rpctrack_bVTXy0,"rpctrack_bVTXy0");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpctrack_bVTXy0 hist to MonGroup" );  
  
	  sc = rpcprd_shift.getHist(rpctrack_bVTXx0,"rpctrack_bVTXx0");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpctrack_bVTXx0 hist to MonGroup" );  
  
	  sc = rpcprd_shift.getHist(rpctrack_bVTXz0,"rpctrack_bVTXz0");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpctrack_bVTXz0 hist to MonGroup" );  
  
	  sc = rpcprd_shift.getHist(rpctrack_b,"rpctrack_b");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpctrack_b hist to MonGroup" );
    
	  sc = rpcprd_shift.getHist(rpcmergepointdistance,"rpcmergepointdistance");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpcmergepointdistance hist to MonGroup" );
   
	  sc = rpcprd_shift.getHist(rpcchi2dof,"rpcchi2dof");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpcchi2dof hist to MonGroup" );
  
	  sc = rpcprd_shift.getHist(rpcetavsphichi2dof,"rpcetavsphichi2dof");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpcetavsphichi2dof hist to MonGroup" );     
     
	  sc = rpcprd_shift.getHist(f_rpcTrackType,"f_rpcTrackType");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register f_rpcTrackType hist to MonGroup" ); 
  
	  sc = rpcprd_shift.getHist(rpcTrackType,"rpcTrackType");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpcTrackType hist to MonGroup" );
  
	  sc = rpcprd_shift.getHist(f_rpcPhiResidual,"f_rpcPhiResidual");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register f_rpcPhiResidual hist to MonGroup" ); 
  
	  sc = rpcprd_shift.getHist(rpcPhiResidual,"rpcPhiResidual");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpcPhiResidual hist to MonGroup" );
  
  
	  sc = rpcprd_shift.getHist(f_rpcEtaResidual,"f_rpcEtaResidual");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register f_rpcEtaResidual hist to MonGroup" ); 
  
	  sc = rpcprd_shift.getHist(rpcEtaResidual,"rpcEtaResidual");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpcEtaResidual hist to MonGroup" );
    
    
	  sc = rpcprd_shift.getHist(f_rpcmergepointdistance,"f_rpcmergepointdistance");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register f_rpcmergepointdistance hist to MonGroup" );
    
	  sc = rpcprd_shift.getHist(f_rpcchi2dof,"f_rpcchi2dof");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register f_rpcchi2dof hist to MonGroup" );
    
	  sc = rpc_dqmf_global.getHist(rpcNtracks,"rpcNtracks");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpcNtracks hist to MonGroup" );
      
	  sc = rpcprd_shift.getHist(f_rpcNtracks,"f_rpcNtracks");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register f_rpcNtracks hist to MonGroup" );
     
	  sc = rpcprd_shift.getHist(rpcPointPerTracks,"rpcPointPerTracks");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpcPointPerTracks hist to MonGroup" );
    
	  sc = rpcprd_shift.getHist(f_rpcPointPerTracks,"f_rpcPointPerTracks");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register f_rpcPointPerTracks hist to MonGroup" );
    
	  sc = rpc_dqmf_global.getHist(LayerTrackProjection,"Track_Projected_on_Layer");		
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register LayerTrackProjection hist to MonGroup" );   
    
	  sc = rpc_dqmf_global.getHist(LayerHitOnTrack,"Hit_on_Track_Layer");		
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register LayerHitOnTrack hist to MonGroup" );   
    
	  sc = rpcprd_dq_BA.getHist(RPCBA_layerTrackProj,"Layer_TrackProj_sideA");	      
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get Layer_TrackProj_sideA  hist" );  
             
	  sc = rpcprd_dq_BC.getHist(RPCBC_layerTrackProj,"Layer_TrackProj_sideC");	      
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get Layer_TrackProj_sideC  hist" );  
         
	  sc = rpcprd_dq_BA.getHist(RPCBA_layerHitOnTrack,"Layer_HitOnTrack_sideA");	   
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get Layer_HitOnTrack_SideA  hist" );  
     	  
	  sc = rpcprd_dq_BC.getHist(RPCBC_layerHitOnTrack,"Layer_HitOnTrack_sideC");	   
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get Layer_HitOnTrack_sideC  hist" );  

	  sc = rpc_dqmf_global.getHist(rpcTimeTrackRes,"TimeTrackResidual") ;
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register TimeTrackResidual hist to MonGroup" );    
    
	  sc = rpcprd_shift.getHist(rpcTimeTrackRMS,"TimeTrackRMS") ;
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register TimeTrackRMS hist to MonGroup" );
    
	  sc = rpcprd_shift.getHist(rpcCS_HitOnTrack, "CS_HitOnTrack_Distribution");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register CS_HitOnTrack_Distribution hist to MonGroup" );
    
	  sc = rpcprd_shift.getHist(rpcCS_NoiseCorr, "CS_NoiseCorr_Distribution");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register CS_NoiseCorr_Distribution hist to MonGroup" );
    
	  sc = rpcprd_shift.getHist(rpcCS_NoiseNotCorr, "CS_NoiseNotCorr_Distribution");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register CS_NoiseNotCorr_Distribution hist to MonGroup" );
    
	  sc = rpcprd_shift.getHist(m_rpcCS_EtavsPhi, "CS_EtavsPhi") ;
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get CS_EtavsPhi" );
	 
	  sc = rpc_triggerefficiency.getHist( hMEtracks  ,"hMEtracks" ) ;		
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << " hMEtracks " );
     
	  Muon::RpcPrepDataContainer::const_iterator it = rpc_clusterContainer->begin();
         
	  // Access by Collection

	  for ( ; it != rpc_clusterContainer->end() ; ++it ) {
     
	    const Muon::RpcPrepDataCollection* clusterCollection = *it;
        
	    if (clusterCollection->size()>0) {
 	  
	      ATH_MSG_DEBUG ( "New Cluster collection" );
          
	      for (Muon::RpcPrepDataCollection::const_iterator rpcCollection = clusterCollection->begin(); 
		   rpcCollection != clusterCollection->end(); ++rpcCollection) {
	      
		Identifier prd_id = (*rpcCollection)->identify();
	 
		ATH_MSG_DEBUG ( "Adding a new cluster " );
            
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
          
 		// get the average strip and cluster position
 	    		  
 		Amg::Vector3D stripPosC(0,0,0);
		av_strip = 0 ;
		for(int i=0; i!=irpc_clus_size ; i++){
		  Identifier id = ((*rpcCollection)->rdoList())[i] ;
		  const MuonGM::RpcReadoutElement* descriptor = m_muonMgr->getRpcReadoutElement(id);
		  stripPosC += descriptor->stripPos(id);
		  strip = int(m_rpcIdHelper->strip(id))            ;
		  av_strip += float(strip)                         ;
		}
		if( irpc_clus_size != 0 ){ 
		  av_strip  =  av_strip / irpc_clus_size ;
		  stripPosC = stripPosC / irpc_clus_size ;
		}
		avstripphi = av_strip + ShiftStrips;
		irpc_clus_posx   = stripPosC.x()   ;
		irpc_clus_posy   = stripPosC.y()   ; 
		irpc_clus_posz   = stripPosC.z()   ;
		irpc_clus_poseta = stripPosC.eta() ;
		irpc_clus_posphi = stripPosC.phi() ;
            
	    		  
		//get information from geomodel to book and fill rpc histos with the right max strip number
		std::vector<int> rpcstripshift   = RpcGM::RpcStripShift(m_muonMgr,m_rpcIdHelper,prd_id, 0)  ;
	    		  
		NphiStrips         =  rpcstripshift[0] ;
		ShiftPhiStrips     =  rpcstripshift[1] ;
		NetaStrips	 =  rpcstripshift[2] ;
		ShiftEtaStrips     =  rpcstripshift[3] ;
		ShiftStrips	 =  rpcstripshift[4] ;
		NetaStripsTot      =  rpcstripshift[5] ;
		NetaStripsTotSideA =  rpcstripshift[6] ;
		NetaStripsTotSideC =  rpcstripshift[7] ;
		ShiftEtaStripsTot  =  rpcstripshift[8] ;
		Nbin               =  rpcstripshift[9] ;
		EtaStripSign       =  rpcstripshift[10];
		SectorLogic 	 =  rpcstripshift[11]  ;
		Side		 =  rpcstripshift[12]  ;
		PanelIndex         =  rpcstripshift[13];
		Settore            =  rpcstripshift[14];
		strip_dbindex      =  rpcstripshift[16];
				    
		m_SummaryHist_Idx = (Settore-1)*m_SummaryHist_Size/16;
 
		//get name for titles and labels 
		std::vector<std::string>   rpclayersectorsidename = RpcGM::RpcLayerSectorSideName(m_rpcIdHelper,prd_id, 0)  ;
  	       
		layer_name               = rpclayersectorsidename[0] ;
		layertodraw1_name        = rpclayersectorsidename[1] ;
		layertodraw2_name        = rpclayersectorsidename[2] ;
		layervslayer_name        = rpclayersectorsidename[3] ;
		layer0_name	       = rpclayersectorsidename[4] ;
		layer1_name	       = rpclayersectorsidename[5] ;
		layer2_name	       = rpclayersectorsidename[6] ;
		layerPhivsEta_name       = rpclayersectorsidename[7] ;
		layerPhivsEtaSector_name = rpclayersectorsidename[8] ;
		sector_name              = rpclayersectorsidename[9] ;
		layeronly_name           = rpclayersectorsidename[10];
		layer_name_panel         = rpclayersectorsidename[11];
		sector_dphi_layer        = rpclayersectorsidename[12];
	    
		// summary occupancy
		if(PanelIndex<1000)m_SummaryHist[enumSumOccupancy_NotNorm+m_SummaryHist_Idx]-> Fill( PanelIndex );
		//
		if ( m_doCoolDB ) {
		  sc = rpcCoolDb.getHist( rpcCool_Occupancy_NotNorm, sector_dphi_layer+"_Occupancy_NotNorm" ) ;		
		  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_Occupancy_NotNorm hist" );
		  if(rpcCool_Occupancy_NotNorm)rpcCool_Occupancy_NotNorm->Fill( strip_dbindex );	
		}
            
		// summary CS_entries
		if(PanelIndex<1000)m_SummaryHist[enumSumCS_entries+m_SummaryHist_Idx]-> Fill( PanelIndex );
		//
		if ( m_doCoolDB ) {
		  sc = rpcCoolDb.getHist( rpcCool_CS_entries, sector_dphi_layer+"_CS_entries" ) ;		
		  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_CS_entries hist" );
		  if(rpcCool_CS_entries)rpcCool_CS_entries->Fill( strip_dbindex );	
		}

		// summary CS
		if(PanelIndex<1000)m_SummaryHist[enumSumCS_NotNorm+m_SummaryHist_Idx]-> Fill( PanelIndex, irpc_clus_size );
		//
		if ( m_doCoolDB ) {
		  sc = rpcCoolDb.getHist( rpcCool_CS_NotNorm, sector_dphi_layer+"_CS_NotNorm" ) ;		
		  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_CS_NotNorm hist" );
		  if(rpcCool_CS_NotNorm)rpcCool_CS_NotNorm->Fill( strip_dbindex, irpc_clus_size );	
		}
	      
		// summary CS_square
		if(PanelIndex<1000)m_SummaryHist[enumSumCS_square+m_SummaryHist_Idx]-> Fill( PanelIndex, irpc_clus_size*irpc_clus_size );
		//
		if ( m_doCoolDB ) {
		  sc = rpcCoolDb.getHist( rpcCool_CS_square, sector_dphi_layer+"_CS_square" ) ;		
		  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_CS_square hist" );
		  if(rpcCool_CS_square)rpcCool_CS_square->Fill( strip_dbindex, irpc_clus_size*irpc_clus_size  );	
		}
	      	      
		// summary Time
		if(PanelIndex<1000)m_SummaryHist[enumSumTime_NotNorm+m_SummaryHist_Idx]-> Fill( PanelIndex, irpc_clus_time );
	      
		sc = rpcprd_dq_BA.getHist( StripTimeDistributionRPCBA, "StripTimeDistribution_" + sector_name +"_SideA");
		if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get StripTimeDistribution RPCBA " );
	         	      
		sc = rpcprd_dq_BC.getHist( StripTimeDistributionRPCBC, "StripTimeDistribution_" + sector_name +"_SideC");
		if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get StripTimeDistribution RPCBC " );
	      
		if ( Side == 1 ) {
		  if (  StripTimeDistributionRPCBA ) StripTimeDistributionRPCBA-> Fill( irpc_clus_time );  
		}	     
		else{
		  if (  StripTimeDistributionRPCBC ) StripTimeDistributionRPCBC-> Fill( irpc_clus_time );
		}
	     
		//
		if ( m_doCoolDB ) {
		  sc = rpcCoolDb.getHist( rpcCool_Time_NotNorm, sector_dphi_layer+"_Time_NotNorm" ) ;		
		  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_Time_NotNorm hist" );
		  if(rpcCool_Time_NotNorm)rpcCool_Time_NotNorm->Fill( strip_dbindex, irpc_clus_time );	
		}
	      
		// summary Time_square
		if(PanelIndex<1000)m_SummaryHist[enumSumTime_square+m_SummaryHist_Idx]-> Fill( PanelIndex, irpc_clus_time*irpc_clus_time  );
		//
		if ( m_doCoolDB ) {
		  sc = rpcCoolDb.getHist( rpcCool_Time_square, sector_dphi_layer+"_Time_square" ) ;		
		  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_Time_square hist" );
		  if(rpcCool_Time_square)rpcCool_Time_square->Fill( strip_dbindex, irpc_clus_time*irpc_clus_time   );	
		}
          
		// summary CS1_entries
		if(PanelIndex<1000&&irpc_clus_size==1)m_SummaryHist[enumSumCS1_entries+m_SummaryHist_Idx]-> Fill( PanelIndex );  

		// summary CS2_entries
		if(PanelIndex<1000&&irpc_clus_size==2)m_SummaryHist[enumSumCS2_entries+m_SummaryHist_Idx]-> Fill( PanelIndex );		
	      	      
	       
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
		  
		    if ( m_rpcCS_EtavsPhi ) m_rpcCS_EtavsPhi->Fill( irpc_clus_size, irpc_clus_sizeII );
  
  
		    //evaluate average strip
		    avstripeta = 0        ;
		    avstripphi = av_strip ; 
		  
		    ShiftEtaStripsTot = RpcGM::RpcStripShift(m_muonMgr,m_rpcIdHelper,prd_idII, 0)[8]  ;  // angelo 07 oct 2009
		    EtaStripSign      = RpcGM::RpcStripShift(m_muonMgr,m_rpcIdHelper,prd_idII, 0)[10] ;  // angelo 07 oct 2009

		    // get the average strip and cluster position
	            
		    Amg::Vector3D stripPosCII(0,0,0);
		  

		    for(int i=0; i!=irpc_clus_sizeII ; i++){
		      Identifier id = ((*rpcCollectionII)->rdoList())[i]             ;
		      const MuonGM::RpcReadoutElement* descriptor = m_muonMgr->getRpcReadoutElement(id);
		      stripPosCII += descriptor->stripPos(id);
		      avstripeta += float(m_rpcIdHelper->strip(id)) ;
		    }
		    if( irpc_clus_sizeII != 0 ){ 
		      avstripeta=  avstripeta/ irpc_clus_sizeII ;
		      stripPosCII = stripPosCII / irpc_clus_sizeII ;
		    }
		    avstripeta += float(ShiftEtaStripsTot)       ;
		    avstripeta  = avstripeta*float(EtaStripSign) ;
		    irpc_clus_posxII   = stripPosCII.x()   ;
		    irpc_clus_posyII   = stripPosCII.y()   ;
		    irpc_clus_poszII   = stripPosCII.z()   ;
		    irpc_clus_posetaII = stripPosCII.eta() ;
		    irpc_clus_posphiII = stripPosCII.phi() ;
	   
  
		    //evaluate layer type
		    layertype = 0 ;
		    if(layeronly_name=="Pivot0"	){
		      layertype = 3 ;
		    }
		    if(layeronly_name=="Pivot1"	){
		      layertype = 4 ;
		    }
		    if(layeronly_name=="LowPt0"	){
		      layertype = 1 ;
		    }
		    if(layeronly_name=="LowPt1"	){
		      layertype = 2 ;
		    }
		    if(layeronly_name=="HighPt0"  ){
		      layertype = 5 ;
		    }
		    if(layeronly_name=="HighPt1"  ){
		      layertype = 6 ;
		    }
	      
		    ATH_MSG_DEBUG("Candidate 3D RPC cluster N_Rpc_Clusters3D = "<<N_Rpc_Clusters3D 
				  << " station name/phi/eta " << irpc_clus_station << " " << irpc_clus_phi << " " << irpc_clus_eta);

		    //build 3D Rpc points without trigger hits //select clusters with size 1 or 2 
		    //Check for muon combined match
	            bool foundmatch3DwithMuon = false;
		    int nm=0;
		    if (Muons){	
                      // CombinedMuons Tight
	             for (const xAOD::Muon* muons: *Muons)
	             {       
	                if ( muons->muonType()!=xAOD::Muon::Combined  || ( muons->quality()!=xAOD::Muon::Tight && m_requireMuonCombinedTight ))continue;
			++nm;
			ATH_MSG_DEBUG("muons passing quality cuts to test cluster-muon track matching is now "<<nm);
	                const xAOD::TrackParticle* metrack = muons->trackParticle( xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle );          
	                if( !metrack   ) continue;		        
			
			ATH_MSG_DEBUG("xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle found for this muon ");
		        if(  sqrt(fabs(irpc_clus_posetaII-metrack->eta())*fabs(irpc_clus_posetaII-metrack->eta()) +  fabs(irpc_clus_posphi-metrack->phi())*fabs(irpc_clus_posphi-metrack->phi())) <   m_MuonDeltaRMatching) foundmatch3DwithMuon = true ;
		    	      
		     }}//end muons
		    
		    if( layertype>0 && irpc_clus_size < 3 && irpc_clus_sizeII < 3 ){
		      if(irpc_clus_station==2||irpc_clus_station==4){
			// large
			if ( m_trOnlySmall ) continue ; 
			SmallLarge.push_back( 1 ) ;
		      }
		      else{
			if ( m_trOnlyLarge ) continue ;
			SmallLarge.push_back( 0 ) ;
		      }
		
		      layertype=layertype-1;
		      planetype=int(float(layertype)/2);
		  
		      LayerType.push_back     (layertype 	         )  ;
		      PlaneType.push_back     (planetype 	         )  ;
		      Rpc_Phi_3D.push_back    (irpc_clus_phi	 )  ;
		      Rpc_Station_3D.push_back(irpc_clus_station   )  ;
		      Rpc_Eta_3D.push_back    (irpc_clus_eta	 )  ;
		      Rpc_DBLr_3D.push_back   (irpc_clus_doublr    )  ;
		      Rpc_DBLphi_3D.push_back (irpc_clus_doublphi  )  ;
		      Rpc_DBLz_3D.push_back   (irpc_clus_doublz    )  ;
		      Rpc_GasGap_3D.push_back (irpc_clus_gasgap    )  ;
		      Rpc_x_3D.push_back      (irpc_clus_posx	 )  ;
		      Rpc_y_3D.push_back      (irpc_clus_posy	 )  ;
		      Rpc_z_3D.push_back      (irpc_clus_poszII          )  ;
		      Rpc_eta_3D.push_back    (irpc_clus_posetaII	 )  ;
		      Rpc_phi_3D.push_back    (irpc_clus_posphi          )  ;
		      Rpc_t_3D.push_back      (irpc_clus_time	 )  ;
		      Rpc_SL_3D.push_back     (SectorLogic	 )  ;
		      Rpc_Side_3D.push_back   (Side		 )  ;
		      Rpc_id_phi_3D.push_back (prd_id  	         )  ;
		      Rpc_id_eta_3D.push_back (prd_idII  	         )  ;
		      Rpc_avEta_3D.push_back  (int (avstripeta )         )  ;
		      Rpc_avPhi_3D.push_back  (int (avstripphi )         )  ;
		      Amg::Vector3D Vector3D  (irpc_clus_posx  , irpc_clus_posy  ,  irpc_clus_poszII);
		      Rpc_Point.push_back     (Vector3D 	         )  ;
		      Rpc_Matched_mu.push_back(foundmatch3DwithMuon 	 )  ;
		  
		    
		      /*
			std::cout << "Next 3D RPC cluster" << std::endl;
			std::cout << N_Rpc_Clusters3D << " " << irpc_clus_phi << " " << irpc_clus_station << " " << irpc_clus_eta << std::endl;
			std::cout << irpc_clus_doublr << " " << irpc_clus_doublphi << " " << irpc_clus_doublz << " " << irpc_clus_gasgap << std::endl;
			std::cout << irpc_clus_posx   << " " << irpc_clus_posy	<< " " << irpc_clus_poszII << " Ltype "<<layertype<<std::endl;
			std::cout << planetype << " "<< irpc_clus_time<< " "<<  SectorLogic+ 32*Side<<"-----" << std::endl;
		      */
		      N_Rpc_Clusters3D++;
	            

		    }
                                        
		  } // for loop on RpcPrepDataCollection
		} //second loop on clusters end	
      
	
 	      }//end clusters collection
 	    }//end if size
	  }//end clusters container
	     
    
    std::list<muctpi_rdo> muctpi_rdo_roi_list;
    muctpi_rdo_roi_list.clear();
    const MuCTPI_RDO* muctpiRDO;
    sc = (*m_activeStore)->retrieve(muctpiRDO,"MUCTPI_RDO");
    if (sc.isFailure()) {
      ATH_MSG_WARNING ( "Cannot retrieve the MuCTPI" );     
      return StatusCode::SUCCESS;
    }
    else {
        
        
        // Create some dummy LVL1 muon thresholds:
        std::vector< TrigConf::TriggerThreshold* > dummy_thresholds;
       

        // Loop over the MuCTPI data words, and "reconstruct" them:
        std::vector< uint32_t >::const_iterator dw_itr = muctpiRDO->dataWord().begin();
        std::vector< uint32_t >::const_iterator dw_end = muctpiRDO->dataWord().end();
        for( ; dw_itr != dw_end; ++dw_itr ) {
            
            muctpi_rdo  muctpi_rdo_roi;
            // Use the same class that is used by the LVL2 steering to decode
            // the muon RoIs:

            uint32_t dataWord = (*dw_itr);
            uint32_t RoIWord = ( ( dataWord & 0x18000000 ) |
        			 ( ( dataWord & 0x3fe0000 ) >> 3 ) |
        			 ( dataWord & 0x3fff ) );

            LVL1::RecMuonRoI roi( RoIWord,
        			  &( *m_rpcRoiSvc ), &( *m_tgcRoiSvc ),
        			  &dummy_thresholds );
            
            muctpi_rdo_roi.eta= roi.eta() ;
            muctpi_rdo_roi.phi= roi.phi() ;
            muctpi_rdo_roi.source= roi.sysID() ;
            muctpi_rdo_roi.hemisphere= roi.subsysID() ;
            int muctpi_bcid = ( ( dataWord >> 14 ) & 0x7 );
            // store difference from L1Acc
            int dbc = muctpi_bcid;
            if (!isMC) dbc = dbc - BCID%8;
            if (dbc<=-4) {
        	dbc=dbc+8;
            }else if (dbc>4) {
        	dbc=dbc-8;
            }
            muctpi_rdo_roi.bcid=dbc;	 
            muctpi_rdo_roi.sectorID= roi.sectorID() ;
            muctpi_rdo_roi.thrNumber= roi.getThresholdNumber() ;
            muctpi_rdo_roi.RoINumber= roi.getRoINumber() ;
            muctpi_rdo_roi.overlapFlags= roi.getOverlap() ;


            muctpi_rdo_roi_list.push_back(muctpi_rdo_roi);
            
        }

        ATH_MSG_DEBUG(" MUCTPI RoIs = " << muctpi_rdo_roi_list.size() );
    }
     
    // Retrieve the Sector Logic container
    sc = (*m_activeStore) -> retrieve(sectorLogicContainer);     
    
    if (sc.isFailure()) {
      if (isMC==1) ATH_MSG_DEBUG ( "Cannot retrieve the RpcSectorLogicContainer ... that's normal in MC: no container is produced in digitization" );
      else 
	{
	  ATH_MSG_WARNING ( "Cannot retrieve the RpcSectorLogicContainer ... however, there's no reason to stop here" );     
	  //return StatusCode::SUCCESS;
	}
    }
    else {
     ATH_MSG_DEBUG("RpcSectorLogicContainer found with size " << sectorLogicContainer->size());

      ///////////////////////////////////////////
      // Loop over the Sector Logic containers //
      ///////////////////////////////////////////

      etaminpad.clear(); 
      etamaxpad.clear(); 
      phiminpad.clear(); 
      phimaxpad.clear();
      thresholdpad.clear();
      RpcSectorLogicContainer::const_iterator its = sectorLogicContainer -> begin();
      
      for ( ; its != sectorLogicContainer -> end() ; ++its ) 
	{
	  	  int i_sectorid = (*its)->sectorId(); 
	  // Loop over the trigger hits of each sector
	  RpcSectorLogic::const_iterator ithit = (*its) -> begin(); 
	  for ( ; ithit != (*its) -> end() ; ++ithit ) 
	    { 
	      // from RpcSLTriggerHit
	      int b_isInput          = (*ithit) -> isInput();
	      //int i_rowinBcid      = (*ithit) -> rowinBcid();//readout window BCid
	      //int i_padid          = (*ithit) -> padId();//tower
	      int i_ptid             = (*ithit) -> ptId();//threshold
	      int i_roi              = (*ithit) -> roi();//region of interest
	      //int i_outerPlane     = (*ithit) -> outerPlane();
	      //int i_overlapPhi     = (*ithit) -> overlapPhi();
	      //int i_overlapEta     = (*ithit) -> overlapEta();
	      //int i_triggerBcid    = (*ithit) -> triggerBcid();
	      int side = 0;
	      int sl   = i_sectorid;
	      if(i_sectorid>31){ 
	       side = 1;
	       sl = i_sectorid - 32 ;
	      }
	      if(b_isInput==0){
	      
	       double a =  EtaROImin[side][sl][i_roi];
	       double b =  EtaROImax[side][sl][i_roi];
	       double c =  PhiROImin[side][sl][i_roi];
	       double d =  PhiROImax[side][sl][i_roi];
	       etaminpad.push_back(a);
	       etamaxpad.push_back(b);
	       phiminpad.push_back(c);
	       phimaxpad.push_back(d);
	       thresholdpad .push_back(i_ptid);
	      } 
	      
      }}}

      	
	
	  /////////////// Trigger hits efficiency
	  //Trigger hits    
          const Muon::RpcCoinDataContainer* rpc_coin_container;
          sc = (*m_activeStore)->retrieve(rpc_coin_container, "RPC_triggerHits" );
          if (sc.isFailure()) {
            ATH_MSG_WARNING ( "Cannot retrieve RPC trigger hits container");
            return sc;
          }
	  
 
	  
	  if (Muons){	
          // CombinedMuons Tight
	  for (const xAOD::Muon* muons: *Muons)
	  {
 
             
	    if ( muons->muonType()!=xAOD::Muon::Combined  || ( muons->quality()!=xAOD::Muon::Tight && m_requireMuonCombinedTight ))continue;
	     
	   
	    const xAOD::TrackParticle* metrack = muons->trackParticle( xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle );
            
	    if( !metrack   ) continue;
	    if( fabs(metrack->eta())>1) continue;
	    sc = rpc_triggerefficiency.getHist( hMEtracks  ,"hMEtracks" ) ;		  
	    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get " << " hMEtracks " );
	    if(hMEtracks)hMEtracks->Fill( metrack->pt() / 1000.);
	     //std::cout <<" Track " <<  metrack->eta() << " " <<metrack->phi() << std::endl;
	    
	    bool foundmatchlowpt_thr0  = false;
	    bool foundmatchlowpt_thr1  = false;
	    bool foundmatchlowpt_thr2  = false;
	    bool foundmatchhighpt_thr0 = false;
	    bool foundmatchhighpt_thr1 = false;
	    bool foundmatchhighpt_thr2 = false;
	    
	    bool foundmatchpad_thr0  = false;
	    bool foundmatchpad_thr1  = false;
	    bool foundmatchpad_thr2  = false;
	    bool foundmatchpad_thr3  = false;
	    bool foundmatchpad_thr4  = false;
	    bool foundmatchpad_thr5  = false;
	    
	    bool foundmatchmuctpi_thr0  = false;
	    bool foundmatchmuctpi_thr1  = false;
	    bool foundmatchmuctpi_thr2  = false;
	    bool foundmatchmuctpi_thr3  = false;
	    bool foundmatchmuctpi_thr4  = false;
	    bool foundmatchmuctpi_thr5  = false;
	     
	   
	   
	   
	   // coin	
	   
           Muon::RpcCoinDataContainer::const_iterator  it_container;
           Muon::RpcCoinDataCollection::const_iterator it_collection;
           Identifier prdcoll_id;
           Muon::RpcCoinDataContainer::const_iterator  it_container_phi;
           Muon::RpcCoinDataCollection::const_iterator it_collection_phi;
           Identifier prdcoll_id_phi;
           const MuonGM::RpcReadoutElement* descriptor_Atl;
           double  eta_atlas, phi_atlas;
	   for( it_container = rpc_coin_container->begin(); it_container != rpc_coin_container->end(); ++it_container ) {
            for ( Muon::RpcCoinDataCollection::const_iterator it_collection = (*it_container)->begin(); it_collection != (*it_container)->end(); ++it_collection ) { // each collection is a trigger signal
	     if( (*it_collection)->isLowPtCoin()  == 0  && (*it_collection)->isHighPtCoin() == 0 ) continue ; 
	     prdcoll_id   = (*it_collection)->identify();
             if(m_rpcIdHelper->measuresPhi(prdcoll_id))continue;
	     int cointhr = (*it_collection)->threshold();
             descriptor_Atl = m_muonMgr->getRpcReadoutElement( prdcoll_id );
             eta_atlas = descriptor_Atl->stripPos(prdcoll_id ).eta();
             phi_atlas = descriptor_Atl->stripPos(prdcoll_id ).phi();
	     
	     
	     //std::cout << " Trigger Hits " << eta_atlas << " "<< phi_atlas <<std::endl;
             if(m_rpcIdHelper->measuresPhi(prdcoll_id))continue;
	     if( sqrt( fabs(eta_atlas-metrack->eta())*fabs(eta_atlas-metrack->eta()) +  fabs(phi_atlas-metrack->phi())*fabs(phi_atlas-metrack->phi()) ) < m_MuonDeltaRMatching) { 
	      //Second coin phi view
	      for( it_container_phi = rpc_coin_container->begin(); it_container_phi != rpc_coin_container->end(); ++it_container_phi ) {
               for ( Muon::RpcCoinDataCollection::const_iterator it_collection_phi = (*it_container_phi)->begin(); it_collection_phi != (*it_container_phi)->end(); ++it_collection_phi ) { // each collection is a trigger signal
	         if( (*it_collection_phi)->isLowPtCoin()  == 0  && (*it_collection_phi)->isHighPtCoin() == 0 ) continue ; ;
	         prdcoll_id_phi   = (*it_collection_phi)->identify(); 
	         if(m_rpcIdHelper->measuresPhi(prdcoll_id_phi)==0)continue;
		 int cointhrphi = (*it_collection)->threshold();
		 if(m_rpcIdHelper->stationPhi (prdcoll_id) != m_rpcIdHelper->stationPhi (prdcoll_id_phi))  continue ;	   
		 if(m_rpcIdHelper->stationName(prdcoll_id) != m_rpcIdHelper->stationName(prdcoll_id_phi))  continue ;	   
		 if(m_rpcIdHelper->stationEta (prdcoll_id) != m_rpcIdHelper->stationEta (prdcoll_id_phi))  continue ;		   
		 if(m_rpcIdHelper->doubletR   (prdcoll_id) != m_rpcIdHelper->doubletR	(prdcoll_id_phi))  continue ;
		 if(m_rpcIdHelper->doubletZ   (prdcoll_id) != m_rpcIdHelper->doubletZ	(prdcoll_id_phi))  continue ;
		 if(m_rpcIdHelper->doubletPhi (prdcoll_id) != m_rpcIdHelper->doubletPhi (prdcoll_id_phi))  continue ;
		 if(m_rpcIdHelper->gasGap     (prdcoll_id) != m_rpcIdHelper->gasGap	(prdcoll_id_phi))  continue ;  
            
		 if( fabs((*it_collection)->time() -  (*it_collection_phi)->time()) > 50. ) continue ;  
		 if( (*it_collection)->isLowPtCoin() != (*it_collection_phi)->isLowPtCoin()  || (*it_collection)->isHighPtCoin() != (*it_collection_phi)->isHighPtCoin()) continue ; 
	     
	          
                 descriptor_Atl = m_muonMgr->getRpcReadoutElement( prdcoll_id_phi );
                 eta_atlas = descriptor_Atl->stripPos(prdcoll_id_phi ).eta();
                 phi_atlas = descriptor_Atl->stripPos(prdcoll_id_phi ).phi(); 
		 //std::cout <<" Trigger Hits ETA PHI " << eta_atlas<<" "  << phi_atlas << " "<< (*it_collection)-> isLowPtCoin()<< " "  << (*it_collection)->isHighPtCoin() <<" "<<cointhr <<" "<< cointhr<<" "<<cointhrphi<<std::endl;	
		 if( sqrt( fabs(eta_atlas-metrack->eta())*fabs(eta_atlas-metrack->eta()) +  fabs(phi_atlas-metrack->phi())*fabs(phi_atlas-metrack->phi()) ) < m_MuonDeltaRMatching) {		    
		    
		    int minthrview = cointhr ; if(cointhrphi<minthrview)minthrview = cointhrphi; 
		    if( (*it_collection)-> isLowPtCoin() &&  (*it_collection_phi)-> isLowPtCoin()){
		     if(minthrview>=0&&!foundmatchlowpt_thr0){
		      hRPCPhiEtaCoinThr[0]->Fill( metrack->pt() / 1000.);
		      foundmatchlowpt_thr0=true;
		     } 
		     if(minthrview>=1&&!foundmatchlowpt_thr1){
		      hRPCPhiEtaCoinThr[1]->Fill( metrack->pt() / 1000.);
		      foundmatchlowpt_thr1=true;
		     } 
		     if(minthrview>=2&&!foundmatchlowpt_thr2){
		      hRPCPhiEtaCoinThr[2]->Fill( metrack->pt() / 1000.);
		      foundmatchlowpt_thr2=true;
		     }
		    } 
		    if( (*it_collection)->isHighPtCoin() &&  (*it_collection_phi)-> isHighPtCoin()){
		     if(minthrview>=0&&!foundmatchhighpt_thr0){
		      hRPCPhiEtaCoinThr[3]->Fill( metrack->pt() / 1000.);
		      foundmatchhighpt_thr0=true;
		     } 
		     if(minthrview>=1&&!foundmatchhighpt_thr1){
		      hRPCPhiEtaCoinThr[4]->Fill( metrack->pt() / 1000.);
		      foundmatchhighpt_thr1=true;
		     } 
		     if(minthrview>=2&&!foundmatchhighpt_thr2){
		      hRPCPhiEtaCoinThr[5]->Fill( metrack->pt() / 1000.);
		      foundmatchhighpt_thr2=true;
		     }	
		      
		    } 
	           
		 }  
		  
	     }}//End second coin phi view 
	     }   
	   }}//end coin loop
	   
	   //Pad	 
	   for (unsigned int i_etaphiPAD=0 ;i_etaphiPAD!=etaminpad.size(); i_etaphiPAD++) { // each collection is a trigger signal
	            
		    int sign = 1 ;
		    if(metrack->eta()<0)sign=-1;
	   
		    //std::cout <<thresholdpad.at(i_etaphiPAD) << " PAD " << etaminpad.at(i_etaphiPAD) << " "<< etamaxpad.at(i_etaphiPAD) <<" phi " << phiminpad.at(i_etaphiPAD) << " "<< phimaxpad.at(i_etaphiPAD) <<std::endl;	
		    if( ( metrack->eta()-etaminpad.at(i_etaphiPAD))*sign> -m_MuonDeltaRMatching ){
		    if( (-metrack->eta()+etamaxpad.at(i_etaphiPAD))*sign> -m_MuonDeltaRMatching ){
		    if(   metrack->phi()-phiminpad.at(i_etaphiPAD)      > -m_MuonDeltaRMatching ){
		    if(  -metrack->phi()+phiminpad.at(i_etaphiPAD)      > -m_MuonDeltaRMatching ){
		      
		      int thr = thresholdpad.at(i_etaphiPAD) ;
		      if(thr>=0&&!foundmatchpad_thr0){
		       hRPCPadThr[0]->Fill( metrack->pt() / 1000.);
		       foundmatchpad_thr0=true;
		      }
		      if(thr>=1&&!foundmatchpad_thr1){
		       hRPCPadThr[1]->Fill( metrack->pt() / 1000.);
		       foundmatchpad_thr1=true;
		      }
		      if(thr>=2&&!foundmatchpad_thr2){
		       hRPCPadThr[2]->Fill( metrack->pt() / 1000.);
		       foundmatchpad_thr2=true;
		      }
		      if(thr>=3&&!foundmatchpad_thr3){
		       hRPCPadThr[3]->Fill( metrack->pt() / 1000.);
		       foundmatchpad_thr3=true;
		      }
		      if(thr>=4&&!foundmatchpad_thr4){
		       hRPCPadThr[4]->Fill( metrack->pt() / 1000.);
		       foundmatchpad_thr4=true;
		      }
		      if(thr>=5&&!foundmatchpad_thr5){
		       hRPCPadThr[5]->Fill( metrack->pt() / 1000.);
		       foundmatchpad_thr5=true;
		      }
		    }}}}  
	   
	   }//end pad loop 
	   
	   //muctpi
	   for (auto i_muctpi_rdo_roi_list=muctpi_rdo_roi_list.begin();i_muctpi_rdo_roi_list!=muctpi_rdo_roi_list.end();i_muctpi_rdo_roi_list++) { // each collection is a trigger signal
	            
		    //std::cout << i_muctpi_rdo_roi_list->thrNumber << " Muctpi " << i_muctpi_rdo_roi_list->eta  << " "<< i_muctpi_rdo_roi_list->phi <<std::endl;	

		    double deta =   metrack->eta()-i_muctpi_rdo_roi_list->eta ;
	            double dphi =   metrack->phi()-i_muctpi_rdo_roi_list->phi ;
		    
	            double dr   = sqrt(deta*deta+dphi*dphi);
		    int thr     = i_muctpi_rdo_roi_list->thrNumber;
		    if(dr<m_MuonDeltaRMatching ){
		      if(thr>=0&&!foundmatchmuctpi_thr0){
		       hRPCMuctpiThr[0]->Fill( metrack->pt() / 1000.);
		       foundmatchmuctpi_thr0=true;
		      }
		      if(thr>=1&&!foundmatchmuctpi_thr1){
		       hRPCMuctpiThr[1]->Fill( metrack->pt() / 1000.);
		       foundmatchmuctpi_thr1=true;
		      }
		      if(thr>=2&&!foundmatchmuctpi_thr2){
		       hRPCMuctpiThr[2]->Fill( metrack->pt() / 1000.);
		       foundmatchmuctpi_thr2=true;
		      }
		      if(thr>=3&&!foundmatchmuctpi_thr3){
		       hRPCMuctpiThr[3]->Fill( metrack->pt() / 1000.);
		       foundmatchmuctpi_thr3=true;
		      }
		      if(thr>=4&&!foundmatchmuctpi_thr4){
		       hRPCMuctpiThr[4]->Fill( metrack->pt() / 1000.);
		       foundmatchmuctpi_thr4=true;
		      }
		      if(thr>=5&&!foundmatchmuctpi_thr5){
		       hRPCMuctpiThr[5]->Fill( metrack->pt() / 1000.);
		       foundmatchmuctpi_thr5=true;
		      }		     
		    
		    } 
	   
	   } //end muctpi loop
	   
	  }//end muon loop
	  }//end if	
	  ATH_MSG_DEBUG("-----------------------Search for 3D clusters is OVER-----------------------N_Rpc_Clusters3D "<<N_Rpc_Clusters3D<<" found");
	  for (int i_3D=0; i_3D!=N_Rpc_Clusters3D; i_3D++) {
	    ATH_MSG_DEBUG("----listing them-----"<<i_3D<<" layertype/planetype "<<LayerType.at(i_3D)<<"/"<<PlaneType.at(i_3D)
			  <<" statName/eta/phi "<<Rpc_Station_3D.at(i_3D)<<"/"<<Rpc_Eta_3D.at(i_3D)<<"/"<<Rpc_Phi_3D.at(i_3D)
			  <<" dbR/Z/P/gg "<<Rpc_DBLr_3D.at(i_3D)<<"/"<<Rpc_DBLz_3D.at(i_3D)<<"/"<<Rpc_DBLphi_3D.at(i_3D)
			  <<"/"<<Rpc_GasGap_3D.at(i_3D)<<" eta/phi strip = "<<Rpc_avEta_3D.at(i_3D)<<"/"<<Rpc_avPhi_3D.at(i_3D));
	  }
			 	  
     
	  /////////////// RPC standalone tracking     
		  
	  N_RpcTrack = 0 ;
	  RpcTrack_Vect3DCos.clear()  ;
	  RpcTrack_Poin3RLin.clear()  ;
	  RpcTrack_chi2dofeta.clear() ;
	  RpcTrack_chi2dofphi.clear() ;
	  RpcTrack_chi2dof.clear()    ;
		  
	  if( m_do_rpctracks && N_Rpc_Clusters3D<1000 )  {
		    
	    Rpc_track = new int[N_Rpc_Clusters3D]; 
			      
	    for (int ilayertype=0; ilayertype!=7; ilayertype++) { //ilayertype==6 do tracks with all 6 layers
	      ATH_MSG_DEBUG("---***---***---***--- Track Search iteration ilayertype="<<ilayertype<<" out of [0-6]");


	      for (int i_3D=0; i_3D!=N_Rpc_Clusters3D; i_3D++) { Rpc_track[ i_3D ] = 0; }
                                        
	      Amg::Vector3D  SegVector(0,0,0)	;
	      Amg::Vector3D   SegPoint(0,0,0)	;
	      Amg::Vector3D  SegVectorII(0,0,0)	;
	      Amg::Vector3D  ImpactVector(0,0,0)  	;
	      Amg::Vector3D  bImpactIP(0,0,0)     	;
      
	      //std::cout << "multi tracks begin " << std::endl ;  
	      ///multi tracks begin
	      //Pattern recognition: link 3D cluster with at least three different layer type two of each Pivot and LowPt 
	      N_Rpc_Tracks   = 0 ;
	      /*
		for (int i_3D=0;i_3D!=N_Rpc_Clusters3D;i_3D++) {
		std::cout << i_3D <<" " <<LayerType.at(i_3D)<<" "<< (Rpc_Point.at(i_3D)).x() << " " << (Rpc_Point.at(i_3D)).y() << " " <<(Rpc_Point.at(i_3D)).z() <<std::endl ;
		}	              
	      */
 	      for (int i_3D0=0; i_3D0!=N_Rpc_Clusters3D; i_3D0++) {
	        if( !(Rpc_Matched_mu.at(i_3D0)) && m_StandAloneMatchedWithTrack )continue;
 		if(LayerType.at(i_3D0)==ilayertype &&  ilayertype!=6  )continue;
 		if(Rpc_track[ i_3D0 ]>0||PlaneType.at(i_3D0)!=0)continue;//Start always with no-track assigned LowPt plane
 		Phi_Rpc_Track = Rpc_Phi_3D[ i_3D0 ]; 
 		PointperTrack = 1  ;
 		Rpc_track[ i_3D0 ] = N_Rpc_Tracks + 1 ;//preliminary assigned
		/*std::cout << "First Cluster 3D " << i_3D0 <<" on LayerType[0-5] =" 
		  <<LayerType.at(i_3D0)<<" "<< (Rpc_Point.at(i_3D0)).x() << " " << (Rpc_Point.at(i_3D0)).y() << " " <<(Rpc_Point.at(i_3D0)).z() 
		  <<" station/eta/phi/dbR/dbZ/dbP/gg = "<< Rpc_Station_3D.at(i_3D0)<<"/"<<Rpc_Eta_3D.at(i_3D0)<<"/"<
		  <Rpc_Phi_3D.at(i_3D0)<<"/"<<Rpc_DBLr_3D.at(i_3D0)<<"/"<<Rpc_DBLz_3D.at(i_3D0)<<"/"<<Rpc_DBLphi_3D.at(i_3D0)
		  <<"/"<<Rpc_GasGap_3D.at(i_3D0)<<std::endl ;*/
 		linkedtrack   = 0 ;
		for (int i_3DI=0;i_3DI!=N_Rpc_Clusters3D;i_3DI++) {
	          if( !(Rpc_Matched_mu.at(i_3DI))  && m_StandAloneMatchedWithTrack )continue;
		  if(linkedtrack == 1 ) continue ;
		  if(  abs(Rpc_Eta_3D.at(i_3DI)-Rpc_Eta_3D.at(i_3D0)) > EtaStationSpan )continue;
		  if(  abs(Rpc_Phi_3D.at(i_3DI)-Rpc_Phi_3D.at(i_3D0)) > DoublePhiSpan  )continue;
		  if(LayerType.at(i_3DI)==ilayertype &&  ilayertype!=6  )continue;
		  if(Rpc_track[ i_3DI ]>0||PlaneType.at(i_3DI)!=1)continue;//Second always no-track assigned Pivot plane
		  Rpc_track[ i_3DI ] = N_Rpc_Tracks + 1 ;
		  PointperTrack = 2 ;
		  /*  std::cout << "Second Cluster 3D " << i_3DI <<" on LayerType[0-5] =" <<LayerType.at(i_3DI)<<" "
		      << (Rpc_Point.at(i_3DI)).x() << " " << (Rpc_Point.at(i_3DI)).y() << " " <<(Rpc_Point.at(i_3DI)).z() 
		      <<" station/eta/phi/dbR/dbZ/dbP/gg = "<< Rpc_Station_3D.at(i_3DI)<<"/"<<Rpc_Eta_3D.at(i_3DI)<<"/"
		      <<Rpc_Phi_3D.at(i_3DI)<<"/"<<Rpc_DBLr_3D.at(i_3DI)<<"/"<<Rpc_DBLz_3D.at(i_3DI)<<"/"<<Rpc_DBLphi_3D.at(i_3DI)<<"/"
		      <<Rpc_GasGap_3D.at(i_3DI)<<std::endl ;*/
		  SegVector = Rpc_Point.at(i_3DI)-Rpc_Point.at(i_3D0) ;	  
		  SegPoint  =			Rpc_Point.at(i_3D0) ;
		  /*std::cout << "for First-Second cluster pair: start     point="
		    <<SegPoint.x() << " " <<SegPoint.y()<< " "<< SegPoint.z()
		    <<" r/phi/z "<<SegPoint.perp()<<"/"<<SegPoint.phi()<<SegPoint.z()<<std::endl ;
		    std::cout << "for First-Second cluster pair: 1-2Vector point="
		    <<SegVector.x() << " " <<SegVector.y()<< " "<< SegVector.z()
		    <<" r/phi/z "<<SegVector.perp()<<"/"<<SegVector.phi()<<SegVector.z()<<std::endl ;*/
		  lookforthirdII   = 0 ;
		  thirdlayertypeII = 0 ; 
		  thirdlayerHPt    = 0 ;
		
		  for (int i_3DII=0;i_3DII!=N_Rpc_Clusters3D;i_3DII++) {
	            if( !(Rpc_Matched_mu.at(i_3DII)) && m_StandAloneMatchedWithTrack )continue;
		    if(  abs(Rpc_Eta_3D.at(i_3DII)-Rpc_Eta_3D.at(i_3DI)) > EtaStationSpan )continue;
		    if(  abs(Rpc_Phi_3D.at(i_3DII)-Rpc_Phi_3D.at(i_3DI)) > DoublePhiSpan  )continue;
		    if(LayerType.at(i_3DII)==ilayertype &&  ilayertype!=6  )continue;
		    if(Rpc_track[ i_3DII ]>0)continue;//Third no-track assigned LowPt or Pivot or HighPt plane
		  
		    /*
		    std::cout << "Third Cluster 3D " << i_3DII <<" on LayerType[0-5] =" <<LayerType.at(i_3DII)<<" "<< (Rpc_Point.at(i_3DII)).x() << " " << (Rpc_Point.at(i_3DII)).y() << " " <<(Rpc_Point.at(i_3DII)).z() <<" station/eta/phi/dbR/dbZ/dbP/gg = "<< Rpc_Station_3D.at(i_3DII)<<"/"<<Rpc_Eta_3D.at(i_3DII)<<"/"<<Rpc_Phi_3D.at(i_3DII)<<"/"<<Rpc_DBLr_3D.at(i_3DII)<<"/"<<Rpc_DBLz_3D.at(i_3DII)<<"/"<<Rpc_DBLphi_3D.at(i_3DII)<<"/"<<Rpc_GasGap_3D.at(i_3DII)<<std::endl ;*/

		    ImpactVector = (SegPoint-Rpc_Point.at(i_3DII)).cross(SegVector);	    
		    if(SegVector.mag()!=0)ImpactVector = ImpactVector/ SegVector.mag();	   
		    
		    /*
		      std::cout << "ImpactVector "<<ImpactVector.x() << " " <<ImpactVector.y()<< " "<< ImpactVector.z()<<std::endl ;
		      std::cout << "SegVector "<<SegVector.x() << " " <<SegVector.y()<< " "<< SegVector.z()<<std::endl ;
		      std::cout << "SegPoint "<<SegPoint.x() << " " <<SegPoint.y()<< " "<< SegPoint.z()<<std::endl ;
		      std::cout << "Distance " << ImpactVector.mag() <<" to be compared with threshold = "<<MergePointDistance<<std::endl ;
		    */
		    
		    if(ilayertype<6){
		     f_rpcmergepointdistance -> Fill (ImpactVector.mag()) ;
		    } 
		    else {
		     rpcmergepointdistance -> Fill (ImpactVector.mag()) ;
		    }      
		    
		    if(ImpactVector.mag()<MergePointDistance){
		      /*
		      std::cout << "Third (or following) cluster matches the segment built with 1st-2nd 3D cluster pair... 1st,2nd,3rd indices = "
				<<i_3D0<<"/"
				<<i_3DI<<"/"
				<<i_3DII
				<<" for track index "<<N_Rpc_Tracks + 1<<" at ilayertype iter="<<ilayertype<<std::endl ;
		      */
		      Rpc_track[ i_3DII ] = N_Rpc_Tracks + 1 ;
		      PointperTrack++ ;
		      lookforthirdII = 1 ;
		      if (LayerType.at(i_3DII)!=LayerType.at(i_3DI) && LayerType.at(i_3DII)!=LayerType.at(i_3D0) ){ 
			thirdlayertypeII = 1 ;
			//std::cout << "third cluster found - is not on the same layer as 1st and 2nd  " <<std::endl ;
		      }
		      if (LayerType.at(i_3DII)> 3 ){ 
			thirdlayerHPt = 1 ;
			//std::cout << "third layer found with HPt" <<std::endl ;
		      }
		     	    
		    }
		    else ATH_MSG_VERBOSE("Third (or following) cluster DO NOT match the segment built with 1st-2nd 3D cluster");

		          
		    
		    //merge in costheta not used
		    SegVectorII = Rpc_Point.at(i_3DII)-Rpc_Point.at(i_3D0) ;
		    cosmerge = SegVector.dot(SegVectorII);
		    if(SegVector.mag()*SegVectorII.mag()!=0)cosmerge=cosmerge/(SegVector.mag()*SegVectorII.mag());
						
			
			
		  }//Third
		  //First and Second do not link with different layer type with any Third let free Second

		  /*
		  std::cout<<"3rd LOOP over clusters is OVER: any good triplet ?? lookforthirdII/thirdlayertypeII/ilayertype/thirdlayerHPt/ = "
			   <<lookforthirdII<<"/"<<thirdlayertypeII<<"/"<<ilayertype<<"/"<<thirdlayerHPt<<"/"<<m_HPtPointForHPteff<<"/"
			   <<m_HPtPointForLPteff<<"/"<<m_HPtPointForTracks<<std::endl;
		  */
			   

		  if( (lookforthirdII==0||thirdlayertypeII==0)                  ||
		      (ilayertype==4&&thirdlayerHPt==0&&m_HPtPointForHPteff==1) ||
		      (ilayertype==5&&thirdlayerHPt==0&&m_HPtPointForHPteff==1) ||
		      (ilayertype <5&&thirdlayerHPt==0&&m_HPtPointForLPteff==1) || 
		      (ilayertype==6&&thirdlayerHPt==0&&m_HPtPointForTracks==1)    ){  
		    Rpc_track[ i_3DI  ] = 0 ;
		    for (int i_3Dx=0;i_3Dx!=N_Rpc_Clusters3D;i_3Dx++) { if((i_3Dx!=i_3D0)&&(Rpc_track[ i_3Dx ]==N_Rpc_Tracks + 1 )) Rpc_track[ i_3Dx ] = 0; }
		    //std::cout << "Clear Second and all merged thirds but firts" <<std::endl ;
		  }
		  else{ 
		    linkedtrack = 1 ; 
		    //std::cout << "Linked " <<std::endl ;
		  } 
  	
		}//Second
		//First, Second and Third do not link with different layer type let free First
		if(linkedtrack == 0 ){
		  Rpc_track[ i_3D0  ] = 0 ;
		  PointperTrack	  = 0 ;
		  //std::cout << "Clear First " <<std::endl ;
		}
		else{
 		  if( ilayertype==6  ) { rpcPointPerTracks -> Fill ( PointperTrack ) ; }
		  else {  f_rpcPointPerTracks -> Fill ( PointperTrack ) ; }
		
		  //rosy decrease to 1000 instead of 10000 since RPC_track is a fixed to 1000 size array
		  if( N_Rpc_Tracks<1000 ) N_Rpc_Tracks ++ ;
		  //std::cout << "Add track " <<N_Rpc_Tracks<< " with N points " <<  PointperTrack <<std::endl ;
 
		  x0Phi = 0 ;
		  xyPhi = 0 ;
		  z0Eta = 0 ;
		  zyEta = 0 ;
 
		  xav=0; x2av=0 ; yav=0 ; y2av=0 ; zav=0 ; z2av=0 ; xyav=0 ; xzav=0 ; zyav=0 ;
		  deltax=0; deltaz=0; deltay=0;
		  projx=0; projz=0; projy=0;
 
		  res=0; residual2=0;
 
		  NplaneSmall0  = 0   ;  NplaneSmall1  = 0   ;   NplaneSmall2  = 0   ; 
		  NplaneLarge0  = 0   ;  NplaneLarge1  = 0   ;   NplaneLarge2  = 0   ;
 
		  mineta = 100 ; maxeta=-100 ;
		  minphi = 100 ; maxphi=-100 ;
 
  
		  for (int i_3D=0;i_3D!=N_Rpc_Clusters3D;i_3D++) {

		    if(Rpc_track[ i_3D ] != N_Rpc_Tracks)continue;
		    //gap to span to find intersections
		    if(minphi>Rpc_Phi_3D.at(i_3D))minphi=Rpc_Phi_3D.at(i_3D);
		    if(maxphi<Rpc_Phi_3D.at(i_3D))maxphi=Rpc_Phi_3D.at(i_3D);
		    if(mineta>Rpc_Eta_3D.at(i_3D))mineta=Rpc_Eta_3D.at(i_3D);
		    if(maxeta<Rpc_Eta_3D.at(i_3D))maxeta=Rpc_Eta_3D.at(i_3D);
 
		    if(SmallLarge.at( i_3D)==0){
		      if(PlaneType.at( i_3D)==0)NplaneSmall0=1;
          if(PlaneType.at( i_3D)==1)NplaneSmall1=1;
          if(PlaneType.at( i_3D)==2)NplaneSmall2=1;
		    }
		    else if(SmallLarge.at( i_3D)==1){
		      if(PlaneType.at( i_3D)==0)NplaneLarge0=1;
          if(PlaneType.at( i_3D)==1)NplaneLarge1=1;
          if(PlaneType.at( i_3D)==2)NplaneLarge2=1;
		    }  
			 
		    xav += Rpc_x_3D.at( i_3D) ; yav += Rpc_y_3D.at( i_3D) ; zav += Rpc_z_3D.at( i_3D) ;
		    x2av += Rpc_x_3D.at( i_3D)*Rpc_x_3D.at( i_3D) ;
		    y2av += Rpc_y_3D.at( i_3D)*Rpc_y_3D.at( i_3D) ;
		    z2av += Rpc_z_3D.at( i_3D)*Rpc_z_3D.at( i_3D) ;
		    xyav += Rpc_x_3D.at( i_3D)*Rpc_y_3D.at( i_3D) ;
		    xzav += Rpc_x_3D.at( i_3D)*Rpc_z_3D.at( i_3D) ;
		    zyav += Rpc_y_3D.at( i_3D)*Rpc_z_3D.at( i_3D) ;
		  }
			
		  TrackType = NplaneSmall0 + NplaneSmall1*2 + NplaneSmall2*4 + NplaneLarge0*8 + NplaneLarge1*16 + NplaneLarge2*32;
 
		  // linear regression with all points in (xy), (yx), (yz)
		  deltax = PointperTrack * x2av - xav*xav ;
		  deltaz = PointperTrack * z2av - zav*zav ;
		  deltay = PointperTrack * y2av - yav*yav ;
 
		  if(deltay!=0){
		    x0Phi = (xav*y2av-yav*xyav)/deltay;
		    xyPhi = (xyav*PointperTrack-xav*yav)/deltay;
		    z0Eta = (zav*y2av-yav*zyav)/deltay;
		    zyEta = (zyav*PointperTrack-zav*yav)/deltay;
		  }
		  else{
		    //std::cout << "deltay = 0 Not accepted with cosmics!!!"  << std::endl ;
		    x0Phi = 0 ;
		    xyPhi = 0 ;
		    z0Eta = 0 ;
		    zyEta = 0 ;
		  }
			
		  chi2dof = 0 ; chi2dofeta = 0 ; chi2dofphi=0;
		  tav = 0; t2av = 0; tres = 0; trms = 0;
		                            
 
		  //time spread, residual and chi2 in (y,z), (y,x) view
		  if(deltay!=0){
		    for (int i_3D=0;i_3D!=N_Rpc_Clusters3D;i_3D++) {
		      if(Rpc_track[ i_3D ] != N_Rpc_Tracks) continue;
		      tav  += Rpc_t_3D.at(i_3D)/PointperTrack;
		      t2av += Rpc_t_3D.at(i_3D)*Rpc_t_3D.at(i_3D)/PointperTrack;
		      projz   =  z0Eta + zyEta * Rpc_y_3D.at(i_3D)  ;
		      res	  = (projz -Rpc_z_3D.at(i_3D))  	;
		                            
		      if ( rpcEtaResidual!=0  &&  ilayertype==6  ) {rpcEtaResidual->Fill( res );}
		      else {ATH_MSG_DEBUG ( "rpcEtaResidual not in hist list!" );}
			  
		      residual2 = res*res				;
		      chi2dofeta  += residual2/(PointperTrack-2)/81.0   ;
 
		      projx   =  x0Phi + xyPhi * Rpc_y_3D.at(i_3D);
		      res	  = (projx -Rpc_x_3D.at(i_3D))        ;
		      cosyx = cos(( (2*(Rpc_Phi_3D.at(i_3D)-1) + SmallLarge.at(i_3D) -5) )*3.14159265/8) ;
		      res = res * cosyx ;
		                            
		      if ( rpcPhiResidual!=0  &&  ilayertype==6  ) {rpcPhiResidual->Fill( res );}
		      else {ATH_MSG_DEBUG ( "rpcPhiResidual not in hist list!" );}
		  		  
		      residual2 = res*res				;
		      chi2dofphi  += residual2/(PointperTrack-2)/81.0   ;
		    }
 
                    //std::cout << "Fitting z0Eta " << z0Eta << " zyEta " << zyEta << " chi2/dof " << chi2dofeta << std::endl ;
                    //std::cout << "Fitting x0Phi " << x0Phi << " xyPhi " << xyPhi << " chi2/dof " << chi2dofphi << std::endl ;
		  
		    chi2dof = (chi2dofeta + chi2dofphi)/2. ;
		    if ( ilayertype==6  )  {
		      rpcchi2dof         -> Fill (chi2dof) ;
		      rpcetavsphichi2dof -> Fill (chi2dofphi,chi2dofeta) ;
		      //std::cout << "t2av - tav*tav " << t2av << " " << tav*tav <<  std::endl ;
		      trms = sqrt(fabs(t2av - tav*tav));
		      rpcTimeTrackRMS -> Fill (trms) ;
		      for (int i_3D=0;i_3D!=N_Rpc_Clusters3D;i_3D++) {
			if(Rpc_track[ i_3D ] != N_Rpc_Tracks) continue;
			tres =  Rpc_t_3D.at(i_3D) - tav;
			rpcTimeTrackRes -> Fill (tres) ;
		      }
		  
		      rpcTrackType->Fill(TrackType);
 
		    }
		    else { 
		      f_rpcchi2dof -> Fill (chi2dof) ; 	  
		      f_rpcTrackType->Fill(TrackType);
		 
		    }
		  }
 		    
 		  if( chi2dof<Chi2dofCut ) {
		  
		    Amg::Vector3D Vect3DCos( xyPhi, 1 , zyEta );
		    Amg::Vector3D  Poin3RLin( x0Phi, 0 , z0Eta );
		  
		  
		    bImpactIP = Poin3RLin.cross(Vect3DCos);	    
		    if(Vect3DCos.mag()!=0)bImpactIP = bImpactIP/ Vect3DCos.mag();
		    Impact_B_IP = bImpactIP.mag();
		        
		    xsurface = Poin3RLin.x()+Vect3DCos.x()*81000.;
		    zsurface = Poin3RLin.z()+Vect3DCos.z()*81000.;
		    xPy0     = Poin3RLin.x() ;
		    zPy0     = Poin3RLin.z() ;
		    if(Vect3DCos.z()!=0) {
		      yPz0     =  - Poin3RLin.z()/Vect3DCos.z()     ;
		      xPz0     =    Poin3RLin.x()+Vect3DCos.x()*yPz0;
		    }
		    if(Vect3DCos.x()!=0) {
		      yPx0     =  - Poin3RLin.x()/Vect3DCos.x()     ;
		      zPx0     =  Poin3RLin.z() +Vect3DCos.z()*yPx0 ;
		    }
		  		  

		    anglephi = 90-atan(abs(xyPhi))*180/3.141592653   ; //atan between -pi/2 and pi/2 , anglephi from 180 to 0	
		    if(xyPhi<0) anglephi = 180.-anglephi;	   
	    	      
		    rhoxy  = sqrt( xyPhi*xyPhi + 1 );
		    rho    = sqrt( xyPhi*xyPhi + 1 + zyEta*zyEta);
		    costh  = zyEta/rho ;
		    sinth  = sqrt(1-costh*costh);
		    pseta  = sinth / (1+costh)  ;
		     
		    if(pseta>0) pseta = -log(pseta) ;
		      	 	      
		    		  
		    if( ilayertype==6  ){
		      rpczxSurfaceView       ->Fill(zsurface,xsurface);
		      rpcxSurfaceView        ->Fill(         xsurface);
		      rpczSurfaceView        ->Fill(zsurface         );
		      rpctrack_phivseta      ->Fill(pseta, anglephi  );
		      rpctrack_bvseta        ->Fill(pseta,Impact_B_IP); 
		      rpctrack_phi           ->Fill(anglephi         );
		      rpctrack_eta           ->Fill(pseta            );
		      rpctrack_b             ->Fill(Impact_B_IP      );
		      rpctrack_bVTXy0        ->Fill(zPy0,xPy0        );
		      rpctrack_bVTXx0        ->Fill(zPx0,yPx0        );
		      rpctrack_bVTXz0        ->Fill(xPz0,yPz0        );
		    		  
		    		  
		      N_RpcTrack++;
		      RpcTrack_Vect3DCos .push_back( Vect3DCos);		
		      RpcTrack_Poin3RLin .push_back( Poin3RLin);		
		      RpcTrack_chi2dofeta.push_back(chi2dofeta);		
		      RpcTrack_chi2dofphi.push_back(chi2dofeta);		
		      RpcTrack_chi2dof   .push_back(   chi2dof);
		    }
		    else{
		      f_rpczxSurfaceView->Fill(zsurface,xsurface);
		    }

		    //NPhiStrip = 0 ;
		    //NEtaStrip = 0 ;
		    phipitch  = 0 ;
		    etapitch  = 0 ;
	 		   
		    //std::cout << "minphi maxphi mineta maxeta " << minphi << " " << maxphi << " " << mineta << " " << maxeta <<std::endl;
		  
		    //start loop on gaps by geomodel
		    if(ilayertype!=6){
		      for(int iname=      2; iname!=       10+1 ; iname++){
			for(int ieta = mineta; ieta != maxeta+1; ieta++ ){ 
			  //if(ieta>6)continue;if(ieta<-6)continue;//remove bml7
			  for(int iphi = minphi; iphi != maxphi+1; iphi++ ){
			    for(int ir   =      1; ir   !=      2+1; ir++	){
			      for(int iz   =      1; iz   !=      3+1; iz++	){
				for(int idp = 1; idp != 2 + 1; idp++ ){

          bool isValid=false;
          Identifier rpcId = m_rpcIdHelper->channelID(iname, ieta, iphi, ir, iz, idp, 1, 1, 1, true, &isValid, true); // last 6 arguments are: int doubletPhi, int gasGap, int measuresPhi, int strip, bool check, bool* isValid, bool noPrint
          if (!isValid) {
            ATH_MSG_DEBUG("Could not find valid Identifier for station="<<iname<<", eta="<<ieta<<", phi="<<iphi<<", doubletR="<<ir<<", doubletZ="<<iz<<", doubletPhi="<<idp<<", continuing...");
            continue;
          }
          const MuonGM::RpcReadoutElement* rpc = m_muonMgr->getRpcReadoutElement(rpcId);
          if(!rpc)continue;
	
	
				  if(iname==2||iname==4){
				    small_large = 1 ;
				  }
				  else {
				    small_large = 0 ;
				  }
 		
				  Identifier idr = rpc->identify();
		
    
 				  for(int ig = 1; ig != 2 + 1; ig++ ){
    
				    Layer = 4 ;
				    if( iname < 4 || iname == 8 ) Layer = 0 ;
				    Layer += ig - 1 + (ir - 1)*2 ;
				    if(Layer!=ilayertype)continue;
    
				    Amg::Vector3D  Vector3DGapG (0.,0.,0.); 
				    Amg::Vector3D norm3DGap   (0.,0.,0.);  
				    Amg::Vector3D  Inters3DG   (0.,0.,0.);   
    
				    Vector3DGapG = rpc->gasGapPos( iz, idp, ig);
				    norm3DGap   = rpc->normal   (/*     idp, ig*/);
				    Inters3DG   = Poin3RLin + Vect3DCos*(norm3DGap.dot(Vector3DGapG-Poin3RLin))/(norm3DGap.dot(Vect3DCos));
  		        
				    
				  
				    Amg::Vector3D  Inters3DL (0.,0.,0.) ;
         
				    phistripN = rpc->NphiStrips()   ;
				    etastripN = rpc->NetaStrips()   ;
				    phipitch  = rpc->StripPitch(1)  ;
				    etapitch  = rpc->StripPitch(0)  ;
    
				    if(phistripN>80||etastripN>40) continue ;
				    if(phistripN< 1||etastripN< 1) continue ;
				    if(phipitch <10||etapitch <10) continue ;
				    if(phipitch >50||etapitch >50) continue ;
    
				    Identifier ideta1  =  m_rpcIdHelper->channelID(idr, iz, idp, ig, 0, 1)	 ;
				    Identifier idetaN  =  m_rpcIdHelper->channelID(idr, iz, idp, ig, 0, etastripN) ;
				    Identifier idphi1  =  m_rpcIdHelper->channelID(idr, iz, idp, ig, 1, 1)	 ;
				    Identifier idphiN  =  m_rpcIdHelper->channelID(idr, iz, idp, ig, 1, phistripN) ;
        
              
				    if( !(rpc->containsId(ideta1)&&rpc->containsId(idetaN)&&rpc->containsId(idphi1)&&rpc->containsId(idphiN) )){
				      /*
					if( !(rpc->containsId(ideta1))){
					ATH_MSG_DEBUG ( "Strip ideta1 not contained in ReadoutElement" );
					ATH_MSG_DEBUG ( iname << " "<<ieta<<" "<<iphi<<" "<<ir<<" "<<iz<<" "<<idp);
					}
					if( !(rpc->containsId(idetaN))){
					ATH_MSG_DEBUG ( "Strip idetaN not contained in ReadoutElement " << etastripN );
					ATH_MSG_DEBUG ( iname << " "<<ieta<<" "<<iphi<<" "<<ir<<" "<<iz<<" "<<idp);
					}
					if( !(rpc->containsId(idphi1))){
					ATH_MSG_DEBUG ( "Strip idphi1 not contained in ReadoutElement" );
					ATH_MSG_DEBUG ( iname << " "<<ieta<<" "<<iphi<<" "<<ir<<" "<<iz<<" "<<idp);
					}
					if( !(rpc->containsId(idphiN))){
					ATH_MSG_DEBUG ( "Strip iphiN not contained in ReadoutElement " << phistripN );
					ATH_MSG_DEBUG ( iname << " "<<ieta<<" "<<iphi<<" "<<ir<<" "<<iz<<" "<<idp);
					}
				      */
				      continue ;
				    }
                      
				    Amg::Vector3D Poseta1  	       (0.,0.,0.);
				    Amg::Vector3D Poseta1L 	       (0.,0.,0.);
				    Amg::Vector3D PosetaN  	       (0.,0.,0.);
				    Amg::Vector3D PosetaNL 	       (0.,0.,0.);
				    Amg::Vector3D Inters3DLeta1	       (0.,0.,0.);
				    Amg::Vector3D Posphi1  	       (0.,0.,0.);
				    Amg::Vector3D Posphi1L 	       (0.,0.,0.);
				    Amg::Vector3D PosphiN  	       (0.,0.,0.);
				    Amg::Vector3D PosphiNL 	       (0.,0.,0.);
				    Amg::Vector3D Inters3DLphi1	       (0.,0.,0.);
                      
				    Poseta1		    = rpc->stripPos(ideta1)			       ;				    
//				    if( Poseta1  	       == 0 ) return StatusCode::SUCCESS;
				    Poseta1L		    = ((rpc->transform(ideta1)).inverse())*Poseta1     ;
// 				    if( Poseta1L 	       == NULL ) return StatusCode::SUCCESS;
 				    PosetaN		    = rpc->stripPos(idetaN)			       ;
// 				    if( PosetaN  	       == NULL ) return StatusCode::SUCCESS;
 				    PosetaNL		    = ((rpc->transform(ideta1)).inverse())*PosetaN     ;
// 				    if( PosetaNL 	       == NULL ) return StatusCode::SUCCESS;
 				    Inters3DLeta1	    = ((rpc->transform(ideta1)).inverse())*Inters3DG   ;
// 				    if( Inters3DLeta1	       == NULL ) return StatusCode::SUCCESS;
 				    Posphi1		    = rpc->stripPos(idphi1)			       ;
// 				    if( Posphi1  	       == NULL ) return StatusCode::SUCCESS;
 				    Posphi1L		    = ((rpc->transform(idphi1)).inverse())*Posphi1     ;
// 				    if( Posphi1L 	       == NULL ) return StatusCode::SUCCESS;
 				    PosphiN		    = rpc->stripPos(idphiN)			       ;
// 				    if( PosphiN  	       == NULL ) return StatusCode::SUCCESS;
 				    PosphiNL		    = ((rpc->transform(idphi1)).inverse())*PosphiN     ;
// 				    if( PosphiNL 	       == NULL ) return StatusCode::SUCCESS;
 				    Inters3DLphi1	    = ((rpc->transform(idphi1)).inverse())*Inters3DG   ;
// 				    if( Inters3DLphi1	       == NULL ) return StatusCode::SUCCESS;
 				    
				    
				
		                
				    hitstripphi = (( Inters3DLphi1.x()-Posphi1L.x() + phipitch  ))/phipitch;
				    if( Posphi1L.x() > PosphiNL.x() )hitstripphi=-hitstripphi;
				    //hitstripphi++;
				    hitstripeta = (( Inters3DLeta1.x()-Poseta1L.x() + etapitch  ))/etapitch;
				    //if(ieta<0)hitstripeta=-hitstripeta;
				    //hitstripeta++;
				    if( Poseta1L.x() > PosetaNL.x() )hitstripeta=-hitstripeta;
       
				    /*
				      std::cout << "Look for Crossing "<< iname << " iname  " << ieta << "  ieta " <<  iphi
				      << "  iphi " << ir <<  " ir  " << iz << " iz " << idp << "  idp " <<
				      ig << "  ig " << hitstripphi  << " hitstripphi " << hitstripeta  << " hitstripeta " <<std::endl;
				    */
       
				    //look for gap intersection
				    if( !(hitstripphi >  float(nstripfiducial)) ) continue ;
				    if( !(hitstripeta >  float(nstripfiducial)) ) continue ;
				    if( !(hitstripphi <  float(rpc->NphiStrips()-nstripfiducial)) ) continue ;
				    if( !(hitstripeta <  float(rpc->NetaStrips()-nstripfiducial)) ) continue ;
       
        
				    //std::cout << "Intersection at "<< Inters3DG.x() << " " << Inters3DG.y() << " "  << Inters3DG.z() << std::endl;
       
				    foundonehiteta =    0 ;
				    foundonehitphi =    0 ;  
				    foundEtaPhi    =    0 ;
				    foundEtaOrPhi  =    0 ;
				    PanelIndexeta  = 1000 ;
				    PanelIndexphi  = 1000 ;
        
 				    for(int imeasphi = 0; imeasphi != 1 + 1; imeasphi++ ){
				      if(imeasphi==0){
					irpcstrip = int(hitstripeta+1);
					if(irpcstrip<1||rpc->NetaStrips()<irpcstrip)continue;
				      }
				      else { 
					irpcstrip = int(hitstripphi+1);
					if(irpcstrip<1||rpc->NphiStrips()<irpcstrip)continue;
				      }
				      Identifier prdcoll_id  =  m_rpcIdHelper->channelID(idr, iz, idp, ig, imeasphi, irpcstrip) ;
       
				      if( prdcoll_id == 0 )continue;
        
				      //get information from geomodel to book and fill rpc histos with the right max strip number
				      std::vector<int>   rpcstripshift = RpcGM::RpcStripShift(m_muonMgr,m_rpcIdHelper,prdcoll_id, 0)  ;
 
				      NphiStrips	     =  rpcstripshift[0] ;
				      ShiftPhiStrips     =  rpcstripshift[1] ;
				      NetaStrips	     =  rpcstripshift[2] ;
				      ShiftEtaStrips     =  rpcstripshift[3] ;
				      ShiftStrips	     =  rpcstripshift[4] ;
				      NetaStripsTot      =  rpcstripshift[5] ;
				      NetaStripsTotSideA =  rpcstripshift[6] ;
				      NetaStripsTotSideC =  rpcstripshift[7] ;
				      ShiftEtaStripsTot  =  rpcstripshift[8] ;
				      Nbin  	     =  rpcstripshift[9] ;
				      EtaStripSign       =  rpcstripshift[10];
				      SectorLogic	       =  rpcstripshift[11]  ;
				      Side	       =  rpcstripshift[12]  ;
				      PanelIndex	     =  rpcstripshift[13];
				      Settore          =  rpcstripshift[14];
				      strip_dbindex      =  rpcstripshift[16]  ; //ang cool
				    
				      m_SummaryHist_Idx = (Settore-1)*m_SummaryHist_Size/16;
 
				      //get name for titles and labels 
				      std::vector<std::string>   rpclayersectorsidename = RpcGM::RpcLayerSectorSideName(m_rpcIdHelper,prdcoll_id, 0)  ;
 
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
				      sector_dphi_layer        = rpclayersectorsidename[12]  ; //ang cool
	      
				      if( (m_doRadiography==1) && (imeasphi==1) ){
					stripetaatlas  =  int( ( hitstripeta + ShiftEtaStripsTot )*EtaStripSign );	     
					stripphisector =  int(  hitstripphi + ShiftPhiStrips ); 
					sc = rpc_radiography.getHist(rpcSectorLayerTrackProj, sector_name+"_"+layeronly_name+"_TrackProj");                       
                
					if (rpcSectorLayerTrackProj) {
					  rpcSectorLayerTrackProj->Fill(stripetaatlas, stripphisector  );
					}
					else {ATH_MSG_DEBUG ( " rpcSectorLayerTrackProj not in hist list!" );}
				      }
		
				      //get chamber hardware name
				      std::string hardware_name=convertChamberName(iname,ieta,iphi,type) ;
          
				      if(m_rpcchamberhist){
 
					ATH_MSG_DEBUG ( "RPC Hardware chamber name in the selected area : " << hardware_name );	    
 
					histo_flag=true;
					for (std::vector<std::string>::const_iterator iter=layer_name_list.begin(); iter!=layer_name_list.end(); iter++){
					  if ( (hardware_name+layer_name)==*iter){histo_flag=false;}
					}
					if (histo_flag){ 
					  layer_name_list.push_back(hardware_name+layer_name); 
					  bookRPCTracksLayerHistograms(hardware_name, layer_name, layer0_name, Nbin, 0 , Nbin);
					}
          
					histo_flag=true ;
					for (std::vector<std::string>::const_iterator iter=layer_name_list_panel.begin(); iter!=layer_name_list_panel.end(); iter++){
					  if ( (hardware_name+layer_name_panel)==*iter){histo_flag=false;}
					}
					if (histo_flag){ 
					  layer_name_list_panel.push_back(hardware_name+layer_name_panel)  ; 
					  layer_name_bin_list_panel.push_back( PanelIndex ) 	       ; 
					}
                                   
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
                 
        
        
				      const MuonDQAHistList& hists1 = m_stationHists.getList( hardware_name + "/Tracks/" + layer_name);
				      TH1* rpctrackprojlayer  = hists1.getH1( hardware_name + "_" + layer_name + "_trackproj" );
				      TH1* rpchitontracklayer = hists1.getH1( hardware_name + "_" + layer_name + "_hitontrack");
				      TH1* rpcresidualslayer  = hists1.getH1( hardware_name + "_" + layer_name + "_residuals" );
        
        
				      if (rpctrackprojlayer) {rpctrackprojlayer->Fill( float( irpcstrip + ShiftStrips)  -0.5 );}
				      else {ATH_MSG_DEBUG ( "rpcstriptrackprojlayer not in hist list!" );}
				      // summary track projection    
				      if(PanelIndex<1000) m_SummaryHist[enumSumTrackProj+m_SummaryHist_Idx]-> Fill( PanelIndex );
				      //
				      if ( m_doCoolDB ) {
					sc = rpcCoolDb.getHist( rpcCool_TrackProj, sector_dphi_layer+"_TrackProj" ) ;	    
					if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_TrackProj hist" );
					if(rpcCool_TrackProj)rpcCool_TrackProj->Fill( strip_dbindex );   
				      }
                 
				      if (LayerTrackProjection) {LayerTrackProjection->Fill( ilayertype + 6*imeasphi + 12*small_large);}
				      if ( Side == 1 ) {
					if ( RPCBA_layerTrackProj )  RPCBA_layerTrackProj->Fill( ilayertype + 6*imeasphi + 12*small_large);
				      }		
				      else{
					if ( RPCBC_layerTrackProj )  RPCBC_layerTrackProj->Fill( ilayertype + 6*imeasphi + 12*small_large);
				      }
		
		     
				      // strip track proj
               
				      //foundonehiteta = 0 ;
				      //foundonehitphi = 0 ;
             				   
				      //look for clusters near the projection
				      it = rpc_clusterContainer->begin();
				      // Access by Collection
				      for ( ; it != rpc_clusterContainer->end() ; ++it ) {
             	   
					const Muon::RpcPrepDataCollection* clusterCollection = *it;
             	    
					if (clusterCollection->size()>0) {
					  ATH_MSG_DEBUG ( "New Cluster collection" );
             	   
					  for (Muon::RpcPrepDataCollection::const_iterator rpcCollection = clusterCollection->begin();
					       rpcCollection != clusterCollection->end(); ++rpcCollection) {
             	   
					    Identifier prd_id = (*rpcCollection)->identify();
             	   
					    ATH_MSG_DEBUG ( "Adding a new cluster " );
               
					    irpc_clus_size	 = ((*rpcCollection)->rdoList()).size();
					    irpc_clus_time	 =  (*rpcCollection)->time()	       ;
					    irpc_clus_station  =  m_rpcIdHelper->stationName(prd_id) ;
					    irpc_clus_eta	 =  m_rpcIdHelper->stationEta(prd_id)  ;
					    irpc_clus_phi	 =  m_rpcIdHelper->stationPhi(prd_id)  ;
					    irpc_clus_doublr   =  m_rpcIdHelper->doubletR(prd_id)    ;
					    irpc_clus_doublz   =  m_rpcIdHelper->doubletZ(prd_id)    ;
					    irpc_clus_doublphi =  m_rpcIdHelper->doubletPhi(prd_id)  ;
					    irpc_clus_gasgap   =  m_rpcIdHelper->gasGap(prd_id)      ;
					    irpc_clus_measphi  =  m_rpcIdHelper->measuresPhi(prd_id) ;
             	
					    if(irpc_clus_station    !=    iname   )continue;
					    if(irpc_clus_eta	  !=     ieta   )continue;
					    if(irpc_clus_phi	  !=     iphi   )continue;
					    if(irpc_clus_doublr     !=       ir   )continue;
					    if(irpc_clus_doublz     !=       iz   )continue;
					    if(irpc_clus_doublphi   !=      idp   )continue;
					    if(irpc_clus_gasgap     !=       ig   )continue;
					    if(irpc_clus_measphi  != imeasphi     )continue;
             			 
               
					    Layer = 4 ;
					    if(  irpc_clus_station < 4 || irpc_clus_station  == 8 ) Layer = 0 ;
					    Layer += irpc_clus_gasgap - 1 + ( irpc_clus_doublr - 1)*2 ;
					    if(Layer!=ilayertype)continue;
	
             	      
					    // get the average strip and cluster position
             	   
					    Amg::Vector3D stripPosC(0,0,0);
					    av_strip = 0 ;
					    for(int i=0; i!=irpc_clus_size ; i++){
					      Identifier id = ((*rpcCollection)->rdoList())[i]	     ;
					      const MuonGM::RpcReadoutElement* descriptor = m_muonMgr->getRpcReadoutElement(id);
					      stripPosC += descriptor->stripPos(id)	   ;
					      strip	   = int(m_rpcIdHelper->strip(id)) ;
					      av_strip  += float(strip)		   ;
					    }
					    if( irpc_clus_size != 0 ){ 
					      av_strip  = av_strip  / irpc_clus_size ;
					      stripPosC = stripPosC / irpc_clus_size ;
					    }
        
					    irpc_clus_posx = stripPosC.x() ;
					    irpc_clus_posy = stripPosC.y() ;
					    irpc_clus_posz = stripPosC.z() ; 
	  
					    Amg::Vector3D Vect3DCosL  (0,0,0)  ;
					    Amg::Vector3D stripPosCL  (0,0,0)  ;
					    Amg::Vector3D Inters3DLCL (0,0,0)  ;
	  
					    Vect3DCosL   = ((rpc->transform(ideta1)).inverse())*Vect3DCosL  ;
//					    if(Vect3DCosL== NULL ) return StatusCode::SUCCESS ;
					    stripPosCL   = ((rpc->transform(ideta1)).inverse())*stripPosC  ;
//					    if(stripPosCL== NULL ) return StatusCode::SUCCESS ;
					    Inters3DLCL  = ((rpc->transform(ideta1)).inverse())*Inters3DG  ;
//					    if(Inters3DLCL== NULL ) return StatusCode::SUCCESS ;
					    
					    if(imeasphi==1){
					      Vect3DCosL   = ((rpc->transform(idphi1)).inverse())*Vect3DCosL  ;
//					      if(Vect3DCosL== NULL ) return StatusCode::SUCCESS ;
					      stripPosCL   = ((rpc->transform(idphi1)).inverse())*stripPosC  ;
//					      if(stripPosCL== NULL ) return StatusCode::SUCCESS ;
					      Inters3DLCL  = ((rpc->transform(idphi1)).inverse())*Inters3DG  ;
//					      if(Inters3DLCL== NULL ) return StatusCode::SUCCESS ;
					    }
	    
					    // std::cout <<imeasphi << " Vect "<< Vect3DCos.x()  << " " <<  Vect3DCos.y()<< "  " << Vect3DCos.z() ;
					    // std::cout << " "<< Vect3DCosL.x()  << " " <<  Vect3DCosL.y()<< "  " << Vect3DCosL.z() ;		                       
					    // std::cout << " clus "<< stripPosC.x()  << " " <<  stripPosC.y()<< "  " << stripPosC.z() ;
					    // std::cout << " "<< stripPosCL.x()  << " " <<  stripPosCL.y()<< "  " << stripPosCL.z() << std::endl;	       
			  
					    /*
					      std::cout << "Look for cluster "<< irpc_clus_station << " iname  " << irpc_clus_eta << "  ieta " <<  irpc_clus_phi
					      << "  iphi " << irpc_clus_doublr <<  " ir  " << irpc_clus_doublz << " iz " << irpc_clus_doublphi << "  idp " <<
					      irpc_clus_gasgap << "  ig " << irpc_clus_size  << "  irpc_clus_size " << int(m_rpcIdHelper->strip(((*rpcCollection)->rdoList())[0]))   << 
					      " strip e measphi " << irpc_clus_measphi<<std::endl;
					    */
    

    
					    //std::cout << irpc_clus_measphi << " Measphi Found hit at "<< irpc_clus_posx << " " << irpc_clus_posy << " "  << irpc_clus_posz << std::endl;

					    stripontrack = 0 ;
					    residuals    = 0 ;
					    residuals    = stripPosCL.x()-Inters3DLCL.x();
             	
					    if (rpcresidualslayer) {rpcresidualslayer->Fill( residuals );}
					    else {  ATH_MSG_DEBUG ( "rpcstripresidualslayer not in hist list!" );}
             			 
					    if (f_rpcEtaResidual!=0 && imeasphi==0) {f_rpcEtaResidual->Fill( residuals );}
					    else {  ATH_MSG_DEBUG ( "f_rpcEtaResidual not in hist list!" );}
               
					    if (f_rpcPhiResidual!=0 && imeasphi==1) {f_rpcPhiResidual->Fill( residuals );}
					    else {  ATH_MSG_DEBUG ( "f_rpcPhiResidual not in hist list!" );} 
	
					    char m_ResidualVsCS_title_char[100] = "ResidualVsCS";
					    sprintf(m_ResidualVsCS_title_char , "ResidualVsCS%d", irpc_clus_size) ;
					    if(irpc_clus_size<maxCSres+1) { 
					      sc = rpcprd_shift.getHist(ResidualVsCS , m_ResidualVsCS_title_char );
					      ResidualVsCS -> Fill (residuals) ;
					    }
					    // summary residuals CS1
					    if ( irpc_clus_size==1 ) {
					      if(PanelIndex<1000) m_SummaryHist[enumSumRes_CS1_entries+m_SummaryHist_Idx]-> Fill( PanelIndex );
					      //
					      if ( m_doCoolDB ) {
						sc = rpcCoolDb.getHist( rpcCool_Res_CS1_entries, sector_dphi_layer+"_Res_CS1_entries" ) ;	    
						if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_Res_CS1_entries hist" );
						if(rpcCool_Res_CS1_entries)rpcCool_Res_CS1_entries->Fill( strip_dbindex ); 
					      }
	
					      if(PanelIndex<1000) m_SummaryHist[enumSumRes_CS1_NotNorm+m_SummaryHist_Idx]-> Fill( PanelIndex, residuals );
					      //
					      if ( m_doCoolDB ) {
						sc = rpcCoolDb.getHist( rpcCool_Res_CS1_NotNorm, sector_dphi_layer+"_Res_CS1_NotNorm" ) ;	    
						if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_Res_CS1_NotNorm hist" );
						if(rpcCool_Res_CS1_NotNorm)rpcCool_Res_CS1_NotNorm->Fill( strip_dbindex, residuals );      
					      }  
    
					      if(PanelIndex<1000) m_SummaryHist[enumSumRes_CS1_square+m_SummaryHist_Idx]-> Fill( PanelIndex, residuals*residuals );
					      //
					      if ( m_doCoolDB ) {
						sc = rpcCoolDb.getHist( rpcCool_Res_CS1_square, sector_dphi_layer+"_Res_CS1_square" ) ;	    
						if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_Res_CS1_square hist" );
						if(rpcCool_Res_CS1_square)rpcCool_Res_CS1_square->Fill( strip_dbindex, residuals*residuals  );     
					      }
					    }
					    // summary residuals CS2
					    if ( irpc_clus_size==2 ) {
					      if(PanelIndex<1000) m_SummaryHist[enumSumRes_CS2_entries+m_SummaryHist_Idx]-> Fill( PanelIndex );
					      //
					      if ( m_doCoolDB ) {
						sc = rpcCoolDb.getHist( rpcCool_Res_CS2_entries, sector_dphi_layer+"_Res_CS2_entries" ) ;	    
						if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_Res_CS2_entries hist" );
						if(rpcCool_Res_CS2_entries)rpcCool_Res_CS2_entries->Fill( strip_dbindex ); 
					      }  
 
					      if(PanelIndex<1000) m_SummaryHist[enumSumRes_CS2_NotNorm+m_SummaryHist_Idx]-> Fill( PanelIndex, residuals );
					      //
					      if ( m_doCoolDB ) {
						sc = rpcCoolDb.getHist( rpcCool_Res_CS2_NotNorm, sector_dphi_layer+"_Res_CS2_NotNorm" ) ;	    
						if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_Res_CS2_NotNorm hist" );
						if(rpcCool_Res_CS2_NotNorm)rpcCool_Res_CS2_NotNorm->Fill( strip_dbindex, residuals );      
					      }
  
					      if(PanelIndex<1000) m_SummaryHist[enumSumRes_CS2_square+m_SummaryHist_Idx]-> Fill( PanelIndex, residuals*residuals );
					      //
					      if ( m_doCoolDB ) {
						sc = rpcCoolDb.getHist( rpcCool_Res_CS2_square, sector_dphi_layer+"_Res_CS2_square" ) ;	    
						if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_Res_CS2_square hist" );
						if(rpcCool_Res_CS2_square)rpcCool_Res_CS2_square->Fill( strip_dbindex, residuals*residuals );      
					      }
					    }
					    // summary residuals CSmore2
					    if ( irpc_clus_size>2 ) {
					      if(PanelIndex<1000) m_SummaryHist[enumSumRes_CSmore2_entries+m_SummaryHist_Idx]-> Fill( PanelIndex );
					      //
					      if ( m_doCoolDB ) {
						sc = rpcCoolDb.getHist( rpcCool_Res_CS2_entries, sector_dphi_layer+"_Res_CS2_entries" ) ;	    
						if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_Res_CS2_entries hist" );
						if(rpcCool_Res_CS2_entries)rpcCool_Res_CS2_entries->Fill( strip_dbindex ); 
					      }
 
					      if(PanelIndex<1000) m_SummaryHist[enumSumRes_CSmore2_NotNorm+m_SummaryHist_Idx]-> Fill( PanelIndex, residuals );
					      //
					      if ( m_doCoolDB ) {
						sc = rpcCoolDb.getHist( rpcCool_Res_CS2_NotNorm, sector_dphi_layer+"_Res_CS2_NotNorm" ) ;	    
						if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_Res_CS2_NotNorm hist" );
						if(rpcCool_Res_CS2_NotNorm)rpcCool_Res_CS2_NotNorm->Fill( strip_dbindex, residuals );      
					      }
            
					      if(PanelIndex<1000) m_SummaryHist[enumSumRes_CSmore2_square+m_SummaryHist_Idx]-> Fill( PanelIndex, residuals*residuals );
					      //
					      if ( m_doCoolDB ) {
						sc = rpcCoolDb.getHist( rpcCool_Res_CS2_square, sector_dphi_layer+"_Res_CS2_square" ) ;	    
						if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_Res_CS2_square hist" );
						if(rpcCool_Res_CS2_square)rpcCool_Res_CS2_square->Fill( strip_dbindex, residuals*residuals  );     
					      }
					    }
					    //efficiency only if average strip +/-1 projected track
    

					    //eta view
					    if(irpc_clus_measphi==0 && foundonehiteta==0){
					      //if(abs(hitstripeta-avstrip)<=nstripfiduceff){
					      if(abs(residuals)<=m_resThEff){
						if (rpchitontracklayer) {rpchitontracklayer->Fill( float( irpcstrip + ShiftStrips)  -0.5 );}
						else {  ATH_MSG_DEBUG ( "rpcstriphitontracklayer not in hist list!" );}
						if (LayerHitOnTrack) {LayerHitOnTrack->Fill( ilayertype + 6*imeasphi + 12*small_large);}
						if ( Side == 1 ) {
						  if ( RPCBA_layerHitOnTrack )  RPCBA_layerHitOnTrack->Fill( ilayertype + 6*imeasphi + 12*small_large);
						}		
						else{
						  if ( RPCBC_layerHitOnTrack )  RPCBC_layerHitOnTrack->Fill( ilayertype + 6*imeasphi + 12*small_large);
						}
			  
			  
						// Summary Hit On Track
						sc = rpcprd_expert_sum.getHist( SummaryHitOnTrack, "SummaryHitOnTrack_" + sector_name ) ;
						if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get SummaryHitOnTrack hist in EtaView" );		    
						if(PanelIndex<1000) SummaryHitOnTrack -> Fill( float(PanelIndex) ) ;
						//
						if ( m_doCoolDB ) {
						  sc = rpcCoolDb.getHist( rpcCool_HitOnTrack, sector_dphi_layer+"_HitOnTrack" ) ;	    
						  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_HitOnTrack hist" );
						  if(rpcCool_HitOnTrack)rpcCool_HitOnTrack->Fill( strip_dbindex ); 
						}
			   
						//incidence angle of tracks on surface (z,rho)
			           
			                        float norm3DGaprho = sqrt(norm3DGap.x() *  norm3DGap.x() + norm3DGap.y() *  norm3DGap.y()) ;
			                        float Vect3DCosrho = sqrt(Vect3DCos.x() *  Vect3DCos.x() + Vect3DCos.y() *  Vect3DCos.y()) ;
						
						incAngle = norm3DGaprho *  Vect3DCosrho + norm3DGap.z() *  Vect3DCos.z() ;
						norm1 = sqrt(norm3DGaprho*norm3DGaprho+norm3DGap.z()*norm3DGap.z()) ;
						norm2 = sqrt(Vect3DCosrho*Vect3DCosrho+Vect3DCos.z()*Vect3DCos.z()) ;
			  			   
						if(norm1*norm2>0)incAngle = incAngle / norm1 / norm2 ;
			  			  
						sc = rpcprd_shift.getHist(m_rpcCS_angleTrasv, "CSvsTrasversalAngleTrack") ;
						if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get CSvsTrasversalAngleTrack  hist in EtaView" );
						if ( m_rpcCS_angleTrasv ) m_rpcCS_angleTrasv->Fill( incAngle, irpc_clus_size );

						//incidence angle of tracks on surface (x,y)
			   
						incAngle = norm3DGap.x() *  Vect3DCos.x() + norm3DGap.y() *  Vect3DCos.y() ;
						norm1 = sqrt(norm3DGap.x()*norm3DGap.x()+norm3DGap.y()*norm3DGap.y()) ;
						norm2 = sqrt(Vect3DCos.x()*Vect3DCos.x()+Vect3DCos.y()*Vect3DCos.y()) ;
			   
						if(norm1*norm2>0)incAngle = incAngle / norm1 / norm2 ;
			   
						sc = rpcprd_shift.getHist(m_rpcCS_angleLong, "CSvsLongitudinalAngleTrack") ;
						if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get CSvsLongitudinalAngleTrack  hist in EtaView" );
						if ( m_rpcCS_angleLong ) m_rpcCS_angleLong->Fill( incAngle, irpc_clus_size );

	
    
						foundonehiteta =	       1 ;
						PanelIndexeta  =    PanelIndex ;
						StripIndexeta  = strip_dbindex ;
					      }}
					       
					    //phi view
					    if(irpc_clus_measphi==1 && foundonehitphi==0){
					      //if(abs(hitstripphi-avstrip)<=nstripfiduceff){
					      if(abs(residuals)<=m_resThEff){
						if (rpchitontracklayer) {rpchitontracklayer->Fill( float( irpcstrip + ShiftStrips)  -0.5 );}
						else {  ATH_MSG_DEBUG ( "rpcstriphitontracklayer not in hist list!" );}
						if (LayerHitOnTrack) {LayerHitOnTrack->Fill( ilayertype + 6*imeasphi + 12*small_large);}
						if ( Side == 1 ) {
						  if ( RPCBA_layerHitOnTrack )  RPCBA_layerHitOnTrack->Fill( ilayertype + 6*imeasphi + 12*small_large);
						}		
						else{
						  if ( RPCBC_layerHitOnTrack )  RPCBC_layerHitOnTrack->Fill( ilayertype + 6*imeasphi + 12*small_large);
						}
						sc = rpcprd_expert_sum.getHist( SummaryHitOnTrack, "SummaryHitOnTrack_" + sector_name ) ;
						if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get SummaryHitOnTrack hist in PhiView" );		    
						if(PanelIndex<1000)SummaryHitOnTrack -> Fill( float(PanelIndex) ) ;
						//
						if ( m_doCoolDB ) {
						  sc = rpcCoolDb.getHist( rpcCool_HitOnTrack, sector_dphi_layer+"_HitOnTrack" ) ;	    
						  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_HitOnTrack hist" );
						  if(rpcCool_HitOnTrack)rpcCool_HitOnTrack->Fill( strip_dbindex ); 
						}  
                          
						//incidence angle of tracks on surface (x,y)
			   
						incAngle = norm3DGap.x() *  Vect3DCos.x() + norm3DGap.y() *  Vect3DCos.y() ;
						norm1 = sqrt(norm3DGap.x()*norm3DGap.x()+norm3DGap.y()*norm3DGap.y()) ;
						norm2 = sqrt(Vect3DCos.x()*Vect3DCos.x()+Vect3DCos.y()*Vect3DCos.y()) ;
			   
						if(norm1*norm2>0)incAngle = incAngle / norm1 / norm2 ;
			                              		      
						sc = rpcprd_shift.getHist(m_rpcCS_angleTrasv, "CSvsTrasversalAngleTrack") ;
						if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get CSvsTrasversalAngleTrack  hist in EtaView" );
						if ( m_rpcCS_angleTrasv ) m_rpcCS_angleTrasv->Fill( incAngle, irpc_clus_size );
			   
						//incidence angle of tracks on surface (z,rho)
			                        
			           
			                        float norm3DGaprho = sqrt(norm3DGap.x() *  norm3DGap.x() + norm3DGap.y() *  norm3DGap.y()) ;
			                        float Vect3DCosrho = sqrt(Vect3DCos.x() *  Vect3DCos.x() + Vect3DCos.y() *  Vect3DCos.y()) ;
						incAngle = norm3DGaprho *  Vect3DCosrho + norm3DGap.z() *  Vect3DCos.z() ;
						norm1 = sqrt(norm3DGaprho*norm3DGaprho+norm3DGap.z()*norm3DGap.z()) ;
						norm2 = sqrt(Vect3DCosrho*Vect3DCosrho+Vect3DCos.z()*Vect3DCos.z()) ;
			 			   
						if(norm1*norm2>0)incAngle = incAngle / norm1 / norm2 ;
			  			  		  
						sc = rpcprd_shift.getHist(m_rpcCS_angleLong, "CSvsLongitudinalAngleTrack") ;
						if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get CSvsLongitudinalAngleTrack  hist in EtaView" );
						if ( m_rpcCS_angleLong ) m_rpcCS_angleLong->Fill( incAngle, irpc_clus_size );
		    	  
						foundonehitphi =	       1 ;
						PanelIndexphi  = PanelIndex	 ;
						StripIndexphi  = strip_dbindex ;
					      }

}
 
					  
					  if ( m_doRadiography ) {
					    if(foundonehitphi==1 && foundonehiteta==1 && imeasphi==1 && foundEtaPhi==0) {
					      sc = rpc_radiography.getHist(rpcSectorLayerResponse,  sector_name+"_"+layeronly_name+"_Response" ); 
					      if (rpcSectorLayerResponse) {
					       rpcSectorLayerResponse->Fill(stripetaatlas, stripphisector  );
					       float a1 =  rpcSectorLayerResponse->GetBinContent(stripetaatlas, stripphisector  );
					       float a2 =  rpcSectorLayerTrackProj->GetBinContent(stripetaatlas, stripphisector  );
					       if(a2<a1)std::cout << " WARNING DIAMOND AND" <<std::endl;
					      }
					      else {  ATH_MSG_DEBUG ( "rpcSectorLayerResponse not in hist list!" );}
					      foundEtaPhi=1;
					     }
					     if ( (foundonehitphi==1 || foundonehiteta==1 ) && imeasphi==1  && foundEtaOrPhi==0 ) {
					      sc = rpc_radiography.getHist(rpcSectorLayerResponseOR, sector_name+"_"+layeronly_name+"_ResponseEtaOrPhi" ); 
					      if ( rpcSectorLayerResponseOR ) { 
					       rpcSectorLayerResponseOR->Fill(stripetaatlas, stripphisector  );
					       float a1 =  rpcSectorLayerResponseOR->GetBinContent(stripetaatlas, stripphisector  );
					       float a2 =  rpcSectorLayerTrackProj->GetBinContent(stripetaatlas, stripphisector  );
					       if(a2<a1)std::cout << " WARNING DIAMOND OR" <<std::endl;
					      }
					      else {  ATH_MSG_DEBUG ( "rpcSectorLayerResponseEtaOrPhi not in hist list!" );}
					      foundEtaOrPhi=1;
					     }
					   }  
   
					    res_th = nstripfiduceff ;

 					  }}}//end loop on cluster
//     
//   
 				    }//end loop on views

				    if(foundonehiteta==1&&foundonehitphi==1){	
				    			
				      if(PanelIndexeta<1000)m_SummaryHist[enumSumHitOnTrack_withCrossStrip+m_SummaryHist_Idx]-> Fill( float(PanelIndexeta) ) ;
				      if(PanelIndexphi<1000)m_SummaryHist[enumSumHitOnTrack_withCrossStrip+m_SummaryHist_Idx]-> Fill( float(PanelIndexphi) ) ;
				      //
				      if ( m_doCoolDB ) {
					sc = rpcCoolDb.getHist( rpcCool_HitOnTrack_withCrossStrip, sector_dphi_layer+"_HitOnTrack_withCrossStrip" ) ;		
					if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_HitOnTrack_withCrossStrip hist" );
					if(rpcCool_HitOnTrack_withCrossStrip)rpcCool_HitOnTrack_withCrossStrip->Fill( StripIndexeta );
					if(rpcCool_HitOnTrack_withCrossStrip)rpcCool_HitOnTrack_withCrossStrip->Fill( StripIndexphi );	
				      }
				    }  
				  
				  
				    if ( m_doRadiography ) {
				      if(foundonehitphi==1 && foundonehiteta==1 ) {
					sc = rpc_radiography.getHist(rpcSectorLayerResponse,  sector_name+"_"+layeronly_name+"_Response" ); 
					if (rpcSectorLayerResponse) {
					  rpcSectorLayerResponse->Fill(stripetaatlas, stripphisector  );
					  //float a1 =  rpcSectorLayerResponse->GetBinContent(stripetaatlas, stripphisector  );
					  //float a2 =  rpcSectorLayerTrackProj->GetBinContent(stripetaatlas, stripphisector  );
					  //if(a2<a1)std::cout << " WARNING DIAMOND AND" <<std::endl;
					}
					else {  ATH_MSG_DEBUG ( "rpcSectorLayerResponse not in hist list!" );}
					      
				      }
				      if ( foundonehitphi==1 || foundonehiteta==1 ) {
					sc = rpc_radiography.getHist(rpcSectorLayerResponseOR, sector_name+"_"+layeronly_name+"_ResponseEtaOrPhi" ); 
					if ( rpcSectorLayerResponseOR ) { 
					  rpcSectorLayerResponseOR->Fill(stripetaatlas, stripphisector  );
					  //float a1 =  rpcSectorLayerResponseOR->GetBinContent(stripetaatlas, stripphisector  );
					  //float a2 =  rpcSectorLayerTrackProj->GetBinContent(stripetaatlas, stripphisector  );
					  //if(a2<a1)std::cout << " WARNING DIAMOND OR" <<std::endl;
					}
					else {  ATH_MSG_DEBUG ( "rpcSectorLayerResponseEtaOrPhi not in hist list!" );}
					      
				      }
				    } // end  Radiography 
				  
				  
				  
    
				  } 
} 
				  } } } } } }//end look for gap intersection

         }}}//Chi2dofCut//First
 
 	      if( ilayertype==6  ) {  rpcNtracks -> Fill (N_Rpc_Tracks) ; }
 	      else { f_rpcNtracks -> Fill (N_Rpc_Tracks) ;  }
     
 	      ///multi tracks end
   
 	    }//for layertype
 	    delete[] Rpc_track ;
 	    Rpc_track=0;     
 	  }//if cluster more than
  
	  // Calculate Noise 
	  it = rpc_clusterContainer->begin();
	  for ( ; it != rpc_clusterContainer->end() ; ++it ) {

	    const Muon::RpcPrepDataCollection* clusterCollection = *it;
    
	    if (clusterCollection->size()>0) {
      
	      ATH_MSG_DEBUG ( "New Cluster collection" );

	      for (Muon::RpcPrepDataCollection::const_iterator rpcCollection = clusterCollection->begin(); 
		   rpcCollection != clusterCollection->end(); ++rpcCollection) {
	
		Identifier prd_id = (*rpcCollection)->identify();
	
	        ATH_MSG_DEBUG ( "Adding a new cluster " );
	     
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
    const MuonGM::RpcReadoutElement* rpc = m_muonMgr->getRpcReadoutElement(prd_id);
    if(!rpc) continue;
    const Amg::Vector3D stripPosC = rpc->stripPos(prd_id);

	      
		irpc_clus_posx = stripPosC.x() ;
		irpc_clus_posy = stripPosC.y() ;
		irpc_clus_posz = stripPosC.z() ;
	
  
		//get information from geomodel to book and fill rpc histos with the right max strip number
		std::vector<int>   rpcstripshift = RpcGM::RpcStripShift(m_muonMgr,m_rpcIdHelper,prd_id, 0)  ;
	    		  
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
		Settore            =  rpcstripshift[14];
		strip_dbindex      =  rpcstripshift[16]  ;
				    
		m_SummaryHist_Idx = (Settore-1)*m_SummaryHist_Size/16;
 
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
	      
	      
		irpc_clus_measphi  =  m_rpcIdHelper->measuresPhi(prd_id);
		  
		Amg::Vector3D  Vector3DGapG = rpc->gasGapPos(irpc_clus_doublz, irpc_clus_doublphi, irpc_clus_gasgap  );
		Amg::Vector3D norm3DGap   = rpc->normal   ( /*                 irpc_clus_doublphi, irpc_clus_gasgap  */);
		invgasgaparea  = rpc->gasGapSsize() * rpc->gasGapZsize() / 100;
		
		if(invgasgaparea!=0)invgasgaparea=1./invgasgaparea;
	      
		if(irpc_clus_measphi==0){
		  invstriparea=invgasgaparea*rpc->NetaStrips() ;
		}   
		else {
		  invstriparea=invgasgaparea*rpc->NphiStrips() ;
		}	
		
	        
		isNoise=1;
		
		Amg::Vector3D Inters3DG  (0,0,0);
		Amg::Vector3D Inters3DL  (0,0,0);
                Amg::Vector3D stripPosCL (0,0,0);
	
		for (int i_RpcTrack=0; i_RpcTrack!=N_RpcTrack; i_RpcTrack++) {
 
		  Inters3DG   =  RpcTrack_Poin3RLin .at(i_RpcTrack) + 
		    RpcTrack_Vect3DCos .at(i_RpcTrack) *
		    (norm3DGap.dot(Vector3DGapG-RpcTrack_Poin3RLin .at(i_RpcTrack)))/
		    (norm3DGap.dot(RpcTrack_Vect3DCos .at(i_RpcTrack) ));
	       
		  Inters3DL	 = ((rpc->transform(prd_id)).inverse())* Inters3DG   ;  				    
		  //if(  	Inters3DL       == NULL ) return StatusCode::SUCCESS;	     
		  stripPosCL	 = ((rpc->transform(prd_id)).inverse())* stripPosC   ;				    
		  //if(  	 stripPosCL       == NULL ) return StatusCode::SUCCESS;
                  
		  distance = fabs(Inters3DL.x() -  stripPosCL.x());
	        
		
		  if(distance<MergePointDistance) {
		    isNoise=0;
		  }   
		} // end loop on tracks
		
		if( isNoise==1 ) {
	        
		  //sc = rpcprd_expert_sum.getHist( SummaryNoiseTot_NotNorm, "SummaryNoiseTot_NotNorm_" + sector_name ) ;
		  //if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get SummaryNoiseTot_NotNorm hist in PhiView" );  			
		  if(PanelIndex<1000)m_SummaryHist[enumSumNoiseTot_NotNorm+m_SummaryHist_Idx]-> Fill( float(PanelIndex) , invgasgaparea) ;
		  //
		  if ( m_doCoolDB ) {
		    sc = rpcCoolDb.getHist( rpcCool_NoiseTot_NotNorm, sector_dphi_layer+"_NoiseTot_NotNorm" ) ;		    
		    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_NoiseTot_NotNorm hist" );
		    if(rpcCool_NoiseTot_NotNorm)rpcCool_NoiseTot_NotNorm->Fill( strip_dbindex, invstriparea);	  
		  }
		  // correlated noise
		  isNoiseCorr=0;
		  for (Muon::RpcPrepDataCollection::const_iterator rpcCollectionII = clusterCollection->begin(); 
		       rpcCollectionII != clusterCollection->end(); ++rpcCollectionII) {
		    Identifier prd_idII = (*rpcCollectionII)->identify();
	     
	            ATH_MSG_DEBUG ( "Adding a new cluster " );
	     
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
	   
	   
		    if( irpc_clus_stationII  != irpc_clus_station     ) continue ;		 
		    if( irpc_clus_etaII	   != irpc_clus_eta	    ) continue ;		 
		    if( irpc_clus_phiII	   != irpc_clus_phi	    ) continue ;		 
		    if( irpc_clus_doublrII   != irpc_clus_doublr      ) continue ;		 
		    if( irpc_clus_doublzII   != irpc_clus_doublz      ) continue ;		 
		    if( irpc_clus_doublphiII != irpc_clus_doublphi    ) continue ;		 
		    if( irpc_clus_gasgapII   == irpc_clus_gasgap      ) continue ;		 
		    if( irpc_clus_measphiII  != irpc_clus_measphi     ) continue ;
	   
		    // get the cluster position
		    const MuonGM::RpcReadoutElement* descriptorII = m_muonMgr->getRpcReadoutElement(prd_idII);
	
		    const Amg::Vector3D stripPosCII = descriptorII->stripPos(prd_idII);
	          
	   
		    distance =   (stripPosCII - stripPosC ).mag() ;
	   
		    if( distance < MergePointDistance ) {
		      isNoiseCorr = 1;
		    }
	
		  }
		  if ( isNoiseCorr==1) {
		    //sc = rpcprd_expert_sum.getHist( SummaryNoiseCorr_NotNorm, "SummaryNoiseCorr_NotNorm_" + sector_name ) ;
		    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get SummaryNoiseCorr_NotNorm hist" );  		      
		    if(PanelIndex<1000)m_SummaryHist[enumSumNoiseCorr_NotNorm+m_SummaryHist_Idx]-> Fill( float(PanelIndex), invgasgaparea) ;
		    //
		    if ( m_doCoolDB ) {
		      sc = rpcCoolDb.getHist( rpcCool_NoiseCorr_NotNorm, sector_dphi_layer+"_NoiseCorr_NotNorm" ) ;	      
		      if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_NoiseCorr_NotNorm hist" );
		      if(rpcCool_NoiseCorr_NotNorm)rpcCool_NoiseCorr_NotNorm->Fill( strip_dbindex, invstriparea );     
		    }
		    rpcCS_NoiseCorr->Fill( irpc_clus_size ) ;
		  }
		  else {
		    rpcCS_NoiseNotCorr->Fill( irpc_clus_size ) ;
		  }
		} // end if isNoise==1   
		else {
		  rpcCS_HitOnTrack->Fill( irpc_clus_size ) ;
		}	   
	     
 		
	      }}} // end loop on cluster to calculate noise
		  		 
	  //calculate noise map
	  if ( m_doRadiography ) {

	    int clsect; 
	    char sector_char[10];
	    Amg::Vector3D ImpVec(0,0,0);
	 
	    for (int i_3D0=0; i_3D0!=N_Rpc_Clusters3D; i_3D0++) {
	      //clsect = 2*Rpc_Phi_3D.at(i_3D0) - ( 1 - SmallLarge.at(i_3D0) ) ;
	      clsect = 2*Rpc_Phi_3D.at(i_3D0) - ( SmallLarge.at(i_3D0) ) ;
	
	      switch ( LayerType.at(i_3D0) ) {
	      case 0 : layeronly_name= "LowPt0"  ; break ;
	      case 1 : layeronly_name= "LowPt1"  ; break ;
	      case 2 : layeronly_name= "Pivot0"  ; break ;
	      case 3 : layeronly_name= "Pivot1"  ; break ;
	      case 4 : layeronly_name= "HighPt0" ; break ;
	      case 5 : layeronly_name= "HighPt1" ; break ;
	      }
	      sprintf( sector_char,"Sector%.2d", clsect );
	      sector_name = sector_char;
 		
		  		 
	      isNoise = 1 ;
	      for (int i_RpcTrack=0; i_RpcTrack!=N_RpcTrack; i_RpcTrack++) {
		ImpVec=( Rpc_Point.at(i_3D0) - RpcTrack_Poin3RLin.at(i_RpcTrack) ).cross( RpcTrack_Vect3DCos.at(i_RpcTrack) ); 
		if (  ( RpcTrack_Vect3DCos.at(i_RpcTrack) ).mag()!=0  ) ImpVec = ImpVec/( RpcTrack_Vect3DCos.at(i_RpcTrack) ).mag();
	 
		if ( ImpVec.mag()<MergePointDistance ) {
		  isNoise= 0 ;
		  //continue   ;
		}
	      }
	      if ( isNoise==1 ) {
		isNoiseCorr=0;
		for (int i_3DI=0; i_3DI!=N_Rpc_Clusters3D; i_3DI++) {
		  if ( Rpc_Phi_3D.at(i_3D0)     != Rpc_Phi_3D.at(i_3DI)     )  continue ;
		  if ( Rpc_Station_3D.at(i_3D0) != Rpc_Station_3D.at(i_3DI) )  continue ;
		  if ( Rpc_DBLr_3D.at(i_3D0)    != Rpc_DBLr_3D.at(i_3DI)    )  continue ;
		  if ( Rpc_Eta_3D.at(i_3D0)     != Rpc_Eta_3D.at(i_3DI)     )  continue ;
		  if ( Rpc_DBLz_3D.at(i_3D0)    != Rpc_DBLz_3D .at(i_3DI)   )  continue ;
		  if ( Rpc_DBLphi_3D.at(i_3D0)  != Rpc_DBLphi_3D.at(i_3DI)  )  continue ;
		  if ( Rpc_GasGap_3D.at(i_3D0)  == Rpc_GasGap_3D.at(i_3DI)  )  continue ;
		  distance = ( Rpc_Point.at(i_3D0) - Rpc_Point.at(i_3DI) ).mag() ;
		  if ( distance < MergePointDistance ) {
		    isNoiseCorr = 1;
		  }
		}
	      
		isNoisePhiStrip =  Rpc_avPhi_3D.at(i_3D0) ;
		if(Rpc_DBLphi_3D.at(i_3D0) == 2){
		  if(LayerType.at(i_3D0)<4&&Rpc_Station_3D.at(i_3D0)==2)isNoisePhiStrip += 64 ;
		  if(LayerType.at(i_3D0)<4&&(Rpc_Station_3D.at(i_3D0)==3||Rpc_Station_3D.at(i_3D0)==8))isNoisePhiStrip += 56 ;
		  if(LayerType.at(i_3D0)>3&&Rpc_Station_3D.at(i_3D0)==4)isNoisePhiStrip += 80 ;
		  if(LayerType.at(i_3D0)>3&&(Rpc_Station_3D.at(i_3D0)==5||Rpc_Station_3D.at(i_3D0)>8))isNoisePhiStrip += 64 ;
		}
		    
		if ( isNoiseCorr==1 ) {
		  sc = rpc_radiography.getHist(rpcSectorLayerNoiseCorr, sector_name+"_"+layeronly_name+"_NoiseCorr" );
		  if ( rpcSectorLayerNoiseCorr ) { rpcSectorLayerNoiseCorr->Fill( Rpc_avEta_3D.at(i_3D0),  isNoisePhiStrip ); }
		  else {  ATH_MSG_DEBUG ( "rpcSectorLayerNoiseCorr not in hist list!" );}		    
		}
		else {
		  sc = rpc_radiography.getHist(rpcSectorLayerNoiseNotCorr, sector_name+"_"+layeronly_name+"_NoiseNotCorr" );
		  if ( rpcSectorLayerNoiseNotCorr ) { rpcSectorLayerNoiseNotCorr->Fill( Rpc_avEta_3D.at(i_3D0), isNoisePhiStrip  ); }
		  else {  ATH_MSG_DEBUG ( "rpcSectorLayerNoiseNotCorr not in hist list!" );}
		}

	      }
	    }//calculate noise map end
	  }
  
     
	} // END IF (m_doClusters)  

	
    }}//m_doRpcESD // AthenaMonManager::tier0 || AthenaMonManager::tier0ESD        
      
  rpc_eventstotal++;

  return sc; // statuscode check 
  
}


/*----------------------------------------------------------------------------------*/
StatusCode RPCStandaloneTracksMon::bookHistogramsRecurrent( )
/*----------------------------------------------------------------------------------*/
{

  ATH_MSG_DEBUG ( "RPCStandaloneTracksMon Histograms being booked" );
 
  StatusCode sc = StatusCode::SUCCESS; 
  if( m_doRpcESD==true ) {if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online ) {       
      //declare a group of histograms
      std::string m_generic_path_rpcmonitoring = "Muon/MuonRawDataMonitoring/RPCStandAloneTrackMon";
      MonGroup rpcprd_shift( this, m_generic_path_rpcmonitoring+"/Overview", run, ATTRIB_UNMANAGED  )      ;
      MonGroup rpcprd_expert( this, m_generic_path_rpcmonitoring+"/Overview", run, ATTRIB_UNMANAGED )     ;
      MonGroup rpcprd_expert_sum( this, m_generic_path_rpcmonitoring +"/Summary", run, ATTRIB_UNMANAGED ) ;
      MonGroup rpc_dqmf_global( this, m_generic_path_rpcmonitoring + "/GLOBAL", run, ATTRIB_UNMANAGED )    ;
      MonGroup rpcprd_dq_BA( this, m_generic_path_rpcmonitoring + "/RPCBA", run, ATTRIB_UNMANAGED  )       ;
      MonGroup rpcprd_dq_BC( this, m_generic_path_rpcmonitoring + "/RPCBC", run, ATTRIB_UNMANAGED )        ;
      MonGroup rpc_radiography(this, m_generic_path_rpcmonitoring +"/RPCRadiography",run, ATTRIB_UNMANAGED) ;
      MonGroup rpc_triggerefficiency(this, m_generic_path_rpcmonitoring +"/TriggerEfficiency",run, ATTRIB_UNMANAGED) ;

      if(newRunFlag())
	{      
	  ATH_MSG_INFO ( "RPCStandaloneTracksMon : begin of run" );
	
	  //book triggereff
	 
	 //PhiEtaCoin Thr_eff
	 hRPCPhiEtaCoinThr_eff.push_back( new TH1F("hRPCPhiEtaCoinThr_eff0","hRPCPhiEtaCoinThr_eff0" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCPhiEtaCoinThr_eff.back()) ; 
         hRPCPhiEtaCoinThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCPhiEtaCoinThr_eff.back()->GetYaxis()->SetTitle("RphiEtaCoin Thr_eff0");   
	 
	 hRPCPhiEtaCoinThr_eff.push_back( new TH1F("hRPCPhiEtaCoinThr_eff1","hRPCPhiEtaCoinThr_eff1" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCPhiEtaCoinThr_eff.back()) ; 
         hRPCPhiEtaCoinThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCPhiEtaCoinThr_eff.back()->GetYaxis()->SetTitle("RphiEtaCoin Thr_eff1");  
	 
	 hRPCPhiEtaCoinThr_eff.push_back( new TH1F("hRPCPhiEtaCoinThr_eff2","hRPCPhiEtaCoinThr_eff2" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCPhiEtaCoinThr_eff.back()) ; 
         hRPCPhiEtaCoinThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCPhiEtaCoinThr_eff.back()->GetYaxis()->SetTitle("RphiEtaCoin Thr_eff2");  
	 
	 hRPCPhiEtaCoinThr_eff.push_back( new TH1F("hRPCPhiEtaCoinThr_eff3","hRPCPhiEtaCoinThr_eff3" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCPhiEtaCoinThr_eff.back()) ; 
         hRPCPhiEtaCoinThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCPhiEtaCoinThr_eff.back()->GetYaxis()->SetTitle("RphiEtaCoin Thr_eff3");  
	 
	 hRPCPhiEtaCoinThr_eff.push_back( new TH1F("hRPCPhiEtaCoinThr_eff4","hRPCPhiEtaCoinThr_eff4" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCPhiEtaCoinThr_eff.back()) ; 
         hRPCPhiEtaCoinThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCPhiEtaCoinThr_eff.back()->GetYaxis()->SetTitle("RphiEtaCoin Thr_eff4"); 
	  
	 hRPCPhiEtaCoinThr_eff.push_back( new TH1F("hRPCPhiEtaCoinThr_eff5","hRPCPhiEtaCoinThr_eff5" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCPhiEtaCoinThr_eff.back()) ; 
         hRPCPhiEtaCoinThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCPhiEtaCoinThr_eff.back()->GetYaxis()->SetTitle("RphiEtaCoin Thr_eff5"); 
	 
	 //Pad Thr_eff
	 hRPCPadThr_eff.push_back( new TH1F("hRPCPadThr_eff0","hRPCPadThr_eff0" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCPadThr_eff.back()) ; 
         hRPCPadThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCPadThr_eff.back()->GetYaxis()->SetTitle("RPad Thr_eff0");   
	 
	 hRPCPadThr_eff.push_back( new TH1F("hRPCPadThr_eff1","hRPCPadThr_eff1" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCPadThr_eff.back()) ; 
         hRPCPadThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCPadThr_eff.back()->GetYaxis()->SetTitle("RPad Thr_eff1");  
	 
	 hRPCPadThr_eff.push_back( new TH1F("hRPCPadThr_eff2","hRPCPadThr_eff2" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCPadThr_eff.back()) ; 
         hRPCPadThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCPadThr_eff.back()->GetYaxis()->SetTitle("RPad Thr_eff2");  
	 
	 hRPCPadThr_eff.push_back( new TH1F("hRPCPadThr_eff3","hRPCPadThr_eff3" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCPadThr_eff.back()) ; 
         hRPCPadThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCPadThr_eff.back()->GetYaxis()->SetTitle("RPad Thr_eff3");  
	 
	 hRPCPadThr_eff.push_back( new TH1F("hRPCPadThr_eff4","hRPCPadThr_eff4" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCPadThr_eff.back()) ; 
         hRPCPadThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCPadThr_eff.back()->GetYaxis()->SetTitle("RPad Thr_eff4"); 
	  
	 hRPCPadThr_eff.push_back( new TH1F("hRPCPadThr_eff5","hRPCPadThr_eff5" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCPadThr_eff.back()) ; 
         hRPCPadThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCPadThr_eff.back()->GetYaxis()->SetTitle("RPad Thr_eff5");
	 
	 //Muctpi Thr_eff
	 hRPCMuctpiThr_eff.push_back( new TH1F("hRPCMuctpiThr_eff0","hRPCMuctpiThr_eff0" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCMuctpiThr_eff.back()) ; 
         hRPCMuctpiThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCMuctpiThr_eff.back()->GetYaxis()->SetTitle("RMuctpi Thr_eff0");   
	 
	 hRPCMuctpiThr_eff.push_back( new TH1F("hRPCMuctpiThr_eff1","hRPCMuctpiThr_eff1" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCMuctpiThr_eff.back()) ; 
         hRPCMuctpiThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCMuctpiThr_eff.back()->GetYaxis()->SetTitle("RMuctpi Thr_eff1");  
	 
	 hRPCMuctpiThr_eff.push_back( new TH1F("hRPCMuctpiThr_eff2","hRPCMuctpiThr_eff2" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCMuctpiThr_eff.back()) ; 
         hRPCMuctpiThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCMuctpiThr_eff.back()->GetYaxis()->SetTitle("RMuctpi Thr_eff2");  
	 
	 hRPCMuctpiThr_eff.push_back( new TH1F("hRPCMuctpiThr_eff3","hRPCMuctpiThr_eff3" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCMuctpiThr_eff.back()) ; 
         hRPCMuctpiThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCMuctpiThr_eff.back()->GetYaxis()->SetTitle("RMuctpi Thr_eff3");  
	 
	 hRPCMuctpiThr_eff.push_back( new TH1F("hRPCMuctpiThr_eff4","hRPCMuctpiThr_eff4" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCMuctpiThr_eff.back()) ; 
         hRPCMuctpiThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCMuctpiThr_eff.back()->GetYaxis()->SetTitle("RMuctpi Thr_eff4"); 
	  
	 hRPCMuctpiThr_eff.push_back( new TH1F("hRPCMuctpiThr_eff5","hRPCMuctpiThr_eff5" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCMuctpiThr_eff.back()) ; 
         hRPCMuctpiThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCMuctpiThr_eff.back()->GetYaxis()->SetTitle("RMuctpi Thr_eff5");

	 
	 //PhiEtaCoin thr
	 hRPCPhiEtaCoinThr.push_back( new TH1F("hRPCPhiEtaCoinThr0","hRPCPhiEtaCoinThr0" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCPhiEtaCoinThr.back()) ; 
         hRPCPhiEtaCoinThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCPhiEtaCoinThr.back()->GetYaxis()->SetTitle("RphiEtaCoin Thr0");   
	 
	 hRPCPhiEtaCoinThr.push_back( new TH1F("hRPCPhiEtaCoinThr1","hRPCPhiEtaCoinThr1" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCPhiEtaCoinThr.back()) ; 
         hRPCPhiEtaCoinThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCPhiEtaCoinThr.back()->GetYaxis()->SetTitle("RphiEtaCoin Thr1");  
	 
	 hRPCPhiEtaCoinThr.push_back( new TH1F("hRPCPhiEtaCoinThr2","hRPCPhiEtaCoinThr2" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCPhiEtaCoinThr.back()) ; 
         hRPCPhiEtaCoinThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCPhiEtaCoinThr.back()->GetYaxis()->SetTitle("RphiEtaCoin Thr2");  
	 
	 hRPCPhiEtaCoinThr.push_back( new TH1F("hRPCPhiEtaCoinThr3","hRPCPhiEtaCoinThr3" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCPhiEtaCoinThr.back()) ; 
         hRPCPhiEtaCoinThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCPhiEtaCoinThr.back()->GetYaxis()->SetTitle("RphiEtaCoin Thr3");  
	 
	 hRPCPhiEtaCoinThr.push_back( new TH1F("hRPCPhiEtaCoinThr4","hRPCPhiEtaCoinThr4" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCPhiEtaCoinThr.back()) ; 
         hRPCPhiEtaCoinThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCPhiEtaCoinThr.back()->GetYaxis()->SetTitle("RphiEtaCoin Thr4"); 
	  
	 hRPCPhiEtaCoinThr.push_back( new TH1F("hRPCPhiEtaCoinThr5","hRPCPhiEtaCoinThr5" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCPhiEtaCoinThr.back()) ; 
         hRPCPhiEtaCoinThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCPhiEtaCoinThr.back()->GetYaxis()->SetTitle("RphiEtaCoin Thr5"); 
	 
	 //Pad thr
	 hRPCPadThr.push_back( new TH1F("hRPCPadThr0","hRPCPadThr0" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCPadThr.back()) ; 
         hRPCPadThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCPadThr.back()->GetYaxis()->SetTitle("RPad Thr0");   
	 
	 hRPCPadThr.push_back( new TH1F("hRPCPadThr1","hRPCPadThr1" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCPadThr.back()) ; 
         hRPCPadThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCPadThr.back()->GetYaxis()->SetTitle("RPad Thr1");  
	 
	 hRPCPadThr.push_back( new TH1F("hRPCPadThr2","hRPCPadThr2" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCPadThr.back()) ; 
         hRPCPadThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCPadThr.back()->GetYaxis()->SetTitle("RPad Thr2");  
	 
	 hRPCPadThr.push_back( new TH1F("hRPCPadThr3","hRPCPadThr3" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCPadThr.back()) ; 
         hRPCPadThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCPadThr.back()->GetYaxis()->SetTitle("RPad Thr3");  
	 
	 hRPCPadThr.push_back( new TH1F("hRPCPadThr4","hRPCPadThr4" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCPadThr.back()) ; 
         hRPCPadThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCPadThr.back()->GetYaxis()->SetTitle("RPad Thr4"); 
	  
	 hRPCPadThr.push_back( new TH1F("hRPCPadThr5","hRPCPadThr5" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCPadThr.back()) ; 
         hRPCPadThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCPadThr.back()->GetYaxis()->SetTitle("RPad Thr5");
	 
	 //Muctpi thr
	 hRPCMuctpiThr.push_back( new TH1F("hRPCMuctpiThr0","hRPCMuctpiThr0" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCMuctpiThr.back()) ; 
         hRPCMuctpiThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCMuctpiThr.back()->GetYaxis()->SetTitle("RMuctpi Thr0");   
	 
	 hRPCMuctpiThr.push_back( new TH1F("hRPCMuctpiThr1","hRPCMuctpiThr1" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCMuctpiThr.back()) ; 
         hRPCMuctpiThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCMuctpiThr.back()->GetYaxis()->SetTitle("RMuctpi Thr1");  
	 
	 hRPCMuctpiThr.push_back( new TH1F("hRPCMuctpiThr2","hRPCMuctpiThr2" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCMuctpiThr.back()) ; 
         hRPCMuctpiThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCMuctpiThr.back()->GetYaxis()->SetTitle("RMuctpi Thr2");  
	 
	 hRPCMuctpiThr.push_back( new TH1F("hRPCMuctpiThr3","hRPCMuctpiThr3" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCMuctpiThr.back()) ; 
         hRPCMuctpiThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCMuctpiThr.back()->GetYaxis()->SetTitle("RMuctpi Thr3");  
	 
	 hRPCMuctpiThr.push_back( new TH1F("hRPCMuctpiThr4","hRPCMuctpiThr4" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCMuctpiThr.back()) ; 
         hRPCMuctpiThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCMuctpiThr.back()->GetYaxis()->SetTitle("RMuctpi Thr4"); 
	  
	 hRPCMuctpiThr.push_back( new TH1F("hRPCMuctpiThr5","hRPCMuctpiThr5" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(hRPCMuctpiThr.back()) ; 
         hRPCMuctpiThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         hRPCMuctpiThr.back()->GetYaxis()->SetTitle("RMuctpi Thr5");
	  
	  
	  TH1 *hMEtracks=new TH1F("hMEtracks","hMEtracks",400,0,100);	  
	  sc = rpc_triggerefficiency.regHist(hMEtracks) ;  
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "hMEtracks Failed to register histogram " );	    
	      return sc;
	    }
	  else {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : hMEtracks");
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );		 
	    ATH_MSG_DEBUG ( "Booked book hMEtracks successfully" );} 
	  hMEtracks->SetFillColor(42);
	  hMEtracks->GetYaxis()->SetTitle("# METracks/0.25GeV");
	  hMEtracks->GetXaxis()->SetTitle("Pt[GeV]"); 
	
	  ////////////////////////book track
	  //track on surface histo for rpc cluster
	
	  std::string m_generic_path_rpczxSurfaceView = m_generic_path_rpcmonitoring+"/Overview";
	  std:: string m_rpczxSurfaceView_title = "rpczxSurfaceView";
	  const char* m_rpczxSurfaceView_title_char = m_rpczxSurfaceView_title.c_str();  
	  	  
	  TH2 *rpczxSurfaceView=new TH2F(m_rpczxSurfaceView_title_char,m_rpczxSurfaceView_title_char,50,-50000,50000,50,-50000,50000);	  
	  sc = rpc_dqmf_global.regHist(rpczxSurfaceView) ;  
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpczxSurfaceView Failed to register histogram " );	    
	      return sc;
	    }
	  rpczxSurfaceView->SetFillColor(42);
	  rpczxSurfaceView->GetYaxis()->SetTitle("rpc track extrapolation x [mm]");
	  rpczxSurfaceView->GetXaxis()->SetTitle("rpc track extrapolation z [mm]"); 
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpczxSurfaceView << m_generic_path_rpczxSurfaceView.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );		 
	    ATH_MSG_DEBUG ( "Booked bookrpczxSurfaceView successfully" );}   
	  	  
	  
	  std::string m_generic_path_rpczSurfaceView = m_generic_path_rpcmonitoring+"/Overview";
	  std:: string m_rpczSurfaceView_title = "rpczSurfaceView";
	  const char* m_rpczSurfaceView_title_char = m_rpczSurfaceView_title.c_str();  

	  TH1 *rpczSurfaceView=new TH1F(m_rpczSurfaceView_title_char,m_rpczSurfaceView_title_char,50,-50000,50000);	  
	  sc=rpcprd_shift.regHist(rpczSurfaceView) ;  
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpczSurfaceView Failed to register histogram " );	    
	      return sc;
	    }
	  rpczSurfaceView->SetFillColor(42);
	  rpczSurfaceView->GetYaxis()->SetTitle("Counts");
	  rpczSurfaceView->GetXaxis()->SetTitle("rpc track extrapolation z [mm]"); 
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpczSurfaceView << m_generic_path_rpczSurfaceView.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );		 
	    ATH_MSG_DEBUG ( "Booked bookrpczSurfaceView successfully" );}   
	  	  
	  std::string m_generic_path_rpcxSurfaceView = m_generic_path_rpcmonitoring+"/Overview";
	  std:: string m_rpcxSurfaceView_title = "rpcxSurfaceView";
	  const char* m_rpcxSurfaceView_title_char = m_rpcxSurfaceView_title.c_str();  
	  
	  TH1 *rpcxSurfaceView=new TH1F(m_rpcxSurfaceView_title_char,m_rpcxSurfaceView_title_char,50,-50000,50000);	  
	  sc= rpcprd_shift.regHist(rpcxSurfaceView) ;  
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpcxSurfaceView Failed to register histogram " );	    
	      return sc;
	    }
	  rpcxSurfaceView->SetFillColor(42);
	  rpcxSurfaceView->GetYaxis()->SetTitle("counts");
	  rpcxSurfaceView->GetXaxis()->SetTitle("rpc track extrapolation x [mm]"); 
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpcxSurfaceView << m_generic_path_rpcxSurfaceView.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );		 
	    ATH_MSG_DEBUG ( "Booked bookrpcxSurfaceView successfully" );}   

    	    

	  std::string m_generic_path_f_rpczxSurfaceView = m_generic_path_rpcmonitoring+"/Overview";
	  std:: string m_f_rpczxSurfaceView_title = "f_rpczxSurfaceView";
	  const char* m_f_rpczxSurfaceView_title_char = m_f_rpczxSurfaceView_title.c_str();  

	  TH2 *f_rpczxSurfaceView=new TH2F(m_f_rpczxSurfaceView_title_char,m_f_rpczxSurfaceView_title_char,50,-50000,50000,50,-50000,50000);	  
	  sc=rpcprd_shift.regHist(f_rpczxSurfaceView) ;  
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "f_rpczxSurfaceView Failed to register histogram " );	    
	      return sc;
	    }
	  f_rpczxSurfaceView->SetFillColor(42);
	  f_rpczxSurfaceView->GetYaxis()->SetTitle("rpc track extrapolation x [mm]");
	  f_rpczxSurfaceView->GetXaxis()->SetTitle("rpc track extrapolation z [mm]"); 
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << f_rpczxSurfaceView << m_generic_path_f_rpczxSurfaceView.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookf_rpczxSurfaceView successfully" );}   
	
	
	  std::string m_generic_path_rpctrack_phivseta = m_generic_path_rpcmonitoring+"/Overview";
	  std:: string m_rpctrack_phivseta_title = "rpctrack_phivseta";
	  const char* m_rpctrack_phivseta_title_char = m_rpctrack_phivseta_title.c_str();
	
	  TH2 *rpctrack_phivseta=new TH2F(m_rpctrack_phivseta_title_char,m_rpctrack_phivseta_title_char,100,-1.5,1.5, 100, 0, 180); // 0-360
	  sc=rpcprd_shift.regHist(rpctrack_phivseta) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpctrack_phivseta Failed to register histogram " );
	      return sc;
	    }
	  rpctrack_phivseta->SetFillColor(42);
	  rpctrack_phivseta->GetXaxis()->SetTitle("pseudo-rapidity");
	  rpctrack_phivseta->GetYaxis()->SetTitle("phi angle [deg]");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpctrack_phivseta << m_generic_path_rpctrack_phivseta.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpctrack_phivseta successfully" );}
	

	  std::string m_generic_path_rpctrack_bvseta = m_generic_path_rpcmonitoring+"/Overview";
	  std:: string m_rpctrack_bvseta_title = "rpctrack_bvseta";
	  const char* m_rpctrack_bvseta_title_char = m_rpctrack_bvseta_title.c_str();

	  TH2 *rpctrack_bvseta=new TH2F(m_rpctrack_bvseta_title_char,m_rpctrack_bvseta_title_char, 100, -1.5, 1.5, 1000, 0, 50000);
	  sc=rpcprd_shift.regHist(rpctrack_bvseta) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpctrack_bvseta Failed to register histogram " );
	      return sc;
	    }
	  rpctrack_bvseta->SetFillColor(42);
	  rpctrack_bvseta->GetXaxis()->SetTitle("rpc track pseudo-rapidity");
	  rpctrack_bvseta->GetYaxis()->SetTitle("rpc track distance from IP [mm]");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpctrack_bvseta << m_generic_path_rpctrack_bvseta.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpctrack_bvseta successfully" );}



	  std::string m_generic_path_rpctrack_bVTXy0 = m_generic_path_rpcmonitoring+"/Overview";
	  std:: string m_rpctrack_bVTXy0_title = "rpctrack_bVTXy0";
	  const char* m_rpctrack_bVTXy0_title_char = m_rpctrack_bVTXy0_title.c_str();

	  TH2 *rpctrack_bVTXy0=new TH2F(m_rpctrack_bVTXy0_title_char,m_rpctrack_bVTXy0_title_char, 100, -5000, 5000, 100, -5000, 5000);
	  sc=rpcprd_shift.regHist(rpctrack_bVTXy0) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpctrack_bVTXy0 Failed to register histogram " );
	      return sc;
	    }
	  rpctrack_bVTXy0->SetFillColor(42);
	  rpctrack_bVTXy0->GetYaxis()->SetTitle("rpc track projection y=0, x [mm]");
	  rpctrack_bVTXy0->GetXaxis()->SetTitle("rpc track projection y=0, z [mm] ");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpctrack_bVTXy0 << m_generic_path_rpctrack_bVTXy0.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpctrack_bVTXy0 successfully" );}
     


	  std::string m_generic_path_rpctrack_bVTXz0 = m_generic_path_rpcmonitoring+"/Overview";
	  std:: string m_rpctrack_bVTXz0_title = "rpctrack_bVTXz0";
	  const char* m_rpctrack_bVTXz0_title_char = m_rpctrack_bVTXz0_title.c_str();

	  TH2 *rpctrack_bVTXz0=new TH2F(m_rpctrack_bVTXz0_title_char,m_rpctrack_bVTXz0_title_char, 100, -5000, 5000, 100, -5000, 5000);
	  sc=rpcprd_shift.regHist(rpctrack_bVTXz0) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpctrack_bVTXz0 Failed to register histogram " );
	      return sc;
	    }
	  rpctrack_bVTXz0->SetFillColor(42);
	  rpctrack_bVTXz0->GetXaxis()->SetTitle("rpc track projection z=0, x [mm]");
	  rpctrack_bVTXz0->GetYaxis()->SetTitle("rpc track projection z=0, y [mm] ");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpctrack_bVTXz0 << m_generic_path_rpctrack_bVTXz0.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpctrack_bVTXz0 successfully" );}
     
     
	  std::string m_generic_path_rpctrack_bVTXx0 = m_generic_path_rpcmonitoring+"/Overview";
	  std:: string m_rpctrack_bVTXx0_title = "rpctrack_bVTXx0";
	  const char* m_rpctrack_bVTXx0_title_char = m_rpctrack_bVTXx0_title.c_str();

	  TH2 *rpctrack_bVTXx0=new TH2F(m_rpctrack_bVTXx0_title_char,m_rpctrack_bVTXx0_title_char, 100, -5000, 5000, 100, -5000, 5000);
	  sc=rpcprd_shift.regHist(rpctrack_bVTXx0) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpctrack_bVTXx0 Failed to register histogram " );
	      return sc;
	    }
	  rpctrack_bVTXx0->SetFillColor(42);
	  rpctrack_bVTXx0->GetXaxis()->SetTitle("rpc track projection x=0, z [mm]");
	  rpctrack_bVTXx0->GetYaxis()->SetTitle("rpc track projection x=0, y [mm] ");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpctrack_bVTXx0 << m_generic_path_rpctrack_bVTXx0.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpctrack_bVTXx0 successfully" );}
     
	  std::string m_generic_path_rpctrack_phi = m_generic_path_rpcmonitoring+"/Overview";
	  std:: string m_rpctrack_phi_title = "rpctrack_phi";
	  const char* m_rpctrack_phi_title_char = m_rpctrack_phi_title.c_str();
     
	  TH1 *rpctrack_phi=new TH1F(m_rpctrack_phi_title_char,m_rpctrack_phi_title_char,100, 0, 360);
	  sc=rpcprd_shift.regHist(rpctrack_phi) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpctrack_phi Failed to register histogram " );
	      return sc;
	    }
	  rpctrack_phi->SetFillColor(42);
	  rpctrack_phi->GetYaxis()->SetTitle("Counts");
	  rpctrack_phi->GetXaxis()->SetTitle("phi angle [deg]");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpctrack_phi << m_generic_path_rpctrack_phi.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpctrack_phi successfully" );}
     
     
	  std::string m_generic_path_rpctrack_eta = m_generic_path_rpcmonitoring+"/Overview";
	  std:: string m_rpctrack_eta_title = "rpctrack_eta";
	  const char* m_rpctrack_eta_title_char = m_rpctrack_eta_title.c_str();
 
        
	  TH1 *rpctrack_eta=new TH1F(m_rpctrack_eta_title_char,m_rpctrack_eta_title_char,100,-1.5,1.5);
	  sc=rpcprd_shift.regHist(rpctrack_eta) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpctrack_eta Failed to register histogram " );
	      return sc;
	    }
	  rpctrack_eta->SetFillColor(42);
	  rpctrack_eta->GetXaxis()->SetTitle("pseudo-rapidity");
	  rpctrack_eta->GetYaxis()->SetTitle("Counts");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpctrack_eta << m_generic_path_rpctrack_eta.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpctrack_eta successfully" );}
	 
     
	  std::string m_generic_path_rpctrack_b = m_generic_path_rpcmonitoring+"/Overview";
	  std:: string m_rpctrack_b_title = "rpctrack_b";
	  const char* m_rpctrack_b_title_char = m_rpctrack_b_title.c_str();
     
	  TH1 *rpctrack_b=new TH1F(m_rpctrack_b_title_char,m_rpctrack_b_title_char,1000,0,50000);
	  sc=rpcprd_shift.regHist(rpctrack_b) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpctrack_b Failed to register histogram " );
	      return sc;
	    }
	  rpctrack_b->SetFillColor(42);
	  rpctrack_b->GetXaxis()->SetTitle("rpc track distance from IP [mm]");
	  rpctrack_b->GetXaxis()->SetTitle("Counts");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpctrack_b << m_generic_path_rpctrack_b.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpctrack_b successfully" );}
     
 
 
     
	  std::string m_generic_path_rpcmergepointdistance = m_generic_path_rpcmonitoring+"/Overview";
	  std:: string m_rpcmergepointdistance_title = "rpcmergepointdistance";
	  const char* m_rpcmergepointdistance_title_char = m_rpcmergepointdistance_title.c_str();
     
	  TH1 *rpcmergepointdistance=new TH1F(m_rpcmergepointdistance_title_char,m_rpcmergepointdistance_title_char,1000,0,1000);
	  sc=rpcprd_shift.regHist(rpcmergepointdistance) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpcmergepointdistance Failed to register histogram " );
	      return sc;
	    }
	  rpcmergepointdistance->SetFillColor(42);
	  rpcmergepointdistance->GetYaxis()->SetTitle("[counts]");
	  rpcmergepointdistance->GetXaxis()->SetTitle("merge point distance [mm]");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpcmergepointdistance << m_generic_path_rpcmergepointdistance.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpcmergepointdistance successfully" );}
     
     
     
 
	
	  std::string m_generic_path_f_rpcmergepointdistance = m_generic_path_rpcmonitoring+"/Overview";
	  std:: string m_f_rpcmergepointdistance_title = "f_rpcmergepointdistance";
	  const char* m_f_rpcmergepointdistance_title_char = m_f_rpcmergepointdistance_title.c_str();
	
	  TH1 *f_rpcmergepointdistance=new TH1F(m_f_rpcmergepointdistance_title_char,m_f_rpcmergepointdistance_title_char,1000,-0,1000);
	  sc=rpcprd_shift.regHist(f_rpcmergepointdistance) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "f_rpcmergepointdistance Failed to register histogram " );
	      return sc;
	    }
	  f_rpcmergepointdistance->SetFillColor(42);
	  f_rpcmergepointdistance->GetYaxis()->SetTitle("[counts]");
	  f_rpcmergepointdistance->GetXaxis()->SetTitle("merge point distance [mm]");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << f_rpcmergepointdistance << m_generic_path_f_rpcmergepointdistance.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookf_rpcmergepointdistance successfully" );}
	
    
 
 
     
	  std::string m_generic_path_rpcchi2dof = m_generic_path_rpcmonitoring+"/Overview";
	  std:: string m_rpcchi2dof_title = "rpcchi2dof";
	  const char* m_rpcchi2dof_title_char = m_rpcchi2dof_title.c_str();
     
	  TH1 *rpcchi2dof=new TH1F(m_rpcchi2dof_title_char,m_rpcchi2dof_title_char,200,0,20);
	  sc=rpcprd_shift.regHist(rpcchi2dof) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpcchi2dof Failed to register histogram " );
	      return sc;
	    }
	  rpcchi2dof->SetFillColor(42);
	  rpcchi2dof->GetYaxis()->SetTitle("[counts]");
	  rpcchi2dof->GetXaxis()->SetTitle("chi2/dof");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpcchi2dof << m_generic_path_rpcchi2dof.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpcchi2dof successfully" );}
     
     
     
 
	
	  std::string m_generic_path_f_rpcchi2dof = m_generic_path_rpcmonitoring+"/Overview";
	  std:: string m_f_rpcchi2dof_title = "f_rpcchi2dof";
	  const char* m_f_rpcchi2dof_title_char = m_f_rpcchi2dof_title.c_str();
	
	  TH1 *f_rpcchi2dof=new TH1F(m_f_rpcchi2dof_title_char,m_f_rpcchi2dof_title_char,200,0,20);
	  sc=rpcprd_shift.regHist(f_rpcchi2dof) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "f_rpcchi2dof Failed to register histogram " );
	      return sc;
	    }
	  f_rpcchi2dof->SetFillColor(42);
	  f_rpcchi2dof->GetYaxis()->SetTitle("[counts]");
	  f_rpcchi2dof->GetXaxis()->SetTitle("chi2/dof");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << f_rpcchi2dof << m_generic_path_f_rpcchi2dof.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookf_rpcchi2dof successfully" );}
        
	  std::string m_generic_path_rpcetavsphichi2dof = m_generic_path_rpcmonitoring+"/Overview";
	  std:: string m_rpcetavsphichi2dof_title = "rpcetavsphichi2dof";
	  const char* m_rpcetavsphichi2dof_title_char = m_rpcetavsphichi2dof_title.c_str();

	  TH1 *rpcetavsphichi2dof=new TH2F(m_rpcetavsphichi2dof_title_char,m_rpcetavsphichi2dof_title_char,200,0,20,200,0,20);
	  sc=rpcprd_shift.regHist(rpcetavsphichi2dof) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpcetavsphichi2dof Failed to register histogram " );
	      return sc;
	    }
	  rpcetavsphichi2dof->SetFillColor(42);
	  rpcetavsphichi2dof->GetYaxis()->SetTitle("etachi2/dof");
	  rpcetavsphichi2dof->GetXaxis()->SetTitle("phichi2/dof");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpcetavsphichi2dof << m_generic_path_rpcetavsphichi2dof.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpcetavsphichi2dof successfully" );}
	
	  
	  std::string m_generic_path_rpcNtracks = m_generic_path_rpcmonitoring+"/Overview";
	  std:: string m_rpcNtracks_title = "rpcNtracks";
	  const char* m_rpcNtracks_title_char = m_rpcNtracks_title.c_str();
	  
	  TH1 *rpcNtracks=new TH1F(m_rpcNtracks_title_char,m_rpcNtracks_title_char,10,0,10);
	  sc=rpc_dqmf_global.regHist(rpcNtracks) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpcNtracks Failed to register histogram " );
	      return sc;
	    }
	  rpcNtracks->SetFillColor(42);
	  rpcNtracks->GetYaxis()->SetTitle("[counts]");
	  rpcNtracks->GetXaxis()->SetTitle("Number of rpc tracks per event");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpcNtracks << m_generic_path_rpcNtracks.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpcNtracks successfully" );}
 
	  	  
	  std::string m_generic_path_f_rpcNtracks = m_generic_path_rpcmonitoring+"/Overview";
	  std:: string m_f_rpcNtracks_title = "f_rpcNtracks";
	  const char* m_f_rpcNtracks_title_char = m_f_rpcNtracks_title.c_str();
	          
	  TH1 *f_rpcNtracks=new TH1F(m_f_rpcNtracks_title_char,m_f_rpcNtracks_title_char,10,0,10);
	  sc=rpcprd_shift.regHist(f_rpcNtracks) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "f_rpcNtracks Failed to register histogram " );
	      return sc;
	    }
	  f_rpcNtracks->SetFillColor(42);
	  f_rpcNtracks->GetYaxis()->SetTitle("[counts]");
	  f_rpcNtracks->GetXaxis()->SetTitle("Number of rpc tracks per event");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << f_rpcNtracks << m_generic_path_f_rpcNtracks.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookf_rpcNtracks successfully" );}

 
	  std::string m_generic_path_rpcPointPerTracks = m_generic_path_rpcmonitoring+"/Overview";
	  std:: string m_rpcPointPerTracks_title = "rpcPointPerTracks";
	  const char* m_rpcPointPerTracks_title_char = m_rpcPointPerTracks_title.c_str();

	  TH1 *rpcPointPerTracks=new TH1F(m_rpcPointPerTracks_title_char,m_rpcPointPerTracks_title_char,10,0,10);
	  sc=rpcprd_shift.regHist(rpcPointPerTracks) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpcPointPerTracks Failed to register histogram " );
	      return sc;
	    }
	  rpcPointPerTracks->SetFillColor(42);
	  rpcPointPerTracks->GetYaxis()->SetTitle("[counts]");
	  rpcPointPerTracks->GetXaxis()->SetTitle("Number of 3D point per tracks");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpcPointPerTracks << m_generic_path_rpcPointPerTracks.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpcPointPerTracks successfully" );}
	  
	  
	  std::string m_generic_path_f_rpcPointPerTracks = m_generic_path_rpcmonitoring+"/Overview";
	  std:: string m_f_rpcPointPerTracks_title = "f_rpcPointPerTracks";
	  const char* m_f_rpcPointPerTracks_title_char = m_f_rpcPointPerTracks_title.c_str();
	
	  TH1 *f_rpcPointPerTracks=new TH1F(m_f_rpcPointPerTracks_title_char,m_f_rpcPointPerTracks_title_char,10,0,10);
	  sc=rpcprd_shift.regHist(f_rpcPointPerTracks) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "f_rpcPointPerTracks Failed to register histogram " );
	      return sc;
	    }
	  f_rpcPointPerTracks->SetFillColor(42);
	  f_rpcPointPerTracks->GetYaxis()->SetTitle("[counts]");
	  f_rpcPointPerTracks->GetXaxis()->SetTitle("Number of 3D point per tracks");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << f_rpcPointPerTracks << m_generic_path_f_rpcPointPerTracks.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookf_rpcPointPerTracks successfully" );}
	  
	
	  
	  std::string m_generic_path_rpcTrackType = m_generic_path_rpcmonitoring+"/Overview";
	  std:: string m_rpcTrackType_title = "rpcTrackType";
	  const char* m_rpcTrackType_title_char = m_rpcTrackType_title.c_str();
	
	  TH1 *rpcTrackType=new TH1F(m_rpcTrackType_title_char,m_rpcTrackType_title_char,64,0,64);
	  sc=rpcprd_shift.regHist(rpcTrackType) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpcTrackType Failed to register histogram " );
	      return sc;
	    }
	  rpcTrackType->SetFillColor(42);
	  rpcTrackType->GetYaxis()->SetTitle("[counts]");
	  rpcTrackType->GetXaxis()->SetLabelSize(0.01);
	  rpcTrackType->GetXaxis()->SetTitle("BMS_LPt*2E0+BMS_Piv*2E1+BOS_HPt*2E2+BML_LPt*2E3+BML_Piv*2E4+BOL_HPt*2E5");
	  rpcTrackType->GetXaxis()->SetTitleSize(0.03) ;
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpcTrackType << m_generic_path_rpcTrackType.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpcTrackType successfully" );}
	  
	  std::string m_generic_path_f_rpcTrackType = m_generic_path_rpcmonitoring+"/Overview";
	  std:: string m_f_rpcTrackType_title = "f_rpcTrackType";
	  const char* m_f_rpcTrackType_title_char = m_f_rpcTrackType_title.c_str();
	
	  TH1 *f_rpcTrackType=new TH1F(m_f_rpcTrackType_title_char,m_f_rpcTrackType_title_char,64,0,64);
	  sc=rpcprd_shift.regHist(f_rpcTrackType) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "f_rpcTrackType Failed to register histogram " );
	      return sc;
	    }
	  f_rpcTrackType->SetFillColor(42);
	  f_rpcTrackType->GetYaxis()->SetTitle("[counts]");  
	  f_rpcTrackType->GetXaxis()->SetLabelSize(0.03);
	  
	  f_rpcTrackType->GetXaxis()->SetTitle("BMS_LPt*2E0+BMS_Piv*2E1+BOS_HPt*2E2+BML_LPt*2E3+BML_Piv*2E4+BOL_HPt*2E5");
	  f_rpcTrackType->GetXaxis()->SetTitleSize(0.03) ;
	  
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << f_rpcTrackType << m_generic_path_f_rpcTrackType.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookf_rpcTrackType successfully" );}
	
	  
	
	  
	  std::string m_generic_path_rpcPhiResidual = m_generic_path_rpcmonitoring+"/Overview";
	  std:: string m_rpcPhiResidual_title = "rpcPhiResidual";
	  const char* m_rpcPhiResidual_title_char = m_rpcPhiResidual_title.c_str();
	
	  TH1 *rpcPhiResidual=new TH1F(m_rpcPhiResidual_title_char,m_rpcPhiResidual_title_char,1000,-500,500);
	  sc=rpcprd_shift.regHist(rpcPhiResidual) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpcPhiResidual Failed to register histogram " );
	      return sc;
	    }
	  rpcPhiResidual->SetFillColor(42);
	  rpcPhiResidual->GetYaxis()->SetTitle("[counts]");
	  rpcPhiResidual->GetXaxis()->SetTitle("Phi residual [mm]");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpcPhiResidual << m_generic_path_rpcPhiResidual.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpcPhiResidual successfully" );}
	  
	  std::string m_generic_path_f_rpcPhiResidual = m_generic_path_rpcmonitoring+"/Overview";
	  std:: string m_f_rpcPhiResidual_title = "f_rpcPhiResidual";
	  const char* m_f_rpcPhiResidual_title_char = m_f_rpcPhiResidual_title.c_str();
	
	  TH1 *f_rpcPhiResidual=new TH1F(m_f_rpcPhiResidual_title_char,m_f_rpcPhiResidual_title_char,1000,-500,500);
	  sc=rpcprd_shift.regHist(f_rpcPhiResidual) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "f_rpcPhiResidual Failed to register histogram " );
	      return sc;
	    }
	  f_rpcPhiResidual->SetFillColor(42);
	  f_rpcPhiResidual->GetYaxis()->SetTitle("[counts]"); 

	  f_rpcPhiResidual->GetXaxis()->SetTitle("Phi spatial resolution");
 	  
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << f_rpcPhiResidual << m_generic_path_f_rpcPhiResidual.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookf_rpcPhiResidual successfully" );}
	

	  std::string m_generic_path_rpcEtaResidual = m_generic_path_rpcmonitoring+"/Overview";
	  std:: string m_rpcEtaResidual_title = "rpcEtaResidual";
	  const char* m_rpcEtaResidual_title_char = m_rpcEtaResidual_title.c_str();
	  
	  TH1 *rpcEtaResidual=new TH1F(m_rpcEtaResidual_title_char,m_rpcEtaResidual_title_char,1000,-500,500);
	  sc=rpcprd_shift.regHist(rpcEtaResidual) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpcEtaResidual Failed to register histogram " );
	      return sc;
	    }
	  rpcEtaResidual->SetFillColor(42);
	  rpcEtaResidual->GetYaxis()->SetTitle("[counts]");
	  rpcEtaResidual->GetXaxis()->SetTitle("Eta Residual [mm]");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpcEtaResidual << m_generic_path_rpcEtaResidual.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpcEtaResidual successfully" );}
	
	  std::string m_generic_path_f_rpcEtaResidual = m_generic_path_rpcmonitoring+"/Overview";
	  std:: string m_f_rpcEtaResidual_title = "f_rpcEtaResidual";
	  const char* m_f_rpcEtaResidual_title_char = m_f_rpcEtaResidual_title.c_str();
	  
	  TH1 *f_rpcEtaResidual=new TH1F(m_f_rpcEtaResidual_title_char,m_f_rpcEtaResidual_title_char,1000,-500,500);
	  sc=rpcprd_shift.regHist(f_rpcEtaResidual) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "f_rpcEtaResidual Failed to register histogram " );
	      return sc;
	    }
	  f_rpcEtaResidual->SetFillColor(42);
	  f_rpcEtaResidual->GetYaxis()->SetTitle("[counts]"); 
	  f_rpcEtaResidual->GetXaxis()->SetTitle("Eta spatial resolution [mm]");
	
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << f_rpcEtaResidual << m_generic_path_f_rpcEtaResidual.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookf_rpcEtaResidual successfully" );}

	 	  
	  std::string m_generic_path_LayerTrackProjection = m_generic_path_rpcmonitoring+"/Overview";
	  std::string m_LayerTrackProjection_title = "Track_Projected_on_Layer";
	  const char* m_LayerTrackProjection_title_char = m_LayerTrackProjection_title.c_str();  
	
	  LayerTrackProjection=new TH1I(m_LayerTrackProjection_title_char,m_LayerTrackProjection_title_char,24,0,24);	    
	  sc = rpc_dqmf_global.regHist(LayerTrackProjection) ;  
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "LayerTrackProjection Failed to register histogram " );       
	      return sc;
	    }
	  LayerTrackProjection->SetFillColor(42);
	  LayerTrackProjection->GetXaxis()->SetTitle("(0=LPt0,1=LPt1,2=Piv0,3=Piv1,4=HPt0,5=HPt1)+(eta=0,1=phi)*6+(0=Small,1=Large)*12");
	  LayerTrackProjection->GetYaxis()->SetTitle("Number of Tracks"); 
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << LayerTrackProjection << m_generic_path_LayerTrackProjection.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );	       
	    ATH_MSG_DEBUG ( "Booked bookLayerTrackProjection successfully" );}   
	

	  std::string m_generic_path_LayerHitOnTrack = m_generic_path_rpcmonitoring+"/Overview";
	  std::string m_LayerHitOnTrack_title = "Hit_on_Track_Layer";
	  const char* m_LayerHitOnTrack_title_char = m_LayerHitOnTrack_title.c_str();  
	 	  
	  LayerHitOnTrack=new TH1I(m_LayerHitOnTrack_title_char,m_LayerHitOnTrack_title_char,24,0,24);	    
	  sc = rpc_dqmf_global.regHist(LayerHitOnTrack) ;  
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "LayerHitOnTrack Failed to register histogram " );       
	      return sc;
	    }
	  LayerHitOnTrack->SetFillColor(42);
	  LayerHitOnTrack->GetXaxis()->SetTitle("LayerType(LPt0,LPt1,Piv0,Piv1,HPt0,HPt1) + ( Eta or Phi)*6 + (Small or Large)*12");
	  LayerHitOnTrack->GetXaxis()->SetTitleSize(0.03)        ;
	  LayerHitOnTrack->GetXaxis()->SetTitleOffset(1.2)       ;
	  LayerHitOnTrack->GetYaxis()->SetTitle("Number of Hits"); 
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << LayerHitOnTrack << m_generic_path_LayerHitOnTrack.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );	       
	    ATH_MSG_DEBUG ( "Booked bookLayerHitOnTrack successfully" );}   
	
	 	  
	 	  
	
	  //////////////////////////////////////
	  // Booking summary plot
	  // declare group of summary histograms per readout strip panels
	  //int TOTPanelsSummary =0;	  
	  for (int i_sec=0; i_sec!=16; i_sec++) {
	    
	    bookRPCSummaryHistograms	(i_sec , "TrackProj"			); //0
	    bookRPCSummaryHistograms	(i_sec , "HitOnTrack"			); //1
	    bookRPCSummaryHistograms	(i_sec , "HitOnTrack_withCrossStrip"	); //2
	    bookRPCSummaryHistograms	(i_sec , "Efficiency"			); //3
	    bookRPCSummaryHistograms	(i_sec , "GapEfficiency"		); //4
	    bookRPCSummaryHistograms	(i_sec , "NoiseTot_NotNorm"		); //5
	    bookRPCSummaryHistograms	(i_sec , "NoiseCorr_NotNorm"		); //6
	    bookRPCSummaryHistograms	(i_sec , "CS_NotNorm"			); //7
	    bookRPCSummaryHistograms	(i_sec , "CS_square"			); //8
	    bookRPCSummaryHistograms	(i_sec , "CS_entries"			); //9
	    bookRPCSummaryHistograms	(i_sec , "Res_CS1_NotNorm"		); //10
	    bookRPCSummaryHistograms	(i_sec , "Res_CS1_square"		); //11
	    bookRPCSummaryHistograms	(i_sec , "Res_CS1_entries"		); //12
	    bookRPCSummaryHistograms	(i_sec , "Res_CS2_NotNorm"		); //13
	    bookRPCSummaryHistograms	(i_sec , "Res_CS2_square"		); //14
	    bookRPCSummaryHistograms	(i_sec , "Res_CS2_entries"		); //15
	    bookRPCSummaryHistograms	(i_sec , "Res_CSmore2_NotNorm"		); //16
	    bookRPCSummaryHistograms	(i_sec , "Res_CSmore2_square"		); //17
	    bookRPCSummaryHistograms	(i_sec , "Res_CSmore2_entries"		); //18
	    bookRPCSummaryHistograms	(i_sec , "Occupancy_NotNorm"		); //19
	    bookRPCSummaryHistograms	(i_sec , "Time_NotNorm"			); //20
	    bookRPCSummaryHistograms	(i_sec , "Time_square"			); //21
	    bookRPCSummaryHistograms	(i_sec , "CS1_entries"			); //22
	    bookRPCSummaryHistograms	(i_sec , "CS2_entries"			); //23
	  

	    bookRPCSummaryFinalHistograms (i_sec , "NoiseCorr"			); //0
	    bookRPCSummaryFinalHistograms	(i_sec , "NoiseTot"			); //1
	    bookRPCSummaryFinalHistograms	(i_sec , "Res_CS1"			); //2
	    bookRPCSummaryFinalHistograms	(i_sec , "Res_CS2"			); //3
	    bookRPCSummaryFinalHistograms	(i_sec , "Res_CSmore2"			); //4
	    bookRPCSummaryFinalHistograms	(i_sec , "Occupancy"			); //5
	    bookRPCSummaryFinalHistograms	(i_sec , "CS"				); //6
	    bookRPCSummaryFinalHistograms	(i_sec , "Time"				); //7
	    
	    
	    char sector_char[100]   ;
	    std::string sector_name ;
	    sprintf(sector_char,"Sector%.2d",i_sec+1) ;  // sector number with 2 digits
	    sector_name = sector_char ;
	
	    // large sectors: (i_sec+1)%2 = 1;  small (i_sec+1)%2 = 0
	    n_eta_station = ( (i_sec+1) %2 ) + 6 + 1 ;  // small: 1-6, large: 1-7, sec 12+14: 0-6, sec 13: 1-7 (8->7)	    
	    int n_bin = 72 * n_eta_station   ;  	    
	    //  72 = 2*2*2*3*3  measPhi + gap + dblPhi + dblZ + stationName + stationEta	  
	    std::string PanelIndexTitle = "# Panel = View + (gap-1)*2 + (dblPhi-1)*4 + (dblZ-1)*8 + (LowPt=0,Pivot=1,HighPt=2)*24 + (Eta)*72";
	  
	    // book panelID histograms
	    std::string m_SummaryPanelID_title      = "SummaryPanelID_" + sector_name ;
	    const char* m_SummaryPanelID_title_char = m_SummaryPanelID_title.c_str()  ;
	  
	    TH1* SummaryPanelID = new TH1F(m_SummaryPanelID_title_char, m_SummaryPanelID_title_char, 2*n_bin, -n_bin, n_bin);
	    SummaryPanelID->SetFillColor(2);
	    SummaryPanelID->GetXaxis()-> SetTitle(PanelIndexTitle.c_str() );
	    SummaryPanelID->GetXaxis()-> SetTitleSize(0.03)  ;
	    SummaryPanelID->GetXaxis()-> SetTitleOffset(1.2) ;
	    SummaryPanelID->GetYaxis()-> SetTitle("Panel Id");
	    SummaryPanelID->SetMarkerStyle(20)               ;
	    SummaryPanelID->SetMarkerColor(2)                ;
	    SummaryPanelID->SetBit(TH1::kIsAverage)          ;
 
	    sc = rpcprd_expert_sum.regHist( SummaryPanelID );
	    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register SummaryPanelID hist to MonGroup" );
	  	  
	    panelBin   = 0   ;
	    indexplane = 0   ;
	    // Identifier gapId ;
	    Identifier panelId ;
	    //std::cout<<" before the loop: sector: "<< i_sec <<std::endl;
	    
	    for (int iname=2; iname!=53+1; iname++ ){
	      if(iname>10&&iname<53)continue;
	      if ( (i_sec%2 == 1) && (iname==2 || iname==4  || iname==53) ) continue;  /* small sectors */
	      if ( (i_sec%2 == 0) && (iname!=2 && iname!=4  && iname!=53) ) continue;  /* large sectors */	      
	      for(int ieta=0; ieta!= 17; ieta++){
		for(int ir=0; ir!= 2; ir++){
		  for(int idbz=0; idbz!= 3; idbz++){
		    for (int idbphi=0; idbphi!=2; idbphi++) {
		      for ( int imeasphi=0; imeasphi!=2; imeasphi++ ) {
			for (int igap=0; igap!=2; igap++) {
        bool isValid=false;
        Identifier rpcId = m_rpcIdHelper->channelID(iname, (ieta-8), int(i_sec/2)+1, ir+1, idbz+1, idbphi, 1, 1, 1, true, &isValid, true); // last 6 arguments are: int doubletPhi, int gasGap, int measuresPhi, int strip, bool check, bool* isValid, bool noPrint
        if (!isValid) {
          ATH_MSG_DEBUG("Could not find valid Identifier for station="<<iname<<", eta="<<(ieta-8)<<", phi="<<int(i_sec/2)+1<<", doubletR="<<ir+1<<", doubletZ="<<idbz+1<<", doubletPhi="<<idbphi+1<<", continuing...");
          continue;
        }
        const MuonGM::RpcReadoutElement* rpc = m_muonMgr->getRpcReadoutElement(rpcId);
			  // need to pay attention to BME case - not yet considered here .... 
			  if (rpc) {

			    panelId = m_rpcIdHelper->panelID(iname, ieta-8, int(i_sec/2)+1, ir+1, idbz+1, idbphi+1, igap+1, imeasphi) ; 
			    
			    indexplane = ir ;
			    
			    if ( ir==0 && (iname==4 || iname==5 || iname == 9 || iname == 10) )  indexplane = 2 ;
			    
			    
			    panelBin = imeasphi + igap*2 + idbphi*4 + idbz*8 + indexplane*24 + abs(ieta-8)*72 ;
			    
			    //BOG0-4 one panel, BOF1-3 two panels, BOF4 one panel 
			    if ( ir==0 && iname == 10 ) {
			      // convention: BOG <-> doubletZ=4 <-> (3-1)*8=16
			      panelBin = imeasphi + igap*2 + idbphi*4 + 16 + indexplane*24 + abs(ieta-8)*72 ;
			    }
			    if ( ir==1 && iname == 10 ) {//feet extension BOG3-4
			      // convention: BOG with ir=1 <-> doubletZ=3 <-> (3-1)*8=16
			      panelBin = imeasphi + igap*2 + idbphi*4 + 16 + indexplane*24 + abs(ieta-8)*72 ;
			    }
			    if ( ir==1 && iname ==  9 ) {//feet extension BOF2-3 and BOF4
			      // convention: BOF with ir=1 <-> eta->eta+2
			      panelBin = imeasphi + igap*2 + idbphi*4 + idbz*8 + indexplane*24 + (abs(ieta-8)+2)*72 ;
			    }
			    //sector 13 BME
			    if ( iname==53 ) {
			      // convention: BME <-> doubletZ=3 <-> (3-1)*8=16
			      panelBin = imeasphi + igap*2 + idbphi*4 + 16 + indexplane*24 + abs(ieta-8)*72 ;
			    }
			    //sector 13 BOL8
			    if ( abs(ieta-8)==8 ) {
			      // convention: BOL8 <-> eta->eta-1
			      panelBin = imeasphi + igap*2 + idbphi*4 + 16 + indexplane*24 + 7*72 ;
			    }
	          
			    if ( (ieta-8) <0 ) panelBin = - panelBin; 
			    SummaryPanelID->Fill(panelBin, panelId.get_identifier32().get_compact() );
			    //std::cout <<" panelBin "<< panelBin  << " " <<panelId.get_identifier32().get_compact() <<" Entries " << SummaryPanelID->GetEntries()<< std::endl; 
			    
			  }
			}
		      }
		    }
		  }
		}
	      }
	    }
	    //std::cout<<" after the loop"<<std::endl;
	  
	    // 1) panel efficiency distribution per sector
	    std::string m_generic_path_SummaryEffDistriPerSector = m_generic_path_rpcmonitoring+"/Summary";
	    m_generic_path_SummaryEffDistriPerSector += sector_name;
	    std::string m_SummaryEffDistriPerSector_title      = "SummaryEffDistriPerSector_" + sector_name ;
	    const char* m_SummaryEffDistriPerSector_title_char = m_SummaryEffDistriPerSector_title.c_str()  ;
	    TH1* SummaryEffDistriPerSector = new TH1F ( m_SummaryEffDistriPerSector_title_char, m_SummaryEffDistriPerSector_title_char, 110, 0, 1.1 ) ;
	    SummaryEffDistriPerSector->SetFillColor(2);
	    SummaryEffDistriPerSector->GetXaxis()-> SetTitle("Panel Efficiency");
	    SummaryEffDistriPerSector->GetXaxis()-> SetTitleSize(0.03)	  ;
	    SummaryEffDistriPerSector->GetXaxis()-> SetTitleOffset(1.2)	  ;
	    SummaryEffDistriPerSector->GetYaxis()-> SetTitle("Counts/0.01");
	    {
	      ATH_MSG_DEBUG ( "INSIDE bookRPCHistograms : " << SummaryEffDistriPerSector << m_generic_path_SummaryEffDistriPerSector.c_str() );
	      //ATH_MSG_DEBUG ( "EXPERT : " << expert );
	      ATH_MSG_DEBUG ( "RUN : " << run );}
	
	    sc = rpcprd_expert_sum.regHist( SummaryEffDistriPerSector ) ;
	    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register  SummaryEffDistriPerSector hist to MonGroup" );    
	  
	    // 2) panel GapEfficiency distribution per sector
	    std::string m_generic_path_SummaryGapEffDistriPerSector = m_generic_path_rpcmonitoring+"/Summary";
	    m_generic_path_SummaryGapEffDistriPerSector += sector_name;
	    std::string m_SummaryGapEffDistriPerSector_title      = "SummaryGapEffDistriPerSector_" + sector_name ;
	    const char* m_SummaryGapEffDistriPerSector_title_char = m_SummaryGapEffDistriPerSector_title.c_str()  ;
	    TH1* SummaryGapEffDistriPerSector = new TH1F ( m_SummaryGapEffDistriPerSector_title_char, m_SummaryGapEffDistriPerSector_title_char, 110, 0, 1.1 ) ;
	    SummaryGapEffDistriPerSector->SetFillColor(2);
	    SummaryGapEffDistriPerSector->GetXaxis()-> SetTitle("GasGap Efficiency");
	    SummaryGapEffDistriPerSector->GetXaxis()-> SetTitleSize(0.03)	  ;
	    SummaryGapEffDistriPerSector->GetXaxis()-> SetTitleOffset(1.2)	  ;
	    SummaryGapEffDistriPerSector->GetYaxis()-> SetTitle("Counts/0.01");
	    {
	      ATH_MSG_DEBUG ( "INSIDE bookRPCHistograms : " << SummaryGapEffDistriPerSector << m_generic_path_SummaryGapEffDistriPerSector.c_str() );
	      //ATH_MSG_DEBUG ( "EXPERT : " << expert );
	      ATH_MSG_DEBUG ( "RUN : " << run );}
	
	    sc = rpcprd_expert_sum.regHist( SummaryGapEffDistriPerSector ) ;
	    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register  SummaryGapEffDistriPerSector hist to MonGroup" );    
	  
	    // 3) panel NoiseCorr distribution per sector
	    std::string m_generic_path_SummaryNoiseCorrDistriPerSector = m_generic_path_rpcmonitoring+"/Summary";
	    m_generic_path_SummaryNoiseCorrDistriPerSector += sector_name;
	    std::string m_SummaryNoiseCorrDistriPerSector_title      = "SummaryNoiseCorrDistriPerSector_" + sector_name ;
	    const char* m_SummaryNoiseCorrDistriPerSector_title_char = m_SummaryNoiseCorrDistriPerSector_title.c_str()  ;
	    TH1* SummaryNoiseCorrDistriPerSector = new TH1F ( m_SummaryNoiseCorrDistriPerSector_title_char, m_SummaryNoiseCorrDistriPerSector_title_char, 1000, 0, 100 ) ;
	    SummaryNoiseCorrDistriPerSector->SetFillColor(2);
	    SummaryNoiseCorrDistriPerSector->GetXaxis()-> SetTitle("Panel Correlated Noise");
	    SummaryNoiseCorrDistriPerSector->GetXaxis()-> SetTitleSize(0.03)	  ;
	    SummaryNoiseCorrDistriPerSector->GetXaxis()-> SetTitleOffset(1.2)	  ;
	    SummaryNoiseCorrDistriPerSector->GetYaxis()-> SetTitle("Counts/0.1Hz/cm2");
	    {
	      ATH_MSG_DEBUG ( "INSIDE bookRPCHistograms : " << SummaryNoiseCorrDistriPerSector << m_generic_path_SummaryNoiseCorrDistriPerSector.c_str() );
	      //ATH_MSG_DEBUG ( "EXPERT : " << expert );
	      ATH_MSG_DEBUG ( "RUN : " << run );}
	
	    sc = rpcprd_expert_sum.regHist( SummaryNoiseCorrDistriPerSector ) ;
	    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register  SummaryNoiseCorrDistriPerSector hist to MonGroup" );    
	  
	    // 4) panel NoiseTot distribution per sector
	    std::string m_generic_path_SummaryNoiseTotDistriPerSector = m_generic_path_rpcmonitoring+"/Summary";
	    m_generic_path_SummaryNoiseTotDistriPerSector += sector_name;
	    std::string m_SummaryNoiseTotDistriPerSector_title      = "SummaryNoiseTotDistriPerSector_" + sector_name ;
	    const char* m_SummaryNoiseTotDistriPerSector_title_char = m_SummaryNoiseTotDistriPerSector_title.c_str()  ;
	    TH1* SummaryNoiseTotDistriPerSector = new TH1F ( m_SummaryNoiseTotDistriPerSector_title_char,  m_SummaryNoiseTotDistriPerSector_title_char, 1000, 0, 100 ) ;
	    SummaryNoiseTotDistriPerSector->SetFillColor(2);
	    SummaryNoiseTotDistriPerSector->GetXaxis()-> SetTitle("Panel Noise Total");
	    SummaryNoiseTotDistriPerSector->GetXaxis()-> SetTitleSize(0.03)	  ;
	    SummaryNoiseTotDistriPerSector->GetXaxis()-> SetTitleOffset(1.2)	  ;
	    SummaryNoiseTotDistriPerSector->GetYaxis()-> SetTitle("Counts/0.1Hz/cm2");
	    {
	      ATH_MSG_DEBUG ( "INSIDE bookRPCHistograms : " << SummaryNoiseTotDistriPerSector << m_generic_path_SummaryNoiseTotDistriPerSector.c_str() );
	      //ATH_MSG_DEBUG ( "EXPERT : " << expert );
	      ATH_MSG_DEBUG ( "RUN : " << run );}
	
	    sc = rpcprd_expert_sum.regHist( SummaryNoiseTotDistriPerSector ) ;
	    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register  SummaryNoiseTotDistriPerSector hist to MonGroup" );    
	  
	    //    panel NoiseNotCorr distribution per sector
	    std::string m_generic_path_SummaryNoiseNotCorrDistriPerSector = m_generic_path_rpcmonitoring+"/Summary";
	    m_generic_path_SummaryNoiseNotCorrDistriPerSector += sector_name;
	    std::string m_SummaryNoiseNotCorrDistriPerSector_title      = "SummaryNoiseNotCorrDistriPerSector_" + sector_name ;
	    const char* m_SummaryNoiseNotCorrDistriPerSector_title_char = m_SummaryNoiseNotCorrDistriPerSector_title.c_str()  ;
	    TH1* SummaryNoiseNotCorrDistriPerSector = new TH1F ( m_SummaryNoiseNotCorrDistriPerSector_title_char,  m_SummaryNoiseNotCorrDistriPerSector_title_char, 1000, 0, 100 ) ;
	    SummaryNoiseNotCorrDistriPerSector->SetFillColor(2);
	    SummaryNoiseNotCorrDistriPerSector->GetXaxis()-> SetTitle("Panel Noise Not Corr");
	    SummaryNoiseNotCorrDistriPerSector->GetXaxis()-> SetTitleSize(0.03)	  ;
	    SummaryNoiseNotCorrDistriPerSector->GetXaxis()-> SetTitleOffset(1.2)	  ;
	    SummaryNoiseNotCorrDistriPerSector->GetYaxis()-> SetTitle("Counts/0.1Hz/cm2");
	    {
	      ATH_MSG_DEBUG ( "INSIDE bookRPCHistograms : " << SummaryNoiseNotCorrDistriPerSector << m_generic_path_SummaryNoiseNotCorrDistriPerSector.c_str() );
	      //ATH_MSG_DEBUG ( "EXPERT : " << expert );
	      ATH_MSG_DEBUG ( "RUN : " << run );}
	
	    sc = rpcprd_expert_sum.regHist( SummaryNoiseNotCorrDistriPerSector ) ;
	    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register  SummaryNoiseNotCorrDistriPerSector hist to MonGroup" );    
	  
	  
	    // 5) panel CS distribution per sector
	    std::string m_generic_path_SummaryCSDistriPerSector = m_generic_path_rpcmonitoring+"/Summary";
	    m_generic_path_SummaryCSDistriPerSector += sector_name;
	    std::string m_SummaryCSDistriPerSector_title      = "SummaryCSDistriPerSector_" + sector_name ;
	    const char* m_SummaryCSDistriPerSector_title_char = m_SummaryCSDistriPerSector_title.c_str()  ;
	    TH1* SummaryCSDistriPerSector = new TH1F ( m_SummaryCSDistriPerSector_title_char, m_SummaryCSDistriPerSector_title_char, 200, 0, 20);
	    SummaryCSDistriPerSector->SetFillColor(2);
	    SummaryCSDistriPerSector->GetXaxis()-> SetTitle("Panel  CS");
	    SummaryCSDistriPerSector->GetXaxis()-> SetTitleSize(0.03)	  ;
	    SummaryCSDistriPerSector->GetXaxis()-> SetTitleOffset(1.2)	  ;
	    SummaryCSDistriPerSector->GetYaxis()-> SetTitle("Counts/0.1");
	    {
	      ATH_MSG_DEBUG ( "INSIDE bookRPCHistograms : " << SummaryCSDistriPerSector << m_generic_path_SummaryCSDistriPerSector.c_str() );
	      //ATH_MSG_DEBUG ( "EXPERT : " << expert );
	      ATH_MSG_DEBUG ( "RUN : " << run );}
	
	    sc = rpcprd_expert_sum.regHist( SummaryCSDistriPerSector ) ;
	    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register  SummaryCSDistriPerSector hist to MonGroup" );    
	  
	    // 6) panel res_CS1 distribution per sector
	    std::string m_generic_path_SummaryRes_CS1DistriPerSector = m_generic_path_rpcmonitoring+"/Summary";
	    m_generic_path_SummaryRes_CS1DistriPerSector += sector_name;
	    std::string m_SummaryRes_CS1DistriPerSector_title      = "SummaryRes_CS1DistriPerSector_" + sector_name ;
	    const char* m_SummaryRes_CS1DistriPerSector_title_char = m_SummaryRes_CS1DistriPerSector_title.c_str()  ;
	    TH1* SummaryRes_CS1DistriPerSector = new TH1F ( m_SummaryRes_CS1DistriPerSector_title_char, m_SummaryRes_CS1DistriPerSector_title_char, 1000, -500, 500 ) ;
	    SummaryRes_CS1DistriPerSector->SetFillColor(2);
	    SummaryRes_CS1DistriPerSector->GetXaxis()-> SetTitle("Average Strip Panel res_CS1 [mm]");
	    SummaryRes_CS1DistriPerSector->GetXaxis()-> SetTitleSize(0.03)	  ;
	    SummaryRes_CS1DistriPerSector->GetXaxis()-> SetTitleOffset(1.2)	  ;
	    SummaryRes_CS1DistriPerSector->GetYaxis()-> SetTitle("Counts/1mm");
	    {
	      ATH_MSG_DEBUG ( "INSIDE bookRPCHistograms : " << SummaryRes_CS1DistriPerSector << m_generic_path_SummaryRes_CS1DistriPerSector.c_str() );
	      //ATH_MSG_DEBUG ( "EXPERT : " << expert );
	      ATH_MSG_DEBUG ( "RUN : " << run );}
	
	    sc = rpcprd_expert_sum.regHist( SummaryRes_CS1DistriPerSector ) ;
	    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register  SummaryRes_CS1DistriPerSector hist to MonGroup" );    
	  
	    // 7) panel res_CS2 distribution per sector
	    std::string m_generic_path_SummaryRes_CS2DistriPerSector = m_generic_path_rpcmonitoring+"/Summary";
	    m_generic_path_SummaryRes_CS2DistriPerSector += sector_name;
	    std::string m_SummaryRes_CS2DistriPerSector_title      = "SummaryRes_CS2DistriPerSector_" + sector_name ;
	    const char* m_SummaryRes_CS2DistriPerSector_title_char = m_SummaryRes_CS2DistriPerSector_title.c_str()  ;
	    TH1* SummaryRes_CS2DistriPerSector = new TH1F ( m_SummaryRes_CS2DistriPerSector_title_char, m_SummaryRes_CS2DistriPerSector_title_char, 1000, -500, 500 ) ;
	    SummaryRes_CS2DistriPerSector->SetFillColor(2);
	    SummaryRes_CS2DistriPerSector->GetXaxis()-> SetTitle("Average Strip Panel res_CS2 [mm]");
	    SummaryRes_CS2DistriPerSector->GetXaxis()-> SetTitleSize(0.03)	  ;
	    SummaryRes_CS2DistriPerSector->GetXaxis()-> SetTitleOffset(1.2)	  ;
	    SummaryRes_CS2DistriPerSector->GetYaxis()-> SetTitle("Counts/1mm");
	    {
	      ATH_MSG_DEBUG ( "INSIDE bookRPCHistograms : " << SummaryRes_CS2DistriPerSector << m_generic_path_SummaryRes_CS2DistriPerSector.c_str() );
	      //ATH_MSG_DEBUG ( "EXPERT : " << expert );
	      ATH_MSG_DEBUG ( "RUN : " << run );}
	
	    sc = rpcprd_expert_sum.regHist( SummaryRes_CS2DistriPerSector ) ;
	    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register  SummaryRes_CS2DistriPerSector hist to MonGroup" );    
	  
	    // 8) panel res_CSmore2 distribution per sector
	    std::string m_generic_path_SummaryRes_CSmore2DistriPerSector = m_generic_path_rpcmonitoring+"/Summary";
	    m_generic_path_SummaryRes_CSmore2DistriPerSector += sector_name;
	    std::string m_SummaryRes_CSmore2DistriPerSector_title      = "SummaryRes_CSmore2DistriPerSector_" + sector_name ;
	    const char* m_SummaryRes_CSmore2DistriPerSector_title_char = m_SummaryRes_CSmore2DistriPerSector_title.c_str()  ;
	    TH1* SummaryRes_CSmore2DistriPerSector = new TH1F ( m_SummaryRes_CSmore2DistriPerSector_title_char, m_SummaryRes_CSmore2DistriPerSector_title_char,1000, -500, 500 ) ;
	    SummaryRes_CSmore2DistriPerSector->SetFillColor(2);
	    SummaryRes_CSmore2DistriPerSector->GetXaxis()-> SetTitle("Average Strip Panel res_CSmore2 [mm]");
	    SummaryRes_CSmore2DistriPerSector->GetXaxis()-> SetTitleSize(0.03)	  ;
	    SummaryRes_CSmore2DistriPerSector->GetXaxis()-> SetTitleOffset(1.2)	  ;
	    SummaryRes_CSmore2DistriPerSector->GetYaxis()-> SetTitle("Counts/1mm");
	    {
	      ATH_MSG_DEBUG ( "INSIDE bookRPCHistograms : " << SummaryRes_CSmore2DistriPerSector << m_generic_path_SummaryRes_CSmore2DistriPerSector.c_str() );
	      //ATH_MSG_DEBUG ( "EXPERT : " << expert );
	      ATH_MSG_DEBUG ( "RUN : " << run );}
	
	    sc = rpcprd_expert_sum.regHist( SummaryRes_CSmore2DistriPerSector ) ;
	    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register  SummaryRes_CSmore2DistriPerSector hist to MonGroup" );    

 	  
	    // 9) panel Occupancy distribution per sector
	    std::string m_generic_path_SummaryOccupancyDistriPerSector = m_generic_path_rpcmonitoring+"/Summary";
	    m_generic_path_SummaryOccupancyDistriPerSector += sector_name;
	    std::string m_SummaryOccupancyDistriPerSector_title      = "SummaryOccupancyDistriPerSector_" + sector_name ;
	    const char* m_SummaryOccupancyDistriPerSector_title_char = m_SummaryOccupancyDistriPerSector_title.c_str()  ;
	    TH1* SummaryOccupancyDistriPerSector = new TH1F ( m_SummaryOccupancyDistriPerSector_title_char, m_SummaryOccupancyDistriPerSector_title_char, 100, -10., 2. ) ;
	    SummaryOccupancyDistriPerSector->SetFillColor(2);
	    SummaryOccupancyDistriPerSector->GetXaxis()-> SetTitle("Occupancy");
	    SummaryOccupancyDistriPerSector->GetXaxis()-> SetTitleSize(0.03)	  ;
	    SummaryOccupancyDistriPerSector->GetXaxis()-> SetTitleOffset(1.2)	  ;
	    SummaryOccupancyDistriPerSector->GetYaxis()-> SetTitle("Counts");
	    {
	      ATH_MSG_DEBUG ( "INSIDE bookRPCHistograms : " << SummaryOccupancyDistriPerSector << m_generic_path_SummaryOccupancyDistriPerSector.c_str() );
	      //ATH_MSG_DEBUG ( "EXPERT : " << expert );
	      ATH_MSG_DEBUG ( "RUN : " << run );}
	
	    sc = rpcprd_expert_sum.regHist( SummaryOccupancyDistriPerSector ) ;
	    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register  SummaryOccupancyDistriPerSector hist to MonGroup" );    

	  
	    // 10) panel res_CS1rms distribution per sector
	    std::string m_generic_path_SummaryRes_CS1rmsDistriPerSector = m_generic_path_rpcmonitoring+"/Summary";
	    m_generic_path_SummaryRes_CS1rmsDistriPerSector += sector_name;
	    std::string m_SummaryRes_CS1rmsDistriPerSector_title      = "SummaryRes_CS1rmsDistriPerSector_" + sector_name ;
	    const char* m_SummaryRes_CS1rmsDistriPerSector_title_char = m_SummaryRes_CS1rmsDistriPerSector_title.c_str()  ;
	    TH1* SummaryRes_CS1rmsDistriPerSector = new TH1F ( m_SummaryRes_CS1rmsDistriPerSector_title_char,  m_SummaryRes_CS1rmsDistriPerSector_title_char, 50, 0, 50 ) ;
	    SummaryRes_CS1rmsDistriPerSector->SetFillColor(2);
	    SummaryRes_CS1rmsDistriPerSector->GetXaxis()-> SetTitle("Average Strip Panel res_CS1 rms [mm]");
	    SummaryRes_CS1rmsDistriPerSector->GetXaxis()-> SetTitleSize(0.03)	  ;
	    SummaryRes_CS1rmsDistriPerSector->GetXaxis()-> SetTitleOffset(1.2)	  ;
	    SummaryRes_CS1rmsDistriPerSector->GetYaxis()-> SetTitle("Counts/1mm");
	    {
	      ATH_MSG_DEBUG ( "INSIDE bookRPCHistograms : " << SummaryRes_CS1rmsDistriPerSector << m_generic_path_SummaryRes_CS1rmsDistriPerSector.c_str() );
	      //ATH_MSG_DEBUG ( "EXPERT : " << expert );
	      ATH_MSG_DEBUG ( "RUN : " << run );}
	
	    sc = rpcprd_expert_sum.regHist( SummaryRes_CS1rmsDistriPerSector ) ;
	    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register  SummaryRes_CS1rmsDistriPerSector hist to MonGroup" );    
	  
	    // 11) panel res_CS2rms distribution per sector
	    std::string m_generic_path_SummaryRes_CS2rmsDistriPerSector = m_generic_path_rpcmonitoring+"/Summary";
	    m_generic_path_SummaryRes_CS2rmsDistriPerSector += sector_name;
	    std::string m_SummaryRes_CS2rmsDistriPerSector_title      = "SummaryRes_CS2rmsDistriPerSector_" + sector_name ;
	    const char* m_SummaryRes_CS2rmsDistriPerSector_title_char = m_SummaryRes_CS2rmsDistriPerSector_title.c_str()  ;
	    TH1* SummaryRes_CS2rmsDistriPerSector = new TH1F ( m_SummaryRes_CS2rmsDistriPerSector_title_char, m_SummaryRes_CS2rmsDistriPerSector_title_char,  50, 0, 50 ) ;
	    SummaryRes_CS2rmsDistriPerSector->SetFillColor(2);
	    SummaryRes_CS2rmsDistriPerSector->GetXaxis()-> SetTitle("Average Strip Panel res_CS2 rms [mm]");
	    SummaryRes_CS2rmsDistriPerSector->GetXaxis()-> SetTitleSize(0.03)	  ;
	    SummaryRes_CS2rmsDistriPerSector->GetXaxis()-> SetTitleOffset(1.2)	  ;
	    SummaryRes_CS2rmsDistriPerSector->GetYaxis()-> SetTitle("Counts/1mm");
	    {
	      ATH_MSG_DEBUG ( "INSIDE bookRPCHistograms : " << SummaryRes_CS2rmsDistriPerSector << m_generic_path_SummaryRes_CS2rmsDistriPerSector.c_str() );
	      //ATH_MSG_DEBUG ( "EXPERT : " << expert );
	      ATH_MSG_DEBUG ( "RUN : " << run );}
	
	    sc = rpcprd_expert_sum.regHist( SummaryRes_CS2rmsDistriPerSector ) ;
	    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register  SummaryRes_CS2rmsDistriPerSector hist to MonGroup" );    

	 	  
	    // 12) panel res_CSmore2rms distribution per sector
	    std::string m_generic_path_SummaryRes_CSmore2rmsDistriPerSector = m_generic_path_rpcmonitoring+"/Summary";
	    m_generic_path_SummaryRes_CSmore2rmsDistriPerSector += sector_name;
	    std::string m_SummaryRes_CSmore2rmsDistriPerSector_title      = "SummaryRes_CSmore2rmsDistriPerSector_" + sector_name ;
	    const char* m_SummaryRes_CSmore2rmsDistriPerSector_title_char = m_SummaryRes_CSmore2rmsDistriPerSector_title.c_str()  ;
	    TH1* SummaryRes_CSmore2rmsDistriPerSector = new TH1F ( m_SummaryRes_CSmore2rmsDistriPerSector_title_char, m_SummaryRes_CSmore2rmsDistriPerSector_title_char, 50, 0, 50 ) ;
	    SummaryRes_CSmore2rmsDistriPerSector->SetFillColor(2);
	    SummaryRes_CSmore2rmsDistriPerSector->GetXaxis()-> SetTitle("Average Strip Panel res_CSmore2 rms [mm]");
	    SummaryRes_CSmore2rmsDistriPerSector->GetXaxis()-> SetTitleSize(0.03)	  ;
	    SummaryRes_CSmore2rmsDistriPerSector->GetXaxis()-> SetTitleOffset(1.2)	  ;
	    SummaryRes_CSmore2rmsDistriPerSector->GetYaxis()-> SetTitle("Counts/1mm");
	    {
	      ATH_MSG_DEBUG ( "INSIDE bookRPCHistograms : " << SummaryRes_CSmore2rmsDistriPerSector << m_generic_path_SummaryRes_CSmore2rmsDistriPerSector.c_str() );
	      //ATH_MSG_DEBUG ( "EXPERT : " << expert );
	      ATH_MSG_DEBUG ( "RUN : " << run );}
	
	    sc = rpcprd_expert_sum.regHist( SummaryRes_CSmore2rmsDistriPerSector ) ;
	    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register  SummaryRes_CSmore2rmsDistriPerSector hist to MonGroup" );    
	

	 	  
	    // 13) panel Time distribution per sector
	    std::string m_generic_path_SummaryTimeDistriPerSector = m_generic_path_rpcmonitoring+"/Summary";
	    m_generic_path_SummaryTimeDistriPerSector += sector_name;
	    std::string m_SummaryTimeDistriPerSector_title      = "SummaryTimeDistriPerSector_" + sector_name ;
	    const char* m_SummaryTimeDistriPerSector_title_char = m_SummaryTimeDistriPerSector_title.c_str()  ;
	    TH1* SummaryTimeDistriPerSector = new TH1F ( m_SummaryTimeDistriPerSector_title_char, m_SummaryTimeDistriPerSector_title_char, 64, 0, 200 ) ;
	    SummaryTimeDistriPerSector->SetFillColor(2);
	    SummaryTimeDistriPerSector->GetXaxis()-> SetTitle("Average Strip Panel Time");
	    SummaryTimeDistriPerSector->GetXaxis()-> SetTitleSize(0.03)	  ;
	    SummaryTimeDistriPerSector->GetXaxis()-> SetTitleOffset(1.2)	  ;
	    SummaryTimeDistriPerSector->GetYaxis()-> SetTitle("Counts/3.125ns");
	    {
	      ATH_MSG_DEBUG ( "INSIDE bookRPCHistograms : " << SummaryTimeDistriPerSector << m_generic_path_SummaryTimeDistriPerSector.c_str() );
	      //ATH_MSG_DEBUG ( "EXPERT : " << expert );
	      ATH_MSG_DEBUG ( "RUN : " << run );}
	
	    sc = rpcprd_expert_sum.regHist( SummaryTimeDistriPerSector ) ;
	    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register  SummaryTimeDistriPerSector hist to MonGroup" ); 
	    
	    //TOTPanelsSummary += SummaryPanelID->GetEntries(); 
	    //std::cout  << " TOTPanelsSummary " << TOTPanelsSummary << std::endl;  
	 	  
	  }
	  m_SummaryHist_Size =  m_SummaryHist.size() ;
	
	
	  //////////////////////////////
	  // DQMF shift histograms   
        
	  // track time residual : dqmf shifter histogram
	  std::string m_generic_path_rpcTimeTrackRes = m_generic_path_rpcmonitoring+"/GLOBAL";
	  const char* m_rpcTimeTrackRes_title_char = "TimeTrackResidual";
	  TH1 *rpcTimeTrackRes = new TH1I(m_rpcTimeTrackRes_title_char, m_rpcTimeTrackRes_title_char, timeNbin, -timemaxrange/2, timemaxrange/2);
	  sc=rpc_dqmf_global.regHist( rpcTimeTrackRes ) ; 
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "TimeTrackResidulal Failed to register histogram " );       
	      return sc;
	    }
	  rpcTimeTrackRes->SetFillColor(42);
	  rpcTimeTrackRes->GetXaxis()->SetTitle("Track Time Residual [ns]");
	  rpcTimeTrackRes->GetYaxis()->SetTitle("Counts/(3.125ns)");
	
	  // track time RMS :  dqmf shifter histogram
	  std::string m_generic_path_rpcTimeTrackRMS = m_generic_path_rpcmonitoring+"/GLOBAL";
	  const char* m_rpcTimeTrackRMS_title_char = "TimeTrackRMS";
	  TH1 *rpcTimeTrackRMS = new TH1I(m_rpcTimeTrackRMS_title_char, m_rpcTimeTrackRMS_title_char, timeNbin, timeminrange, timemaxrange);
	  sc=rpcprd_shift.regHist( rpcTimeTrackRMS ) ; 
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "TimeTrackRMS Failed to register histogram " );       
	      return sc;
	    }
	  rpcTimeTrackRMS->SetFillColor(42);
	  rpcTimeTrackRMS->GetXaxis()->SetTitle("Track Time RMS [ns]");
	  rpcTimeTrackRMS->GetYaxis()->SetTitle("Counts/(3.125ns)");

	
	  // average efficiency per layer : dqmf shifter histogram	 (under evaluation)  	    
	  std::string m_generic_path_LayerEff = m_generic_path_rpcmonitoring+"/Overview";
	  std::string m_LayerEff_title = "Layer_Efficiency";
	  const char* m_LayerEff_title_char = m_LayerEff_title.c_str();  

	  LayerEff=new TH1F(m_LayerEff_title_char,m_LayerEff_title_char,24,0,24);	    
	  sc = rpc_dqmf_global.regHist(LayerEff) ;  
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "LayerEff Failed to register histogram " );       
	      return sc;
	    }
	  LayerEff->SetFillColor(42);
	  LayerEff->GetXaxis()->SetTitle("LayerType(LPt0,LPt1,Piv0,Piv1,HPt0,HPt1) + ( Eta or Phi)*6 + (Small or Large)*12");
	  LayerEff->GetXaxis()->SetTitleSize(0.03) ;
	  LayerEff->GetXaxis()->SetTitleOffset(1.2);
	  LayerEff->GetYaxis()->SetTitle("Average Layer Efficiency"); 
	  LayerEff->SetMarkerStyle(20)      ;
	  LayerEff->SetMarkerColor(2)       ;
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << LayerEff << m_generic_path_LayerEff.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );	       
	    ATH_MSG_DEBUG ( "Booked bookLayerEff successfully" );} 
       

	  //Summary Distribution Booking Plots 
	  ATH_MSG_DEBUG ( "RPCStandaloneTracksMon HistogramsSummaryDistribution being booked" );
	  bookHistogramsSummaryDistribution(   );
 
	  if ( m_doRadiography ) {
	    for (int isec=0; isec!=16; isec++ ) {
	      bookRPCLayerRadiographyHistograms(isec, "Pivot0"  ) ;
	      bookRPCLayerRadiographyHistograms(isec, "Pivot1"  ) ;
	      bookRPCLayerRadiographyHistograms(isec, "LowPt0"  ) ;
	      bookRPCLayerRadiographyHistograms(isec, "LowPt1"  ) ;
	      bookRPCLayerRadiographyHistograms(isec, "HighPt0" ) ;
	      bookRPCLayerRadiographyHistograms(isec, "HighPt1" ) ;
	    }
	  }
	
	  // CS hit on track
	  const char* m_rpcCS_HitOnTrack_title_char = "CS_HitOnTrack_Distribution";
	  TH1 *rpcCS_HitOnTrack=new TH1I(m_rpcCS_HitOnTrack_title_char, m_rpcCS_HitOnTrack_title_char, 20, 0.5, 20.5);
	  sc=rpcprd_shift.regHist(rpcCS_HitOnTrack) ;  
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "CS_HitOnTrack_Distribution Failed to register histogram " );	    
	      return sc;
	    }					
	  rpcCS_HitOnTrack->SetFillColor(42);
	  rpcCS_HitOnTrack->GetXaxis()->SetTitle("Cluster Size on correlated noise ");
	  rpcCS_HitOnTrack->GetYaxis()->SetTitle("Counts");      
	
	  // CS of noise hit distribution
	  const char* m_rpcCS_NoiseCorr_title_char = "CS_NoiseCorr_Distribution";
	  TH1 *rpcCS_NoiseCorr=new TH1I(m_rpcCS_NoiseCorr_title_char, m_rpcCS_NoiseCorr_title_char, 20, 0.5, 20.5);
	  sc=rpcprd_shift.regHist(rpcCS_NoiseCorr) ;  
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "CS_NoiseCorr_Distribution Failed to register histogram " );	    
	      return sc;
	    }					
	  rpcCS_NoiseCorr->SetFillColor(42);
	  rpcCS_NoiseCorr->GetXaxis()->SetTitle("Cluster Size on correlated noise ");
	  rpcCS_NoiseCorr->GetYaxis()->SetTitle("Counts");      
	
	  const char* m_rpcCS_NoiseNotCorr_title_char = "CS_NoiseNotCorr_Distribution";
	  TH1 *rpcCS_NoiseNotCorr=new TH1I(m_rpcCS_NoiseNotCorr_title_char, m_rpcCS_NoiseNotCorr_title_char, 20, 0.5, 20.5);
	  sc=rpcprd_shift.regHist(rpcCS_NoiseNotCorr) ;  
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "CS_NoiseNotCorr_Distribution Failed to register histogram " );	    
	      return sc;
	    }					
	  rpcCS_NoiseNotCorr->SetFillColor(42);
	  rpcCS_NoiseNotCorr->GetXaxis()->SetTitle("Cluster Size on correlated noise ");
	  rpcCS_NoiseNotCorr->GetYaxis()->SetTitle("Counts");      
	
	  // CS vs angle track 
	  const char* m_rpcCS_EtavsPhi_title_char = "CS_EtavsPhi";
	  TH1 *m_rpcCS_EtavsPhi=new TH2I(m_rpcCS_EtavsPhi_title_char, m_rpcCS_EtavsPhi_title_char, 20, 0.5, 20.5, 20, 0.5, 20.5);
	  sc=rpcprd_shift.regHist( m_rpcCS_EtavsPhi ) ;  
	  if(sc.isFailure())
	    { 
	      ATH_MSG_FATAL ( "CS_EtavsPhi Failed to register histogram " );	    
	      return sc;
	    }
	  m_rpcCS_EtavsPhi->GetXaxis()->SetTitle("CS Phi");     
	  m_rpcCS_EtavsPhi->GetYaxis()->SetTitle("CS Eta");
          
	
	  // CS vs angle track 
	  const char* m_rpcCS_angleLong_title_char = "CSvsLongitudinalAngleTrack";
	  TH1 *m_rpcCS_angleLong=new TH2I(m_rpcCS_angleLong_title_char, m_rpcCS_angleLong_title_char, 50, 0, 1, 20, 0.5, 20.5);
	  sc=rpcprd_shift.regHist( m_rpcCS_angleLong ) ;  
	  if(sc.isFailure())
	    { 
	      ATH_MSG_FATAL ( "CSvsLongitudinalAngleTrack Failed to register histogram " );	    
	      return sc;
	    }
	  m_rpcCS_angleLong->GetXaxis()->SetTitle("Longitudinal Incidence angle of tracks [degrees]");     
	  m_rpcCS_angleLong->GetYaxis()->SetTitle("Cluster Size");
          	
	  // CS vs angle track
	  const char* m_rpcCS_angleTrasv_title_char = "CSvsTrasversalAngleTrack";
	  TH1 *m_rpcCS_angleTrasv=new TH2I(m_rpcCS_angleTrasv_title_char, m_rpcCS_angleTrasv_title_char, 50, 0, 1, 20, 0.5, 20.5);
	  sc=rpcprd_shift.regHist( m_rpcCS_angleTrasv ) ;  
	  if(sc.isFailure())
	    { 
	      ATH_MSG_FATAL ( "CSvsTrasversalAngleTrack Failed to register histogram " );	    
	      return sc;
	    }
	  m_rpcCS_angleTrasv->GetXaxis()->SetTitle("trasversal Incidence angle of tracks [degrees]");  
	  m_rpcCS_angleTrasv->GetYaxis()->SetTitle("Cluster Size");
             
	  //Residual for CS
	  for ( int iCS=1; iCS!=maxCSres+1; iCS++ ) {
	         
	    char m_ResidualVsCS_title_char[100] = "ResidualVsCS";
	    sprintf(m_ResidualVsCS_title_char , "ResidualVsCS%d", iCS) ;  
	  
	    TH1 *m_ResidualVsCS=new TH1I(m_ResidualVsCS_title_char, m_ResidualVsCS_title_char, 2*(50+iCS*30) , -50-iCS*30, 50+iCS*30);
	    sc=rpcprd_shift.regHist( m_ResidualVsCS ) ;  
	    if(sc.isFailure())
	      { 
		ATH_MSG_FATAL ( "ResidualVsCS Failed to register histogram " );	    
		return sc;
	      }
	    m_ResidualVsCS->GetXaxis()->SetTitle("Residual [mm]");  
	    m_ResidualVsCS->GetYaxis()->SetTitle("Counts/[mm]");	
	 
	  }
         
	
	  if ( m_doCoolDB ) {
	    DB_NotNorm_list.push_back( "TrackProj" 			);
	    DB_NotNorm_list.push_back( "HitOnTrack"  			);
	    DB_NotNorm_list.push_back( "HitOnTrack_withCrossStrip"  	);
	    DB_NotNorm_list.push_back( "NoiseCorr_NotNorm"  		);
	    DB_NotNorm_list.push_back( "NoiseTot_NotNorm"  		);
	    DB_NotNorm_list.push_back( "Res_CS1_NotNorm"  		);
	    DB_NotNorm_list.push_back( "Res_CS1_entries"  		);
	    DB_NotNorm_list.push_back( "Res_CS1_square"  		        );
	    DB_NotNorm_list.push_back( "Res_CS2_NotNorm"  		);
	    DB_NotNorm_list.push_back( "Res_CS2_entries"  		);
	    DB_NotNorm_list.push_back( "Res_CS2_square"  		        );
	    DB_NotNorm_list.push_back( "Res_CSmore2_NotNorm"  		);
	    DB_NotNorm_list.push_back( "Res_CSmore2_entries"  		);
	    DB_NotNorm_list.push_back( "Res_CSmore2_square"  		);
	    DB_NotNorm_list.push_back( "Occupancy_NotNorm"  		);
	    DB_NotNorm_list.push_back( "CS_NotNorm"  			);
	    DB_NotNorm_list.push_back( "CS_entries"  			);
	    DB_NotNorm_list.push_back( "CS_square"  			);
	    DB_NotNorm_list.push_back( "Time_NotNorm"  			);
	    DB_NotNorm_list.push_back( "Time_square"  			);
	
	    for ( std::vector<std::string>::const_iterator iter=DB_NotNorm_list.begin(); iter!=DB_NotNorm_list.end(); iter++ ) {
	      for ( int isec=0; isec!=16; isec++ ) {
		for ( int idblPhi=0; idblPhi!=2; idblPhi ++) {
		  bookRPCCoolHistograms_NotNorm( iter, isec, idblPhi, "Pivot0"  ) ;
		  bookRPCCoolHistograms_NotNorm( iter, isec, idblPhi, "Pivot1"  ) ;
		  bookRPCCoolHistograms_NotNorm( iter, isec, idblPhi, "LowPt0"  ) ;
		  bookRPCCoolHistograms_NotNorm( iter, isec, idblPhi, "LowPt1"  ) ;
		  bookRPCCoolHistograms_NotNorm( iter, isec, idblPhi, "HighPt0" ) ;
		  bookRPCCoolHistograms_NotNorm( iter, isec, idblPhi, "HighPt1" ) ;
		}
	      }
	    }
	    DB_list.push_back( "Efficiency"  	);
	    DB_list.push_back( "GapEfficiency"  	);
	    DB_list.push_back( "NoiseCorr"  	);
	    DB_list.push_back( "NoiseTot"  	);
	    DB_list.push_back( "Res_CS1"  	);
	    DB_list.push_back( "Res_CS2"  	);
	    DB_list.push_back( "Res_CSmore2"  	);
	    DB_list.push_back( "Occupancy"  	);
	    DB_list.push_back( "CS"  		);
	    DB_list.push_back( "Time"  		);
	    for ( std::vector<std::string>::const_iterator iter=DB_list.begin(); iter!=DB_list.end(); iter++ ) {
	      for ( int isec=0; isec!=16; isec++ ) {
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
	  
	  
	  }
	
 
	} // end if isNewRun
    }}//m_doRpcESD // AthenaMonManager::tier0 || AthenaMonManager::tier0ESD  
  return sc;
}

/*----------------------------------------------------------------------------------*/
void RPCStandaloneTracksMon::bookHistogramsSummaryDistribution(  )
/*----------------------------------------------------------------------------------*/
{
  
  ATH_MSG_DEBUG ( "RPCStandaloneTracksMon HistogramsSummaryDistribution being booked" );
  		    
  StatusCode sc = StatusCode::SUCCESS;
  
  //declare a group of histograms
  std::string m_generic_path_rpcmonitoring = "Muon/MuonRawDataMonitoring/RPCStandAloneTrackMon";
     
  MonGroup rpcprd_dq_BA      ( this, m_generic_path_rpcmonitoring + "/RPCBA" , run, ATTRIB_UNMANAGED );
  MonGroup rpcprd_dq_BC      ( this, m_generic_path_rpcmonitoring + "/RPCBC" , run, ATTRIB_UNMANAGED );
		
  //1) distribution of average panel Efficiency
  std::string m_rpcAverageEff_C_title      = "Efficiency_Distribution_sideC"          ;
  const char* m_rpcAverageEff_C_title_char = m_rpcAverageEff_C_title.c_str();  

  rpcAverageSide_C.push_back( new TH1F(m_rpcAverageEff_C_title_char, m_rpcAverageEff_C_title_char, 110, -0.01, 1.09));	    
  sc=rpcprd_dq_BC.regHist(rpcAverageSide_C.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageEff_C Failed to register histogram " );       
      return ;
    }
  rpcAverageSide_C.back()->SetFillColor(42);
  rpcAverageSide_C.back()->GetXaxis()->SetTitle("Average Strip Panel Efficiency Side C");
  rpcAverageSide_C.back()->GetYaxis()->SetTitle("Counts/0.01"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " <<  m_rpcAverageEff_C_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageEff_C successfully" );}   
  
  std::string m_rpcAverageEff_A_title      = "Efficiency_Distribution_sideA"          ;
  const char* m_rpcAverageEff_A_title_char = m_rpcAverageEff_A_title.c_str();  
		
  rpcAverageSide_A.push_back( new TH1F(m_rpcAverageEff_A_title_char, m_rpcAverageEff_A_title_char, 110, -0.01, 1.09));	    
  sc=rpcprd_dq_BA.regHist(rpcAverageSide_A.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageEff_A Failed to register histogram " );       
      return;
    }
  rpcAverageSide_A.back()->SetFillColor(42);
  rpcAverageSide_A.back()->GetXaxis()->SetTitle("Average Strip Panel Efficiency Side A");
  rpcAverageSide_A.back()->GetYaxis()->SetTitle("Counts/0.01"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << m_rpcAverageEff_A_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageEff_A successfully" );}           

  //2) distribution of average panel GapEfficiency
  std::string m_rpcAverageGapEff_C_title      = "GapEfficiency_Distribution_sideC"          ;
  const char* m_rpcAverageGapEff_C_title_char = m_rpcAverageGapEff_C_title.c_str();  

  rpcAverageSide_C.push_back( new TH1F(m_rpcAverageGapEff_C_title_char, m_rpcAverageGapEff_C_title_char, 110, -0.01, 1.09));	    
  sc=rpcprd_dq_BC.regHist(rpcAverageSide_C.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageGasGapEff_C Failed to register histogram " );       
      return;
    }
  rpcAverageSide_C.back()->SetFillColor(42);
  rpcAverageSide_C.back()->GetXaxis()->SetTitle("Average Strip Panel GapEfficiency Side C");
  rpcAverageSide_C.back()->GetYaxis()->SetTitle("Counts/0.01"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << m_rpcAverageGapEff_C_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageGapEff_C successfully" );}
   		     	
  std::string m_rpcAverageGapEff_A_title      = "GapEfficiency_Distribution_sideA"          ;
  const char* m_rpcAverageGapEff_A_title_char = m_rpcAverageGapEff_A_title.c_str();  

  rpcAverageSide_A.push_back( new TH1F(m_rpcAverageGapEff_A_title_char, m_rpcAverageGapEff_A_title_char, 110, -0.01, 1.09));	    
  sc=rpcprd_dq_BA.regHist(rpcAverageSide_A.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageGasGapEfficiency Failed to register histogram " );       
      return;
    }
  rpcAverageSide_A.back()->SetFillColor(42);
  rpcAverageSide_A.back()->GetXaxis()->SetTitle("Average Strip Panel GapEfficiency Side A");
  rpcAverageSide_A.back()->GetYaxis()->SetTitle("Counts/0.01"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << m_rpcAverageGapEff_A_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageGapEff_A successfully" );}         
    
  //3) distribution of average panel NoiseCorr
  std::string m_rpcAverageNoiseCorr_C_title      = "NoiseCorr_Distribution_sideC"          ;
  const char* m_rpcAverageNoiseCorr_C_title_char = m_rpcAverageNoiseCorr_C_title.c_str();  
  
  rpcAverageSide_C.push_back( new TH1F(m_rpcAverageNoiseCorr_C_title_char, m_rpcAverageNoiseCorr_C_title_char, 1000, 0, 100));	    
  sc=rpcprd_dq_BC.regHist(rpcAverageSide_C.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageNoiseCorr_Ciciency Failed to register histogram " );       
      return;
    }
  rpcAverageSide_C.back()->SetFillColor(42);
  rpcAverageSide_C.back()->GetXaxis()->SetTitle("Average Strip Panel NoiseCorr Side C");
  rpcAverageSide_C.back()->GetYaxis()->SetTitle("Counts/0.1/Hz/cm2"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : "  << m_rpcAverageNoiseCorr_C_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageNoiseCorr_C successfully" );}

  std::string m_rpcAverageNoiseCorr_A_title      = "NoiseCorr_Distribution_sideA"          ;
  const char* m_rpcAverageNoiseCorr_A_title_char = m_rpcAverageNoiseCorr_A_title.c_str();  

  rpcAverageSide_A.push_back( new TH1F(m_rpcAverageNoiseCorr_A_title_char, m_rpcAverageNoiseCorr_A_title_char, 1000, 0, 100));	    
  sc=rpcprd_dq_BA.regHist(rpcAverageSide_A.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageNoiseCorr_A Failed to register histogram " );       
      return;
    }
  rpcAverageSide_A.back()->SetFillColor(42);
  rpcAverageSide_A.back()->GetXaxis()->SetTitle("Average Strip Panel NoiseCorr Side A");
  rpcAverageSide_A.back()->GetYaxis()->SetTitle("Counts/0.1/Hz/cm2"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " <<  m_rpcAverageNoiseCorr_A_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageNoiseCorr_A successfully" );}          
  
  
  //4) distribution of average panel NoiseTot
  std::string m_rpcAverageNoiseTot_C_title      = "NoiseTot_Distribution_sideC"          ;
  const char* m_rpcAverageNoiseTot_C_title_char = m_rpcAverageNoiseTot_C_title.c_str();  
 
  rpcAverageSide_C.push_back( new TH1F(m_rpcAverageNoiseTot_C_title_char, m_rpcAverageNoiseTot_C_title_char, 1000, 0, 100));	    
  sc=rpcprd_dq_BC.regHist(rpcAverageSide_C.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageNoiseTot_C Failed to register histogram " );       
      return;
    }
  rpcAverageSide_C.back()->SetFillColor(42);
  rpcAverageSide_C.back()->GetXaxis()->SetTitle("Average Strip Panel NoiseTot Side C");
  rpcAverageSide_C.back()->GetYaxis()->SetTitle("Counts/0.1/Hz/cm2"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " <<  m_rpcAverageNoiseTot_C_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageNoiseTot_C successfully" );}      
		
  std::string m_rpcAverageNoiseTot_A_title      = "NoiseTot_Distribution_sideA"          ;
  const char* m_rpcAverageNoiseTot_A_title_char = m_rpcAverageNoiseTot_A_title.c_str()   ;  

  rpcAverageSide_A.push_back( new TH1F(m_rpcAverageNoiseTot_A_title_char, m_rpcAverageNoiseTot_A_title_char, 1000, 0, 100));	    
  sc=rpcprd_dq_BA.regHist(rpcAverageSide_A.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageNoiseTot_A Failed to register histogram " );       
      return;
    }
  rpcAverageSide_A.back()->SetFillColor(42);
  rpcAverageSide_A.back()->GetXaxis()->SetTitle("Average Strip Panel NoiseTot Side A");
  rpcAverageSide_A.back()->GetYaxis()->SetTitle("Counts/0.1/Hz/cm2"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " <<  m_rpcAverageNoiseTot_A_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageNoiseTot_A successfully" );}            
   		     	

  //5)   distribution of average panel NoiseNotCorr
  std::string m_rpcAverageNoiseNotCorr_C_title      = "NoiseNotCorr_Distribution_sideC"          ;
  const char* m_rpcAverageNoiseNotCorr_C_title_char = m_rpcAverageNoiseNotCorr_C_title.c_str();  
 
  rpcAverageSide_C.push_back( new TH1F(m_rpcAverageNoiseNotCorr_C_title_char, m_rpcAverageNoiseNotCorr_C_title_char, 1000, 0, 100));	    
  sc=rpcprd_dq_BC.regHist(rpcAverageSide_C.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageNoiseNotCorr_C Failed to register histogram " );       
      return;
    }
  rpcAverageSide_C.back()->SetFillColor(42);
  rpcAverageSide_C.back()->GetXaxis()->SetTitle("Average Strip Panel NoiseNotCorr Side C");
  rpcAverageSide_C.back()->GetYaxis()->SetTitle("Counts/0.1/Hz/cm2"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " <<  m_rpcAverageNoiseNotCorr_C_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageNoiseNotCorr_C successfully" );}      

  std::string m_rpcAverageNoiseNotCorr_A_title      = "NoiseNotCorr_Distribution_sideA"          ;
  const char* m_rpcAverageNoiseNotCorr_A_title_char = m_rpcAverageNoiseNotCorr_A_title.c_str();  

  rpcAverageSide_A.push_back( new TH1F(m_rpcAverageNoiseNotCorr_A_title_char, m_rpcAverageNoiseNotCorr_A_title_char, 1000, 0, 100));	    
  sc=rpcprd_dq_BA.regHist(rpcAverageSide_A.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageNoiseNotCorr_A Failed to register histogram " );       
      return;
    }
  rpcAverageSide_A.back()->SetFillColor(42);
  rpcAverageSide_A.back()->GetXaxis()->SetTitle("Average Strip Panel NoiseNotCorr Side A");
  rpcAverageSide_A.back()->GetYaxis()->SetTitle("Counts/0.1/Hz/cm2"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " <<  m_rpcAverageNoiseNotCorr_A_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageNoiseNotCorr_A successfully" );}      

  
  //5) distribution of average panel CS
  std::string m_rpcAverageCS_C_title      = "CS_Distribution_sideC"          ;
  const char* m_rpcAverageCS_C_title_char = m_rpcAverageCS_C_title.c_str();  
  
  rpcAverageSide_C.push_back( new TH1F(m_rpcAverageCS_C_title_char, m_rpcAverageCS_C_title_char, 200, 0.1, 19.9) );	    
  sc=rpcprd_dq_BC.regHist(rpcAverageSide_C.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageCS_C Failed to register histogram " );       
      return;
    }
  rpcAverageSide_C.back()->SetFillColor(42);
  rpcAverageSide_C.back()->GetXaxis()->SetTitle("Average Strip Panel CS Side C");
  rpcAverageSide_C.back()->GetYaxis()->SetTitle("Counts/0.1"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " <<  m_rpcAverageCS_C_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageCS_C successfully" );}  
		
  std::string m_rpcAverageCS_A_title      = "CS_Distribution_sideA"          ;
  const char* m_rpcAverageCS_A_title_char = m_rpcAverageCS_A_title.c_str();  
  
  rpcAverageSide_A.push_back( new TH1F(m_rpcAverageCS_A_title_char, m_rpcAverageCS_A_title_char, 200, 0.1, 19.9) );	    
  sc=rpcprd_dq_BA.regHist(rpcAverageSide_A.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageCS_A Failed to register histogram " );       
      return;
    }
  rpcAverageSide_A.back()->SetFillColor(42);
  rpcAverageSide_A.back()->GetXaxis()->SetTitle("Average Strip Panel CS Side A");
  rpcAverageSide_A.back()->GetYaxis()->SetTitle("Counts/0.1"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " <<  m_rpcAverageCS_A_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageCS_A successfully" );}  
  

  //6) distribution of average panel res_CS1
  std::string m_rpcAverageRes_CS1_C_title      = "Res_CS1_Distribution_sideC"          ;
  const char* m_rpcAverageRes_CS1_C_title_char = m_rpcAverageRes_CS1_C_title.c_str();  

  rpcAverageSide_C.push_back( new TH1F(m_rpcAverageRes_CS1_C_title_char, m_rpcAverageRes_CS1_C_title_char, 1000, -500, 500));	    
  sc=rpcprd_dq_BC.regHist(rpcAverageSide_C.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageRes_CS1_C Failed to register histogram " );       
      return;
    }
  rpcAverageSide_C.back()->SetFillColor(42);
  rpcAverageSide_C.back()->GetXaxis()->SetTitle("Average Strip Panel res_CS1 Side C [mm]");
  rpcAverageSide_C.back()->GetYaxis()->SetTitle("Counts/1mm"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " <<  m_rpcAverageRes_CS1_C_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageRes_CS1_C successfully" );} 

  std::string m_rpcAverageRes_CS1_A_title      = "Res_CS1_Distribution_sideA"          ;
  const char* m_rpcAverageRes_CS1_A_title_char = m_rpcAverageRes_CS1_A_title.c_str();  

  rpcAverageSide_A.push_back( new TH1F(m_rpcAverageRes_CS1_A_title_char, m_rpcAverageRes_CS1_A_title_char, 1000, -500, 500));	    
  sc=rpcprd_dq_BA.regHist(rpcAverageSide_A.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageRes_CS1_A Failed to register histogram " );       
      return;
    }
  rpcAverageSide_A.back()->SetFillColor(42);
  rpcAverageSide_A.back()->GetXaxis()->SetTitle("Average Strip Panel res_CS1 Side A");
  rpcAverageSide_A.back()->GetYaxis()->SetTitle("Counts/1mm"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " <<  m_rpcAverageRes_CS1_A_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageRes_CS1_A successfully" );}        
		
  //7) distribution of average panel res_CS2
  std::string m_rpcAverageRes_CS2_C_title      = "Res_CS2_Distribution_sideC"          ;
  const char* m_rpcAverageRes_CS2_C_title_char = m_rpcAverageRes_CS2_C_title.c_str();  

  rpcAverageSide_C.push_back( new TH1F(m_rpcAverageRes_CS2_C_title_char, m_rpcAverageRes_CS2_C_title_char, 1000, -500, 500));	    
  sc=rpcprd_dq_BC.regHist(rpcAverageSide_C.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageRes_CS2_C Failed to register histogram " );       
      return;
    }
  rpcAverageSide_C.back()->SetFillColor(42);
  rpcAverageSide_C.back()->GetXaxis()->SetTitle("Average Strip Panel res_CS2 Side C [mm]");
  rpcAverageSide_C.back()->GetYaxis()->SetTitle("Counts/1mm"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << m_rpcAverageRes_CS2_C_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageRes_CS2_C successfully" );}         

  std::string m_rpcAverageRes_CS2_A_title      = "Res_CS2_Distribution_sideA"          ;
  const char* m_rpcAverageRes_CS2_A_title_char = m_rpcAverageRes_CS2_A_title.c_str();  

  rpcAverageSide_A.push_back( new TH1F(m_rpcAverageRes_CS2_A_title_char, m_rpcAverageRes_CS2_A_title_char, 1000, -500, 500));	    
  sc=rpcprd_dq_BA.regHist(rpcAverageSide_A.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageRes_CS2_A Failed to register histogram " );       
      return;
    }
  rpcAverageSide_A.back()->SetFillColor(42);
  rpcAverageSide_A.back()->GetXaxis()->SetTitle("Average Strip Panel res_CS2 Side A [mm]");
  rpcAverageSide_A.back()->GetYaxis()->SetTitle("Counts/1mm"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " <<  m_rpcAverageRes_CS2_A_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageRes_CS2_A successfully" );}

  //8) distribution of average panel res_CSmore2
  std::string m_rpcAverageRes_CSmore2_C_title      = "Res_CSmore2_Distribution_sideC"          ;
  const char* m_rpcAverageRes_CSmore2_C_title_char = m_rpcAverageRes_CSmore2_C_title.c_str();  
  
  rpcAverageSide_C.push_back( new TH1F(m_rpcAverageRes_CSmore2_C_title_char, m_rpcAverageRes_CSmore2_C_title_char, 1000, -500, 500));	    
  sc=rpcprd_dq_BC.regHist(rpcAverageSide_C.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageRes_CSmore2_C Failed to register histogram " );       
      return;
    }
  rpcAverageSide_C.back()->SetFillColor(42);
  rpcAverageSide_C.back()->GetXaxis()->SetTitle("Average Strip Panel res_CSmore2 Side C [mm]");
  rpcAverageSide_C.back()->GetYaxis()->SetTitle("Counts/1mm"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << m_rpcAverageRes_CSmore2_C_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageRes_CSmore2_C successfully" );}         
   		    
  std::string m_rpcAverageRes_CSmore2_A_title      = "Res_CSmore2_Distribution_sideA"          ;
  const char* m_rpcAverageRes_CSmore2_A_title_char = m_rpcAverageRes_CSmore2_A_title.c_str();  
  
  rpcAverageSide_A.push_back( new TH1F(m_rpcAverageRes_CSmore2_A_title_char, m_rpcAverageRes_CSmore2_A_title_char, 1000, -500, 500));	    
  sc=rpcprd_dq_BA.regHist(rpcAverageSide_A.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageRes_CSmore2_A Failed to register histogram " );       
      return;
    }
  rpcAverageSide_A.back()->SetFillColor(42);
  rpcAverageSide_A.back()->GetXaxis()->SetTitle("Average Strip Panel res_CSmore2 Side A [mm]");
  rpcAverageSide_A.back()->GetYaxis()->SetTitle("Counts/1mm"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << m_rpcAverageRes_CSmore2_A_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageRes_CSmore2_A successfully" );} 

   		     	
  //9) distribution of average panel res_CS1rms
  std::string m_rpcAverageRes_CS1_rms_C_title      = "Res_CS1rms_Distribution_sideC"          ;
  const char* m_rpcAverageRes_CS1_rms_C_title_char = m_rpcAverageRes_CS1_rms_C_title.c_str();  

  rpcAverageSide_C.push_back( new TH1F(m_rpcAverageRes_CS1_rms_C_title_char, m_rpcAverageRes_CS1_rms_C_title_char, 1000, 0, 500));	    
  sc=rpcprd_dq_BC.regHist(rpcAverageSide_C.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageRes_CS1_rms_C Failed to register histogram " );       
      return;
    }
  rpcAverageSide_C.back()->SetFillColor(42);
  rpcAverageSide_C.back()->GetXaxis()->SetTitle("Average Strip Panel res_CS1rms Side C [mm]");
  rpcAverageSide_C.back()->GetYaxis()->SetTitle("Counts/1mm"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << m_rpcAverageRes_CS1_rms_C_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageRes_CS1_rms_C successfully" );}         
   		     	
  
  std::string m_rpcAverageRes_CS1_rms_A_title      = "Res_CS1rms_Distribution_sideA"          ;
  const char* m_rpcAverageRes_CS1_rms_A_title_char = m_rpcAverageRes_CS1_rms_A_title.c_str();  
  
  rpcAverageSide_A.push_back( new TH1F(m_rpcAverageRes_CS1_rms_A_title_char, m_rpcAverageRes_CS1_rms_A_title_char, 1000, 0, 500));	    
  sc=rpcprd_dq_BA.regHist(rpcAverageSide_A.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageRes_CS1_rms_A Failed to register histogram " );       
      return;
    }
  rpcAverageSide_A.back()->SetFillColor(42);
  rpcAverageSide_A.back()->GetXaxis()->SetTitle("Average Strip Panel res_CS1rms SideA");
  rpcAverageSide_A.back()->GetYaxis()->SetTitle("Counts/1mm"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << m_rpcAverageRes_CS1_rms_A_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageRes_CS1_rms_A successfully" );} 
	   
  //10) distribution of average panel res_CS2rms
  std::string m_rpcAverageRes_CS2_rms_C_title      = "Res_CS2rms_Distribution_sideC"          ;
  const char* m_rpcAverageRes_CS2_rms_C_title_char = m_rpcAverageRes_CS2_rms_C_title.c_str();  

  rpcAverageSide_C.push_back( new TH1F(m_rpcAverageRes_CS2_rms_C_title_char, m_rpcAverageRes_CS2_rms_C_title_char, 1000, 0, 500));	    
  sc=rpcprd_dq_BC.regHist(rpcAverageSide_C.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageRes_CS2_rms_C Failed to register histogram " );       
      return;
    }
  rpcAverageSide_C.back()->SetFillColor(42);
  rpcAverageSide_C.back()->GetXaxis()->SetTitle("Average Strip Panel res_CS2rms Side C [mm]");
  rpcAverageSide_C.back()->GetYaxis()->SetTitle("Counts/1mm"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << m_rpcAverageRes_CS2_rms_C_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageRes_CS2_rms_C successfully" );}         
   		     	

  std::string m_rpcAverageRes_CS2_rms_A_title      = "Res_CS2rms_Distribution_sideA"          ;
  const char* m_rpcAverageRes_CS2_rms_A_title_char = m_rpcAverageRes_CS2_rms_A_title.c_str();  

  rpcAverageSide_A.push_back( new TH1F(m_rpcAverageRes_CS2_rms_A_title_char, m_rpcAverageRes_CS2_rms_A_title_char, 1000, 0, 500));	    
  sc=rpcprd_dq_BA.regHist(rpcAverageSide_A.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageRes_CS2_rms_A Failed to register histogram " );       
      return;
    }
  rpcAverageSide_A.back()->SetFillColor(42);
  rpcAverageSide_A.back()->GetXaxis()->SetTitle("Average Strip Panel res_CS2rms Side A [mm]");
  rpcAverageSide_A.back()->GetYaxis()->SetTitle("Counts/1mm"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " <<  m_rpcAverageRes_CS2_rms_A_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageRes_CS2_rms_A successfully" );} 

  //11) distribution of average panel res_CSmore2rms
  std::string m_rpcAverageRes_CSmore2_rms_C_title      = "Res_CSmore2rms_Distribution_sideC"          ;
  const char* m_rpcAverageRes_CSmore2_rms_C_title_char = m_rpcAverageRes_CSmore2_rms_C_title.c_str();  
 
  rpcAverageSide_C.push_back( new TH1F(m_rpcAverageRes_CSmore2_rms_C_title_char, m_rpcAverageRes_CSmore2_rms_C_title_char, 1000, 0, 500));	    
  sc=rpcprd_dq_BC.regHist(rpcAverageSide_C.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageRes_CSmore2_rms_C Failed to register histogram " );       
      return;
    }
  rpcAverageSide_C.back()->SetFillColor(42);
  rpcAverageSide_C.back()->GetXaxis()->SetTitle("Average Strip Panel res_CSmore2rms Side C [mm]");
  rpcAverageSide_C.back()->GetYaxis()->SetTitle("Counts/1mm"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << m_rpcAverageRes_CSmore2_rms_C_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageRes_CSmore2_rms_C successfully" );} 
 
 
  std::string m_rpcAverageRes_CSmore2_rms_A_title      = "Res_CSmore2rms_Distribution_sideA"          ;
  const char* m_rpcAverageRes_CSmore2_rms_A_title_char = m_rpcAverageRes_CSmore2_rms_A_title.c_str();  
    
  rpcAverageSide_A.push_back( new TH1F(m_rpcAverageRes_CSmore2_rms_A_title_char, m_rpcAverageRes_CSmore2_rms_A_title_char, 1000, 0, 500));	    
  sc=rpcprd_dq_BA.regHist(rpcAverageSide_A.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageRes_CSmore2_rms_A Failed to register histogram " );       
      return;
    }
  rpcAverageSide_A.back()->SetFillColor(42);
  rpcAverageSide_A.back()->GetXaxis()->SetTitle("Average Strip Panel res_CSmore2rms Side A [mm]");
  rpcAverageSide_A.back()->GetYaxis()->SetTitle("Counts/1mm"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << m_rpcAverageRes_CSmore2_rms_A_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageRes_CSmore2_rms_A successfully" );} 
    
  //12) distribution of average panel Occupancy
  std::string m_generic_path_rpcAverageOccupancy_C = m_generic_path_rpcmonitoring+"/GLOBAL";
  std::string m_rpcAverageOccupancy_C_title      = "Occupancy_Distribution_sideC"          ;
  const char* m_rpcAverageOccupancy_C_title_char = m_rpcAverageOccupancy_C_title.c_str();  
  
  rpcAverageSide_C.push_back( new TH1F(m_rpcAverageOccupancy_C_title_char, m_rpcAverageOccupancy_C_title_char, 100, -10, 2));	    
  sc=rpcprd_dq_BC.regHist(rpcAverageSide_C.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpc_Occupancy_distribution Failed to register histogram " );       
      return;
    }
  rpcAverageSide_C.back()->SetFillColor(42);
  rpcAverageSide_C.back()->GetXaxis()->SetTitle("log10(Average Strip Panel Occupancy Side C)");
  rpcAverageSide_C.back()->GetYaxis()->SetTitle("Counts/0.01"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << m_rpcAverageOccupancy_C_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageOccupancy_C distribution successfully" );}  

  std::string m_generic_path_rpcAverageOccupancy_A = m_generic_path_rpcmonitoring+"/GLOBAL";
  std::string m_rpcAverageOccupancy_A_title      = "Occupancy_Distribution_sideA"          ;
  const char* m_rpcAverageOccupancy_A_title_char = m_rpcAverageOccupancy_A_title.c_str();  
  
  rpcAverageSide_A.push_back( new TH1F(m_rpcAverageOccupancy_A_title_char, m_rpcAverageOccupancy_A_title_char, 100, -10, 2));	    
  sc=rpcprd_dq_BA.regHist(rpcAverageSide_A.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpc_Occupancy_distribution Failed to register histogram " );       
      return;
    }
  rpcAverageSide_A.back()->SetFillColor(42);
  rpcAverageSide_A.back()->GetXaxis()->SetTitle("log10(Average Strip Panel Occupancy Side A)");
  rpcAverageSide_A.back()->GetYaxis()->SetTitle("Counts/0.01"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " <<  m_rpcAverageOccupancy_A_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageOccupancy_A distribution successfully" );}
		
     
  //13) distribution of average panel Time
  std::string m_rpcAverageTime_C_title      = "Time_Distribution_sideC"          ;
  const char* m_rpcAverageTime_C_title_char = m_rpcAverageTime_C_title.c_str();  
 
  rpcAverageSide_C.push_back( new TH1F(m_rpcAverageTime_C_title_char, m_rpcAverageTime_C_title_char, 64, 0, 200));	    
  sc=rpcprd_dq_BC.regHist(rpcAverageSide_C.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageTime_C Failed to register histogram " );       
      return;
    }
  rpcAverageSide_C.back()->SetFillColor(42);
  rpcAverageSide_C.back()->GetXaxis()->SetTitle("Average Strip Panel Time Side C");
  rpcAverageSide_C.back()->GetYaxis()->SetTitle("Counts/3.125ns"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << m_rpcAverageTime_C_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageTime_C successfully" );}      
    
  std::string m_rpcAverageTime_A_title      = "Time_Distribution_sideA"          ;
  const char* m_rpcAverageTime_A_title_char = m_rpcAverageTime_A_title.c_str();  
    
  rpcAverageSide_A.push_back( new TH1F(m_rpcAverageTime_A_title_char, m_rpcAverageTime_A_title_char, 64, 0, 200));	    
  sc=rpcprd_dq_BA.regHist(rpcAverageSide_A.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageTime_A Failed to register histogram " );       
      return;
    }
  rpcAverageSide_A.back()->SetFillColor(42);
  rpcAverageSide_A.back()->GetXaxis()->SetTitle("Average Strip Panel Time Side A");
  rpcAverageSide_A.back()->GetYaxis()->SetTitle("Counts/3.125ns"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << m_rpcAverageTime_A_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageTime_A successfully" );}      
    
}
    
// Radiography histograms
void RPCStandaloneTracksMon::bookRPCLayerRadiographyHistograms( int m_isec, std::string m_layer_name )
{

  ATH_MSG_DEBUG ( "bookRPCLayerRadiographyHistograms" );
  
  gErrorIgnoreLevel=kInfo;
  StatusCode sc = StatusCode::SUCCESS;      
  if(sc.isFailure() )  return ;
   
  if( m_doRpcESD==true ) {if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD ) { 
      sprintf(histName_char,"Sector%.2d", m_isec+1 ) ;
      std::string m_sector_name = histName_char      ;
      istatPhi = int( m_isec/2) + 1;
      iName    = 0              ;
      if ( m_isec!=11 &&  m_isec!=13) {
	// if ( m_layer.find("Pivot",0) )
	if ( m_layer_name == "Pivot0" || m_layer_name == "Pivot1" )   {
	  iName = 2 + (m_isec%2 ) ;
	  ir    = 2 	        ;
	}
	if ( m_layer_name == "LowPt0" || m_layer_name == "LowPt1" )   {
	  iName = 2 + (m_isec%2 ) ;
	  ir    = 1 	        ;
	}
	if ( m_layer_name == "HighPt0" || m_layer_name == "HighPt1" ) {
	  iName = 4 + (m_isec%2 ) ;
	  ir    = 1 	        ;
	}
      }
      else {
	if ( m_layer_name == "Pivot0" || m_layer_name == "Pivot1" )   {
	  iName = 8 ;
	  ir    = 2 ;
	}
	if ( m_layer_name == "LowPt0" || m_layer_name == "LowPt1" )   {
	  iName = 8 ;
	  ir    = 1 ;
	}
	if ( m_layer_name == "HighPt0" || m_layer_name == "HighPt1" ) {
	  iName = 9 ; // or 10 ;
	  ir    = 1 ; // doubletR=2 -> upgrade of Atlas
	}   
      }  
      NEtaTotStripsSideC = 0; NEtaTotStripsSideA = 0; NPhiTotStrips=0;

      Identifier rpcId = m_rpcIdHelper->channelID(iName, 1, istatPhi, ir, 1, 1, 1, 1, 1); // last 5 arguments are: int doubletZ, int doubletPhi, int gasGap, int measuresPhi, int strip
      if (!rpcId.is_valid()) {
        ATH_MSG_WARNING("Could not get valid Identifier for stationName="<<iName<<", eta=1, phi="<<istatPhi<<", doubletR="<<ir);
      }
      const MuonGM::RpcReadoutElement* rpc = m_muonMgr->getRpcReadoutElement(rpcId); 
      if(rpc){
	Identifier idr = rpc->identify();
	std::vector<int>   rpcstripshift = RpcGM::RpcStripShift(m_muonMgr,m_rpcIdHelper,idr, 0)  ;
	NphiStrips         =  rpcstripshift[0] ;
	NetaStripsTotSideA =  rpcstripshift[6] ;
	NetaStripsTotSideC =  rpcstripshift[7] ;
    
      }
    
      std::string m_generic_path_rpcmonitoring = "Muon/MuonRawDataMonitoring/RPCStandAloneTrackMon";
      MonGroup rpc_radiography(this, m_generic_path_rpcmonitoring + "/RPCRadiography", run, ATTRIB_UNMANAGED ) ;
    
      binz    = (NetaStripsTotSideA+NetaStripsTotSideC)/m_rpcreducenbinsstrip ;
      binminz = - NetaStripsTotSideC ;
      binmaxz =	NetaStripsTotSideA ;
      binx    = NphiStrips/m_rpcreducenbinsstrip ;
      binminx = 0 ;
      binmaxx = NphiStrips ;
   
 
      std::string m_rpcSectorLayerProj_title	= m_sector_name + "_" + m_layer_name + "_TrackProj" ;
      const char* m_rpcSectorLayerProj_title_char = m_rpcSectorLayerProj_title.c_str();
    
      TH2 * rpcSectorLayerTrackProj = new TH2I( m_rpcSectorLayerProj_title_char, m_rpcSectorLayerProj_title_char, binz, binminz, binmaxz, binx, binminx, binmaxx);
    
      sc = rpc_radiography.regHist( rpcSectorLayerTrackProj );
      if(sc.isFailure() ){  
       ATH_MSG_WARNING ( "couldn't register " << m_rpcSectorLayerProj_title << " hist to MonGroup" );
       return ;
      }
    
      rpcSectorLayerTrackProj->SetOption("COLZ");
      rpcSectorLayerTrackProj->GetXaxis()->SetTitle("<--- SIDE C      Rpc Eta strip      SIDE A --->");
      if ( (m_isec%2)== 0 ) { rpcSectorLayerTrackProj->GetYaxis()->SetTitle("<--- HV side     Rpc Phi strip     RO side --->"); }  // large
      else		  { rpcSectorLayerTrackProj->GetYaxis()->SetTitle("<--- RO side     Rpc Phi strip     HV side --->"); }
    
    
      std::string m_rpcSectorLayerResponse_title      = m_sector_name + "_" + m_layer_name + "_Response" ;
      const char* m_rpcSectorLayerResponse_title_char = m_rpcSectorLayerResponse_title.c_str();
 
      TH2 * rpcSectorLayerResponse = new TH2I( m_rpcSectorLayerResponse_title_char, m_rpcSectorLayerResponse_title_char ,binz, binminz, binmaxz, binx, binminx, binmaxx);
 
      sc = rpc_radiography.regHist( rpcSectorLayerResponse );
      if(sc.isFailure() )  
      {  
       ATH_MSG_WARNING ( "couldn't register " << m_rpcSectorLayerResponse_title << " hist to MonGroup" );
       return ;
      }
      rpcSectorLayerResponse->SetOption("COLZ");
      rpcSectorLayerResponse->GetXaxis()->SetTitle("<--- SIDE C	   Rpc Eta strip      SIDE A --->");
      if ( (m_isec%2) == 0) { rpcSectorLayerResponse->GetYaxis()->SetTitle("<--- HV side     Rpc Phi strip     RO side --->"); }  // large
      else	          { rpcSectorLayerResponse->GetYaxis()->SetTitle("<--- RO side     Rpc Phi strip     HV side --->"); }
    
      // eta OR phi response
      std::string m_rpcSectorLayerResponseOR_title      = m_sector_name + "_" + m_layer_name + "_ResponseEtaOrPhi" ;
      const char* m_rpcSectorLayerResponseOR_title_char = m_rpcSectorLayerResponseOR_title.c_str();
 
      TH2 * rpcSectorLayerResponseOR = new TH2I( m_rpcSectorLayerResponseOR_title_char, m_rpcSectorLayerResponseOR_title_char ,binz, binminz, binmaxz, binx, binminx, binmaxx);
 
      sc = rpc_radiography.regHist( rpcSectorLayerResponseOR );
      if(sc.isFailure() ) 
      { 
       ATH_MSG_WARNING ( "couldn't register " << m_rpcSectorLayerResponseOR_title << " hist to MonGroup" );
       return ;
      }
      
      
      rpcSectorLayerResponseOR->SetOption("COLZ");
      rpcSectorLayerResponseOR->GetXaxis()->SetTitle("<--- SIDE C	   Rpc Eta strip      SIDE A --->");
      if ( (m_isec%2) == 0) { rpcSectorLayerResponseOR->GetYaxis()->SetTitle("<--- HV side     Rpc Phi strip     RO side --->"); }  // large
      else	          { rpcSectorLayerResponse->GetYaxis()->SetTitle("<--- RO side     Rpc Phi strip     HV side --->"); }
    
    
    
      //ATLAS noise 
      std::string m_rpcSectorLayerNoiseCorr_title      = m_sector_name + "_" + m_layer_name + "_NoiseCorr" ;
      const char* m_rpcSectorLayerNoiseCorr_title_char = m_rpcSectorLayerNoiseCorr_title.c_str();
   
      TH2 * rpcSectorLayerNoiseCorr = new TH2I( m_rpcSectorLayerNoiseCorr_title_char, m_rpcSectorLayerNoiseCorr_title_char ,binz, binminz, binmaxz, binx, binminx, binmaxx);
 
      sc = rpc_radiography.regHist( rpcSectorLayerNoiseCorr );
      if(sc.isFailure() )  
      {  
       ATH_MSG_WARNING ( "couldn't register " << m_rpcSectorLayerNoiseCorr_title << " hist to MonGroup" );
       return ;
      }
      
      rpcSectorLayerNoiseCorr->SetOption("COLZ");
      rpcSectorLayerNoiseCorr->GetXaxis()->SetTitle("<--- SIDE C	   Rpc Eta strip      SIDE A --->");
      if ( (m_isec%2) == 0) { rpcSectorLayerNoiseCorr->GetYaxis()->SetTitle("<--- HV side     Rpc Phi strip     RO side --->"); }  // large
      else	          { rpcSectorLayerNoiseCorr->GetYaxis()->SetTitle("<--- RO side     Rpc Phi strip     HV side --->"); }
    
    
      std::string m_rpcSectorLayerNoiseNotCorr_title      = m_sector_name + "_" + m_layer_name + "_NoiseNotCorr" ;
      const char* m_rpcSectorLayerNoiseNotCorr_title_char = m_rpcSectorLayerNoiseNotCorr_title.c_str();
   
      TH2 * rpcSectorLayerNoiseNotCorr = new TH2I( m_rpcSectorLayerNoiseNotCorr_title_char, m_rpcSectorLayerNoiseNotCorr_title_char ,binz, binminz, binmaxz, binx, binminx, binmaxx);
 
      sc = rpc_radiography.regHist( rpcSectorLayerNoiseNotCorr );
      if(sc.isFailure() )  
      {  
      ATH_MSG_WARNING ( "couldn't register " << m_rpcSectorLayerNoiseNotCorr_title << " hist to MonGroup" ); 
       return ;
      }
      rpcSectorLayerNoiseNotCorr->SetOption("COLZ");
      rpcSectorLayerNoiseNotCorr->GetXaxis()->SetTitle("<--- SIDE C	   Rpc Eta strip      SIDE A --->");
      if ( (m_isec%2) == 0) { rpcSectorLayerNoiseNotCorr->GetYaxis()->SetTitle("<--- HV side     Rpc Phi strip     RO side --->"); }  // large
      else	          { rpcSectorLayerNoiseNotCorr->GetYaxis()->SetTitle("<--- RO side     Rpc Phi strip     HV side --->"); }
    
    
    }}  
  
}

// BOOK COOL STRIP HISTOGRAMSS
void RPCStandaloneTracksMon::bookRPCCoolHistograms_NotNorm( std::vector<std::string>::const_iterator & m_iter, int m_isec, int m_idblPhi, std::string m_layer ) 
{
  gErrorIgnoreLevel=kInfo;
  ATH_MSG_DEBUG ( "RPCStandaloneTracksMon: bookRPCCoolHistograms_NotNorm" );
  
  StatusCode sc = StatusCode::SUCCESS ;
  if(sc.isFailure() )  
  {
       return ;
  }
  std::string m_generic_path_rpcmonitoring = "Muon/MuonRawDataMonitoring/RPCStandAloneTrackMon";
  MonGroup rpcCoolDb( this, m_generic_path_rpcmonitoring+"/CoolDB", run, ATTRIB_UNMANAGED );
  
  sprintf(histName_char,"Sector%.2d_%s_dblPhi%d", m_isec+1, m_layer.c_str(), m_idblPhi+1) ;
  // example: Sector01_Pivot0_dblPhi1_StripId
  
  histName  = histName_char  ;
  histName += "_"            ;
  histName += *m_iter        ;  //histName += m_coolQuantity ;
  istatPhi  = int( m_isec/2) ;
  iName     = 0              ; 
  
  //BML7(dr=1) is associated to LowPt and not Pivot
  if ( m_isec<11 ||  m_isec>13) {
    // if ( m_layer.find("Pivot",0) )
    if ( m_layer == "Pivot0" || m_layer == "Pivot1" )   {
      iName = 2 + (m_isec%2 ) ;
      ir    = 2 	      ; 
    }
    if ( m_layer == "LowPt0" || m_layer == "LowPt1" )   {
      iName = 2 + (m_isec%2 ) ;
      ir    = 1 	      ; 
    }
    if ( m_layer == "HighPt0" || m_layer == "HighPt1" ) {
      iName = 4 + (m_isec%2 ) ;
      ir    = 1   ;
    }
  }  
  else if ( m_isec==12) {
    // if ( m_layer.find("Pivot",0) )
    if ( m_layer == "Pivot0" || m_layer == "Pivot1" )   {
      iName     =  1      ; 
      ir    = 2 	      ;	 
    }
    if ( m_layer == "LowPt0" || m_layer == "LowPt1" )   {
      iName     =  1      ; 
      ir    = 1 	      ; 
    }
    if ( m_layer == "HighPt0" || m_layer == "HighPt1" ) {
      iName = 4               ;
      ir    = 1 	      ;   
    }
     
  }
  else {
    if ( m_layer == "Pivot0" || m_layer == "Pivot1" )   {
      iName     =   8  ; 
      ir    = 2 ;     
    }
    if ( m_layer == "LowPt0" || m_layer == "LowPt1" )   {
      iName = 8 ; 
      ir    = 1 ; 
    }
    if ( m_layer == "HighPt0" || m_layer == "HighPt1" ) {
      iName = 9 ; // or 10 ; 
      ir    = 1 ; // doubletR=2 -> upgrade of Atlas 
    }
  } // end sectors 12 and 14
  
  int NTotStripsSideA = 1;
  int NTotStripsSideC = 1;     
 
  int kName = iName ;
  if(kName==1)kName=53;//BMLE

  Identifier rpcId = m_rpcIdHelper->channelID(kName, 1 , istatPhi+1, ir, 1, m_idblPhi+1, 1, 1, 1); // last 3 arguments are: int gasGap, int measuresPhi, int strip
  if (!rpcId.is_valid()) {
    ATH_MSG_WARNING("Could not get valid Identifier for stationName="<<kName<<", eta=1, phi="<<istatPhi+1<<", doubletR="<<ir<<", doubletZ=1, doubletPhi="<<m_idblPhi+1);
  }
  Identifier rpcId_c = m_rpcIdHelper->channelID(kName, -1 , istatPhi+1, ir, 1, m_idblPhi+1, 1, 1, 1); // last 3 arguments are: int gasGap, int measuresPhi, int strip
  if (!rpcId_c.is_valid()) {
    ATH_MSG_WARNING("Could not get valid Identifier for stationName="<<kName<<", eta=-1, phi="<<istatPhi+1<<", doubletR="<<ir<<", doubletZ=1, doubletPhi="<<m_idblPhi+1);
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
  
 
  
}
  
  
void RPCStandaloneTracksMon::bookRPCCoolHistograms( std::vector<std::string>::const_iterator & m_iter, int m_isec, int m_idblPhi, std::string m_layer ) 
{ 
  /* book histograms with normalized quantities: this histograms are filled by RPC PostProcess		*/
  /* only metadata histograms are written by monitoring 						*/
  /* see:						                                                */
  /* https://twiki.cern.ch/twiki/bin/view/Atlas/DQOperationalRecipes#How_can_one_apply_algorithms_to    */
	  	 
  gErrorIgnoreLevel=kInfo;
  ATH_MSG_DEBUG ( "RPCStandaloneTracksMon: bookRPCCoolHistograms" );
	  												   
  StatusCode sc = StatusCode::SUCCESS ;
  if(sc.isFailure() )  
  {  
       return ;
  }
  std::string m_generic_path_rpcmonitoring = "Muon/MuonRawDataMonitoring/RPCStandAloneTrackMon";
  MonGroup rpcCoolDb( this, m_generic_path_rpcmonitoring+"/CoolDB", run, ATTRIB_UNMANAGED );
    
  sprintf(histName_char,"Sector%.2d_%s_dblPhi%d", m_isec+1, m_layer.c_str(), m_idblPhi+1) ;
  // example: Sector01_Pivot0_dblPhi1_StripId
  
  histName  = histName_char  ;
  histName += "_"            ;
  histName += *m_iter        ;  //histName += m_coolQuantity ;
  istatPhi  = int( m_isec/2) ;
  iName     = 0              ; 
  
  //BML7(dr=1) is associated to LowPt and not Pivot
  if ( m_isec<11 ||  m_isec>13) {
    // if ( m_layer.find("Pivot",0) )
    if ( m_layer == "Pivot0" || m_layer == "Pivot1" )   {
      iName = 2 + (m_isec%2 ) ;
      ir    = 2 	      ;	 
    }
    if ( m_layer == "LowPt0" || m_layer == "LowPt1" )   {
      iName = 2 + (m_isec%2 ) ;
      ir    = 1 	      ; 
    }
    if ( m_layer == "HighPt0" || m_layer == "HighPt1" ) {
      iName = 4 + (m_isec%2 ) ;
      ir    = 1 	      ; 
    } 
  }  
  else if ( m_isec==12) {
    // if ( m_layer.find("Pivot",0) )
    if ( m_layer == "Pivot0" || m_layer == "Pivot1" )   {
      iName     =  1      ; 
      ir    = 2 	      ;	 
    }
    if ( m_layer == "LowPt0" || m_layer == "LowPt1" )   {
      iName     =  1      ; 
      ir    = 1 	      ; 
    }
    if ( m_layer == "HighPt0" || m_layer == "HighPt1" ) {
      iName = 4               ;
      ir    = 1 	      ;  
    }
     
  }
  else {
    if ( m_layer == "Pivot0" || m_layer == "Pivot1" )   {
      iName     =   8  ; 
      ir    = 2 ;      
    }
    if ( m_layer == "LowPt0" || m_layer == "LowPt1" )   {
      iName = 8 ; 
      ir    = 1 ; 
    }
    if ( m_layer == "HighPt0" || m_layer == "HighPt1" ) {
      iName = 9 ; // or 10 ; 
      ir    = 1 ; // doubletR=2 -> upgrade of Atlas 
    }
  } // end sectors 12 and 14
  
  int NTotStripsSideA = 1;
  int NTotStripsSideC = 1;     
 
  int kName = iName ;
  if(kName==1)kName=53;//BMLE

  Identifier rpcId = m_rpcIdHelper->channelID(kName, 1 , istatPhi+1, ir, 1, m_idblPhi+1, 1, 1, 1); // last 3 arguments are: int gasGap, int measuresPhi, int strip
  if (!rpcId.is_valid()) {
    ATH_MSG_WARNING("Could not get valid Identifier for stationName="<<kName<<", eta=1, phi="<<istatPhi+1<<", doubletR="<<ir<<", doubletZ=1, doubletPhi="<<m_idblPhi+1);
  }
  Identifier rpcId_c = m_rpcIdHelper->channelID(kName, -1 , istatPhi+1, ir, 1, m_idblPhi+1, 1, 1, 1); // last 3 arguments are: int gasGap, int measuresPhi, int strip
  if (!rpcId_c.is_valid()) {
    ATH_MSG_WARNING("Could not get valid Identifier for stationName="<<kName<<", eta=-1, phi="<<istatPhi+1<<", doubletR="<<ir<<", doubletZ=1, doubletPhi="<<m_idblPhi+1);
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
    
  
  bool doRecursiveReferenceDelete = gROOT->MustClean();
  gROOT->SetMustClean(false);
  
  
  TDirectory* currentDir = gDirectory;
  TDirectory* targetDir = rpcCoolHisto->GetDirectory();
  if( targetDir != 0 ) {
    targetDir->cd();
  }
  rpcCoolHisto->Write();
  currentDir->cd();
  rpcCoolDb.deregHist( rpcCoolHisto ).ignore();
  delete rpcCoolHisto ;
  gROOT->SetMustClean(doRecursiveReferenceDelete);
  
}
  
void RPCStandaloneTracksMon::bookRPCTracksLayerHistograms(std::string m_hardware_name, std::string m_layer_name, std::string m_layer0_name, int bin, int binmin, int binmax )
{
  gErrorIgnoreLevel=kError;
  StatusCode sc = StatusCode::SUCCESS;
  if(sc.isFailure() )  
  {  
    return ;
  }
  if( m_doRpcESD==true ) {if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online ) {    		    
      //declare a group of histograms
      std::string m_generic_path_rpcmonitoring = "Muon/MuonRawDataMonitoring/RPCStandAloneTrackMon";
      MonGroup rpcprd_expert( this, m_generic_path_rpcmonitoring+"/Chambers/"+m_hardware_name+"/Tracks/", run, ATTRIB_UNMANAGED );   
      MuonDQAHistList& lst = m_stationHists.getList( m_hardware_name +"/Tracks/" + m_layer_name);    
  
  
      //histo path for rpc trackproj
      //
      std::string m_generic_path_rpctrackprojlayer = m_generic_path_rpcmonitoring+"/Chambers/"+m_hardware_name+"/Projection/";
      m_generic_path_rpctrackprojlayer += m_hardware_name + "_" + m_layer_name + "_trackproj";
      std::string m_rpctrackprojlayer_title = m_hardware_name + "_" + m_layer_name + "_trackproj";	
      const char* m_rpctrackprojlayer_title_char = m_rpctrackprojlayer_title.c_str();
  
      TH1 *rpctrackprojlayer=new TH1I(m_rpctrackprojlayer_title_char, m_rpctrackprojlayer_title_char, bin/m_rpcreducenbins, binmin, binmax ); 			
      lst.addHist(rpctrackprojlayer);  
      rpctrackprojlayer->SetFillColor(42);  
      //rpctrackprojlayer->SetOption("COLZ");  
      rpctrackprojlayer->GetXaxis()->SetTitle(m_layer0_name.c_str());
      rpctrackprojlayer->GetYaxis()->SetTitle("trackproj ");

      {
	ATH_MSG_DEBUG ( "INSIDE bookRPCLayerHistograms : " << rpctrackprojlayer << m_generic_path_rpctrackprojlayer.c_str() );
	//ATH_MSG_DEBUG ( "EXPERT : " << expert );
	ATH_MSG_DEBUG ( "RUN : " << run );}    
   		     	
      sc = rpcprd_expert.regHist( rpctrackprojlayer );
      if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpctrackprojlayer hist to MonGroup" ); 
        
      //histo path for rpc hitontrack
      //
      std::string m_generic_path_rpchitontracklayer = m_generic_path_rpcmonitoring+"/Chambers/"+m_hardware_name+"/Projection/";
      m_generic_path_rpchitontracklayer += m_hardware_name + "_" + m_layer_name + "_hitontrack";
      std::string m_rpchitontracklayer_title = m_hardware_name + "_" + m_layer_name + "_hitontrack";	
      const char* m_rpchitontracklayer_title_char = m_rpchitontracklayer_title.c_str();
		
      TH1 *rpchitontracklayer=new TH1I(m_rpchitontracklayer_title_char, m_rpchitontracklayer_title_char, bin/m_rpcreducenbins, binmin, binmax ); 			
      lst.addHist(rpchitontracklayer);  
      rpchitontracklayer->SetFillColor(42);  
      //rpchitontracklayer->SetOption("COLZ");  
      rpchitontracklayer->GetXaxis()->SetTitle(m_layer0_name.c_str());
      rpchitontracklayer->GetYaxis()->SetTitle("hitontrack ");

      {
	ATH_MSG_DEBUG ( "INSIDE bookRPCLayerHistograms : " << rpchitontracklayer << m_generic_path_rpchitontracklayer.c_str() );
	//ATH_MSG_DEBUG ( "EXPERT : " << expert );
	ATH_MSG_DEBUG ( "RUN : " << run );}    
   		     	
      sc = rpcprd_expert.regHist( rpchitontracklayer );
      if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpchitontracklayer hist to MonGroup" ); 
   
  
      //histo path for rpc residuals
      //
      std::string m_generic_path_rpcresidualslayer = m_generic_path_rpcmonitoring+"/Chambers/"+m_hardware_name+"/Tracks/";
      m_generic_path_rpcresidualslayer += m_hardware_name + "_" + m_layer_name + "_residuals";
      std::string m_rpcresidualslayer_title = m_hardware_name + "_" + m_layer_name + "_residuals";	
      const char* m_rpcresidualslayer_title_char = m_rpcresidualslayer_title.c_str();
		
      TH1 *rpcresidualslayer=new TH1F(m_rpcresidualslayer_title_char, m_rpcresidualslayer_title_char, 1000, -500, 500); 			
      lst.addHist(rpcresidualslayer);  
      rpcresidualslayer->SetFillColor(42);   
      rpcresidualslayer->GetXaxis()->SetTitle(m_layer0_name.c_str());
      rpcresidualslayer->GetYaxis()->SetTitle("Counts/Strip");

      {
	ATH_MSG_DEBUG ( "INSIDE bookRPCLayerHistograms : " << rpcresidualslayer << m_generic_path_rpcresidualslayer.c_str() );
	//ATH_MSG_DEBUG ( "EXPERT : " << expert );
	ATH_MSG_DEBUG ( "RUN : " << run );}    
   		     	
      sc = rpcprd_expert.regHist( rpcresidualslayer );
      if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpcresidualslayer hist to MonGroup" );  
         
	 
    
    }}//m_doRpcESD // AthenaMonManager::tier0 || AthenaMonManager::tier0ESD  
  
}


//void RPCStandaloneTracksMon::bookRPCSummaryHistograms( int m_i_sec, std::vector<std::string>::const_iterator & m_iter )
void RPCStandaloneTracksMon::bookRPCSummaryHistograms( int m_i_sec, const std::string & m_quantity)
{
  StatusCode sc = StatusCode::SUCCESS; 
  
  ATH_MSG_DEBUG ( "Start bookRPCSummaryHistograms: " << m_quantity ); 
  
  std::string m_generic_path_rpcmonitoring = "Muon/MuonRawDataMonitoring/RPCStandAloneTrackMon";
  MonGroup rpcprd_expert_sum( this, m_generic_path_rpcmonitoring +"/Summary", run, ATTRIB_UNMANAGED ) ;
   
  char sector_char[10]    ;
  std::string sector_name ;
  sprintf(sector_char, "Sector%.2d", m_i_sec+1) ;  // sector number with 2 digits
  sector_name = sector_char ;
	
  // large sectors: (i_sec+1)%2 = 1;  small (i_sec+1)%2 = 0
  n_eta_station = ( (m_i_sec+1) %2 ) + 6 + 1 ;  // small: 1-6, large: 1-7, sec 12+14: 0-6, sec 13: 1-7 (8->7)	  
  int n_bin = 72 * n_eta_station   ;   
  //  72 = 2*2*2*3*3  measPhi + gap + dblPhi + dblZ + stationName + stationEta
  std::string PanelIndexTitle = "# Panel = View + (gap-1)*2 + (dblPhi-1)*4 + (dblZ-1)*8 + (LowPt=0,Pivot=1,HighPt=2)*24 + (Eta)*72";
 
  std::string m_SummaryHist_title = "Summary" + m_quantity + "_" + sector_name ;
  const char* m_SummaryHist_title_char = m_SummaryHist_title.c_str(); 
  m_SummaryHist.push_back (new TH1F(m_SummaryHist_title_char, m_SummaryHist_title_char, 2*n_bin, -n_bin, n_bin));
  m_SummaryHist.back()->SetFillColor(2);
  m_SummaryHist.back()->GetXaxis()-> SetTitle(PanelIndexTitle.c_str() );
  m_SummaryHist.back()->GetXaxis()-> SetTitleSize(0.03)  	      ;
  m_SummaryHist.back()->GetXaxis()-> SetTitleOffset(1.2) 	      ;
  m_SummaryHist.back()->GetYaxis()-> SetTitle((m_quantity).c_str() )   ;
   
  sc = rpcprd_expert_sum.regHist( m_SummaryHist.back() ); 
  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register "<< m_SummaryHist_title <<" hist to MonGroup" );
  
 
  //
  
  MonGroup rpcprd_dq_BA      ( this, m_generic_path_rpcmonitoring + "/RPCBA" , run, ATTRIB_UNMANAGED );
  MonGroup rpcprd_dq_BC      ( this, m_generic_path_rpcmonitoring + "/RPCBC" , run, ATTRIB_UNMANAGED ); 
     
 
  if(m_quantity == "Time_NotNorm"){
  
    std::string m_SummaryHistRPCBA_title = "StripTimeDistribution_" + sector_name +"_SideA" ;
    const char* m_SummaryHistRPCBA_title_char = m_SummaryHistRPCBA_title.c_str();
  
    m_SummaryHistRPCBA.push_back( new TH1F(m_SummaryHistRPCBA_title_char, m_SummaryHistRPCBA_title_char, timeNbin, timeminrange, timemaxrange));
    m_SummaryHistRPCBA.back()->SetFillColor(2);
    m_SummaryHistRPCBA.back()->GetXaxis()-> SetTitle(PanelIndexTitle.c_str() );
    m_SummaryHistRPCBA.back()->GetXaxis()-> SetTitleSize(0.03)	        ;
    m_SummaryHistRPCBA.back()->GetXaxis()-> SetTitleOffset(1.2)	        ;
    m_SummaryHistRPCBA.back()->GetYaxis()-> SetTitle((m_quantity).c_str() )   ;
  
    sc = rpcprd_dq_BA.regHist( m_SummaryHistRPCBA.back() );
    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register "<< m_SummaryHistRPCBA_title <<" hist to MonGroup" ); 
   
    std::string m_SummaryHistRPCBC_title = "StripTimeDistribution_" + sector_name +"_SideC"  ;
    const char* m_SummaryHistRPCBC_title_char = m_SummaryHistRPCBC_title.c_str();
  
    m_SummaryHistRPCBC.push_back( new TH1F(m_SummaryHistRPCBC_title_char, m_SummaryHistRPCBC_title_char, timeNbin, timeminrange, timemaxrange));
    m_SummaryHistRPCBC.back()->SetFillColor(2);
    m_SummaryHistRPCBC.back()->GetXaxis()-> SetTitle(PanelIndexTitle.c_str() );
    m_SummaryHistRPCBC.back()->GetXaxis()-> SetTitleSize(0.03)	        ;
    m_SummaryHistRPCBC.back()->GetXaxis()-> SetTitleOffset(1.2)	        ;
    m_SummaryHistRPCBC.back()->GetYaxis()-> SetTitle((m_quantity).c_str() )   ;
  
    sc = rpcprd_dq_BC.regHist( m_SummaryHistRPCBC.back() );
    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register "<< m_SummaryHistRPCBC_title <<" hist to MonGroup" ); 
  
  }
     
 
  if(m_i_sec == 0 ){  
    std::string m_SummaryHistRPCBA_title = "Layer_" + m_quantity+"_sideA";
    const char* m_SummaryHistRPCBA_title_char = m_SummaryHistRPCBA_title.c_str();
  
    m_SummaryHistRPCBA.push_back(  new TH1F(m_SummaryHistRPCBA_title_char, m_SummaryHistRPCBA_title_char,24,0,24) );
    m_SummaryHistRPCBA.back()->SetFillColor(42);
    m_SummaryHistRPCBA.back()->GetXaxis()-> SetTitle( "LayerType(LPt0,LPt1,Piv0,Piv1,HPt0,HPt1) + ( Eta or Phi)*6 + (Small or Large)*12" );
    m_SummaryHistRPCBA.back()->GetXaxis()-> SetTitleSize(0.03)	         ;
    m_SummaryHistRPCBA.back()->GetXaxis()-> SetTitleOffset(1.2)	         ;
    m_SummaryHistRPCBA.back()->GetYaxis()-> SetTitle(("RPCBA_Layer_"+(m_quantity)).c_str() )   ;    
    m_SummaryHistRPCBA.back()->SetMarkerStyle(20)	  ;
    m_SummaryHistRPCBA.back()->SetMarkerColor(2)  ;
 
    sc = rpcprd_dq_BA.regHist( m_SummaryHistRPCBA.back() );
    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register "<< m_SummaryHistRPCBA_title <<" hist to MonGroup" ); 
     
    std::string m_SummaryHistRPCBC_title = "Layer_" + m_quantity+"_sideC";
    const char* m_SummaryHistRPCBC_title_char = m_SummaryHistRPCBC_title.c_str();
  
    m_SummaryHistRPCBC.push_back(  new TH1F(m_SummaryHistRPCBC_title_char, m_SummaryHistRPCBC_title_char,24,0,24) );
    m_SummaryHistRPCBC.back()->SetFillColor(42);
    m_SummaryHistRPCBC.back()->GetXaxis()-> SetTitle( "LayerType(LPt0,LPt1,Piv0,Piv1,HPt0,HPt1) + ( Eta or Phi)*6 + (Small or Large)*12" );
    m_SummaryHistRPCBC.back()->GetXaxis()-> SetTitleSize(0.03)	         ;
    m_SummaryHistRPCBC.back()->GetXaxis()-> SetTitleOffset(1.2)	         ;
    m_SummaryHistRPCBC.back()->GetYaxis()-> SetTitle(("RPCBC_Layer_"+(m_quantity)).c_str() )   ;       
    m_SummaryHistRPCBC.back()-> SetMarkerStyle(20)	   ;
    m_SummaryHistRPCBC.back()-> SetMarkerColor(2)  ;
  
    sc = rpcprd_dq_BC.regHist( m_SummaryHistRPCBC.back() );
    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register "<< m_SummaryHistRPCBC_title <<" hist to MonGroup" ); 
      
 
  } 
  
}

void RPCStandaloneTracksMon::bookRPCSummaryFinalHistograms( int m_i_sec, const std::string & m_quantity)
{
  StatusCode sc = StatusCode::SUCCESS; 
  std::string m_generic_path_rpcmonitoring = "Muon/MuonRawDataMonitoring/RPCStandAloneTrackMon";
  MonGroup rpcprd_expert_sum( this, m_generic_path_rpcmonitoring +"/Summary", run,  ATTRIB_UNMANAGED) ;
  
  ATH_MSG_DEBUG ( "Start bookRPCSummaryFinalHistograms: " << m_i_sec << " " << m_quantity ); 
  
  char sector_char[10]    ;
  std::string sector_name ;
  sprintf(sector_char, "Sector%.2d", m_i_sec+1) ;  // sector number with 2 digits
  sector_name = sector_char ;
	
  // large sectors: (i_sec+1)%2 = 1;  small (i_sec+1)%2 = 0
  n_eta_station = ( (m_i_sec+1) %2 ) + 6 + 1 ;  // small: 1-6, large: 1-7, sec 12+14: 0-6, sec 13: 1-7 (8->7)	  
  int n_bin = 72 * n_eta_station   ;   
  //  72 = 2*2*2*3*3  measPhi + gap + dblPhi + dblZ + stationName + stationEta
  std::string PanelIndexTitle = "# Panel = View + (gap-1)*2 + (dblPhi-1)*4 + (dblZ-1)*8 + (LowPt=0,Pivot=1,HighPt=2)*24 + (Eta)*72";
  
   
  std::string m_SummaryHist_title = "Summary" + m_quantity + "_" + sector_name ;
  const char* m_SummaryHist_title_char = m_SummaryHist_title.c_str();
  bool doRecursiveReferenceDelete = gROOT->MustClean();
  gROOT->SetMustClean(false);
  m_sumTmpHist.push_back ( new TH1F(m_SummaryHist_title_char, m_SummaryHist_title_char, 2*n_bin, -n_bin, n_bin) );

  sc = rpcprd_expert_sum.regHist( m_sumTmpHist.back() );
  
  TDirectory* currentDir = gDirectory;
   

   
  TDirectory* targetDir = m_sumTmpHist.back()->GetDirectory();
  if( targetDir != 0 ) {
    targetDir->cd();
  }
  m_sumTmpHist.back()->Write();
 
  currentDir->cd();
 
  //   rpcprd_expert_sum.deregHist( m_sumTmpHist.back() ).ignore();
  //   
 
  //   
  //   delete m_sumTmpHist.back();  
 
  gROOT->SetMustClean(doRecursiveReferenceDelete);  
 
  // 
  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register "<< m_SummaryHist_title <<" hist to MonGroup" );
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode RPCStandaloneTracksMon::procHistograms( )
{
  StatusCode sc = StatusCode::SUCCESS;
  
  ATH_MSG_DEBUG ( "********Reached Last Event in RPCStandaloneTracksMon !!!" );	  
  ATH_MSG_DEBUG ( "RPCStandaloneTracksMon finalize()" ); 	  
  
  if( m_doRpcESD==true ) {if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online ) {    
      std::string m_generic_path_rpcmonitoring = "Muon/MuonRawDataMonitoring/RPCStandAloneTrackMon" ;	
      MonGroup rpcprd_expert     ( this, m_generic_path_rpcmonitoring+"/Overview"  ,  run, ATTRIB_UNMANAGED );
      MonGroup rpcprd_shift      ( this, m_generic_path_rpcmonitoring+"/Overview"  ,  run, ATTRIB_UNMANAGED );
      MonGroup rpc_dqmf_global   ( this, m_generic_path_rpcmonitoring+"/GLOBAL"    ,  run, ATTRIB_UNMANAGED );
      MonGroup rpcprd_expert_sum ( this, m_generic_path_rpcmonitoring + "/Summary" ,  run, ATTRIB_UNMANAGED );
      MonGroup rpcprd_dq_BA      ( this, m_generic_path_rpcmonitoring + "/RPCBA"   ,  run, ATTRIB_UNMANAGED );
      MonGroup rpcprd_dq_BC      ( this, m_generic_path_rpcmonitoring + "/RPCBC"   ,  run, ATTRIB_UNMANAGED );
    
      if(endOfRunFlag()){        
    
	sc = rpcprd_dq_BA.getHist(RPCBA_layerTrackProj,"Layer_TrackProj_sideA");	      
	if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get Layer_TrackProj_sideA  hist" );  
             
	sc = rpcprd_dq_BC.getHist(RPCBC_layerTrackProj,"Layer_TrackProj_sideC");	      
	if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get Layer_TrackProj_sideC  hist" );  
         
	sc = rpcprd_dq_BA.getHist(RPCBA_layerHitOnTrack,"Layer_HitOnTrack_sideA");	   
	if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get Layer_HitOnTrack_sideA  hist" );  
     	  
	sc = rpcprd_dq_BC.getHist(RPCBC_layerHitOnTrack,"Layer_HitOnTrack_sideC");	   
	if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get Layer_HitOnTrack_sideC  hist" );  
	 	         
	sc = rpcprd_dq_BA.getHist(RPCBA_layerEfficiency,"Layer_Efficiency_sideA");	   
	if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get Layer_Efficiency_sideA  hist" );  
     	  
	sc = rpcprd_dq_BC.getHist(RPCBC_layerEfficiency,"Layer_Efficiency_sideC");	   
	if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get Layer_Efficiency_sideC  hist" );
      
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
      
	RPCLyHitOnTr		      = 0 ;
	RPCLyTrkPrj		      = 0 ;
	RPCLyEff 		      = 0 ;
	RPCLyEff_err		      = 0 ;
	PanelVal 		      = 0 ; 
	PanelVal_err		      = 0 ;
	PanelVal_entries 	      = 0 ; 
	PanelVal_square  	      = 0 ; 
	PanelHitOnTrack  	      = 0 ;
	PanelTrackProj		      = 0 ;
	PanelHitOnTrack_withCrossStrip = 0 ;
	  

	for (int ibin=0; ibin!=24; ibin++) {
	  RPCLyTrkPrj = LayerTrackProjection ->GetBinContent ( ibin + 1 ) ;
	  if ( RPCLyTrkPrj>0 ) {
	    RPCLyHitOnTr = LayerHitOnTrack ->GetBinContent ( ibin + 1 ) ;
	    RPCLyEff     = RPCLyHitOnTr / RPCLyTrkPrj ;
	    RPCLyEff_err = sqrt( fabs( RPCLyHitOnTr-0.5*0) / RPCLyTrkPrj ) *
	      sqrt( 1. - fabs( RPCLyHitOnTr-0.5*0) / RPCLyTrkPrj ) /
	      sqrt( RPCLyTrkPrj ) ;
	  
	    LayerEff->SetBinContent ( ibin + 1 , RPCLyEff     ) ;
	    LayerEff->SetBinError   ( ibin + 1 , RPCLyEff_err ) ;
	  }	
	  RPCLyTrkPrj = RPCBA_layerTrackProj ->GetBinContent ( ibin + 1 ) ;
	  if ( RPCLyTrkPrj>0 ) {
	    RPCLyHitOnTr = RPCBA_layerHitOnTrack ->GetBinContent ( ibin + 1 ) ;
	    RPCLyEff     = RPCLyHitOnTr / RPCLyTrkPrj ;
	    RPCLyEff_err = sqrt( fabs( RPCLyHitOnTr-0.5*0) / RPCLyTrkPrj ) *
	      sqrt( 1. - fabs( RPCLyHitOnTr-0.5*0) / RPCLyTrkPrj ) /
	      sqrt( RPCLyTrkPrj ) ;
	  
	    RPCBA_layerEfficiency->SetBinContent ( ibin + 1 , RPCLyEff     ) ;
	    RPCBA_layerEfficiency->SetBinError   ( ibin + 1 , RPCLyEff_err ) ;
	  }	
	  RPCLyTrkPrj = RPCBC_layerTrackProj ->GetBinContent ( ibin + 1 ) ;
	  if ( RPCLyTrkPrj>0 ) {
	    RPCLyHitOnTr = RPCBC_layerHitOnTrack ->GetBinContent ( ibin + 1 ) ;
	    RPCLyEff     = RPCLyHitOnTr / RPCLyTrkPrj ;
	    RPCLyEff_err = sqrt( fabs( RPCLyHitOnTr-0.5*0) / RPCLyTrkPrj ) *
	      sqrt( 1. - fabs( RPCLyHitOnTr-0.5*0) / RPCLyTrkPrj ) /
	      sqrt( RPCLyTrkPrj ) ;
	  
	    RPCBC_layerEfficiency->SetBinContent ( ibin + 1 , RPCLyEff     ) ;
	    RPCBC_layerEfficiency->SetBinError   ( ibin + 1 , RPCLyEff_err ) ;
	  }
	
	
	}
	
	//TriggerEfficiency
	int nb = hMEtracks->GetNbinsX() ;
	for (int ibin=0; ibin!=nb; ibin++) {
	  int TrkPrj = hMEtracks ->GetBinContent ( ibin + 1 ) ;
	  if ( TrkPrj>0 ) {
	    //hRPCPhiEtaCoinThr0
	      int   RPCOnTr    = hRPCPhiEtaCoinThr[0] ->GetBinContent ( ibin + 1 ) ;
	      float RPCEff     = RPCOnTr / TrkPrj ;
	      float RPCEff_err = sqrt( fabs( RPCOnTr-0.5*0) / TrkPrj ) *
	      sqrt( 1. - fabs( RPCOnTr-0.5*0) / TrkPrj ) /
	      sqrt( TrkPrj ) ;	  
	      hRPCPhiEtaCoinThr_eff[0]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      hRPCPhiEtaCoinThr_eff[0]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	      
	    //hRPCPhiEtaCoinThr1
	      RPCOnTr      = hRPCPhiEtaCoinThr[1] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff     = RPCOnTr / TrkPrj ;
	      RPCEff_err = sqrt( fabs( RPCOnTr-0.5*0) / TrkPrj ) *
	      sqrt( 1. - fabs( RPCOnTr-0.5*0) / TrkPrj ) /
	      sqrt( TrkPrj ) ;	  
	      hRPCPhiEtaCoinThr_eff[1]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      hRPCPhiEtaCoinThr_eff[1]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	      
	    //hRPCPhiEtaCoinThr2
	      RPCOnTr      = hRPCPhiEtaCoinThr[2] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff     = RPCOnTr / TrkPrj ;
	      RPCEff_err = sqrt( fabs( RPCOnTr-0.5*0) / TrkPrj ) *
	      sqrt( 1. - fabs( RPCOnTr-0.5*0) / TrkPrj ) /
	      sqrt( TrkPrj ) ;	  
	      hRPCPhiEtaCoinThr_eff[2]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      hRPCPhiEtaCoinThr_eff[2]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	      
	    //hRPCPhiEtaCoinThr3
	      RPCOnTr      = hRPCPhiEtaCoinThr[3] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff     = RPCOnTr / TrkPrj ;
	      RPCEff_err = sqrt( fabs( RPCOnTr-0.5*0) / TrkPrj ) *
	      sqrt( 1. - fabs( RPCOnTr-0.5*0) / TrkPrj ) /
	      sqrt( TrkPrj ) ;	  
	      hRPCPhiEtaCoinThr_eff[3]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      hRPCPhiEtaCoinThr_eff[3]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	      
	    //hRPCPhiEtaCoinThr4
	      RPCOnTr      = hRPCPhiEtaCoinThr[4] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff     = RPCOnTr / TrkPrj ;
	      RPCEff_err = sqrt( fabs( RPCOnTr-0.5*0) / TrkPrj ) *
	      sqrt( 1. - fabs( RPCOnTr-0.5*0) / TrkPrj ) /
	      sqrt( TrkPrj ) ;	  
	      hRPCPhiEtaCoinThr_eff[4]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      hRPCPhiEtaCoinThr_eff[4]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	      
	    //hRPCPhiEtaCoinThr5
	      RPCOnTr      = hRPCPhiEtaCoinThr[5] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff     = RPCOnTr / TrkPrj ;
	      RPCEff_err = sqrt( fabs( RPCOnTr-0.5*0) / TrkPrj ) *
	      sqrt( 1. - fabs( RPCOnTr-0.5*0) / TrkPrj ) /
	      sqrt( TrkPrj ) ;	  
	      hRPCPhiEtaCoinThr_eff[5]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      hRPCPhiEtaCoinThr_eff[5]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	    //hRPCPadThr0
	      RPCOnTr	 = hRPCPadThr[0] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff	 = RPCOnTr / TrkPrj ;
	      RPCEff_err = sqrt( fabs( RPCOnTr-0.5*0) / TrkPrj ) *
	      sqrt( 1. - fabs( RPCOnTr-0.5*0) / TrkPrj ) /
	      sqrt( TrkPrj ) ;	  
	      hRPCPadThr_eff[0]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      hRPCPadThr_eff[0]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	      
	    //hRPCPadThr1
	      RPCOnTr      = hRPCPadThr[1] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff     = RPCOnTr / TrkPrj ;
	      RPCEff_err = sqrt( fabs( RPCOnTr-0.5*0) / TrkPrj ) *
	      sqrt( 1. - fabs( RPCOnTr-0.5*0) / TrkPrj ) /
	      sqrt( TrkPrj ) ;	  
	      hRPCPadThr_eff[1]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      hRPCPadThr_eff[1]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	      
	    //hRPCPadThr2
	      RPCOnTr      = hRPCPadThr[2] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff     = RPCOnTr / TrkPrj ;
	      RPCEff_err = sqrt( fabs( RPCOnTr-0.5*0) / TrkPrj ) *
	      sqrt( 1. - fabs( RPCOnTr-0.5*0) / TrkPrj ) /
	      sqrt( TrkPrj ) ;	  
	      hRPCPadThr_eff[2]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      hRPCPadThr_eff[2]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	      
	    //hRPCPadThr3
	      RPCOnTr      = hRPCPadThr[3] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff     = RPCOnTr / TrkPrj ;
	      RPCEff_err = sqrt( fabs( RPCOnTr-0.5*0) / TrkPrj ) *
	      sqrt( 1. - fabs( RPCOnTr-0.5*0) / TrkPrj ) /
	      sqrt( TrkPrj ) ;	  
	      hRPCPadThr_eff[3]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      hRPCPadThr_eff[3]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	      
	    //hRPCPadThr4
	      RPCOnTr      = hRPCPadThr[4] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff     = RPCOnTr / TrkPrj ;
	      RPCEff_err = sqrt( fabs( RPCOnTr-0.5*0) / TrkPrj ) *
	      sqrt( 1. - fabs( RPCOnTr-0.5*0) / TrkPrj ) /
	      sqrt( TrkPrj ) ;	  
	      hRPCPadThr_eff[4]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      hRPCPadThr_eff[4]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	      
	    //hRPCPadThr5
	      RPCOnTr      = hRPCPadThr[5] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff     = RPCOnTr / TrkPrj ;
	      RPCEff_err = sqrt( fabs( RPCOnTr-0.5*0) / TrkPrj ) *
	      sqrt( 1. - fabs( RPCOnTr-0.5*0) / TrkPrj ) /
	      sqrt( TrkPrj ) ;	  
	      hRPCPadThr_eff[5]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      hRPCPadThr_eff[5]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	    //hRPCMuctpiThr0
	      RPCOnTr	 = hRPCMuctpiThr[0] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff	 = RPCOnTr / TrkPrj ;
	      RPCEff_err = sqrt( fabs( RPCOnTr-0.5*0) / TrkPrj ) *
	      sqrt( 1. - fabs( RPCOnTr-0.5*0) / TrkPrj ) /
	      sqrt( TrkPrj ) ;	  
	      hRPCMuctpiThr_eff[0]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      hRPCMuctpiThr_eff[0]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	      
	    //hRPCMuctpiThr1
	      RPCOnTr      = hRPCMuctpiThr[1] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff     = RPCOnTr / TrkPrj ;
	      RPCEff_err = sqrt( fabs( RPCOnTr-0.5*0) / TrkPrj ) *
	      sqrt( 1. - fabs( RPCOnTr-0.5*0) / TrkPrj ) /
	      sqrt( TrkPrj ) ;	  
	      hRPCMuctpiThr_eff[1]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      hRPCMuctpiThr_eff[1]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	      
	    //hRPCMuctpiThr2
	      RPCOnTr      = hRPCMuctpiThr[2] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff     = RPCOnTr / TrkPrj ;
	      RPCEff_err = sqrt( fabs( RPCOnTr-0.5*0) / TrkPrj ) *
	      sqrt( 1. - fabs( RPCOnTr-0.5*0) / TrkPrj ) /
	      sqrt( TrkPrj ) ;	  
	      hRPCMuctpiThr_eff[2]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      hRPCMuctpiThr_eff[2]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	      
	    //hRPCMuctpiThr3
	      RPCOnTr      = hRPCMuctpiThr[3] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff     = RPCOnTr / TrkPrj ;
	      RPCEff_err = sqrt( fabs( RPCOnTr-0.5*0) / TrkPrj ) *
	      sqrt( 1. - fabs( RPCOnTr-0.5*0) / TrkPrj ) /
	      sqrt( TrkPrj ) ;	  
	      hRPCMuctpiThr_eff[3]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      hRPCMuctpiThr_eff[3]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	      
	    //hRPCMuctpiThr4
	      RPCOnTr      = hRPCMuctpiThr[4] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff     = RPCOnTr / TrkPrj ;
	      RPCEff_err = sqrt( fabs( RPCOnTr-0.5*0) / TrkPrj ) *
	      sqrt( 1. - fabs( RPCOnTr-0.5*0) / TrkPrj ) /
	      sqrt( TrkPrj ) ;	  
	      hRPCMuctpiThr_eff[4]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      hRPCMuctpiThr_eff[4]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	      
	    //hRPCMuctpiThr5
	      RPCOnTr      = hRPCMuctpiThr[5] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff     = RPCOnTr / TrkPrj ;
	      RPCEff_err = sqrt( fabs( RPCOnTr-0.5*0) / TrkPrj ) *
	      sqrt( 1. - fabs( RPCOnTr-0.5*0) / TrkPrj ) /
	      sqrt( TrkPrj ) ;	  
	      hRPCMuctpiThr_eff[5]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      hRPCMuctpiThr_eff[5]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	  }	
	
	}
	
	
	//TriggerEfficiency end

    
	std::vector<int>::const_iterator iter_bin=layer_name_bin_list_panel.begin() ;
	for (std::vector<std::string>::const_iterator iter=layer_name_list_panel.begin(); iter!=layer_name_list_panel.end(); iter++) {
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
	  m_SummaryHist_Idx = (atoi(sector_num.c_str())-1)*m_SummaryHist_Size/16;
            
      
	  //1) Efficiency
	  sc = rpcprd_expert_sum.getHist ( SummaryHitOnTrack        ,"SummaryHitOnTrack_"         + sector_name ) ;
	  sc = rpcprd_expert_sum.getHist ( SummaryEfficiency        ,"SummaryEfficiency_"         + sector_name ) ;
	  sc = rpcprd_expert_sum.getHist ( SummaryEffDistriPerSector,"SummaryEffDistriPerSector_" + sector_name ) ;
	        
	  // fix 23 ott 2008
	  shift_pos = m_SummaryHist[enumSumTrackProj+m_SummaryHist_Idx]-> GetNbinsX()/2 + 1;
	
	  PanelHitOnTrack = float ( SummaryHitOnTrack -> GetBinContent( pos + shift_pos ) ) ;
	  PanelTrackProj  = float ( m_SummaryHist[enumSumTrackProj+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;
	  PanelVal = 0 ;
	  if ( PanelTrackProj != 0 )  {
	    PanelVal     = PanelHitOnTrack/PanelTrackProj ;
	    PanelVal_err = sqrt( fabs( PanelHitOnTrack-0.5*0) / PanelTrackProj ) *
	      sqrt( 1. - fabs( PanelHitOnTrack-0.5*0) / PanelTrackProj ) /
	      sqrt( PanelTrackProj ) ;
	    SummaryEfficiency 	   -> SetBinContent ( pos + shift_pos , PanelVal     ) ;
	    SummaryEfficiency 	   -> SetBinError   ( pos + shift_pos , PanelVal_err ) ;
	    if ( pos>0 ) { rpcAverageSide_A[enumAvEff] -> Fill(PanelVal) ;  } 
	    else         { rpcAverageSide_C[enumAvEff] -> Fill(PanelVal) ;  }
	    SummaryEffDistriPerSector-> Fill(PanelVal) ; 
	  }
             
      
	  //2) GapEfficiency
	  sc = rpcprd_expert_sum.getHist ( SummaryGapEffDistriPerSector    ,"SummaryGapEffDistriPerSector_"     + sector_name ) ;
	
	  PanelHitOnTrack                = float ( SummaryHitOnTrack -> GetBinContent( pos + shift_pos ) ) ;
	  PanelTrackProj                 = float ( m_SummaryHist[enumSumTrackProj+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;
	  PanelHitOnTrack_withCrossStrip = float ( m_SummaryHist[enumSumHitOnTrack_withCrossStrip+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;
	  PanelVal = 0 ;
	  if ( PanelTrackProj != 0 )  {
	    PanelVal     = PanelHitOnTrack/PanelTrackProj ;
	    PanelVal_err = sqrt( fabs( PanelHitOnTrack-0.5*0) / PanelTrackProj ) *
	      sqrt( 1. - fabs( PanelHitOnTrack-0.5*0) / PanelTrackProj ) /
	      sqrt( PanelTrackProj ) ;
	    m_SummaryHist[enumSumGapEfficiency+m_SummaryHist_Idx]-> SetBinContent ( pos + shift_pos , PanelVal	 ) ;
	    m_SummaryHist[enumSumGapEfficiency+m_SummaryHist_Idx]-> SetBinError	( pos + shift_pos , PanelVal_err ) ;	      
	    if ( pos>0 ) { rpcAverageSide_A[enumAvGapEff] -> Fill(PanelVal) ;  } 
	    else         { rpcAverageSide_C[enumAvGapEff] -> Fill(PanelVal) ;  }
	    SummaryGapEffDistriPerSector-> Fill(PanelVal) ; 
	  }


    
	  //3) NoiseCorr
	  sc = rpcprd_expert_sum.getHist ( SummaryNoiseCorrDistriPerSector,"SummaryNoiseCorrDistriPerSector_" + sector_name ) ;
	
	  PanelVal = float ( m_SummaryHist[enumSumNoiseCorr_NotNorm+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;
	  PanelVal_err = sqrt( PanelVal );
	  if( m_rpc_readout_window * rpc_eventstotal !=0 ) { 
	    PanelVal     = PanelVal     / m_rpc_readout_window  ;
	    PanelVal_err = PanelVal_err / m_rpc_readout_window  ; 
	
	    m_SummaryHist[enumSumNoiseCorr_NotNorm+m_SummaryHist_Idx]-> SetBinContent ( pos + shift_pos , PanelVal     ) ;
	    m_SummaryHist[enumSumNoiseCorr_NotNorm+m_SummaryHist_Idx]-> SetBinError   ( pos + shift_pos , PanelVal_err ) ;	  
	    if ( pos>0 ) { rpcAverageSide_A[enumAvNoiseCorr] -> Fill(PanelVal/rpc_eventstotal) ;  } 
	    else         { rpcAverageSide_C[enumAvNoiseCorr] -> Fill(PanelVal/rpc_eventstotal) ;  } 
	    SummaryNoiseCorrDistriPerSector-> Fill(PanelVal/ rpc_eventstotal) ; 
	  }
	
	  //4) NoiseTot
	  sc = rpcprd_expert_sum.getHist ( SummaryNoiseTotDistriPerSector,"SummaryNoiseTotDistriPerSector_" + sector_name ) ;

	  PanelVal = float ( m_SummaryHist[enumSumNoiseTot_NotNorm+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;
	  PanelVal_err = sqrt( PanelVal );	

	  if( m_rpc_readout_window * rpc_eventstotal !=0 ) { 
	    PanelVal     = PanelVal     / m_rpc_readout_window ;
	    PanelVal_err = PanelVal_err / m_rpc_readout_window ;
	  
	    m_SummaryHist[enumSumNoiseTot_NotNorm+m_SummaryHist_Idx]-> SetBinContent ( pos + shift_pos , PanelVal     ) ;
	    m_SummaryHist[enumSumNoiseTot_NotNorm+m_SummaryHist_Idx]-> SetBinError   ( pos + shift_pos , PanelVal_err ) ;
	    if ( pos>0 ) { rpcAverageSide_A[enumAvNoiseTot] -> Fill(PanelVal/ rpc_eventstotal) ; } 
	    else         { rpcAverageSide_C[enumAvNoiseTot] -> Fill(PanelVal/ rpc_eventstotal) ; }
	    SummaryNoiseTotDistriPerSector-> Fill(PanelVal/ rpc_eventstotal) ; 
	  }
             
      
	  //5) AverageCS
	  sc = rpcprd_expert_sum.getHist ( SummaryCSDistriPerSector ,"SummaryCSDistriPerSector_" + sector_name  );
	
	  PanelVal_entries = float ( m_SummaryHist[enumSumCS_entries+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;	
	  PanelVal         = float ( m_SummaryHist[enumSumCS_NotNorm+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;
	  PanelVal_square  = float ( m_SummaryHist[enumSumCS_square+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;	 
	  if(PanelVal_entries>0){
	    PanelVal=PanelVal/PanelVal_entries;
	    PanelVal_square=PanelVal_square/PanelVal_entries;	
	  }
	  PanelVal_err     = sqrt  ( fabs(PanelVal_square - PanelVal*PanelVal) ) ;
	  // rpcAverageCS	 -> Fill(PanelVal) ; 
	  if ( pos>0 ) { rpcAverageSide_A[enumAvCS] -> Fill(PanelVal) ; } 
	  else         { rpcAverageSide_C[enumAvCS] -> Fill(PanelVal) ; }
	  SummaryCSDistriPerSector-> Fill(PanelVal) ; 
	
	
	  //6) Res_CS1
	  sc = rpcprd_expert_sum.getHist ( SummaryRes_CS1DistriPerSector    ,"SummaryRes_CS1DistriPerSector_"    + sector_name ) ;
	  sc = rpcprd_expert_sum.getHist ( SummaryRes_CS1rmsDistriPerSector ,"SummaryRes_CS1rmsDistriPerSector_" + sector_name		  ) ;
     
	  PanelVal_entries = float ( m_SummaryHist[enumSumRes_CS1_entries+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;	
	  PanelVal         = float ( m_SummaryHist[enumSumRes_CS1_NotNorm+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;	
	  PanelVal_square  = float ( m_SummaryHist[enumSumRes_CS1_square+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;
	  if(PanelVal_entries>0){
	    PanelVal=PanelVal/PanelVal_entries;
	    PanelVal_square=PanelVal_square/PanelVal_entries;	
	  }
	  PanelVal_err     = sqrt  ( fabs(PanelVal_square - PanelVal*PanelVal) ) ;
	  if ( pos>0 ) { rpcAverageSide_A[enumAvRes_CS1] -> Fill(PanelVal) ;  } 
	  else         { rpcAverageSide_C[enumAvRes_CS1] -> Fill(PanelVal) ;  } 
	  SummaryRes_CS1DistriPerSector    -> Fill(PanelVal) ;
	  if ( pos>0 ) { rpcAverageSide_A[enumAvRes_CS1rms] -> Fill(PanelVal_err) ;  } 
	  else         { rpcAverageSide_C[enumAvRes_CS1rms] -> Fill(PanelVal_err) ;  }  
	  SummaryRes_CS1rmsDistriPerSector -> Fill(PanelVal_err) ;
	
	  //7) Res_CS2
	  sc = rpcprd_expert_sum.getHist ( SummaryRes_CS2DistriPerSector    ,"SummaryRes_CS2DistriPerSector_"    + sector_name ) ;
	  sc = rpcprd_expert_sum.getHist ( SummaryRes_CS2rmsDistriPerSector ,"SummaryRes_CS2rmsDistriPerSector_" + sector_name ) ;
	
	  PanelVal_entries = float ( m_SummaryHist[enumSumRes_CS2_entries+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;	
	  PanelVal         = float ( m_SummaryHist[enumSumRes_CS2_NotNorm+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;	
	  PanelVal_square  = float ( m_SummaryHist[enumSumRes_CS2_square+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;	 
	  if(PanelVal_entries>0){
	    PanelVal=PanelVal/PanelVal_entries;
	    PanelVal_square=PanelVal_square/PanelVal_entries;	
	  }
	  PanelVal_err     = sqrt  ( fabs(PanelVal_square - PanelVal*PanelVal) ) ;
	  if ( pos>0 ) { rpcAverageSide_A[enumAvRes_CS2] -> Fill(PanelVal) ;  } 
	  else         { rpcAverageSide_C[enumAvRes_CS2] -> Fill(PanelVal) ;  }  
	  SummaryRes_CS2DistriPerSector   -> Fill(PanelVal) ;
	  if ( pos>0 ) { rpcAverageSide_A[enumAvRes_CS2rms] -> Fill(PanelVal_err) ;  } 
	  else         { rpcAverageSide_C[enumAvRes_CS2rms] -> Fill(PanelVal_err) ;  } 
	  SummaryRes_CS2rmsDistriPerSector-> Fill(PanelVal_err) ;
     
    
	  //8) Res_CSmore2
	  sc = rpcprd_expert_sum.getHist ( SummaryRes_CSmore2DistriPerSector    ,"SummaryRes_CSmore2DistriPerSector_"    + sector_name ) ;
	  sc = rpcprd_expert_sum.getHist ( SummaryRes_CSmore2rmsDistriPerSector ,"SummaryRes_CSmore2rmsDistriPerSector_" + sector_name ) ;
	
	  PanelVal_entries = float ( m_SummaryHist[enumSumRes_CSmore2_entries+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;	
	  PanelVal         = float ( m_SummaryHist[enumSumRes_CSmore2_NotNorm+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;	
	  PanelVal_square  = float ( m_SummaryHist[enumSumRes_CSmore2_square+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;
	  if(PanelVal_entries>0){
	    PanelVal=PanelVal/PanelVal_entries;
	    PanelVal_square=PanelVal_square/PanelVal_entries;	
	  }
	  PanelVal_err     = sqrt  ( fabs(PanelVal_square - PanelVal*PanelVal) ) ;
	  if ( pos>0 ) { rpcAverageSide_A[enumAvRes_CSmore2] -> Fill(PanelVal) ;  } 
	  else         { rpcAverageSide_C[enumAvRes_CSmore2] -> Fill(PanelVal) ;  }  
	  SummaryRes_CSmore2DistriPerSector   -> Fill(PanelVal) ;
	  if ( pos>0 ) { rpcAverageSide_A[enumAvRes_CSmore2rms] -> Fill(PanelVal_err) ;  } 
	  else         { rpcAverageSide_C[enumAvRes_CSmore2rms] -> Fill(PanelVal_err) ;  }  
	  SummaryRes_CSmore2rmsDistriPerSector-> Fill(PanelVal_err) ;
	    
	        
	  //9) Occupancy
	  sc = rpcprd_expert_sum.getHist ( SummaryOccupancyDistriPerSector ,"SummaryOccupancyDistriPerSector_" + sector_name	   );
	
	  PanelVal         = float ( m_SummaryHist[enumSumOccupancy_NotNorm+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ; 
	  PanelVal_err     = sqrt(PanelVal);
	
	  m_SummaryHist[enumSumOccupancy_NotNorm+m_SummaryHist_Idx]-> SetBinContent ( pos + shift_pos , PanelVal    ) ;
	  m_SummaryHist[enumSumOccupancy_NotNorm+m_SummaryHist_Idx]-> SetBinError   ( pos + shift_pos , PanelVal    ) ;
	
	  if(rpc_eventstotal>0) {
	    PanelVal = PanelVal / rpc_eventstotal ;
	    if(PanelVal>0) {
	      PanelVal     = log10(PanelVal     ) ;
	      PanelVal_err = log10(PanelVal_err ) ;
     
	    }
	    else {
	      PanelVal     = -10 ;
	      PanelVal_err =   0 ;
	    }
	  }      
	  else {
	    PanelVal     = 0 ;
	    PanelVal_err = 0 ;
	  }

	  if ( pos>0 ) { rpcAverageSide_A[enumAvOccupancy] -> Fill(PanelVal) ;  } 
	  else         { rpcAverageSide_C[enumAvOccupancy] -> Fill(PanelVal) ;  }  
	  SummaryOccupancyDistriPerSector-> Fill(PanelVal) ;	    
	        
	  //10) Time
	  sc = rpcprd_expert_sum.getHist ( SummaryTimeDistriPerSector ,"SummaryTimeDistriPerSector_" + sector_name );
	
	  PanelVal_entries = float ( m_SummaryHist[enumSumCS_entries+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;	
	  PanelVal         = float ( m_SummaryHist[enumSumTime_NotNorm+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;	
	  PanelVal_square  = float ( m_SummaryHist[enumSumTime_square+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;	
	  if(PanelVal_entries>0){
	    PanelVal=PanelVal/PanelVal_entries;
	    PanelVal_square=PanelVal_square/PanelVal_entries;	
	  }
	  PanelVal_err     = sqrt  ( fabs(PanelVal_square - PanelVal*PanelVal) ) ;
	  if ( pos>0 ) { rpcAverageSide_A[enumAvTime] -> Fill(PanelVal) ;  } 
	  else         { rpcAverageSide_C[enumAvTime] -> Fill(PanelVal) ;  }  
	  SummaryTimeDistriPerSector    -> Fill(PanelVal) ;	
             
      
	}
    
  
  
      } // end if isEndOfRun
  
    }}//m_doRpcESD // AthenaMonManager::tier0 || AthenaMonManager::tier0ESD  
  
  return sc;
}
 
 
 
//======================================================================================//
/**  finalize */
//======================================================================================//
StatusCode RPCStandaloneTracksMon:: finalize() 
{ 

  StatusCode sc = ManagedMonitorToolBase::finalize();
  if(!sc.isSuccess()) return sc;


  ATH_MSG_DEBUG ( "RPCStandaloneTracksMon::finalize() " );

  // Clear Muon Monitoring Histograms 
  m_SummaryHist.clear();
  m_SummaryHistRPCBA.clear();
  m_SummaryHistRPCBC.clear();
  m_sumTmpHist.clear();
  rpcAverageSide_A.clear(); 
  rpcAverageSide_C.clear(); 
  
  return sc;
}
 
//======================================================================================//
/**  finalize */
//======================================================================================//
StatusCode RPCStandaloneTracksMon:: RPC_ROI_Mapping() 
{ 
 
  StatusCode sc = StatusCode::SUCCESS; 
  ATH_MSG_DEBUG ( "RPCStandaloneTracksMon::RPC_ROI_Mapping()" );
     EtaROImin[0][ 0][ 0]=  -0.238757;EtaROImax[0][ 0][ 0]=   -0.13107;PhiROImin[0][ 0][ 0]= 0.00296767;PhiROImax[0][ 0][ 0]=	0.113212;
     EtaROImin[0][ 0][ 1]=	    0;EtaROImax[0][ 0][ 1]=	     0;PhiROImin[0][ 0][ 1]=	      0;PhiROImax[0][ 0][ 1]=	       0;
     EtaROImin[0][ 0][ 2]=  -0.238757;EtaROImax[0][ 0][ 2]=   -0.13107;PhiROImin[0][ 0][ 2]=   0.116737;PhiROImax[0][ 0][ 2]=	0.224169;
     EtaROImin[0][ 0][ 3]=	    0;EtaROImax[0][ 0][ 3]=	     0;PhiROImin[0][ 0][ 3]=	      0;PhiROImax[0][ 0][ 3]=	       0;
     EtaROImin[0][ 0][ 4]=   -0.35049;EtaROImax[0][ 0][ 4]=  -0.246207;PhiROImin[0][ 0][ 4]= 0.00295181;PhiROImax[0][ 0][ 4]=	0.112612;
     EtaROImin[0][ 0][ 5]=  -0.452099;EtaROImax[0][ 0][ 5]=  -0.350831;PhiROImin[0][ 0][ 5]= 0.00295181;PhiROImax[0][ 0][ 5]=	0.112612;
     EtaROImin[0][ 0][ 6]=   -0.35049;EtaROImax[0][ 0][ 6]=  -0.246207;PhiROImin[0][ 0][ 6]=   0.116119;PhiROImax[0][ 0][ 6]=	0.223011;
     EtaROImin[0][ 0][ 7]=  -0.452099;EtaROImax[0][ 0][ 7]=  -0.350831;PhiROImin[0][ 0][ 7]=   0.116119;PhiROImax[0][ 0][ 7]=	0.223011;
     EtaROImin[0][ 0][ 8]=  -0.554444;EtaROImax[0][ 0][ 8]=  -0.458091;PhiROImin[0][ 0][ 8]= 0.00295181;PhiROImax[0][ 0][ 8]=	0.112612;
     EtaROImin[0][ 0][ 9]=  -0.648101;EtaROImax[0][ 0][ 9]=  -0.555789;PhiROImin[0][ 0][ 9]= 0.00295181;PhiROImax[0][ 0][ 9]=	0.112612;
     EtaROImin[0][ 0][10]=  -0.554444;EtaROImax[0][ 0][10]=  -0.458091;PhiROImin[0][ 0][10]=   0.116119;PhiROImax[0][ 0][10]=	0.223011;
     EtaROImin[0][ 0][11]=  -0.648101;EtaROImax[0][ 0][11]=  -0.555789;PhiROImin[0][ 0][11]=   0.116119;PhiROImax[0][ 0][11]=	0.223011;
     EtaROImin[0][ 0][12]=  -0.703298;EtaROImax[0][ 0][12]=  -0.655857;PhiROImin[0][ 0][12]= 0.00296767;PhiROImax[0][ 0][12]=	0.113212;
     EtaROImin[0][ 0][13]=  -0.776449;EtaROImax[0][ 0][13]=  -0.706413;PhiROImin[0][ 0][13]= 0.00296767;PhiROImax[0][ 0][13]=	0.113212;
     EtaROImin[0][ 0][14]=  -0.703298;EtaROImax[0][ 0][14]=  -0.655857;PhiROImin[0][ 0][14]=   0.116737;PhiROImax[0][ 0][14]=	0.224169;
     EtaROImin[0][ 0][15]=  -0.776449;EtaROImax[0][ 0][15]=  -0.706413;PhiROImin[0][ 0][15]=   0.116737;PhiROImax[0][ 0][15]=	0.224169;
     EtaROImin[0][ 0][16]=   -0.82768;EtaROImax[0][ 0][16]=  -0.783867;PhiROImin[0][ 0][16]= 0.00296767;PhiROImax[0][ 0][16]=	0.113212;
     EtaROImin[0][ 0][17]=  -0.895163;EtaROImax[0][ 0][17]=  -0.830556;PhiROImin[0][ 0][17]= 0.00296767;PhiROImax[0][ 0][17]=	0.113212;
     EtaROImin[0][ 0][18]=   -0.82768;EtaROImax[0][ 0][18]=  -0.783867;PhiROImin[0][ 0][18]=   0.116737;PhiROImax[0][ 0][18]=	0.224169;
     EtaROImin[0][ 0][19]=  -0.895163;EtaROImax[0][ 0][19]=  -0.830556;PhiROImin[0][ 0][19]=   0.116737;PhiROImax[0][ 0][19]=	0.224169;
     EtaROImin[0][ 0][20]=  -0.961344;EtaROImax[0][ 0][20]=  -0.898201;PhiROImin[0][ 0][20]= 0.00295181;PhiROImax[0][ 0][20]=	0.112612;
     EtaROImin[0][ 0][21]=   -1.02514;EtaROImax[0][ 0][21]=  -0.964674;PhiROImin[0][ 0][21]= 0.00295181;PhiROImax[0][ 0][21]=	0.112612;
     EtaROImin[0][ 0][22]=  -0.961344;EtaROImax[0][ 0][22]=  -0.898201;PhiROImin[0][ 0][22]=   0.116119;PhiROImax[0][ 0][22]=	0.223011;
     EtaROImin[0][ 0][23]=   -1.02514;EtaROImax[0][ 0][23]=  -0.964674;PhiROImin[0][ 0][23]=   0.116119;PhiROImax[0][ 0][23]=	0.223011;
     EtaROImin[0][ 0][24]=   -1.06547;EtaROImax[0][ 0][24]=   -1.03003;PhiROImin[0][ 0][24]= 0.00306876;PhiROImax[0][ 0][24]=	0.113429;
     EtaROImin[0][ 0][25]=	    0;EtaROImax[0][ 0][25]=	     0;PhiROImin[0][ 0][25]=	      0;PhiROImax[0][ 0][25]=	       0;
     EtaROImin[0][ 0][26]=   -1.06547;EtaROImax[0][ 0][26]=   -1.03003;PhiROImin[0][ 0][26]=   0.116958;PhiROImax[0][ 0][26]=	0.197143;
     EtaROImin[0][ 0][27]=	    0;EtaROImax[0][ 0][27]=	     0;PhiROImin[0][ 0][27]=	      0;PhiROImax[0][ 0][27]=	       0;
     EtaROImin[0][ 0][28]=	    0;EtaROImax[0][ 0][28]=	     0;PhiROImin[0][ 0][28]=	      0;PhiROImax[0][ 0][28]=	       0;
     EtaROImin[0][ 0][29]=	    0;EtaROImax[0][ 0][29]=	     0;PhiROImin[0][ 0][29]=	      0;PhiROImax[0][ 0][29]=	       0;
     EtaROImin[0][ 0][30]=	    0;EtaROImax[0][ 0][30]=	     0;PhiROImin[0][ 0][30]=	      0;PhiROImax[0][ 0][30]=	       0;
     EtaROImin[0][ 0][31]=	    0;EtaROImax[0][ 0][31]=	     0;PhiROImin[0][ 0][31]=	      0;PhiROImax[0][ 0][31]=	       0;
     EtaROImin[0][ 1][ 0]=  -0.213185;EtaROImax[0][ 1][ 0]=  -0.116816;PhiROImin[0][ 1][ 0]=   0.305953;PhiROImax[0][ 1][ 0]=	0.389909;
     EtaROImin[0][ 1][ 1]=  -0.118734;EtaROImax[0][ 1][ 1]= -0.0208251;PhiROImin[0][ 1][ 1]=   0.305953;PhiROImax[0][ 1][ 1]=	0.389909;
     EtaROImin[0][ 1][ 2]=  -0.213185;EtaROImax[0][ 1][ 2]=  -0.116816;PhiROImin[0][ 1][ 2]=   0.219649;PhiROImax[0][ 1][ 2]=	0.302322;
     EtaROImin[0][ 1][ 3]=  -0.118734;EtaROImax[0][ 1][ 3]= -0.0208251;PhiROImin[0][ 1][ 3]=   0.219649;PhiROImax[0][ 1][ 3]=	0.302322;
     EtaROImin[0][ 1][ 4]=  -0.416721;EtaROImax[0][ 1][ 4]=   -0.30075;PhiROImin[0][ 1][ 4]=   0.305953;PhiROImax[0][ 1][ 4]=	0.389909;
     EtaROImin[0][ 1][ 5]=  -0.302928;EtaROImax[0][ 1][ 5]=  -0.222449;PhiROImin[0][ 1][ 5]=   0.305953;PhiROImax[0][ 1][ 5]=	0.389909;
     EtaROImin[0][ 1][ 6]=  -0.416721;EtaROImax[0][ 1][ 6]=   -0.30075;PhiROImin[0][ 1][ 6]=   0.219649;PhiROImax[0][ 1][ 6]=	0.302322;
     EtaROImin[0][ 1][ 7]=  -0.302928;EtaROImax[0][ 1][ 7]=  -0.222449;PhiROImin[0][ 1][ 7]=   0.219649;PhiROImax[0][ 1][ 7]=	0.302322;
     EtaROImin[0][ 1][ 8]=  -0.573871;EtaROImax[0][ 1][ 8]=  -0.501681;PhiROImin[0][ 1][ 8]=   0.305953;PhiROImax[0][ 1][ 8]=	0.389909;
     EtaROImin[0][ 1][ 9]=  -0.504617;EtaROImax[0][ 1][ 9]=   -0.42967;PhiROImin[0][ 1][ 9]=   0.305953;PhiROImax[0][ 1][ 9]=	0.389909;
     EtaROImin[0][ 1][10]=  -0.573871;EtaROImax[0][ 1][10]=  -0.501681;PhiROImin[0][ 1][10]=   0.219649;PhiROImax[0][ 1][10]=	0.302322;
     EtaROImin[0][ 1][11]=  -0.504617;EtaROImax[0][ 1][11]=   -0.42967;PhiROImin[0][ 1][11]=   0.219649;PhiROImax[0][ 1][11]=	0.302322;
     EtaROImin[0][ 1][12]=  -0.741516;EtaROImax[0][ 1][12]=  -0.649933;PhiROImin[0][ 1][12]=   0.305953;PhiROImax[0][ 1][12]=	0.389909;
     EtaROImin[0][ 1][13]=  -0.653329;EtaROImax[0][ 1][13]=  -0.583785;PhiROImin[0][ 1][13]=   0.305953;PhiROImax[0][ 1][13]=	0.389909;
     EtaROImin[0][ 1][14]=  -0.741516;EtaROImax[0][ 1][14]=  -0.649933;PhiROImin[0][ 1][14]=   0.219649;PhiROImax[0][ 1][14]=	0.302322;
     EtaROImin[0][ 1][15]=  -0.653329;EtaROImax[0][ 1][15]=  -0.583785;PhiROImin[0][ 1][15]=   0.219649;PhiROImax[0][ 1][15]=	0.302322;
     EtaROImin[0][ 1][16]=	    0;EtaROImax[0][ 1][16]=	     0;PhiROImin[0][ 1][16]=	      0;PhiROImax[0][ 1][16]=	       0;
     EtaROImin[0][ 1][17]=  -0.837822;EtaROImax[0][ 1][17]=  -0.756521;PhiROImin[0][ 1][17]=   0.305953;PhiROImax[0][ 1][17]=	0.389909;
     EtaROImin[0][ 1][18]=	    0;EtaROImax[0][ 1][18]=	     0;PhiROImin[0][ 1][18]=	      0;PhiROImax[0][ 1][18]=	       0;
     EtaROImin[0][ 1][19]=  -0.837822;EtaROImax[0][ 1][19]=  -0.756521;PhiROImin[0][ 1][19]=   0.219649;PhiROImax[0][ 1][19]=	0.302322;
     EtaROImin[0][ 1][20]=  -0.956037;EtaROImax[0][ 1][20]=  -0.899344;PhiROImin[0][ 1][20]=   0.305953;PhiROImax[0][ 1][20]=	0.389909;
     EtaROImin[0][ 1][21]=  -0.903324;EtaROImax[0][ 1][21]=  -0.844116;PhiROImin[0][ 1][21]=   0.305953;PhiROImax[0][ 1][21]=	0.389909;
     EtaROImin[0][ 1][22]=  -0.956037;EtaROImax[0][ 1][22]=  -0.899344;PhiROImin[0][ 1][22]=   0.219649;PhiROImax[0][ 1][22]=	0.302322;
     EtaROImin[0][ 1][23]=  -0.903324;EtaROImax[0][ 1][23]=  -0.844116;PhiROImin[0][ 1][23]=   0.219649;PhiROImax[0][ 1][23]=	0.302322;
     EtaROImin[0][ 1][24]=	    0;EtaROImax[0][ 1][24]=	     0;PhiROImin[0][ 1][24]=	      0;PhiROImax[0][ 1][24]=	       0;
     EtaROImin[0][ 1][25]=	    0;EtaROImax[0][ 1][25]=	     0;PhiROImin[0][ 1][25]=	      0;PhiROImax[0][ 1][25]=	       0;
     EtaROImin[0][ 1][26]=	    0;EtaROImax[0][ 1][26]=	     0;PhiROImin[0][ 1][26]=	      0;PhiROImax[0][ 1][26]=	       0;
     EtaROImin[0][ 1][27]=	    0;EtaROImax[0][ 1][27]=	     0;PhiROImin[0][ 1][27]=	      0;PhiROImax[0][ 1][27]=	       0;
     EtaROImin[0][ 1][28]=	    0;EtaROImax[0][ 1][28]=	     0;PhiROImin[0][ 1][28]=	      0;PhiROImax[0][ 1][28]=	       0;
     EtaROImin[0][ 1][29]=	    0;EtaROImax[0][ 1][29]=	     0;PhiROImin[0][ 1][29]=	      0;PhiROImax[0][ 1][29]=	       0;
     EtaROImin[0][ 1][30]=	    0;EtaROImax[0][ 1][30]=	     0;PhiROImin[0][ 1][30]=	      0;PhiROImax[0][ 1][30]=	       0;
     EtaROImin[0][ 1][31]=	    0;EtaROImax[0][ 1][31]=	     0;PhiROImin[0][ 1][31]=	      0;PhiROImax[0][ 1][31]=	       0;
     EtaROImin[0][ 2][ 0]=  -0.118734;EtaROImax[0][ 2][ 0]= -0.0208251;PhiROImin[0][ 2][ 0]=   0.395489;PhiROImax[0][ 2][ 0]=	0.479445;
     EtaROImin[0][ 2][ 1]=  -0.213185;EtaROImax[0][ 2][ 1]=  -0.116816;PhiROImin[0][ 2][ 1]=   0.395489;PhiROImax[0][ 2][ 1]=	0.479445;
     EtaROImin[0][ 2][ 2]=  -0.118734;EtaROImax[0][ 2][ 2]= -0.0208251;PhiROImin[0][ 2][ 2]=   0.483076;PhiROImax[0][ 2][ 2]=	0.565749;
     EtaROImin[0][ 2][ 3]=  -0.213185;EtaROImax[0][ 2][ 3]=  -0.116816;PhiROImin[0][ 2][ 3]=   0.483076;PhiROImax[0][ 2][ 3]=	0.565749;
     EtaROImin[0][ 2][ 4]=  -0.302928;EtaROImax[0][ 2][ 4]=  -0.222449;PhiROImin[0][ 2][ 4]=   0.395489;PhiROImax[0][ 2][ 4]=	0.479445;
     EtaROImin[0][ 2][ 5]=  -0.416721;EtaROImax[0][ 2][ 5]=   -0.30075;PhiROImin[0][ 2][ 5]=   0.395489;PhiROImax[0][ 2][ 5]=	0.479445;
     EtaROImin[0][ 2][ 6]=  -0.302928;EtaROImax[0][ 2][ 6]=  -0.222449;PhiROImin[0][ 2][ 6]=   0.483076;PhiROImax[0][ 2][ 6]=	0.565749;
     EtaROImin[0][ 2][ 7]=  -0.416721;EtaROImax[0][ 2][ 7]=   -0.30075;PhiROImin[0][ 2][ 7]=   0.483076;PhiROImax[0][ 2][ 7]=	0.565749;
     EtaROImin[0][ 2][ 8]=  -0.504617;EtaROImax[0][ 2][ 8]=   -0.42967;PhiROImin[0][ 2][ 8]=   0.395489;PhiROImax[0][ 2][ 8]=	0.479445;
     EtaROImin[0][ 2][ 9]=  -0.573871;EtaROImax[0][ 2][ 9]=  -0.501681;PhiROImin[0][ 2][ 9]=   0.395489;PhiROImax[0][ 2][ 9]=	0.479445;
     EtaROImin[0][ 2][10]=  -0.504617;EtaROImax[0][ 2][10]=   -0.42967;PhiROImin[0][ 2][10]=   0.483076;PhiROImax[0][ 2][10]=	0.565749;
     EtaROImin[0][ 2][11]=  -0.573871;EtaROImax[0][ 2][11]=  -0.501681;PhiROImin[0][ 2][11]=   0.483076;PhiROImax[0][ 2][11]=	0.565749;
     EtaROImin[0][ 2][12]=  -0.653329;EtaROImax[0][ 2][12]=  -0.583785;PhiROImin[0][ 2][12]=   0.395489;PhiROImax[0][ 2][12]=	0.479445;
     EtaROImin[0][ 2][13]=  -0.741516;EtaROImax[0][ 2][13]=  -0.649933;PhiROImin[0][ 2][13]=   0.395489;PhiROImax[0][ 2][13]=	0.479445;
     EtaROImin[0][ 2][14]=  -0.653329;EtaROImax[0][ 2][14]=  -0.583785;PhiROImin[0][ 2][14]=   0.483076;PhiROImax[0][ 2][14]=	0.565749;
     EtaROImin[0][ 2][15]=  -0.741516;EtaROImax[0][ 2][15]=  -0.649933;PhiROImin[0][ 2][15]=   0.483076;PhiROImax[0][ 2][15]=	0.565749;
     EtaROImin[0][ 2][16]=  -0.837822;EtaROImax[0][ 2][16]=  -0.756521;PhiROImin[0][ 2][16]=   0.395489;PhiROImax[0][ 2][16]=	0.479445;
     EtaROImin[0][ 2][17]=	    0;EtaROImax[0][ 2][17]=	     0;PhiROImin[0][ 2][17]=	      0;PhiROImax[0][ 2][17]=	       0;
     EtaROImin[0][ 2][18]=  -0.837822;EtaROImax[0][ 2][18]=  -0.756521;PhiROImin[0][ 2][18]=   0.483076;PhiROImax[0][ 2][18]=	0.565749;
     EtaROImin[0][ 2][19]=	    0;EtaROImax[0][ 2][19]=	     0;PhiROImin[0][ 2][19]=	      0;PhiROImax[0][ 2][19]=	       0;
     EtaROImin[0][ 2][20]=  -0.903324;EtaROImax[0][ 2][20]=  -0.844116;PhiROImin[0][ 2][20]=   0.395489;PhiROImax[0][ 2][20]=	0.479445;
     EtaROImin[0][ 2][21]=  -0.956037;EtaROImax[0][ 2][21]=  -0.899344;PhiROImin[0][ 2][21]=   0.395489;PhiROImax[0][ 2][21]=	0.479445;
     EtaROImin[0][ 2][22]=  -0.903324;EtaROImax[0][ 2][22]=  -0.844116;PhiROImin[0][ 2][22]=   0.483076;PhiROImax[0][ 2][22]=	0.565749;
     EtaROImin[0][ 2][23]=  -0.956037;EtaROImax[0][ 2][23]=  -0.899344;PhiROImin[0][ 2][23]=   0.483076;PhiROImax[0][ 2][23]=	0.565749;
     EtaROImin[0][ 2][24]=	    0;EtaROImax[0][ 2][24]=	     0;PhiROImin[0][ 2][24]=	      0;PhiROImax[0][ 2][24]=	       0;
     EtaROImin[0][ 2][25]=	    0;EtaROImax[0][ 2][25]=	     0;PhiROImin[0][ 2][25]=	      0;PhiROImax[0][ 2][25]=	       0;
     EtaROImin[0][ 2][26]=	    0;EtaROImax[0][ 2][26]=	     0;PhiROImin[0][ 2][26]=	      0;PhiROImax[0][ 2][26]=	       0;
     EtaROImin[0][ 2][27]=	    0;EtaROImax[0][ 2][27]=	     0;PhiROImin[0][ 2][27]=	      0;PhiROImax[0][ 2][27]=	       0;
     EtaROImin[0][ 2][28]=	    0;EtaROImax[0][ 2][28]=	     0;PhiROImin[0][ 2][28]=	      0;PhiROImax[0][ 2][28]=	       0;
     EtaROImin[0][ 2][29]=	    0;EtaROImax[0][ 2][29]=	     0;PhiROImin[0][ 2][29]=	      0;PhiROImax[0][ 2][29]=	       0;
     EtaROImin[0][ 2][30]=	    0;EtaROImax[0][ 2][30]=	     0;PhiROImin[0][ 2][30]=	      0;PhiROImax[0][ 2][30]=	       0;
     EtaROImin[0][ 2][31]=	    0;EtaROImax[0][ 2][31]=	     0;PhiROImin[0][ 2][31]=	      0;PhiROImax[0][ 2][31]=	       0;
     EtaROImin[0][ 3][ 0]=  -0.238532;EtaROImax[0][ 3][ 0]=  -0.147302;PhiROImin[0][ 3][ 0]=   0.672786;PhiROImax[0][ 3][ 0]=	0.782446;
     EtaROImin[0][ 3][ 1]=  -0.147437;EtaROImax[0][ 3][ 1]= -0.0554683;PhiROImin[0][ 3][ 1]=   0.672786;PhiROImax[0][ 3][ 1]=	0.782446;
     EtaROImin[0][ 3][ 2]=  -0.238532;EtaROImax[0][ 3][ 2]=  -0.147302;PhiROImin[0][ 3][ 2]=   0.562387;PhiROImax[0][ 3][ 2]=	0.669279;
     EtaROImin[0][ 3][ 3]=  -0.147437;EtaROImax[0][ 3][ 3]= -0.0554683;PhiROImin[0][ 3][ 3]=   0.562387;PhiROImax[0][ 3][ 3]=	0.669279;
     EtaROImin[0][ 3][ 4]=  -0.452099;EtaROImax[0][ 3][ 4]=  -0.350831;PhiROImin[0][ 3][ 4]=   0.672786;PhiROImax[0][ 3][ 4]=	0.782446;
     EtaROImin[0][ 3][ 5]=   -0.35049;EtaROImax[0][ 3][ 5]=  -0.246207;PhiROImin[0][ 3][ 5]=   0.672786;PhiROImax[0][ 3][ 5]=	0.782446;
     EtaROImin[0][ 3][ 6]=  -0.452099;EtaROImax[0][ 3][ 6]=  -0.350831;PhiROImin[0][ 3][ 6]=   0.562387;PhiROImax[0][ 3][ 6]=	0.669279;
     EtaROImin[0][ 3][ 7]=   -0.35049;EtaROImax[0][ 3][ 7]=  -0.246207;PhiROImin[0][ 3][ 7]=   0.562387;PhiROImax[0][ 3][ 7]=	0.669279;
     EtaROImin[0][ 3][ 8]=  -0.648101;EtaROImax[0][ 3][ 8]=  -0.555789;PhiROImin[0][ 3][ 8]=   0.672786;PhiROImax[0][ 3][ 8]=	0.782446;
     EtaROImin[0][ 3][ 9]=  -0.554444;EtaROImax[0][ 3][ 9]=  -0.458091;PhiROImin[0][ 3][ 9]=   0.672786;PhiROImax[0][ 3][ 9]=	0.782446;
     EtaROImin[0][ 3][10]=  -0.648101;EtaROImax[0][ 3][10]=  -0.555789;PhiROImin[0][ 3][10]=   0.562387;PhiROImax[0][ 3][10]=	0.669279;
     EtaROImin[0][ 3][11]=  -0.554444;EtaROImax[0][ 3][11]=  -0.458091;PhiROImin[0][ 3][11]=   0.562387;PhiROImax[0][ 3][11]=	0.669279;
     EtaROImin[0][ 3][12]=  -0.776449;EtaROImax[0][ 3][12]=  -0.731124;PhiROImin[0][ 3][12]=   0.672186;PhiROImax[0][ 3][12]=	 0.78243;
     EtaROImin[0][ 3][13]=  -0.728056;EtaROImax[0][ 3][13]=  -0.655857;PhiROImin[0][ 3][13]=   0.672186;PhiROImax[0][ 3][13]=	 0.78243;
     EtaROImin[0][ 3][14]=  -0.776449;EtaROImax[0][ 3][14]=  -0.731124;PhiROImin[0][ 3][14]=   0.561229;PhiROImax[0][ 3][14]=	0.668661;
     EtaROImin[0][ 3][15]=  -0.728056;EtaROImax[0][ 3][15]=  -0.655857;PhiROImin[0][ 3][15]=   0.561229;PhiROImax[0][ 3][15]=	0.668661;
     EtaROImin[0][ 3][16]=  -0.895163;EtaROImax[0][ 3][16]=  -0.853359;PhiROImin[0][ 3][16]=   0.672186;PhiROImax[0][ 3][16]=	 0.78243;
     EtaROImin[0][ 3][17]=  -0.850528;EtaROImax[0][ 3][17]=  -0.783867;PhiROImin[0][ 3][17]=   0.672186;PhiROImax[0][ 3][17]=	 0.78243;
     EtaROImin[0][ 3][18]=  -0.895163;EtaROImax[0][ 3][18]=  -0.853359;PhiROImin[0][ 3][18]=   0.561229;PhiROImax[0][ 3][18]=	0.668661;
     EtaROImin[0][ 3][19]=  -0.850528;EtaROImax[0][ 3][19]=  -0.783867;PhiROImin[0][ 3][19]=   0.561229;PhiROImax[0][ 3][19]=	0.668661;
     EtaROImin[0][ 3][20]=   -1.02514;EtaROImax[0][ 3][20]=  -0.964674;PhiROImin[0][ 3][20]=   0.672786;PhiROImax[0][ 3][20]=	0.782446;
     EtaROImin[0][ 3][21]=  -0.961344;EtaROImax[0][ 3][21]=  -0.898201;PhiROImin[0][ 3][21]=   0.672786;PhiROImax[0][ 3][21]=	0.782446;
     EtaROImin[0][ 3][22]=   -1.02514;EtaROImax[0][ 3][22]=  -0.964674;PhiROImin[0][ 3][22]=   0.562387;PhiROImax[0][ 3][22]=	0.669279;
     EtaROImin[0][ 3][23]=  -0.961344;EtaROImax[0][ 3][23]=  -0.898201;PhiROImin[0][ 3][23]=   0.562387;PhiROImax[0][ 3][23]=	0.669279;
     EtaROImin[0][ 3][24]=	    0;EtaROImax[0][ 3][24]=	     0;PhiROImin[0][ 3][24]=	      0;PhiROImax[0][ 3][24]=	       0;
     EtaROImin[0][ 3][25]=   -1.06547;EtaROImax[0][ 3][25]=   -1.03003;PhiROImin[0][ 3][25]=   0.671969;PhiROImax[0][ 3][25]=	0.782329;
     EtaROImin[0][ 3][26]=	    0;EtaROImax[0][ 3][26]=	     0;PhiROImin[0][ 3][26]=	      0;PhiROImax[0][ 3][26]=	       0;
     EtaROImin[0][ 3][27]=   -1.06547;EtaROImax[0][ 3][27]=   -1.03003;PhiROImin[0][ 3][27]=   0.588255;PhiROImax[0][ 3][27]=	 0.66844;
     EtaROImin[0][ 3][28]=	    0;EtaROImax[0][ 3][28]=	     0;PhiROImin[0][ 3][28]=	      0;PhiROImax[0][ 3][28]=	       0;
     EtaROImin[0][ 3][29]=	    0;EtaROImax[0][ 3][29]=	     0;PhiROImin[0][ 3][29]=	      0;PhiROImax[0][ 3][29]=	       0;
     EtaROImin[0][ 3][30]=	    0;EtaROImax[0][ 3][30]=	     0;PhiROImin[0][ 3][30]=	      0;PhiROImax[0][ 3][30]=	       0;
     EtaROImin[0][ 3][31]=	    0;EtaROImax[0][ 3][31]=	     0;PhiROImin[0][ 3][31]=	      0;PhiROImax[0][ 3][31]=	       0;
     EtaROImin[0][ 4][ 0]=  -0.147437;EtaROImax[0][ 4][ 0]= -0.0554683;PhiROImin[0][ 4][ 0]=	0.78835;PhiROImax[0][ 4][ 0]=	0.898011;
     EtaROImin[0][ 4][ 1]=  -0.238532;EtaROImax[0][ 4][ 1]=  -0.147302;PhiROImin[0][ 4][ 1]=	0.78835;PhiROImax[0][ 4][ 1]=	0.898011;
     EtaROImin[0][ 4][ 2]=  -0.147437;EtaROImax[0][ 4][ 2]= -0.0554683;PhiROImin[0][ 4][ 2]=   0.901517;PhiROImax[0][ 4][ 2]=	 1.00841;
     EtaROImin[0][ 4][ 3]=  -0.238532;EtaROImax[0][ 4][ 3]=  -0.147302;PhiROImin[0][ 4][ 3]=   0.901517;PhiROImax[0][ 4][ 3]=	 1.00841;
     EtaROImin[0][ 4][ 4]=   -0.35049;EtaROImax[0][ 4][ 4]=  -0.246207;PhiROImin[0][ 4][ 4]=	0.78835;PhiROImax[0][ 4][ 4]=	0.898011;
     EtaROImin[0][ 4][ 5]=  -0.452099;EtaROImax[0][ 4][ 5]=  -0.350831;PhiROImin[0][ 4][ 5]=	0.78835;PhiROImax[0][ 4][ 5]=	0.898011;
     EtaROImin[0][ 4][ 6]=   -0.35049;EtaROImax[0][ 4][ 6]=  -0.246207;PhiROImin[0][ 4][ 6]=   0.901517;PhiROImax[0][ 4][ 6]=	 1.00841;
     EtaROImin[0][ 4][ 7]=  -0.452099;EtaROImax[0][ 4][ 7]=  -0.350831;PhiROImin[0][ 4][ 7]=   0.901517;PhiROImax[0][ 4][ 7]=	 1.00841;
     EtaROImin[0][ 4][ 8]=  -0.554444;EtaROImax[0][ 4][ 8]=  -0.458091;PhiROImin[0][ 4][ 8]=	0.78835;PhiROImax[0][ 4][ 8]=	0.898011;
     EtaROImin[0][ 4][ 9]=  -0.648101;EtaROImax[0][ 4][ 9]=  -0.555789;PhiROImin[0][ 4][ 9]=	0.78835;PhiROImax[0][ 4][ 9]=	0.898011;
     EtaROImin[0][ 4][10]=  -0.554444;EtaROImax[0][ 4][10]=  -0.458091;PhiROImin[0][ 4][10]=   0.901517;PhiROImax[0][ 4][10]=	 1.00841;
     EtaROImin[0][ 4][11]=  -0.648101;EtaROImax[0][ 4][11]=  -0.555789;PhiROImin[0][ 4][11]=   0.901517;PhiROImax[0][ 4][11]=	 1.00841;
     EtaROImin[0][ 4][12]=  -0.703298;EtaROImax[0][ 4][12]=  -0.655857;PhiROImin[0][ 4][12]=   0.788366;PhiROImax[0][ 4][12]=	 0.89861;
     EtaROImin[0][ 4][13]=  -0.776449;EtaROImax[0][ 4][13]=  -0.706413;PhiROImin[0][ 4][13]=   0.788366;PhiROImax[0][ 4][13]=	 0.89861;
     EtaROImin[0][ 4][14]=  -0.703298;EtaROImax[0][ 4][14]=  -0.655857;PhiROImin[0][ 4][14]=   0.902135;PhiROImax[0][ 4][14]=	 1.00957;
     EtaROImin[0][ 4][15]=  -0.776449;EtaROImax[0][ 4][15]=  -0.706413;PhiROImin[0][ 4][15]=   0.902135;PhiROImax[0][ 4][15]=	 1.00957;
     EtaROImin[0][ 4][16]=   -0.82768;EtaROImax[0][ 4][16]=  -0.783867;PhiROImin[0][ 4][16]=   0.788366;PhiROImax[0][ 4][16]=	 0.89861;
     EtaROImin[0][ 4][17]=  -0.895163;EtaROImax[0][ 4][17]=  -0.830556;PhiROImin[0][ 4][17]=   0.788366;PhiROImax[0][ 4][17]=	 0.89861;
     EtaROImin[0][ 4][18]=   -0.82768;EtaROImax[0][ 4][18]=  -0.783867;PhiROImin[0][ 4][18]=   0.902135;PhiROImax[0][ 4][18]=	 1.00957;
     EtaROImin[0][ 4][19]=  -0.895163;EtaROImax[0][ 4][19]=  -0.830556;PhiROImin[0][ 4][19]=   0.902135;PhiROImax[0][ 4][19]=	 1.00957;
     EtaROImin[0][ 4][20]=  -0.961344;EtaROImax[0][ 4][20]=  -0.898201;PhiROImin[0][ 4][20]=	0.78835;PhiROImax[0][ 4][20]=	0.898011;
     EtaROImin[0][ 4][21]=   -1.02514;EtaROImax[0][ 4][21]=  -0.964674;PhiROImin[0][ 4][21]=	0.78835;PhiROImax[0][ 4][21]=	0.898011;
     EtaROImin[0][ 4][22]=  -0.961344;EtaROImax[0][ 4][22]=  -0.898201;PhiROImin[0][ 4][22]=   0.901517;PhiROImax[0][ 4][22]=	 1.00841;
     EtaROImin[0][ 4][23]=   -1.02514;EtaROImax[0][ 4][23]=  -0.964674;PhiROImin[0][ 4][23]=   0.901517;PhiROImax[0][ 4][23]=	 1.00841;
     EtaROImin[0][ 4][24]=   -1.06547;EtaROImax[0][ 4][24]=   -1.03003;PhiROImin[0][ 4][24]=   0.788467;PhiROImax[0][ 4][24]=	0.898827;
     EtaROImin[0][ 4][25]=	    0;EtaROImax[0][ 4][25]=	     0;PhiROImin[0][ 4][25]=	      0;PhiROImax[0][ 4][25]=	       0;
     EtaROImin[0][ 4][26]=   -1.06547;EtaROImax[0][ 4][26]=   -1.03003;PhiROImin[0][ 4][26]=   0.902356;PhiROImax[0][ 4][26]=	0.982541;
     EtaROImin[0][ 4][27]=	    0;EtaROImax[0][ 4][27]=	     0;PhiROImin[0][ 4][27]=	      0;PhiROImax[0][ 4][27]=	       0;
     EtaROImin[0][ 4][28]=	    0;EtaROImax[0][ 4][28]=	     0;PhiROImin[0][ 4][28]=	      0;PhiROImax[0][ 4][28]=	       0;
     EtaROImin[0][ 4][29]=	    0;EtaROImax[0][ 4][29]=	     0;PhiROImin[0][ 4][29]=	      0;PhiROImax[0][ 4][29]=	       0;
     EtaROImin[0][ 4][30]=	    0;EtaROImax[0][ 4][30]=	     0;PhiROImin[0][ 4][30]=	      0;PhiROImax[0][ 4][30]=	       0;
     EtaROImin[0][ 4][31]=	    0;EtaROImax[0][ 4][31]=	     0;PhiROImin[0][ 4][31]=	      0;PhiROImax[0][ 4][31]=	       0;
     EtaROImin[0][ 5][ 0]=  -0.213185;EtaROImax[0][ 5][ 0]=  -0.116816;PhiROImin[0][ 5][ 0]=	1.09135;PhiROImax[0][ 5][ 0]=	 1.17531;
     EtaROImin[0][ 5][ 1]=  -0.118734;EtaROImax[0][ 5][ 1]= -0.0208251;PhiROImin[0][ 5][ 1]=	1.09135;PhiROImax[0][ 5][ 1]=	 1.17531;
     EtaROImin[0][ 5][ 2]=  -0.213185;EtaROImax[0][ 5][ 2]=  -0.116816;PhiROImin[0][ 5][ 2]=	1.00505;PhiROImax[0][ 5][ 2]=	 1.08772;
     EtaROImin[0][ 5][ 3]=  -0.118734;EtaROImax[0][ 5][ 3]= -0.0208251;PhiROImin[0][ 5][ 3]=	1.00505;PhiROImax[0][ 5][ 3]=	 1.08772;
     EtaROImin[0][ 5][ 4]=  -0.416721;EtaROImax[0][ 5][ 4]=   -0.30075;PhiROImin[0][ 5][ 4]=	1.09135;PhiROImax[0][ 5][ 4]=	 1.17531;
     EtaROImin[0][ 5][ 5]=  -0.302928;EtaROImax[0][ 5][ 5]=  -0.222449;PhiROImin[0][ 5][ 5]=	1.09135;PhiROImax[0][ 5][ 5]=	 1.17531;
     EtaROImin[0][ 5][ 6]=  -0.416721;EtaROImax[0][ 5][ 6]=   -0.30075;PhiROImin[0][ 5][ 6]=	1.00505;PhiROImax[0][ 5][ 6]=	 1.08772;
     EtaROImin[0][ 5][ 7]=  -0.302928;EtaROImax[0][ 5][ 7]=  -0.222449;PhiROImin[0][ 5][ 7]=	1.00505;PhiROImax[0][ 5][ 7]=	 1.08772;
     EtaROImin[0][ 5][ 8]=  -0.573871;EtaROImax[0][ 5][ 8]=  -0.501681;PhiROImin[0][ 5][ 8]=	1.09135;PhiROImax[0][ 5][ 8]=	 1.17531;
     EtaROImin[0][ 5][ 9]=  -0.504617;EtaROImax[0][ 5][ 9]=   -0.42967;PhiROImin[0][ 5][ 9]=	1.09135;PhiROImax[0][ 5][ 9]=	 1.17531;
     EtaROImin[0][ 5][10]=  -0.573871;EtaROImax[0][ 5][10]=  -0.501681;PhiROImin[0][ 5][10]=	1.00505;PhiROImax[0][ 5][10]=	 1.08772;
     EtaROImin[0][ 5][11]=  -0.504617;EtaROImax[0][ 5][11]=   -0.42967;PhiROImin[0][ 5][11]=	1.00505;PhiROImax[0][ 5][11]=	 1.08772;
     EtaROImin[0][ 5][12]=  -0.741516;EtaROImax[0][ 5][12]=  -0.649933;PhiROImin[0][ 5][12]=	1.09135;PhiROImax[0][ 5][12]=	 1.17531;
     EtaROImin[0][ 5][13]=  -0.653329;EtaROImax[0][ 5][13]=  -0.583785;PhiROImin[0][ 5][13]=	1.09135;PhiROImax[0][ 5][13]=	 1.17531;
     EtaROImin[0][ 5][14]=  -0.741516;EtaROImax[0][ 5][14]=  -0.649933;PhiROImin[0][ 5][14]=	1.00505;PhiROImax[0][ 5][14]=	 1.08772;
     EtaROImin[0][ 5][15]=  -0.653329;EtaROImax[0][ 5][15]=  -0.583785;PhiROImin[0][ 5][15]=	1.00505;PhiROImax[0][ 5][15]=	 1.08772;
     EtaROImin[0][ 5][16]=	    0;EtaROImax[0][ 5][16]=	     0;PhiROImin[0][ 5][16]=	      0;PhiROImax[0][ 5][16]=	       0;
     EtaROImin[0][ 5][17]=  -0.837822;EtaROImax[0][ 5][17]=  -0.756521;PhiROImin[0][ 5][17]=	1.09135;PhiROImax[0][ 5][17]=	 1.17531;
     EtaROImin[0][ 5][18]=	    0;EtaROImax[0][ 5][18]=	     0;PhiROImin[0][ 5][18]=	      0;PhiROImax[0][ 5][18]=	       0;
     EtaROImin[0][ 5][19]=  -0.837822;EtaROImax[0][ 5][19]=  -0.756521;PhiROImin[0][ 5][19]=	1.00505;PhiROImax[0][ 5][19]=	 1.08772;
     EtaROImin[0][ 5][20]=  -0.956037;EtaROImax[0][ 5][20]=  -0.899344;PhiROImin[0][ 5][20]=	1.09135;PhiROImax[0][ 5][20]=	 1.17531;
     EtaROImin[0][ 5][21]=  -0.903324;EtaROImax[0][ 5][21]=  -0.844116;PhiROImin[0][ 5][21]=	1.09135;PhiROImax[0][ 5][21]=	 1.17531;
     EtaROImin[0][ 5][22]=  -0.956037;EtaROImax[0][ 5][22]=  -0.899344;PhiROImin[0][ 5][22]=	1.00505;PhiROImax[0][ 5][22]=	 1.08772;
     EtaROImin[0][ 5][23]=  -0.903324;EtaROImax[0][ 5][23]=  -0.844116;PhiROImin[0][ 5][23]=	1.00505;PhiROImax[0][ 5][23]=	 1.08772;
     EtaROImin[0][ 5][24]=	    0;EtaROImax[0][ 5][24]=	     0;PhiROImin[0][ 5][24]=	      0;PhiROImax[0][ 5][24]=	       0;
     EtaROImin[0][ 5][25]=	    0;EtaROImax[0][ 5][25]=	     0;PhiROImin[0][ 5][25]=	      0;PhiROImax[0][ 5][25]=	       0;
     EtaROImin[0][ 5][26]=	    0;EtaROImax[0][ 5][26]=	     0;PhiROImin[0][ 5][26]=	      0;PhiROImax[0][ 5][26]=	       0;
     EtaROImin[0][ 5][27]=	    0;EtaROImax[0][ 5][27]=	     0;PhiROImin[0][ 5][27]=	      0;PhiROImax[0][ 5][27]=	       0;
     EtaROImin[0][ 5][28]=	    0;EtaROImax[0][ 5][28]=	     0;PhiROImin[0][ 5][28]=	      0;PhiROImax[0][ 5][28]=	       0;
     EtaROImin[0][ 5][29]=	    0;EtaROImax[0][ 5][29]=	     0;PhiROImin[0][ 5][29]=	      0;PhiROImax[0][ 5][29]=	       0;
     EtaROImin[0][ 5][30]=	    0;EtaROImax[0][ 5][30]=	     0;PhiROImin[0][ 5][30]=	      0;PhiROImax[0][ 5][30]=	       0;
     EtaROImin[0][ 5][31]=	    0;EtaROImax[0][ 5][31]=	     0;PhiROImin[0][ 5][31]=	      0;PhiROImax[0][ 5][31]=	       0;
     EtaROImin[0][ 6][ 0]=  -0.118734;EtaROImax[0][ 6][ 0]= -0.0208251;PhiROImin[0][ 6][ 0]=	1.18089;PhiROImax[0][ 6][ 0]=	 1.26484;
     EtaROImin[0][ 6][ 1]=  -0.213185;EtaROImax[0][ 6][ 1]=  -0.116816;PhiROImin[0][ 6][ 1]=	1.18089;PhiROImax[0][ 6][ 1]=	 1.26484;
     EtaROImin[0][ 6][ 2]=  -0.118734;EtaROImax[0][ 6][ 2]= -0.0208251;PhiROImin[0][ 6][ 2]=	1.26847;PhiROImax[0][ 6][ 2]=	 1.35115;
     EtaROImin[0][ 6][ 3]=  -0.213185;EtaROImax[0][ 6][ 3]=  -0.116816;PhiROImin[0][ 6][ 3]=	1.26847;PhiROImax[0][ 6][ 3]=	 1.35115;
     EtaROImin[0][ 6][ 4]=  -0.302928;EtaROImax[0][ 6][ 4]=  -0.222449;PhiROImin[0][ 6][ 4]=	1.18089;PhiROImax[0][ 6][ 4]=	 1.26484;
     EtaROImin[0][ 6][ 5]=  -0.416721;EtaROImax[0][ 6][ 5]=   -0.30075;PhiROImin[0][ 6][ 5]=	1.18089;PhiROImax[0][ 6][ 5]=	 1.26484;
     EtaROImin[0][ 6][ 6]=  -0.302928;EtaROImax[0][ 6][ 6]=  -0.222449;PhiROImin[0][ 6][ 6]=	1.26847;PhiROImax[0][ 6][ 6]=	 1.35115;
     EtaROImin[0][ 6][ 7]=  -0.416721;EtaROImax[0][ 6][ 7]=   -0.30075;PhiROImin[0][ 6][ 7]=	1.26847;PhiROImax[0][ 6][ 7]=	 1.35115;
     EtaROImin[0][ 6][ 8]=  -0.504617;EtaROImax[0][ 6][ 8]=   -0.42967;PhiROImin[0][ 6][ 8]=	1.18089;PhiROImax[0][ 6][ 8]=	 1.26484;
     EtaROImin[0][ 6][ 9]=  -0.573871;EtaROImax[0][ 6][ 9]=  -0.501681;PhiROImin[0][ 6][ 9]=	1.18089;PhiROImax[0][ 6][ 9]=	 1.26484;
     EtaROImin[0][ 6][10]=  -0.504617;EtaROImax[0][ 6][10]=   -0.42967;PhiROImin[0][ 6][10]=	1.26847;PhiROImax[0][ 6][10]=	 1.35115;
     EtaROImin[0][ 6][11]=  -0.573871;EtaROImax[0][ 6][11]=  -0.501681;PhiROImin[0][ 6][11]=	1.26847;PhiROImax[0][ 6][11]=	 1.35115;
     EtaROImin[0][ 6][12]=  -0.653329;EtaROImax[0][ 6][12]=  -0.583785;PhiROImin[0][ 6][12]=	1.18089;PhiROImax[0][ 6][12]=	 1.26484;
     EtaROImin[0][ 6][13]=  -0.741516;EtaROImax[0][ 6][13]=  -0.649933;PhiROImin[0][ 6][13]=	1.18089;PhiROImax[0][ 6][13]=	 1.26484;
     EtaROImin[0][ 6][14]=  -0.653329;EtaROImax[0][ 6][14]=  -0.583785;PhiROImin[0][ 6][14]=	1.26847;PhiROImax[0][ 6][14]=	 1.35115;
     EtaROImin[0][ 6][15]=  -0.741516;EtaROImax[0][ 6][15]=  -0.649933;PhiROImin[0][ 6][15]=	1.26847;PhiROImax[0][ 6][15]=	 1.35115;
     EtaROImin[0][ 6][16]=  -0.837822;EtaROImax[0][ 6][16]=  -0.756521;PhiROImin[0][ 6][16]=	1.18089;PhiROImax[0][ 6][16]=	 1.26484;
     EtaROImin[0][ 6][17]=	    0;EtaROImax[0][ 6][17]=	     0;PhiROImin[0][ 6][17]=	      0;PhiROImax[0][ 6][17]=	       0;
     EtaROImin[0][ 6][18]=  -0.837822;EtaROImax[0][ 6][18]=  -0.756521;PhiROImin[0][ 6][18]=	1.26847;PhiROImax[0][ 6][18]=	 1.35115;
     EtaROImin[0][ 6][19]=	    0;EtaROImax[0][ 6][19]=	     0;PhiROImin[0][ 6][19]=	      0;PhiROImax[0][ 6][19]=	       0;
     EtaROImin[0][ 6][20]=  -0.903324;EtaROImax[0][ 6][20]=  -0.844116;PhiROImin[0][ 6][20]=	1.18089;PhiROImax[0][ 6][20]=	 1.26484;
     EtaROImin[0][ 6][21]=  -0.956037;EtaROImax[0][ 6][21]=  -0.899344;PhiROImin[0][ 6][21]=	1.18089;PhiROImax[0][ 6][21]=	 1.26484;
     EtaROImin[0][ 6][22]=  -0.903324;EtaROImax[0][ 6][22]=  -0.844116;PhiROImin[0][ 6][22]=	1.26847;PhiROImax[0][ 6][22]=	 1.35115;
     EtaROImin[0][ 6][23]=  -0.956037;EtaROImax[0][ 6][23]=  -0.899344;PhiROImin[0][ 6][23]=	1.26847;PhiROImax[0][ 6][23]=	 1.35115;
     EtaROImin[0][ 6][24]=	    0;EtaROImax[0][ 6][24]=	     0;PhiROImin[0][ 6][24]=	      0;PhiROImax[0][ 6][24]=	       0;
     EtaROImin[0][ 6][25]=	    0;EtaROImax[0][ 6][25]=	     0;PhiROImin[0][ 6][25]=	      0;PhiROImax[0][ 6][25]=	       0;
     EtaROImin[0][ 6][26]=	    0;EtaROImax[0][ 6][26]=	     0;PhiROImin[0][ 6][26]=	      0;PhiROImax[0][ 6][26]=	       0;
     EtaROImin[0][ 6][27]=	    0;EtaROImax[0][ 6][27]=	     0;PhiROImin[0][ 6][27]=	      0;PhiROImax[0][ 6][27]=	       0;
     EtaROImin[0][ 6][28]=	    0;EtaROImax[0][ 6][28]=	     0;PhiROImin[0][ 6][28]=	      0;PhiROImax[0][ 6][28]=	       0;
     EtaROImin[0][ 6][29]=	    0;EtaROImax[0][ 6][29]=	     0;PhiROImin[0][ 6][29]=	      0;PhiROImax[0][ 6][29]=	       0;
     EtaROImin[0][ 6][30]=	    0;EtaROImax[0][ 6][30]=	     0;PhiROImin[0][ 6][30]=	      0;PhiROImax[0][ 6][30]=	       0;
     EtaROImin[0][ 6][31]=	    0;EtaROImax[0][ 6][31]=	     0;PhiROImin[0][ 6][31]=	      0;PhiROImax[0][ 6][31]=	       0;
     EtaROImin[0][ 7][ 0]=  -0.238313;EtaROImax[0][ 7][ 0]=   -0.14708;PhiROImin[0][ 7][ 0]=	1.45818;PhiROImax[0][ 7][ 0]=	 1.56784;
     EtaROImin[0][ 7][ 1]=  -0.147216;EtaROImax[0][ 7][ 1]= -0.0552456;PhiROImin[0][ 7][ 1]=	1.45818;PhiROImax[0][ 7][ 1]=	 1.56784;
     EtaROImin[0][ 7][ 2]=  -0.238313;EtaROImax[0][ 7][ 2]=   -0.14708;PhiROImin[0][ 7][ 2]=	1.34779;PhiROImax[0][ 7][ 2]=	 1.45468;
     EtaROImin[0][ 7][ 3]=  -0.147216;EtaROImax[0][ 7][ 3]= -0.0552456;PhiROImin[0][ 7][ 3]=	1.34779;PhiROImax[0][ 7][ 3]=	 1.45468;
     EtaROImin[0][ 7][ 4]=  -0.452099;EtaROImax[0][ 7][ 4]=  -0.350831;PhiROImin[0][ 7][ 4]=	1.45818;PhiROImax[0][ 7][ 4]=	 1.56784;
     EtaROImin[0][ 7][ 5]=   -0.35049;EtaROImax[0][ 7][ 5]=  -0.246207;PhiROImin[0][ 7][ 5]=	1.45818;PhiROImax[0][ 7][ 5]=	 1.56784;
     EtaROImin[0][ 7][ 6]=  -0.452099;EtaROImax[0][ 7][ 6]=  -0.350831;PhiROImin[0][ 7][ 6]=	1.34779;PhiROImax[0][ 7][ 6]=	 1.45468;
     EtaROImin[0][ 7][ 7]=   -0.35049;EtaROImax[0][ 7][ 7]=  -0.246207;PhiROImin[0][ 7][ 7]=	1.34779;PhiROImax[0][ 7][ 7]=	 1.45468;
     EtaROImin[0][ 7][ 8]=  -0.648101;EtaROImax[0][ 7][ 8]=  -0.555789;PhiROImin[0][ 7][ 8]=	1.45818;PhiROImax[0][ 7][ 8]=	 1.56784;
     EtaROImin[0][ 7][ 9]=  -0.554444;EtaROImax[0][ 7][ 9]=  -0.458091;PhiROImin[0][ 7][ 9]=	1.45818;PhiROImax[0][ 7][ 9]=	 1.56784;
     EtaROImin[0][ 7][10]=  -0.648101;EtaROImax[0][ 7][10]=  -0.555789;PhiROImin[0][ 7][10]=	1.34779;PhiROImax[0][ 7][10]=	 1.45468;
     EtaROImin[0][ 7][11]=  -0.554444;EtaROImax[0][ 7][11]=  -0.458091;PhiROImin[0][ 7][11]=	1.34779;PhiROImax[0][ 7][11]=	 1.45468;
     EtaROImin[0][ 7][12]=  -0.776449;EtaROImax[0][ 7][12]=  -0.731124;PhiROImin[0][ 7][12]=	1.45758;PhiROImax[0][ 7][12]=	 1.56783;
     EtaROImin[0][ 7][13]=  -0.728056;EtaROImax[0][ 7][13]=  -0.655857;PhiROImin[0][ 7][13]=	1.45758;PhiROImax[0][ 7][13]=	 1.56783;
     EtaROImin[0][ 7][14]=  -0.776449;EtaROImax[0][ 7][14]=  -0.731124;PhiROImin[0][ 7][14]=	1.34663;PhiROImax[0][ 7][14]=	 1.45406;
     EtaROImin[0][ 7][15]=  -0.728056;EtaROImax[0][ 7][15]=  -0.655857;PhiROImin[0][ 7][15]=	1.34663;PhiROImax[0][ 7][15]=	 1.45406;
     EtaROImin[0][ 7][16]=  -0.895163;EtaROImax[0][ 7][16]=  -0.853359;PhiROImin[0][ 7][16]=	1.45758;PhiROImax[0][ 7][16]=	 1.56783;
     EtaROImin[0][ 7][17]=  -0.850528;EtaROImax[0][ 7][17]=  -0.783867;PhiROImin[0][ 7][17]=	1.45758;PhiROImax[0][ 7][17]=	 1.56783;
     EtaROImin[0][ 7][18]=  -0.895163;EtaROImax[0][ 7][18]=  -0.853359;PhiROImin[0][ 7][18]=	1.34663;PhiROImax[0][ 7][18]=	 1.45406;
     EtaROImin[0][ 7][19]=  -0.850528;EtaROImax[0][ 7][19]=  -0.783867;PhiROImin[0][ 7][19]=	1.34663;PhiROImax[0][ 7][19]=	 1.45406;
     EtaROImin[0][ 7][20]=   -1.02514;EtaROImax[0][ 7][20]=  -0.964674;PhiROImin[0][ 7][20]=	1.45818;PhiROImax[0][ 7][20]=	 1.56784;
     EtaROImin[0][ 7][21]=  -0.961344;EtaROImax[0][ 7][21]=  -0.898201;PhiROImin[0][ 7][21]=	1.45818;PhiROImax[0][ 7][21]=	 1.56784;
     EtaROImin[0][ 7][22]=   -1.02514;EtaROImax[0][ 7][22]=  -0.964674;PhiROImin[0][ 7][22]=	1.34779;PhiROImax[0][ 7][22]=	 1.45468;
     EtaROImin[0][ 7][23]=  -0.961344;EtaROImax[0][ 7][23]=  -0.898201;PhiROImin[0][ 7][23]=	1.34779;PhiROImax[0][ 7][23]=	 1.45468;
     EtaROImin[0][ 7][24]=	    0;EtaROImax[0][ 7][24]=	     0;PhiROImin[0][ 7][24]=	      0;PhiROImax[0][ 7][24]=	       0;
     EtaROImin[0][ 7][25]=   -1.06547;EtaROImax[0][ 7][25]=   -1.03003;PhiROImin[0][ 7][25]=	1.45737;PhiROImax[0][ 7][25]=	 1.56773;
     EtaROImin[0][ 7][26]=	    0;EtaROImax[0][ 7][26]=	     0;PhiROImin[0][ 7][26]=	      0;PhiROImax[0][ 7][26]=	       0;
     EtaROImin[0][ 7][27]=   -1.06547;EtaROImax[0][ 7][27]=   -1.03003;PhiROImin[0][ 7][27]=	1.37365;PhiROImax[0][ 7][27]=	 1.45384;
     EtaROImin[0][ 7][28]=	    0;EtaROImax[0][ 7][28]=	     0;PhiROImin[0][ 7][28]=	      0;PhiROImax[0][ 7][28]=	       0;
     EtaROImin[0][ 7][29]=	    0;EtaROImax[0][ 7][29]=	     0;PhiROImin[0][ 7][29]=	      0;PhiROImax[0][ 7][29]=	       0;
     EtaROImin[0][ 7][30]=	    0;EtaROImax[0][ 7][30]=	     0;PhiROImin[0][ 7][30]=	      0;PhiROImax[0][ 7][30]=	       0;
     EtaROImin[0][ 7][31]=	    0;EtaROImax[0][ 7][31]=	     0;PhiROImin[0][ 7][31]=	      0;PhiROImax[0][ 7][31]=	       0;
     EtaROImin[0][ 8][ 0]=  -0.147216;EtaROImax[0][ 8][ 0]= -0.0552456;PhiROImin[0][ 8][ 0]=	1.57375;PhiROImax[0][ 8][ 0]=	 1.68341;
     EtaROImin[0][ 8][ 1]=  -0.238313;EtaROImax[0][ 8][ 1]=   -0.14708;PhiROImin[0][ 8][ 1]=	1.57375;PhiROImax[0][ 8][ 1]=	 1.68341;
     EtaROImin[0][ 8][ 2]=  -0.147216;EtaROImax[0][ 8][ 2]= -0.0552456;PhiROImin[0][ 8][ 2]=	1.68692;PhiROImax[0][ 8][ 2]=	 1.79381;
     EtaROImin[0][ 8][ 3]=  -0.238313;EtaROImax[0][ 8][ 3]=   -0.14708;PhiROImin[0][ 8][ 3]=	1.68692;PhiROImax[0][ 8][ 3]=	 1.79381;
     EtaROImin[0][ 8][ 4]=   -0.35049;EtaROImax[0][ 8][ 4]=  -0.246207;PhiROImin[0][ 8][ 4]=	1.57375;PhiROImax[0][ 8][ 4]=	 1.68341;
     EtaROImin[0][ 8][ 5]=  -0.452099;EtaROImax[0][ 8][ 5]=  -0.350831;PhiROImin[0][ 8][ 5]=	1.57375;PhiROImax[0][ 8][ 5]=	 1.68341;
     EtaROImin[0][ 8][ 6]=   -0.35049;EtaROImax[0][ 8][ 6]=  -0.246207;PhiROImin[0][ 8][ 6]=	1.68692;PhiROImax[0][ 8][ 6]=	 1.79381;
     EtaROImin[0][ 8][ 7]=  -0.452099;EtaROImax[0][ 8][ 7]=  -0.350831;PhiROImin[0][ 8][ 7]=	1.68692;PhiROImax[0][ 8][ 7]=	 1.79381;
     EtaROImin[0][ 8][ 8]=  -0.554444;EtaROImax[0][ 8][ 8]=  -0.458091;PhiROImin[0][ 8][ 8]=	1.57375;PhiROImax[0][ 8][ 8]=	 1.68341;
     EtaROImin[0][ 8][ 9]=  -0.648101;EtaROImax[0][ 8][ 9]=  -0.555789;PhiROImin[0][ 8][ 9]=	1.57375;PhiROImax[0][ 8][ 9]=	 1.68341;
     EtaROImin[0][ 8][10]=  -0.554444;EtaROImax[0][ 8][10]=  -0.458091;PhiROImin[0][ 8][10]=	1.68692;PhiROImax[0][ 8][10]=	 1.79381;
     EtaROImin[0][ 8][11]=  -0.648101;EtaROImax[0][ 8][11]=  -0.555789;PhiROImin[0][ 8][11]=	1.68692;PhiROImax[0][ 8][11]=	 1.79381;
     EtaROImin[0][ 8][12]=  -0.703298;EtaROImax[0][ 8][12]=  -0.655857;PhiROImin[0][ 8][12]=	1.57376;PhiROImax[0][ 8][12]=	 1.68401;
     EtaROImin[0][ 8][13]=  -0.776449;EtaROImax[0][ 8][13]=  -0.706413;PhiROImin[0][ 8][13]=	1.57376;PhiROImax[0][ 8][13]=	 1.68401;
     EtaROImin[0][ 8][14]=  -0.703298;EtaROImax[0][ 8][14]=  -0.655857;PhiROImin[0][ 8][14]=	1.68753;PhiROImax[0][ 8][14]=	 1.79497;
     EtaROImin[0][ 8][15]=  -0.776449;EtaROImax[0][ 8][15]=  -0.706413;PhiROImin[0][ 8][15]=	1.68753;PhiROImax[0][ 8][15]=	 1.79497;
     EtaROImin[0][ 8][16]=   -0.82768;EtaROImax[0][ 8][16]=  -0.783867;PhiROImin[0][ 8][16]=	1.57376;PhiROImax[0][ 8][16]=	 1.68401;
     EtaROImin[0][ 8][17]=  -0.895163;EtaROImax[0][ 8][17]=  -0.830556;PhiROImin[0][ 8][17]=	1.57376;PhiROImax[0][ 8][17]=	 1.68401;
     EtaROImin[0][ 8][18]=   -0.82768;EtaROImax[0][ 8][18]=  -0.783867;PhiROImin[0][ 8][18]=	1.68753;PhiROImax[0][ 8][18]=	 1.79497;
     EtaROImin[0][ 8][19]=  -0.895163;EtaROImax[0][ 8][19]=  -0.830556;PhiROImin[0][ 8][19]=	1.68753;PhiROImax[0][ 8][19]=	 1.79497;
     EtaROImin[0][ 8][20]=  -0.961344;EtaROImax[0][ 8][20]=  -0.898201;PhiROImin[0][ 8][20]=	1.57375;PhiROImax[0][ 8][20]=	 1.68341;
     EtaROImin[0][ 8][21]=   -1.02514;EtaROImax[0][ 8][21]=  -0.964674;PhiROImin[0][ 8][21]=	1.57375;PhiROImax[0][ 8][21]=	 1.68341;
     EtaROImin[0][ 8][22]=  -0.961344;EtaROImax[0][ 8][22]=  -0.898201;PhiROImin[0][ 8][22]=	1.68692;PhiROImax[0][ 8][22]=	 1.79381;
     EtaROImin[0][ 8][23]=   -1.02514;EtaROImax[0][ 8][23]=  -0.964674;PhiROImin[0][ 8][23]=	1.68692;PhiROImax[0][ 8][23]=	 1.79381;
     EtaROImin[0][ 8][24]=   -1.06547;EtaROImax[0][ 8][24]=   -1.03003;PhiROImin[0][ 8][24]=	1.57387;PhiROImax[0][ 8][24]=	 1.68423;
     EtaROImin[0][ 8][25]=	    0;EtaROImax[0][ 8][25]=	     0;PhiROImin[0][ 8][25]=	      0;PhiROImax[0][ 8][25]=	       0;
     EtaROImin[0][ 8][26]=   -1.06547;EtaROImax[0][ 8][26]=   -1.03003;PhiROImin[0][ 8][26]=	1.68775;PhiROImax[0][ 8][26]=	 1.76794;
     EtaROImin[0][ 8][27]=	    0;EtaROImax[0][ 8][27]=	     0;PhiROImin[0][ 8][27]=	      0;PhiROImax[0][ 8][27]=	       0;
     EtaROImin[0][ 8][28]=	    0;EtaROImax[0][ 8][28]=	     0;PhiROImin[0][ 8][28]=	      0;PhiROImax[0][ 8][28]=	       0;
     EtaROImin[0][ 8][29]=	    0;EtaROImax[0][ 8][29]=	     0;PhiROImin[0][ 8][29]=	      0;PhiROImax[0][ 8][29]=	       0;
     EtaROImin[0][ 8][30]=	    0;EtaROImax[0][ 8][30]=	     0;PhiROImin[0][ 8][30]=	      0;PhiROImax[0][ 8][30]=	       0;
     EtaROImin[0][ 8][31]=	    0;EtaROImax[0][ 8][31]=	     0;PhiROImin[0][ 8][31]=	      0;PhiROImax[0][ 8][31]=	       0;
     EtaROImin[0][ 9][ 0]=  -0.213185;EtaROImax[0][ 9][ 0]=  -0.116816;PhiROImin[0][ 9][ 0]=	1.87675;PhiROImax[0][ 9][ 0]=	 1.96071;
     EtaROImin[0][ 9][ 1]=  -0.118734;EtaROImax[0][ 9][ 1]= -0.0208251;PhiROImin[0][ 9][ 1]=	1.87675;PhiROImax[0][ 9][ 1]=	 1.96071;
     EtaROImin[0][ 9][ 2]=  -0.213185;EtaROImax[0][ 9][ 2]=  -0.116816;PhiROImin[0][ 9][ 2]=	1.79045;PhiROImax[0][ 9][ 2]=	 1.87312;
     EtaROImin[0][ 9][ 3]=  -0.118734;EtaROImax[0][ 9][ 3]= -0.0208251;PhiROImin[0][ 9][ 3]=	1.79045;PhiROImax[0][ 9][ 3]=	 1.87312;
     EtaROImin[0][ 9][ 4]=  -0.416721;EtaROImax[0][ 9][ 4]=   -0.30075;PhiROImin[0][ 9][ 4]=	1.87675;PhiROImax[0][ 9][ 4]=	 1.96071;
     EtaROImin[0][ 9][ 5]=  -0.302928;EtaROImax[0][ 9][ 5]=  -0.222449;PhiROImin[0][ 9][ 5]=	1.87675;PhiROImax[0][ 9][ 5]=	 1.96071;
     EtaROImin[0][ 9][ 6]=  -0.416721;EtaROImax[0][ 9][ 6]=   -0.30075;PhiROImin[0][ 9][ 6]=	1.79045;PhiROImax[0][ 9][ 6]=	 1.87312;
     EtaROImin[0][ 9][ 7]=  -0.302928;EtaROImax[0][ 9][ 7]=  -0.222449;PhiROImin[0][ 9][ 7]=	1.79045;PhiROImax[0][ 9][ 7]=	 1.87312;
     EtaROImin[0][ 9][ 8]=  -0.573871;EtaROImax[0][ 9][ 8]=  -0.501681;PhiROImin[0][ 9][ 8]=	1.87675;PhiROImax[0][ 9][ 8]=	 1.96071;
     EtaROImin[0][ 9][ 9]=  -0.504617;EtaROImax[0][ 9][ 9]=   -0.42967;PhiROImin[0][ 9][ 9]=	1.87675;PhiROImax[0][ 9][ 9]=	 1.96071;
     EtaROImin[0][ 9][10]=  -0.573871;EtaROImax[0][ 9][10]=  -0.501681;PhiROImin[0][ 9][10]=	1.79045;PhiROImax[0][ 9][10]=	 1.87312;
     EtaROImin[0][ 9][11]=  -0.504617;EtaROImax[0][ 9][11]=   -0.42967;PhiROImin[0][ 9][11]=	1.79045;PhiROImax[0][ 9][11]=	 1.87312;
     EtaROImin[0][ 9][12]=  -0.741516;EtaROImax[0][ 9][12]=  -0.649933;PhiROImin[0][ 9][12]=	1.87675;PhiROImax[0][ 9][12]=	 1.96071;
     EtaROImin[0][ 9][13]=  -0.653329;EtaROImax[0][ 9][13]=  -0.583785;PhiROImin[0][ 9][13]=	1.87675;PhiROImax[0][ 9][13]=	 1.96071;
     EtaROImin[0][ 9][14]=  -0.741516;EtaROImax[0][ 9][14]=  -0.649933;PhiROImin[0][ 9][14]=	1.79045;PhiROImax[0][ 9][14]=	 1.87312;
     EtaROImin[0][ 9][15]=  -0.653329;EtaROImax[0][ 9][15]=  -0.583785;PhiROImin[0][ 9][15]=	1.79045;PhiROImax[0][ 9][15]=	 1.87312;
     EtaROImin[0][ 9][16]=	    0;EtaROImax[0][ 9][16]=	     0;PhiROImin[0][ 9][16]=	      0;PhiROImax[0][ 9][16]=	       0;
     EtaROImin[0][ 9][17]=  -0.837822;EtaROImax[0][ 9][17]=  -0.756521;PhiROImin[0][ 9][17]=	1.87675;PhiROImax[0][ 9][17]=	 1.96071;
     EtaROImin[0][ 9][18]=	    0;EtaROImax[0][ 9][18]=	     0;PhiROImin[0][ 9][18]=	      0;PhiROImax[0][ 9][18]=	       0;
     EtaROImin[0][ 9][19]=  -0.837822;EtaROImax[0][ 9][19]=  -0.756521;PhiROImin[0][ 9][19]=	1.79045;PhiROImax[0][ 9][19]=	 1.87312;
     EtaROImin[0][ 9][20]=  -0.956037;EtaROImax[0][ 9][20]=  -0.899344;PhiROImin[0][ 9][20]=	1.87675;PhiROImax[0][ 9][20]=	 1.96071;
     EtaROImin[0][ 9][21]=  -0.903324;EtaROImax[0][ 9][21]=  -0.844116;PhiROImin[0][ 9][21]=	1.87675;PhiROImax[0][ 9][21]=	 1.96071;
     EtaROImin[0][ 9][22]=  -0.956037;EtaROImax[0][ 9][22]=  -0.899344;PhiROImin[0][ 9][22]=	1.79045;PhiROImax[0][ 9][22]=	 1.87312;
     EtaROImin[0][ 9][23]=  -0.903324;EtaROImax[0][ 9][23]=  -0.844116;PhiROImin[0][ 9][23]=	1.79045;PhiROImax[0][ 9][23]=	 1.87312;
     EtaROImin[0][ 9][24]=	    0;EtaROImax[0][ 9][24]=	     0;PhiROImin[0][ 9][24]=	      0;PhiROImax[0][ 9][24]=	       0;
     EtaROImin[0][ 9][25]=	    0;EtaROImax[0][ 9][25]=	     0;PhiROImin[0][ 9][25]=	      0;PhiROImax[0][ 9][25]=	       0;
     EtaROImin[0][ 9][26]=	    0;EtaROImax[0][ 9][26]=	     0;PhiROImin[0][ 9][26]=	      0;PhiROImax[0][ 9][26]=	       0;
     EtaROImin[0][ 9][27]=	    0;EtaROImax[0][ 9][27]=	     0;PhiROImin[0][ 9][27]=	      0;PhiROImax[0][ 9][27]=	       0;
     EtaROImin[0][ 9][28]=	    0;EtaROImax[0][ 9][28]=	     0;PhiROImin[0][ 9][28]=	      0;PhiROImax[0][ 9][28]=	       0;
     EtaROImin[0][ 9][29]=	    0;EtaROImax[0][ 9][29]=	     0;PhiROImin[0][ 9][29]=	      0;PhiROImax[0][ 9][29]=	       0;
     EtaROImin[0][ 9][30]=	    0;EtaROImax[0][ 9][30]=	     0;PhiROImin[0][ 9][30]=	      0;PhiROImax[0][ 9][30]=	       0;
     EtaROImin[0][ 9][31]=	    0;EtaROImax[0][ 9][31]=	     0;PhiROImin[0][ 9][31]=	      0;PhiROImax[0][ 9][31]=	       0;
     EtaROImin[0][10][ 0]=  -0.118734;EtaROImax[0][10][ 0]= -0.0208251;PhiROImin[0][10][ 0]=	1.96629;PhiROImax[0][10][ 0]=	 2.05024;
     EtaROImin[0][10][ 1]=  -0.213185;EtaROImax[0][10][ 1]=  -0.116816;PhiROImin[0][10][ 1]=	1.96629;PhiROImax[0][10][ 1]=	 2.05024;
     EtaROImin[0][10][ 2]=  -0.118734;EtaROImax[0][10][ 2]= -0.0208251;PhiROImin[0][10][ 2]=	2.05387;PhiROImax[0][10][ 2]=	 2.13655;
     EtaROImin[0][10][ 3]=  -0.213185;EtaROImax[0][10][ 3]=  -0.116816;PhiROImin[0][10][ 3]=	2.05387;PhiROImax[0][10][ 3]=	 2.13655;
     EtaROImin[0][10][ 4]=  -0.302928;EtaROImax[0][10][ 4]=  -0.222449;PhiROImin[0][10][ 4]=	1.96629;PhiROImax[0][10][ 4]=	 2.05024;
     EtaROImin[0][10][ 5]=  -0.416721;EtaROImax[0][10][ 5]=   -0.30075;PhiROImin[0][10][ 5]=	1.96629;PhiROImax[0][10][ 5]=	 2.05024;
     EtaROImin[0][10][ 6]=  -0.302928;EtaROImax[0][10][ 6]=  -0.222449;PhiROImin[0][10][ 6]=	2.05387;PhiROImax[0][10][ 6]=	 2.13655;
     EtaROImin[0][10][ 7]=  -0.416721;EtaROImax[0][10][ 7]=   -0.30075;PhiROImin[0][10][ 7]=	2.05387;PhiROImax[0][10][ 7]=	 2.13655;
     EtaROImin[0][10][ 8]=  -0.504617;EtaROImax[0][10][ 8]=   -0.42967;PhiROImin[0][10][ 8]=	1.96629;PhiROImax[0][10][ 8]=	 2.05024;
     EtaROImin[0][10][ 9]=  -0.573871;EtaROImax[0][10][ 9]=  -0.501681;PhiROImin[0][10][ 9]=	1.96629;PhiROImax[0][10][ 9]=	 2.05024;
     EtaROImin[0][10][10]=  -0.504617;EtaROImax[0][10][10]=   -0.42967;PhiROImin[0][10][10]=	2.05387;PhiROImax[0][10][10]=	 2.13655;
     EtaROImin[0][10][11]=  -0.573871;EtaROImax[0][10][11]=  -0.501681;PhiROImin[0][10][11]=	2.05387;PhiROImax[0][10][11]=	 2.13655;
     EtaROImin[0][10][12]=  -0.653329;EtaROImax[0][10][12]=  -0.583785;PhiROImin[0][10][12]=	1.96629;PhiROImax[0][10][12]=	 2.05024;
     EtaROImin[0][10][13]=  -0.741516;EtaROImax[0][10][13]=  -0.649933;PhiROImin[0][10][13]=	1.96629;PhiROImax[0][10][13]=	 2.05024;
     EtaROImin[0][10][14]=  -0.653329;EtaROImax[0][10][14]=  -0.583785;PhiROImin[0][10][14]=	2.05387;PhiROImax[0][10][14]=	 2.13655;
     EtaROImin[0][10][15]=  -0.741516;EtaROImax[0][10][15]=  -0.649933;PhiROImin[0][10][15]=	2.05387;PhiROImax[0][10][15]=	 2.13655;
     EtaROImin[0][10][16]=  -0.837822;EtaROImax[0][10][16]=  -0.756521;PhiROImin[0][10][16]=	1.96629;PhiROImax[0][10][16]=	 2.05024;
     EtaROImin[0][10][17]=	    0;EtaROImax[0][10][17]=	     0;PhiROImin[0][10][17]=	      0;PhiROImax[0][10][17]=	       0;
     EtaROImin[0][10][18]=  -0.837822;EtaROImax[0][10][18]=  -0.756521;PhiROImin[0][10][18]=	2.05387;PhiROImax[0][10][18]=	 2.13655;
     EtaROImin[0][10][19]=	    0;EtaROImax[0][10][19]=	     0;PhiROImin[0][10][19]=	      0;PhiROImax[0][10][19]=	       0;
     EtaROImin[0][10][20]=  -0.903324;EtaROImax[0][10][20]=  -0.844116;PhiROImin[0][10][20]=	1.96629;PhiROImax[0][10][20]=	 2.05024;
     EtaROImin[0][10][21]=  -0.956037;EtaROImax[0][10][21]=  -0.899344;PhiROImin[0][10][21]=	1.96629;PhiROImax[0][10][21]=	 2.05024;
     EtaROImin[0][10][22]=  -0.903324;EtaROImax[0][10][22]=  -0.844116;PhiROImin[0][10][22]=	2.05387;PhiROImax[0][10][22]=	 2.13655;
     EtaROImin[0][10][23]=  -0.956037;EtaROImax[0][10][23]=  -0.899344;PhiROImin[0][10][23]=	2.05387;PhiROImax[0][10][23]=	 2.13655;
     EtaROImin[0][10][24]=	    0;EtaROImax[0][10][24]=	     0;PhiROImin[0][10][24]=	      0;PhiROImax[0][10][24]=	       0;
     EtaROImin[0][10][25]=	    0;EtaROImax[0][10][25]=	     0;PhiROImin[0][10][25]=	      0;PhiROImax[0][10][25]=	       0;
     EtaROImin[0][10][26]=	    0;EtaROImax[0][10][26]=	     0;PhiROImin[0][10][26]=	      0;PhiROImax[0][10][26]=	       0;
     EtaROImin[0][10][27]=	    0;EtaROImax[0][10][27]=	     0;PhiROImin[0][10][27]=	      0;PhiROImax[0][10][27]=	       0;
     EtaROImin[0][10][28]=	    0;EtaROImax[0][10][28]=	     0;PhiROImin[0][10][28]=	      0;PhiROImax[0][10][28]=	       0;
     EtaROImin[0][10][29]=	    0;EtaROImax[0][10][29]=	     0;PhiROImin[0][10][29]=	      0;PhiROImax[0][10][29]=	       0;
     EtaROImin[0][10][30]=	    0;EtaROImax[0][10][30]=	     0;PhiROImin[0][10][30]=	      0;PhiROImax[0][10][30]=	       0;
     EtaROImin[0][10][31]=	    0;EtaROImax[0][10][31]=	     0;PhiROImin[0][10][31]=	      0;PhiROImax[0][10][31]=	       0;
     EtaROImin[0][11][ 0]=	    0;EtaROImax[0][11][ 0]=	     0;PhiROImin[0][11][ 0]=	      0;PhiROImax[0][11][ 0]=	       0;
     EtaROImin[0][11][ 1]=  -0.238612;EtaROImax[0][11][ 1]=  -0.130922;PhiROImin[0][11][ 1]=	2.24298;PhiROImax[0][11][ 1]=	 2.35323;
     EtaROImin[0][11][ 2]=	    0;EtaROImax[0][11][ 2]=	     0;PhiROImin[0][11][ 2]=	      0;PhiROImax[0][11][ 2]=	       0;
     EtaROImin[0][11][ 3]=  -0.238612;EtaROImax[0][11][ 3]=  -0.130922;PhiROImin[0][11][ 3]=	2.13203;PhiROImax[0][11][ 3]=	 2.23946;
     EtaROImin[0][11][ 4]=  -0.452099;EtaROImax[0][11][ 4]=  -0.350831;PhiROImin[0][11][ 4]=	2.24358;PhiROImax[0][11][ 4]=	 2.35324;
     EtaROImin[0][11][ 5]=   -0.35049;EtaROImax[0][11][ 5]=  -0.246207;PhiROImin[0][11][ 5]=	2.24358;PhiROImax[0][11][ 5]=	 2.35324;
     EtaROImin[0][11][ 6]=  -0.452099;EtaROImax[0][11][ 6]=  -0.350831;PhiROImin[0][11][ 6]=	2.13318;PhiROImax[0][11][ 6]=	 2.24008;
     EtaROImin[0][11][ 7]=   -0.35049;EtaROImax[0][11][ 7]=  -0.246207;PhiROImin[0][11][ 7]=	2.13318;PhiROImax[0][11][ 7]=	 2.24008;
     EtaROImin[0][11][ 8]=  -0.648101;EtaROImax[0][11][ 8]=  -0.555789;PhiROImin[0][11][ 8]=	2.24358;PhiROImax[0][11][ 8]=	 2.35324;
     EtaROImin[0][11][ 9]=  -0.554444;EtaROImax[0][11][ 9]=  -0.458091;PhiROImin[0][11][ 9]=	2.24358;PhiROImax[0][11][ 9]=	 2.35324;
     EtaROImin[0][11][10]=  -0.648101;EtaROImax[0][11][10]=  -0.555789;PhiROImin[0][11][10]=	2.13318;PhiROImax[0][11][10]=	 2.24008;
     EtaROImin[0][11][11]=  -0.554444;EtaROImax[0][11][11]=  -0.458091;PhiROImin[0][11][11]=	2.13318;PhiROImax[0][11][11]=	 2.24008;
     EtaROImin[0][11][12]=  -0.776449;EtaROImax[0][11][12]=  -0.731124;PhiROImin[0][11][12]=	2.24298;PhiROImax[0][11][12]=	 2.35323;
     EtaROImin[0][11][13]=  -0.728056;EtaROImax[0][11][13]=  -0.655857;PhiROImin[0][11][13]=	2.24298;PhiROImax[0][11][13]=	 2.35323;
     EtaROImin[0][11][14]=  -0.776449;EtaROImax[0][11][14]=  -0.731124;PhiROImin[0][11][14]=	2.13203;PhiROImax[0][11][14]=	 2.23946;
     EtaROImin[0][11][15]=  -0.728056;EtaROImax[0][11][15]=  -0.655857;PhiROImin[0][11][15]=	2.13203;PhiROImax[0][11][15]=	 2.23946;
     EtaROImin[0][11][16]=  -0.895163;EtaROImax[0][11][16]=  -0.853359;PhiROImin[0][11][16]=	2.24298;PhiROImax[0][11][16]=	 2.35323;
     EtaROImin[0][11][17]=  -0.850528;EtaROImax[0][11][17]=  -0.783867;PhiROImin[0][11][17]=	2.24298;PhiROImax[0][11][17]=	 2.35323;
     EtaROImin[0][11][18]=  -0.895163;EtaROImax[0][11][18]=  -0.853359;PhiROImin[0][11][18]=	2.13203;PhiROImax[0][11][18]=	 2.23946;
     EtaROImin[0][11][19]=  -0.850528;EtaROImax[0][11][19]=  -0.783867;PhiROImin[0][11][19]=	2.13203;PhiROImax[0][11][19]=	 2.23946;
     EtaROImin[0][11][20]=   -1.02514;EtaROImax[0][11][20]=  -0.964674;PhiROImin[0][11][20]=	2.24358;PhiROImax[0][11][20]=	 2.35324;
     EtaROImin[0][11][21]=  -0.961344;EtaROImax[0][11][21]=  -0.898201;PhiROImin[0][11][21]=	2.24358;PhiROImax[0][11][21]=	 2.35324;
     EtaROImin[0][11][22]=   -1.02514;EtaROImax[0][11][22]=  -0.964674;PhiROImin[0][11][22]=	2.13318;PhiROImax[0][11][22]=	 2.24008;
     EtaROImin[0][11][23]=  -0.961344;EtaROImax[0][11][23]=  -0.898201;PhiROImin[0][11][23]=	2.13318;PhiROImax[0][11][23]=	 2.24008;
     EtaROImin[0][11][24]=	    0;EtaROImax[0][11][24]=	     0;PhiROImin[0][11][24]=	      0;PhiROImax[0][11][24]=	       0;
     EtaROImin[0][11][25]=   -1.06547;EtaROImax[0][11][25]=   -1.03003;PhiROImin[0][11][25]=	2.24277;PhiROImax[0][11][25]=	 2.35313;
     EtaROImin[0][11][26]=	    0;EtaROImax[0][11][26]=	     0;PhiROImin[0][11][26]=	      0;PhiROImax[0][11][26]=	       0;
     EtaROImin[0][11][27]=   -1.06547;EtaROImax[0][11][27]=   -1.03003;PhiROImin[0][11][27]=	2.15905;PhiROImax[0][11][27]=	 2.23924;
     EtaROImin[0][11][28]=	    0;EtaROImax[0][11][28]=	     0;PhiROImin[0][11][28]=	      0;PhiROImax[0][11][28]=	       0;
     EtaROImin[0][11][29]=	    0;EtaROImax[0][11][29]=	     0;PhiROImin[0][11][29]=	      0;PhiROImax[0][11][29]=	       0;
     EtaROImin[0][11][30]=	    0;EtaROImax[0][11][30]=	     0;PhiROImin[0][11][30]=	      0;PhiROImax[0][11][30]=	       0;
     EtaROImin[0][11][31]=	    0;EtaROImax[0][11][31]=	     0;PhiROImin[0][11][31]=	      0;PhiROImax[0][11][31]=	       0;
     EtaROImin[0][12][ 0]=  -0.238612;EtaROImax[0][12][ 0]=  -0.130922;PhiROImin[0][12][ 0]=	2.35916;PhiROImax[0][12][ 0]=	 2.46941;
     EtaROImin[0][12][ 1]=	    0;EtaROImax[0][12][ 1]=	     0;PhiROImin[0][12][ 1]=	      0;PhiROImax[0][12][ 1]=	       0;
     EtaROImin[0][12][ 2]=  -0.238612;EtaROImax[0][12][ 2]=  -0.130922;PhiROImin[0][12][ 2]=	2.47293;PhiROImax[0][12][ 2]=	 2.58036;
     EtaROImin[0][12][ 3]=	    0;EtaROImax[0][12][ 3]=	     0;PhiROImin[0][12][ 3]=	      0;PhiROImax[0][12][ 3]=	       0;
     EtaROImin[0][12][ 4]=   -0.35049;EtaROImax[0][12][ 4]=  -0.246207;PhiROImin[0][12][ 4]=	2.35915;PhiROImax[0][12][ 4]=	 2.46881;
     EtaROImin[0][12][ 5]=  -0.452099;EtaROImax[0][12][ 5]=  -0.350831;PhiROImin[0][12][ 5]=	2.35915;PhiROImax[0][12][ 5]=	 2.46881;
     EtaROImin[0][12][ 6]=   -0.35049;EtaROImax[0][12][ 6]=  -0.246207;PhiROImin[0][12][ 6]=	2.47231;PhiROImax[0][12][ 6]=	 2.57921;
     EtaROImin[0][12][ 7]=  -0.452099;EtaROImax[0][12][ 7]=  -0.350831;PhiROImin[0][12][ 7]=	2.47231;PhiROImax[0][12][ 7]=	 2.57921;
     EtaROImin[0][12][ 8]=  -0.554444;EtaROImax[0][12][ 8]=  -0.458091;PhiROImin[0][12][ 8]=	2.35915;PhiROImax[0][12][ 8]=	 2.46881;
     EtaROImin[0][12][ 9]=  -0.648101;EtaROImax[0][12][ 9]=  -0.555789;PhiROImin[0][12][ 9]=	2.35915;PhiROImax[0][12][ 9]=	 2.46881;
     EtaROImin[0][12][10]=  -0.554444;EtaROImax[0][12][10]=  -0.458091;PhiROImin[0][12][10]=	2.47231;PhiROImax[0][12][10]=	 2.57921;
     EtaROImin[0][12][11]=  -0.648101;EtaROImax[0][12][11]=  -0.555789;PhiROImin[0][12][11]=	2.47231;PhiROImax[0][12][11]=	 2.57921;
     EtaROImin[0][12][12]=  -0.703298;EtaROImax[0][12][12]=  -0.655857;PhiROImin[0][12][12]=	2.35916;PhiROImax[0][12][12]=	 2.46941;
     EtaROImin[0][12][13]=  -0.776449;EtaROImax[0][12][13]=  -0.706413;PhiROImin[0][12][13]=	2.35916;PhiROImax[0][12][13]=	 2.46941;
     EtaROImin[0][12][14]=  -0.703298;EtaROImax[0][12][14]=  -0.655857;PhiROImin[0][12][14]=	2.47293;PhiROImax[0][12][14]=	 2.58036;
     EtaROImin[0][12][15]=  -0.776449;EtaROImax[0][12][15]=  -0.706413;PhiROImin[0][12][15]=	2.47293;PhiROImax[0][12][15]=	 2.58036;
     EtaROImin[0][12][16]=   -0.82768;EtaROImax[0][12][16]=  -0.783867;PhiROImin[0][12][16]=	2.35916;PhiROImax[0][12][16]=	 2.46941;
     EtaROImin[0][12][17]=  -0.895163;EtaROImax[0][12][17]=  -0.830556;PhiROImin[0][12][17]=	2.35916;PhiROImax[0][12][17]=	 2.46941;
     EtaROImin[0][12][18]=   -0.82768;EtaROImax[0][12][18]=  -0.783867;PhiROImin[0][12][18]=	2.47293;PhiROImax[0][12][18]=	 2.58036;
     EtaROImin[0][12][19]=  -0.895163;EtaROImax[0][12][19]=  -0.830556;PhiROImin[0][12][19]=	2.47293;PhiROImax[0][12][19]=	 2.58036;
     EtaROImin[0][12][20]=  -0.961344;EtaROImax[0][12][20]=  -0.898201;PhiROImin[0][12][20]=	2.35915;PhiROImax[0][12][20]=	 2.46881;
     EtaROImin[0][12][21]=   -1.02514;EtaROImax[0][12][21]=  -0.964674;PhiROImin[0][12][21]=	2.35915;PhiROImax[0][12][21]=	 2.46881;
     EtaROImin[0][12][22]=  -0.961344;EtaROImax[0][12][22]=  -0.898201;PhiROImin[0][12][22]=	2.47231;PhiROImax[0][12][22]=	 2.57921;
     EtaROImin[0][12][23]=   -1.02514;EtaROImax[0][12][23]=  -0.964674;PhiROImin[0][12][23]=	2.47231;PhiROImax[0][12][23]=	 2.57921;
     EtaROImin[0][12][24]=   -1.06547;EtaROImax[0][12][24]=   -1.03003;PhiROImin[0][12][24]=	2.35926;PhiROImax[0][12][24]=	 2.46962;
     EtaROImin[0][12][25]=	    0;EtaROImax[0][12][25]=	     0;PhiROImin[0][12][25]=	      0;PhiROImax[0][12][25]=	       0;
     EtaROImin[0][12][26]=   -1.06547;EtaROImax[0][12][26]=   -1.03003;PhiROImin[0][12][26]=	2.47315;PhiROImax[0][12][26]=	 2.55334;
     EtaROImin[0][12][27]=	    0;EtaROImax[0][12][27]=	     0;PhiROImin[0][12][27]=	      0;PhiROImax[0][12][27]=	       0;
     EtaROImin[0][12][28]=	    0;EtaROImax[0][12][28]=	     0;PhiROImin[0][12][28]=	      0;PhiROImax[0][12][28]=	       0;
     EtaROImin[0][12][29]=	    0;EtaROImax[0][12][29]=	     0;PhiROImin[0][12][29]=	      0;PhiROImax[0][12][29]=	       0;
     EtaROImin[0][12][30]=	    0;EtaROImax[0][12][30]=	     0;PhiROImin[0][12][30]=	      0;PhiROImax[0][12][30]=	       0;
     EtaROImin[0][12][31]=	    0;EtaROImax[0][12][31]=	     0;PhiROImin[0][12][31]=	      0;PhiROImax[0][12][31]=	       0;
     EtaROImin[0][13][ 0]=  -0.214185;EtaROImax[0][13][ 0]=  -0.117369;PhiROImin[0][13][ 0]=	2.66256;PhiROImax[0][13][ 0]=	 2.74612;
     EtaROImin[0][13][ 1]=  -0.118174;EtaROImax[0][13][ 1]= -0.0207264;PhiROImin[0][13][ 1]=	2.66256;PhiROImax[0][13][ 1]=	 2.74612;
     EtaROImin[0][13][ 2]=  -0.214185;EtaROImax[0][13][ 2]=  -0.117369;PhiROImin[0][13][ 2]=	2.57665;PhiROImax[0][13][ 2]=	 2.65895;
     EtaROImin[0][13][ 3]=  -0.118174;EtaROImax[0][13][ 3]= -0.0207264;PhiROImin[0][13][ 3]=	2.57665;PhiROImax[0][13][ 3]=	 2.65895;
     EtaROImin[0][13][ 4]=  -0.416721;EtaROImax[0][13][ 4]=   -0.30075;PhiROImin[0][13][ 4]=	2.66215;PhiROImax[0][13][ 4]=	  2.7461;
     EtaROImin[0][13][ 5]=  -0.302928;EtaROImax[0][13][ 5]=  -0.222449;PhiROImin[0][13][ 5]=	2.66215;PhiROImax[0][13][ 5]=	  2.7461;
     EtaROImin[0][13][ 6]=  -0.416721;EtaROImax[0][13][ 6]=   -0.30075;PhiROImin[0][13][ 6]=	2.57584;PhiROImax[0][13][ 6]=	 2.65852;
     EtaROImin[0][13][ 7]=  -0.302928;EtaROImax[0][13][ 7]=  -0.222449;PhiROImin[0][13][ 7]=	2.57584;PhiROImax[0][13][ 7]=	 2.65852;
     EtaROImin[0][13][ 8]=  -0.573871;EtaROImax[0][13][ 8]=  -0.501681;PhiROImin[0][13][ 8]=	2.66215;PhiROImax[0][13][ 8]=	  2.7461;
     EtaROImin[0][13][ 9]=  -0.504617;EtaROImax[0][13][ 9]=   -0.42967;PhiROImin[0][13][ 9]=	2.66215;PhiROImax[0][13][ 9]=	  2.7461;
     EtaROImin[0][13][10]=  -0.573871;EtaROImax[0][13][10]=  -0.501681;PhiROImin[0][13][10]=	2.57584;PhiROImax[0][13][10]=	 2.65852;
     EtaROImin[0][13][11]=  -0.504617;EtaROImax[0][13][11]=   -0.42967;PhiROImin[0][13][11]=	2.57584;PhiROImax[0][13][11]=	 2.65852;
     EtaROImin[0][13][12]=  -0.741516;EtaROImax[0][13][12]=  -0.649933;PhiROImin[0][13][12]=	2.66215;PhiROImax[0][13][12]=	  2.7461;
     EtaROImin[0][13][13]=  -0.653329;EtaROImax[0][13][13]=  -0.583785;PhiROImin[0][13][13]=	2.66215;PhiROImax[0][13][13]=	  2.7461;
     EtaROImin[0][13][14]=  -0.741516;EtaROImax[0][13][14]=  -0.649933;PhiROImin[0][13][14]=	2.57584;PhiROImax[0][13][14]=	 2.65852;
     EtaROImin[0][13][15]=  -0.653329;EtaROImax[0][13][15]=  -0.583785;PhiROImin[0][13][15]=	2.57584;PhiROImax[0][13][15]=	 2.65852;
     EtaROImin[0][13][16]=	    0;EtaROImax[0][13][16]=	     0;PhiROImin[0][13][16]=	      0;PhiROImax[0][13][16]=	       0;
     EtaROImin[0][13][17]=  -0.837822;EtaROImax[0][13][17]=  -0.756521;PhiROImin[0][13][17]=	2.66215;PhiROImax[0][13][17]=	  2.7461;
     EtaROImin[0][13][18]=	    0;EtaROImax[0][13][18]=	     0;PhiROImin[0][13][18]=	      0;PhiROImax[0][13][18]=	       0;
     EtaROImin[0][13][19]=  -0.837822;EtaROImax[0][13][19]=  -0.756521;PhiROImin[0][13][19]=	2.57584;PhiROImax[0][13][19]=	 2.65852;
     EtaROImin[0][13][20]=  -0.956037;EtaROImax[0][13][20]=  -0.899344;PhiROImin[0][13][20]=	2.66215;PhiROImax[0][13][20]=	  2.7461;
     EtaROImin[0][13][21]=  -0.903324;EtaROImax[0][13][21]=  -0.844116;PhiROImin[0][13][21]=	2.66215;PhiROImax[0][13][21]=	  2.7461;
     EtaROImin[0][13][22]=  -0.956037;EtaROImax[0][13][22]=  -0.899344;PhiROImin[0][13][22]=	2.57584;PhiROImax[0][13][22]=	 2.65852;
     EtaROImin[0][13][23]=  -0.903324;EtaROImax[0][13][23]=  -0.844116;PhiROImin[0][13][23]=	2.57584;PhiROImax[0][13][23]=	 2.65852;
     EtaROImin[0][13][24]=	    0;EtaROImax[0][13][24]=	     0;PhiROImin[0][13][24]=	      0;PhiROImax[0][13][24]=	       0;
     EtaROImin[0][13][25]=	    0;EtaROImax[0][13][25]=	     0;PhiROImin[0][13][25]=	      0;PhiROImax[0][13][25]=	       0;
     EtaROImin[0][13][26]=	    0;EtaROImax[0][13][26]=	     0;PhiROImin[0][13][26]=	      0;PhiROImax[0][13][26]=	       0;
     EtaROImin[0][13][27]=	    0;EtaROImax[0][13][27]=	     0;PhiROImin[0][13][27]=	      0;PhiROImax[0][13][27]=	       0;
     EtaROImin[0][13][28]=	    0;EtaROImax[0][13][28]=	     0;PhiROImin[0][13][28]=	      0;PhiROImax[0][13][28]=	       0;
     EtaROImin[0][13][29]=	    0;EtaROImax[0][13][29]=	     0;PhiROImin[0][13][29]=	      0;PhiROImax[0][13][29]=	       0;
     EtaROImin[0][13][30]=	    0;EtaROImax[0][13][30]=	     0;PhiROImin[0][13][30]=	      0;PhiROImax[0][13][30]=	       0;
     EtaROImin[0][13][31]=	    0;EtaROImax[0][13][31]=	     0;PhiROImin[0][13][31]=	      0;PhiROImax[0][13][31]=	       0;
     EtaROImin[0][14][ 0]=  -0.118174;EtaROImax[0][14][ 0]= -0.0207264;PhiROImin[0][14][ 0]=	2.75167;PhiROImax[0][14][ 0]=	 2.83523;
     EtaROImin[0][14][ 1]=  -0.214185;EtaROImax[0][14][ 1]=  -0.117369;PhiROImin[0][14][ 1]=	2.75167;PhiROImax[0][14][ 1]=	 2.83523;
     EtaROImin[0][14][ 2]=  -0.118174;EtaROImax[0][14][ 2]= -0.0207264;PhiROImin[0][14][ 2]=	2.83884;PhiROImax[0][14][ 2]=	 2.92113;
     EtaROImin[0][14][ 3]=  -0.214185;EtaROImax[0][14][ 3]=  -0.117369;PhiROImin[0][14][ 3]=	2.83884;PhiROImax[0][14][ 3]=	 2.92113;
     EtaROImin[0][14][ 4]=  -0.302928;EtaROImax[0][14][ 4]=  -0.222449;PhiROImin[0][14][ 4]=	2.75168;PhiROImax[0][14][ 4]=	 2.83564;
     EtaROImin[0][14][ 5]=  -0.416721;EtaROImax[0][14][ 5]=   -0.30075;PhiROImin[0][14][ 5]=	2.75168;PhiROImax[0][14][ 5]=	 2.83564;
     EtaROImin[0][14][ 6]=  -0.302928;EtaROImax[0][14][ 6]=  -0.222449;PhiROImin[0][14][ 6]=	2.83927;PhiROImax[0][14][ 6]=	 2.92194;
     EtaROImin[0][14][ 7]=  -0.416721;EtaROImax[0][14][ 7]=   -0.30075;PhiROImin[0][14][ 7]=	2.83927;PhiROImax[0][14][ 7]=	 2.92194;
     EtaROImin[0][14][ 8]=  -0.504617;EtaROImax[0][14][ 8]=   -0.42967;PhiROImin[0][14][ 8]=	2.75168;PhiROImax[0][14][ 8]=	 2.83564;
     EtaROImin[0][14][ 9]=  -0.573871;EtaROImax[0][14][ 9]=  -0.501681;PhiROImin[0][14][ 9]=	2.75168;PhiROImax[0][14][ 9]=	 2.83564;
     EtaROImin[0][14][10]=  -0.504617;EtaROImax[0][14][10]=   -0.42967;PhiROImin[0][14][10]=	2.83927;PhiROImax[0][14][10]=	 2.92194;
     EtaROImin[0][14][11]=  -0.573871;EtaROImax[0][14][11]=  -0.501681;PhiROImin[0][14][11]=	2.83927;PhiROImax[0][14][11]=	 2.92194;
     EtaROImin[0][14][12]=  -0.653329;EtaROImax[0][14][12]=  -0.583785;PhiROImin[0][14][12]=	2.75168;PhiROImax[0][14][12]=	 2.83564;
     EtaROImin[0][14][13]=  -0.741516;EtaROImax[0][14][13]=  -0.649933;PhiROImin[0][14][13]=	2.75168;PhiROImax[0][14][13]=	 2.83564;
     EtaROImin[0][14][14]=  -0.653329;EtaROImax[0][14][14]=  -0.583785;PhiROImin[0][14][14]=	2.83927;PhiROImax[0][14][14]=	 2.92194;
     EtaROImin[0][14][15]=  -0.741516;EtaROImax[0][14][15]=  -0.649933;PhiROImin[0][14][15]=	2.83927;PhiROImax[0][14][15]=	 2.92194;
     EtaROImin[0][14][16]=  -0.837822;EtaROImax[0][14][16]=  -0.756521;PhiROImin[0][14][16]=	2.75168;PhiROImax[0][14][16]=	 2.83564;
     EtaROImin[0][14][17]=	    0;EtaROImax[0][14][17]=	     0;PhiROImin[0][14][17]=	      0;PhiROImax[0][14][17]=	       0;
     EtaROImin[0][14][18]=  -0.837822;EtaROImax[0][14][18]=  -0.756521;PhiROImin[0][14][18]=	2.83927;PhiROImax[0][14][18]=	 2.92194;
     EtaROImin[0][14][19]=	    0;EtaROImax[0][14][19]=	     0;PhiROImin[0][14][19]=	      0;PhiROImax[0][14][19]=	       0;
     EtaROImin[0][14][20]=  -0.903324;EtaROImax[0][14][20]=  -0.844116;PhiROImin[0][14][20]=	2.75168;PhiROImax[0][14][20]=	 2.83564;
     EtaROImin[0][14][21]=  -0.956037;EtaROImax[0][14][21]=  -0.899344;PhiROImin[0][14][21]=	2.75168;PhiROImax[0][14][21]=	 2.83564;
     EtaROImin[0][14][22]=  -0.903324;EtaROImax[0][14][22]=  -0.844116;PhiROImin[0][14][22]=	2.83927;PhiROImax[0][14][22]=	 2.92194;
     EtaROImin[0][14][23]=  -0.956037;EtaROImax[0][14][23]=  -0.899344;PhiROImin[0][14][23]=	2.83927;PhiROImax[0][14][23]=	 2.92194;
     EtaROImin[0][14][24]=	    0;EtaROImax[0][14][24]=	     0;PhiROImin[0][14][24]=	      0;PhiROImax[0][14][24]=	       0;
     EtaROImin[0][14][25]=	    0;EtaROImax[0][14][25]=	     0;PhiROImin[0][14][25]=	      0;PhiROImax[0][14][25]=	       0;
     EtaROImin[0][14][26]=	    0;EtaROImax[0][14][26]=	     0;PhiROImin[0][14][26]=	      0;PhiROImax[0][14][26]=	       0;
     EtaROImin[0][14][27]=	    0;EtaROImax[0][14][27]=	     0;PhiROImin[0][14][27]=	      0;PhiROImax[0][14][27]=	       0;
     EtaROImin[0][14][28]=	    0;EtaROImax[0][14][28]=	     0;PhiROImin[0][14][28]=	      0;PhiROImax[0][14][28]=	       0;
     EtaROImin[0][14][29]=	    0;EtaROImax[0][14][29]=	     0;PhiROImin[0][14][29]=	      0;PhiROImax[0][14][29]=	       0;
     EtaROImin[0][14][30]=	    0;EtaROImax[0][14][30]=	     0;PhiROImin[0][14][30]=	      0;PhiROImax[0][14][30]=	       0;
     EtaROImin[0][14][31]=	    0;EtaROImax[0][14][31]=	     0;PhiROImin[0][14][31]=	      0;PhiROImax[0][14][31]=	       0;
     EtaROImin[0][15][ 0]=  -0.238476;EtaROImax[0][15][ 0]=  -0.180926;PhiROImin[0][15][ 0]=	3.02838;PhiROImax[0][15][ 0]=	 3.13862;
     EtaROImin[0][15][ 1]=  -0.177066;EtaROImax[0][15][ 1]= -0.0876766;PhiROImin[0][15][ 1]=	3.02838;PhiROImax[0][15][ 1]=	 3.13862;
     EtaROImin[0][15][ 2]=  -0.238476;EtaROImax[0][15][ 2]=  -0.180926;PhiROImin[0][15][ 2]=	2.91742;PhiROImax[0][15][ 2]=	 3.02486;
     EtaROImin[0][15][ 3]=  -0.177066;EtaROImax[0][15][ 3]= -0.0876766;PhiROImin[0][15][ 3]=	2.91742;PhiROImax[0][15][ 3]=	 3.02486;
     EtaROImin[0][15][ 4]=  -0.452099;EtaROImax[0][15][ 4]=  -0.350831;PhiROImin[0][15][ 4]=	3.02898;PhiROImax[0][15][ 4]=	 3.13864;
     EtaROImin[0][15][ 5]=   -0.35049;EtaROImax[0][15][ 5]=  -0.246207;PhiROImin[0][15][ 5]=	3.02898;PhiROImax[0][15][ 5]=	 3.13864;
     EtaROImin[0][15][ 6]=  -0.452099;EtaROImax[0][15][ 6]=  -0.350831;PhiROImin[0][15][ 6]=	2.91858;PhiROImax[0][15][ 6]=	 3.02547;
     EtaROImin[0][15][ 7]=   -0.35049;EtaROImax[0][15][ 7]=  -0.246207;PhiROImin[0][15][ 7]=	2.91858;PhiROImax[0][15][ 7]=	 3.02547;
     EtaROImin[0][15][ 8]=  -0.648101;EtaROImax[0][15][ 8]=  -0.555789;PhiROImin[0][15][ 8]=	3.02898;PhiROImax[0][15][ 8]=	 3.13864;
     EtaROImin[0][15][ 9]=  -0.554444;EtaROImax[0][15][ 9]=  -0.458091;PhiROImin[0][15][ 9]=	3.02898;PhiROImax[0][15][ 9]=	 3.13864;
     EtaROImin[0][15][10]=  -0.648101;EtaROImax[0][15][10]=  -0.555789;PhiROImin[0][15][10]=	2.91858;PhiROImax[0][15][10]=	 3.02547;
     EtaROImin[0][15][11]=  -0.554444;EtaROImax[0][15][11]=  -0.458091;PhiROImin[0][15][11]=	2.91858;PhiROImax[0][15][11]=	 3.02547;
     EtaROImin[0][15][12]=  -0.776449;EtaROImax[0][15][12]=  -0.731124;PhiROImin[0][15][12]=	3.02838;PhiROImax[0][15][12]=	 3.13862;
     EtaROImin[0][15][13]=  -0.728056;EtaROImax[0][15][13]=  -0.655857;PhiROImin[0][15][13]=	3.02838;PhiROImax[0][15][13]=	 3.13862;
     EtaROImin[0][15][14]=  -0.776449;EtaROImax[0][15][14]=  -0.731124;PhiROImin[0][15][14]=	2.91742;PhiROImax[0][15][14]=	 3.02486;
     EtaROImin[0][15][15]=  -0.728056;EtaROImax[0][15][15]=  -0.655857;PhiROImin[0][15][15]=	2.91742;PhiROImax[0][15][15]=	 3.02486;
     EtaROImin[0][15][16]=  -0.895163;EtaROImax[0][15][16]=  -0.853359;PhiROImin[0][15][16]=	3.02838;PhiROImax[0][15][16]=	 3.13862;
     EtaROImin[0][15][17]=  -0.850528;EtaROImax[0][15][17]=  -0.783867;PhiROImin[0][15][17]=	3.02838;PhiROImax[0][15][17]=	 3.13862;
     EtaROImin[0][15][18]=  -0.895163;EtaROImax[0][15][18]=  -0.853359;PhiROImin[0][15][18]=	2.91742;PhiROImax[0][15][18]=	 3.02486;
     EtaROImin[0][15][19]=  -0.850528;EtaROImax[0][15][19]=  -0.783867;PhiROImin[0][15][19]=	2.91742;PhiROImax[0][15][19]=	 3.02486;
     EtaROImin[0][15][20]=   -1.02514;EtaROImax[0][15][20]=  -0.964674;PhiROImin[0][15][20]=	3.02898;PhiROImax[0][15][20]=	 3.13864;
     EtaROImin[0][15][21]=  -0.961344;EtaROImax[0][15][21]=  -0.898201;PhiROImin[0][15][21]=	3.02898;PhiROImax[0][15][21]=	 3.13864;
     EtaROImin[0][15][22]=   -1.02514;EtaROImax[0][15][22]=  -0.964674;PhiROImin[0][15][22]=	2.91858;PhiROImax[0][15][22]=	 3.02547;
     EtaROImin[0][15][23]=  -0.961344;EtaROImax[0][15][23]=  -0.898201;PhiROImin[0][15][23]=	2.91858;PhiROImax[0][15][23]=	 3.02547;
     EtaROImin[0][15][24]=	    0;EtaROImax[0][15][24]=	     0;PhiROImin[0][15][24]=	      0;PhiROImax[0][15][24]=	       0;
     EtaROImin[0][15][25]=   -1.06547;EtaROImax[0][15][25]=   -1.03003;PhiROImin[0][15][25]=	3.02816;PhiROImax[0][15][25]=	 3.13852;
     EtaROImin[0][15][26]=	    0;EtaROImax[0][15][26]=	     0;PhiROImin[0][15][26]=	      0;PhiROImax[0][15][26]=	       0;
     EtaROImin[0][15][27]=   -1.06547;EtaROImax[0][15][27]=   -1.03003;PhiROImin[0][15][27]=	2.94445;PhiROImax[0][15][27]=	 3.02463;
     EtaROImin[0][15][28]=	    0;EtaROImax[0][15][28]=	     0;PhiROImin[0][15][28]=	      0;PhiROImax[0][15][28]=	       0;
     EtaROImin[0][15][29]=	    0;EtaROImax[0][15][29]=	     0;PhiROImin[0][15][29]=	      0;PhiROImax[0][15][29]=	       0;
     EtaROImin[0][15][30]=	    0;EtaROImax[0][15][30]=	     0;PhiROImin[0][15][30]=	      0;PhiROImax[0][15][30]=	       0;
     EtaROImin[0][15][31]=	    0;EtaROImax[0][15][31]=	     0;PhiROImin[0][15][31]=	      0;PhiROImax[0][15][31]=	       0;
     EtaROImin[0][16][ 0]=  -0.146097;EtaROImax[0][16][ 0]= -0.0876766;PhiROImin[0][16][ 0]=   -3.13862;PhiROImax[0][16][ 0]=	-3.02838;
     EtaROImin[0][16][ 1]=  -0.238476;EtaROImax[0][16][ 1]=  -0.149976;PhiROImin[0][16][ 1]=   -3.13862;PhiROImax[0][16][ 1]=	-3.02838;
     EtaROImin[0][16][ 2]=  -0.146097;EtaROImax[0][16][ 2]= -0.0876766;PhiROImin[0][16][ 2]=   -3.02486;PhiROImax[0][16][ 2]=	-2.91742;
     EtaROImin[0][16][ 3]=  -0.238476;EtaROImax[0][16][ 3]=  -0.149976;PhiROImin[0][16][ 3]=   -3.02486;PhiROImax[0][16][ 3]=	-2.91742;
     EtaROImin[0][16][ 4]=   -0.35049;EtaROImax[0][16][ 4]=  -0.246207;PhiROImin[0][16][ 4]=   -3.13864;PhiROImax[0][16][ 4]=	-3.02898;
     EtaROImin[0][16][ 5]=  -0.452099;EtaROImax[0][16][ 5]=  -0.350831;PhiROImin[0][16][ 5]=   -3.13864;PhiROImax[0][16][ 5]=	-3.02898;
     EtaROImin[0][16][ 6]=   -0.35049;EtaROImax[0][16][ 6]=  -0.246207;PhiROImin[0][16][ 6]=   -3.02547;PhiROImax[0][16][ 6]=	-2.91858;
     EtaROImin[0][16][ 7]=  -0.452099;EtaROImax[0][16][ 7]=  -0.350831;PhiROImin[0][16][ 7]=   -3.02547;PhiROImax[0][16][ 7]=	-2.91858;
     EtaROImin[0][16][ 8]=  -0.554444;EtaROImax[0][16][ 8]=  -0.458091;PhiROImin[0][16][ 8]=   -3.13864;PhiROImax[0][16][ 8]=	-3.02898;
     EtaROImin[0][16][ 9]=  -0.648101;EtaROImax[0][16][ 9]=  -0.555789;PhiROImin[0][16][ 9]=   -3.13864;PhiROImax[0][16][ 9]=	-3.02898;
     EtaROImin[0][16][10]=  -0.554444;EtaROImax[0][16][10]=  -0.458091;PhiROImin[0][16][10]=   -3.02547;PhiROImax[0][16][10]=	-2.91858;
     EtaROImin[0][16][11]=  -0.648101;EtaROImax[0][16][11]=  -0.555789;PhiROImin[0][16][11]=   -3.02547;PhiROImax[0][16][11]=	-2.91858;
     EtaROImin[0][16][12]=  -0.703298;EtaROImax[0][16][12]=  -0.655857;PhiROImin[0][16][12]=   -3.13862;PhiROImax[0][16][12]=	-3.02838;
     EtaROImin[0][16][13]=  -0.776449;EtaROImax[0][16][13]=  -0.706413;PhiROImin[0][16][13]=   -3.13862;PhiROImax[0][16][13]=	-3.02838;
     EtaROImin[0][16][14]=  -0.703298;EtaROImax[0][16][14]=  -0.655857;PhiROImin[0][16][14]=   -3.02486;PhiROImax[0][16][14]=	-2.91742;
     EtaROImin[0][16][15]=  -0.776449;EtaROImax[0][16][15]=  -0.706413;PhiROImin[0][16][15]=   -3.02486;PhiROImax[0][16][15]=	-2.91742;
     EtaROImin[0][16][16]=   -0.82768;EtaROImax[0][16][16]=  -0.783867;PhiROImin[0][16][16]=   -3.13862;PhiROImax[0][16][16]=	-3.02838;
     EtaROImin[0][16][17]=  -0.895163;EtaROImax[0][16][17]=  -0.830556;PhiROImin[0][16][17]=   -3.13862;PhiROImax[0][16][17]=	-3.02838;
     EtaROImin[0][16][18]=   -0.82768;EtaROImax[0][16][18]=  -0.783867;PhiROImin[0][16][18]=   -3.02486;PhiROImax[0][16][18]=	-2.91742;
     EtaROImin[0][16][19]=  -0.895163;EtaROImax[0][16][19]=  -0.830556;PhiROImin[0][16][19]=   -3.02486;PhiROImax[0][16][19]=	-2.91742;
     EtaROImin[0][16][20]=  -0.961344;EtaROImax[0][16][20]=  -0.898201;PhiROImin[0][16][20]=   -3.13864;PhiROImax[0][16][20]=	-3.02898;
     EtaROImin[0][16][21]=   -1.02514;EtaROImax[0][16][21]=  -0.964674;PhiROImin[0][16][21]=   -3.13864;PhiROImax[0][16][21]=	-3.02898;
     EtaROImin[0][16][22]=  -0.961344;EtaROImax[0][16][22]=  -0.898201;PhiROImin[0][16][22]=   -3.02547;PhiROImax[0][16][22]=	-2.91858;
     EtaROImin[0][16][23]=   -1.02514;EtaROImax[0][16][23]=  -0.964674;PhiROImin[0][16][23]=   -3.02547;PhiROImax[0][16][23]=	-2.91858;
     EtaROImin[0][16][24]=   -1.06547;EtaROImax[0][16][24]=   -1.03003;PhiROImin[0][16][24]=   -3.13852;PhiROImax[0][16][24]=	-3.02816;
     EtaROImin[0][16][25]=	    0;EtaROImax[0][16][25]=	     0;PhiROImin[0][16][25]=	      0;PhiROImax[0][16][25]=	       0;
     EtaROImin[0][16][26]=   -1.06547;EtaROImax[0][16][26]=   -1.03003;PhiROImin[0][16][26]=   -3.02463;PhiROImax[0][16][26]=	-2.94445;
     EtaROImin[0][16][27]=	    0;EtaROImax[0][16][27]=	     0;PhiROImin[0][16][27]=	      0;PhiROImax[0][16][27]=	       0;
     EtaROImin[0][16][28]=	    0;EtaROImax[0][16][28]=	     0;PhiROImin[0][16][28]=	      0;PhiROImax[0][16][28]=	       0;
     EtaROImin[0][16][29]=	    0;EtaROImax[0][16][29]=	     0;PhiROImin[0][16][29]=	      0;PhiROImax[0][16][29]=	       0;
     EtaROImin[0][16][30]=	    0;EtaROImax[0][16][30]=	     0;PhiROImin[0][16][30]=	      0;PhiROImax[0][16][30]=	       0;
     EtaROImin[0][16][31]=	    0;EtaROImax[0][16][31]=	     0;PhiROImin[0][16][31]=	      0;PhiROImax[0][16][31]=	       0;
     EtaROImin[0][17][ 0]=  -0.213185;EtaROImax[0][17][ 0]=  -0.116816;PhiROImin[0][17][ 0]=   -2.83564;PhiROImax[0][17][ 0]=	-2.75168;
     EtaROImin[0][17][ 1]=  -0.118734;EtaROImax[0][17][ 1]= -0.0208251;PhiROImin[0][17][ 1]=   -2.83564;PhiROImax[0][17][ 1]=	-2.75168;
     EtaROImin[0][17][ 2]=  -0.213185;EtaROImax[0][17][ 2]=  -0.116816;PhiROImin[0][17][ 2]=   -2.92194;PhiROImax[0][17][ 2]=	-2.83927;
     EtaROImin[0][17][ 3]=  -0.118734;EtaROImax[0][17][ 3]= -0.0208251;PhiROImin[0][17][ 3]=   -2.92194;PhiROImax[0][17][ 3]=	-2.83927;
     EtaROImin[0][17][ 4]=  -0.416721;EtaROImax[0][17][ 4]=   -0.30075;PhiROImin[0][17][ 4]=   -2.83564;PhiROImax[0][17][ 4]=	-2.75168;
     EtaROImin[0][17][ 5]=  -0.302928;EtaROImax[0][17][ 5]=  -0.222449;PhiROImin[0][17][ 5]=   -2.83564;PhiROImax[0][17][ 5]=	-2.75168;
     EtaROImin[0][17][ 6]=  -0.416721;EtaROImax[0][17][ 6]=   -0.30075;PhiROImin[0][17][ 6]=   -2.92194;PhiROImax[0][17][ 6]=	-2.83927;
     EtaROImin[0][17][ 7]=  -0.302928;EtaROImax[0][17][ 7]=  -0.222449;PhiROImin[0][17][ 7]=   -2.92194;PhiROImax[0][17][ 7]=	-2.83927;
     EtaROImin[0][17][ 8]=  -0.573871;EtaROImax[0][17][ 8]=  -0.501681;PhiROImin[0][17][ 8]=   -2.83564;PhiROImax[0][17][ 8]=	-2.75168;
     EtaROImin[0][17][ 9]=  -0.504617;EtaROImax[0][17][ 9]=   -0.42967;PhiROImin[0][17][ 9]=   -2.83564;PhiROImax[0][17][ 9]=	-2.75168;
     EtaROImin[0][17][10]=  -0.573871;EtaROImax[0][17][10]=  -0.501681;PhiROImin[0][17][10]=   -2.92194;PhiROImax[0][17][10]=	-2.83927;
     EtaROImin[0][17][11]=  -0.504617;EtaROImax[0][17][11]=   -0.42967;PhiROImin[0][17][11]=   -2.92194;PhiROImax[0][17][11]=	-2.83927;
     EtaROImin[0][17][12]=  -0.741516;EtaROImax[0][17][12]=  -0.649933;PhiROImin[0][17][12]=   -2.83564;PhiROImax[0][17][12]=	-2.75168;
     EtaROImin[0][17][13]=  -0.653329;EtaROImax[0][17][13]=  -0.583785;PhiROImin[0][17][13]=   -2.83564;PhiROImax[0][17][13]=	-2.75168;
     EtaROImin[0][17][14]=  -0.741516;EtaROImax[0][17][14]=  -0.649933;PhiROImin[0][17][14]=   -2.92194;PhiROImax[0][17][14]=	-2.83927;
     EtaROImin[0][17][15]=  -0.653329;EtaROImax[0][17][15]=  -0.583785;PhiROImin[0][17][15]=   -2.92194;PhiROImax[0][17][15]=	-2.83927;
     EtaROImin[0][17][16]=	    0;EtaROImax[0][17][16]=	     0;PhiROImin[0][17][16]=	      0;PhiROImax[0][17][16]=	       0;
     EtaROImin[0][17][17]=  -0.837822;EtaROImax[0][17][17]=  -0.756521;PhiROImin[0][17][17]=   -2.83564;PhiROImax[0][17][17]=	-2.75168;
     EtaROImin[0][17][18]=	    0;EtaROImax[0][17][18]=	     0;PhiROImin[0][17][18]=	      0;PhiROImax[0][17][18]=	       0;
     EtaROImin[0][17][19]=  -0.837822;EtaROImax[0][17][19]=  -0.756521;PhiROImin[0][17][19]=   -2.92194;PhiROImax[0][17][19]=	-2.83927;
     EtaROImin[0][17][20]=  -0.956037;EtaROImax[0][17][20]=  -0.899344;PhiROImin[0][17][20]=   -2.83564;PhiROImax[0][17][20]=	-2.75168;
     EtaROImin[0][17][21]=  -0.903324;EtaROImax[0][17][21]=  -0.844116;PhiROImin[0][17][21]=   -2.83564;PhiROImax[0][17][21]=	-2.75168;
     EtaROImin[0][17][22]=  -0.956037;EtaROImax[0][17][22]=  -0.899344;PhiROImin[0][17][22]=   -2.92194;PhiROImax[0][17][22]=	-2.83927;
     EtaROImin[0][17][23]=  -0.903324;EtaROImax[0][17][23]=  -0.844116;PhiROImin[0][17][23]=   -2.92194;PhiROImax[0][17][23]=	-2.83927;
     EtaROImin[0][17][24]=	    0;EtaROImax[0][17][24]=	     0;PhiROImin[0][17][24]=	      0;PhiROImax[0][17][24]=	       0;
     EtaROImin[0][17][25]=	    0;EtaROImax[0][17][25]=	     0;PhiROImin[0][17][25]=	      0;PhiROImax[0][17][25]=	       0;
     EtaROImin[0][17][26]=	    0;EtaROImax[0][17][26]=	     0;PhiROImin[0][17][26]=	      0;PhiROImax[0][17][26]=	       0;
     EtaROImin[0][17][27]=	    0;EtaROImax[0][17][27]=	     0;PhiROImin[0][17][27]=	      0;PhiROImax[0][17][27]=	       0;
     EtaROImin[0][17][28]=	    0;EtaROImax[0][17][28]=	     0;PhiROImin[0][17][28]=	      0;PhiROImax[0][17][28]=	       0;
     EtaROImin[0][17][29]=	    0;EtaROImax[0][17][29]=	     0;PhiROImin[0][17][29]=	      0;PhiROImax[0][17][29]=	       0;
     EtaROImin[0][17][30]=	    0;EtaROImax[0][17][30]=	     0;PhiROImin[0][17][30]=	      0;PhiROImax[0][17][30]=	       0;
     EtaROImin[0][17][31]=	    0;EtaROImax[0][17][31]=	     0;PhiROImin[0][17][31]=	      0;PhiROImax[0][17][31]=	       0;
     EtaROImin[0][18][ 0]=  -0.118734;EtaROImax[0][18][ 0]= -0.0208251;PhiROImin[0][18][ 0]=	-2.7461;PhiROImax[0][18][ 0]=	-2.66215;
     EtaROImin[0][18][ 1]=  -0.213185;EtaROImax[0][18][ 1]=  -0.116816;PhiROImin[0][18][ 1]=	-2.7461;PhiROImax[0][18][ 1]=	-2.66215;
     EtaROImin[0][18][ 2]=  -0.118734;EtaROImax[0][18][ 2]= -0.0208251;PhiROImin[0][18][ 2]=   -2.65852;PhiROImax[0][18][ 2]=	-2.57584;
     EtaROImin[0][18][ 3]=  -0.213185;EtaROImax[0][18][ 3]=  -0.116816;PhiROImin[0][18][ 3]=   -2.65852;PhiROImax[0][18][ 3]=	-2.57584;
     EtaROImin[0][18][ 4]=  -0.302928;EtaROImax[0][18][ 4]=  -0.222449;PhiROImin[0][18][ 4]=	-2.7461;PhiROImax[0][18][ 4]=	-2.66215;
     EtaROImin[0][18][ 5]=  -0.416721;EtaROImax[0][18][ 5]=   -0.30075;PhiROImin[0][18][ 5]=	-2.7461;PhiROImax[0][18][ 5]=	-2.66215;
     EtaROImin[0][18][ 6]=  -0.302928;EtaROImax[0][18][ 6]=  -0.222449;PhiROImin[0][18][ 6]=   -2.65852;PhiROImax[0][18][ 6]=	-2.57584;
     EtaROImin[0][18][ 7]=  -0.416721;EtaROImax[0][18][ 7]=   -0.30075;PhiROImin[0][18][ 7]=   -2.65852;PhiROImax[0][18][ 7]=	-2.57584;
     EtaROImin[0][18][ 8]=  -0.504617;EtaROImax[0][18][ 8]=   -0.42967;PhiROImin[0][18][ 8]=	-2.7461;PhiROImax[0][18][ 8]=	-2.66215;
     EtaROImin[0][18][ 9]=  -0.573871;EtaROImax[0][18][ 9]=  -0.501681;PhiROImin[0][18][ 9]=	-2.7461;PhiROImax[0][18][ 9]=	-2.66215;
     EtaROImin[0][18][10]=  -0.504617;EtaROImax[0][18][10]=   -0.42967;PhiROImin[0][18][10]=   -2.65852;PhiROImax[0][18][10]=	-2.57584;
     EtaROImin[0][18][11]=  -0.573871;EtaROImax[0][18][11]=  -0.501681;PhiROImin[0][18][11]=   -2.65852;PhiROImax[0][18][11]=	-2.57584;
     EtaROImin[0][18][12]=  -0.653329;EtaROImax[0][18][12]=  -0.583785;PhiROImin[0][18][12]=	-2.7461;PhiROImax[0][18][12]=	-2.66215;
     EtaROImin[0][18][13]=  -0.741516;EtaROImax[0][18][13]=  -0.649933;PhiROImin[0][18][13]=	-2.7461;PhiROImax[0][18][13]=	-2.66215;
     EtaROImin[0][18][14]=  -0.653329;EtaROImax[0][18][14]=  -0.583785;PhiROImin[0][18][14]=   -2.65852;PhiROImax[0][18][14]=	-2.57584;
     EtaROImin[0][18][15]=  -0.741516;EtaROImax[0][18][15]=  -0.649933;PhiROImin[0][18][15]=   -2.65852;PhiROImax[0][18][15]=	-2.57584;
     EtaROImin[0][18][16]=  -0.837822;EtaROImax[0][18][16]=  -0.756521;PhiROImin[0][18][16]=	-2.7461;PhiROImax[0][18][16]=	-2.66215;
     EtaROImin[0][18][17]=	    0;EtaROImax[0][18][17]=	     0;PhiROImin[0][18][17]=	      0;PhiROImax[0][18][17]=	       0;
     EtaROImin[0][18][18]=  -0.837822;EtaROImax[0][18][18]=  -0.756521;PhiROImin[0][18][18]=   -2.65852;PhiROImax[0][18][18]=	-2.57584;
     EtaROImin[0][18][19]=	    0;EtaROImax[0][18][19]=	     0;PhiROImin[0][18][19]=	      0;PhiROImax[0][18][19]=	       0;
     EtaROImin[0][18][20]=  -0.903324;EtaROImax[0][18][20]=  -0.844116;PhiROImin[0][18][20]=	-2.7461;PhiROImax[0][18][20]=	-2.66215;
     EtaROImin[0][18][21]=  -0.956037;EtaROImax[0][18][21]=  -0.899344;PhiROImin[0][18][21]=	-2.7461;PhiROImax[0][18][21]=	-2.66215;
     EtaROImin[0][18][22]=  -0.903324;EtaROImax[0][18][22]=  -0.844116;PhiROImin[0][18][22]=   -2.65852;PhiROImax[0][18][22]=	-2.57584;
     EtaROImin[0][18][23]=  -0.956037;EtaROImax[0][18][23]=  -0.899344;PhiROImin[0][18][23]=   -2.65852;PhiROImax[0][18][23]=	-2.57584;
     EtaROImin[0][18][24]=	    0;EtaROImax[0][18][24]=	     0;PhiROImin[0][18][24]=	      0;PhiROImax[0][18][24]=	       0;
     EtaROImin[0][18][25]=	    0;EtaROImax[0][18][25]=	     0;PhiROImin[0][18][25]=	      0;PhiROImax[0][18][25]=	       0;
     EtaROImin[0][18][26]=	    0;EtaROImax[0][18][26]=	     0;PhiROImin[0][18][26]=	      0;PhiROImax[0][18][26]=	       0;
     EtaROImin[0][18][27]=	    0;EtaROImax[0][18][27]=	     0;PhiROImin[0][18][27]=	      0;PhiROImax[0][18][27]=	       0;
     EtaROImin[0][18][28]=	    0;EtaROImax[0][18][28]=	     0;PhiROImin[0][18][28]=	      0;PhiROImax[0][18][28]=	       0;
     EtaROImin[0][18][29]=	    0;EtaROImax[0][18][29]=	     0;PhiROImin[0][18][29]=	      0;PhiROImax[0][18][29]=	       0;
     EtaROImin[0][18][30]=	    0;EtaROImax[0][18][30]=	     0;PhiROImin[0][18][30]=	      0;PhiROImax[0][18][30]=	       0;
     EtaROImin[0][18][31]=	    0;EtaROImax[0][18][31]=	     0;PhiROImin[0][18][31]=	      0;PhiROImax[0][18][31]=	       0;
     EtaROImin[0][19][ 0]=  -0.238294;EtaROImax[0][19][ 0]=  -0.180742;PhiROImin[0][19][ 0]=   -2.46941;PhiROImax[0][19][ 0]=	-2.35916;
     EtaROImin[0][19][ 1]=  -0.176882;EtaROImax[0][19][ 1]= -0.0874905;PhiROImin[0][19][ 1]=   -2.46941;PhiROImax[0][19][ 1]=	-2.35916;
     EtaROImin[0][19][ 2]=  -0.238294;EtaROImax[0][19][ 2]=  -0.180742;PhiROImin[0][19][ 2]=   -2.58036;PhiROImax[0][19][ 2]=	-2.47293;
     EtaROImin[0][19][ 3]=  -0.176882;EtaROImax[0][19][ 3]= -0.0874905;PhiROImin[0][19][ 3]=   -2.58036;PhiROImax[0][19][ 3]=	-2.47293;
     EtaROImin[0][19][ 4]=  -0.452099;EtaROImax[0][19][ 4]=  -0.350831;PhiROImin[0][19][ 4]=   -2.46881;PhiROImax[0][19][ 4]=	-2.35915;
     EtaROImin[0][19][ 5]=   -0.35049;EtaROImax[0][19][ 5]=  -0.246207;PhiROImin[0][19][ 5]=   -2.46881;PhiROImax[0][19][ 5]=	-2.35915;
     EtaROImin[0][19][ 6]=  -0.452099;EtaROImax[0][19][ 6]=  -0.350831;PhiROImin[0][19][ 6]=   -2.57921;PhiROImax[0][19][ 6]=	-2.47231;
     EtaROImin[0][19][ 7]=   -0.35049;EtaROImax[0][19][ 7]=  -0.246207;PhiROImin[0][19][ 7]=   -2.57921;PhiROImax[0][19][ 7]=	-2.47231;
     EtaROImin[0][19][ 8]=  -0.648101;EtaROImax[0][19][ 8]=  -0.555789;PhiROImin[0][19][ 8]=   -2.46881;PhiROImax[0][19][ 8]=	-2.35915;
     EtaROImin[0][19][ 9]=  -0.554444;EtaROImax[0][19][ 9]=  -0.458091;PhiROImin[0][19][ 9]=   -2.46881;PhiROImax[0][19][ 9]=	-2.35915;
     EtaROImin[0][19][10]=  -0.648101;EtaROImax[0][19][10]=  -0.555789;PhiROImin[0][19][10]=   -2.57921;PhiROImax[0][19][10]=	-2.47231;
     EtaROImin[0][19][11]=  -0.554444;EtaROImax[0][19][11]=  -0.458091;PhiROImin[0][19][11]=   -2.57921;PhiROImax[0][19][11]=	-2.47231;
     EtaROImin[0][19][12]=  -0.776449;EtaROImax[0][19][12]=  -0.731124;PhiROImin[0][19][12]=   -2.46941;PhiROImax[0][19][12]=	-2.35916;
     EtaROImin[0][19][13]=  -0.728056;EtaROImax[0][19][13]=  -0.655857;PhiROImin[0][19][13]=   -2.46941;PhiROImax[0][19][13]=	-2.35916;
     EtaROImin[0][19][14]=  -0.776449;EtaROImax[0][19][14]=  -0.731124;PhiROImin[0][19][14]=   -2.58036;PhiROImax[0][19][14]=	-2.47293;
     EtaROImin[0][19][15]=  -0.728056;EtaROImax[0][19][15]=  -0.655857;PhiROImin[0][19][15]=   -2.58036;PhiROImax[0][19][15]=	-2.47293;
     EtaROImin[0][19][16]=  -0.895163;EtaROImax[0][19][16]=  -0.853359;PhiROImin[0][19][16]=   -2.46941;PhiROImax[0][19][16]=	-2.35916;
     EtaROImin[0][19][17]=  -0.850528;EtaROImax[0][19][17]=  -0.783867;PhiROImin[0][19][17]=   -2.46941;PhiROImax[0][19][17]=	-2.35916;
     EtaROImin[0][19][18]=  -0.895163;EtaROImax[0][19][18]=  -0.853359;PhiROImin[0][19][18]=   -2.58036;PhiROImax[0][19][18]=	-2.47293;
     EtaROImin[0][19][19]=  -0.850528;EtaROImax[0][19][19]=  -0.783867;PhiROImin[0][19][19]=   -2.58036;PhiROImax[0][19][19]=	-2.47293;
     EtaROImin[0][19][20]=   -1.02514;EtaROImax[0][19][20]=  -0.964674;PhiROImin[0][19][20]=   -2.46881;PhiROImax[0][19][20]=	-2.35915;
     EtaROImin[0][19][21]=  -0.961344;EtaROImax[0][19][21]=  -0.898201;PhiROImin[0][19][21]=   -2.46881;PhiROImax[0][19][21]=	-2.35915;
     EtaROImin[0][19][22]=   -1.02514;EtaROImax[0][19][22]=  -0.964674;PhiROImin[0][19][22]=   -2.57921;PhiROImax[0][19][22]=	-2.47231;
     EtaROImin[0][19][23]=  -0.961344;EtaROImax[0][19][23]=  -0.898201;PhiROImin[0][19][23]=   -2.57921;PhiROImax[0][19][23]=	-2.47231;
     EtaROImin[0][19][24]=	    0;EtaROImax[0][19][24]=	     0;PhiROImin[0][19][24]=	      0;PhiROImax[0][19][24]=	       0;
     EtaROImin[0][19][25]=   -1.06547;EtaROImax[0][19][25]=   -1.03003;PhiROImin[0][19][25]=   -2.46962;PhiROImax[0][19][25]=	-2.35926;
     EtaROImin[0][19][26]=	    0;EtaROImax[0][19][26]=	     0;PhiROImin[0][19][26]=	      0;PhiROImax[0][19][26]=	       0;
     EtaROImin[0][19][27]=   -1.06547;EtaROImax[0][19][27]=   -1.03003;PhiROImin[0][19][27]=   -2.55334;PhiROImax[0][19][27]=	-2.47315;
     EtaROImin[0][19][28]=	    0;EtaROImax[0][19][28]=	     0;PhiROImin[0][19][28]=	      0;PhiROImax[0][19][28]=	       0;
     EtaROImin[0][19][29]=	    0;EtaROImax[0][19][29]=	     0;PhiROImin[0][19][29]=	      0;PhiROImax[0][19][29]=	       0;
     EtaROImin[0][19][30]=	    0;EtaROImax[0][19][30]=	     0;PhiROImin[0][19][30]=	      0;PhiROImax[0][19][30]=	       0;
     EtaROImin[0][19][31]=	    0;EtaROImax[0][19][31]=	     0;PhiROImin[0][19][31]=	      0;PhiROImax[0][19][31]=	       0;
     EtaROImin[0][20][ 0]=  -0.145912;EtaROImax[0][20][ 0]= -0.0874905;PhiROImin[0][20][ 0]=   -2.35323;PhiROImax[0][20][ 0]=	-2.24298;
     EtaROImin[0][20][ 1]=  -0.238294;EtaROImax[0][20][ 1]=  -0.149792;PhiROImin[0][20][ 1]=   -2.35323;PhiROImax[0][20][ 1]=	-2.24298;
     EtaROImin[0][20][ 2]=  -0.145912;EtaROImax[0][20][ 2]= -0.0874905;PhiROImin[0][20][ 2]=   -2.23946;PhiROImax[0][20][ 2]=	-2.13203;
     EtaROImin[0][20][ 3]=  -0.238294;EtaROImax[0][20][ 3]=  -0.149792;PhiROImin[0][20][ 3]=   -2.23946;PhiROImax[0][20][ 3]=	-2.13203;
     EtaROImin[0][20][ 4]=   -0.35049;EtaROImax[0][20][ 4]=  -0.246207;PhiROImin[0][20][ 4]=   -2.35324;PhiROImax[0][20][ 4]=	-2.24358;
     EtaROImin[0][20][ 5]=  -0.452099;EtaROImax[0][20][ 5]=  -0.350831;PhiROImin[0][20][ 5]=   -2.35324;PhiROImax[0][20][ 5]=	-2.24358;
     EtaROImin[0][20][ 6]=   -0.35049;EtaROImax[0][20][ 6]=  -0.246207;PhiROImin[0][20][ 6]=   -2.24008;PhiROImax[0][20][ 6]=	-2.13318;
     EtaROImin[0][20][ 7]=  -0.452099;EtaROImax[0][20][ 7]=  -0.350831;PhiROImin[0][20][ 7]=   -2.24008;PhiROImax[0][20][ 7]=	-2.13318;
     EtaROImin[0][20][ 8]=  -0.554444;EtaROImax[0][20][ 8]=  -0.458091;PhiROImin[0][20][ 8]=   -2.35324;PhiROImax[0][20][ 8]=	-2.24358;
     EtaROImin[0][20][ 9]=  -0.648101;EtaROImax[0][20][ 9]=  -0.555789;PhiROImin[0][20][ 9]=   -2.35324;PhiROImax[0][20][ 9]=	-2.24358;
     EtaROImin[0][20][10]=  -0.554444;EtaROImax[0][20][10]=  -0.458091;PhiROImin[0][20][10]=   -2.24008;PhiROImax[0][20][10]=	-2.13318;
     EtaROImin[0][20][11]=  -0.648101;EtaROImax[0][20][11]=  -0.555789;PhiROImin[0][20][11]=   -2.24008;PhiROImax[0][20][11]=	-2.13318;
     EtaROImin[0][20][12]=  -0.703298;EtaROImax[0][20][12]=  -0.655857;PhiROImin[0][20][12]=   -2.35323;PhiROImax[0][20][12]=	-2.24298;
     EtaROImin[0][20][13]=  -0.776449;EtaROImax[0][20][13]=  -0.706413;PhiROImin[0][20][13]=   -2.35323;PhiROImax[0][20][13]=	-2.24298;
     EtaROImin[0][20][14]=  -0.703298;EtaROImax[0][20][14]=  -0.655857;PhiROImin[0][20][14]=   -2.23946;PhiROImax[0][20][14]=	-2.13203;
     EtaROImin[0][20][15]=  -0.776449;EtaROImax[0][20][15]=  -0.706413;PhiROImin[0][20][15]=   -2.23946;PhiROImax[0][20][15]=	-2.13203;
     EtaROImin[0][20][16]=   -0.82768;EtaROImax[0][20][16]=  -0.783867;PhiROImin[0][20][16]=   -2.35323;PhiROImax[0][20][16]=	-2.24298;
     EtaROImin[0][20][17]=  -0.895163;EtaROImax[0][20][17]=  -0.830556;PhiROImin[0][20][17]=   -2.35323;PhiROImax[0][20][17]=	-2.24298;
     EtaROImin[0][20][18]=   -0.82768;EtaROImax[0][20][18]=  -0.783867;PhiROImin[0][20][18]=   -2.23946;PhiROImax[0][20][18]=	-2.13203;
     EtaROImin[0][20][19]=  -0.895163;EtaROImax[0][20][19]=  -0.830556;PhiROImin[0][20][19]=   -2.23946;PhiROImax[0][20][19]=	-2.13203;
     EtaROImin[0][20][20]=  -0.961344;EtaROImax[0][20][20]=  -0.898201;PhiROImin[0][20][20]=   -2.35324;PhiROImax[0][20][20]=	-2.24358;
     EtaROImin[0][20][21]=   -1.02514;EtaROImax[0][20][21]=  -0.964674;PhiROImin[0][20][21]=   -2.35324;PhiROImax[0][20][21]=	-2.24358;
     EtaROImin[0][20][22]=  -0.961344;EtaROImax[0][20][22]=  -0.898201;PhiROImin[0][20][22]=   -2.24008;PhiROImax[0][20][22]=	-2.13318;
     EtaROImin[0][20][23]=   -1.02514;EtaROImax[0][20][23]=  -0.964674;PhiROImin[0][20][23]=   -2.24008;PhiROImax[0][20][23]=	-2.13318;
     EtaROImin[0][20][24]=   -1.06547;EtaROImax[0][20][24]=   -1.03003;PhiROImin[0][20][24]=   -2.35313;PhiROImax[0][20][24]=	-2.24277;
     EtaROImin[0][20][25]=	    0;EtaROImax[0][20][25]=	     0;PhiROImin[0][20][25]=	      0;PhiROImax[0][20][25]=	       0;
     EtaROImin[0][20][26]=   -1.06547;EtaROImax[0][20][26]=   -1.03003;PhiROImin[0][20][26]=   -2.23924;PhiROImax[0][20][26]=	-2.15905;
     EtaROImin[0][20][27]=	    0;EtaROImax[0][20][27]=	     0;PhiROImin[0][20][27]=	      0;PhiROImax[0][20][27]=	       0;
     EtaROImin[0][20][28]=	    0;EtaROImax[0][20][28]=	     0;PhiROImin[0][20][28]=	      0;PhiROImax[0][20][28]=	       0;
     EtaROImin[0][20][29]=	    0;EtaROImax[0][20][29]=	     0;PhiROImin[0][20][29]=	      0;PhiROImax[0][20][29]=	       0;
     EtaROImin[0][20][30]=	    0;EtaROImax[0][20][30]=	     0;PhiROImin[0][20][30]=	      0;PhiROImax[0][20][30]=	       0;
     EtaROImin[0][20][31]=	    0;EtaROImax[0][20][31]=	     0;PhiROImin[0][20][31]=	      0;PhiROImax[0][20][31]=	       0;
     EtaROImin[0][21][ 0]=  -0.203032;EtaROImax[0][21][ 0]=  -0.155043;PhiROImin[0][21][ 0]=   -2.05024;PhiROImax[0][21][ 0]=	-1.96629;
     EtaROImin[0][21][ 1]=   -0.15183;EtaROImax[0][21][ 1]= -0.0775634;PhiROImin[0][21][ 1]=   -2.05024;PhiROImax[0][21][ 1]=	-1.96629;
     EtaROImin[0][21][ 2]=  -0.203032;EtaROImax[0][21][ 2]=  -0.155043;PhiROImin[0][21][ 2]=   -2.13655;PhiROImax[0][21][ 2]=	-2.05387;
     EtaROImin[0][21][ 3]=   -0.15183;EtaROImax[0][21][ 3]= -0.0775634;PhiROImin[0][21][ 3]=   -2.13655;PhiROImax[0][21][ 3]=	-2.05387;
     EtaROImin[0][21][ 4]=  -0.322732;EtaROImax[0][21][ 4]=  -0.276301;PhiROImin[0][21][ 4]=   -2.04983;PhiROImax[0][21][ 4]=	-1.96627;
     EtaROImin[0][21][ 5]=  -0.273184;EtaROImax[0][21][ 5]=  -0.200823;PhiROImin[0][21][ 5]=   -2.04983;PhiROImax[0][21][ 5]=	-1.96627;
     EtaROImin[0][21][ 6]=  -0.322732;EtaROImax[0][21][ 6]=  -0.276301;PhiROImin[0][21][ 6]=   -2.13573;PhiROImax[0][21][ 6]=	-2.05344;
     EtaROImin[0][21][ 7]=  -0.273184;EtaROImax[0][21][ 7]=  -0.200823;PhiROImin[0][21][ 7]=   -2.13573;PhiROImax[0][21][ 7]=	-2.05344;
     EtaROImin[0][21][ 8]=   -0.51487;EtaROImax[0][21][ 8]=  -0.439011;PhiROImin[0][21][ 8]=   -2.04536;PhiROImax[0][21][ 8]=	-1.96564;
     EtaROImin[0][21][ 9]=  -0.441017;EtaROImax[0][21][ 9]=  -0.362484;PhiROImin[0][21][ 9]=   -2.04536;PhiROImax[0][21][ 9]=	-1.96564;
     EtaROImin[0][21][10]=   -0.51487;EtaROImax[0][21][10]=  -0.439011;PhiROImin[0][21][10]=   -2.12657;PhiROImax[0][21][10]=	-2.04792;
     EtaROImin[0][21][11]=  -0.441017;EtaROImax[0][21][11]=  -0.362484;PhiROImin[0][21][11]=   -2.12657;PhiROImax[0][21][11]=	-2.04792;
     EtaROImin[0][21][12]=  -0.659332;EtaROImax[0][21][12]=  -0.566067;PhiROImin[0][21][12]=   -2.05024;PhiROImax[0][21][12]=	-1.96629;
     EtaROImin[0][21][13]=  -0.569141;EtaROImax[0][21][13]=  -0.470878;PhiROImin[0][21][13]=   -2.05024;PhiROImax[0][21][13]=	-1.96629;
     EtaROImin[0][21][14]=  -0.659332;EtaROImax[0][21][14]=  -0.566067;PhiROImin[0][21][14]=   -2.13655;PhiROImax[0][21][14]=	-2.05387;
     EtaROImin[0][21][15]=  -0.569141;EtaROImax[0][21][15]=  -0.470878;PhiROImin[0][21][15]=   -2.13655;PhiROImax[0][21][15]=	-2.05387;
     EtaROImin[0][21][16]=   -0.72184;EtaROImax[0][21][16]=  -0.671529;PhiROImin[0][21][16]=   -2.04536;PhiROImax[0][21][16]=	-1.96564;
     EtaROImin[0][21][17]=  -0.674155;EtaROImax[0][21][17]=  -0.622205;PhiROImin[0][21][17]=   -2.04536;PhiROImax[0][21][17]=	-1.96564;
     EtaROImin[0][21][18]=   -0.72184;EtaROImax[0][21][18]=  -0.671529;PhiROImin[0][21][18]=   -2.12657;PhiROImax[0][21][18]=	-2.04792;
     EtaROImin[0][21][19]=  -0.674155;EtaROImax[0][21][19]=  -0.622205;PhiROImin[0][21][19]=   -2.12657;PhiROImax[0][21][19]=	-2.04792;
     EtaROImin[0][21][20]=  -0.893028;EtaROImax[0][21][20]=  -0.833797;PhiROImin[0][21][20]=   -2.04536;PhiROImax[0][21][20]=	-1.96564;
     EtaROImin[0][21][21]=  -0.831846;EtaROImax[0][21][21]=  -0.744499;PhiROImin[0][21][21]=   -2.04536;PhiROImax[0][21][21]=	-1.96564;
     EtaROImin[0][21][22]=  -0.893028;EtaROImax[0][21][22]=  -0.833797;PhiROImin[0][21][22]=   -2.12657;PhiROImax[0][21][22]=	-2.04792;
     EtaROImin[0][21][23]=  -0.831846;EtaROImax[0][21][23]=  -0.744499;PhiROImin[0][21][23]=   -2.12657;PhiROImax[0][21][23]=	-2.04792;
     EtaROImin[0][21][24]=  -0.905303;EtaROImax[0][21][24]=  -0.846464;PhiROImin[0][21][24]=   -2.05024;PhiROImax[0][21][24]=	-1.96629;
     EtaROImin[0][21][25]=  -0.850339;EtaROImax[0][21][25]=  -0.788882;PhiROImin[0][21][25]=   -2.05024;PhiROImax[0][21][25]=	-1.96629;
     EtaROImin[0][21][26]=  -0.905303;EtaROImax[0][21][26]=  -0.846464;PhiROImin[0][21][26]=   -2.13655;PhiROImax[0][21][26]=	-2.05387;
     EtaROImin[0][21][27]=  -0.850339;EtaROImax[0][21][27]=  -0.788882;PhiROImin[0][21][27]=   -2.13655;PhiROImax[0][21][27]=	-2.05387;
     EtaROImin[0][21][28]=   -0.98143;EtaROImax[0][21][28]=  -0.954883;PhiROImin[0][21][28]=   -2.03358;PhiROImax[0][21][28]=	-1.98444;
     EtaROImin[0][21][29]=  -0.953094;EtaROImax[0][21][29]=  -0.911293;PhiROImin[0][21][29]=   -2.03358;PhiROImax[0][21][29]=	-1.98444;
     EtaROImin[0][21][30]=   -0.98143;EtaROImax[0][21][30]=  -0.954883;PhiROImin[0][21][30]=   -2.12127;PhiROImax[0][21][30]=	-2.03635;
     EtaROImin[0][21][31]=  -0.953094;EtaROImax[0][21][31]=  -0.911293;PhiROImin[0][21][31]=   -2.12127;PhiROImax[0][21][31]=	-2.03635;
     EtaROImin[0][22][ 0]=  -0.126072;EtaROImax[0][22][ 0]= -0.0775634;PhiROImin[0][22][ 0]=   -1.96071;PhiROImax[0][22][ 0]=	-1.87675;
     EtaROImin[0][22][ 1]=  -0.203032;EtaROImax[0][22][ 1]=  -0.129296;PhiROImin[0][22][ 1]=   -1.96071;PhiROImax[0][22][ 1]=	-1.87675;
     EtaROImin[0][22][ 2]=  -0.126072;EtaROImax[0][22][ 2]= -0.0775634;PhiROImin[0][22][ 2]=   -1.87312;PhiROImax[0][22][ 2]=	-1.79045;
     EtaROImin[0][22][ 3]=  -0.203032;EtaROImax[0][22][ 3]=  -0.129296;PhiROImin[0][22][ 3]=   -1.87312;PhiROImax[0][22][ 3]=	-1.79045;
     EtaROImin[0][22][ 4]=  -0.248155;EtaROImax[0][22][ 4]=  -0.200823;PhiROImin[0][22][ 4]=   -1.96072;PhiROImax[0][22][ 4]=	-1.87716;
     EtaROImin[0][22][ 5]=  -0.322732;EtaROImax[0][22][ 5]=  -0.251292;PhiROImin[0][22][ 5]=   -1.96072;PhiROImax[0][22][ 5]=	-1.87716;
     EtaROImin[0][22][ 6]=  -0.248155;EtaROImax[0][22][ 6]=  -0.200823;PhiROImin[0][22][ 6]=   -1.87355;PhiROImax[0][22][ 6]=	-1.79126;
     EtaROImin[0][22][ 7]=  -0.322732;EtaROImax[0][22][ 7]=  -0.251292;PhiROImin[0][22][ 7]=   -1.87355;PhiROImax[0][22][ 7]=	-1.79126;
     EtaROImin[0][22][ 8]=  -0.441017;EtaROImax[0][22][ 8]=  -0.362484;PhiROImin[0][22][ 8]=   -1.96135;PhiROImax[0][22][ 8]=	-1.88163;
     EtaROImin[0][22][ 9]=   -0.51487;EtaROImax[0][22][ 9]=  -0.439011;PhiROImin[0][22][ 9]=   -1.96135;PhiROImax[0][22][ 9]=	-1.88163;
     EtaROImin[0][22][10]=  -0.441017;EtaROImax[0][22][10]=  -0.362484;PhiROImin[0][22][10]=   -1.87907;PhiROImax[0][22][10]=	-1.80042;
     EtaROImin[0][22][11]=   -0.51487;EtaROImax[0][22][11]=  -0.439011;PhiROImin[0][22][11]=   -1.87907;PhiROImax[0][22][11]=	-1.80042;
     EtaROImin[0][22][12]=  -0.569141;EtaROImax[0][22][12]=  -0.470878;PhiROImin[0][22][12]=   -1.96071;PhiROImax[0][22][12]=	-1.87675;
     EtaROImin[0][22][13]=  -0.659332;EtaROImax[0][22][13]=  -0.566067;PhiROImin[0][22][13]=   -1.96071;PhiROImax[0][22][13]=	-1.87675;
     EtaROImin[0][22][14]=  -0.569141;EtaROImax[0][22][14]=  -0.470878;PhiROImin[0][22][14]=   -1.87312;PhiROImax[0][22][14]=	-1.79045;
     EtaROImin[0][22][15]=  -0.659332;EtaROImax[0][22][15]=  -0.566067;PhiROImin[0][22][15]=   -1.87312;PhiROImax[0][22][15]=	-1.79045;
     EtaROImin[0][22][16]=  -0.674155;EtaROImax[0][22][16]=  -0.622205;PhiROImin[0][22][16]=   -1.96135;PhiROImax[0][22][16]=	-1.88163;
     EtaROImin[0][22][17]=   -0.72184;EtaROImax[0][22][17]=  -0.671529;PhiROImin[0][22][17]=   -1.96135;PhiROImax[0][22][17]=	-1.88163;
     EtaROImin[0][22][18]=  -0.674155;EtaROImax[0][22][18]=  -0.622205;PhiROImin[0][22][18]=   -1.87907;PhiROImax[0][22][18]=	-1.80042;
     EtaROImin[0][22][19]=   -0.72184;EtaROImax[0][22][19]=  -0.671529;PhiROImin[0][22][19]=   -1.87907;PhiROImax[0][22][19]=	-1.80042;
     EtaROImin[0][22][20]=  -0.831846;EtaROImax[0][22][20]=  -0.744499;PhiROImin[0][22][20]=   -1.96135;PhiROImax[0][22][20]=	-1.88163;
     EtaROImin[0][22][21]=  -0.893028;EtaROImax[0][22][21]=  -0.833797;PhiROImin[0][22][21]=   -1.96135;PhiROImax[0][22][21]=	-1.88163;
     EtaROImin[0][22][22]=  -0.831846;EtaROImax[0][22][22]=  -0.744499;PhiROImin[0][22][22]=   -1.87907;PhiROImax[0][22][22]=	-1.80042;
     EtaROImin[0][22][23]=  -0.893028;EtaROImax[0][22][23]=  -0.833797;PhiROImin[0][22][23]=   -1.87907;PhiROImax[0][22][23]=	-1.80042;
     EtaROImin[0][22][24]=  -0.850339;EtaROImax[0][22][24]=  -0.788882;PhiROImin[0][22][24]=   -1.96071;PhiROImax[0][22][24]=	-1.87675;
     EtaROImin[0][22][25]=  -0.905303;EtaROImax[0][22][25]=  -0.846464;PhiROImin[0][22][25]=   -1.96071;PhiROImax[0][22][25]=	-1.87675;
     EtaROImin[0][22][26]=  -0.850339;EtaROImax[0][22][26]=  -0.788882;PhiROImin[0][22][26]=   -1.87312;PhiROImax[0][22][26]=	-1.79045;
     EtaROImin[0][22][27]=  -0.905303;EtaROImax[0][22][27]=  -0.846464;PhiROImin[0][22][27]=   -1.87312;PhiROImax[0][22][27]=	-1.79045;
     EtaROImin[0][22][28]=  -0.940931;EtaROImax[0][22][28]=  -0.913486;PhiROImin[0][22][28]=   -1.97686;PhiROImax[0][22][28]=	-1.89087;
     EtaROImin[0][22][29]=  -0.983719;EtaROImax[0][22][29]=  -0.942741;PhiROImin[0][22][29]=   -1.97686;PhiROImax[0][22][29]=	-1.89087;
     EtaROImin[0][22][30]=  -0.940931;EtaROImax[0][22][30]=  -0.913486;PhiROImin[0][22][30]=   -1.98444;PhiROImax[0][22][30]=	-1.88811;
     EtaROImin[0][22][31]=  -0.983719;EtaROImax[0][22][31]=  -0.942741;PhiROImin[0][22][31]=   -1.98444;PhiROImax[0][22][31]=	-1.88811;
     EtaROImin[0][23][ 0]=  -0.238866;EtaROImax[0][23][ 0]=  -0.131182;PhiROImin[0][23][ 0]=   -1.68341;PhiROImax[0][23][ 0]=	-1.57375;
     EtaROImin[0][23][ 1]=  -0.132007;EtaROImax[0][23][ 1]=   -0.02338;PhiROImin[0][23][ 1]=   -1.68341;PhiROImax[0][23][ 1]=	-1.57375;
     EtaROImin[0][23][ 2]=  -0.238866;EtaROImax[0][23][ 2]=  -0.131182;PhiROImin[0][23][ 2]=   -1.79381;PhiROImax[0][23][ 2]=	-1.68692;
     EtaROImin[0][23][ 3]=  -0.132007;EtaROImax[0][23][ 3]=   -0.02338;PhiROImin[0][23][ 3]=   -1.79381;PhiROImax[0][23][ 3]=	-1.68692;
     EtaROImin[0][23][ 4]=  -0.452099;EtaROImax[0][23][ 4]=  -0.350831;PhiROImin[0][23][ 4]=   -1.68341;PhiROImax[0][23][ 4]=	-1.57375;
     EtaROImin[0][23][ 5]=   -0.35049;EtaROImax[0][23][ 5]=  -0.246207;PhiROImin[0][23][ 5]=   -1.68341;PhiROImax[0][23][ 5]=	-1.57375;
     EtaROImin[0][23][ 6]=  -0.452099;EtaROImax[0][23][ 6]=  -0.350831;PhiROImin[0][23][ 6]=   -1.79381;PhiROImax[0][23][ 6]=	-1.68692;
     EtaROImin[0][23][ 7]=   -0.35049;EtaROImax[0][23][ 7]=  -0.246207;PhiROImin[0][23][ 7]=   -1.79381;PhiROImax[0][23][ 7]=	-1.68692;
     EtaROImin[0][23][ 8]=  -0.648101;EtaROImax[0][23][ 8]=  -0.555789;PhiROImin[0][23][ 8]=   -1.68341;PhiROImax[0][23][ 8]=	-1.57375;
     EtaROImin[0][23][ 9]=  -0.554444;EtaROImax[0][23][ 9]=  -0.458091;PhiROImin[0][23][ 9]=   -1.68341;PhiROImax[0][23][ 9]=	-1.57375;
     EtaROImin[0][23][10]=  -0.648101;EtaROImax[0][23][10]=  -0.555789;PhiROImin[0][23][10]=   -1.79381;PhiROImax[0][23][10]=	-1.68692;
     EtaROImin[0][23][11]=  -0.554444;EtaROImax[0][23][11]=  -0.458091;PhiROImin[0][23][11]=   -1.79381;PhiROImax[0][23][11]=	-1.68692;
     EtaROImin[0][23][12]=  -0.786462;EtaROImax[0][23][12]=  -0.745327;PhiROImin[0][23][12]=   -1.62692;PhiROImax[0][23][12]=	 -1.5708;
     EtaROImin[0][23][13]=  -0.742545;EtaROImax[0][23][13]=  -0.677222;PhiROImin[0][23][13]=   -1.62692;PhiROImax[0][23][13]=	 -1.5708;
     EtaROImin[0][23][14]=  -0.786462;EtaROImax[0][23][14]=  -0.745327;PhiROImin[0][23][14]=   -1.69744;PhiROImax[0][23][14]=	-1.63046;
     EtaROImin[0][23][15]=  -0.742545;EtaROImax[0][23][15]=  -0.677222;PhiROImin[0][23][15]=   -1.69744;PhiROImax[0][23][15]=	-1.63046;
     EtaROImin[0][23][16]=  -0.895163;EtaROImax[0][23][16]=  -0.853359;PhiROImin[0][23][16]=   -1.68401;PhiROImax[0][23][16]=	-1.57376;
     EtaROImin[0][23][17]=  -0.850528;EtaROImax[0][23][17]=  -0.783867;PhiROImin[0][23][17]=   -1.68401;PhiROImax[0][23][17]=	-1.57376;
     EtaROImin[0][23][18]=  -0.895163;EtaROImax[0][23][18]=  -0.853359;PhiROImin[0][23][18]=   -1.79497;PhiROImax[0][23][18]=	-1.68753;
     EtaROImin[0][23][19]=  -0.850528;EtaROImax[0][23][19]=  -0.783867;PhiROImin[0][23][19]=   -1.79497;PhiROImax[0][23][19]=	-1.68753;
     EtaROImin[0][23][20]=   -1.02514;EtaROImax[0][23][20]=  -0.964674;PhiROImin[0][23][20]=   -1.68341;PhiROImax[0][23][20]=	-1.57375;
     EtaROImin[0][23][21]=  -0.961344;EtaROImax[0][23][21]=  -0.898201;PhiROImin[0][23][21]=   -1.68341;PhiROImax[0][23][21]=	-1.57375;
     EtaROImin[0][23][22]=   -1.02514;EtaROImax[0][23][22]=  -0.964674;PhiROImin[0][23][22]=   -1.79381;PhiROImax[0][23][22]=	-1.68692;
     EtaROImin[0][23][23]=  -0.961344;EtaROImax[0][23][23]=  -0.898201;PhiROImin[0][23][23]=   -1.79381;PhiROImax[0][23][23]=	-1.68692;
     EtaROImin[0][23][24]=	    0;EtaROImax[0][23][24]=	     0;PhiROImin[0][23][24]=	      0;PhiROImax[0][23][24]=	       0;
     EtaROImin[0][23][25]=   -1.06547;EtaROImax[0][23][25]=   -1.03003;PhiROImin[0][23][25]=   -1.68423;PhiROImax[0][23][25]=	-1.57387;
     EtaROImin[0][23][26]=	    0;EtaROImax[0][23][26]=	     0;PhiROImin[0][23][26]=	      0;PhiROImax[0][23][26]=	       0;
     EtaROImin[0][23][27]=   -1.06547;EtaROImax[0][23][27]=   -1.03003;PhiROImin[0][23][27]=   -1.76794;PhiROImax[0][23][27]=	-1.68775;
     EtaROImin[0][23][28]=	    0;EtaROImax[0][23][28]=	     0;PhiROImin[0][23][28]=	      0;PhiROImax[0][23][28]=	       0;
     EtaROImin[0][23][29]=	    0;EtaROImax[0][23][29]=	     0;PhiROImin[0][23][29]=	      0;PhiROImax[0][23][29]=	       0;
     EtaROImin[0][23][30]=	    0;EtaROImax[0][23][30]=	     0;PhiROImin[0][23][30]=	      0;PhiROImax[0][23][30]=	       0;
     EtaROImin[0][23][31]=	    0;EtaROImax[0][23][31]=	     0;PhiROImin[0][23][31]=	      0;PhiROImax[0][23][31]=	       0;
     EtaROImin[0][24][ 0]=  -0.132007;EtaROImax[0][24][ 0]=   -0.02338;PhiROImin[0][24][ 0]=   -1.56784;PhiROImax[0][24][ 0]=	-1.45818;
     EtaROImin[0][24][ 1]=  -0.238866;EtaROImax[0][24][ 1]=  -0.131182;PhiROImin[0][24][ 1]=   -1.56784;PhiROImax[0][24][ 1]=	-1.45818;
     EtaROImin[0][24][ 2]=  -0.132007;EtaROImax[0][24][ 2]=   -0.02338;PhiROImin[0][24][ 2]=   -1.45468;PhiROImax[0][24][ 2]=	-1.34779;
     EtaROImin[0][24][ 3]=  -0.238866;EtaROImax[0][24][ 3]=  -0.131182;PhiROImin[0][24][ 3]=   -1.45468;PhiROImax[0][24][ 3]=	-1.34779;
     EtaROImin[0][24][ 4]=   -0.35049;EtaROImax[0][24][ 4]=  -0.246207;PhiROImin[0][24][ 4]=   -1.56784;PhiROImax[0][24][ 4]=	-1.45818;
     EtaROImin[0][24][ 5]=  -0.452099;EtaROImax[0][24][ 5]=  -0.350831;PhiROImin[0][24][ 5]=   -1.56784;PhiROImax[0][24][ 5]=	-1.45818;
     EtaROImin[0][24][ 6]=   -0.35049;EtaROImax[0][24][ 6]=  -0.246207;PhiROImin[0][24][ 6]=   -1.45468;PhiROImax[0][24][ 6]=	-1.34779;
     EtaROImin[0][24][ 7]=  -0.452099;EtaROImax[0][24][ 7]=  -0.350831;PhiROImin[0][24][ 7]=   -1.45468;PhiROImax[0][24][ 7]=	-1.34779;
     EtaROImin[0][24][ 8]=  -0.554444;EtaROImax[0][24][ 8]=  -0.458091;PhiROImin[0][24][ 8]=   -1.56784;PhiROImax[0][24][ 8]=	-1.45818;
     EtaROImin[0][24][ 9]=  -0.648101;EtaROImax[0][24][ 9]=  -0.555789;PhiROImin[0][24][ 9]=   -1.56784;PhiROImax[0][24][ 9]=	-1.45818;
     EtaROImin[0][24][10]=  -0.554444;EtaROImax[0][24][10]=  -0.458091;PhiROImin[0][24][10]=   -1.45468;PhiROImax[0][24][10]=	-1.34779;
     EtaROImin[0][24][11]=  -0.648101;EtaROImax[0][24][11]=  -0.555789;PhiROImin[0][24][11]=   -1.45468;PhiROImax[0][24][11]=	-1.34779;
     EtaROImin[0][24][12]=  -0.720119;EtaROImax[0][24][12]=  -0.677222;PhiROImin[0][24][12]=   -1.56798;PhiROImax[0][24][12]=	 -1.5005;
     EtaROImin[0][24][13]=  -0.786462;EtaROImax[0][24][13]=   -0.72294;PhiROImin[0][24][13]=   -1.56798;PhiROImax[0][24][13]=	 -1.5005;
     EtaROImin[0][24][14]=  -0.720119;EtaROImax[0][24][14]=  -0.677222;PhiROImin[0][24][14]=	-1.5708;PhiROImax[0][24][14]=	-1.49696;
     EtaROImin[0][24][15]=  -0.786462;EtaROImax[0][24][15]=   -0.72294;PhiROImin[0][24][15]=	-1.5708;PhiROImax[0][24][15]=	-1.49696;
     EtaROImin[0][24][16]=   -0.82768;EtaROImax[0][24][16]=  -0.783867;PhiROImin[0][24][16]=   -1.56783;PhiROImax[0][24][16]=	-1.45758;
     EtaROImin[0][24][17]=  -0.895163;EtaROImax[0][24][17]=  -0.830556;PhiROImin[0][24][17]=   -1.56783;PhiROImax[0][24][17]=	-1.45758;
     EtaROImin[0][24][18]=   -0.82768;EtaROImax[0][24][18]=  -0.783867;PhiROImin[0][24][18]=   -1.45406;PhiROImax[0][24][18]=	-1.34663;
     EtaROImin[0][24][19]=  -0.895163;EtaROImax[0][24][19]=  -0.830556;PhiROImin[0][24][19]=   -1.45406;PhiROImax[0][24][19]=	-1.34663;
     EtaROImin[0][24][20]=  -0.961344;EtaROImax[0][24][20]=  -0.898201;PhiROImin[0][24][20]=   -1.56784;PhiROImax[0][24][20]=	-1.45818;
     EtaROImin[0][24][21]=   -1.02514;EtaROImax[0][24][21]=  -0.964674;PhiROImin[0][24][21]=   -1.56784;PhiROImax[0][24][21]=	-1.45818;
     EtaROImin[0][24][22]=  -0.961344;EtaROImax[0][24][22]=  -0.898201;PhiROImin[0][24][22]=   -1.45468;PhiROImax[0][24][22]=	-1.34779;
     EtaROImin[0][24][23]=   -1.02514;EtaROImax[0][24][23]=  -0.964674;PhiROImin[0][24][23]=   -1.45468;PhiROImax[0][24][23]=	-1.34779;
     EtaROImin[0][24][24]=   -1.06547;EtaROImax[0][24][24]=   -1.03003;PhiROImin[0][24][24]=   -1.56773;PhiROImax[0][24][24]=	-1.45737;
     EtaROImin[0][24][25]=	    0;EtaROImax[0][24][25]=	     0;PhiROImin[0][24][25]=	      0;PhiROImax[0][24][25]=	       0;
     EtaROImin[0][24][26]=   -1.06547;EtaROImax[0][24][26]=   -1.03003;PhiROImin[0][24][26]=   -1.45384;PhiROImax[0][24][26]=	-1.37365;
     EtaROImin[0][24][27]=	    0;EtaROImax[0][24][27]=	     0;PhiROImin[0][24][27]=	      0;PhiROImax[0][24][27]=	       0;
     EtaROImin[0][24][28]=	    0;EtaROImax[0][24][28]=	     0;PhiROImin[0][24][28]=	      0;PhiROImax[0][24][28]=	       0;
     EtaROImin[0][24][29]=	    0;EtaROImax[0][24][29]=	     0;PhiROImin[0][24][29]=	      0;PhiROImax[0][24][29]=	       0;
     EtaROImin[0][24][30]=	    0;EtaROImax[0][24][30]=	     0;PhiROImin[0][24][30]=	      0;PhiROImax[0][24][30]=	       0;
     EtaROImin[0][24][31]=	    0;EtaROImax[0][24][31]=	     0;PhiROImin[0][24][31]=	      0;PhiROImax[0][24][31]=	       0;
     EtaROImin[0][25][ 0]=  -0.203032;EtaROImax[0][25][ 0]=  -0.155043;PhiROImin[0][25][ 0]=   -1.26484;PhiROImax[0][25][ 0]=	-1.18089;
     EtaROImin[0][25][ 1]=   -0.15183;EtaROImax[0][25][ 1]= -0.0775634;PhiROImin[0][25][ 1]=   -1.26484;PhiROImax[0][25][ 1]=	-1.18089;
     EtaROImin[0][25][ 2]=  -0.203032;EtaROImax[0][25][ 2]=  -0.155043;PhiROImin[0][25][ 2]=   -1.35115;PhiROImax[0][25][ 2]=	-1.26847;
     EtaROImin[0][25][ 3]=   -0.15183;EtaROImax[0][25][ 3]= -0.0775634;PhiROImin[0][25][ 3]=   -1.35115;PhiROImax[0][25][ 3]=	-1.26847;
     EtaROImin[0][25][ 4]=  -0.322732;EtaROImax[0][25][ 4]=  -0.276301;PhiROImin[0][25][ 4]=   -1.26443;PhiROImax[0][25][ 4]=	-1.18087;
     EtaROImin[0][25][ 5]=  -0.273184;EtaROImax[0][25][ 5]=  -0.200823;PhiROImin[0][25][ 5]=   -1.26443;PhiROImax[0][25][ 5]=	-1.18087;
     EtaROImin[0][25][ 6]=  -0.322732;EtaROImax[0][25][ 6]=  -0.276301;PhiROImin[0][25][ 6]=   -1.35034;PhiROImax[0][25][ 6]=	-1.26804;
     EtaROImin[0][25][ 7]=  -0.273184;EtaROImax[0][25][ 7]=  -0.200823;PhiROImin[0][25][ 7]=   -1.35034;PhiROImax[0][25][ 7]=	-1.26804;
     EtaROImin[0][25][ 8]=  -0.516583;EtaROImax[0][25][ 8]=  -0.440504;PhiROImin[0][25][ 8]=   -1.25967;PhiROImax[0][25][ 8]=	-1.18023;
     EtaROImin[0][25][ 9]=  -0.439522;EtaROImax[0][25][ 9]=  -0.361231;PhiROImin[0][25][ 9]=   -1.25967;PhiROImax[0][25][ 9]=	-1.18023;
     EtaROImin[0][25][10]=  -0.516583;EtaROImax[0][25][10]=  -0.440504;PhiROImin[0][25][10]=   -1.34059;PhiROImax[0][25][10]=	-1.26222;
     EtaROImin[0][25][11]=  -0.439522;EtaROImax[0][25][11]=  -0.361231;PhiROImin[0][25][11]=   -1.34059;PhiROImax[0][25][11]=	-1.26222;
     EtaROImin[0][25][12]=  -0.659332;EtaROImax[0][25][12]=  -0.566067;PhiROImin[0][25][12]=   -1.26484;PhiROImax[0][25][12]=	-1.18089;
     EtaROImin[0][25][13]=  -0.569141;EtaROImax[0][25][13]=  -0.470878;PhiROImin[0][25][13]=   -1.26484;PhiROImax[0][25][13]=	-1.18089;
     EtaROImin[0][25][14]=  -0.659332;EtaROImax[0][25][14]=  -0.566067;PhiROImin[0][25][14]=   -1.35115;PhiROImax[0][25][14]=	-1.26847;
     EtaROImin[0][25][15]=  -0.569141;EtaROImax[0][25][15]=  -0.470878;PhiROImin[0][25][15]=   -1.35115;PhiROImax[0][25][15]=	-1.26847;
     EtaROImin[0][25][16]=  -0.724075;EtaROImax[0][25][16]=  -0.673648;PhiROImin[0][25][16]=   -1.25967;PhiROImax[0][25][16]=	-1.18023;
     EtaROImin[0][25][17]=  -0.672035;EtaROImax[0][25][17]=  -0.620212;PhiROImin[0][25][17]=   -1.25967;PhiROImax[0][25][17]=	-1.18023;
     EtaROImin[0][25][18]=  -0.724075;EtaROImax[0][25][18]=  -0.673648;PhiROImin[0][25][18]=   -1.34059;PhiROImax[0][25][18]=	-1.26222;
     EtaROImin[0][25][19]=  -0.672035;EtaROImax[0][25][19]=  -0.620212;PhiROImin[0][25][19]=   -1.34059;PhiROImax[0][25][19]=	-1.26222;
     EtaROImin[0][25][20]=  -0.893028;EtaROImax[0][25][20]=  -0.833797;PhiROImin[0][25][20]=   -1.25997;PhiROImax[0][25][20]=	-1.18024;
     EtaROImin[0][25][21]=  -0.831846;EtaROImax[0][25][21]=  -0.744499;PhiROImin[0][25][21]=   -1.25997;PhiROImax[0][25][21]=	-1.18024;
     EtaROImin[0][25][22]=  -0.893028;EtaROImax[0][25][22]=  -0.833797;PhiROImin[0][25][22]=   -1.34117;PhiROImax[0][25][22]=	-1.26253;
     EtaROImin[0][25][23]=  -0.831846;EtaROImax[0][25][23]=  -0.744499;PhiROImin[0][25][23]=   -1.34117;PhiROImax[0][25][23]=	-1.26253;
     EtaROImin[0][25][24]=  -0.905303;EtaROImax[0][25][24]=  -0.846464;PhiROImin[0][25][24]=   -1.26484;PhiROImax[0][25][24]=	-1.18089;
     EtaROImin[0][25][25]=  -0.850339;EtaROImax[0][25][25]=  -0.788882;PhiROImin[0][25][25]=   -1.26484;PhiROImax[0][25][25]=	-1.18089;
     EtaROImin[0][25][26]=  -0.905303;EtaROImax[0][25][26]=  -0.846464;PhiROImin[0][25][26]=   -1.35115;PhiROImax[0][25][26]=	-1.26847;
     EtaROImin[0][25][27]=  -0.850339;EtaROImax[0][25][27]=  -0.788882;PhiROImin[0][25][27]=   -1.35115;PhiROImax[0][25][27]=	-1.26847;
     EtaROImin[0][25][28]=  -0.984405;EtaROImax[0][25][28]=  -0.957812;PhiROImin[0][25][28]=   -1.20642;PhiROImax[0][25][28]=	-1.15713;
     EtaROImin[0][25][29]=   -0.95602;EtaROImax[0][25][29]=  -0.914143;PhiROImin[0][25][29]=   -1.20642;PhiROImax[0][25][29]=	-1.15713;
     EtaROImin[0][25][30]=  -0.984405;EtaROImax[0][25][30]=  -0.957812;PhiROImin[0][25][30]=   -1.29487;PhiROImax[0][25][30]=	 -1.2092;
     EtaROImin[0][25][31]=   -0.95602;EtaROImax[0][25][31]=  -0.914143;PhiROImin[0][25][31]=   -1.29487;PhiROImax[0][25][31]=	 -1.2092;
     EtaROImin[0][26][ 0]=  -0.126072;EtaROImax[0][26][ 0]= -0.0775634;PhiROImin[0][26][ 0]=   -1.17531;PhiROImax[0][26][ 0]=	-1.09135;
     EtaROImin[0][26][ 1]=  -0.203032;EtaROImax[0][26][ 1]=  -0.129296;PhiROImin[0][26][ 1]=   -1.17531;PhiROImax[0][26][ 1]=	-1.09135;
     EtaROImin[0][26][ 2]=  -0.126072;EtaROImax[0][26][ 2]= -0.0775634;PhiROImin[0][26][ 2]=   -1.08772;PhiROImax[0][26][ 2]=	-1.00505;
     EtaROImin[0][26][ 3]=  -0.203032;EtaROImax[0][26][ 3]=  -0.129296;PhiROImin[0][26][ 3]=   -1.08772;PhiROImax[0][26][ 3]=	-1.00505;
     EtaROImin[0][26][ 4]=  -0.248155;EtaROImax[0][26][ 4]=  -0.200823;PhiROImin[0][26][ 4]=   -1.17532;PhiROImax[0][26][ 4]=	-1.09176;
     EtaROImin[0][26][ 5]=  -0.322732;EtaROImax[0][26][ 5]=  -0.251292;PhiROImin[0][26][ 5]=   -1.17532;PhiROImax[0][26][ 5]=	-1.09176;
     EtaROImin[0][26][ 6]=  -0.248155;EtaROImax[0][26][ 6]=  -0.200823;PhiROImin[0][26][ 6]=   -1.08815;PhiROImax[0][26][ 6]=	-1.00586;
     EtaROImin[0][26][ 7]=  -0.322732;EtaROImax[0][26][ 7]=  -0.251292;PhiROImin[0][26][ 7]=   -1.08815;PhiROImax[0][26][ 7]=	-1.00586;
     EtaROImin[0][26][ 8]=  -0.439522;EtaROImax[0][26][ 8]=  -0.361231;PhiROImin[0][26][ 8]=   -1.17596;PhiROImax[0][26][ 8]=	-1.09652;
     EtaROImin[0][26][ 9]=  -0.516583;EtaROImax[0][26][ 9]=  -0.440504;PhiROImin[0][26][ 9]=   -1.17596;PhiROImax[0][26][ 9]=	-1.09652;
     EtaROImin[0][26][10]=  -0.439522;EtaROImax[0][26][10]=  -0.361231;PhiROImin[0][26][10]=   -1.09397;PhiROImax[0][26][10]=	 -1.0156;
     EtaROImin[0][26][11]=  -0.516583;EtaROImax[0][26][11]=  -0.440504;PhiROImin[0][26][11]=   -1.09397;PhiROImax[0][26][11]=	 -1.0156;
     EtaROImin[0][26][12]=  -0.569141;EtaROImax[0][26][12]=  -0.470878;PhiROImin[0][26][12]=   -1.17531;PhiROImax[0][26][12]=	-1.09135;
     EtaROImin[0][26][13]=  -0.659332;EtaROImax[0][26][13]=  -0.566067;PhiROImin[0][26][13]=   -1.17531;PhiROImax[0][26][13]=	-1.09135;
     EtaROImin[0][26][14]=  -0.569141;EtaROImax[0][26][14]=  -0.470878;PhiROImin[0][26][14]=   -1.08772;PhiROImax[0][26][14]=	-1.00505;
     EtaROImin[0][26][15]=  -0.659332;EtaROImax[0][26][15]=  -0.566067;PhiROImin[0][26][15]=   -1.08772;PhiROImax[0][26][15]=	-1.00505;
     EtaROImin[0][26][16]=  -0.672035;EtaROImax[0][26][16]=  -0.620212;PhiROImin[0][26][16]=   -1.17596;PhiROImax[0][26][16]=	-1.09652;
     EtaROImin[0][26][17]=  -0.724075;EtaROImax[0][26][17]=  -0.673648;PhiROImin[0][26][17]=   -1.17596;PhiROImax[0][26][17]=	-1.09652;
     EtaROImin[0][26][18]=  -0.672035;EtaROImax[0][26][18]=  -0.620212;PhiROImin[0][26][18]=   -1.09397;PhiROImax[0][26][18]=	 -1.0156;
     EtaROImin[0][26][19]=  -0.724075;EtaROImax[0][26][19]=  -0.673648;PhiROImin[0][26][19]=   -1.09397;PhiROImax[0][26][19]=	 -1.0156;
     EtaROImin[0][26][20]=  -0.831846;EtaROImax[0][26][20]=  -0.744499;PhiROImin[0][26][20]=   -1.17595;PhiROImax[0][26][20]=	-1.09623;
     EtaROImin[0][26][21]=  -0.893028;EtaROImax[0][26][21]=  -0.833797;PhiROImin[0][26][21]=   -1.17595;PhiROImax[0][26][21]=	-1.09623;
     EtaROImin[0][26][22]=  -0.831846;EtaROImax[0][26][22]=  -0.744499;PhiROImin[0][26][22]=   -1.09367;PhiROImax[0][26][22]=	-1.01502;
     EtaROImin[0][26][23]=  -0.893028;EtaROImax[0][26][23]=  -0.833797;PhiROImin[0][26][23]=   -1.09367;PhiROImax[0][26][23]=	-1.01502;
     EtaROImin[0][26][24]=  -0.850339;EtaROImax[0][26][24]=  -0.788882;PhiROImin[0][26][24]=   -1.17531;PhiROImax[0][26][24]=	-1.09135;
     EtaROImin[0][26][25]=  -0.905303;EtaROImax[0][26][25]=  -0.846464;PhiROImin[0][26][25]=   -1.17531;PhiROImax[0][26][25]=	-1.09135;
     EtaROImin[0][26][26]=  -0.850339;EtaROImax[0][26][26]=  -0.788882;PhiROImin[0][26][26]=   -1.08772;PhiROImax[0][26][26]=	-1.00505;
     EtaROImin[0][26][27]=  -0.905303;EtaROImax[0][26][27]=  -0.846464;PhiROImin[0][26][27]=   -1.08772;PhiROImax[0][26][27]=	-1.00505;
     EtaROImin[0][26][28]=  -0.939363;EtaROImax[0][26][28]=  -0.911945;PhiROImin[0][26][28]=   -1.14955;PhiROImax[0][26][28]=	-1.06384;
     EtaROImin[0][26][29]=   -0.98211;EtaROImax[0][26][29]=  -0.941171;PhiROImin[0][26][29]=   -1.14955;PhiROImax[0][26][29]=	-1.06384;
     EtaROImin[0][26][30]=  -0.939363;EtaROImax[0][26][30]=  -0.911945;PhiROImin[0][26][30]=   -1.15713;PhiROImax[0][26][30]=	 -1.0611;
     EtaROImin[0][26][31]=   -0.98211;EtaROImax[0][26][31]=  -0.941171;PhiROImin[0][26][31]=   -1.15713;PhiROImax[0][26][31]=	 -1.0611;
     EtaROImin[0][27][ 0]=  -0.238294;EtaROImax[0][27][ 0]=  -0.180742;PhiROImin[0][27][ 0]=   -0.89861;PhiROImax[0][27][ 0]=  -0.788366;
     EtaROImin[0][27][ 1]=  -0.176882;EtaROImax[0][27][ 1]= -0.0874905;PhiROImin[0][27][ 1]=   -0.89861;PhiROImax[0][27][ 1]=  -0.788366;
     EtaROImin[0][27][ 2]=  -0.238294;EtaROImax[0][27][ 2]=  -0.180742;PhiROImin[0][27][ 2]=   -1.00957;PhiROImax[0][27][ 2]=  -0.902135;
     EtaROImin[0][27][ 3]=  -0.176882;EtaROImax[0][27][ 3]= -0.0874905;PhiROImin[0][27][ 3]=   -1.00957;PhiROImax[0][27][ 3]=  -0.902135;
     EtaROImin[0][27][ 4]=  -0.452099;EtaROImax[0][27][ 4]=  -0.350831;PhiROImin[0][27][ 4]=  -0.898011;PhiROImax[0][27][ 4]=	-0.78835;
     EtaROImin[0][27][ 5]=   -0.35049;EtaROImax[0][27][ 5]=  -0.246207;PhiROImin[0][27][ 5]=  -0.898011;PhiROImax[0][27][ 5]=	-0.78835;
     EtaROImin[0][27][ 6]=  -0.452099;EtaROImax[0][27][ 6]=  -0.350831;PhiROImin[0][27][ 6]=   -1.00841;PhiROImax[0][27][ 6]=  -0.901517;
     EtaROImin[0][27][ 7]=   -0.35049;EtaROImax[0][27][ 7]=  -0.246207;PhiROImin[0][27][ 7]=   -1.00841;PhiROImax[0][27][ 7]=  -0.901517;
     EtaROImin[0][27][ 8]=  -0.648101;EtaROImax[0][27][ 8]=  -0.555789;PhiROImin[0][27][ 8]=  -0.898011;PhiROImax[0][27][ 8]=	-0.78835;
     EtaROImin[0][27][ 9]=  -0.554444;EtaROImax[0][27][ 9]=  -0.458091;PhiROImin[0][27][ 9]=  -0.898011;PhiROImax[0][27][ 9]=	-0.78835;
     EtaROImin[0][27][10]=  -0.648101;EtaROImax[0][27][10]=  -0.555789;PhiROImin[0][27][10]=   -1.00841;PhiROImax[0][27][10]=  -0.901517;
     EtaROImin[0][27][11]=  -0.554444;EtaROImax[0][27][11]=  -0.458091;PhiROImin[0][27][11]=   -1.00841;PhiROImax[0][27][11]=  -0.901517;
     EtaROImin[0][27][12]=  -0.776449;EtaROImax[0][27][12]=  -0.731124;PhiROImin[0][27][12]=   -0.89861;PhiROImax[0][27][12]=  -0.788366;
     EtaROImin[0][27][13]=  -0.728056;EtaROImax[0][27][13]=  -0.655857;PhiROImin[0][27][13]=   -0.89861;PhiROImax[0][27][13]=  -0.788366;
     EtaROImin[0][27][14]=  -0.776449;EtaROImax[0][27][14]=  -0.731124;PhiROImin[0][27][14]=   -1.00957;PhiROImax[0][27][14]=  -0.902135;
     EtaROImin[0][27][15]=  -0.728056;EtaROImax[0][27][15]=  -0.655857;PhiROImin[0][27][15]=   -1.00957;PhiROImax[0][27][15]=  -0.902135;
     EtaROImin[0][27][16]=  -0.895163;EtaROImax[0][27][16]=  -0.853359;PhiROImin[0][27][16]=   -0.89861;PhiROImax[0][27][16]=  -0.788366;
     EtaROImin[0][27][17]=  -0.850528;EtaROImax[0][27][17]=  -0.783867;PhiROImin[0][27][17]=   -0.89861;PhiROImax[0][27][17]=  -0.788366;
     EtaROImin[0][27][18]=  -0.895163;EtaROImax[0][27][18]=  -0.853359;PhiROImin[0][27][18]=   -1.00957;PhiROImax[0][27][18]=  -0.902135;
     EtaROImin[0][27][19]=  -0.850528;EtaROImax[0][27][19]=  -0.783867;PhiROImin[0][27][19]=   -1.00957;PhiROImax[0][27][19]=  -0.902135;
     EtaROImin[0][27][20]=   -1.02514;EtaROImax[0][27][20]=  -0.964674;PhiROImin[0][27][20]=  -0.898011;PhiROImax[0][27][20]=	-0.78835;
     EtaROImin[0][27][21]=  -0.961344;EtaROImax[0][27][21]=  -0.898201;PhiROImin[0][27][21]=  -0.898011;PhiROImax[0][27][21]=	-0.78835;
     EtaROImin[0][27][22]=   -1.02514;EtaROImax[0][27][22]=  -0.964674;PhiROImin[0][27][22]=   -1.00841;PhiROImax[0][27][22]=  -0.901517;
     EtaROImin[0][27][23]=  -0.961344;EtaROImax[0][27][23]=  -0.898201;PhiROImin[0][27][23]=   -1.00841;PhiROImax[0][27][23]=  -0.901517;
     EtaROImin[0][27][24]=	    0;EtaROImax[0][27][24]=	     0;PhiROImin[0][27][24]=	      0;PhiROImax[0][27][24]=	       0;
     EtaROImin[0][27][25]=   -1.06547;EtaROImax[0][27][25]=   -1.03003;PhiROImin[0][27][25]=  -0.898827;PhiROImax[0][27][25]=  -0.788467;
     EtaROImin[0][27][26]=	    0;EtaROImax[0][27][26]=	     0;PhiROImin[0][27][26]=	      0;PhiROImax[0][27][26]=	       0;
     EtaROImin[0][27][27]=   -1.06547;EtaROImax[0][27][27]=   -1.03003;PhiROImin[0][27][27]=  -0.982541;PhiROImax[0][27][27]=  -0.902356;
     EtaROImin[0][27][28]=	    0;EtaROImax[0][27][28]=	     0;PhiROImin[0][27][28]=	      0;PhiROImax[0][27][28]=	       0;
     EtaROImin[0][27][29]=	    0;EtaROImax[0][27][29]=	     0;PhiROImin[0][27][29]=	      0;PhiROImax[0][27][29]=	       0;
     EtaROImin[0][27][30]=	    0;EtaROImax[0][27][30]=	     0;PhiROImin[0][27][30]=	      0;PhiROImax[0][27][30]=	       0;
     EtaROImin[0][27][31]=	    0;EtaROImax[0][27][31]=	     0;PhiROImin[0][27][31]=	      0;PhiROImax[0][27][31]=	       0;
     EtaROImin[0][28][ 0]=  -0.145912;EtaROImax[0][28][ 0]= -0.0874905;PhiROImin[0][28][ 0]=   -0.78243;PhiROImax[0][28][ 0]=  -0.672186;
     EtaROImin[0][28][ 1]=  -0.238294;EtaROImax[0][28][ 1]=  -0.149792;PhiROImin[0][28][ 1]=   -0.78243;PhiROImax[0][28][ 1]=  -0.672186;
     EtaROImin[0][28][ 2]=  -0.145912;EtaROImax[0][28][ 2]= -0.0874905;PhiROImin[0][28][ 2]=  -0.668661;PhiROImax[0][28][ 2]=  -0.561229;
     EtaROImin[0][28][ 3]=  -0.238294;EtaROImax[0][28][ 3]=  -0.149792;PhiROImin[0][28][ 3]=  -0.668661;PhiROImax[0][28][ 3]=  -0.561229;
     EtaROImin[0][28][ 4]=   -0.35049;EtaROImax[0][28][ 4]=  -0.246207;PhiROImin[0][28][ 4]=  -0.782446;PhiROImax[0][28][ 4]=  -0.672786;
     EtaROImin[0][28][ 5]=  -0.452099;EtaROImax[0][28][ 5]=  -0.350831;PhiROImin[0][28][ 5]=  -0.782446;PhiROImax[0][28][ 5]=  -0.672786;
     EtaROImin[0][28][ 6]=   -0.35049;EtaROImax[0][28][ 6]=  -0.246207;PhiROImin[0][28][ 6]=  -0.669279;PhiROImax[0][28][ 6]=  -0.562387;
     EtaROImin[0][28][ 7]=  -0.452099;EtaROImax[0][28][ 7]=  -0.350831;PhiROImin[0][28][ 7]=  -0.669279;PhiROImax[0][28][ 7]=  -0.562387;
     EtaROImin[0][28][ 8]=  -0.554444;EtaROImax[0][28][ 8]=  -0.458091;PhiROImin[0][28][ 8]=  -0.782446;PhiROImax[0][28][ 8]=  -0.672786;
     EtaROImin[0][28][ 9]=  -0.648101;EtaROImax[0][28][ 9]=  -0.555789;PhiROImin[0][28][ 9]=  -0.782446;PhiROImax[0][28][ 9]=  -0.672786;
     EtaROImin[0][28][10]=  -0.554444;EtaROImax[0][28][10]=  -0.458091;PhiROImin[0][28][10]=  -0.669279;PhiROImax[0][28][10]=  -0.562387;
     EtaROImin[0][28][11]=  -0.648101;EtaROImax[0][28][11]=  -0.555789;PhiROImin[0][28][11]=  -0.669279;PhiROImax[0][28][11]=  -0.562387;
     EtaROImin[0][28][12]=  -0.703298;EtaROImax[0][28][12]=  -0.655857;PhiROImin[0][28][12]=   -0.78243;PhiROImax[0][28][12]=  -0.672186;
     EtaROImin[0][28][13]=  -0.776449;EtaROImax[0][28][13]=  -0.706413;PhiROImin[0][28][13]=   -0.78243;PhiROImax[0][28][13]=  -0.672186;
     EtaROImin[0][28][14]=  -0.703298;EtaROImax[0][28][14]=  -0.655857;PhiROImin[0][28][14]=  -0.668661;PhiROImax[0][28][14]=  -0.561229;
     EtaROImin[0][28][15]=  -0.776449;EtaROImax[0][28][15]=  -0.706413;PhiROImin[0][28][15]=  -0.668661;PhiROImax[0][28][15]=  -0.561229;
     EtaROImin[0][28][16]=   -0.82768;EtaROImax[0][28][16]=  -0.783867;PhiROImin[0][28][16]=   -0.78243;PhiROImax[0][28][16]=  -0.672186;
     EtaROImin[0][28][17]=  -0.895163;EtaROImax[0][28][17]=  -0.830556;PhiROImin[0][28][17]=   -0.78243;PhiROImax[0][28][17]=  -0.672186;
     EtaROImin[0][28][18]=   -0.82768;EtaROImax[0][28][18]=  -0.783867;PhiROImin[0][28][18]=  -0.668661;PhiROImax[0][28][18]=  -0.561229;
     EtaROImin[0][28][19]=  -0.895163;EtaROImax[0][28][19]=  -0.830556;PhiROImin[0][28][19]=  -0.668661;PhiROImax[0][28][19]=  -0.561229;
     EtaROImin[0][28][20]=  -0.961344;EtaROImax[0][28][20]=  -0.898201;PhiROImin[0][28][20]=  -0.782446;PhiROImax[0][28][20]=  -0.672786;
     EtaROImin[0][28][21]=   -1.02514;EtaROImax[0][28][21]=  -0.964674;PhiROImin[0][28][21]=  -0.782446;PhiROImax[0][28][21]=  -0.672786;
     EtaROImin[0][28][22]=  -0.961344;EtaROImax[0][28][22]=  -0.898201;PhiROImin[0][28][22]=  -0.669279;PhiROImax[0][28][22]=  -0.562387;
     EtaROImin[0][28][23]=   -1.02514;EtaROImax[0][28][23]=  -0.964674;PhiROImin[0][28][23]=  -0.669279;PhiROImax[0][28][23]=  -0.562387;
     EtaROImin[0][28][24]=   -1.06547;EtaROImax[0][28][24]=   -1.03003;PhiROImin[0][28][24]=  -0.782329;PhiROImax[0][28][24]=  -0.671969;
     EtaROImin[0][28][25]=	    0;EtaROImax[0][28][25]=	     0;PhiROImin[0][28][25]=	      0;PhiROImax[0][28][25]=	       0;
     EtaROImin[0][28][26]=   -1.06547;EtaROImax[0][28][26]=   -1.03003;PhiROImin[0][28][26]=   -0.66844;PhiROImax[0][28][26]=  -0.588255;
     EtaROImin[0][28][27]=	    0;EtaROImax[0][28][27]=	     0;PhiROImin[0][28][27]=	      0;PhiROImax[0][28][27]=	       0;
     EtaROImin[0][28][28]=	    0;EtaROImax[0][28][28]=	     0;PhiROImin[0][28][28]=	      0;PhiROImax[0][28][28]=	       0;
     EtaROImin[0][28][29]=	    0;EtaROImax[0][28][29]=	     0;PhiROImin[0][28][29]=	      0;PhiROImax[0][28][29]=	       0;
     EtaROImin[0][28][30]=	    0;EtaROImax[0][28][30]=	     0;PhiROImin[0][28][30]=	      0;PhiROImax[0][28][30]=	       0;
     EtaROImin[0][28][31]=	    0;EtaROImax[0][28][31]=	     0;PhiROImin[0][28][31]=	      0;PhiROImax[0][28][31]=	       0;
     EtaROImin[0][29][ 0]=  -0.213185;EtaROImax[0][29][ 0]=  -0.116816;PhiROImin[0][29][ 0]=  -0.479445;PhiROImax[0][29][ 0]=  -0.395489;
     EtaROImin[0][29][ 1]=  -0.118734;EtaROImax[0][29][ 1]= -0.0208251;PhiROImin[0][29][ 1]=  -0.479445;PhiROImax[0][29][ 1]=  -0.395489;
     EtaROImin[0][29][ 2]=  -0.213185;EtaROImax[0][29][ 2]=  -0.116816;PhiROImin[0][29][ 2]=  -0.565749;PhiROImax[0][29][ 2]=  -0.483076;
     EtaROImin[0][29][ 3]=  -0.118734;EtaROImax[0][29][ 3]= -0.0208251;PhiROImin[0][29][ 3]=  -0.565749;PhiROImax[0][29][ 3]=  -0.483076;
     EtaROImin[0][29][ 4]=  -0.416721;EtaROImax[0][29][ 4]=   -0.30075;PhiROImin[0][29][ 4]=  -0.479445;PhiROImax[0][29][ 4]=  -0.395489;
     EtaROImin[0][29][ 5]=  -0.302928;EtaROImax[0][29][ 5]=  -0.222449;PhiROImin[0][29][ 5]=  -0.479445;PhiROImax[0][29][ 5]=  -0.395489;
     EtaROImin[0][29][ 6]=  -0.416721;EtaROImax[0][29][ 6]=   -0.30075;PhiROImin[0][29][ 6]=  -0.565749;PhiROImax[0][29][ 6]=  -0.483076;
     EtaROImin[0][29][ 7]=  -0.302928;EtaROImax[0][29][ 7]=  -0.222449;PhiROImin[0][29][ 7]=  -0.565749;PhiROImax[0][29][ 7]=  -0.483076;
     EtaROImin[0][29][ 8]=  -0.573871;EtaROImax[0][29][ 8]=  -0.501681;PhiROImin[0][29][ 8]=  -0.479445;PhiROImax[0][29][ 8]=  -0.395489;
     EtaROImin[0][29][ 9]=  -0.504617;EtaROImax[0][29][ 9]=   -0.42967;PhiROImin[0][29][ 9]=  -0.479445;PhiROImax[0][29][ 9]=  -0.395489;
     EtaROImin[0][29][10]=  -0.573871;EtaROImax[0][29][10]=  -0.501681;PhiROImin[0][29][10]=  -0.565749;PhiROImax[0][29][10]=  -0.483076;
     EtaROImin[0][29][11]=  -0.504617;EtaROImax[0][29][11]=   -0.42967;PhiROImin[0][29][11]=  -0.565749;PhiROImax[0][29][11]=  -0.483076;
     EtaROImin[0][29][12]=  -0.741516;EtaROImax[0][29][12]=  -0.649933;PhiROImin[0][29][12]=  -0.479445;PhiROImax[0][29][12]=  -0.395489;
     EtaROImin[0][29][13]=  -0.653329;EtaROImax[0][29][13]=  -0.583785;PhiROImin[0][29][13]=  -0.479445;PhiROImax[0][29][13]=  -0.395489;
     EtaROImin[0][29][14]=  -0.741516;EtaROImax[0][29][14]=  -0.649933;PhiROImin[0][29][14]=  -0.565749;PhiROImax[0][29][14]=  -0.483076;
     EtaROImin[0][29][15]=  -0.653329;EtaROImax[0][29][15]=  -0.583785;PhiROImin[0][29][15]=  -0.565749;PhiROImax[0][29][15]=  -0.483076;
     EtaROImin[0][29][16]=	    0;EtaROImax[0][29][16]=	     0;PhiROImin[0][29][16]=	      0;PhiROImax[0][29][16]=	       0;
     EtaROImin[0][29][17]=  -0.837822;EtaROImax[0][29][17]=  -0.756521;PhiROImin[0][29][17]=  -0.479445;PhiROImax[0][29][17]=  -0.395489;
     EtaROImin[0][29][18]=	    0;EtaROImax[0][29][18]=	     0;PhiROImin[0][29][18]=	      0;PhiROImax[0][29][18]=	       0;
     EtaROImin[0][29][19]=  -0.837822;EtaROImax[0][29][19]=  -0.756521;PhiROImin[0][29][19]=  -0.565749;PhiROImax[0][29][19]=  -0.483076;
     EtaROImin[0][29][20]=  -0.956037;EtaROImax[0][29][20]=  -0.899344;PhiROImin[0][29][20]=  -0.479445;PhiROImax[0][29][20]=  -0.395489;
     EtaROImin[0][29][21]=  -0.903324;EtaROImax[0][29][21]=  -0.844116;PhiROImin[0][29][21]=  -0.479445;PhiROImax[0][29][21]=  -0.395489;
     EtaROImin[0][29][22]=  -0.956037;EtaROImax[0][29][22]=  -0.899344;PhiROImin[0][29][22]=  -0.565749;PhiROImax[0][29][22]=  -0.483076;
     EtaROImin[0][29][23]=  -0.903324;EtaROImax[0][29][23]=  -0.844116;PhiROImin[0][29][23]=  -0.565749;PhiROImax[0][29][23]=  -0.483076;
     EtaROImin[0][29][24]=	    0;EtaROImax[0][29][24]=	     0;PhiROImin[0][29][24]=	      0;PhiROImax[0][29][24]=	       0;
     EtaROImin[0][29][25]=	    0;EtaROImax[0][29][25]=	     0;PhiROImin[0][29][25]=	      0;PhiROImax[0][29][25]=	       0;
     EtaROImin[0][29][26]=	    0;EtaROImax[0][29][26]=	     0;PhiROImin[0][29][26]=	      0;PhiROImax[0][29][26]=	       0;
     EtaROImin[0][29][27]=	    0;EtaROImax[0][29][27]=	     0;PhiROImin[0][29][27]=	      0;PhiROImax[0][29][27]=	       0;
     EtaROImin[0][29][28]=	    0;EtaROImax[0][29][28]=	     0;PhiROImin[0][29][28]=	      0;PhiROImax[0][29][28]=	       0;
     EtaROImin[0][29][29]=	    0;EtaROImax[0][29][29]=	     0;PhiROImin[0][29][29]=	      0;PhiROImax[0][29][29]=	       0;
     EtaROImin[0][29][30]=	    0;EtaROImax[0][29][30]=	     0;PhiROImin[0][29][30]=	      0;PhiROImax[0][29][30]=	       0;
     EtaROImin[0][29][31]=	    0;EtaROImax[0][29][31]=	     0;PhiROImin[0][29][31]=	      0;PhiROImax[0][29][31]=	       0;
     EtaROImin[0][30][ 0]=  -0.118734;EtaROImax[0][30][ 0]= -0.0208251;PhiROImin[0][30][ 0]=  -0.389909;PhiROImax[0][30][ 0]=  -0.305953;
     EtaROImin[0][30][ 1]=  -0.213185;EtaROImax[0][30][ 1]=  -0.116816;PhiROImin[0][30][ 1]=  -0.389909;PhiROImax[0][30][ 1]=  -0.305953;
     EtaROImin[0][30][ 2]=  -0.118734;EtaROImax[0][30][ 2]= -0.0208251;PhiROImin[0][30][ 2]=  -0.302322;PhiROImax[0][30][ 2]=  -0.219649;
     EtaROImin[0][30][ 3]=  -0.213185;EtaROImax[0][30][ 3]=  -0.116816;PhiROImin[0][30][ 3]=  -0.302322;PhiROImax[0][30][ 3]=  -0.219649;
     EtaROImin[0][30][ 4]=  -0.302928;EtaROImax[0][30][ 4]=  -0.222449;PhiROImin[0][30][ 4]=  -0.389909;PhiROImax[0][30][ 4]=  -0.305953;
     EtaROImin[0][30][ 5]=  -0.416721;EtaROImax[0][30][ 5]=   -0.30075;PhiROImin[0][30][ 5]=  -0.389909;PhiROImax[0][30][ 5]=  -0.305953;
     EtaROImin[0][30][ 6]=  -0.302928;EtaROImax[0][30][ 6]=  -0.222449;PhiROImin[0][30][ 6]=  -0.302322;PhiROImax[0][30][ 6]=  -0.219649;
     EtaROImin[0][30][ 7]=  -0.416721;EtaROImax[0][30][ 7]=   -0.30075;PhiROImin[0][30][ 7]=  -0.302322;PhiROImax[0][30][ 7]=  -0.219649;
     EtaROImin[0][30][ 8]=  -0.504617;EtaROImax[0][30][ 8]=   -0.42967;PhiROImin[0][30][ 8]=  -0.389909;PhiROImax[0][30][ 8]=  -0.305953;
     EtaROImin[0][30][ 9]=  -0.573871;EtaROImax[0][30][ 9]=  -0.501681;PhiROImin[0][30][ 9]=  -0.389909;PhiROImax[0][30][ 9]=  -0.305953;
     EtaROImin[0][30][10]=  -0.504617;EtaROImax[0][30][10]=   -0.42967;PhiROImin[0][30][10]=  -0.302322;PhiROImax[0][30][10]=  -0.219649;
     EtaROImin[0][30][11]=  -0.573871;EtaROImax[0][30][11]=  -0.501681;PhiROImin[0][30][11]=  -0.302322;PhiROImax[0][30][11]=  -0.219649;
     EtaROImin[0][30][12]=  -0.653329;EtaROImax[0][30][12]=  -0.583785;PhiROImin[0][30][12]=  -0.389909;PhiROImax[0][30][12]=  -0.305953;
     EtaROImin[0][30][13]=  -0.741516;EtaROImax[0][30][13]=  -0.649933;PhiROImin[0][30][13]=  -0.389909;PhiROImax[0][30][13]=  -0.305953;
     EtaROImin[0][30][14]=  -0.653329;EtaROImax[0][30][14]=  -0.583785;PhiROImin[0][30][14]=  -0.302322;PhiROImax[0][30][14]=  -0.219649;
     EtaROImin[0][30][15]=  -0.741516;EtaROImax[0][30][15]=  -0.649933;PhiROImin[0][30][15]=  -0.302322;PhiROImax[0][30][15]=  -0.219649;
     EtaROImin[0][30][16]=  -0.837822;EtaROImax[0][30][16]=  -0.756521;PhiROImin[0][30][16]=  -0.389909;PhiROImax[0][30][16]=  -0.305953;
     EtaROImin[0][30][17]=	    0;EtaROImax[0][30][17]=	     0;PhiROImin[0][30][17]=	      0;PhiROImax[0][30][17]=	       0;
     EtaROImin[0][30][18]=  -0.837822;EtaROImax[0][30][18]=  -0.756521;PhiROImin[0][30][18]=  -0.302322;PhiROImax[0][30][18]=  -0.219649;
     EtaROImin[0][30][19]=	    0;EtaROImax[0][30][19]=	     0;PhiROImin[0][30][19]=	      0;PhiROImax[0][30][19]=	       0;
     EtaROImin[0][30][20]=  -0.903324;EtaROImax[0][30][20]=  -0.844116;PhiROImin[0][30][20]=  -0.389909;PhiROImax[0][30][20]=  -0.305953;
     EtaROImin[0][30][21]=  -0.956037;EtaROImax[0][30][21]=  -0.899344;PhiROImin[0][30][21]=  -0.389909;PhiROImax[0][30][21]=  -0.305953;
     EtaROImin[0][30][22]=  -0.903324;EtaROImax[0][30][22]=  -0.844116;PhiROImin[0][30][22]=  -0.302322;PhiROImax[0][30][22]=  -0.219649;
     EtaROImin[0][30][23]=  -0.956037;EtaROImax[0][30][23]=  -0.899344;PhiROImin[0][30][23]=  -0.302322;PhiROImax[0][30][23]=  -0.219649;
     EtaROImin[0][30][24]=	    0;EtaROImax[0][30][24]=	     0;PhiROImin[0][30][24]=	      0;PhiROImax[0][30][24]=	       0;
     EtaROImin[0][30][25]=	    0;EtaROImax[0][30][25]=	     0;PhiROImin[0][30][25]=	      0;PhiROImax[0][30][25]=	       0;
     EtaROImin[0][30][26]=	    0;EtaROImax[0][30][26]=	     0;PhiROImin[0][30][26]=	      0;PhiROImax[0][30][26]=	       0;
     EtaROImin[0][30][27]=	    0;EtaROImax[0][30][27]=	     0;PhiROImin[0][30][27]=	      0;PhiROImax[0][30][27]=	       0;
     EtaROImin[0][30][28]=	    0;EtaROImax[0][30][28]=	     0;PhiROImin[0][30][28]=	      0;PhiROImax[0][30][28]=	       0;
     EtaROImin[0][30][29]=	    0;EtaROImax[0][30][29]=	     0;PhiROImin[0][30][29]=	      0;PhiROImax[0][30][29]=	       0;
     EtaROImin[0][30][30]=	    0;EtaROImax[0][30][30]=	     0;PhiROImin[0][30][30]=	      0;PhiROImax[0][30][30]=	       0;
     EtaROImin[0][30][31]=	    0;EtaROImax[0][30][31]=	     0;PhiROImin[0][30][31]=	      0;PhiROImax[0][30][31]=	       0;
     EtaROImin[0][31][ 0]=	    0;EtaROImax[0][31][ 0]=	     0;PhiROImin[0][31][ 0]=	      0;PhiROImax[0][31][ 0]=	       0;
     EtaROImin[0][31][ 1]=  -0.238757;EtaROImax[0][31][ 1]=   -0.13107;PhiROImin[0][31][ 1]=  -0.113212;PhiROImax[0][31][ 1]=-0.00296767;
     EtaROImin[0][31][ 2]=	    0;EtaROImax[0][31][ 2]=	     0;PhiROImin[0][31][ 2]=	      0;PhiROImax[0][31][ 2]=	       0;
     EtaROImin[0][31][ 3]=  -0.238757;EtaROImax[0][31][ 3]=   -0.13107;PhiROImin[0][31][ 3]=  -0.224169;PhiROImax[0][31][ 3]=  -0.116737;
     EtaROImin[0][31][ 4]=  -0.452099;EtaROImax[0][31][ 4]=  -0.350831;PhiROImin[0][31][ 4]=  -0.112612;PhiROImax[0][31][ 4]=-0.00295181;
     EtaROImin[0][31][ 5]=   -0.35049;EtaROImax[0][31][ 5]=  -0.246207;PhiROImin[0][31][ 5]=  -0.112612;PhiROImax[0][31][ 5]=-0.00295181;
     EtaROImin[0][31][ 6]=  -0.452099;EtaROImax[0][31][ 6]=  -0.350831;PhiROImin[0][31][ 6]=  -0.223011;PhiROImax[0][31][ 6]=  -0.116119;
     EtaROImin[0][31][ 7]=   -0.35049;EtaROImax[0][31][ 7]=  -0.246207;PhiROImin[0][31][ 7]=  -0.223011;PhiROImax[0][31][ 7]=  -0.116119;
     EtaROImin[0][31][ 8]=  -0.648101;EtaROImax[0][31][ 8]=  -0.555789;PhiROImin[0][31][ 8]=  -0.112612;PhiROImax[0][31][ 8]=-0.00295181;
     EtaROImin[0][31][ 9]=  -0.554444;EtaROImax[0][31][ 9]=  -0.458091;PhiROImin[0][31][ 9]=  -0.112612;PhiROImax[0][31][ 9]=-0.00295181;
     EtaROImin[0][31][10]=  -0.648101;EtaROImax[0][31][10]=  -0.555789;PhiROImin[0][31][10]=  -0.223011;PhiROImax[0][31][10]=  -0.116119;
     EtaROImin[0][31][11]=  -0.554444;EtaROImax[0][31][11]=  -0.458091;PhiROImin[0][31][11]=  -0.223011;PhiROImax[0][31][11]=  -0.116119;
     EtaROImin[0][31][12]=  -0.776449;EtaROImax[0][31][12]=  -0.731124;PhiROImin[0][31][12]=  -0.113212;PhiROImax[0][31][12]=-0.00296767;
     EtaROImin[0][31][13]=  -0.728056;EtaROImax[0][31][13]=  -0.655857;PhiROImin[0][31][13]=  -0.113212;PhiROImax[0][31][13]=-0.00296767;
     EtaROImin[0][31][14]=  -0.776449;EtaROImax[0][31][14]=  -0.731124;PhiROImin[0][31][14]=  -0.224169;PhiROImax[0][31][14]=  -0.116737;
     EtaROImin[0][31][15]=  -0.728056;EtaROImax[0][31][15]=  -0.655857;PhiROImin[0][31][15]=  -0.224169;PhiROImax[0][31][15]=  -0.116737;
     EtaROImin[0][31][16]=  -0.895163;EtaROImax[0][31][16]=  -0.853359;PhiROImin[0][31][16]=  -0.113212;PhiROImax[0][31][16]=-0.00296767;
     EtaROImin[0][31][17]=  -0.850528;EtaROImax[0][31][17]=  -0.783867;PhiROImin[0][31][17]=  -0.113212;PhiROImax[0][31][17]=-0.00296767;
     EtaROImin[0][31][18]=  -0.895163;EtaROImax[0][31][18]=  -0.853359;PhiROImin[0][31][18]=  -0.224169;PhiROImax[0][31][18]=  -0.116737;
     EtaROImin[0][31][19]=  -0.850528;EtaROImax[0][31][19]=  -0.783867;PhiROImin[0][31][19]=  -0.224169;PhiROImax[0][31][19]=  -0.116737;
     EtaROImin[0][31][20]=   -1.02514;EtaROImax[0][31][20]=  -0.964674;PhiROImin[0][31][20]=  -0.112612;PhiROImax[0][31][20]=-0.00295181;
     EtaROImin[0][31][21]=  -0.961344;EtaROImax[0][31][21]=  -0.898201;PhiROImin[0][31][21]=  -0.112612;PhiROImax[0][31][21]=-0.00295181;
     EtaROImin[0][31][22]=   -1.02514;EtaROImax[0][31][22]=  -0.964674;PhiROImin[0][31][22]=  -0.223011;PhiROImax[0][31][22]=  -0.116119;
     EtaROImin[0][31][23]=  -0.961344;EtaROImax[0][31][23]=  -0.898201;PhiROImin[0][31][23]=  -0.223011;PhiROImax[0][31][23]=  -0.116119;
     EtaROImin[0][31][24]=	    0;EtaROImax[0][31][24]=	     0;PhiROImin[0][31][24]=	      0;PhiROImax[0][31][24]=	       0;
     EtaROImin[0][31][25]=   -1.06547;EtaROImax[0][31][25]=   -1.03003;PhiROImin[0][31][25]=  -0.113429;PhiROImax[0][31][25]=-0.00306876;
     EtaROImin[0][31][26]=	    0;EtaROImax[0][31][26]=	     0;PhiROImin[0][31][26]=	      0;PhiROImax[0][31][26]=	       0;
     EtaROImin[0][31][27]=   -1.06547;EtaROImax[0][31][27]=   -1.03003;PhiROImin[0][31][27]=  -0.197143;PhiROImax[0][31][27]=  -0.116958;
     EtaROImin[0][31][28]=	    0;EtaROImax[0][31][28]=	     0;PhiROImin[0][31][28]=	      0;PhiROImax[0][31][28]=	       0;
     EtaROImin[0][31][29]=	    0;EtaROImax[0][31][29]=	     0;PhiROImin[0][31][29]=	      0;PhiROImax[0][31][29]=	       0;
     EtaROImin[0][31][30]=	    0;EtaROImax[0][31][30]=	     0;PhiROImin[0][31][30]=	      0;PhiROImax[0][31][30]=	       0;
     EtaROImin[0][31][31]=	    0;EtaROImax[0][31][31]=	     0;PhiROImin[0][31][31]=	      0;PhiROImax[0][31][31]=	       0;
     EtaROImin[1][ 0][ 0]=   0.130922;EtaROImax[1][ 0][ 0]=   0.238612;PhiROImin[1][ 0][ 0]= 0.00295181;PhiROImax[1][ 0][ 0]=	0.112612;
     EtaROImin[1][ 0][ 1]=  0.0231199;EtaROImax[1][ 0][ 1]=   0.131749;PhiROImin[1][ 0][ 1]= 0.00295181;PhiROImax[1][ 0][ 1]=	0.112612;
     EtaROImin[1][ 0][ 2]=   0.130922;EtaROImax[1][ 0][ 2]=   0.238612;PhiROImin[1][ 0][ 2]=   0.116119;PhiROImax[1][ 0][ 2]=	0.223011;
     EtaROImin[1][ 0][ 3]=  0.0231199;EtaROImax[1][ 0][ 3]=   0.131749;PhiROImin[1][ 0][ 3]=   0.116119;PhiROImax[1][ 0][ 3]=	0.223011;
     EtaROImin[1][ 0][ 4]=   0.350831;EtaROImax[1][ 0][ 4]=   0.452099;PhiROImin[1][ 0][ 4]= 0.00295181;PhiROImax[1][ 0][ 4]=	0.112612;
     EtaROImin[1][ 0][ 5]=   0.246207;EtaROImax[1][ 0][ 5]=    0.35049;PhiROImin[1][ 0][ 5]= 0.00295181;PhiROImax[1][ 0][ 5]=	0.112612;
     EtaROImin[1][ 0][ 6]=   0.350831;EtaROImax[1][ 0][ 6]=   0.452099;PhiROImin[1][ 0][ 6]=   0.116119;PhiROImax[1][ 0][ 6]=	0.223011;
     EtaROImin[1][ 0][ 7]=   0.246207;EtaROImax[1][ 0][ 7]=    0.35049;PhiROImin[1][ 0][ 7]=   0.116119;PhiROImax[1][ 0][ 7]=	0.223011;
     EtaROImin[1][ 0][ 8]=   0.555789;EtaROImax[1][ 0][ 8]=   0.648101;PhiROImin[1][ 0][ 8]= 0.00295181;PhiROImax[1][ 0][ 8]=	0.112612;
     EtaROImin[1][ 0][ 9]=   0.458091;EtaROImax[1][ 0][ 9]=   0.554444;PhiROImin[1][ 0][ 9]= 0.00295181;PhiROImax[1][ 0][ 9]=	0.112612;
     EtaROImin[1][ 0][10]=   0.555789;EtaROImax[1][ 0][10]=   0.648101;PhiROImin[1][ 0][10]=   0.116119;PhiROImax[1][ 0][10]=	0.223011;
     EtaROImin[1][ 0][11]=   0.458091;EtaROImax[1][ 0][11]=   0.554444;PhiROImin[1][ 0][11]=   0.116119;PhiROImax[1][ 0][11]=	0.223011;
     EtaROImin[1][ 0][12]=   0.731124;EtaROImax[1][ 0][12]=   0.776449;PhiROImin[1][ 0][12]= 0.00296767;PhiROImax[1][ 0][12]=	0.113212;
     EtaROImin[1][ 0][13]=   0.655857;EtaROImax[1][ 0][13]=   0.728056;PhiROImin[1][ 0][13]= 0.00296767;PhiROImax[1][ 0][13]=	0.113212;
     EtaROImin[1][ 0][14]=   0.731124;EtaROImax[1][ 0][14]=   0.776449;PhiROImin[1][ 0][14]=   0.116737;PhiROImax[1][ 0][14]=	0.224169;
     EtaROImin[1][ 0][15]=   0.655857;EtaROImax[1][ 0][15]=   0.728056;PhiROImin[1][ 0][15]=   0.116737;PhiROImax[1][ 0][15]=	0.224169;
     EtaROImin[1][ 0][16]=   0.853359;EtaROImax[1][ 0][16]=   0.895163;PhiROImin[1][ 0][16]= 0.00296767;PhiROImax[1][ 0][16]=	0.113212;
     EtaROImin[1][ 0][17]=   0.783867;EtaROImax[1][ 0][17]=   0.850528;PhiROImin[1][ 0][17]= 0.00296767;PhiROImax[1][ 0][17]=	0.113212;
     EtaROImin[1][ 0][18]=   0.853359;EtaROImax[1][ 0][18]=   0.895163;PhiROImin[1][ 0][18]=   0.116737;PhiROImax[1][ 0][18]=	0.224169;
     EtaROImin[1][ 0][19]=   0.783867;EtaROImax[1][ 0][19]=   0.850528;PhiROImin[1][ 0][19]=   0.116737;PhiROImax[1][ 0][19]=	0.224169;
     EtaROImin[1][ 0][20]=   0.964674;EtaROImax[1][ 0][20]=    1.02514;PhiROImin[1][ 0][20]= 0.00295181;PhiROImax[1][ 0][20]=	0.112612;
     EtaROImin[1][ 0][21]=   0.898201;EtaROImax[1][ 0][21]=   0.961344;PhiROImin[1][ 0][21]= 0.00295181;PhiROImax[1][ 0][21]=	0.112612;
     EtaROImin[1][ 0][22]=   0.964674;EtaROImax[1][ 0][22]=    1.02514;PhiROImin[1][ 0][22]=   0.116119;PhiROImax[1][ 0][22]=	0.223011;
     EtaROImin[1][ 0][23]=   0.898201;EtaROImax[1][ 0][23]=   0.961344;PhiROImin[1][ 0][23]=   0.116119;PhiROImax[1][ 0][23]=	0.223011;
     EtaROImin[1][ 0][24]=	    0;EtaROImax[1][ 0][24]=	     0;PhiROImin[1][ 0][24]=	      0;PhiROImax[1][ 0][24]=	       0;
     EtaROImin[1][ 0][25]=    1.03003;EtaROImax[1][ 0][25]=    1.06547;PhiROImin[1][ 0][25]= 0.00306876;PhiROImax[1][ 0][25]=	0.113429;
     EtaROImin[1][ 0][26]=	    0;EtaROImax[1][ 0][26]=	     0;PhiROImin[1][ 0][26]=	      0;PhiROImax[1][ 0][26]=	       0;
     EtaROImin[1][ 0][27]=    1.03003;EtaROImax[1][ 0][27]=    1.06547;PhiROImin[1][ 0][27]=   0.116958;PhiROImax[1][ 0][27]=	0.197143;
     EtaROImin[1][ 0][28]=	    0;EtaROImax[1][ 0][28]=	     0;PhiROImin[1][ 0][28]=	      0;PhiROImax[1][ 0][28]=	       0;
     EtaROImin[1][ 0][29]=	    0;EtaROImax[1][ 0][29]=	     0;PhiROImin[1][ 0][29]=	      0;PhiROImax[1][ 0][29]=	       0;
     EtaROImin[1][ 0][30]=	    0;EtaROImax[1][ 0][30]=	     0;PhiROImin[1][ 0][30]=	      0;PhiROImax[1][ 0][30]=	       0;
     EtaROImin[1][ 0][31]=	    0;EtaROImax[1][ 0][31]=	     0;PhiROImin[1][ 0][31]=	      0;PhiROImax[1][ 0][31]=	       0;
     EtaROImin[1][ 1][ 0]=  0.0208251;EtaROImax[1][ 1][ 0]=   0.118734;PhiROImin[1][ 1][ 0]=   0.305953;PhiROImax[1][ 1][ 0]=	0.389909;
     EtaROImin[1][ 1][ 1]=   0.116816;EtaROImax[1][ 1][ 1]=   0.213185;PhiROImin[1][ 1][ 1]=   0.305953;PhiROImax[1][ 1][ 1]=	0.389909;
     EtaROImin[1][ 1][ 2]=  0.0208251;EtaROImax[1][ 1][ 2]=   0.118734;PhiROImin[1][ 1][ 2]=   0.219649;PhiROImax[1][ 1][ 2]=	0.302322;
     EtaROImin[1][ 1][ 3]=   0.116816;EtaROImax[1][ 1][ 3]=   0.213185;PhiROImin[1][ 1][ 3]=   0.219649;PhiROImax[1][ 1][ 3]=	0.302322;
     EtaROImin[1][ 1][ 4]=   0.222449;EtaROImax[1][ 1][ 4]=   0.302928;PhiROImin[1][ 1][ 4]=   0.305953;PhiROImax[1][ 1][ 4]=	0.389909;
     EtaROImin[1][ 1][ 5]=    0.30075;EtaROImax[1][ 1][ 5]=   0.416721;PhiROImin[1][ 1][ 5]=   0.305953;PhiROImax[1][ 1][ 5]=	0.389909;
     EtaROImin[1][ 1][ 6]=   0.222449;EtaROImax[1][ 1][ 6]=   0.302928;PhiROImin[1][ 1][ 6]=   0.219649;PhiROImax[1][ 1][ 6]=	0.302322;
     EtaROImin[1][ 1][ 7]=    0.30075;EtaROImax[1][ 1][ 7]=   0.416721;PhiROImin[1][ 1][ 7]=   0.219649;PhiROImax[1][ 1][ 7]=	0.302322;
     EtaROImin[1][ 1][ 8]=    0.42967;EtaROImax[1][ 1][ 8]=   0.504617;PhiROImin[1][ 1][ 8]=   0.305953;PhiROImax[1][ 1][ 8]=	0.389909;
     EtaROImin[1][ 1][ 9]=   0.501681;EtaROImax[1][ 1][ 9]=   0.573871;PhiROImin[1][ 1][ 9]=   0.305953;PhiROImax[1][ 1][ 9]=	0.389909;
     EtaROImin[1][ 1][10]=    0.42967;EtaROImax[1][ 1][10]=   0.504617;PhiROImin[1][ 1][10]=   0.219649;PhiROImax[1][ 1][10]=	0.302322;
     EtaROImin[1][ 1][11]=   0.501681;EtaROImax[1][ 1][11]=   0.573871;PhiROImin[1][ 1][11]=   0.219649;PhiROImax[1][ 1][11]=	0.302322;
     EtaROImin[1][ 1][12]=   0.583785;EtaROImax[1][ 1][12]=   0.653329;PhiROImin[1][ 1][12]=   0.305953;PhiROImax[1][ 1][12]=	0.389909;
     EtaROImin[1][ 1][13]=   0.649934;EtaROImax[1][ 1][13]=   0.741516;PhiROImin[1][ 1][13]=   0.305953;PhiROImax[1][ 1][13]=	0.389909;
     EtaROImin[1][ 1][14]=   0.583785;EtaROImax[1][ 1][14]=   0.653329;PhiROImin[1][ 1][14]=   0.219649;PhiROImax[1][ 1][14]=	0.302322;
     EtaROImin[1][ 1][15]=   0.649934;EtaROImax[1][ 1][15]=   0.741516;PhiROImin[1][ 1][15]=   0.219649;PhiROImax[1][ 1][15]=	0.302322;
     EtaROImin[1][ 1][16]=   0.756521;EtaROImax[1][ 1][16]=   0.837822;PhiROImin[1][ 1][16]=   0.305953;PhiROImax[1][ 1][16]=	0.389909;
     EtaROImin[1][ 1][17]=	    0;EtaROImax[1][ 1][17]=	     0;PhiROImin[1][ 1][17]=	      0;PhiROImax[1][ 1][17]=	       0;
     EtaROImin[1][ 1][18]=   0.756521;EtaROImax[1][ 1][18]=   0.837822;PhiROImin[1][ 1][18]=   0.219649;PhiROImax[1][ 1][18]=	0.302322;
     EtaROImin[1][ 1][19]=	    0;EtaROImax[1][ 1][19]=	     0;PhiROImin[1][ 1][19]=	      0;PhiROImax[1][ 1][19]=	       0;
     EtaROImin[1][ 1][20]=   0.844116;EtaROImax[1][ 1][20]=   0.903324;PhiROImin[1][ 1][20]=   0.305953;PhiROImax[1][ 1][20]=	0.389909;
     EtaROImin[1][ 1][21]=   0.899344;EtaROImax[1][ 1][21]=   0.956037;PhiROImin[1][ 1][21]=   0.305953;PhiROImax[1][ 1][21]=	0.389909;
     EtaROImin[1][ 1][22]=   0.844116;EtaROImax[1][ 1][22]=   0.903324;PhiROImin[1][ 1][22]=   0.219649;PhiROImax[1][ 1][22]=	0.302322;
     EtaROImin[1][ 1][23]=   0.899344;EtaROImax[1][ 1][23]=   0.956037;PhiROImin[1][ 1][23]=   0.219649;PhiROImax[1][ 1][23]=	0.302322;
     EtaROImin[1][ 1][24]=	    0;EtaROImax[1][ 1][24]=	     0;PhiROImin[1][ 1][24]=	      0;PhiROImax[1][ 1][24]=	       0;
     EtaROImin[1][ 1][25]=	    0;EtaROImax[1][ 1][25]=	     0;PhiROImin[1][ 1][25]=	      0;PhiROImax[1][ 1][25]=	       0;
     EtaROImin[1][ 1][26]=	    0;EtaROImax[1][ 1][26]=	     0;PhiROImin[1][ 1][26]=	      0;PhiROImax[1][ 1][26]=	       0;
     EtaROImin[1][ 1][27]=	    0;EtaROImax[1][ 1][27]=	     0;PhiROImin[1][ 1][27]=	      0;PhiROImax[1][ 1][27]=	       0;
     EtaROImin[1][ 1][28]=	    0;EtaROImax[1][ 1][28]=	     0;PhiROImin[1][ 1][28]=	      0;PhiROImax[1][ 1][28]=	       0;
     EtaROImin[1][ 1][29]=	    0;EtaROImax[1][ 1][29]=	     0;PhiROImin[1][ 1][29]=	      0;PhiROImax[1][ 1][29]=	       0;
     EtaROImin[1][ 1][30]=	    0;EtaROImax[1][ 1][30]=	     0;PhiROImin[1][ 1][30]=	      0;PhiROImax[1][ 1][30]=	       0;
     EtaROImin[1][ 1][31]=	    0;EtaROImax[1][ 1][31]=	     0;PhiROImin[1][ 1][31]=	      0;PhiROImax[1][ 1][31]=	       0;
     EtaROImin[1][ 2][ 0]=   0.116816;EtaROImax[1][ 2][ 0]=   0.213185;PhiROImin[1][ 2][ 0]=   0.395489;PhiROImax[1][ 2][ 0]=	0.479445;
     EtaROImin[1][ 2][ 1]=  0.0208251;EtaROImax[1][ 2][ 1]=   0.118734;PhiROImin[1][ 2][ 1]=   0.395489;PhiROImax[1][ 2][ 1]=	0.479445;
     EtaROImin[1][ 2][ 2]=   0.116816;EtaROImax[1][ 2][ 2]=   0.213185;PhiROImin[1][ 2][ 2]=   0.483076;PhiROImax[1][ 2][ 2]=	0.565749;
     EtaROImin[1][ 2][ 3]=  0.0208251;EtaROImax[1][ 2][ 3]=   0.118734;PhiROImin[1][ 2][ 3]=   0.483076;PhiROImax[1][ 2][ 3]=	0.565749;
     EtaROImin[1][ 2][ 4]=    0.30075;EtaROImax[1][ 2][ 4]=   0.416721;PhiROImin[1][ 2][ 4]=   0.395489;PhiROImax[1][ 2][ 4]=	0.479445;
     EtaROImin[1][ 2][ 5]=   0.222449;EtaROImax[1][ 2][ 5]=   0.302928;PhiROImin[1][ 2][ 5]=   0.395489;PhiROImax[1][ 2][ 5]=	0.479445;
     EtaROImin[1][ 2][ 6]=    0.30075;EtaROImax[1][ 2][ 6]=   0.416721;PhiROImin[1][ 2][ 6]=   0.483076;PhiROImax[1][ 2][ 6]=	0.565749;
     EtaROImin[1][ 2][ 7]=   0.222449;EtaROImax[1][ 2][ 7]=   0.302928;PhiROImin[1][ 2][ 7]=   0.483076;PhiROImax[1][ 2][ 7]=	0.565749;
     EtaROImin[1][ 2][ 8]=   0.501681;EtaROImax[1][ 2][ 8]=   0.573871;PhiROImin[1][ 2][ 8]=   0.395489;PhiROImax[1][ 2][ 8]=	0.479445;
     EtaROImin[1][ 2][ 9]=    0.42967;EtaROImax[1][ 2][ 9]=   0.504617;PhiROImin[1][ 2][ 9]=   0.395489;PhiROImax[1][ 2][ 9]=	0.479445;
     EtaROImin[1][ 2][10]=   0.501681;EtaROImax[1][ 2][10]=   0.573871;PhiROImin[1][ 2][10]=   0.483076;PhiROImax[1][ 2][10]=	0.565749;
     EtaROImin[1][ 2][11]=    0.42967;EtaROImax[1][ 2][11]=   0.504617;PhiROImin[1][ 2][11]=   0.483076;PhiROImax[1][ 2][11]=	0.565749;
     EtaROImin[1][ 2][12]=   0.649934;EtaROImax[1][ 2][12]=   0.741516;PhiROImin[1][ 2][12]=   0.395489;PhiROImax[1][ 2][12]=	0.479445;
     EtaROImin[1][ 2][13]=   0.583785;EtaROImax[1][ 2][13]=   0.653329;PhiROImin[1][ 2][13]=   0.395489;PhiROImax[1][ 2][13]=	0.479445;
     EtaROImin[1][ 2][14]=   0.649934;EtaROImax[1][ 2][14]=   0.741516;PhiROImin[1][ 2][14]=   0.483076;PhiROImax[1][ 2][14]=	0.565749;
     EtaROImin[1][ 2][15]=   0.583785;EtaROImax[1][ 2][15]=   0.653329;PhiROImin[1][ 2][15]=   0.483076;PhiROImax[1][ 2][15]=	0.565749;
     EtaROImin[1][ 2][16]=	    0;EtaROImax[1][ 2][16]=	     0;PhiROImin[1][ 2][16]=	      0;PhiROImax[1][ 2][16]=	       0;
     EtaROImin[1][ 2][17]=   0.756521;EtaROImax[1][ 2][17]=   0.837822;PhiROImin[1][ 2][17]=   0.395489;PhiROImax[1][ 2][17]=	0.479445;
     EtaROImin[1][ 2][18]=	    0;EtaROImax[1][ 2][18]=	     0;PhiROImin[1][ 2][18]=	      0;PhiROImax[1][ 2][18]=	       0;
     EtaROImin[1][ 2][19]=   0.756521;EtaROImax[1][ 2][19]=   0.837822;PhiROImin[1][ 2][19]=   0.483076;PhiROImax[1][ 2][19]=	0.565749;
     EtaROImin[1][ 2][20]=   0.899344;EtaROImax[1][ 2][20]=   0.956037;PhiROImin[1][ 2][20]=   0.395489;PhiROImax[1][ 2][20]=	0.479445;
     EtaROImin[1][ 2][21]=   0.844116;EtaROImax[1][ 2][21]=   0.903324;PhiROImin[1][ 2][21]=   0.395489;PhiROImax[1][ 2][21]=	0.479445;
     EtaROImin[1][ 2][22]=   0.899344;EtaROImax[1][ 2][22]=   0.956037;PhiROImin[1][ 2][22]=   0.483076;PhiROImax[1][ 2][22]=	0.565749;
     EtaROImin[1][ 2][23]=   0.844116;EtaROImax[1][ 2][23]=   0.903324;PhiROImin[1][ 2][23]=   0.483076;PhiROImax[1][ 2][23]=	0.565749;
     EtaROImin[1][ 2][24]=	    0;EtaROImax[1][ 2][24]=	     0;PhiROImin[1][ 2][24]=	      0;PhiROImax[1][ 2][24]=	       0;
     EtaROImin[1][ 2][25]=	    0;EtaROImax[1][ 2][25]=	     0;PhiROImin[1][ 2][25]=	      0;PhiROImax[1][ 2][25]=	       0;
     EtaROImin[1][ 2][26]=	    0;EtaROImax[1][ 2][26]=	     0;PhiROImin[1][ 2][26]=	      0;PhiROImax[1][ 2][26]=	       0;
     EtaROImin[1][ 2][27]=	    0;EtaROImax[1][ 2][27]=	     0;PhiROImin[1][ 2][27]=	      0;PhiROImax[1][ 2][27]=	       0;
     EtaROImin[1][ 2][28]=	    0;EtaROImax[1][ 2][28]=	     0;PhiROImin[1][ 2][28]=	      0;PhiROImax[1][ 2][28]=	       0;
     EtaROImin[1][ 2][29]=	    0;EtaROImax[1][ 2][29]=	     0;PhiROImin[1][ 2][29]=	      0;PhiROImax[1][ 2][29]=	       0;
     EtaROImin[1][ 2][30]=	    0;EtaROImax[1][ 2][30]=	     0;PhiROImin[1][ 2][30]=	      0;PhiROImax[1][ 2][30]=	       0;
     EtaROImin[1][ 2][31]=	    0;EtaROImax[1][ 2][31]=	     0;PhiROImin[1][ 2][31]=	      0;PhiROImax[1][ 2][31]=	       0;
     EtaROImin[1][ 3][ 0]=  0.0231199;EtaROImax[1][ 3][ 0]=   0.131749;PhiROImin[1][ 3][ 0]=   0.672786;PhiROImax[1][ 3][ 0]=	0.782446;
     EtaROImin[1][ 3][ 1]=   0.130922;EtaROImax[1][ 3][ 1]=   0.238612;PhiROImin[1][ 3][ 1]=   0.672786;PhiROImax[1][ 3][ 1]=	0.782446;
     EtaROImin[1][ 3][ 2]=  0.0231199;EtaROImax[1][ 3][ 2]=   0.131749;PhiROImin[1][ 3][ 2]=   0.562387;PhiROImax[1][ 3][ 2]=	0.669279;
     EtaROImin[1][ 3][ 3]=   0.130922;EtaROImax[1][ 3][ 3]=   0.238612;PhiROImin[1][ 3][ 3]=   0.562387;PhiROImax[1][ 3][ 3]=	0.669279;
     EtaROImin[1][ 3][ 4]=   0.246207;EtaROImax[1][ 3][ 4]=    0.35049;PhiROImin[1][ 3][ 4]=   0.672786;PhiROImax[1][ 3][ 4]=	0.782446;
     EtaROImin[1][ 3][ 5]=   0.350831;EtaROImax[1][ 3][ 5]=   0.452099;PhiROImin[1][ 3][ 5]=   0.672786;PhiROImax[1][ 3][ 5]=	0.782446;
     EtaROImin[1][ 3][ 6]=   0.246207;EtaROImax[1][ 3][ 6]=    0.35049;PhiROImin[1][ 3][ 6]=   0.562387;PhiROImax[1][ 3][ 6]=	0.669279;
     EtaROImin[1][ 3][ 7]=   0.350831;EtaROImax[1][ 3][ 7]=   0.452099;PhiROImin[1][ 3][ 7]=   0.562387;PhiROImax[1][ 3][ 7]=	0.669279;
     EtaROImin[1][ 3][ 8]=   0.458091;EtaROImax[1][ 3][ 8]=   0.554444;PhiROImin[1][ 3][ 8]=   0.672786;PhiROImax[1][ 3][ 8]=	0.782446;
     EtaROImin[1][ 3][ 9]=   0.555789;EtaROImax[1][ 3][ 9]=   0.648101;PhiROImin[1][ 3][ 9]=   0.672786;PhiROImax[1][ 3][ 9]=	0.782446;
     EtaROImin[1][ 3][10]=   0.458091;EtaROImax[1][ 3][10]=   0.554444;PhiROImin[1][ 3][10]=   0.562387;PhiROImax[1][ 3][10]=	0.669279;
     EtaROImin[1][ 3][11]=   0.555789;EtaROImax[1][ 3][11]=   0.648101;PhiROImin[1][ 3][11]=   0.562387;PhiROImax[1][ 3][11]=	0.669279;
     EtaROImin[1][ 3][12]=   0.655857;EtaROImax[1][ 3][12]=   0.703298;PhiROImin[1][ 3][12]=   0.672186;PhiROImax[1][ 3][12]=	 0.78243;
     EtaROImin[1][ 3][13]=   0.706413;EtaROImax[1][ 3][13]=   0.776449;PhiROImin[1][ 3][13]=   0.672186;PhiROImax[1][ 3][13]=	 0.78243;
     EtaROImin[1][ 3][14]=   0.655857;EtaROImax[1][ 3][14]=   0.703298;PhiROImin[1][ 3][14]=   0.561229;PhiROImax[1][ 3][14]=	0.668661;
     EtaROImin[1][ 3][15]=   0.706413;EtaROImax[1][ 3][15]=   0.776449;PhiROImin[1][ 3][15]=   0.561229;PhiROImax[1][ 3][15]=	0.668661;
     EtaROImin[1][ 3][16]=   0.783867;EtaROImax[1][ 3][16]=    0.82768;PhiROImin[1][ 3][16]=   0.672186;PhiROImax[1][ 3][16]=	 0.78243;
     EtaROImin[1][ 3][17]=   0.830556;EtaROImax[1][ 3][17]=   0.895163;PhiROImin[1][ 3][17]=   0.672186;PhiROImax[1][ 3][17]=	 0.78243;
     EtaROImin[1][ 3][18]=   0.783867;EtaROImax[1][ 3][18]=    0.82768;PhiROImin[1][ 3][18]=   0.561229;PhiROImax[1][ 3][18]=	0.668661;
     EtaROImin[1][ 3][19]=   0.830556;EtaROImax[1][ 3][19]=   0.895163;PhiROImin[1][ 3][19]=   0.561229;PhiROImax[1][ 3][19]=	0.668661;
     EtaROImin[1][ 3][20]=   0.898201;EtaROImax[1][ 3][20]=   0.961344;PhiROImin[1][ 3][20]=   0.672786;PhiROImax[1][ 3][20]=	0.782446;
     EtaROImin[1][ 3][21]=   0.964674;EtaROImax[1][ 3][21]=    1.02514;PhiROImin[1][ 3][21]=   0.672786;PhiROImax[1][ 3][21]=	0.782446;
     EtaROImin[1][ 3][22]=   0.898201;EtaROImax[1][ 3][22]=   0.961344;PhiROImin[1][ 3][22]=   0.562387;PhiROImax[1][ 3][22]=	0.669279;
     EtaROImin[1][ 3][23]=   0.964674;EtaROImax[1][ 3][23]=    1.02514;PhiROImin[1][ 3][23]=   0.562387;PhiROImax[1][ 3][23]=	0.669279;
     EtaROImin[1][ 3][24]=    1.03003;EtaROImax[1][ 3][24]=    1.06547;PhiROImin[1][ 3][24]=   0.671969;PhiROImax[1][ 3][24]=	0.782329;
     EtaROImin[1][ 3][25]=	    0;EtaROImax[1][ 3][25]=	     0;PhiROImin[1][ 3][25]=	      0;PhiROImax[1][ 3][25]=	       0;
     EtaROImin[1][ 3][26]=    1.03003;EtaROImax[1][ 3][26]=    1.06547;PhiROImin[1][ 3][26]=   0.588255;PhiROImax[1][ 3][26]=	 0.66844;
     EtaROImin[1][ 3][27]=	    0;EtaROImax[1][ 3][27]=	     0;PhiROImin[1][ 3][27]=	      0;PhiROImax[1][ 3][27]=	       0;
     EtaROImin[1][ 3][28]=	    0;EtaROImax[1][ 3][28]=	     0;PhiROImin[1][ 3][28]=	      0;PhiROImax[1][ 3][28]=	       0;
     EtaROImin[1][ 3][29]=	    0;EtaROImax[1][ 3][29]=	     0;PhiROImin[1][ 3][29]=	      0;PhiROImax[1][ 3][29]=	       0;
     EtaROImin[1][ 3][30]=	    0;EtaROImax[1][ 3][30]=	     0;PhiROImin[1][ 3][30]=	      0;PhiROImax[1][ 3][30]=	       0;
     EtaROImin[1][ 3][31]=	    0;EtaROImax[1][ 3][31]=	     0;PhiROImin[1][ 3][31]=	      0;PhiROImax[1][ 3][31]=	       0;
     EtaROImin[1][ 4][ 0]=   0.130922;EtaROImax[1][ 4][ 0]=   0.238612;PhiROImin[1][ 4][ 0]=	0.78835;PhiROImax[1][ 4][ 0]=	0.898011;
     EtaROImin[1][ 4][ 1]=  0.0231199;EtaROImax[1][ 4][ 1]=   0.131749;PhiROImin[1][ 4][ 1]=	0.78835;PhiROImax[1][ 4][ 1]=	0.898011;
     EtaROImin[1][ 4][ 2]=   0.130922;EtaROImax[1][ 4][ 2]=   0.238612;PhiROImin[1][ 4][ 2]=   0.901517;PhiROImax[1][ 4][ 2]=	 1.00841;
     EtaROImin[1][ 4][ 3]=  0.0231199;EtaROImax[1][ 4][ 3]=   0.131749;PhiROImin[1][ 4][ 3]=   0.901517;PhiROImax[1][ 4][ 3]=	 1.00841;
     EtaROImin[1][ 4][ 4]=   0.350831;EtaROImax[1][ 4][ 4]=   0.452099;PhiROImin[1][ 4][ 4]=	0.78835;PhiROImax[1][ 4][ 4]=	0.898011;
     EtaROImin[1][ 4][ 5]=   0.246207;EtaROImax[1][ 4][ 5]=    0.35049;PhiROImin[1][ 4][ 5]=	0.78835;PhiROImax[1][ 4][ 5]=	0.898011;
     EtaROImin[1][ 4][ 6]=   0.350831;EtaROImax[1][ 4][ 6]=   0.452099;PhiROImin[1][ 4][ 6]=   0.901517;PhiROImax[1][ 4][ 6]=	 1.00841;
     EtaROImin[1][ 4][ 7]=   0.246207;EtaROImax[1][ 4][ 7]=    0.35049;PhiROImin[1][ 4][ 7]=   0.901517;PhiROImax[1][ 4][ 7]=	 1.00841;
     EtaROImin[1][ 4][ 8]=   0.555789;EtaROImax[1][ 4][ 8]=   0.648101;PhiROImin[1][ 4][ 8]=	0.78835;PhiROImax[1][ 4][ 8]=	0.898011;
     EtaROImin[1][ 4][ 9]=   0.458091;EtaROImax[1][ 4][ 9]=   0.554444;PhiROImin[1][ 4][ 9]=	0.78835;PhiROImax[1][ 4][ 9]=	0.898011;
     EtaROImin[1][ 4][10]=   0.555789;EtaROImax[1][ 4][10]=   0.648101;PhiROImin[1][ 4][10]=   0.901517;PhiROImax[1][ 4][10]=	 1.00841;
     EtaROImin[1][ 4][11]=   0.458091;EtaROImax[1][ 4][11]=   0.554444;PhiROImin[1][ 4][11]=   0.901517;PhiROImax[1][ 4][11]=	 1.00841;
     EtaROImin[1][ 4][12]=   0.731124;EtaROImax[1][ 4][12]=   0.776449;PhiROImin[1][ 4][12]=   0.788366;PhiROImax[1][ 4][12]=	 0.89861;
     EtaROImin[1][ 4][13]=   0.655857;EtaROImax[1][ 4][13]=   0.728056;PhiROImin[1][ 4][13]=   0.788366;PhiROImax[1][ 4][13]=	 0.89861;
     EtaROImin[1][ 4][14]=   0.731124;EtaROImax[1][ 4][14]=   0.776449;PhiROImin[1][ 4][14]=   0.902135;PhiROImax[1][ 4][14]=	 1.00957;
     EtaROImin[1][ 4][15]=   0.655857;EtaROImax[1][ 4][15]=   0.728056;PhiROImin[1][ 4][15]=   0.902135;PhiROImax[1][ 4][15]=	 1.00957;
     EtaROImin[1][ 4][16]=   0.853359;EtaROImax[1][ 4][16]=   0.895163;PhiROImin[1][ 4][16]=   0.788366;PhiROImax[1][ 4][16]=	 0.89861;
     EtaROImin[1][ 4][17]=   0.783867;EtaROImax[1][ 4][17]=   0.850528;PhiROImin[1][ 4][17]=   0.788366;PhiROImax[1][ 4][17]=	 0.89861;
     EtaROImin[1][ 4][18]=   0.853359;EtaROImax[1][ 4][18]=   0.895163;PhiROImin[1][ 4][18]=   0.902135;PhiROImax[1][ 4][18]=	 1.00957;
     EtaROImin[1][ 4][19]=   0.783867;EtaROImax[1][ 4][19]=   0.850528;PhiROImin[1][ 4][19]=   0.902135;PhiROImax[1][ 4][19]=	 1.00957;
     EtaROImin[1][ 4][20]=   0.964674;EtaROImax[1][ 4][20]=    1.02514;PhiROImin[1][ 4][20]=	0.78835;PhiROImax[1][ 4][20]=	0.898011;
     EtaROImin[1][ 4][21]=   0.898201;EtaROImax[1][ 4][21]=   0.961344;PhiROImin[1][ 4][21]=	0.78835;PhiROImax[1][ 4][21]=	0.898011;
     EtaROImin[1][ 4][22]=   0.964674;EtaROImax[1][ 4][22]=    1.02514;PhiROImin[1][ 4][22]=   0.901517;PhiROImax[1][ 4][22]=	 1.00841;
     EtaROImin[1][ 4][23]=   0.898201;EtaROImax[1][ 4][23]=   0.961344;PhiROImin[1][ 4][23]=   0.901517;PhiROImax[1][ 4][23]=	 1.00841;
     EtaROImin[1][ 4][24]=	    0;EtaROImax[1][ 4][24]=	     0;PhiROImin[1][ 4][24]=	      0;PhiROImax[1][ 4][24]=	       0;
     EtaROImin[1][ 4][25]=    1.03003;EtaROImax[1][ 4][25]=    1.06547;PhiROImin[1][ 4][25]=   0.788467;PhiROImax[1][ 4][25]=	0.898827;
     EtaROImin[1][ 4][26]=	    0;EtaROImax[1][ 4][26]=	     0;PhiROImin[1][ 4][26]=	      0;PhiROImax[1][ 4][26]=	       0;
     EtaROImin[1][ 4][27]=    1.03003;EtaROImax[1][ 4][27]=    1.06547;PhiROImin[1][ 4][27]=   0.902356;PhiROImax[1][ 4][27]=	0.982541;
     EtaROImin[1][ 4][28]=	    0;EtaROImax[1][ 4][28]=	     0;PhiROImin[1][ 4][28]=	      0;PhiROImax[1][ 4][28]=	       0;
     EtaROImin[1][ 4][29]=	    0;EtaROImax[1][ 4][29]=	     0;PhiROImin[1][ 4][29]=	      0;PhiROImax[1][ 4][29]=	       0;
     EtaROImin[1][ 4][30]=	    0;EtaROImax[1][ 4][30]=	     0;PhiROImin[1][ 4][30]=	      0;PhiROImax[1][ 4][30]=	       0;
     EtaROImin[1][ 4][31]=	    0;EtaROImax[1][ 4][31]=	     0;PhiROImin[1][ 4][31]=	      0;PhiROImax[1][ 4][31]=	       0;
     EtaROImin[1][ 5][ 0]=  0.0208251;EtaROImax[1][ 5][ 0]=   0.118734;PhiROImin[1][ 5][ 0]=	1.09135;PhiROImax[1][ 5][ 0]=	 1.17531;
     EtaROImin[1][ 5][ 1]=   0.116816;EtaROImax[1][ 5][ 1]=   0.213185;PhiROImin[1][ 5][ 1]=	1.09135;PhiROImax[1][ 5][ 1]=	 1.17531;
     EtaROImin[1][ 5][ 2]=  0.0208251;EtaROImax[1][ 5][ 2]=   0.118734;PhiROImin[1][ 5][ 2]=	1.00505;PhiROImax[1][ 5][ 2]=	 1.08772;
     EtaROImin[1][ 5][ 3]=   0.116816;EtaROImax[1][ 5][ 3]=   0.213185;PhiROImin[1][ 5][ 3]=	1.00505;PhiROImax[1][ 5][ 3]=	 1.08772;
     EtaROImin[1][ 5][ 4]=   0.222449;EtaROImax[1][ 5][ 4]=   0.302928;PhiROImin[1][ 5][ 4]=	1.09135;PhiROImax[1][ 5][ 4]=	 1.17531;
     EtaROImin[1][ 5][ 5]=    0.30075;EtaROImax[1][ 5][ 5]=   0.416721;PhiROImin[1][ 5][ 5]=	1.09135;PhiROImax[1][ 5][ 5]=	 1.17531;
     EtaROImin[1][ 5][ 6]=   0.222449;EtaROImax[1][ 5][ 6]=   0.302928;PhiROImin[1][ 5][ 6]=	1.00505;PhiROImax[1][ 5][ 6]=	 1.08772;
     EtaROImin[1][ 5][ 7]=    0.30075;EtaROImax[1][ 5][ 7]=   0.416721;PhiROImin[1][ 5][ 7]=	1.00505;PhiROImax[1][ 5][ 7]=	 1.08772;
     EtaROImin[1][ 5][ 8]=    0.42967;EtaROImax[1][ 5][ 8]=   0.504617;PhiROImin[1][ 5][ 8]=	1.09135;PhiROImax[1][ 5][ 8]=	 1.17531;
     EtaROImin[1][ 5][ 9]=   0.501681;EtaROImax[1][ 5][ 9]=   0.573871;PhiROImin[1][ 5][ 9]=	1.09135;PhiROImax[1][ 5][ 9]=	 1.17531;
     EtaROImin[1][ 5][10]=    0.42967;EtaROImax[1][ 5][10]=   0.504617;PhiROImin[1][ 5][10]=	1.00505;PhiROImax[1][ 5][10]=	 1.08772;
     EtaROImin[1][ 5][11]=   0.501681;EtaROImax[1][ 5][11]=   0.573871;PhiROImin[1][ 5][11]=	1.00505;PhiROImax[1][ 5][11]=	 1.08772;
     EtaROImin[1][ 5][12]=   0.583785;EtaROImax[1][ 5][12]=   0.653329;PhiROImin[1][ 5][12]=	1.09135;PhiROImax[1][ 5][12]=	 1.17531;
     EtaROImin[1][ 5][13]=   0.649934;EtaROImax[1][ 5][13]=   0.741516;PhiROImin[1][ 5][13]=	1.09135;PhiROImax[1][ 5][13]=	 1.17531;
     EtaROImin[1][ 5][14]=   0.583785;EtaROImax[1][ 5][14]=   0.653329;PhiROImin[1][ 5][14]=	1.00505;PhiROImax[1][ 5][14]=	 1.08772;
     EtaROImin[1][ 5][15]=   0.649934;EtaROImax[1][ 5][15]=   0.741516;PhiROImin[1][ 5][15]=	1.00505;PhiROImax[1][ 5][15]=	 1.08772;
     EtaROImin[1][ 5][16]=   0.756521;EtaROImax[1][ 5][16]=   0.837822;PhiROImin[1][ 5][16]=	1.09135;PhiROImax[1][ 5][16]=	 1.17531;
     EtaROImin[1][ 5][17]=	    0;EtaROImax[1][ 5][17]=	     0;PhiROImin[1][ 5][17]=	      0;PhiROImax[1][ 5][17]=	       0;
     EtaROImin[1][ 5][18]=   0.756521;EtaROImax[1][ 5][18]=   0.837822;PhiROImin[1][ 5][18]=	1.00505;PhiROImax[1][ 5][18]=	 1.08772;
     EtaROImin[1][ 5][19]=	    0;EtaROImax[1][ 5][19]=	     0;PhiROImin[1][ 5][19]=	      0;PhiROImax[1][ 5][19]=	       0;
     EtaROImin[1][ 5][20]=   0.844116;EtaROImax[1][ 5][20]=   0.903324;PhiROImin[1][ 5][20]=	1.09135;PhiROImax[1][ 5][20]=	 1.17531;
     EtaROImin[1][ 5][21]=   0.899344;EtaROImax[1][ 5][21]=   0.956037;PhiROImin[1][ 5][21]=	1.09135;PhiROImax[1][ 5][21]=	 1.17531;
     EtaROImin[1][ 5][22]=   0.844116;EtaROImax[1][ 5][22]=   0.903324;PhiROImin[1][ 5][22]=	1.00505;PhiROImax[1][ 5][22]=	 1.08772;
     EtaROImin[1][ 5][23]=   0.899344;EtaROImax[1][ 5][23]=   0.956037;PhiROImin[1][ 5][23]=	1.00505;PhiROImax[1][ 5][23]=	 1.08772;
     EtaROImin[1][ 5][24]=	    0;EtaROImax[1][ 5][24]=	     0;PhiROImin[1][ 5][24]=	      0;PhiROImax[1][ 5][24]=	       0;
     EtaROImin[1][ 5][25]=	    0;EtaROImax[1][ 5][25]=	     0;PhiROImin[1][ 5][25]=	      0;PhiROImax[1][ 5][25]=	       0;
     EtaROImin[1][ 5][26]=	    0;EtaROImax[1][ 5][26]=	     0;PhiROImin[1][ 5][26]=	      0;PhiROImax[1][ 5][26]=	       0;
     EtaROImin[1][ 5][27]=	    0;EtaROImax[1][ 5][27]=	     0;PhiROImin[1][ 5][27]=	      0;PhiROImax[1][ 5][27]=	       0;
     EtaROImin[1][ 5][28]=	    0;EtaROImax[1][ 5][28]=	     0;PhiROImin[1][ 5][28]=	      0;PhiROImax[1][ 5][28]=	       0;
     EtaROImin[1][ 5][29]=	    0;EtaROImax[1][ 5][29]=	     0;PhiROImin[1][ 5][29]=	      0;PhiROImax[1][ 5][29]=	       0;
     EtaROImin[1][ 5][30]=	    0;EtaROImax[1][ 5][30]=	     0;PhiROImin[1][ 5][30]=	      0;PhiROImax[1][ 5][30]=	       0;
     EtaROImin[1][ 5][31]=	    0;EtaROImax[1][ 5][31]=	     0;PhiROImin[1][ 5][31]=	      0;PhiROImax[1][ 5][31]=	       0;
     EtaROImin[1][ 6][ 0]=   0.116816;EtaROImax[1][ 6][ 0]=   0.213185;PhiROImin[1][ 6][ 0]=	1.18089;PhiROImax[1][ 6][ 0]=	 1.26484;
     EtaROImin[1][ 6][ 1]=  0.0208251;EtaROImax[1][ 6][ 1]=   0.118734;PhiROImin[1][ 6][ 1]=	1.18089;PhiROImax[1][ 6][ 1]=	 1.26484;
     EtaROImin[1][ 6][ 2]=   0.116816;EtaROImax[1][ 6][ 2]=   0.213185;PhiROImin[1][ 6][ 2]=	1.26847;PhiROImax[1][ 6][ 2]=	 1.35115;
     EtaROImin[1][ 6][ 3]=  0.0208251;EtaROImax[1][ 6][ 3]=   0.118734;PhiROImin[1][ 6][ 3]=	1.26847;PhiROImax[1][ 6][ 3]=	 1.35115;
     EtaROImin[1][ 6][ 4]=    0.30075;EtaROImax[1][ 6][ 4]=   0.416721;PhiROImin[1][ 6][ 4]=	1.18089;PhiROImax[1][ 6][ 4]=	 1.26484;
     EtaROImin[1][ 6][ 5]=   0.222449;EtaROImax[1][ 6][ 5]=   0.302928;PhiROImin[1][ 6][ 5]=	1.18089;PhiROImax[1][ 6][ 5]=	 1.26484;
     EtaROImin[1][ 6][ 6]=    0.30075;EtaROImax[1][ 6][ 6]=   0.416721;PhiROImin[1][ 6][ 6]=	1.26847;PhiROImax[1][ 6][ 6]=	 1.35115;
     EtaROImin[1][ 6][ 7]=   0.222449;EtaROImax[1][ 6][ 7]=   0.302928;PhiROImin[1][ 6][ 7]=	1.26847;PhiROImax[1][ 6][ 7]=	 1.35115;
     EtaROImin[1][ 6][ 8]=   0.501681;EtaROImax[1][ 6][ 8]=   0.573871;PhiROImin[1][ 6][ 8]=	1.18089;PhiROImax[1][ 6][ 8]=	 1.26484;
     EtaROImin[1][ 6][ 9]=    0.42967;EtaROImax[1][ 6][ 9]=   0.504617;PhiROImin[1][ 6][ 9]=	1.18089;PhiROImax[1][ 6][ 9]=	 1.26484;
     EtaROImin[1][ 6][10]=   0.501681;EtaROImax[1][ 6][10]=   0.573871;PhiROImin[1][ 6][10]=	1.26847;PhiROImax[1][ 6][10]=	 1.35115;
     EtaROImin[1][ 6][11]=    0.42967;EtaROImax[1][ 6][11]=   0.504617;PhiROImin[1][ 6][11]=	1.26847;PhiROImax[1][ 6][11]=	 1.35115;
     EtaROImin[1][ 6][12]=   0.649934;EtaROImax[1][ 6][12]=   0.741516;PhiROImin[1][ 6][12]=	1.18089;PhiROImax[1][ 6][12]=	 1.26484;
     EtaROImin[1][ 6][13]=   0.583785;EtaROImax[1][ 6][13]=   0.653329;PhiROImin[1][ 6][13]=	1.18089;PhiROImax[1][ 6][13]=	 1.26484;
     EtaROImin[1][ 6][14]=   0.649934;EtaROImax[1][ 6][14]=   0.741516;PhiROImin[1][ 6][14]=	1.26847;PhiROImax[1][ 6][14]=	 1.35115;
     EtaROImin[1][ 6][15]=   0.583785;EtaROImax[1][ 6][15]=   0.653329;PhiROImin[1][ 6][15]=	1.26847;PhiROImax[1][ 6][15]=	 1.35115;
     EtaROImin[1][ 6][16]=	    0;EtaROImax[1][ 6][16]=	     0;PhiROImin[1][ 6][16]=	      0;PhiROImax[1][ 6][16]=	       0;
     EtaROImin[1][ 6][17]=   0.756521;EtaROImax[1][ 6][17]=   0.837822;PhiROImin[1][ 6][17]=	1.18089;PhiROImax[1][ 6][17]=	 1.26484;
     EtaROImin[1][ 6][18]=	    0;EtaROImax[1][ 6][18]=	     0;PhiROImin[1][ 6][18]=	      0;PhiROImax[1][ 6][18]=	       0;
     EtaROImin[1][ 6][19]=   0.756521;EtaROImax[1][ 6][19]=   0.837822;PhiROImin[1][ 6][19]=	1.26847;PhiROImax[1][ 6][19]=	 1.35115;
     EtaROImin[1][ 6][20]=   0.899344;EtaROImax[1][ 6][20]=   0.956037;PhiROImin[1][ 6][20]=	1.18089;PhiROImax[1][ 6][20]=	 1.26484;
     EtaROImin[1][ 6][21]=   0.844116;EtaROImax[1][ 6][21]=   0.903324;PhiROImin[1][ 6][21]=	1.18089;PhiROImax[1][ 6][21]=	 1.26484;
     EtaROImin[1][ 6][22]=   0.899344;EtaROImax[1][ 6][22]=   0.956037;PhiROImin[1][ 6][22]=	1.26847;PhiROImax[1][ 6][22]=	 1.35115;
     EtaROImin[1][ 6][23]=   0.844116;EtaROImax[1][ 6][23]=   0.903324;PhiROImin[1][ 6][23]=	1.26847;PhiROImax[1][ 6][23]=	 1.35115;
     EtaROImin[1][ 6][24]=	    0;EtaROImax[1][ 6][24]=	     0;PhiROImin[1][ 6][24]=	      0;PhiROImax[1][ 6][24]=	       0;
     EtaROImin[1][ 6][25]=	    0;EtaROImax[1][ 6][25]=	     0;PhiROImin[1][ 6][25]=	      0;PhiROImax[1][ 6][25]=	       0;
     EtaROImin[1][ 6][26]=	    0;EtaROImax[1][ 6][26]=	     0;PhiROImin[1][ 6][26]=	      0;PhiROImax[1][ 6][26]=	       0;
     EtaROImin[1][ 6][27]=	    0;EtaROImax[1][ 6][27]=	     0;PhiROImin[1][ 6][27]=	      0;PhiROImax[1][ 6][27]=	       0;
     EtaROImin[1][ 6][28]=	    0;EtaROImax[1][ 6][28]=	     0;PhiROImin[1][ 6][28]=	      0;PhiROImax[1][ 6][28]=	       0;
     EtaROImin[1][ 6][29]=	    0;EtaROImax[1][ 6][29]=	     0;PhiROImin[1][ 6][29]=	      0;PhiROImax[1][ 6][29]=	       0;
     EtaROImin[1][ 6][30]=	    0;EtaROImax[1][ 6][30]=	     0;PhiROImin[1][ 6][30]=	      0;PhiROImax[1][ 6][30]=	       0;
     EtaROImin[1][ 6][31]=	    0;EtaROImax[1][ 6][31]=	     0;PhiROImin[1][ 6][31]=	      0;PhiROImax[1][ 6][31]=	       0;
     EtaROImin[1][ 7][ 0]=  0.0552456;EtaROImax[1][ 7][ 0]=   0.147216;PhiROImin[1][ 7][ 0]=	1.45818;PhiROImax[1][ 7][ 0]=	 1.56784;
     EtaROImin[1][ 7][ 1]=    0.14708;EtaROImax[1][ 7][ 1]=   0.238313;PhiROImin[1][ 7][ 1]=	1.45818;PhiROImax[1][ 7][ 1]=	 1.56784;
     EtaROImin[1][ 7][ 2]=  0.0552456;EtaROImax[1][ 7][ 2]=   0.147216;PhiROImin[1][ 7][ 2]=	1.34779;PhiROImax[1][ 7][ 2]=	 1.45468;
     EtaROImin[1][ 7][ 3]=    0.14708;EtaROImax[1][ 7][ 3]=   0.238313;PhiROImin[1][ 7][ 3]=	1.34779;PhiROImax[1][ 7][ 3]=	 1.45468;
     EtaROImin[1][ 7][ 4]=   0.246207;EtaROImax[1][ 7][ 4]=    0.35049;PhiROImin[1][ 7][ 4]=	1.45818;PhiROImax[1][ 7][ 4]=	 1.56784;
     EtaROImin[1][ 7][ 5]=   0.350831;EtaROImax[1][ 7][ 5]=   0.452099;PhiROImin[1][ 7][ 5]=	1.45818;PhiROImax[1][ 7][ 5]=	 1.56784;
     EtaROImin[1][ 7][ 6]=   0.246207;EtaROImax[1][ 7][ 6]=    0.35049;PhiROImin[1][ 7][ 6]=	1.34779;PhiROImax[1][ 7][ 6]=	 1.45468;
     EtaROImin[1][ 7][ 7]=   0.350831;EtaROImax[1][ 7][ 7]=   0.452099;PhiROImin[1][ 7][ 7]=	1.34779;PhiROImax[1][ 7][ 7]=	 1.45468;
     EtaROImin[1][ 7][ 8]=   0.458091;EtaROImax[1][ 7][ 8]=   0.554444;PhiROImin[1][ 7][ 8]=	1.45818;PhiROImax[1][ 7][ 8]=	 1.56784;
     EtaROImin[1][ 7][ 9]=   0.555789;EtaROImax[1][ 7][ 9]=   0.648101;PhiROImin[1][ 7][ 9]=	1.45818;PhiROImax[1][ 7][ 9]=	 1.56784;
     EtaROImin[1][ 7][10]=   0.458091;EtaROImax[1][ 7][10]=   0.554444;PhiROImin[1][ 7][10]=	1.34779;PhiROImax[1][ 7][10]=	 1.45468;
     EtaROImin[1][ 7][11]=   0.555789;EtaROImax[1][ 7][11]=   0.648101;PhiROImin[1][ 7][11]=	1.34779;PhiROImax[1][ 7][11]=	 1.45468;
     EtaROImin[1][ 7][12]=   0.655857;EtaROImax[1][ 7][12]=   0.703298;PhiROImin[1][ 7][12]=	1.45758;PhiROImax[1][ 7][12]=	 1.56783;
     EtaROImin[1][ 7][13]=   0.706413;EtaROImax[1][ 7][13]=   0.776449;PhiROImin[1][ 7][13]=	1.45758;PhiROImax[1][ 7][13]=	 1.56783;
     EtaROImin[1][ 7][14]=   0.655857;EtaROImax[1][ 7][14]=   0.703298;PhiROImin[1][ 7][14]=	1.34663;PhiROImax[1][ 7][14]=	 1.45406;
     EtaROImin[1][ 7][15]=   0.706413;EtaROImax[1][ 7][15]=   0.776449;PhiROImin[1][ 7][15]=	1.34663;PhiROImax[1][ 7][15]=	 1.45406;
     EtaROImin[1][ 7][16]=   0.783867;EtaROImax[1][ 7][16]=    0.82768;PhiROImin[1][ 7][16]=	1.45758;PhiROImax[1][ 7][16]=	 1.56783;
     EtaROImin[1][ 7][17]=   0.830556;EtaROImax[1][ 7][17]=   0.895163;PhiROImin[1][ 7][17]=	1.45758;PhiROImax[1][ 7][17]=	 1.56783;
     EtaROImin[1][ 7][18]=   0.783867;EtaROImax[1][ 7][18]=    0.82768;PhiROImin[1][ 7][18]=	1.34663;PhiROImax[1][ 7][18]=	 1.45406;
     EtaROImin[1][ 7][19]=   0.830556;EtaROImax[1][ 7][19]=   0.895163;PhiROImin[1][ 7][19]=	1.34663;PhiROImax[1][ 7][19]=	 1.45406;
     EtaROImin[1][ 7][20]=   0.898201;EtaROImax[1][ 7][20]=   0.961344;PhiROImin[1][ 7][20]=	1.45818;PhiROImax[1][ 7][20]=	 1.56784;
     EtaROImin[1][ 7][21]=   0.964674;EtaROImax[1][ 7][21]=    1.02514;PhiROImin[1][ 7][21]=	1.45818;PhiROImax[1][ 7][21]=	 1.56784;
     EtaROImin[1][ 7][22]=   0.898201;EtaROImax[1][ 7][22]=   0.961344;PhiROImin[1][ 7][22]=	1.34779;PhiROImax[1][ 7][22]=	 1.45468;
     EtaROImin[1][ 7][23]=   0.964674;EtaROImax[1][ 7][23]=    1.02514;PhiROImin[1][ 7][23]=	1.34779;PhiROImax[1][ 7][23]=	 1.45468;
     EtaROImin[1][ 7][24]=    1.03003;EtaROImax[1][ 7][24]=    1.06547;PhiROImin[1][ 7][24]=	1.45737;PhiROImax[1][ 7][24]=	 1.56773;
     EtaROImin[1][ 7][25]=	    0;EtaROImax[1][ 7][25]=	     0;PhiROImin[1][ 7][25]=	      0;PhiROImax[1][ 7][25]=	       0;
     EtaROImin[1][ 7][26]=    1.03003;EtaROImax[1][ 7][26]=    1.06547;PhiROImin[1][ 7][26]=	1.37365;PhiROImax[1][ 7][26]=	 1.45384;
     EtaROImin[1][ 7][27]=	    0;EtaROImax[1][ 7][27]=	     0;PhiROImin[1][ 7][27]=	      0;PhiROImax[1][ 7][27]=	       0;
     EtaROImin[1][ 7][28]=	    0;EtaROImax[1][ 7][28]=	     0;PhiROImin[1][ 7][28]=	      0;PhiROImax[1][ 7][28]=	       0;
     EtaROImin[1][ 7][29]=	    0;EtaROImax[1][ 7][29]=	     0;PhiROImin[1][ 7][29]=	      0;PhiROImax[1][ 7][29]=	       0;
     EtaROImin[1][ 7][30]=	    0;EtaROImax[1][ 7][30]=	     0;PhiROImin[1][ 7][30]=	      0;PhiROImax[1][ 7][30]=	       0;
     EtaROImin[1][ 7][31]=	    0;EtaROImax[1][ 7][31]=	     0;PhiROImin[1][ 7][31]=	      0;PhiROImax[1][ 7][31]=	       0;
     EtaROImin[1][ 8][ 0]=    0.14708;EtaROImax[1][ 8][ 0]=   0.238313;PhiROImin[1][ 8][ 0]=	1.57375;PhiROImax[1][ 8][ 0]=	 1.68341;
     EtaROImin[1][ 8][ 1]=  0.0552456;EtaROImax[1][ 8][ 1]=   0.147216;PhiROImin[1][ 8][ 1]=	1.57375;PhiROImax[1][ 8][ 1]=	 1.68341;
     EtaROImin[1][ 8][ 2]=    0.14708;EtaROImax[1][ 8][ 2]=   0.238313;PhiROImin[1][ 8][ 2]=	1.68692;PhiROImax[1][ 8][ 2]=	 1.79381;
     EtaROImin[1][ 8][ 3]=  0.0552456;EtaROImax[1][ 8][ 3]=   0.147216;PhiROImin[1][ 8][ 3]=	1.68692;PhiROImax[1][ 8][ 3]=	 1.79381;
     EtaROImin[1][ 8][ 4]=   0.350831;EtaROImax[1][ 8][ 4]=   0.452099;PhiROImin[1][ 8][ 4]=	1.57375;PhiROImax[1][ 8][ 4]=	 1.68341;
     EtaROImin[1][ 8][ 5]=   0.246207;EtaROImax[1][ 8][ 5]=    0.35049;PhiROImin[1][ 8][ 5]=	1.57375;PhiROImax[1][ 8][ 5]=	 1.68341;
     EtaROImin[1][ 8][ 6]=   0.350831;EtaROImax[1][ 8][ 6]=   0.452099;PhiROImin[1][ 8][ 6]=	1.68692;PhiROImax[1][ 8][ 6]=	 1.79381;
     EtaROImin[1][ 8][ 7]=   0.246207;EtaROImax[1][ 8][ 7]=    0.35049;PhiROImin[1][ 8][ 7]=	1.68692;PhiROImax[1][ 8][ 7]=	 1.79381;
     EtaROImin[1][ 8][ 8]=   0.555789;EtaROImax[1][ 8][ 8]=   0.648101;PhiROImin[1][ 8][ 8]=	1.57375;PhiROImax[1][ 8][ 8]=	 1.68341;
     EtaROImin[1][ 8][ 9]=   0.458091;EtaROImax[1][ 8][ 9]=   0.554444;PhiROImin[1][ 8][ 9]=	1.57375;PhiROImax[1][ 8][ 9]=	 1.68341;
     EtaROImin[1][ 8][10]=   0.555789;EtaROImax[1][ 8][10]=   0.648101;PhiROImin[1][ 8][10]=	1.68692;PhiROImax[1][ 8][10]=	 1.79381;
     EtaROImin[1][ 8][11]=   0.458091;EtaROImax[1][ 8][11]=   0.554444;PhiROImin[1][ 8][11]=	1.68692;PhiROImax[1][ 8][11]=	 1.79381;
     EtaROImin[1][ 8][12]=   0.731124;EtaROImax[1][ 8][12]=   0.776449;PhiROImin[1][ 8][12]=	1.57376;PhiROImax[1][ 8][12]=	 1.68401;
     EtaROImin[1][ 8][13]=   0.655857;EtaROImax[1][ 8][13]=   0.728056;PhiROImin[1][ 8][13]=	1.57376;PhiROImax[1][ 8][13]=	 1.68401;
     EtaROImin[1][ 8][14]=   0.731124;EtaROImax[1][ 8][14]=   0.776449;PhiROImin[1][ 8][14]=	1.68753;PhiROImax[1][ 8][14]=	 1.79497;
     EtaROImin[1][ 8][15]=   0.655857;EtaROImax[1][ 8][15]=   0.728056;PhiROImin[1][ 8][15]=	1.68753;PhiROImax[1][ 8][15]=	 1.79497;
     EtaROImin[1][ 8][16]=   0.853359;EtaROImax[1][ 8][16]=   0.895163;PhiROImin[1][ 8][16]=	1.57376;PhiROImax[1][ 8][16]=	 1.68401;
     EtaROImin[1][ 8][17]=   0.783867;EtaROImax[1][ 8][17]=   0.850528;PhiROImin[1][ 8][17]=	1.57376;PhiROImax[1][ 8][17]=	 1.68401;
     EtaROImin[1][ 8][18]=   0.853359;EtaROImax[1][ 8][18]=   0.895163;PhiROImin[1][ 8][18]=	1.68753;PhiROImax[1][ 8][18]=	 1.79497;
     EtaROImin[1][ 8][19]=   0.783867;EtaROImax[1][ 8][19]=   0.850528;PhiROImin[1][ 8][19]=	1.68753;PhiROImax[1][ 8][19]=	 1.79497;
     EtaROImin[1][ 8][20]=   0.964674;EtaROImax[1][ 8][20]=    1.02514;PhiROImin[1][ 8][20]=	1.57375;PhiROImax[1][ 8][20]=	 1.68341;
     EtaROImin[1][ 8][21]=   0.898201;EtaROImax[1][ 8][21]=   0.961344;PhiROImin[1][ 8][21]=	1.57375;PhiROImax[1][ 8][21]=	 1.68341;
     EtaROImin[1][ 8][22]=   0.964674;EtaROImax[1][ 8][22]=    1.02514;PhiROImin[1][ 8][22]=	1.68692;PhiROImax[1][ 8][22]=	 1.79381;
     EtaROImin[1][ 8][23]=   0.898201;EtaROImax[1][ 8][23]=   0.961344;PhiROImin[1][ 8][23]=	1.68692;PhiROImax[1][ 8][23]=	 1.79381;
     EtaROImin[1][ 8][24]=	    0;EtaROImax[1][ 8][24]=	     0;PhiROImin[1][ 8][24]=	      0;PhiROImax[1][ 8][24]=	       0;
     EtaROImin[1][ 8][25]=    1.03003;EtaROImax[1][ 8][25]=    1.06547;PhiROImin[1][ 8][25]=	1.57387;PhiROImax[1][ 8][25]=	 1.68423;
     EtaROImin[1][ 8][26]=	    0;EtaROImax[1][ 8][26]=	     0;PhiROImin[1][ 8][26]=	      0;PhiROImax[1][ 8][26]=	       0;
     EtaROImin[1][ 8][27]=    1.03003;EtaROImax[1][ 8][27]=    1.06547;PhiROImin[1][ 8][27]=	1.68775;PhiROImax[1][ 8][27]=	 1.76794;
     EtaROImin[1][ 8][28]=	    0;EtaROImax[1][ 8][28]=	     0;PhiROImin[1][ 8][28]=	      0;PhiROImax[1][ 8][28]=	       0;
     EtaROImin[1][ 8][29]=	    0;EtaROImax[1][ 8][29]=	     0;PhiROImin[1][ 8][29]=	      0;PhiROImax[1][ 8][29]=	       0;
     EtaROImin[1][ 8][30]=	    0;EtaROImax[1][ 8][30]=	     0;PhiROImin[1][ 8][30]=	      0;PhiROImax[1][ 8][30]=	       0;
     EtaROImin[1][ 8][31]=	    0;EtaROImax[1][ 8][31]=	     0;PhiROImin[1][ 8][31]=	      0;PhiROImax[1][ 8][31]=	       0;
     EtaROImin[1][ 9][ 0]=  0.0208251;EtaROImax[1][ 9][ 0]=   0.118734;PhiROImin[1][ 9][ 0]=	1.87675;PhiROImax[1][ 9][ 0]=	 1.96071;
     EtaROImin[1][ 9][ 1]=   0.116816;EtaROImax[1][ 9][ 1]=   0.213185;PhiROImin[1][ 9][ 1]=	1.87675;PhiROImax[1][ 9][ 1]=	 1.96071;
     EtaROImin[1][ 9][ 2]=  0.0208251;EtaROImax[1][ 9][ 2]=   0.118734;PhiROImin[1][ 9][ 2]=	1.79045;PhiROImax[1][ 9][ 2]=	 1.87312;
     EtaROImin[1][ 9][ 3]=   0.116816;EtaROImax[1][ 9][ 3]=   0.213185;PhiROImin[1][ 9][ 3]=	1.79045;PhiROImax[1][ 9][ 3]=	 1.87312;
     EtaROImin[1][ 9][ 4]=   0.222449;EtaROImax[1][ 9][ 4]=   0.302928;PhiROImin[1][ 9][ 4]=	1.87675;PhiROImax[1][ 9][ 4]=	 1.96071;
     EtaROImin[1][ 9][ 5]=    0.30075;EtaROImax[1][ 9][ 5]=   0.416721;PhiROImin[1][ 9][ 5]=	1.87675;PhiROImax[1][ 9][ 5]=	 1.96071;
     EtaROImin[1][ 9][ 6]=   0.222449;EtaROImax[1][ 9][ 6]=   0.302928;PhiROImin[1][ 9][ 6]=	1.79045;PhiROImax[1][ 9][ 6]=	 1.87312;
     EtaROImin[1][ 9][ 7]=    0.30075;EtaROImax[1][ 9][ 7]=   0.416721;PhiROImin[1][ 9][ 7]=	1.79045;PhiROImax[1][ 9][ 7]=	 1.87312;
     EtaROImin[1][ 9][ 8]=    0.42967;EtaROImax[1][ 9][ 8]=   0.504617;PhiROImin[1][ 9][ 8]=	1.87675;PhiROImax[1][ 9][ 8]=	 1.96071;
     EtaROImin[1][ 9][ 9]=   0.501681;EtaROImax[1][ 9][ 9]=   0.573871;PhiROImin[1][ 9][ 9]=	1.87675;PhiROImax[1][ 9][ 9]=	 1.96071;
     EtaROImin[1][ 9][10]=    0.42967;EtaROImax[1][ 9][10]=   0.504617;PhiROImin[1][ 9][10]=	1.79045;PhiROImax[1][ 9][10]=	 1.87312;
     EtaROImin[1][ 9][11]=   0.501681;EtaROImax[1][ 9][11]=   0.573871;PhiROImin[1][ 9][11]=	1.79045;PhiROImax[1][ 9][11]=	 1.87312;
     EtaROImin[1][ 9][12]=   0.583785;EtaROImax[1][ 9][12]=   0.653329;PhiROImin[1][ 9][12]=	1.87675;PhiROImax[1][ 9][12]=	 1.96071;
     EtaROImin[1][ 9][13]=   0.649934;EtaROImax[1][ 9][13]=   0.741516;PhiROImin[1][ 9][13]=	1.87675;PhiROImax[1][ 9][13]=	 1.96071;
     EtaROImin[1][ 9][14]=   0.583785;EtaROImax[1][ 9][14]=   0.653329;PhiROImin[1][ 9][14]=	1.79045;PhiROImax[1][ 9][14]=	 1.87312;
     EtaROImin[1][ 9][15]=   0.649934;EtaROImax[1][ 9][15]=   0.741516;PhiROImin[1][ 9][15]=	1.79045;PhiROImax[1][ 9][15]=	 1.87312;
     EtaROImin[1][ 9][16]=   0.756521;EtaROImax[1][ 9][16]=   0.837822;PhiROImin[1][ 9][16]=	1.87675;PhiROImax[1][ 9][16]=	 1.96071;
     EtaROImin[1][ 9][17]=	    0;EtaROImax[1][ 9][17]=	     0;PhiROImin[1][ 9][17]=	      0;PhiROImax[1][ 9][17]=	       0;
     EtaROImin[1][ 9][18]=   0.756521;EtaROImax[1][ 9][18]=   0.837822;PhiROImin[1][ 9][18]=	1.79045;PhiROImax[1][ 9][18]=	 1.87312;
     EtaROImin[1][ 9][19]=	    0;EtaROImax[1][ 9][19]=	     0;PhiROImin[1][ 9][19]=	      0;PhiROImax[1][ 9][19]=	       0;
     EtaROImin[1][ 9][20]=   0.844116;EtaROImax[1][ 9][20]=   0.903324;PhiROImin[1][ 9][20]=	1.87675;PhiROImax[1][ 9][20]=	 1.96071;
     EtaROImin[1][ 9][21]=   0.899344;EtaROImax[1][ 9][21]=   0.956037;PhiROImin[1][ 9][21]=	1.87675;PhiROImax[1][ 9][21]=	 1.96071;
     EtaROImin[1][ 9][22]=   0.844116;EtaROImax[1][ 9][22]=   0.903324;PhiROImin[1][ 9][22]=	1.79045;PhiROImax[1][ 9][22]=	 1.87312;
     EtaROImin[1][ 9][23]=   0.899344;EtaROImax[1][ 9][23]=   0.956037;PhiROImin[1][ 9][23]=	1.79045;PhiROImax[1][ 9][23]=	 1.87312;
     EtaROImin[1][ 9][24]=	    0;EtaROImax[1][ 9][24]=	     0;PhiROImin[1][ 9][24]=	      0;PhiROImax[1][ 9][24]=	       0;
     EtaROImin[1][ 9][25]=	    0;EtaROImax[1][ 9][25]=	     0;PhiROImin[1][ 9][25]=	      0;PhiROImax[1][ 9][25]=	       0;
     EtaROImin[1][ 9][26]=	    0;EtaROImax[1][ 9][26]=	     0;PhiROImin[1][ 9][26]=	      0;PhiROImax[1][ 9][26]=	       0;
     EtaROImin[1][ 9][27]=	    0;EtaROImax[1][ 9][27]=	     0;PhiROImin[1][ 9][27]=	      0;PhiROImax[1][ 9][27]=	       0;
     EtaROImin[1][ 9][28]=	    0;EtaROImax[1][ 9][28]=	     0;PhiROImin[1][ 9][28]=	      0;PhiROImax[1][ 9][28]=	       0;
     EtaROImin[1][ 9][29]=	    0;EtaROImax[1][ 9][29]=	     0;PhiROImin[1][ 9][29]=	      0;PhiROImax[1][ 9][29]=	       0;
     EtaROImin[1][ 9][30]=	    0;EtaROImax[1][ 9][30]=	     0;PhiROImin[1][ 9][30]=	      0;PhiROImax[1][ 9][30]=	       0;
     EtaROImin[1][ 9][31]=	    0;EtaROImax[1][ 9][31]=	     0;PhiROImin[1][ 9][31]=	      0;PhiROImax[1][ 9][31]=	       0;
     EtaROImin[1][10][ 0]=   0.116816;EtaROImax[1][10][ 0]=   0.213185;PhiROImin[1][10][ 0]=	1.96629;PhiROImax[1][10][ 0]=	 2.05024;
     EtaROImin[1][10][ 1]=  0.0208251;EtaROImax[1][10][ 1]=   0.118734;PhiROImin[1][10][ 1]=	1.96629;PhiROImax[1][10][ 1]=	 2.05024;
     EtaROImin[1][10][ 2]=   0.116816;EtaROImax[1][10][ 2]=   0.213185;PhiROImin[1][10][ 2]=	2.05387;PhiROImax[1][10][ 2]=	 2.13655;
     EtaROImin[1][10][ 3]=  0.0208251;EtaROImax[1][10][ 3]=   0.118734;PhiROImin[1][10][ 3]=	2.05387;PhiROImax[1][10][ 3]=	 2.13655;
     EtaROImin[1][10][ 4]=    0.30075;EtaROImax[1][10][ 4]=   0.416721;PhiROImin[1][10][ 4]=	1.96629;PhiROImax[1][10][ 4]=	 2.05024;
     EtaROImin[1][10][ 5]=   0.222449;EtaROImax[1][10][ 5]=   0.302928;PhiROImin[1][10][ 5]=	1.96629;PhiROImax[1][10][ 5]=	 2.05024;
     EtaROImin[1][10][ 6]=    0.30075;EtaROImax[1][10][ 6]=   0.416721;PhiROImin[1][10][ 6]=	2.05387;PhiROImax[1][10][ 6]=	 2.13655;
     EtaROImin[1][10][ 7]=   0.222449;EtaROImax[1][10][ 7]=   0.302928;PhiROImin[1][10][ 7]=	2.05387;PhiROImax[1][10][ 7]=	 2.13655;
     EtaROImin[1][10][ 8]=   0.501681;EtaROImax[1][10][ 8]=   0.573871;PhiROImin[1][10][ 8]=	1.96629;PhiROImax[1][10][ 8]=	 2.05024;
     EtaROImin[1][10][ 9]=    0.42967;EtaROImax[1][10][ 9]=   0.504617;PhiROImin[1][10][ 9]=	1.96629;PhiROImax[1][10][ 9]=	 2.05024;
     EtaROImin[1][10][10]=   0.501681;EtaROImax[1][10][10]=   0.573871;PhiROImin[1][10][10]=	2.05387;PhiROImax[1][10][10]=	 2.13655;
     EtaROImin[1][10][11]=    0.42967;EtaROImax[1][10][11]=   0.504617;PhiROImin[1][10][11]=	2.05387;PhiROImax[1][10][11]=	 2.13655;
     EtaROImin[1][10][12]=   0.649934;EtaROImax[1][10][12]=   0.741516;PhiROImin[1][10][12]=	1.96629;PhiROImax[1][10][12]=	 2.05024;
     EtaROImin[1][10][13]=   0.583785;EtaROImax[1][10][13]=   0.653329;PhiROImin[1][10][13]=	1.96629;PhiROImax[1][10][13]=	 2.05024;
     EtaROImin[1][10][14]=   0.649934;EtaROImax[1][10][14]=   0.741516;PhiROImin[1][10][14]=	2.05387;PhiROImax[1][10][14]=	 2.13655;
     EtaROImin[1][10][15]=   0.583785;EtaROImax[1][10][15]=   0.653329;PhiROImin[1][10][15]=	2.05387;PhiROImax[1][10][15]=	 2.13655;
     EtaROImin[1][10][16]=	    0;EtaROImax[1][10][16]=	     0;PhiROImin[1][10][16]=	      0;PhiROImax[1][10][16]=	       0;
     EtaROImin[1][10][17]=   0.756521;EtaROImax[1][10][17]=   0.837822;PhiROImin[1][10][17]=	1.96629;PhiROImax[1][10][17]=	 2.05024;
     EtaROImin[1][10][18]=	    0;EtaROImax[1][10][18]=	     0;PhiROImin[1][10][18]=	      0;PhiROImax[1][10][18]=	       0;
     EtaROImin[1][10][19]=   0.756521;EtaROImax[1][10][19]=   0.837822;PhiROImin[1][10][19]=	2.05387;PhiROImax[1][10][19]=	 2.13655;
     EtaROImin[1][10][20]=   0.899344;EtaROImax[1][10][20]=   0.956037;PhiROImin[1][10][20]=	1.96629;PhiROImax[1][10][20]=	 2.05024;
     EtaROImin[1][10][21]=   0.844116;EtaROImax[1][10][21]=   0.903324;PhiROImin[1][10][21]=	1.96629;PhiROImax[1][10][21]=	 2.05024;
     EtaROImin[1][10][22]=   0.899344;EtaROImax[1][10][22]=   0.956037;PhiROImin[1][10][22]=	2.05387;PhiROImax[1][10][22]=	 2.13655;
     EtaROImin[1][10][23]=   0.844116;EtaROImax[1][10][23]=   0.903324;PhiROImin[1][10][23]=	2.05387;PhiROImax[1][10][23]=	 2.13655;
     EtaROImin[1][10][24]=	    0;EtaROImax[1][10][24]=	     0;PhiROImin[1][10][24]=	      0;PhiROImax[1][10][24]=	       0;
     EtaROImin[1][10][25]=	    0;EtaROImax[1][10][25]=	     0;PhiROImin[1][10][25]=	      0;PhiROImax[1][10][25]=	       0;
     EtaROImin[1][10][26]=	    0;EtaROImax[1][10][26]=	     0;PhiROImin[1][10][26]=	      0;PhiROImax[1][10][26]=	       0;
     EtaROImin[1][10][27]=	    0;EtaROImax[1][10][27]=	     0;PhiROImin[1][10][27]=	      0;PhiROImax[1][10][27]=	       0;
     EtaROImin[1][10][28]=	    0;EtaROImax[1][10][28]=	     0;PhiROImin[1][10][28]=	      0;PhiROImax[1][10][28]=	       0;
     EtaROImin[1][10][29]=	    0;EtaROImax[1][10][29]=	     0;PhiROImin[1][10][29]=	      0;PhiROImax[1][10][29]=	       0;
     EtaROImin[1][10][30]=	    0;EtaROImax[1][10][30]=	     0;PhiROImin[1][10][30]=	      0;PhiROImax[1][10][30]=	       0;
     EtaROImin[1][10][31]=	    0;EtaROImax[1][10][31]=	     0;PhiROImin[1][10][31]=	      0;PhiROImax[1][10][31]=	       0;
     EtaROImin[1][11][ 0]=   0.130922;EtaROImax[1][11][ 0]=   0.238612;PhiROImin[1][11][ 0]=	2.24298;PhiROImax[1][11][ 0]=	 2.35323;
     EtaROImin[1][11][ 1]=	    0;EtaROImax[1][11][ 1]=	     0;PhiROImin[1][11][ 1]=	      0;PhiROImax[1][11][ 1]=	       0;
     EtaROImin[1][11][ 2]=   0.130922;EtaROImax[1][11][ 2]=   0.238612;PhiROImin[1][11][ 2]=	2.13203;PhiROImax[1][11][ 2]=	 2.23946;
     EtaROImin[1][11][ 3]=	    0;EtaROImax[1][11][ 3]=	     0;PhiROImin[1][11][ 3]=	      0;PhiROImax[1][11][ 3]=	       0;
     EtaROImin[1][11][ 4]=   0.246207;EtaROImax[1][11][ 4]=    0.35049;PhiROImin[1][11][ 4]=	2.24358;PhiROImax[1][11][ 4]=	 2.35324;
     EtaROImin[1][11][ 5]=   0.350831;EtaROImax[1][11][ 5]=   0.452099;PhiROImin[1][11][ 5]=	2.24358;PhiROImax[1][11][ 5]=	 2.35324;
     EtaROImin[1][11][ 6]=   0.246207;EtaROImax[1][11][ 6]=    0.35049;PhiROImin[1][11][ 6]=	2.13318;PhiROImax[1][11][ 6]=	 2.24008;
     EtaROImin[1][11][ 7]=   0.350831;EtaROImax[1][11][ 7]=   0.452099;PhiROImin[1][11][ 7]=	2.13318;PhiROImax[1][11][ 7]=	 2.24008;
     EtaROImin[1][11][ 8]=   0.458091;EtaROImax[1][11][ 8]=   0.554444;PhiROImin[1][11][ 8]=	2.24358;PhiROImax[1][11][ 8]=	 2.35324;
     EtaROImin[1][11][ 9]=   0.555789;EtaROImax[1][11][ 9]=   0.648101;PhiROImin[1][11][ 9]=	2.24358;PhiROImax[1][11][ 9]=	 2.35324;
     EtaROImin[1][11][10]=   0.458091;EtaROImax[1][11][10]=   0.554444;PhiROImin[1][11][10]=	2.13318;PhiROImax[1][11][10]=	 2.24008;
     EtaROImin[1][11][11]=   0.555789;EtaROImax[1][11][11]=   0.648101;PhiROImin[1][11][11]=	2.13318;PhiROImax[1][11][11]=	 2.24008;
     EtaROImin[1][11][12]=   0.655857;EtaROImax[1][11][12]=   0.703298;PhiROImin[1][11][12]=	2.24298;PhiROImax[1][11][12]=	 2.35323;
     EtaROImin[1][11][13]=   0.706413;EtaROImax[1][11][13]=   0.776449;PhiROImin[1][11][13]=	2.24298;PhiROImax[1][11][13]=	 2.35323;
     EtaROImin[1][11][14]=   0.655857;EtaROImax[1][11][14]=   0.703298;PhiROImin[1][11][14]=	2.13203;PhiROImax[1][11][14]=	 2.23946;
     EtaROImin[1][11][15]=   0.706413;EtaROImax[1][11][15]=   0.776449;PhiROImin[1][11][15]=	2.13203;PhiROImax[1][11][15]=	 2.23946;
     EtaROImin[1][11][16]=   0.783867;EtaROImax[1][11][16]=    0.82768;PhiROImin[1][11][16]=	2.24298;PhiROImax[1][11][16]=	 2.35323;
     EtaROImin[1][11][17]=   0.830556;EtaROImax[1][11][17]=   0.895163;PhiROImin[1][11][17]=	2.24298;PhiROImax[1][11][17]=	 2.35323;
     EtaROImin[1][11][18]=   0.783867;EtaROImax[1][11][18]=    0.82768;PhiROImin[1][11][18]=	2.13203;PhiROImax[1][11][18]=	 2.23946;
     EtaROImin[1][11][19]=   0.830556;EtaROImax[1][11][19]=   0.895163;PhiROImin[1][11][19]=	2.13203;PhiROImax[1][11][19]=	 2.23946;
     EtaROImin[1][11][20]=   0.898201;EtaROImax[1][11][20]=   0.961344;PhiROImin[1][11][20]=	2.24358;PhiROImax[1][11][20]=	 2.35324;
     EtaROImin[1][11][21]=   0.964674;EtaROImax[1][11][21]=    1.02514;PhiROImin[1][11][21]=	2.24358;PhiROImax[1][11][21]=	 2.35324;
     EtaROImin[1][11][22]=   0.898201;EtaROImax[1][11][22]=   0.961344;PhiROImin[1][11][22]=	2.13318;PhiROImax[1][11][22]=	 2.24008;
     EtaROImin[1][11][23]=   0.964674;EtaROImax[1][11][23]=    1.02514;PhiROImin[1][11][23]=	2.13318;PhiROImax[1][11][23]=	 2.24008;
     EtaROImin[1][11][24]=    1.03003;EtaROImax[1][11][24]=    1.06547;PhiROImin[1][11][24]=	2.24277;PhiROImax[1][11][24]=	 2.35313;
     EtaROImin[1][11][25]=	    0;EtaROImax[1][11][25]=	     0;PhiROImin[1][11][25]=	      0;PhiROImax[1][11][25]=	       0;
     EtaROImin[1][11][26]=    1.03003;EtaROImax[1][11][26]=    1.06547;PhiROImin[1][11][26]=	2.15905;PhiROImax[1][11][26]=	 2.23924;
     EtaROImin[1][11][27]=	    0;EtaROImax[1][11][27]=	     0;PhiROImin[1][11][27]=	      0;PhiROImax[1][11][27]=	       0;
     EtaROImin[1][11][28]=	    0;EtaROImax[1][11][28]=	     0;PhiROImin[1][11][28]=	      0;PhiROImax[1][11][28]=	       0;
     EtaROImin[1][11][29]=	    0;EtaROImax[1][11][29]=	     0;PhiROImin[1][11][29]=	      0;PhiROImax[1][11][29]=	       0;
     EtaROImin[1][11][30]=	    0;EtaROImax[1][11][30]=	     0;PhiROImin[1][11][30]=	      0;PhiROImax[1][11][30]=	       0;
     EtaROImin[1][11][31]=	    0;EtaROImax[1][11][31]=	     0;PhiROImin[1][11][31]=	      0;PhiROImax[1][11][31]=	       0;
     EtaROImin[1][12][ 0]=	    0;EtaROImax[1][12][ 0]=	     0;PhiROImin[1][12][ 0]=	      0;PhiROImax[1][12][ 0]=	       0;
     EtaROImin[1][12][ 1]=   0.130922;EtaROImax[1][12][ 1]=   0.238612;PhiROImin[1][12][ 1]=	2.35916;PhiROImax[1][12][ 1]=	 2.46941;
     EtaROImin[1][12][ 2]=	    0;EtaROImax[1][12][ 2]=	     0;PhiROImin[1][12][ 2]=	      0;PhiROImax[1][12][ 2]=	       0;
     EtaROImin[1][12][ 3]=   0.130922;EtaROImax[1][12][ 3]=   0.238612;PhiROImin[1][12][ 3]=	2.47293;PhiROImax[1][12][ 3]=	 2.58036;
     EtaROImin[1][12][ 4]=   0.350831;EtaROImax[1][12][ 4]=   0.452099;PhiROImin[1][12][ 4]=	2.35915;PhiROImax[1][12][ 4]=	 2.46881;
     EtaROImin[1][12][ 5]=   0.246207;EtaROImax[1][12][ 5]=    0.35049;PhiROImin[1][12][ 5]=	2.35915;PhiROImax[1][12][ 5]=	 2.46881;
     EtaROImin[1][12][ 6]=   0.350831;EtaROImax[1][12][ 6]=   0.452099;PhiROImin[1][12][ 6]=	2.47231;PhiROImax[1][12][ 6]=	 2.57921;
     EtaROImin[1][12][ 7]=   0.246207;EtaROImax[1][12][ 7]=    0.35049;PhiROImin[1][12][ 7]=	2.47231;PhiROImax[1][12][ 7]=	 2.57921;
     EtaROImin[1][12][ 8]=   0.555789;EtaROImax[1][12][ 8]=   0.648101;PhiROImin[1][12][ 8]=	2.35915;PhiROImax[1][12][ 8]=	 2.46881;
     EtaROImin[1][12][ 9]=   0.458091;EtaROImax[1][12][ 9]=   0.554444;PhiROImin[1][12][ 9]=	2.35915;PhiROImax[1][12][ 9]=	 2.46881;
     EtaROImin[1][12][10]=   0.555789;EtaROImax[1][12][10]=   0.648101;PhiROImin[1][12][10]=	2.47231;PhiROImax[1][12][10]=	 2.57921;
     EtaROImin[1][12][11]=   0.458091;EtaROImax[1][12][11]=   0.554444;PhiROImin[1][12][11]=	2.47231;PhiROImax[1][12][11]=	 2.57921;
     EtaROImin[1][12][12]=   0.731124;EtaROImax[1][12][12]=   0.776449;PhiROImin[1][12][12]=	2.35916;PhiROImax[1][12][12]=	 2.46941;
     EtaROImin[1][12][13]=   0.655857;EtaROImax[1][12][13]=   0.728056;PhiROImin[1][12][13]=	2.35916;PhiROImax[1][12][13]=	 2.46941;
     EtaROImin[1][12][14]=   0.731124;EtaROImax[1][12][14]=   0.776449;PhiROImin[1][12][14]=	2.47293;PhiROImax[1][12][14]=	 2.58036;
     EtaROImin[1][12][15]=   0.655857;EtaROImax[1][12][15]=   0.728056;PhiROImin[1][12][15]=	2.47293;PhiROImax[1][12][15]=	 2.58036;
     EtaROImin[1][12][16]=   0.853359;EtaROImax[1][12][16]=   0.895163;PhiROImin[1][12][16]=	2.35916;PhiROImax[1][12][16]=	 2.46941;
     EtaROImin[1][12][17]=   0.783867;EtaROImax[1][12][17]=   0.850528;PhiROImin[1][12][17]=	2.35916;PhiROImax[1][12][17]=	 2.46941;
     EtaROImin[1][12][18]=   0.853359;EtaROImax[1][12][18]=   0.895163;PhiROImin[1][12][18]=	2.47293;PhiROImax[1][12][18]=	 2.58036;
     EtaROImin[1][12][19]=   0.783867;EtaROImax[1][12][19]=   0.850528;PhiROImin[1][12][19]=	2.47293;PhiROImax[1][12][19]=	 2.58036;
     EtaROImin[1][12][20]=   0.964674;EtaROImax[1][12][20]=    1.02514;PhiROImin[1][12][20]=	2.35915;PhiROImax[1][12][20]=	 2.46881;
     EtaROImin[1][12][21]=   0.898201;EtaROImax[1][12][21]=   0.961344;PhiROImin[1][12][21]=	2.35915;PhiROImax[1][12][21]=	 2.46881;
     EtaROImin[1][12][22]=   0.964674;EtaROImax[1][12][22]=    1.02514;PhiROImin[1][12][22]=	2.47231;PhiROImax[1][12][22]=	 2.57921;
     EtaROImin[1][12][23]=   0.898201;EtaROImax[1][12][23]=   0.961344;PhiROImin[1][12][23]=	2.47231;PhiROImax[1][12][23]=	 2.57921;
     EtaROImin[1][12][24]=	    0;EtaROImax[1][12][24]=	     0;PhiROImin[1][12][24]=	      0;PhiROImax[1][12][24]=	       0;
     EtaROImin[1][12][25]=    1.03003;EtaROImax[1][12][25]=    1.06547;PhiROImin[1][12][25]=	2.35926;PhiROImax[1][12][25]=	 2.46962;
     EtaROImin[1][12][26]=	    0;EtaROImax[1][12][26]=	     0;PhiROImin[1][12][26]=	      0;PhiROImax[1][12][26]=	       0;
     EtaROImin[1][12][27]=    1.03003;EtaROImax[1][12][27]=    1.06547;PhiROImin[1][12][27]=	2.47315;PhiROImax[1][12][27]=	 2.55334;
     EtaROImin[1][12][28]=	    0;EtaROImax[1][12][28]=	     0;PhiROImin[1][12][28]=	      0;PhiROImax[1][12][28]=	       0;
     EtaROImin[1][12][29]=	    0;EtaROImax[1][12][29]=	     0;PhiROImin[1][12][29]=	      0;PhiROImax[1][12][29]=	       0;
     EtaROImin[1][12][30]=	    0;EtaROImax[1][12][30]=	     0;PhiROImin[1][12][30]=	      0;PhiROImax[1][12][30]=	       0;
     EtaROImin[1][12][31]=	    0;EtaROImax[1][12][31]=	     0;PhiROImin[1][12][31]=	      0;PhiROImax[1][12][31]=	       0;
     EtaROImin[1][13][ 0]=  0.0208251;EtaROImax[1][13][ 0]=   0.118734;PhiROImin[1][13][ 0]=	2.66215;PhiROImax[1][13][ 0]=	  2.7461;
     EtaROImin[1][13][ 1]=   0.116816;EtaROImax[1][13][ 1]=   0.213185;PhiROImin[1][13][ 1]=	2.66215;PhiROImax[1][13][ 1]=	  2.7461;
     EtaROImin[1][13][ 2]=  0.0208251;EtaROImax[1][13][ 2]=   0.118734;PhiROImin[1][13][ 2]=	2.57584;PhiROImax[1][13][ 2]=	 2.65852;
     EtaROImin[1][13][ 3]=   0.116816;EtaROImax[1][13][ 3]=   0.213185;PhiROImin[1][13][ 3]=	2.57584;PhiROImax[1][13][ 3]=	 2.65852;
     EtaROImin[1][13][ 4]=   0.222449;EtaROImax[1][13][ 4]=   0.302928;PhiROImin[1][13][ 4]=	2.66215;PhiROImax[1][13][ 4]=	  2.7461;
     EtaROImin[1][13][ 5]=    0.30075;EtaROImax[1][13][ 5]=   0.416721;PhiROImin[1][13][ 5]=	2.66215;PhiROImax[1][13][ 5]=	  2.7461;
     EtaROImin[1][13][ 6]=   0.222449;EtaROImax[1][13][ 6]=   0.302928;PhiROImin[1][13][ 6]=	2.57584;PhiROImax[1][13][ 6]=	 2.65852;
     EtaROImin[1][13][ 7]=    0.30075;EtaROImax[1][13][ 7]=   0.416721;PhiROImin[1][13][ 7]=	2.57584;PhiROImax[1][13][ 7]=	 2.65852;
     EtaROImin[1][13][ 8]=    0.42967;EtaROImax[1][13][ 8]=   0.504617;PhiROImin[1][13][ 8]=	2.66215;PhiROImax[1][13][ 8]=	  2.7461;
     EtaROImin[1][13][ 9]=   0.501681;EtaROImax[1][13][ 9]=   0.573871;PhiROImin[1][13][ 9]=	2.66215;PhiROImax[1][13][ 9]=	  2.7461;
     EtaROImin[1][13][10]=    0.42967;EtaROImax[1][13][10]=   0.504617;PhiROImin[1][13][10]=	2.57584;PhiROImax[1][13][10]=	 2.65852;
     EtaROImin[1][13][11]=   0.501681;EtaROImax[1][13][11]=   0.573871;PhiROImin[1][13][11]=	2.57584;PhiROImax[1][13][11]=	 2.65852;
     EtaROImin[1][13][12]=   0.583785;EtaROImax[1][13][12]=   0.653329;PhiROImin[1][13][12]=	2.66215;PhiROImax[1][13][12]=	  2.7461;
     EtaROImin[1][13][13]=   0.649934;EtaROImax[1][13][13]=   0.741516;PhiROImin[1][13][13]=	2.66215;PhiROImax[1][13][13]=	  2.7461;
     EtaROImin[1][13][14]=   0.583785;EtaROImax[1][13][14]=   0.653329;PhiROImin[1][13][14]=	2.57584;PhiROImax[1][13][14]=	 2.65852;
     EtaROImin[1][13][15]=   0.649934;EtaROImax[1][13][15]=   0.741516;PhiROImin[1][13][15]=	2.57584;PhiROImax[1][13][15]=	 2.65852;
     EtaROImin[1][13][16]=   0.756521;EtaROImax[1][13][16]=   0.837822;PhiROImin[1][13][16]=	2.66215;PhiROImax[1][13][16]=	  2.7461;
     EtaROImin[1][13][17]=	    0;EtaROImax[1][13][17]=	     0;PhiROImin[1][13][17]=	      0;PhiROImax[1][13][17]=	       0;
     EtaROImin[1][13][18]=   0.756521;EtaROImax[1][13][18]=   0.837822;PhiROImin[1][13][18]=	2.57584;PhiROImax[1][13][18]=	 2.65852;
     EtaROImin[1][13][19]=	    0;EtaROImax[1][13][19]=	     0;PhiROImin[1][13][19]=	      0;PhiROImax[1][13][19]=	       0;
     EtaROImin[1][13][20]=   0.844116;EtaROImax[1][13][20]=   0.903324;PhiROImin[1][13][20]=	2.66215;PhiROImax[1][13][20]=	  2.7461;
     EtaROImin[1][13][21]=   0.899344;EtaROImax[1][13][21]=   0.956037;PhiROImin[1][13][21]=	2.66215;PhiROImax[1][13][21]=	  2.7461;
     EtaROImin[1][13][22]=   0.844116;EtaROImax[1][13][22]=   0.903324;PhiROImin[1][13][22]=	2.57584;PhiROImax[1][13][22]=	 2.65852;
     EtaROImin[1][13][23]=   0.899344;EtaROImax[1][13][23]=   0.956037;PhiROImin[1][13][23]=	2.57584;PhiROImax[1][13][23]=	 2.65852;
     EtaROImin[1][13][24]=	    0;EtaROImax[1][13][24]=	     0;PhiROImin[1][13][24]=	      0;PhiROImax[1][13][24]=	       0;
     EtaROImin[1][13][25]=	    0;EtaROImax[1][13][25]=	     0;PhiROImin[1][13][25]=	      0;PhiROImax[1][13][25]=	       0;
     EtaROImin[1][13][26]=	    0;EtaROImax[1][13][26]=	     0;PhiROImin[1][13][26]=	      0;PhiROImax[1][13][26]=	       0;
     EtaROImin[1][13][27]=	    0;EtaROImax[1][13][27]=	     0;PhiROImin[1][13][27]=	      0;PhiROImax[1][13][27]=	       0;
     EtaROImin[1][13][28]=	    0;EtaROImax[1][13][28]=	     0;PhiROImin[1][13][28]=	      0;PhiROImax[1][13][28]=	       0;
     EtaROImin[1][13][29]=	    0;EtaROImax[1][13][29]=	     0;PhiROImin[1][13][29]=	      0;PhiROImax[1][13][29]=	       0;
     EtaROImin[1][13][30]=	    0;EtaROImax[1][13][30]=	     0;PhiROImin[1][13][30]=	      0;PhiROImax[1][13][30]=	       0;
     EtaROImin[1][13][31]=	    0;EtaROImax[1][13][31]=	     0;PhiROImin[1][13][31]=	      0;PhiROImax[1][13][31]=	       0;
     EtaROImin[1][14][ 0]=   0.116816;EtaROImax[1][14][ 0]=   0.213185;PhiROImin[1][14][ 0]=	2.75168;PhiROImax[1][14][ 0]=	 2.83564;
     EtaROImin[1][14][ 1]=  0.0208251;EtaROImax[1][14][ 1]=   0.118734;PhiROImin[1][14][ 1]=	2.75168;PhiROImax[1][14][ 1]=	 2.83564;
     EtaROImin[1][14][ 2]=   0.116816;EtaROImax[1][14][ 2]=   0.213185;PhiROImin[1][14][ 2]=	2.83927;PhiROImax[1][14][ 2]=	 2.92194;
     EtaROImin[1][14][ 3]=  0.0208251;EtaROImax[1][14][ 3]=   0.118734;PhiROImin[1][14][ 3]=	2.83927;PhiROImax[1][14][ 3]=	 2.92194;
     EtaROImin[1][14][ 4]=    0.30075;EtaROImax[1][14][ 4]=   0.416721;PhiROImin[1][14][ 4]=	2.75168;PhiROImax[1][14][ 4]=	 2.83564;
     EtaROImin[1][14][ 5]=   0.222449;EtaROImax[1][14][ 5]=   0.302928;PhiROImin[1][14][ 5]=	2.75168;PhiROImax[1][14][ 5]=	 2.83564;
     EtaROImin[1][14][ 6]=    0.30075;EtaROImax[1][14][ 6]=   0.416721;PhiROImin[1][14][ 6]=	2.83927;PhiROImax[1][14][ 6]=	 2.92194;
     EtaROImin[1][14][ 7]=   0.222449;EtaROImax[1][14][ 7]=   0.302928;PhiROImin[1][14][ 7]=	2.83927;PhiROImax[1][14][ 7]=	 2.92194;
     EtaROImin[1][14][ 8]=   0.501681;EtaROImax[1][14][ 8]=   0.573871;PhiROImin[1][14][ 8]=	2.75168;PhiROImax[1][14][ 8]=	 2.83564;
     EtaROImin[1][14][ 9]=    0.42967;EtaROImax[1][14][ 9]=   0.504617;PhiROImin[1][14][ 9]=	2.75168;PhiROImax[1][14][ 9]=	 2.83564;
     EtaROImin[1][14][10]=   0.501681;EtaROImax[1][14][10]=   0.573871;PhiROImin[1][14][10]=	2.83927;PhiROImax[1][14][10]=	 2.92194;
     EtaROImin[1][14][11]=    0.42967;EtaROImax[1][14][11]=   0.504617;PhiROImin[1][14][11]=	2.83927;PhiROImax[1][14][11]=	 2.92194;
     EtaROImin[1][14][12]=   0.649934;EtaROImax[1][14][12]=   0.741516;PhiROImin[1][14][12]=	2.75168;PhiROImax[1][14][12]=	 2.83564;
     EtaROImin[1][14][13]=   0.583785;EtaROImax[1][14][13]=   0.653329;PhiROImin[1][14][13]=	2.75168;PhiROImax[1][14][13]=	 2.83564;
     EtaROImin[1][14][14]=   0.649934;EtaROImax[1][14][14]=   0.741516;PhiROImin[1][14][14]=	2.83927;PhiROImax[1][14][14]=	 2.92194;
     EtaROImin[1][14][15]=   0.583785;EtaROImax[1][14][15]=   0.653329;PhiROImin[1][14][15]=	2.83927;PhiROImax[1][14][15]=	 2.92194;
     EtaROImin[1][14][16]=	    0;EtaROImax[1][14][16]=	     0;PhiROImin[1][14][16]=	      0;PhiROImax[1][14][16]=	       0;
     EtaROImin[1][14][17]=   0.756521;EtaROImax[1][14][17]=   0.837822;PhiROImin[1][14][17]=	2.75168;PhiROImax[1][14][17]=	 2.83564;
     EtaROImin[1][14][18]=	    0;EtaROImax[1][14][18]=	     0;PhiROImin[1][14][18]=	      0;PhiROImax[1][14][18]=	       0;
     EtaROImin[1][14][19]=   0.756521;EtaROImax[1][14][19]=   0.837822;PhiROImin[1][14][19]=	2.83927;PhiROImax[1][14][19]=	 2.92194;
     EtaROImin[1][14][20]=   0.899344;EtaROImax[1][14][20]=   0.956037;PhiROImin[1][14][20]=	2.75168;PhiROImax[1][14][20]=	 2.83564;
     EtaROImin[1][14][21]=   0.844116;EtaROImax[1][14][21]=   0.903324;PhiROImin[1][14][21]=	2.75168;PhiROImax[1][14][21]=	 2.83564;
     EtaROImin[1][14][22]=   0.899344;EtaROImax[1][14][22]=   0.956037;PhiROImin[1][14][22]=	2.83927;PhiROImax[1][14][22]=	 2.92194;
     EtaROImin[1][14][23]=   0.844116;EtaROImax[1][14][23]=   0.903324;PhiROImin[1][14][23]=	2.83927;PhiROImax[1][14][23]=	 2.92194;
     EtaROImin[1][14][24]=	    0;EtaROImax[1][14][24]=	     0;PhiROImin[1][14][24]=	      0;PhiROImax[1][14][24]=	       0;
     EtaROImin[1][14][25]=	    0;EtaROImax[1][14][25]=	     0;PhiROImin[1][14][25]=	      0;PhiROImax[1][14][25]=	       0;
     EtaROImin[1][14][26]=	    0;EtaROImax[1][14][26]=	     0;PhiROImin[1][14][26]=	      0;PhiROImax[1][14][26]=	       0;
     EtaROImin[1][14][27]=	    0;EtaROImax[1][14][27]=	     0;PhiROImin[1][14][27]=	      0;PhiROImax[1][14][27]=	       0;
     EtaROImin[1][14][28]=	    0;EtaROImax[1][14][28]=	     0;PhiROImin[1][14][28]=	      0;PhiROImax[1][14][28]=	       0;
     EtaROImin[1][14][29]=	    0;EtaROImax[1][14][29]=	     0;PhiROImin[1][14][29]=	      0;PhiROImax[1][14][29]=	       0;
     EtaROImin[1][14][30]=	    0;EtaROImax[1][14][30]=	     0;PhiROImin[1][14][30]=	      0;PhiROImax[1][14][30]=	       0;
     EtaROImin[1][14][31]=	    0;EtaROImax[1][14][31]=	     0;PhiROImin[1][14][31]=	      0;PhiROImax[1][14][31]=	       0;
     EtaROImin[1][15][ 0]=   0.130922;EtaROImax[1][15][ 0]=   0.238612;PhiROImin[1][15][ 0]=	3.02838;PhiROImax[1][15][ 0]=	 3.13862;
     EtaROImin[1][15][ 1]=	    0;EtaROImax[1][15][ 1]=	     0;PhiROImin[1][15][ 1]=	      0;PhiROImax[1][15][ 1]=	       0;
     EtaROImin[1][15][ 2]=   0.130922;EtaROImax[1][15][ 2]=   0.238612;PhiROImin[1][15][ 2]=	2.91742;PhiROImax[1][15][ 2]=	 3.02486;
     EtaROImin[1][15][ 3]=	    0;EtaROImax[1][15][ 3]=	     0;PhiROImin[1][15][ 3]=	      0;PhiROImax[1][15][ 3]=	       0;
     EtaROImin[1][15][ 4]=   0.246207;EtaROImax[1][15][ 4]=    0.35049;PhiROImin[1][15][ 4]=	3.02898;PhiROImax[1][15][ 4]=	 3.13864;
     EtaROImin[1][15][ 5]=   0.350831;EtaROImax[1][15][ 5]=   0.452099;PhiROImin[1][15][ 5]=	3.02898;PhiROImax[1][15][ 5]=	 3.13864;
     EtaROImin[1][15][ 6]=   0.246207;EtaROImax[1][15][ 6]=    0.35049;PhiROImin[1][15][ 6]=	2.91858;PhiROImax[1][15][ 6]=	 3.02547;
     EtaROImin[1][15][ 7]=   0.350831;EtaROImax[1][15][ 7]=   0.452099;PhiROImin[1][15][ 7]=	2.91858;PhiROImax[1][15][ 7]=	 3.02547;
     EtaROImin[1][15][ 8]=   0.458091;EtaROImax[1][15][ 8]=   0.554444;PhiROImin[1][15][ 8]=	3.02898;PhiROImax[1][15][ 8]=	 3.13864;
     EtaROImin[1][15][ 9]=   0.555789;EtaROImax[1][15][ 9]=   0.648101;PhiROImin[1][15][ 9]=	3.02898;PhiROImax[1][15][ 9]=	 3.13864;
     EtaROImin[1][15][10]=   0.458091;EtaROImax[1][15][10]=   0.554444;PhiROImin[1][15][10]=	2.91858;PhiROImax[1][15][10]=	 3.02547;
     EtaROImin[1][15][11]=   0.555789;EtaROImax[1][15][11]=   0.648101;PhiROImin[1][15][11]=	2.91858;PhiROImax[1][15][11]=	 3.02547;
     EtaROImin[1][15][12]=   0.655857;EtaROImax[1][15][12]=   0.703298;PhiROImin[1][15][12]=	3.02838;PhiROImax[1][15][12]=	 3.13862;
     EtaROImin[1][15][13]=   0.706413;EtaROImax[1][15][13]=   0.776449;PhiROImin[1][15][13]=	3.02838;PhiROImax[1][15][13]=	 3.13862;
     EtaROImin[1][15][14]=   0.655857;EtaROImax[1][15][14]=   0.703298;PhiROImin[1][15][14]=	2.91742;PhiROImax[1][15][14]=	 3.02486;
     EtaROImin[1][15][15]=   0.706413;EtaROImax[1][15][15]=   0.776449;PhiROImin[1][15][15]=	2.91742;PhiROImax[1][15][15]=	 3.02486;
     EtaROImin[1][15][16]=   0.783867;EtaROImax[1][15][16]=    0.82768;PhiROImin[1][15][16]=	3.02838;PhiROImax[1][15][16]=	 3.13862;
     EtaROImin[1][15][17]=   0.830556;EtaROImax[1][15][17]=   0.895163;PhiROImin[1][15][17]=	3.02838;PhiROImax[1][15][17]=	 3.13862;
     EtaROImin[1][15][18]=   0.783867;EtaROImax[1][15][18]=    0.82768;PhiROImin[1][15][18]=	2.91742;PhiROImax[1][15][18]=	 3.02486;
     EtaROImin[1][15][19]=   0.830556;EtaROImax[1][15][19]=   0.895163;PhiROImin[1][15][19]=	2.91742;PhiROImax[1][15][19]=	 3.02486;
     EtaROImin[1][15][20]=   0.898201;EtaROImax[1][15][20]=   0.961344;PhiROImin[1][15][20]=	3.02898;PhiROImax[1][15][20]=	 3.13864;
     EtaROImin[1][15][21]=   0.964674;EtaROImax[1][15][21]=    1.02514;PhiROImin[1][15][21]=	3.02898;PhiROImax[1][15][21]=	 3.13864;
     EtaROImin[1][15][22]=   0.898201;EtaROImax[1][15][22]=   0.961344;PhiROImin[1][15][22]=	2.91858;PhiROImax[1][15][22]=	 3.02547;
     EtaROImin[1][15][23]=   0.964674;EtaROImax[1][15][23]=    1.02514;PhiROImin[1][15][23]=	2.91858;PhiROImax[1][15][23]=	 3.02547;
     EtaROImin[1][15][24]=    1.03003;EtaROImax[1][15][24]=    1.06547;PhiROImin[1][15][24]=	3.02816;PhiROImax[1][15][24]=	 3.13852;
     EtaROImin[1][15][25]=	    0;EtaROImax[1][15][25]=	     0;PhiROImin[1][15][25]=	      0;PhiROImax[1][15][25]=	       0;
     EtaROImin[1][15][26]=    1.03003;EtaROImax[1][15][26]=    1.06547;PhiROImin[1][15][26]=	2.94445;PhiROImax[1][15][26]=	 3.02463;
     EtaROImin[1][15][27]=	    0;EtaROImax[1][15][27]=	     0;PhiROImin[1][15][27]=	      0;PhiROImax[1][15][27]=	       0;
     EtaROImin[1][15][28]=	    0;EtaROImax[1][15][28]=	     0;PhiROImin[1][15][28]=	      0;PhiROImax[1][15][28]=	       0;
     EtaROImin[1][15][29]=	    0;EtaROImax[1][15][29]=	     0;PhiROImin[1][15][29]=	      0;PhiROImax[1][15][29]=	       0;
     EtaROImin[1][15][30]=	    0;EtaROImax[1][15][30]=	     0;PhiROImin[1][15][30]=	      0;PhiROImax[1][15][30]=	       0;
     EtaROImin[1][15][31]=	    0;EtaROImax[1][15][31]=	     0;PhiROImin[1][15][31]=	      0;PhiROImax[1][15][31]=	       0;
     EtaROImin[1][16][ 0]=	    0;EtaROImax[1][16][ 0]=	     0;PhiROImin[1][16][ 0]=	      0;PhiROImax[1][16][ 0]=	       0;
     EtaROImin[1][16][ 1]=   0.130922;EtaROImax[1][16][ 1]=   0.238612;PhiROImin[1][16][ 1]=   -3.13862;PhiROImax[1][16][ 1]=	-3.02838;
     EtaROImin[1][16][ 2]=	    0;EtaROImax[1][16][ 2]=	     0;PhiROImin[1][16][ 2]=	      0;PhiROImax[1][16][ 2]=	       0;
     EtaROImin[1][16][ 3]=   0.130922;EtaROImax[1][16][ 3]=   0.238612;PhiROImin[1][16][ 3]=   -3.02486;PhiROImax[1][16][ 3]=	-2.91742;
     EtaROImin[1][16][ 4]=   0.350831;EtaROImax[1][16][ 4]=   0.452099;PhiROImin[1][16][ 4]=   -3.13864;PhiROImax[1][16][ 4]=	-3.02898;
     EtaROImin[1][16][ 5]=   0.246207;EtaROImax[1][16][ 5]=    0.35049;PhiROImin[1][16][ 5]=   -3.13864;PhiROImax[1][16][ 5]=	-3.02898;
     EtaROImin[1][16][ 6]=   0.350831;EtaROImax[1][16][ 6]=   0.452099;PhiROImin[1][16][ 6]=   -3.02547;PhiROImax[1][16][ 6]=	-2.91858;
     EtaROImin[1][16][ 7]=   0.246207;EtaROImax[1][16][ 7]=    0.35049;PhiROImin[1][16][ 7]=   -3.02547;PhiROImax[1][16][ 7]=	-2.91858;
     EtaROImin[1][16][ 8]=   0.555789;EtaROImax[1][16][ 8]=   0.648101;PhiROImin[1][16][ 8]=   -3.13864;PhiROImax[1][16][ 8]=	-3.02898;
     EtaROImin[1][16][ 9]=   0.458091;EtaROImax[1][16][ 9]=   0.554444;PhiROImin[1][16][ 9]=   -3.13864;PhiROImax[1][16][ 9]=	-3.02898;
     EtaROImin[1][16][10]=   0.555789;EtaROImax[1][16][10]=   0.648101;PhiROImin[1][16][10]=   -3.02547;PhiROImax[1][16][10]=	-2.91858;
     EtaROImin[1][16][11]=   0.458091;EtaROImax[1][16][11]=   0.554444;PhiROImin[1][16][11]=   -3.02547;PhiROImax[1][16][11]=	-2.91858;
     EtaROImin[1][16][12]=   0.731124;EtaROImax[1][16][12]=   0.776449;PhiROImin[1][16][12]=   -3.13862;PhiROImax[1][16][12]=	-3.02838;
     EtaROImin[1][16][13]=   0.655857;EtaROImax[1][16][13]=   0.728056;PhiROImin[1][16][13]=   -3.13862;PhiROImax[1][16][13]=	-3.02838;
     EtaROImin[1][16][14]=   0.731124;EtaROImax[1][16][14]=   0.776449;PhiROImin[1][16][14]=   -3.02486;PhiROImax[1][16][14]=	-2.91742;
     EtaROImin[1][16][15]=   0.655857;EtaROImax[1][16][15]=   0.728056;PhiROImin[1][16][15]=   -3.02486;PhiROImax[1][16][15]=	-2.91742;
     EtaROImin[1][16][16]=   0.853359;EtaROImax[1][16][16]=   0.895163;PhiROImin[1][16][16]=   -3.13862;PhiROImax[1][16][16]=	-3.02838;
     EtaROImin[1][16][17]=   0.783867;EtaROImax[1][16][17]=   0.850528;PhiROImin[1][16][17]=   -3.13862;PhiROImax[1][16][17]=	-3.02838;
     EtaROImin[1][16][18]=   0.853359;EtaROImax[1][16][18]=   0.895163;PhiROImin[1][16][18]=   -3.02486;PhiROImax[1][16][18]=	-2.91742;
     EtaROImin[1][16][19]=   0.783867;EtaROImax[1][16][19]=   0.850528;PhiROImin[1][16][19]=   -3.02486;PhiROImax[1][16][19]=	-2.91742;
     EtaROImin[1][16][20]=   0.964674;EtaROImax[1][16][20]=    1.02514;PhiROImin[1][16][20]=   -3.13864;PhiROImax[1][16][20]=	-3.02898;
     EtaROImin[1][16][21]=   0.898201;EtaROImax[1][16][21]=   0.961344;PhiROImin[1][16][21]=   -3.13864;PhiROImax[1][16][21]=	-3.02898;
     EtaROImin[1][16][22]=   0.964674;EtaROImax[1][16][22]=    1.02514;PhiROImin[1][16][22]=   -3.02547;PhiROImax[1][16][22]=	-2.91858;
     EtaROImin[1][16][23]=   0.898201;EtaROImax[1][16][23]=   0.961344;PhiROImin[1][16][23]=   -3.02547;PhiROImax[1][16][23]=	-2.91858;
     EtaROImin[1][16][24]=	    0;EtaROImax[1][16][24]=	     0;PhiROImin[1][16][24]=	      0;PhiROImax[1][16][24]=	       0;
     EtaROImin[1][16][25]=    1.03003;EtaROImax[1][16][25]=    1.06547;PhiROImin[1][16][25]=   -3.13852;PhiROImax[1][16][25]=	-3.02816;
     EtaROImin[1][16][26]=	    0;EtaROImax[1][16][26]=	     0;PhiROImin[1][16][26]=	      0;PhiROImax[1][16][26]=	       0;
     EtaROImin[1][16][27]=    1.03003;EtaROImax[1][16][27]=    1.06547;PhiROImin[1][16][27]=   -3.02463;PhiROImax[1][16][27]=	-2.94445;
     EtaROImin[1][16][28]=	    0;EtaROImax[1][16][28]=	     0;PhiROImin[1][16][28]=	      0;PhiROImax[1][16][28]=	       0;
     EtaROImin[1][16][29]=	    0;EtaROImax[1][16][29]=	     0;PhiROImin[1][16][29]=	      0;PhiROImax[1][16][29]=	       0;
     EtaROImin[1][16][30]=	    0;EtaROImax[1][16][30]=	     0;PhiROImin[1][16][30]=	      0;PhiROImax[1][16][30]=	       0;
     EtaROImin[1][16][31]=	    0;EtaROImax[1][16][31]=	     0;PhiROImin[1][16][31]=	      0;PhiROImax[1][16][31]=	       0;
     EtaROImin[1][17][ 0]=  0.0208251;EtaROImax[1][17][ 0]=   0.118734;PhiROImin[1][17][ 0]=   -2.83564;PhiROImax[1][17][ 0]=	-2.75168;
     EtaROImin[1][17][ 1]=   0.116816;EtaROImax[1][17][ 1]=   0.213185;PhiROImin[1][17][ 1]=   -2.83564;PhiROImax[1][17][ 1]=	-2.75168;
     EtaROImin[1][17][ 2]=  0.0208251;EtaROImax[1][17][ 2]=   0.118734;PhiROImin[1][17][ 2]=   -2.92194;PhiROImax[1][17][ 2]=	-2.83927;
     EtaROImin[1][17][ 3]=   0.116816;EtaROImax[1][17][ 3]=   0.213185;PhiROImin[1][17][ 3]=   -2.92194;PhiROImax[1][17][ 3]=	-2.83927;
     EtaROImin[1][17][ 4]=   0.222449;EtaROImax[1][17][ 4]=   0.302928;PhiROImin[1][17][ 4]=   -2.83564;PhiROImax[1][17][ 4]=	-2.75168;
     EtaROImin[1][17][ 5]=    0.30075;EtaROImax[1][17][ 5]=   0.416721;PhiROImin[1][17][ 5]=   -2.83564;PhiROImax[1][17][ 5]=	-2.75168;
     EtaROImin[1][17][ 6]=   0.222449;EtaROImax[1][17][ 6]=   0.302928;PhiROImin[1][17][ 6]=   -2.92194;PhiROImax[1][17][ 6]=	-2.83927;
     EtaROImin[1][17][ 7]=    0.30075;EtaROImax[1][17][ 7]=   0.416721;PhiROImin[1][17][ 7]=   -2.92194;PhiROImax[1][17][ 7]=	-2.83927;
     EtaROImin[1][17][ 8]=    0.42967;EtaROImax[1][17][ 8]=   0.504617;PhiROImin[1][17][ 8]=   -2.83564;PhiROImax[1][17][ 8]=	-2.75168;
     EtaROImin[1][17][ 9]=   0.501681;EtaROImax[1][17][ 9]=   0.573871;PhiROImin[1][17][ 9]=   -2.83564;PhiROImax[1][17][ 9]=	-2.75168;
     EtaROImin[1][17][10]=    0.42967;EtaROImax[1][17][10]=   0.504617;PhiROImin[1][17][10]=   -2.92194;PhiROImax[1][17][10]=	-2.83927;
     EtaROImin[1][17][11]=   0.501681;EtaROImax[1][17][11]=   0.573871;PhiROImin[1][17][11]=   -2.92194;PhiROImax[1][17][11]=	-2.83927;
     EtaROImin[1][17][12]=   0.583785;EtaROImax[1][17][12]=   0.653329;PhiROImin[1][17][12]=   -2.83564;PhiROImax[1][17][12]=	-2.75168;
     EtaROImin[1][17][13]=   0.649934;EtaROImax[1][17][13]=   0.741516;PhiROImin[1][17][13]=   -2.83564;PhiROImax[1][17][13]=	-2.75168;
     EtaROImin[1][17][14]=   0.583785;EtaROImax[1][17][14]=   0.653329;PhiROImin[1][17][14]=   -2.92194;PhiROImax[1][17][14]=	-2.83927;
     EtaROImin[1][17][15]=   0.649934;EtaROImax[1][17][15]=   0.741516;PhiROImin[1][17][15]=   -2.92194;PhiROImax[1][17][15]=	-2.83927;
     EtaROImin[1][17][16]=   0.756521;EtaROImax[1][17][16]=   0.837822;PhiROImin[1][17][16]=   -2.83564;PhiROImax[1][17][16]=	-2.75168;
     EtaROImin[1][17][17]=	    0;EtaROImax[1][17][17]=	     0;PhiROImin[1][17][17]=	      0;PhiROImax[1][17][17]=	       0;
     EtaROImin[1][17][18]=   0.756521;EtaROImax[1][17][18]=   0.837822;PhiROImin[1][17][18]=   -2.92194;PhiROImax[1][17][18]=	-2.83927;
     EtaROImin[1][17][19]=	    0;EtaROImax[1][17][19]=	     0;PhiROImin[1][17][19]=	      0;PhiROImax[1][17][19]=	       0;
     EtaROImin[1][17][20]=   0.844116;EtaROImax[1][17][20]=   0.903324;PhiROImin[1][17][20]=   -2.83564;PhiROImax[1][17][20]=	-2.75168;
     EtaROImin[1][17][21]=   0.899344;EtaROImax[1][17][21]=   0.956037;PhiROImin[1][17][21]=   -2.83564;PhiROImax[1][17][21]=	-2.75168;
     EtaROImin[1][17][22]=   0.844116;EtaROImax[1][17][22]=   0.903324;PhiROImin[1][17][22]=   -2.92194;PhiROImax[1][17][22]=	-2.83927;
     EtaROImin[1][17][23]=   0.899344;EtaROImax[1][17][23]=   0.956037;PhiROImin[1][17][23]=   -2.92194;PhiROImax[1][17][23]=	-2.83927;
     EtaROImin[1][17][24]=	    0;EtaROImax[1][17][24]=	     0;PhiROImin[1][17][24]=	      0;PhiROImax[1][17][24]=	       0;
     EtaROImin[1][17][25]=	    0;EtaROImax[1][17][25]=	     0;PhiROImin[1][17][25]=	      0;PhiROImax[1][17][25]=	       0;
     EtaROImin[1][17][26]=	    0;EtaROImax[1][17][26]=	     0;PhiROImin[1][17][26]=	      0;PhiROImax[1][17][26]=	       0;
     EtaROImin[1][17][27]=	    0;EtaROImax[1][17][27]=	     0;PhiROImin[1][17][27]=	      0;PhiROImax[1][17][27]=	       0;
     EtaROImin[1][17][28]=	    0;EtaROImax[1][17][28]=	     0;PhiROImin[1][17][28]=	      0;PhiROImax[1][17][28]=	       0;
     EtaROImin[1][17][29]=	    0;EtaROImax[1][17][29]=	     0;PhiROImin[1][17][29]=	      0;PhiROImax[1][17][29]=	       0;
     EtaROImin[1][17][30]=	    0;EtaROImax[1][17][30]=	     0;PhiROImin[1][17][30]=	      0;PhiROImax[1][17][30]=	       0;
     EtaROImin[1][17][31]=	    0;EtaROImax[1][17][31]=	     0;PhiROImin[1][17][31]=	      0;PhiROImax[1][17][31]=	       0;
     EtaROImin[1][18][ 0]=   0.116816;EtaROImax[1][18][ 0]=   0.213185;PhiROImin[1][18][ 0]=	-2.7461;PhiROImax[1][18][ 0]=	-2.66215;
     EtaROImin[1][18][ 1]=  0.0208251;EtaROImax[1][18][ 1]=   0.118734;PhiROImin[1][18][ 1]=	-2.7461;PhiROImax[1][18][ 1]=	-2.66215;
     EtaROImin[1][18][ 2]=   0.116816;EtaROImax[1][18][ 2]=   0.213185;PhiROImin[1][18][ 2]=   -2.65852;PhiROImax[1][18][ 2]=	-2.57584;
     EtaROImin[1][18][ 3]=  0.0208251;EtaROImax[1][18][ 3]=   0.118734;PhiROImin[1][18][ 3]=   -2.65852;PhiROImax[1][18][ 3]=	-2.57584;
     EtaROImin[1][18][ 4]=    0.30075;EtaROImax[1][18][ 4]=   0.416721;PhiROImin[1][18][ 4]=	-2.7461;PhiROImax[1][18][ 4]=	-2.66215;
     EtaROImin[1][18][ 5]=   0.222449;EtaROImax[1][18][ 5]=   0.302928;PhiROImin[1][18][ 5]=	-2.7461;PhiROImax[1][18][ 5]=	-2.66215;
     EtaROImin[1][18][ 6]=    0.30075;EtaROImax[1][18][ 6]=   0.416721;PhiROImin[1][18][ 6]=   -2.65852;PhiROImax[1][18][ 6]=	-2.57584;
     EtaROImin[1][18][ 7]=   0.222449;EtaROImax[1][18][ 7]=   0.302928;PhiROImin[1][18][ 7]=   -2.65852;PhiROImax[1][18][ 7]=	-2.57584;
     EtaROImin[1][18][ 8]=   0.501681;EtaROImax[1][18][ 8]=   0.573871;PhiROImin[1][18][ 8]=	-2.7461;PhiROImax[1][18][ 8]=	-2.66215;
     EtaROImin[1][18][ 9]=    0.42967;EtaROImax[1][18][ 9]=   0.504617;PhiROImin[1][18][ 9]=	-2.7461;PhiROImax[1][18][ 9]=	-2.66215;
     EtaROImin[1][18][10]=   0.501681;EtaROImax[1][18][10]=   0.573871;PhiROImin[1][18][10]=   -2.65852;PhiROImax[1][18][10]=	-2.57584;
     EtaROImin[1][18][11]=    0.42967;EtaROImax[1][18][11]=   0.504617;PhiROImin[1][18][11]=   -2.65852;PhiROImax[1][18][11]=	-2.57584;
     EtaROImin[1][18][12]=   0.649934;EtaROImax[1][18][12]=   0.741516;PhiROImin[1][18][12]=	-2.7461;PhiROImax[1][18][12]=	-2.66215;
     EtaROImin[1][18][13]=   0.583785;EtaROImax[1][18][13]=   0.653329;PhiROImin[1][18][13]=	-2.7461;PhiROImax[1][18][13]=	-2.66215;
     EtaROImin[1][18][14]=   0.649934;EtaROImax[1][18][14]=   0.741516;PhiROImin[1][18][14]=   -2.65852;PhiROImax[1][18][14]=	-2.57584;
     EtaROImin[1][18][15]=   0.583785;EtaROImax[1][18][15]=   0.653329;PhiROImin[1][18][15]=   -2.65852;PhiROImax[1][18][15]=	-2.57584;
     EtaROImin[1][18][16]=	    0;EtaROImax[1][18][16]=	     0;PhiROImin[1][18][16]=	      0;PhiROImax[1][18][16]=	       0;
     EtaROImin[1][18][17]=   0.756521;EtaROImax[1][18][17]=   0.837822;PhiROImin[1][18][17]=	-2.7461;PhiROImax[1][18][17]=	-2.66215;
     EtaROImin[1][18][18]=	    0;EtaROImax[1][18][18]=	     0;PhiROImin[1][18][18]=	      0;PhiROImax[1][18][18]=	       0;
     EtaROImin[1][18][19]=   0.756521;EtaROImax[1][18][19]=   0.837822;PhiROImin[1][18][19]=   -2.65852;PhiROImax[1][18][19]=	-2.57584;
     EtaROImin[1][18][20]=   0.899344;EtaROImax[1][18][20]=   0.956037;PhiROImin[1][18][20]=	-2.7461;PhiROImax[1][18][20]=	-2.66215;
     EtaROImin[1][18][21]=   0.844116;EtaROImax[1][18][21]=   0.903324;PhiROImin[1][18][21]=	-2.7461;PhiROImax[1][18][21]=	-2.66215;
     EtaROImin[1][18][22]=   0.899344;EtaROImax[1][18][22]=   0.956037;PhiROImin[1][18][22]=   -2.65852;PhiROImax[1][18][22]=	-2.57584;
     EtaROImin[1][18][23]=   0.844116;EtaROImax[1][18][23]=   0.903324;PhiROImin[1][18][23]=   -2.65852;PhiROImax[1][18][23]=	-2.57584;
     EtaROImin[1][18][24]=	    0;EtaROImax[1][18][24]=	     0;PhiROImin[1][18][24]=	      0;PhiROImax[1][18][24]=	       0;
     EtaROImin[1][18][25]=	    0;EtaROImax[1][18][25]=	     0;PhiROImin[1][18][25]=	      0;PhiROImax[1][18][25]=	       0;
     EtaROImin[1][18][26]=	    0;EtaROImax[1][18][26]=	     0;PhiROImin[1][18][26]=	      0;PhiROImax[1][18][26]=	       0;
     EtaROImin[1][18][27]=	    0;EtaROImax[1][18][27]=	     0;PhiROImin[1][18][27]=	      0;PhiROImax[1][18][27]=	       0;
     EtaROImin[1][18][28]=	    0;EtaROImax[1][18][28]=	     0;PhiROImin[1][18][28]=	      0;PhiROImax[1][18][28]=	       0;
     EtaROImin[1][18][29]=	    0;EtaROImax[1][18][29]=	     0;PhiROImin[1][18][29]=	      0;PhiROImax[1][18][29]=	       0;
     EtaROImin[1][18][30]=	    0;EtaROImax[1][18][30]=	     0;PhiROImin[1][18][30]=	      0;PhiROImax[1][18][30]=	       0;
     EtaROImin[1][18][31]=	    0;EtaROImax[1][18][31]=	     0;PhiROImin[1][18][31]=	      0;PhiROImax[1][18][31]=	       0;
     EtaROImin[1][19][ 0]=  0.0874905;EtaROImax[1][19][ 0]=   0.145912;PhiROImin[1][19][ 0]=   -2.46941;PhiROImax[1][19][ 0]=	-2.35916;
     EtaROImin[1][19][ 1]=   0.149792;EtaROImax[1][19][ 1]=   0.238294;PhiROImin[1][19][ 1]=   -2.46941;PhiROImax[1][19][ 1]=	-2.35916;
     EtaROImin[1][19][ 2]=  0.0874905;EtaROImax[1][19][ 2]=   0.145912;PhiROImin[1][19][ 2]=   -2.58036;PhiROImax[1][19][ 2]=	-2.47293;
     EtaROImin[1][19][ 3]=   0.149792;EtaROImax[1][19][ 3]=   0.238294;PhiROImin[1][19][ 3]=   -2.58036;PhiROImax[1][19][ 3]=	-2.47293;
     EtaROImin[1][19][ 4]=   0.246207;EtaROImax[1][19][ 4]=    0.35049;PhiROImin[1][19][ 4]=   -2.46881;PhiROImax[1][19][ 4]=	-2.35915;
     EtaROImin[1][19][ 5]=   0.350831;EtaROImax[1][19][ 5]=   0.452099;PhiROImin[1][19][ 5]=   -2.46881;PhiROImax[1][19][ 5]=	-2.35915;
     EtaROImin[1][19][ 6]=   0.246207;EtaROImax[1][19][ 6]=    0.35049;PhiROImin[1][19][ 6]=   -2.57921;PhiROImax[1][19][ 6]=	-2.47231;
     EtaROImin[1][19][ 7]=   0.350831;EtaROImax[1][19][ 7]=   0.452099;PhiROImin[1][19][ 7]=   -2.57921;PhiROImax[1][19][ 7]=	-2.47231;
     EtaROImin[1][19][ 8]=   0.458091;EtaROImax[1][19][ 8]=   0.554444;PhiROImin[1][19][ 8]=   -2.46881;PhiROImax[1][19][ 8]=	-2.35915;
     EtaROImin[1][19][ 9]=   0.555789;EtaROImax[1][19][ 9]=   0.648101;PhiROImin[1][19][ 9]=   -2.46881;PhiROImax[1][19][ 9]=	-2.35915;
     EtaROImin[1][19][10]=   0.458091;EtaROImax[1][19][10]=   0.554444;PhiROImin[1][19][10]=   -2.57921;PhiROImax[1][19][10]=	-2.47231;
     EtaROImin[1][19][11]=   0.555789;EtaROImax[1][19][11]=   0.648101;PhiROImin[1][19][11]=   -2.57921;PhiROImax[1][19][11]=	-2.47231;
     EtaROImin[1][19][12]=   0.655857;EtaROImax[1][19][12]=   0.703298;PhiROImin[1][19][12]=   -2.46941;PhiROImax[1][19][12]=	-2.35916;
     EtaROImin[1][19][13]=   0.706413;EtaROImax[1][19][13]=   0.776449;PhiROImin[1][19][13]=   -2.46941;PhiROImax[1][19][13]=	-2.35916;
     EtaROImin[1][19][14]=   0.655857;EtaROImax[1][19][14]=   0.703298;PhiROImin[1][19][14]=   -2.58036;PhiROImax[1][19][14]=	-2.47293;
     EtaROImin[1][19][15]=   0.706413;EtaROImax[1][19][15]=   0.776449;PhiROImin[1][19][15]=   -2.58036;PhiROImax[1][19][15]=	-2.47293;
     EtaROImin[1][19][16]=   0.783867;EtaROImax[1][19][16]=    0.82768;PhiROImin[1][19][16]=   -2.46941;PhiROImax[1][19][16]=	-2.35916;
     EtaROImin[1][19][17]=   0.830556;EtaROImax[1][19][17]=   0.895163;PhiROImin[1][19][17]=   -2.46941;PhiROImax[1][19][17]=	-2.35916;
     EtaROImin[1][19][18]=   0.783867;EtaROImax[1][19][18]=    0.82768;PhiROImin[1][19][18]=   -2.58036;PhiROImax[1][19][18]=	-2.47293;
     EtaROImin[1][19][19]=   0.830556;EtaROImax[1][19][19]=   0.895163;PhiROImin[1][19][19]=   -2.58036;PhiROImax[1][19][19]=	-2.47293;
     EtaROImin[1][19][20]=   0.898201;EtaROImax[1][19][20]=   0.961344;PhiROImin[1][19][20]=   -2.46881;PhiROImax[1][19][20]=	-2.35915;
     EtaROImin[1][19][21]=   0.964674;EtaROImax[1][19][21]=    1.02514;PhiROImin[1][19][21]=   -2.46881;PhiROImax[1][19][21]=	-2.35915;
     EtaROImin[1][19][22]=   0.898201;EtaROImax[1][19][22]=   0.961344;PhiROImin[1][19][22]=   -2.57921;PhiROImax[1][19][22]=	-2.47231;
     EtaROImin[1][19][23]=   0.964674;EtaROImax[1][19][23]=    1.02514;PhiROImin[1][19][23]=   -2.57921;PhiROImax[1][19][23]=	-2.47231;
     EtaROImin[1][19][24]=    1.03003;EtaROImax[1][19][24]=    1.06547;PhiROImin[1][19][24]=   -2.46962;PhiROImax[1][19][24]=	-2.35926;
     EtaROImin[1][19][25]=	    0;EtaROImax[1][19][25]=	     0;PhiROImin[1][19][25]=	      0;PhiROImax[1][19][25]=	       0;
     EtaROImin[1][19][26]=    1.03003;EtaROImax[1][19][26]=    1.06547;PhiROImin[1][19][26]=   -2.55334;PhiROImax[1][19][26]=	-2.47315;
     EtaROImin[1][19][27]=	    0;EtaROImax[1][19][27]=	     0;PhiROImin[1][19][27]=	      0;PhiROImax[1][19][27]=	       0;
     EtaROImin[1][19][28]=	    0;EtaROImax[1][19][28]=	     0;PhiROImin[1][19][28]=	      0;PhiROImax[1][19][28]=	       0;
     EtaROImin[1][19][29]=	    0;EtaROImax[1][19][29]=	     0;PhiROImin[1][19][29]=	      0;PhiROImax[1][19][29]=	       0;
     EtaROImin[1][19][30]=	    0;EtaROImax[1][19][30]=	     0;PhiROImin[1][19][30]=	      0;PhiROImax[1][19][30]=	       0;
     EtaROImin[1][19][31]=	    0;EtaROImax[1][19][31]=	     0;PhiROImin[1][19][31]=	      0;PhiROImax[1][19][31]=	       0;
     EtaROImin[1][20][ 0]=   0.180742;EtaROImax[1][20][ 0]=   0.238294;PhiROImin[1][20][ 0]=   -2.35323;PhiROImax[1][20][ 0]=	-2.24298;
     EtaROImin[1][20][ 1]=  0.0874905;EtaROImax[1][20][ 1]=   0.176882;PhiROImin[1][20][ 1]=   -2.35323;PhiROImax[1][20][ 1]=	-2.24298;
     EtaROImin[1][20][ 2]=   0.180742;EtaROImax[1][20][ 2]=   0.238294;PhiROImin[1][20][ 2]=   -2.23946;PhiROImax[1][20][ 2]=	-2.13203;
     EtaROImin[1][20][ 3]=  0.0874905;EtaROImax[1][20][ 3]=   0.176882;PhiROImin[1][20][ 3]=   -2.23946;PhiROImax[1][20][ 3]=	-2.13203;
     EtaROImin[1][20][ 4]=   0.350831;EtaROImax[1][20][ 4]=   0.452099;PhiROImin[1][20][ 4]=   -2.35324;PhiROImax[1][20][ 4]=	-2.24358;
     EtaROImin[1][20][ 5]=   0.246207;EtaROImax[1][20][ 5]=    0.35049;PhiROImin[1][20][ 5]=   -2.35324;PhiROImax[1][20][ 5]=	-2.24358;
     EtaROImin[1][20][ 6]=   0.350831;EtaROImax[1][20][ 6]=   0.452099;PhiROImin[1][20][ 6]=   -2.24008;PhiROImax[1][20][ 6]=	-2.13318;
     EtaROImin[1][20][ 7]=   0.246207;EtaROImax[1][20][ 7]=    0.35049;PhiROImin[1][20][ 7]=   -2.24008;PhiROImax[1][20][ 7]=	-2.13318;
     EtaROImin[1][20][ 8]=   0.555789;EtaROImax[1][20][ 8]=   0.648101;PhiROImin[1][20][ 8]=   -2.35324;PhiROImax[1][20][ 8]=	-2.24358;
     EtaROImin[1][20][ 9]=   0.458091;EtaROImax[1][20][ 9]=   0.554444;PhiROImin[1][20][ 9]=   -2.35324;PhiROImax[1][20][ 9]=	-2.24358;
     EtaROImin[1][20][10]=   0.555789;EtaROImax[1][20][10]=   0.648101;PhiROImin[1][20][10]=   -2.24008;PhiROImax[1][20][10]=	-2.13318;
     EtaROImin[1][20][11]=   0.458091;EtaROImax[1][20][11]=   0.554444;PhiROImin[1][20][11]=   -2.24008;PhiROImax[1][20][11]=	-2.13318;
     EtaROImin[1][20][12]=   0.731124;EtaROImax[1][20][12]=   0.776449;PhiROImin[1][20][12]=   -2.35323;PhiROImax[1][20][12]=	-2.24298;
     EtaROImin[1][20][13]=   0.655857;EtaROImax[1][20][13]=   0.728056;PhiROImin[1][20][13]=   -2.35323;PhiROImax[1][20][13]=	-2.24298;
     EtaROImin[1][20][14]=   0.731124;EtaROImax[1][20][14]=   0.776449;PhiROImin[1][20][14]=   -2.23946;PhiROImax[1][20][14]=	-2.13203;
     EtaROImin[1][20][15]=   0.655857;EtaROImax[1][20][15]=   0.728056;PhiROImin[1][20][15]=   -2.23946;PhiROImax[1][20][15]=	-2.13203;
     EtaROImin[1][20][16]=   0.853359;EtaROImax[1][20][16]=   0.895163;PhiROImin[1][20][16]=   -2.35323;PhiROImax[1][20][16]=	-2.24298;
     EtaROImin[1][20][17]=   0.783867;EtaROImax[1][20][17]=   0.850528;PhiROImin[1][20][17]=   -2.35323;PhiROImax[1][20][17]=	-2.24298;
     EtaROImin[1][20][18]=   0.853359;EtaROImax[1][20][18]=   0.895163;PhiROImin[1][20][18]=   -2.23946;PhiROImax[1][20][18]=	-2.13203;
     EtaROImin[1][20][19]=   0.783867;EtaROImax[1][20][19]=   0.850528;PhiROImin[1][20][19]=   -2.23946;PhiROImax[1][20][19]=	-2.13203;
     EtaROImin[1][20][20]=   0.964674;EtaROImax[1][20][20]=    1.02514;PhiROImin[1][20][20]=   -2.35324;PhiROImax[1][20][20]=	-2.24358;
     EtaROImin[1][20][21]=   0.898201;EtaROImax[1][20][21]=   0.961344;PhiROImin[1][20][21]=   -2.35324;PhiROImax[1][20][21]=	-2.24358;
     EtaROImin[1][20][22]=   0.964674;EtaROImax[1][20][22]=    1.02514;PhiROImin[1][20][22]=   -2.24008;PhiROImax[1][20][22]=	-2.13318;
     EtaROImin[1][20][23]=   0.898201;EtaROImax[1][20][23]=   0.961344;PhiROImin[1][20][23]=   -2.24008;PhiROImax[1][20][23]=	-2.13318;
     EtaROImin[1][20][24]=	    0;EtaROImax[1][20][24]=	     0;PhiROImin[1][20][24]=	      0;PhiROImax[1][20][24]=	       0;
     EtaROImin[1][20][25]=    1.03003;EtaROImax[1][20][25]=    1.06547;PhiROImin[1][20][25]=   -2.35313;PhiROImax[1][20][25]=	-2.24277;
     EtaROImin[1][20][26]=	    0;EtaROImax[1][20][26]=	     0;PhiROImin[1][20][26]=	      0;PhiROImax[1][20][26]=	       0;
     EtaROImin[1][20][27]=    1.03003;EtaROImax[1][20][27]=    1.06547;PhiROImin[1][20][27]=   -2.23924;PhiROImax[1][20][27]=	-2.15905;
     EtaROImin[1][20][28]=	    0;EtaROImax[1][20][28]=	     0;PhiROImin[1][20][28]=	      0;PhiROImax[1][20][28]=	       0;
     EtaROImin[1][20][29]=	    0;EtaROImax[1][20][29]=	     0;PhiROImin[1][20][29]=	      0;PhiROImax[1][20][29]=	       0;
     EtaROImin[1][20][30]=	    0;EtaROImax[1][20][30]=	     0;PhiROImin[1][20][30]=	      0;PhiROImax[1][20][30]=	       0;
     EtaROImin[1][20][31]=	    0;EtaROImax[1][20][31]=	     0;PhiROImin[1][20][31]=	      0;PhiROImax[1][20][31]=	       0;
     EtaROImin[1][21][ 0]=  0.0775634;EtaROImax[1][21][ 0]=   0.126072;PhiROImin[1][21][ 0]=   -2.05024;PhiROImax[1][21][ 0]=	-1.96629;
     EtaROImin[1][21][ 1]=   0.129296;EtaROImax[1][21][ 1]=   0.203032;PhiROImin[1][21][ 1]=   -2.05024;PhiROImax[1][21][ 1]=	-1.96629;
     EtaROImin[1][21][ 2]=  0.0775634;EtaROImax[1][21][ 2]=   0.126072;PhiROImin[1][21][ 2]=   -2.13655;PhiROImax[1][21][ 2]=	-2.05387;
     EtaROImin[1][21][ 3]=   0.129296;EtaROImax[1][21][ 3]=   0.203032;PhiROImin[1][21][ 3]=   -2.13655;PhiROImax[1][21][ 3]=	-2.05387;
     EtaROImin[1][21][ 4]=   0.200823;EtaROImax[1][21][ 4]=   0.248155;PhiROImin[1][21][ 4]=   -2.04983;PhiROImax[1][21][ 4]=	-1.96627;
     EtaROImin[1][21][ 5]=   0.251292;EtaROImax[1][21][ 5]=   0.322732;PhiROImin[1][21][ 5]=   -2.04983;PhiROImax[1][21][ 5]=	-1.96627;
     EtaROImin[1][21][ 6]=   0.200823;EtaROImax[1][21][ 6]=   0.248155;PhiROImin[1][21][ 6]=   -2.13573;PhiROImax[1][21][ 6]=	-2.05344;
     EtaROImin[1][21][ 7]=   0.251292;EtaROImax[1][21][ 7]=   0.322732;PhiROImin[1][21][ 7]=   -2.13573;PhiROImax[1][21][ 7]=	-2.05344;
     EtaROImin[1][21][ 8]=   0.362484;EtaROImax[1][21][ 8]=   0.441017;PhiROImin[1][21][ 8]=   -2.04536;PhiROImax[1][21][ 8]=	-1.96564;
     EtaROImin[1][21][ 9]=   0.439011;EtaROImax[1][21][ 9]=    0.51487;PhiROImin[1][21][ 9]=   -2.04536;PhiROImax[1][21][ 9]=	-1.96564;
     EtaROImin[1][21][10]=   0.362484;EtaROImax[1][21][10]=   0.441017;PhiROImin[1][21][10]=   -2.12657;PhiROImax[1][21][10]=	-2.04792;
     EtaROImin[1][21][11]=   0.439011;EtaROImax[1][21][11]=    0.51487;PhiROImin[1][21][11]=   -2.12657;PhiROImax[1][21][11]=	-2.04792;
     EtaROImin[1][21][12]=   0.470878;EtaROImax[1][21][12]=   0.569141;PhiROImin[1][21][12]=   -2.05024;PhiROImax[1][21][12]=	-1.96629;
     EtaROImin[1][21][13]=   0.566067;EtaROImax[1][21][13]=   0.659332;PhiROImin[1][21][13]=   -2.05024;PhiROImax[1][21][13]=	-1.96629;
     EtaROImin[1][21][14]=   0.470878;EtaROImax[1][21][14]=   0.569141;PhiROImin[1][21][14]=   -2.13655;PhiROImax[1][21][14]=	-2.05387;
     EtaROImin[1][21][15]=   0.566067;EtaROImax[1][21][15]=   0.659332;PhiROImin[1][21][15]=   -2.13655;PhiROImax[1][21][15]=	-2.05387;
     EtaROImin[1][21][16]=   0.622205;EtaROImax[1][21][16]=   0.674155;PhiROImin[1][21][16]=   -2.04536;PhiROImax[1][21][16]=	-1.96564;
     EtaROImin[1][21][17]=   0.671529;EtaROImax[1][21][17]=    0.72184;PhiROImin[1][21][17]=   -2.04536;PhiROImax[1][21][17]=	-1.96564;
     EtaROImin[1][21][18]=   0.622205;EtaROImax[1][21][18]=   0.674155;PhiROImin[1][21][18]=   -2.12657;PhiROImax[1][21][18]=	-2.04792;
     EtaROImin[1][21][19]=   0.671529;EtaROImax[1][21][19]=    0.72184;PhiROImin[1][21][19]=   -2.12657;PhiROImax[1][21][19]=	-2.04792;
     EtaROImin[1][21][20]=   0.744499;EtaROImax[1][21][20]=   0.831846;PhiROImin[1][21][20]=   -2.04536;PhiROImax[1][21][20]=	-1.96564;
     EtaROImin[1][21][21]=   0.833797;EtaROImax[1][21][21]=   0.893028;PhiROImin[1][21][21]=   -2.04536;PhiROImax[1][21][21]=	-1.96564;
     EtaROImin[1][21][22]=   0.744499;EtaROImax[1][21][22]=   0.831846;PhiROImin[1][21][22]=   -2.12657;PhiROImax[1][21][22]=	-2.04792;
     EtaROImin[1][21][23]=   0.833797;EtaROImax[1][21][23]=   0.893028;PhiROImin[1][21][23]=   -2.12657;PhiROImax[1][21][23]=	-2.04792;
     EtaROImin[1][21][24]=   0.788882;EtaROImax[1][21][24]=   0.850339;PhiROImin[1][21][24]=   -2.05024;PhiROImax[1][21][24]=	-1.96629;
     EtaROImin[1][21][25]=   0.846464;EtaROImax[1][21][25]=   0.905303;PhiROImin[1][21][25]=   -2.05024;PhiROImax[1][21][25]=	-1.96629;
     EtaROImin[1][21][26]=   0.788882;EtaROImax[1][21][26]=   0.850339;PhiROImin[1][21][26]=   -2.13655;PhiROImax[1][21][26]=	-2.05387;
     EtaROImin[1][21][27]=   0.846464;EtaROImax[1][21][27]=   0.905303;PhiROImin[1][21][27]=   -2.13655;PhiROImax[1][21][27]=	-2.05387;
     EtaROImin[1][21][28]=   0.911293;EtaROImax[1][21][28]=   0.938699;PhiROImin[1][21][28]=   -2.03358;PhiROImax[1][21][28]=	-1.98444;
     EtaROImin[1][21][29]=   0.940507;EtaROImax[1][21][29]=    0.98143;PhiROImin[1][21][29]=   -2.03358;PhiROImax[1][21][29]=	-1.98444;
     EtaROImin[1][21][30]=   0.911293;EtaROImax[1][21][30]=   0.938699;PhiROImin[1][21][30]=   -2.12127;PhiROImax[1][21][30]=	-2.03635;
     EtaROImin[1][21][31]=   0.940507;EtaROImax[1][21][31]=    0.98143;PhiROImin[1][21][31]=   -2.12127;PhiROImax[1][21][31]=	-2.03635;
     EtaROImin[1][22][ 0]=   0.155043;EtaROImax[1][22][ 0]=   0.203032;PhiROImin[1][22][ 0]=   -1.96071;PhiROImax[1][22][ 0]=	-1.87675;
     EtaROImin[1][22][ 1]=  0.0775634;EtaROImax[1][22][ 1]=    0.15183;PhiROImin[1][22][ 1]=   -1.96071;PhiROImax[1][22][ 1]=	-1.87675;
     EtaROImin[1][22][ 2]=   0.155043;EtaROImax[1][22][ 2]=   0.203032;PhiROImin[1][22][ 2]=   -1.87312;PhiROImax[1][22][ 2]=	-1.79045;
     EtaROImin[1][22][ 3]=  0.0775634;EtaROImax[1][22][ 3]=    0.15183;PhiROImin[1][22][ 3]=   -1.87312;PhiROImax[1][22][ 3]=	-1.79045;
     EtaROImin[1][22][ 4]=   0.276301;EtaROImax[1][22][ 4]=   0.322732;PhiROImin[1][22][ 4]=   -1.96072;PhiROImax[1][22][ 4]=	-1.87716;
     EtaROImin[1][22][ 5]=   0.200823;EtaROImax[1][22][ 5]=   0.273184;PhiROImin[1][22][ 5]=   -1.96072;PhiROImax[1][22][ 5]=	-1.87716;
     EtaROImin[1][22][ 6]=   0.276301;EtaROImax[1][22][ 6]=   0.322732;PhiROImin[1][22][ 6]=   -1.87355;PhiROImax[1][22][ 6]=	-1.79126;
     EtaROImin[1][22][ 7]=   0.200823;EtaROImax[1][22][ 7]=   0.273184;PhiROImin[1][22][ 7]=   -1.87355;PhiROImax[1][22][ 7]=	-1.79126;
     EtaROImin[1][22][ 8]=   0.439011;EtaROImax[1][22][ 8]=    0.51487;PhiROImin[1][22][ 8]=   -1.96135;PhiROImax[1][22][ 8]=	-1.88163;
     EtaROImin[1][22][ 9]=   0.362484;EtaROImax[1][22][ 9]=   0.441017;PhiROImin[1][22][ 9]=   -1.96135;PhiROImax[1][22][ 9]=	-1.88163;
     EtaROImin[1][22][10]=   0.439011;EtaROImax[1][22][10]=    0.51487;PhiROImin[1][22][10]=   -1.87907;PhiROImax[1][22][10]=	-1.80042;
     EtaROImin[1][22][11]=   0.362484;EtaROImax[1][22][11]=   0.441017;PhiROImin[1][22][11]=   -1.87907;PhiROImax[1][22][11]=	-1.80042;
     EtaROImin[1][22][12]=   0.566067;EtaROImax[1][22][12]=   0.659332;PhiROImin[1][22][12]=   -1.96071;PhiROImax[1][22][12]=	-1.87675;
     EtaROImin[1][22][13]=   0.470878;EtaROImax[1][22][13]=   0.569141;PhiROImin[1][22][13]=   -1.96071;PhiROImax[1][22][13]=	-1.87675;
     EtaROImin[1][22][14]=   0.566067;EtaROImax[1][22][14]=   0.659332;PhiROImin[1][22][14]=   -1.87312;PhiROImax[1][22][14]=	-1.79045;
     EtaROImin[1][22][15]=   0.470878;EtaROImax[1][22][15]=   0.569141;PhiROImin[1][22][15]=   -1.87312;PhiROImax[1][22][15]=	-1.79045;
     EtaROImin[1][22][16]=   0.671529;EtaROImax[1][22][16]=    0.72184;PhiROImin[1][22][16]=   -1.96135;PhiROImax[1][22][16]=	-1.88163;
     EtaROImin[1][22][17]=   0.622205;EtaROImax[1][22][17]=   0.674155;PhiROImin[1][22][17]=   -1.96135;PhiROImax[1][22][17]=	-1.88163;
     EtaROImin[1][22][18]=   0.671529;EtaROImax[1][22][18]=    0.72184;PhiROImin[1][22][18]=   -1.87907;PhiROImax[1][22][18]=	-1.80042;
     EtaROImin[1][22][19]=   0.622205;EtaROImax[1][22][19]=   0.674155;PhiROImin[1][22][19]=   -1.87907;PhiROImax[1][22][19]=	-1.80042;
     EtaROImin[1][22][20]=   0.833797;EtaROImax[1][22][20]=   0.893028;PhiROImin[1][22][20]=   -1.96135;PhiROImax[1][22][20]=	-1.88163;
     EtaROImin[1][22][21]=   0.744499;EtaROImax[1][22][21]=   0.831846;PhiROImin[1][22][21]=   -1.96135;PhiROImax[1][22][21]=	-1.88163;
     EtaROImin[1][22][22]=   0.833797;EtaROImax[1][22][22]=   0.893028;PhiROImin[1][22][22]=   -1.87907;PhiROImax[1][22][22]=	-1.80042;
     EtaROImin[1][22][23]=   0.744499;EtaROImax[1][22][23]=   0.831846;PhiROImin[1][22][23]=   -1.87907;PhiROImax[1][22][23]=	-1.80042;
     EtaROImin[1][22][24]=   0.846464;EtaROImax[1][22][24]=   0.905303;PhiROImin[1][22][24]=   -1.96071;PhiROImax[1][22][24]=	-1.87675;
     EtaROImin[1][22][25]=   0.788882;EtaROImax[1][22][25]=   0.850339;PhiROImin[1][22][25]=   -1.96071;PhiROImax[1][22][25]=	-1.87675;
     EtaROImin[1][22][26]=   0.846464;EtaROImax[1][22][26]=   0.905303;PhiROImin[1][22][26]=   -1.87312;PhiROImax[1][22][26]=	-1.79045;
     EtaROImin[1][22][27]=   0.788882;EtaROImax[1][22][27]=   0.850339;PhiROImin[1][22][27]=   -1.87312;PhiROImax[1][22][27]=	-1.79045;
     EtaROImin[1][22][28]=   0.957137;EtaROImax[1][22][28]=   0.983719;PhiROImin[1][22][28]=   -1.97686;PhiROImax[1][22][28]=	-1.89087;
     EtaROImin[1][22][29]=   0.913486;EtaROImax[1][22][29]=   0.955345;PhiROImin[1][22][29]=   -1.97686;PhiROImax[1][22][29]=	-1.89087;
     EtaROImin[1][22][30]=   0.957137;EtaROImax[1][22][30]=   0.983719;PhiROImin[1][22][30]=   -1.98444;PhiROImax[1][22][30]=	-1.88811;
     EtaROImin[1][22][31]=   0.913486;EtaROImax[1][22][31]=   0.955345;PhiROImin[1][22][31]=   -1.98444;PhiROImax[1][22][31]=	-1.88811;
     EtaROImin[1][23][ 0]=  0.0552456;EtaROImax[1][23][ 0]=   0.147216;PhiROImin[1][23][ 0]=   -1.68341;PhiROImax[1][23][ 0]=	-1.57375;
     EtaROImin[1][23][ 1]=    0.14708;EtaROImax[1][23][ 1]=   0.238313;PhiROImin[1][23][ 1]=   -1.68341;PhiROImax[1][23][ 1]=	-1.57375;
     EtaROImin[1][23][ 2]=  0.0552456;EtaROImax[1][23][ 2]=   0.147216;PhiROImin[1][23][ 2]=   -1.79381;PhiROImax[1][23][ 2]=	-1.68692;
     EtaROImin[1][23][ 3]=    0.14708;EtaROImax[1][23][ 3]=   0.238313;PhiROImin[1][23][ 3]=   -1.79381;PhiROImax[1][23][ 3]=	-1.68692;
     EtaROImin[1][23][ 4]=   0.246207;EtaROImax[1][23][ 4]=    0.35049;PhiROImin[1][23][ 4]=   -1.68341;PhiROImax[1][23][ 4]=	-1.57375;
     EtaROImin[1][23][ 5]=   0.350831;EtaROImax[1][23][ 5]=   0.452099;PhiROImin[1][23][ 5]=   -1.68341;PhiROImax[1][23][ 5]=	-1.57375;
     EtaROImin[1][23][ 6]=   0.246207;EtaROImax[1][23][ 6]=    0.35049;PhiROImin[1][23][ 6]=   -1.79381;PhiROImax[1][23][ 6]=	-1.68692;
     EtaROImin[1][23][ 7]=   0.350831;EtaROImax[1][23][ 7]=   0.452099;PhiROImin[1][23][ 7]=   -1.79381;PhiROImax[1][23][ 7]=	-1.68692;
     EtaROImin[1][23][ 8]=   0.458091;EtaROImax[1][23][ 8]=   0.554444;PhiROImin[1][23][ 8]=   -1.68341;PhiROImax[1][23][ 8]=	-1.57375;
     EtaROImin[1][23][ 9]=   0.555789;EtaROImax[1][23][ 9]=   0.648101;PhiROImin[1][23][ 9]=   -1.68341;PhiROImax[1][23][ 9]=	-1.57375;
     EtaROImin[1][23][10]=   0.458091;EtaROImax[1][23][10]=   0.554444;PhiROImin[1][23][10]=   -1.79381;PhiROImax[1][23][10]=	-1.68692;
     EtaROImin[1][23][11]=   0.555789;EtaROImax[1][23][11]=   0.648101;PhiROImin[1][23][11]=   -1.79381;PhiROImax[1][23][11]=	-1.68692;
     EtaROImin[1][23][12]=   0.677222;EtaROImax[1][23][12]=   0.720119;PhiROImin[1][23][12]=   -1.62692;PhiROImax[1][23][12]=	 -1.5708;
     EtaROImin[1][23][13]=    0.72294;EtaROImax[1][23][13]=   0.786462;PhiROImin[1][23][13]=   -1.62692;PhiROImax[1][23][13]=	 -1.5708;
     EtaROImin[1][23][14]=   0.677222;EtaROImax[1][23][14]=   0.720119;PhiROImin[1][23][14]=   -1.69744;PhiROImax[1][23][14]=	-1.63046;
     EtaROImin[1][23][15]=    0.72294;EtaROImax[1][23][15]=   0.786462;PhiROImin[1][23][15]=   -1.69744;PhiROImax[1][23][15]=	-1.63046;
     EtaROImin[1][23][16]=   0.783867;EtaROImax[1][23][16]=    0.82768;PhiROImin[1][23][16]=   -1.68401;PhiROImax[1][23][16]=	-1.57376;
     EtaROImin[1][23][17]=   0.830556;EtaROImax[1][23][17]=   0.895163;PhiROImin[1][23][17]=   -1.68401;PhiROImax[1][23][17]=	-1.57376;
     EtaROImin[1][23][18]=   0.783867;EtaROImax[1][23][18]=    0.82768;PhiROImin[1][23][18]=   -1.79497;PhiROImax[1][23][18]=	-1.68753;
     EtaROImin[1][23][19]=   0.830556;EtaROImax[1][23][19]=   0.895163;PhiROImin[1][23][19]=   -1.79497;PhiROImax[1][23][19]=	-1.68753;
     EtaROImin[1][23][20]=   0.898201;EtaROImax[1][23][20]=   0.961344;PhiROImin[1][23][20]=   -1.68341;PhiROImax[1][23][20]=	-1.57375;
     EtaROImin[1][23][21]=   0.964674;EtaROImax[1][23][21]=    1.02514;PhiROImin[1][23][21]=   -1.68341;PhiROImax[1][23][21]=	-1.57375;
     EtaROImin[1][23][22]=   0.898201;EtaROImax[1][23][22]=   0.961344;PhiROImin[1][23][22]=   -1.79381;PhiROImax[1][23][22]=	-1.68692;
     EtaROImin[1][23][23]=   0.964674;EtaROImax[1][23][23]=    1.02514;PhiROImin[1][23][23]=   -1.79381;PhiROImax[1][23][23]=	-1.68692;
     EtaROImin[1][23][24]=    1.03003;EtaROImax[1][23][24]=    1.06547;PhiROImin[1][23][24]=   -1.68423;PhiROImax[1][23][24]=	-1.57387;
     EtaROImin[1][23][25]=	    0;EtaROImax[1][23][25]=	     0;PhiROImin[1][23][25]=	      0;PhiROImax[1][23][25]=	       0;
     EtaROImin[1][23][26]=    1.03003;EtaROImax[1][23][26]=    1.06547;PhiROImin[1][23][26]=   -1.76794;PhiROImax[1][23][26]=	-1.68775;
     EtaROImin[1][23][27]=	    0;EtaROImax[1][23][27]=	     0;PhiROImin[1][23][27]=	      0;PhiROImax[1][23][27]=	       0;
     EtaROImin[1][23][28]=	    0;EtaROImax[1][23][28]=	     0;PhiROImin[1][23][28]=	      0;PhiROImax[1][23][28]=	       0;
     EtaROImin[1][23][29]=	    0;EtaROImax[1][23][29]=	     0;PhiROImin[1][23][29]=	      0;PhiROImax[1][23][29]=	       0;
     EtaROImin[1][23][30]=	    0;EtaROImax[1][23][30]=	     0;PhiROImin[1][23][30]=	      0;PhiROImax[1][23][30]=	       0;
     EtaROImin[1][23][31]=	    0;EtaROImax[1][23][31]=	     0;PhiROImin[1][23][31]=	      0;PhiROImax[1][23][31]=	       0;
     EtaROImin[1][24][ 0]=    0.14708;EtaROImax[1][24][ 0]=   0.238313;PhiROImin[1][24][ 0]=   -1.56784;PhiROImax[1][24][ 0]=	-1.45818;
     EtaROImin[1][24][ 1]=  0.0552456;EtaROImax[1][24][ 1]=   0.147216;PhiROImin[1][24][ 1]=   -1.56784;PhiROImax[1][24][ 1]=	-1.45818;
     EtaROImin[1][24][ 2]=    0.14708;EtaROImax[1][24][ 2]=   0.238313;PhiROImin[1][24][ 2]=   -1.45468;PhiROImax[1][24][ 2]=	-1.34779;
     EtaROImin[1][24][ 3]=  0.0552456;EtaROImax[1][24][ 3]=   0.147216;PhiROImin[1][24][ 3]=   -1.45468;PhiROImax[1][24][ 3]=	-1.34779;
     EtaROImin[1][24][ 4]=   0.350831;EtaROImax[1][24][ 4]=   0.452099;PhiROImin[1][24][ 4]=   -1.56784;PhiROImax[1][24][ 4]=	-1.45818;
     EtaROImin[1][24][ 5]=   0.246207;EtaROImax[1][24][ 5]=    0.35049;PhiROImin[1][24][ 5]=   -1.56784;PhiROImax[1][24][ 5]=	-1.45818;
     EtaROImin[1][24][ 6]=   0.350831;EtaROImax[1][24][ 6]=   0.452099;PhiROImin[1][24][ 6]=   -1.45468;PhiROImax[1][24][ 6]=	-1.34779;
     EtaROImin[1][24][ 7]=   0.246207;EtaROImax[1][24][ 7]=    0.35049;PhiROImin[1][24][ 7]=   -1.45468;PhiROImax[1][24][ 7]=	-1.34779;
     EtaROImin[1][24][ 8]=   0.555789;EtaROImax[1][24][ 8]=   0.648101;PhiROImin[1][24][ 8]=   -1.56784;PhiROImax[1][24][ 8]=	-1.45818;
     EtaROImin[1][24][ 9]=   0.458091;EtaROImax[1][24][ 9]=   0.554444;PhiROImin[1][24][ 9]=   -1.56784;PhiROImax[1][24][ 9]=	-1.45818;
     EtaROImin[1][24][10]=   0.555789;EtaROImax[1][24][10]=   0.648101;PhiROImin[1][24][10]=   -1.45468;PhiROImax[1][24][10]=	-1.34779;
     EtaROImin[1][24][11]=   0.458091;EtaROImax[1][24][11]=   0.554444;PhiROImin[1][24][11]=   -1.45468;PhiROImax[1][24][11]=	-1.34779;
     EtaROImin[1][24][12]=   0.745327;EtaROImax[1][24][12]=   0.786462;PhiROImin[1][24][12]=   -1.56798;PhiROImax[1][24][12]=	 -1.5005;
     EtaROImin[1][24][13]=   0.677222;EtaROImax[1][24][13]=   0.742545;PhiROImin[1][24][13]=   -1.56798;PhiROImax[1][24][13]=	 -1.5005;
     EtaROImin[1][24][14]=   0.745327;EtaROImax[1][24][14]=   0.786462;PhiROImin[1][24][14]=	-1.5708;PhiROImax[1][24][14]=	-1.49696;
     EtaROImin[1][24][15]=   0.677222;EtaROImax[1][24][15]=   0.742545;PhiROImin[1][24][15]=	-1.5708;PhiROImax[1][24][15]=	-1.49696;
     EtaROImin[1][24][16]=   0.853359;EtaROImax[1][24][16]=   0.895163;PhiROImin[1][24][16]=   -1.56783;PhiROImax[1][24][16]=	-1.45758;
     EtaROImin[1][24][17]=   0.783867;EtaROImax[1][24][17]=   0.850528;PhiROImin[1][24][17]=   -1.56783;PhiROImax[1][24][17]=	-1.45758;
     EtaROImin[1][24][18]=   0.853359;EtaROImax[1][24][18]=   0.895163;PhiROImin[1][24][18]=   -1.45406;PhiROImax[1][24][18]=	-1.34663;
     EtaROImin[1][24][19]=   0.783867;EtaROImax[1][24][19]=   0.850528;PhiROImin[1][24][19]=   -1.45406;PhiROImax[1][24][19]=	-1.34663;
     EtaROImin[1][24][20]=   0.964674;EtaROImax[1][24][20]=    1.02514;PhiROImin[1][24][20]=   -1.56784;PhiROImax[1][24][20]=	-1.45818;
     EtaROImin[1][24][21]=   0.898201;EtaROImax[1][24][21]=   0.961344;PhiROImin[1][24][21]=   -1.56784;PhiROImax[1][24][21]=	-1.45818;
     EtaROImin[1][24][22]=   0.964674;EtaROImax[1][24][22]=    1.02514;PhiROImin[1][24][22]=   -1.45468;PhiROImax[1][24][22]=	-1.34779;
     EtaROImin[1][24][23]=   0.898201;EtaROImax[1][24][23]=   0.961344;PhiROImin[1][24][23]=   -1.45468;PhiROImax[1][24][23]=	-1.34779;
     EtaROImin[1][24][24]=	    0;EtaROImax[1][24][24]=	     0;PhiROImin[1][24][24]=	      0;PhiROImax[1][24][24]=	       0;
     EtaROImin[1][24][25]=    1.03003;EtaROImax[1][24][25]=    1.06547;PhiROImin[1][24][25]=   -1.56773;PhiROImax[1][24][25]=	-1.45737;
     EtaROImin[1][24][26]=	    0;EtaROImax[1][24][26]=	     0;PhiROImin[1][24][26]=	      0;PhiROImax[1][24][26]=	       0;
     EtaROImin[1][24][27]=    1.03003;EtaROImax[1][24][27]=    1.06547;PhiROImin[1][24][27]=   -1.45384;PhiROImax[1][24][27]=	-1.37365;
     EtaROImin[1][24][28]=	    0;EtaROImax[1][24][28]=	     0;PhiROImin[1][24][28]=	      0;PhiROImax[1][24][28]=	       0;
     EtaROImin[1][24][29]=	    0;EtaROImax[1][24][29]=	     0;PhiROImin[1][24][29]=	      0;PhiROImax[1][24][29]=	       0;
     EtaROImin[1][24][30]=	    0;EtaROImax[1][24][30]=	     0;PhiROImin[1][24][30]=	      0;PhiROImax[1][24][30]=	       0;
     EtaROImin[1][24][31]=	    0;EtaROImax[1][24][31]=	     0;PhiROImin[1][24][31]=	      0;PhiROImax[1][24][31]=	       0;
     EtaROImin[1][25][ 0]=  0.0775634;EtaROImax[1][25][ 0]=   0.126072;PhiROImin[1][25][ 0]=   -1.26484;PhiROImax[1][25][ 0]=	-1.18089;
     EtaROImin[1][25][ 1]=   0.129296;EtaROImax[1][25][ 1]=   0.203032;PhiROImin[1][25][ 1]=   -1.26484;PhiROImax[1][25][ 1]=	-1.18089;
     EtaROImin[1][25][ 2]=  0.0775634;EtaROImax[1][25][ 2]=   0.126072;PhiROImin[1][25][ 2]=   -1.35115;PhiROImax[1][25][ 2]=	-1.26847;
     EtaROImin[1][25][ 3]=   0.129296;EtaROImax[1][25][ 3]=   0.203032;PhiROImin[1][25][ 3]=   -1.35115;PhiROImax[1][25][ 3]=	-1.26847;
     EtaROImin[1][25][ 4]=   0.200823;EtaROImax[1][25][ 4]=   0.248155;PhiROImin[1][25][ 4]=   -1.26443;PhiROImax[1][25][ 4]=	-1.18087;
     EtaROImin[1][25][ 5]=   0.251292;EtaROImax[1][25][ 5]=   0.322732;PhiROImin[1][25][ 5]=   -1.26443;PhiROImax[1][25][ 5]=	-1.18087;
     EtaROImin[1][25][ 6]=   0.200823;EtaROImax[1][25][ 6]=   0.248155;PhiROImin[1][25][ 6]=   -1.35034;PhiROImax[1][25][ 6]=	-1.26804;
     EtaROImin[1][25][ 7]=   0.251292;EtaROImax[1][25][ 7]=   0.322732;PhiROImin[1][25][ 7]=   -1.35034;PhiROImax[1][25][ 7]=	-1.26804;
     EtaROImin[1][25][ 8]=   0.361231;EtaROImax[1][25][ 8]=   0.439522;PhiROImin[1][25][ 8]=   -1.25967;PhiROImax[1][25][ 8]=	-1.18023;
     EtaROImin[1][25][ 9]=   0.440504;EtaROImax[1][25][ 9]=   0.516583;PhiROImin[1][25][ 9]=   -1.25967;PhiROImax[1][25][ 9]=	-1.18023;
     EtaROImin[1][25][10]=   0.361231;EtaROImax[1][25][10]=   0.439522;PhiROImin[1][25][10]=   -1.34059;PhiROImax[1][25][10]=	-1.26222;
     EtaROImin[1][25][11]=   0.440504;EtaROImax[1][25][11]=   0.516583;PhiROImin[1][25][11]=   -1.34059;PhiROImax[1][25][11]=	-1.26222;
     EtaROImin[1][25][12]=   0.470878;EtaROImax[1][25][12]=   0.569141;PhiROImin[1][25][12]=   -1.26484;PhiROImax[1][25][12]=	-1.18089;
     EtaROImin[1][25][13]=   0.566067;EtaROImax[1][25][13]=   0.659332;PhiROImin[1][25][13]=   -1.26484;PhiROImax[1][25][13]=	-1.18089;
     EtaROImin[1][25][14]=   0.470878;EtaROImax[1][25][14]=   0.569141;PhiROImin[1][25][14]=   -1.35115;PhiROImax[1][25][14]=	-1.26847;
     EtaROImin[1][25][15]=   0.566067;EtaROImax[1][25][15]=   0.659332;PhiROImin[1][25][15]=   -1.35115;PhiROImax[1][25][15]=	-1.26847;
     EtaROImin[1][25][16]=   0.620212;EtaROImax[1][25][16]=   0.672035;PhiROImin[1][25][16]=   -1.25967;PhiROImax[1][25][16]=	-1.18023;
     EtaROImin[1][25][17]=   0.673648;EtaROImax[1][25][17]=   0.724075;PhiROImin[1][25][17]=   -1.25967;PhiROImax[1][25][17]=	-1.18023;
     EtaROImin[1][25][18]=   0.620212;EtaROImax[1][25][18]=   0.672035;PhiROImin[1][25][18]=   -1.34059;PhiROImax[1][25][18]=	-1.26222;
     EtaROImin[1][25][19]=   0.673648;EtaROImax[1][25][19]=   0.724075;PhiROImin[1][25][19]=   -1.34059;PhiROImax[1][25][19]=	-1.26222;
     EtaROImin[1][25][20]=   0.744499;EtaROImax[1][25][20]=   0.831846;PhiROImin[1][25][20]=   -1.25997;PhiROImax[1][25][20]=	-1.18024;
     EtaROImin[1][25][21]=   0.833797;EtaROImax[1][25][21]=   0.893028;PhiROImin[1][25][21]=   -1.25997;PhiROImax[1][25][21]=	-1.18024;
     EtaROImin[1][25][22]=   0.744499;EtaROImax[1][25][22]=   0.831846;PhiROImin[1][25][22]=   -1.34117;PhiROImax[1][25][22]=	-1.26253;
     EtaROImin[1][25][23]=   0.833797;EtaROImax[1][25][23]=   0.893028;PhiROImin[1][25][23]=   -1.34117;PhiROImax[1][25][23]=	-1.26253;
     EtaROImin[1][25][24]=   0.788882;EtaROImax[1][25][24]=   0.850339;PhiROImin[1][25][24]=   -1.26484;PhiROImax[1][25][24]=	-1.18089;
     EtaROImin[1][25][25]=   0.846464;EtaROImax[1][25][25]=   0.905303;PhiROImin[1][25][25]=   -1.26484;PhiROImax[1][25][25]=	-1.18089;
     EtaROImin[1][25][26]=   0.788882;EtaROImax[1][25][26]=   0.850339;PhiROImin[1][25][26]=   -1.35115;PhiROImax[1][25][26]=	-1.26847;
     EtaROImin[1][25][27]=   0.846464;EtaROImax[1][25][27]=   0.905303;PhiROImin[1][25][27]=   -1.35115;PhiROImax[1][25][27]=	-1.26847;
     EtaROImin[1][25][28]=   0.914143;EtaROImax[1][25][28]=	0.9416;PhiROImin[1][25][28]=   -1.20642;PhiROImax[1][25][28]=	-1.15713;
     EtaROImin[1][25][29]=   0.943411;EtaROImax[1][25][29]=   0.984405;PhiROImin[1][25][29]=   -1.20642;PhiROImax[1][25][29]=	-1.15713;
     EtaROImin[1][25][30]=   0.914143;EtaROImax[1][25][30]=	0.9416;PhiROImin[1][25][30]=   -1.29487;PhiROImax[1][25][30]=	 -1.2092;
     EtaROImin[1][25][31]=   0.943411;EtaROImax[1][25][31]=   0.984405;PhiROImin[1][25][31]=   -1.29487;PhiROImax[1][25][31]=	 -1.2092;
     EtaROImin[1][26][ 0]=   0.155043;EtaROImax[1][26][ 0]=   0.203032;PhiROImin[1][26][ 0]=   -1.17531;PhiROImax[1][26][ 0]=	-1.09135;
     EtaROImin[1][26][ 1]=  0.0775634;EtaROImax[1][26][ 1]=    0.15183;PhiROImin[1][26][ 1]=   -1.17531;PhiROImax[1][26][ 1]=	-1.09135;
     EtaROImin[1][26][ 2]=   0.155043;EtaROImax[1][26][ 2]=   0.203032;PhiROImin[1][26][ 2]=   -1.08772;PhiROImax[1][26][ 2]=	-1.00505;
     EtaROImin[1][26][ 3]=  0.0775634;EtaROImax[1][26][ 3]=    0.15183;PhiROImin[1][26][ 3]=   -1.08772;PhiROImax[1][26][ 3]=	-1.00505;
     EtaROImin[1][26][ 4]=   0.276301;EtaROImax[1][26][ 4]=   0.322732;PhiROImin[1][26][ 4]=   -1.17532;PhiROImax[1][26][ 4]=	-1.09176;
     EtaROImin[1][26][ 5]=   0.200823;EtaROImax[1][26][ 5]=   0.273184;PhiROImin[1][26][ 5]=   -1.17532;PhiROImax[1][26][ 5]=	-1.09176;
     EtaROImin[1][26][ 6]=   0.276301;EtaROImax[1][26][ 6]=   0.322732;PhiROImin[1][26][ 6]=   -1.08815;PhiROImax[1][26][ 6]=	-1.00586;
     EtaROImin[1][26][ 7]=   0.200823;EtaROImax[1][26][ 7]=   0.273184;PhiROImin[1][26][ 7]=   -1.08815;PhiROImax[1][26][ 7]=	-1.00586;
     EtaROImin[1][26][ 8]=   0.440504;EtaROImax[1][26][ 8]=   0.516583;PhiROImin[1][26][ 8]=   -1.17596;PhiROImax[1][26][ 8]=	-1.09652;
     EtaROImin[1][26][ 9]=   0.361231;EtaROImax[1][26][ 9]=   0.439522;PhiROImin[1][26][ 9]=   -1.17596;PhiROImax[1][26][ 9]=	-1.09652;
     EtaROImin[1][26][10]=   0.440504;EtaROImax[1][26][10]=   0.516583;PhiROImin[1][26][10]=   -1.09397;PhiROImax[1][26][10]=	 -1.0156;
     EtaROImin[1][26][11]=   0.361231;EtaROImax[1][26][11]=   0.439522;PhiROImin[1][26][11]=   -1.09397;PhiROImax[1][26][11]=	 -1.0156;
     EtaROImin[1][26][12]=   0.566067;EtaROImax[1][26][12]=   0.659332;PhiROImin[1][26][12]=   -1.17531;PhiROImax[1][26][12]=	-1.09135;
     EtaROImin[1][26][13]=   0.470878;EtaROImax[1][26][13]=   0.569141;PhiROImin[1][26][13]=   -1.17531;PhiROImax[1][26][13]=	-1.09135;
     EtaROImin[1][26][14]=   0.566067;EtaROImax[1][26][14]=   0.659332;PhiROImin[1][26][14]=   -1.08772;PhiROImax[1][26][14]=	-1.00505;
     EtaROImin[1][26][15]=   0.470878;EtaROImax[1][26][15]=   0.569141;PhiROImin[1][26][15]=   -1.08772;PhiROImax[1][26][15]=	-1.00505;
     EtaROImin[1][26][16]=   0.673648;EtaROImax[1][26][16]=   0.724075;PhiROImin[1][26][16]=   -1.17596;PhiROImax[1][26][16]=	-1.09652;
     EtaROImin[1][26][17]=   0.620212;EtaROImax[1][26][17]=   0.672035;PhiROImin[1][26][17]=   -1.17596;PhiROImax[1][26][17]=	-1.09652;
     EtaROImin[1][26][18]=   0.673648;EtaROImax[1][26][18]=   0.724075;PhiROImin[1][26][18]=   -1.09397;PhiROImax[1][26][18]=	 -1.0156;
     EtaROImin[1][26][19]=   0.620212;EtaROImax[1][26][19]=   0.672035;PhiROImin[1][26][19]=   -1.09397;PhiROImax[1][26][19]=	 -1.0156;
     EtaROImin[1][26][20]=   0.833797;EtaROImax[1][26][20]=   0.893028;PhiROImin[1][26][20]=   -1.17595;PhiROImax[1][26][20]=	-1.09623;
     EtaROImin[1][26][21]=   0.744499;EtaROImax[1][26][21]=   0.831846;PhiROImin[1][26][21]=   -1.17595;PhiROImax[1][26][21]=	-1.09623;
     EtaROImin[1][26][22]=   0.833797;EtaROImax[1][26][22]=   0.893028;PhiROImin[1][26][22]=   -1.09367;PhiROImax[1][26][22]=	-1.01502;
     EtaROImin[1][26][23]=   0.744499;EtaROImax[1][26][23]=   0.831846;PhiROImin[1][26][23]=   -1.09367;PhiROImax[1][26][23]=	-1.01502;
     EtaROImin[1][26][24]=   0.846464;EtaROImax[1][26][24]=   0.905303;PhiROImin[1][26][24]=   -1.17531;PhiROImax[1][26][24]=	-1.09135;
     EtaROImin[1][26][25]=   0.788882;EtaROImax[1][26][25]=   0.850339;PhiROImin[1][26][25]=   -1.17531;PhiROImax[1][26][25]=	-1.09135;
     EtaROImin[1][26][26]=   0.846464;EtaROImax[1][26][26]=   0.905303;PhiROImin[1][26][26]=   -1.08772;PhiROImax[1][26][26]=	-1.00505;
     EtaROImin[1][26][27]=   0.788882;EtaROImax[1][26][27]=   0.850339;PhiROImin[1][26][27]=   -1.08772;PhiROImax[1][26][27]=	-1.00505;
     EtaROImin[1][26][28]=   0.955553;EtaROImax[1][26][28]=    0.98211;PhiROImin[1][26][28]=   -1.14955;PhiROImax[1][26][28]=	-1.06384;
     EtaROImin[1][26][29]=   0.911945;EtaROImax[1][26][29]=   0.953764;PhiROImin[1][26][29]=   -1.14955;PhiROImax[1][26][29]=	-1.06384;
     EtaROImin[1][26][30]=   0.955553;EtaROImax[1][26][30]=    0.98211;PhiROImin[1][26][30]=   -1.15713;PhiROImax[1][26][30]=	 -1.0611;
     EtaROImin[1][26][31]=   0.911945;EtaROImax[1][26][31]=   0.953764;PhiROImin[1][26][31]=   -1.15713;PhiROImax[1][26][31]=	 -1.0611;
     EtaROImin[1][27][ 0]=  0.0874905;EtaROImax[1][27][ 0]=   0.145912;PhiROImin[1][27][ 0]=   -0.89861;PhiROImax[1][27][ 0]=  -0.788366;
     EtaROImin[1][27][ 1]=   0.149792;EtaROImax[1][27][ 1]=   0.238294;PhiROImin[1][27][ 1]=   -0.89861;PhiROImax[1][27][ 1]=  -0.788366;
     EtaROImin[1][27][ 2]=  0.0874905;EtaROImax[1][27][ 2]=   0.145912;PhiROImin[1][27][ 2]=   -1.00957;PhiROImax[1][27][ 2]=  -0.902135;
     EtaROImin[1][27][ 3]=   0.149792;EtaROImax[1][27][ 3]=   0.238294;PhiROImin[1][27][ 3]=   -1.00957;PhiROImax[1][27][ 3]=  -0.902135;
     EtaROImin[1][27][ 4]=   0.246207;EtaROImax[1][27][ 4]=    0.35049;PhiROImin[1][27][ 4]=  -0.898011;PhiROImax[1][27][ 4]=	-0.78835;
     EtaROImin[1][27][ 5]=   0.350831;EtaROImax[1][27][ 5]=   0.452099;PhiROImin[1][27][ 5]=  -0.898011;PhiROImax[1][27][ 5]=	-0.78835;
     EtaROImin[1][27][ 6]=   0.246207;EtaROImax[1][27][ 6]=    0.35049;PhiROImin[1][27][ 6]=   -1.00841;PhiROImax[1][27][ 6]=  -0.901517;
     EtaROImin[1][27][ 7]=   0.350831;EtaROImax[1][27][ 7]=   0.452099;PhiROImin[1][27][ 7]=   -1.00841;PhiROImax[1][27][ 7]=  -0.901517;
     EtaROImin[1][27][ 8]=   0.458091;EtaROImax[1][27][ 8]=   0.554444;PhiROImin[1][27][ 8]=  -0.898011;PhiROImax[1][27][ 8]=	-0.78835;
     EtaROImin[1][27][ 9]=   0.555789;EtaROImax[1][27][ 9]=   0.648101;PhiROImin[1][27][ 9]=  -0.898011;PhiROImax[1][27][ 9]=	-0.78835;
     EtaROImin[1][27][10]=   0.458091;EtaROImax[1][27][10]=   0.554444;PhiROImin[1][27][10]=   -1.00841;PhiROImax[1][27][10]=  -0.901517;
     EtaROImin[1][27][11]=   0.555789;EtaROImax[1][27][11]=   0.648101;PhiROImin[1][27][11]=   -1.00841;PhiROImax[1][27][11]=  -0.901517;
     EtaROImin[1][27][12]=   0.655857;EtaROImax[1][27][12]=   0.703298;PhiROImin[1][27][12]=   -0.89861;PhiROImax[1][27][12]=  -0.788366;
     EtaROImin[1][27][13]=   0.706413;EtaROImax[1][27][13]=   0.776449;PhiROImin[1][27][13]=   -0.89861;PhiROImax[1][27][13]=  -0.788366;
     EtaROImin[1][27][14]=   0.655857;EtaROImax[1][27][14]=   0.703298;PhiROImin[1][27][14]=   -1.00957;PhiROImax[1][27][14]=  -0.902135;
     EtaROImin[1][27][15]=   0.706413;EtaROImax[1][27][15]=   0.776449;PhiROImin[1][27][15]=   -1.00957;PhiROImax[1][27][15]=  -0.902135;
     EtaROImin[1][27][16]=   0.783867;EtaROImax[1][27][16]=    0.82768;PhiROImin[1][27][16]=   -0.89861;PhiROImax[1][27][16]=  -0.788366;
     EtaROImin[1][27][17]=   0.830556;EtaROImax[1][27][17]=   0.895163;PhiROImin[1][27][17]=   -0.89861;PhiROImax[1][27][17]=  -0.788366;
     EtaROImin[1][27][18]=   0.783867;EtaROImax[1][27][18]=    0.82768;PhiROImin[1][27][18]=   -1.00957;PhiROImax[1][27][18]=  -0.902135;
     EtaROImin[1][27][19]=   0.830556;EtaROImax[1][27][19]=   0.895163;PhiROImin[1][27][19]=   -1.00957;PhiROImax[1][27][19]=  -0.902135;
     EtaROImin[1][27][20]=   0.898201;EtaROImax[1][27][20]=   0.961344;PhiROImin[1][27][20]=  -0.898011;PhiROImax[1][27][20]=	-0.78835;
     EtaROImin[1][27][21]=   0.964674;EtaROImax[1][27][21]=    1.02514;PhiROImin[1][27][21]=  -0.898011;PhiROImax[1][27][21]=	-0.78835;
     EtaROImin[1][27][22]=   0.898201;EtaROImax[1][27][22]=   0.961344;PhiROImin[1][27][22]=   -1.00841;PhiROImax[1][27][22]=  -0.901517;
     EtaROImin[1][27][23]=   0.964674;EtaROImax[1][27][23]=    1.02514;PhiROImin[1][27][23]=   -1.00841;PhiROImax[1][27][23]=  -0.901517;
     EtaROImin[1][27][24]=    1.03003;EtaROImax[1][27][24]=    1.06547;PhiROImin[1][27][24]=  -0.898827;PhiROImax[1][27][24]=  -0.788467;
     EtaROImin[1][27][25]=	    0;EtaROImax[1][27][25]=	     0;PhiROImin[1][27][25]=	      0;PhiROImax[1][27][25]=	       0;
     EtaROImin[1][27][26]=    1.03003;EtaROImax[1][27][26]=    1.06547;PhiROImin[1][27][26]=  -0.982541;PhiROImax[1][27][26]=  -0.902356;
     EtaROImin[1][27][27]=	    0;EtaROImax[1][27][27]=	     0;PhiROImin[1][27][27]=	      0;PhiROImax[1][27][27]=	       0;
     EtaROImin[1][27][28]=	    0;EtaROImax[1][27][28]=	     0;PhiROImin[1][27][28]=	      0;PhiROImax[1][27][28]=	       0;
     EtaROImin[1][27][29]=	    0;EtaROImax[1][27][29]=	     0;PhiROImin[1][27][29]=	      0;PhiROImax[1][27][29]=	       0;
     EtaROImin[1][27][30]=	    0;EtaROImax[1][27][30]=	     0;PhiROImin[1][27][30]=	      0;PhiROImax[1][27][30]=	       0;
     EtaROImin[1][27][31]=	    0;EtaROImax[1][27][31]=	     0;PhiROImin[1][27][31]=	      0;PhiROImax[1][27][31]=	       0;
     EtaROImin[1][28][ 0]=   0.180742;EtaROImax[1][28][ 0]=   0.238294;PhiROImin[1][28][ 0]=   -0.78243;PhiROImax[1][28][ 0]=  -0.672186;
     EtaROImin[1][28][ 1]=  0.0874905;EtaROImax[1][28][ 1]=   0.176882;PhiROImin[1][28][ 1]=   -0.78243;PhiROImax[1][28][ 1]=  -0.672186;
     EtaROImin[1][28][ 2]=   0.180742;EtaROImax[1][28][ 2]=   0.238294;PhiROImin[1][28][ 2]=  -0.668661;PhiROImax[1][28][ 2]=  -0.561229;
     EtaROImin[1][28][ 3]=  0.0874905;EtaROImax[1][28][ 3]=   0.176882;PhiROImin[1][28][ 3]=  -0.668661;PhiROImax[1][28][ 3]=  -0.561229;
     EtaROImin[1][28][ 4]=   0.350831;EtaROImax[1][28][ 4]=   0.452099;PhiROImin[1][28][ 4]=  -0.782446;PhiROImax[1][28][ 4]=  -0.672786;
     EtaROImin[1][28][ 5]=   0.246207;EtaROImax[1][28][ 5]=    0.35049;PhiROImin[1][28][ 5]=  -0.782446;PhiROImax[1][28][ 5]=  -0.672786;
     EtaROImin[1][28][ 6]=   0.350831;EtaROImax[1][28][ 6]=   0.452099;PhiROImin[1][28][ 6]=  -0.669279;PhiROImax[1][28][ 6]=  -0.562387;
     EtaROImin[1][28][ 7]=   0.246207;EtaROImax[1][28][ 7]=    0.35049;PhiROImin[1][28][ 7]=  -0.669279;PhiROImax[1][28][ 7]=  -0.562387;
     EtaROImin[1][28][ 8]=   0.555789;EtaROImax[1][28][ 8]=   0.648101;PhiROImin[1][28][ 8]=  -0.782446;PhiROImax[1][28][ 8]=  -0.672786;
     EtaROImin[1][28][ 9]=   0.458091;EtaROImax[1][28][ 9]=   0.554444;PhiROImin[1][28][ 9]=  -0.782446;PhiROImax[1][28][ 9]=  -0.672786;
     EtaROImin[1][28][10]=   0.555789;EtaROImax[1][28][10]=   0.648101;PhiROImin[1][28][10]=  -0.669279;PhiROImax[1][28][10]=  -0.562387;
     EtaROImin[1][28][11]=   0.458091;EtaROImax[1][28][11]=   0.554444;PhiROImin[1][28][11]=  -0.669279;PhiROImax[1][28][11]=  -0.562387;
     EtaROImin[1][28][12]=   0.731124;EtaROImax[1][28][12]=   0.776449;PhiROImin[1][28][12]=   -0.78243;PhiROImax[1][28][12]=  -0.672186;
     EtaROImin[1][28][13]=   0.655857;EtaROImax[1][28][13]=   0.728056;PhiROImin[1][28][13]=   -0.78243;PhiROImax[1][28][13]=  -0.672186;
     EtaROImin[1][28][14]=   0.731124;EtaROImax[1][28][14]=   0.776449;PhiROImin[1][28][14]=  -0.668661;PhiROImax[1][28][14]=  -0.561229;
     EtaROImin[1][28][15]=   0.655857;EtaROImax[1][28][15]=   0.728056;PhiROImin[1][28][15]=  -0.668661;PhiROImax[1][28][15]=  -0.561229;
     EtaROImin[1][28][16]=   0.853359;EtaROImax[1][28][16]=   0.895163;PhiROImin[1][28][16]=   -0.78243;PhiROImax[1][28][16]=  -0.672186;
     EtaROImin[1][28][17]=   0.783867;EtaROImax[1][28][17]=   0.850528;PhiROImin[1][28][17]=   -0.78243;PhiROImax[1][28][17]=  -0.672186;
     EtaROImin[1][28][18]=   0.853359;EtaROImax[1][28][18]=   0.895163;PhiROImin[1][28][18]=  -0.668661;PhiROImax[1][28][18]=  -0.561229;
     EtaROImin[1][28][19]=   0.783867;EtaROImax[1][28][19]=   0.850528;PhiROImin[1][28][19]=  -0.668661;PhiROImax[1][28][19]=  -0.561229;
     EtaROImin[1][28][20]=   0.964674;EtaROImax[1][28][20]=    1.02514;PhiROImin[1][28][20]=  -0.782446;PhiROImax[1][28][20]=  -0.672786;
     EtaROImin[1][28][21]=   0.898201;EtaROImax[1][28][21]=   0.961344;PhiROImin[1][28][21]=  -0.782446;PhiROImax[1][28][21]=  -0.672786;
     EtaROImin[1][28][22]=   0.964674;EtaROImax[1][28][22]=    1.02514;PhiROImin[1][28][22]=  -0.669279;PhiROImax[1][28][22]=  -0.562387;
     EtaROImin[1][28][23]=   0.898201;EtaROImax[1][28][23]=   0.961344;PhiROImin[1][28][23]=  -0.669279;PhiROImax[1][28][23]=  -0.562387;
     EtaROImin[1][28][24]=	    0;EtaROImax[1][28][24]=	     0;PhiROImin[1][28][24]=	      0;PhiROImax[1][28][24]=	       0;
     EtaROImin[1][28][25]=    1.03003;EtaROImax[1][28][25]=    1.06547;PhiROImin[1][28][25]=  -0.782329;PhiROImax[1][28][25]=  -0.671969;
     EtaROImin[1][28][26]=	    0;EtaROImax[1][28][26]=	     0;PhiROImin[1][28][26]=	      0;PhiROImax[1][28][26]=	       0;
     EtaROImin[1][28][27]=    1.03003;EtaROImax[1][28][27]=    1.06547;PhiROImin[1][28][27]=   -0.66844;PhiROImax[1][28][27]=  -0.588255;
     EtaROImin[1][28][28]=	    0;EtaROImax[1][28][28]=	     0;PhiROImin[1][28][28]=	      0;PhiROImax[1][28][28]=	       0;
     EtaROImin[1][28][29]=	    0;EtaROImax[1][28][29]=	     0;PhiROImin[1][28][29]=	      0;PhiROImax[1][28][29]=	       0;
     EtaROImin[1][28][30]=	    0;EtaROImax[1][28][30]=	     0;PhiROImin[1][28][30]=	      0;PhiROImax[1][28][30]=	       0;
     EtaROImin[1][28][31]=	    0;EtaROImax[1][28][31]=	     0;PhiROImin[1][28][31]=	      0;PhiROImax[1][28][31]=	       0;
     EtaROImin[1][29][ 0]=  0.0208251;EtaROImax[1][29][ 0]=   0.118734;PhiROImin[1][29][ 0]=  -0.479445;PhiROImax[1][29][ 0]=  -0.395489;
     EtaROImin[1][29][ 1]=   0.116816;EtaROImax[1][29][ 1]=   0.213185;PhiROImin[1][29][ 1]=  -0.479445;PhiROImax[1][29][ 1]=  -0.395489;
     EtaROImin[1][29][ 2]=  0.0208251;EtaROImax[1][29][ 2]=   0.118734;PhiROImin[1][29][ 2]=  -0.565749;PhiROImax[1][29][ 2]=  -0.483076;
     EtaROImin[1][29][ 3]=   0.116816;EtaROImax[1][29][ 3]=   0.213185;PhiROImin[1][29][ 3]=  -0.565749;PhiROImax[1][29][ 3]=  -0.483076;
     EtaROImin[1][29][ 4]=   0.222449;EtaROImax[1][29][ 4]=   0.302928;PhiROImin[1][29][ 4]=  -0.479445;PhiROImax[1][29][ 4]=  -0.395489;
     EtaROImin[1][29][ 5]=    0.30075;EtaROImax[1][29][ 5]=   0.416721;PhiROImin[1][29][ 5]=  -0.479445;PhiROImax[1][29][ 5]=  -0.395489;
     EtaROImin[1][29][ 6]=   0.222449;EtaROImax[1][29][ 6]=   0.302928;PhiROImin[1][29][ 6]=  -0.565749;PhiROImax[1][29][ 6]=  -0.483076;
     EtaROImin[1][29][ 7]=    0.30075;EtaROImax[1][29][ 7]=   0.416721;PhiROImin[1][29][ 7]=  -0.565749;PhiROImax[1][29][ 7]=  -0.483076;
     EtaROImin[1][29][ 8]=    0.42967;EtaROImax[1][29][ 8]=   0.504617;PhiROImin[1][29][ 8]=  -0.479445;PhiROImax[1][29][ 8]=  -0.395489;
     EtaROImin[1][29][ 9]=   0.501681;EtaROImax[1][29][ 9]=   0.573871;PhiROImin[1][29][ 9]=  -0.479445;PhiROImax[1][29][ 9]=  -0.395489;
     EtaROImin[1][29][10]=    0.42967;EtaROImax[1][29][10]=   0.504617;PhiROImin[1][29][10]=  -0.565749;PhiROImax[1][29][10]=  -0.483076;
     EtaROImin[1][29][11]=   0.501681;EtaROImax[1][29][11]=   0.573871;PhiROImin[1][29][11]=  -0.565749;PhiROImax[1][29][11]=  -0.483076;
     EtaROImin[1][29][12]=   0.583785;EtaROImax[1][29][12]=   0.653329;PhiROImin[1][29][12]=  -0.479445;PhiROImax[1][29][12]=  -0.395489;
     EtaROImin[1][29][13]=   0.649934;EtaROImax[1][29][13]=   0.741516;PhiROImin[1][29][13]=  -0.479445;PhiROImax[1][29][13]=  -0.395489;
     EtaROImin[1][29][14]=   0.583785;EtaROImax[1][29][14]=   0.653329;PhiROImin[1][29][14]=  -0.565749;PhiROImax[1][29][14]=  -0.483076;
     EtaROImin[1][29][15]=   0.649934;EtaROImax[1][29][15]=   0.741516;PhiROImin[1][29][15]=  -0.565749;PhiROImax[1][29][15]=  -0.483076;
     EtaROImin[1][29][16]=   0.756521;EtaROImax[1][29][16]=   0.837822;PhiROImin[1][29][16]=  -0.479445;PhiROImax[1][29][16]=  -0.395489;
     EtaROImin[1][29][17]=	    0;EtaROImax[1][29][17]=	     0;PhiROImin[1][29][17]=	      0;PhiROImax[1][29][17]=	       0;
     EtaROImin[1][29][18]=   0.756521;EtaROImax[1][29][18]=   0.837822;PhiROImin[1][29][18]=  -0.565749;PhiROImax[1][29][18]=  -0.483076;
     EtaROImin[1][29][19]=	    0;EtaROImax[1][29][19]=	     0;PhiROImin[1][29][19]=	      0;PhiROImax[1][29][19]=	       0;
     EtaROImin[1][29][20]=   0.844116;EtaROImax[1][29][20]=   0.903324;PhiROImin[1][29][20]=  -0.479445;PhiROImax[1][29][20]=  -0.395489;
     EtaROImin[1][29][21]=   0.899344;EtaROImax[1][29][21]=   0.956037;PhiROImin[1][29][21]=  -0.479445;PhiROImax[1][29][21]=  -0.395489;
     EtaROImin[1][29][22]=   0.844116;EtaROImax[1][29][22]=   0.903324;PhiROImin[1][29][22]=  -0.565749;PhiROImax[1][29][22]=  -0.483076;
     EtaROImin[1][29][23]=   0.899344;EtaROImax[1][29][23]=   0.956037;PhiROImin[1][29][23]=  -0.565749;PhiROImax[1][29][23]=  -0.483076;
     EtaROImin[1][29][24]=	    0;EtaROImax[1][29][24]=	     0;PhiROImin[1][29][24]=	      0;PhiROImax[1][29][24]=	       0;
     EtaROImin[1][29][25]=	    0;EtaROImax[1][29][25]=	     0;PhiROImin[1][29][25]=	      0;PhiROImax[1][29][25]=	       0;
     EtaROImin[1][29][26]=	    0;EtaROImax[1][29][26]=	     0;PhiROImin[1][29][26]=	      0;PhiROImax[1][29][26]=	       0;
     EtaROImin[1][29][27]=	    0;EtaROImax[1][29][27]=	     0;PhiROImin[1][29][27]=	      0;PhiROImax[1][29][27]=	       0;
     EtaROImin[1][29][28]=	    0;EtaROImax[1][29][28]=	     0;PhiROImin[1][29][28]=	      0;PhiROImax[1][29][28]=	       0;
     EtaROImin[1][29][29]=	    0;EtaROImax[1][29][29]=	     0;PhiROImin[1][29][29]=	      0;PhiROImax[1][29][29]=	       0;
     EtaROImin[1][29][30]=	    0;EtaROImax[1][29][30]=	     0;PhiROImin[1][29][30]=	      0;PhiROImax[1][29][30]=	       0;
     EtaROImin[1][29][31]=	    0;EtaROImax[1][29][31]=	     0;PhiROImin[1][29][31]=	      0;PhiROImax[1][29][31]=	       0;
     EtaROImin[1][30][ 0]=   0.116816;EtaROImax[1][30][ 0]=   0.213185;PhiROImin[1][30][ 0]=  -0.389909;PhiROImax[1][30][ 0]=  -0.305953;
     EtaROImin[1][30][ 1]=  0.0208251;EtaROImax[1][30][ 1]=   0.118734;PhiROImin[1][30][ 1]=  -0.389909;PhiROImax[1][30][ 1]=  -0.305953;
     EtaROImin[1][30][ 2]=   0.116816;EtaROImax[1][30][ 2]=   0.213185;PhiROImin[1][30][ 2]=  -0.302322;PhiROImax[1][30][ 2]=  -0.219649;
     EtaROImin[1][30][ 3]=  0.0208251;EtaROImax[1][30][ 3]=   0.118734;PhiROImin[1][30][ 3]=  -0.302322;PhiROImax[1][30][ 3]=  -0.219649;
     EtaROImin[1][30][ 4]=    0.30075;EtaROImax[1][30][ 4]=   0.416721;PhiROImin[1][30][ 4]=  -0.389909;PhiROImax[1][30][ 4]=  -0.305953;
     EtaROImin[1][30][ 5]=   0.222449;EtaROImax[1][30][ 5]=   0.302928;PhiROImin[1][30][ 5]=  -0.389909;PhiROImax[1][30][ 5]=  -0.305953;
     EtaROImin[1][30][ 6]=    0.30075;EtaROImax[1][30][ 6]=   0.416721;PhiROImin[1][30][ 6]=  -0.302322;PhiROImax[1][30][ 6]=  -0.219649;
     EtaROImin[1][30][ 7]=   0.222449;EtaROImax[1][30][ 7]=   0.302928;PhiROImin[1][30][ 7]=  -0.302322;PhiROImax[1][30][ 7]=  -0.219649;
     EtaROImin[1][30][ 8]=   0.501681;EtaROImax[1][30][ 8]=   0.573871;PhiROImin[1][30][ 8]=  -0.389909;PhiROImax[1][30][ 8]=  -0.305953;
     EtaROImin[1][30][ 9]=    0.42967;EtaROImax[1][30][ 9]=   0.504617;PhiROImin[1][30][ 9]=  -0.389909;PhiROImax[1][30][ 9]=  -0.305953;
     EtaROImin[1][30][10]=   0.501681;EtaROImax[1][30][10]=   0.573871;PhiROImin[1][30][10]=  -0.302322;PhiROImax[1][30][10]=  -0.219649;
     EtaROImin[1][30][11]=    0.42967;EtaROImax[1][30][11]=   0.504617;PhiROImin[1][30][11]=  -0.302322;PhiROImax[1][30][11]=  -0.219649;
     EtaROImin[1][30][12]=   0.649934;EtaROImax[1][30][12]=   0.741516;PhiROImin[1][30][12]=  -0.389909;PhiROImax[1][30][12]=  -0.305953;
     EtaROImin[1][30][13]=   0.583785;EtaROImax[1][30][13]=   0.653329;PhiROImin[1][30][13]=  -0.389909;PhiROImax[1][30][13]=  -0.305953;
     EtaROImin[1][30][14]=   0.649934;EtaROImax[1][30][14]=   0.741516;PhiROImin[1][30][14]=  -0.302322;PhiROImax[1][30][14]=  -0.219649;
     EtaROImin[1][30][15]=   0.583785;EtaROImax[1][30][15]=   0.653329;PhiROImin[1][30][15]=  -0.302322;PhiROImax[1][30][15]=  -0.219649;
     EtaROImin[1][30][16]=	    0;EtaROImax[1][30][16]=	     0;PhiROImin[1][30][16]=	      0;PhiROImax[1][30][16]=	       0;
     EtaROImin[1][30][17]=   0.756521;EtaROImax[1][30][17]=   0.837822;PhiROImin[1][30][17]=  -0.389909;PhiROImax[1][30][17]=  -0.305953;
     EtaROImin[1][30][18]=	    0;EtaROImax[1][30][18]=	     0;PhiROImin[1][30][18]=	      0;PhiROImax[1][30][18]=	       0;
     EtaROImin[1][30][19]=   0.756521;EtaROImax[1][30][19]=   0.837822;PhiROImin[1][30][19]=  -0.302322;PhiROImax[1][30][19]=  -0.219649;
     EtaROImin[1][30][20]=   0.899344;EtaROImax[1][30][20]=   0.956037;PhiROImin[1][30][20]=  -0.389909;PhiROImax[1][30][20]=  -0.305953;
     EtaROImin[1][30][21]=   0.844116;EtaROImax[1][30][21]=   0.903324;PhiROImin[1][30][21]=  -0.389909;PhiROImax[1][30][21]=  -0.305953;
     EtaROImin[1][30][22]=   0.899344;EtaROImax[1][30][22]=   0.956037;PhiROImin[1][30][22]=  -0.302322;PhiROImax[1][30][22]=  -0.219649;
     EtaROImin[1][30][23]=   0.844116;EtaROImax[1][30][23]=   0.903324;PhiROImin[1][30][23]=  -0.302322;PhiROImax[1][30][23]=  -0.219649;
     EtaROImin[1][30][24]=	    0;EtaROImax[1][30][24]=	     0;PhiROImin[1][30][24]=	      0;PhiROImax[1][30][24]=	       0;
     EtaROImin[1][30][25]=	    0;EtaROImax[1][30][25]=	     0;PhiROImin[1][30][25]=	      0;PhiROImax[1][30][25]=	       0;
     EtaROImin[1][30][26]=	    0;EtaROImax[1][30][26]=	     0;PhiROImin[1][30][26]=	      0;PhiROImax[1][30][26]=	       0;
     EtaROImin[1][30][27]=	    0;EtaROImax[1][30][27]=	     0;PhiROImin[1][30][27]=	      0;PhiROImax[1][30][27]=	       0;
     EtaROImin[1][30][28]=	    0;EtaROImax[1][30][28]=	     0;PhiROImin[1][30][28]=	      0;PhiROImax[1][30][28]=	       0;
     EtaROImin[1][30][29]=	    0;EtaROImax[1][30][29]=	     0;PhiROImin[1][30][29]=	      0;PhiROImax[1][30][29]=	       0;
     EtaROImin[1][30][30]=	    0;EtaROImax[1][30][30]=	     0;PhiROImin[1][30][30]=	      0;PhiROImax[1][30][30]=	       0;
     EtaROImin[1][30][31]=	    0;EtaROImax[1][30][31]=	     0;PhiROImin[1][30][31]=	      0;PhiROImax[1][30][31]=	       0;
     EtaROImin[1][31][ 0]=  0.0231199;EtaROImax[1][31][ 0]=   0.131749;PhiROImin[1][31][ 0]=  -0.112612;PhiROImax[1][31][ 0]=-0.00295181;
     EtaROImin[1][31][ 1]=   0.130922;EtaROImax[1][31][ 1]=   0.238612;PhiROImin[1][31][ 1]=  -0.112612;PhiROImax[1][31][ 1]=-0.00295181;
     EtaROImin[1][31][ 2]=  0.0231199;EtaROImax[1][31][ 2]=   0.131749;PhiROImin[1][31][ 2]=  -0.223011;PhiROImax[1][31][ 2]=  -0.116119;
     EtaROImin[1][31][ 3]=   0.130922;EtaROImax[1][31][ 3]=   0.238612;PhiROImin[1][31][ 3]=  -0.223011;PhiROImax[1][31][ 3]=  -0.116119;
     EtaROImin[1][31][ 4]=   0.246207;EtaROImax[1][31][ 4]=    0.35049;PhiROImin[1][31][ 4]=  -0.112612;PhiROImax[1][31][ 4]=-0.00295181;
     EtaROImin[1][31][ 5]=   0.350831;EtaROImax[1][31][ 5]=   0.452099;PhiROImin[1][31][ 5]=  -0.112612;PhiROImax[1][31][ 5]=-0.00295181;
     EtaROImin[1][31][ 6]=   0.246207;EtaROImax[1][31][ 6]=    0.35049;PhiROImin[1][31][ 6]=  -0.223011;PhiROImax[1][31][ 6]=  -0.116119;
     EtaROImin[1][31][ 7]=   0.350831;EtaROImax[1][31][ 7]=   0.452099;PhiROImin[1][31][ 7]=  -0.223011;PhiROImax[1][31][ 7]=  -0.116119;
     EtaROImin[1][31][ 8]=   0.458091;EtaROImax[1][31][ 8]=   0.554444;PhiROImin[1][31][ 8]=  -0.112612;PhiROImax[1][31][ 8]=-0.00295181;
     EtaROImin[1][31][ 9]=   0.555789;EtaROImax[1][31][ 9]=   0.648101;PhiROImin[1][31][ 9]=  -0.112612;PhiROImax[1][31][ 9]=-0.00295181;
     EtaROImin[1][31][10]=   0.458091;EtaROImax[1][31][10]=   0.554444;PhiROImin[1][31][10]=  -0.223011;PhiROImax[1][31][10]=  -0.116119;
     EtaROImin[1][31][11]=   0.555789;EtaROImax[1][31][11]=   0.648101;PhiROImin[1][31][11]=  -0.223011;PhiROImax[1][31][11]=  -0.116119;
     EtaROImin[1][31][12]=   0.655857;EtaROImax[1][31][12]=   0.703298;PhiROImin[1][31][12]=  -0.113212;PhiROImax[1][31][12]=-0.00296767;
     EtaROImin[1][31][13]=   0.706413;EtaROImax[1][31][13]=   0.776449;PhiROImin[1][31][13]=  -0.113212;PhiROImax[1][31][13]=-0.00296767;
     EtaROImin[1][31][14]=   0.655857;EtaROImax[1][31][14]=   0.703298;PhiROImin[1][31][14]=  -0.224169;PhiROImax[1][31][14]=  -0.116737;
     EtaROImin[1][31][15]=   0.706413;EtaROImax[1][31][15]=   0.776449;PhiROImin[1][31][15]=  -0.224169;PhiROImax[1][31][15]=  -0.116737;
     EtaROImin[1][31][16]=   0.783867;EtaROImax[1][31][16]=    0.82768;PhiROImin[1][31][16]=  -0.113212;PhiROImax[1][31][16]=-0.00296767;
     EtaROImin[1][31][17]=   0.830556;EtaROImax[1][31][17]=   0.895163;PhiROImin[1][31][17]=  -0.113212;PhiROImax[1][31][17]=-0.00296767;
     EtaROImin[1][31][18]=   0.783867;EtaROImax[1][31][18]=    0.82768;PhiROImin[1][31][18]=  -0.224169;PhiROImax[1][31][18]=  -0.116737;
     EtaROImin[1][31][19]=   0.830556;EtaROImax[1][31][19]=   0.895163;PhiROImin[1][31][19]=  -0.224169;PhiROImax[1][31][19]=  -0.116737;
     EtaROImin[1][31][20]=   0.898201;EtaROImax[1][31][20]=   0.961344;PhiROImin[1][31][20]=  -0.112612;PhiROImax[1][31][20]=-0.00295181;
     EtaROImin[1][31][21]=   0.964674;EtaROImax[1][31][21]=    1.02514;PhiROImin[1][31][21]=  -0.112612;PhiROImax[1][31][21]=-0.00295181;
     EtaROImin[1][31][22]=   0.898201;EtaROImax[1][31][22]=   0.961344;PhiROImin[1][31][22]=  -0.223011;PhiROImax[1][31][22]=  -0.116119;
     EtaROImin[1][31][23]=   0.964674;EtaROImax[1][31][23]=    1.02514;PhiROImin[1][31][23]=  -0.223011;PhiROImax[1][31][23]=  -0.116119;
     EtaROImin[1][31][24]=    1.03003;EtaROImax[1][31][24]=    1.06547;PhiROImin[1][31][24]=  -0.113429;PhiROImax[1][31][24]=-0.00306876;
     EtaROImin[1][31][25]=	    0;EtaROImax[1][31][25]=	     0;PhiROImin[1][31][25]=	      0;PhiROImax[1][31][25]=	       0;
     EtaROImin[1][31][26]=    1.03003;EtaROImax[1][31][26]=    1.06547;PhiROImin[1][31][26]=  -0.197143;PhiROImax[1][31][26]=  -0.116958;
     EtaROImin[1][31][27]=	    0;EtaROImax[1][31][27]=	     0;PhiROImin[1][31][27]=	      0;PhiROImax[1][31][27]=	       0;
     EtaROImin[1][31][28]=	    0;EtaROImax[1][31][28]=	     0;PhiROImin[1][31][28]=	      0;PhiROImax[1][31][28]=	       0;
     EtaROImin[1][31][29]=	    0;EtaROImax[1][31][29]=	     0;PhiROImin[1][31][29]=	      0;PhiROImax[1][31][29]=	       0;
     EtaROImin[1][31][30]=	    0;EtaROImax[1][31][30]=	     0;PhiROImin[1][31][30]=	      0;PhiROImax[1][31][30]=	       0;
     EtaROImin[1][31][31]=	    0;EtaROImax[1][31][31]=	     0;PhiROImin[1][31][31]=	      0;PhiROImax[1][31][31]=	       0;

  
  return sc;
}
