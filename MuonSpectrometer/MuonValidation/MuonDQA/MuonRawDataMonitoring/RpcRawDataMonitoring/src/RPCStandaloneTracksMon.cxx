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

#include "MuonReadoutGeometry/RpcReadoutSet.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MuonReadoutElement.h"  
#include "MuonReadoutGeometry/RpcReadoutElement.h"

#include "MuonRDO/RpcFiredChannel.h"
#include "MuonRDO/RpcCoinMatrix.h"
#include "MuonRDO/RpcPad.h"
#include "MuonRDO/RpcPadContainer.h"
 
#include "TrigT1Result/MuCTPI_DataWord_Decoder.h"
#include "TrigT1Interfaces/RecMuonRoI.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "MuonDigitContainer/RpcDigitContainer.h"
 
#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonDQAUtils/MuonCosmicSetup.h"
#include "MuonDQAUtils/MuonDQAHistMap.h" 

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
#include <cmath>

static const   int timeminrange	      =	 -200;
static const   int timemaxrange	      =	  200;
static const   int timeNbin	      =	  128;
static const   int nstripfiducial     =     2;
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
  declareProperty("RPCTriggerContainer", m_key_trig		= "RPC_triggerHits"	);
  declareProperty("MinimunEntries",      m_MinEntries		= 10	);    // min entries required for summary plot 
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
  
  declareProperty("MuonSegmentCollection",	     m_muonSegmentsName     = "MuonSegments");
  declareProperty("MuonTrackCollection",	     m_muonTracksName	    = "MuonSpectrometerTrackParticles");
  declareProperty("MuonExtrapolatedTrackCollection", m_muonExtrapTracksName = "ExtrapolatedMuonTrackParticles");
  declareProperty("InDetTrackParticles",	     m_innerTracksName      = "InDetTrackParticles");
  declareProperty("MSVertexCollection",              m_msVertexCollection   = "MSDisplacedVertex");
  
  
  
  declareProperty("MuonDeltaRMatching"          , m_MuonDeltaRMatching         =   0.15 ); 
  declareProperty("requireMuonCombinedTight"    , m_requireMuonCombinedTight   = false  );
  declareProperty("StandAloneMatchedWithTrack"  , m_StandAloneMatchedWithTrack = true   );

  declareProperty("isMC"                        , m_isMC                       = false  );
  
  declareProperty( "selectTriggerChainGroup"    , m_selectTriggerChainGroup    = false     );
  declareProperty( "deSelectTriggerChainGroup"  , m_deselectTriggerChainGroup  = false     );
  declareProperty( "triggerChainGroupRegNot"    , m_triggerChainGroupRegNot    ="HLT_j.*" ); 
  declareProperty( "triggerChainGroupRegExp"    , m_triggerChainGroupRegExp    ="HLT_mu.*"  );//".*" all triggers//"HLT_mu.*" all EF muon triggers//"HLT_.*" all EF triggers//"L2_.*"  all L2 triggers//"L1_.*"  all L1 triggers 

  
  m_padsId     = 0;
  m_chambersId = 0;

  m_chainGroupSelect=nullptr;
  m_chainGroupVeto=nullptr;


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

  // Initialize the IdHelper
  StoreGateSvc* detStore = 0;
  sc = service("DetectorStore", detStore);
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "DetectorStore service not found !" );
    return StatusCode::FAILURE;
  }   
  
  // MuonDetectorManager from the conditions store
  ATH_CHECK(m_DetectorManagerKey.initialize());

  ATH_CHECK(m_idHelperSvc.retrieve() );
    
  m_hardware_name_list.push_back("XXX");
  
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
  else {
    m_trigDec.disable();
  }
 
  m_rpc_eventstotal=0;
  
  // Clear Muon Monitoring Histograms 
  m_SummaryHist.clear();
  m_SummaryHistRPCBA.clear();
  m_SummaryHistRPCBC.clear();
  m_sumTmpHist.clear();
  m_rpcAverageSide_A.clear(); 
  m_rpcAverageSide_C.clear();  
  
  m_hRPCPhiEtaCoinThr.clear()	 ;
  m_hRPCPadThr.clear()		 ;
  m_hRPCMuctpiThr.clear()  	 ;
  
  m_hRPCPhiEtaCoinThr_eff.clear()	 ;
  m_hRPCPadThr_eff.clear()		 ;
  m_hRPCMuctpiThr_eff.clear()  	 ;
	 
  ATH_CHECK(m_key_rpc.initialize());
  ATH_CHECK(m_eventInfo.initialize());
  ATH_CHECK(m_clusterContainerName.initialize());
  ATH_CHECK(m_muCTPI_RDO_key.initialize());
  ATH_CHECK(m_sectorLogicContainerKey.initialize(!m_isMC));
  ATH_CHECK(m_rpc_coin_key.initialize());
  ATH_CHECK(m_muonsName.initialize());

  return StatusCode::SUCCESS;
}

/*----------------------------------------------------------------------------------*/
StatusCode RPCStandaloneTracksMon::fillHistograms()
/*----------------------------------------------------------------------------------*/
{
  StatusCode sc = StatusCode::SUCCESS; 
  
  std::vector<double> etaminpad   ;
  std::vector<double> etamaxpad   ;
  std::vector<double> phiminpad   ;
  std::vector<double> phimaxpad   ;
  std::vector<int>    thresholdpad;

  std::string sector_dphi_layer;
  std::string sector_name;
  std::string layeronly_name;

  ATH_MSG_DEBUG ( "RPCStandaloneTracksMon::RPCStandaloneTracksMon Histograms being filled" );
  if( m_doRpcESD==true ) { if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online ) {  
          
      // MuonDetectorManager from the conditions store
      SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey};
      const MuonGM::MuonDetectorManager* MuonDetMgr = DetectorManagerHandle.cptr(); 
      if(!MuonDetMgr){
	ATH_MSG_ERROR("nullptr to the read MuonDetectorManager conditions object");
	return StatusCode::FAILURE; 
      } 

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
         SG::ReadHandle<xAOD::MuonContainer> Muons(m_muonsName);
	 ATH_MSG_DEBUG ("Muon container with key: " << m_muonsName.key()<<" found");
//       const xAOD::VertexContainer*	       MSVertices   = evtStore()->retrieve< const xAOD::VertexContainer >           (m_msVertexCollection);
//       const xAOD::TrackParticleContainer*   METracks     = evtStore()->retrieve< const xAOD::TrackParticleContainer >( m_muonExtrapTracksName );
//       const xAOD::TrackParticleContainer*   IDTracks     = evtStore()->retrieve< const xAOD::TrackParticleContainer >     ( m_innerTracksName );
  
      
      
      SG::ReadHandle<Muon::RpcPrepDataContainer> rpc_container(m_key_rpc);
      ATH_MSG_DEBUG ( "RpcPrepDataContainer " << m_key_rpc.key() <<" found");

      SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfo);

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
    
      ATH_MSG_DEBUG ("****** rpc->size() : " << rpc_container->size());  
    
      Muon::RpcPrepDataContainer::const_iterator containerIt;
   
    
      m_type="RPC";


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
      SG::ReadHandle<Muon::RpcPrepDataContainer> rpc_clusterContainer(m_clusterContainerName);
    
      if (m_doClusters)
	{  
	  ATH_MSG_DEBUG ( "Start RPC Cluster Monitoring" );
    
	  // RPC clusters histograms
    
	  m_nClus=0;
    
	  int N_Rpc_Clusters3D = 0;
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
    
	  sc = rpcprd_shift.getHist(m_rpctrack_phivseta,"rpctrack_phivseta");		
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpctrack_phivseta hist to MonGroup" ); 
    
	  sc = rpcprd_shift.getHist(m_rpctrack_phi,"rpctrack_phi");		
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpctrack_phi hist to MonGroup" ); 
    
	  sc = rpcprd_shift.getHist(m_rpctrack_eta,"rpctrack_eta");		
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpctrack_eta hist to MonGroup" );
           
	  sc = rpc_dqmf_global.getHist(m_rpczxSurfaceView,"rpczxSurfaceView");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpczxSurfaceView hist to MonGroup" ); 
  
	  sc = rpcprd_shift.getHist(m_rpcxSurfaceView,"rpcxSurfaceView");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpcxSurfaceView hist to MonGroup" );  
     
	  sc = rpcprd_shift.getHist(m_rpczSurfaceView,"rpczSurfaceView");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpczSurfaceView hist to MonGroup" ); 
  
	  sc = rpcprd_shift.getHist(m_f_rpczxSurfaceView,"f_rpczxSurfaceView");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register f_rpczxSurfaceView hist to MonGroup" );
  
	  sc = rpcprd_shift.getHist(m_rpctrack_bvseta,"rpctrack_bvseta");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpctrack_bvseta hist to MonGroup" );   
  
	  sc = rpcprd_shift.getHist(m_rpctrack_bVTXy0,"rpctrack_bVTXy0");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpctrack_bVTXy0 hist to MonGroup" );  
  
	  sc = rpcprd_shift.getHist(m_rpctrack_bVTXx0,"rpctrack_bVTXx0");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpctrack_bVTXx0 hist to MonGroup" );  
  
	  sc = rpcprd_shift.getHist(m_rpctrack_bVTXz0,"rpctrack_bVTXz0");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpctrack_bVTXz0 hist to MonGroup" );  
  
	  sc = rpcprd_shift.getHist(m_rpctrack_b,"rpctrack_b");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpctrack_b hist to MonGroup" );
    
	  sc = rpcprd_shift.getHist(m_rpcmergepointdistance,"rpcmergepointdistance");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpcmergepointdistance hist to MonGroup" );
   
	  sc = rpcprd_shift.getHist(m_rpcchi2dof,"rpcchi2dof");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpcchi2dof hist to MonGroup" );
  
	  sc = rpcprd_shift.getHist(m_rpcetavsphichi2dof,"rpcetavsphichi2dof");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpcetavsphichi2dof hist to MonGroup" );     
     
	  sc = rpcprd_shift.getHist(m_f_rpcTrackType,"f_rpcTrackType");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register f_rpcTrackType hist to MonGroup" ); 
  
	  sc = rpcprd_shift.getHist(m_rpcTrackType,"rpcTrackType");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpcTrackType hist to MonGroup" );
  
	  sc = rpcprd_shift.getHist(m_f_rpcPhiResidual,"f_rpcPhiResidual");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register f_rpcPhiResidual hist to MonGroup" ); 
  
	  sc = rpcprd_shift.getHist(m_rpcPhiResidual,"rpcPhiResidual");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpcPhiResidual hist to MonGroup" );
  
  
	  sc = rpcprd_shift.getHist(m_f_rpcEtaResidual,"f_rpcEtaResidual");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register f_rpcEtaResidual hist to MonGroup" ); 
  
	  sc = rpcprd_shift.getHist(m_rpcEtaResidual,"rpcEtaResidual");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpcEtaResidual hist to MonGroup" );
    
    
	  sc = rpcprd_shift.getHist(m_f_rpcmergepointdistance,"f_rpcmergepointdistance");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register f_rpcmergepointdistance hist to MonGroup" );
    
	  sc = rpcprd_shift.getHist(m_f_rpcchi2dof,"f_rpcchi2dof");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register f_rpcchi2dof hist to MonGroup" );
    
	  sc = rpc_dqmf_global.getHist(m_rpcNtracks,"rpcNtracks");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpcNtracks hist to MonGroup" );
      
	  sc = rpcprd_shift.getHist(m_f_rpcNtracks,"f_rpcNtracks");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register f_rpcNtracks hist to MonGroup" );
     
	  sc = rpcprd_shift.getHist(m_rpcPointPerTracks,"rpcPointPerTracks");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpcPointPerTracks hist to MonGroup" );
    
	  sc = rpcprd_shift.getHist(m_f_rpcPointPerTracks,"f_rpcPointPerTracks");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register f_rpcPointPerTracks hist to MonGroup" );
    
	  sc = rpc_dqmf_global.getHist(m_LayerTrackProjection,"Track_Projected_on_Layer");		
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register LayerTrackProjection hist to MonGroup" );   
    
	  sc = rpc_dqmf_global.getHist(m_LayerHitOnTrack,"Hit_on_Track_Layer");		
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register LayerHitOnTrack hist to MonGroup" );   
    
          TH1* RPCBA_layerTrackProj = nullptr;
	  sc = rpcprd_dq_BA.getHist(RPCBA_layerTrackProj,"Layer_TrackProj_sideA");	      
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get Layer_TrackProj_sideA  hist" );  
             
          TH1* RPCBC_layerTrackProj = nullptr;
	  sc = rpcprd_dq_BC.getHist(RPCBC_layerTrackProj,"Layer_TrackProj_sideC");	      
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get Layer_TrackProj_sideC  hist" );  
         
          TH1* RPCBA_layerHitOnTrack = nullptr;
	  sc = rpcprd_dq_BA.getHist(RPCBA_layerHitOnTrack,"Layer_HitOnTrack_sideA");	   
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get Layer_HitOnTrack_SideA  hist" );  
     	  
          TH1* RPCBC_layerHitOnTrack = nullptr;
	  sc = rpcprd_dq_BC.getHist(RPCBC_layerHitOnTrack,"Layer_HitOnTrack_sideC");	   
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get Layer_HitOnTrack_sideC  hist" );  

	  sc = rpc_dqmf_global.getHist(m_rpcTimeTrackRes,"TimeTrackResidual") ;
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register TimeTrackResidual hist to MonGroup" );    
    
	  sc = rpcprd_shift.getHist(m_rpcTimeTrackRMS,"TimeTrackRMS") ;
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register TimeTrackRMS hist to MonGroup" );
    
	  sc = rpcprd_shift.getHist(m_rpcCS_HitOnTrack, "CS_HitOnTrack_Distribution");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register CS_HitOnTrack_Distribution hist to MonGroup" );
    
	  sc = rpcprd_shift.getHist(m_rpcCS_NoiseCorr, "CS_NoiseCorr_Distribution");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register CS_NoiseCorr_Distribution hist to MonGroup" );
    
	  sc = rpcprd_shift.getHist(m_rpcCS_NoiseNotCorr, "CS_NoiseNotCorr_Distribution");
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register CS_NoiseNotCorr_Distribution hist to MonGroup" );
    
	  sc = rpcprd_shift.getHist(m_rpcCS_EtavsPhi, "CS_EtavsPhi") ;
	  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get CS_EtavsPhi" );
	 
	  sc = rpc_triggerefficiency.getHist(m_hMEtracks  ,"hMEtracks" ) ;		
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
            
		int irpc_clus_size     =  ((*rpcCollection)->rdoList()).size();
		double irpc_clus_time     =  (*rpcCollection)->time()            ;
		int irpc_clus_station  =  m_idHelperSvc->rpcIdHelper().stationName(prd_id)  ;
		int irpc_clus_eta      =  m_idHelperSvc->rpcIdHelper().stationEta(prd_id)   ;
		int irpc_clus_phi      =  m_idHelperSvc->rpcIdHelper().stationPhi(prd_id)   ;
		int irpc_clus_doublr   =  m_idHelperSvc->rpcIdHelper().doubletR(prd_id)     ;
		int irpc_clus_doublz   =  m_idHelperSvc->rpcIdHelper().doubletZ(prd_id)     ;
		int irpc_clus_doublphi =  m_idHelperSvc->rpcIdHelper().doubletPhi(prd_id)   ;
		int irpc_clus_gasgap   =  m_idHelperSvc->rpcIdHelper().gasGap(prd_id)       ;
		int irpc_clus_measphi  =  m_idHelperSvc->rpcIdHelper().measuresPhi(prd_id)  ;
          
 		// get the average strip and cluster position
 	    		  
 		Amg::Vector3D stripPosC(0,0,0);
		float av_strip = 0 ;
		for(int i=0; i!=irpc_clus_size ; i++){
		  Identifier id = ((*rpcCollection)->rdoList())[i] ;
		  const MuonGM::RpcReadoutElement* descriptor = MuonDetMgr->getRpcReadoutElement(id);
		  stripPosC += descriptor->stripPos(id);
		  int strip = int(m_idHelperSvc->rpcIdHelper().strip(id))            ;
		  av_strip += float(strip)                         ;
		}
		if( irpc_clus_size != 0 ){ 
		  av_strip  =  av_strip / irpc_clus_size ;
		  stripPosC = stripPosC / irpc_clus_size ;
		}
		double irpc_clus_posx   = stripPosC.x()   ;
		double irpc_clus_posy   = stripPosC.y()   ; 
		double irpc_clus_posphi = stripPosC.phi() ;
            
	    		  
		//get information from geomodel to book and fill rpc histos with the right max strip number
		std::vector<int> rpcstripshift   = RpcGM::RpcStripShift(MuonDetMgr,m_idHelperSvc->rpcIdHelper(),prd_id, 0)  ;
	    		  
		int ShiftEtaStripsTot  =  rpcstripshift[8] ;
		int EtaStripSign       =  rpcstripshift[10];
		int SectorLogic 	 =  rpcstripshift[11]  ;
		int Side		 =  rpcstripshift[12]  ;
		int PanelIndex         =  rpcstripshift[13];
		int Settore            =  rpcstripshift[14];
		int strip_dbindex      =  rpcstripshift[16];
				    
		m_SummaryHist_Idx = (Settore-1)*m_SummaryHist_Size/16;
 
		//get name for titles and labels 
		std::vector<std::string>   rpclayersectorsidename = RpcGM::RpcLayerSectorSideName(m_idHelperSvc->rpcIdHelper(),prd_id, 0)  ;
  	       
                std::string layer_name               = rpclayersectorsidename[0] ;
                std::string layertodraw1_name        = rpclayersectorsidename[1] ;
                std::string layertodraw2_name        = rpclayersectorsidename[2] ;
                std::string layervslayer_name        = rpclayersectorsidename[3] ;
                std::string layer0_name	         = rpclayersectorsidename[4] ;
                std::string layer1_name	         = rpclayersectorsidename[5] ;
                std::string layer2_name	         = rpclayersectorsidename[6] ;
                std::string layerPhivsEta_name       = rpclayersectorsidename[7] ;
                std::string layerPhivsEtaSector_name = rpclayersectorsidename[8] ;
                            sector_name              = rpclayersectorsidename[9] ;
                            layeronly_name           = rpclayersectorsidename[10];
                std::string layer_name_panel         = rpclayersectorsidename[11];
                            sector_dphi_layer        = rpclayersectorsidename[12];
	    
		// summary occupancy
		if(PanelIndex<1000)m_SummaryHist[enumSumOccupancy_NotNorm+m_SummaryHist_Idx]-> Fill( PanelIndex );
		//
		if ( m_doCoolDB ) {
                  TH1*  rpcCool_Occupancy_NotNorm = nullptr;
		  sc = rpcCoolDb.getHist(rpcCool_Occupancy_NotNorm, sector_dphi_layer+"_Occupancy_NotNorm" ) ;		
		  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_Occupancy_NotNorm hist" );
		  if(rpcCool_Occupancy_NotNorm)rpcCool_Occupancy_NotNorm->Fill( strip_dbindex );	
		}
            
		// summary CS_entries
		if(PanelIndex<1000)m_SummaryHist[enumSumCS_entries+m_SummaryHist_Idx]-> Fill( PanelIndex );
		//
		if ( m_doCoolDB ) {
                  TH1*  rpcCool_CS_entries = nullptr;
		  sc = rpcCoolDb.getHist(rpcCool_CS_entries, sector_dphi_layer+"_CS_entries" ) ;		
		  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_CS_entries hist" );
		  if(rpcCool_CS_entries)rpcCool_CS_entries->Fill( strip_dbindex );	
		}

		// summary CS
		if(PanelIndex<1000)m_SummaryHist[enumSumCS_NotNorm+m_SummaryHist_Idx]-> Fill( PanelIndex, irpc_clus_size );
		//
		if ( m_doCoolDB ) {
                  TH1*  rpcCool_CS_NotNorm = nullptr;
		  sc = rpcCoolDb.getHist(rpcCool_CS_NotNorm, sector_dphi_layer+"_CS_NotNorm" ) ;		
		  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_CS_NotNorm hist" );
		  if(rpcCool_CS_NotNorm)rpcCool_CS_NotNorm->Fill( strip_dbindex, irpc_clus_size );	
		}
	      
		// summary CS_square
		if(PanelIndex<1000)m_SummaryHist[enumSumCS_square+m_SummaryHist_Idx]-> Fill( PanelIndex, irpc_clus_size*irpc_clus_size );
		//
		if ( m_doCoolDB ) {
                  TH1*  rpcCool_CS_square = nullptr;
		  sc = rpcCoolDb.getHist(rpcCool_CS_square, sector_dphi_layer+"_CS_square" ) ;		
		  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_CS_square hist" );
		  if(rpcCool_CS_square)rpcCool_CS_square->Fill( strip_dbindex, irpc_clus_size*irpc_clus_size  );	
		}
	      	      
		// summary Time
		if(PanelIndex<1000)m_SummaryHist[enumSumTime_NotNorm+m_SummaryHist_Idx]-> Fill( PanelIndex, irpc_clus_time );
	      
                TH1*StripTimeDistributionRPCBA = nullptr;
		sc = rpcprd_dq_BA.getHist(StripTimeDistributionRPCBA, "StripTimeDistribution_" + sector_name +"_SideA");
		if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get StripTimeDistribution RPCBA " );
	         	      
                TH1*StripTimeDistributionRPCBC = nullptr;
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
                  TH1*  rpcCool_Time_NotNorm = nullptr;
		  sc = rpcCoolDb.getHist( rpcCool_Time_NotNorm, sector_dphi_layer+"_Time_NotNorm" ) ;		
		  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_Time_NotNorm hist" );
		  if(rpcCool_Time_NotNorm)rpcCool_Time_NotNorm->Fill( strip_dbindex, irpc_clus_time );	
		}
	      
		// summary Time_square
		if(PanelIndex<1000)m_SummaryHist[enumSumTime_square+m_SummaryHist_Idx]-> Fill( PanelIndex, irpc_clus_time*irpc_clus_time  );
		//
		if ( m_doCoolDB ) {
                  TH1*  rpcCool_Time_square = nullptr;
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

		    int irpc_clus_sizeII     = ((*rpcCollectionII)->rdoList()).size();
		    int irpc_clus_stationII  =  m_idHelperSvc->rpcIdHelper().stationName(prd_idII) ;
		    int irpc_clus_etaII      =  m_idHelperSvc->rpcIdHelper().stationEta(prd_idII)  ;
		    int irpc_clus_phiII      =  m_idHelperSvc->rpcIdHelper().stationPhi(prd_idII)  ;
		    int irpc_clus_doublrII   =  m_idHelperSvc->rpcIdHelper().doubletR(prd_idII)    ;
		    int irpc_clus_doublzII   =  m_idHelperSvc->rpcIdHelper().doubletZ(prd_idII)    ;
		    int irpc_clus_doublphiII =  m_idHelperSvc->rpcIdHelper().doubletPhi(prd_idII)  ;
		    int irpc_clus_gasgapII   =  m_idHelperSvc->rpcIdHelper().gasGap(prd_idII)      ; 
		    int irpc_clus_measphiII  =  m_idHelperSvc->rpcIdHelper().measuresPhi(prd_idII) ;
	   
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
		    float avstripeta = 0        ;
		    float avstripphi = av_strip ; 
		  
		    ShiftEtaStripsTot = RpcGM::RpcStripShift(MuonDetMgr,m_idHelperSvc->rpcIdHelper(),prd_idII, 0)[8]  ;  // angelo 07 oct 2009
		    EtaStripSign      = RpcGM::RpcStripShift(MuonDetMgr,m_idHelperSvc->rpcIdHelper(),prd_idII, 0)[10] ;  // angelo 07 oct 2009

		    // get the average strip and cluster position
	            
		    Amg::Vector3D stripPosCII(0,0,0);
		  

		    for(int i=0; i!=irpc_clus_sizeII ; i++){
		      Identifier id = ((*rpcCollectionII)->rdoList())[i];
		      const MuonGM::RpcReadoutElement* descriptor = MuonDetMgr->getRpcReadoutElement(id);
		      stripPosCII += descriptor->stripPos(id);
		      avstripeta += float(m_idHelperSvc->rpcIdHelper().strip(id)) ;
		    }
		    if( irpc_clus_sizeII != 0 ){ 
		      avstripeta=  avstripeta/ irpc_clus_sizeII ;
		      stripPosCII = stripPosCII / irpc_clus_sizeII ;
		    }
		    avstripeta += float(ShiftEtaStripsTot)       ;
		    avstripeta  = avstripeta*float(EtaStripSign) ;
		    double irpc_clus_poszII   = stripPosCII.z()   ;
		    double irpc_clus_posetaII = stripPosCII.eta() ;
	   
  
		    //evaluate layer m_type
		    int layertype = 0 ;
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
		    if (Muons.cptr()){	
                      // CombinedMuons Tight
	             for (const xAOD::Muon* muons: *Muons)
	             {       
	                if ( muons->muonType()!=xAOD::Muon::Combined  || ( muons->quality()!=xAOD::Muon::Tight && m_requireMuonCombinedTight ))continue;
			++nm;
			ATH_MSG_DEBUG("muons passing quality cuts to test cluster-muon track matching is now "<<nm);
	                const xAOD::TrackParticle* metrack = muons->trackParticle( xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle );          
	                if( !metrack   ) continue;		        
			
			ATH_MSG_DEBUG("xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle found for this muon ");
		        if(  std::sqrt(std::abs(irpc_clus_posetaII-metrack->eta())*std::abs(irpc_clus_posetaII-metrack->eta()) +  std::abs(irpc_clus_posphi-metrack->phi())*std::abs(irpc_clus_posphi-metrack->phi())) <   m_MuonDeltaRMatching) foundmatch3DwithMuon = true ;
		    	      
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
		      int planetype=int(float(layertype)/2);
		  
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

		      N_Rpc_Clusters3D++;
	            

		    }
                                        
		  } // for loop on RpcPrepDataCollection
		} //second loop on clusters end	
      
	
 	      }//end clusters collection
 	    }//end if size
	  }//end clusters container
	     
    
    std::list<muctpi_rdo> muctpi_rdo_roi_list;
    muctpi_rdo_roi_list.clear();
    SG::ReadHandle<MuCTPI_RDO> muctpiRDO(m_muCTPI_RDO_key);
    if (!muctpiRDO.isValid()) {
      // FIXME: q222 was building a configuration in which this monitoring
      //        tool is enabled without the MuCTPI_RDO being available.
      //        Remove this once that is fixed.
      ATH_MSG_WARNING ( "Cannot retrieve the MuCTPI" );
      return StatusCode::SUCCESS;
    }
        
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
      if (!m_isMC) dbc = dbc - BCID%8;
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
     
    // Retrieve the Sector Logic container
    
    if (m_isMC) {
      ATH_MSG_DEBUG ( "Cannot retrieve the RpcSectorLogicContainer ... that's normal in MC: no container is produced in digitization" );
    }
    else {
      SG::ReadHandle<RpcSectorLogicContainer> sectorLogicContainer(m_sectorLogicContainerKey);
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
	      
	       double a =  m_EtaROImin[side][sl][i_roi];
	       double b =  m_EtaROImax[side][sl][i_roi];
	       double c =  m_PhiROImin[side][sl][i_roi];
	       double d =  m_PhiROImax[side][sl][i_roi];
	       etaminpad.push_back(a);
	       etamaxpad.push_back(b);
	       phiminpad.push_back(c);
	       phimaxpad.push_back(d);
	       thresholdpad .push_back(i_ptid);
	      } 
	      
      }}}

      	
	
	  /////////////// Trigger hits efficiency
	  //Trigger hits    
          SG::ReadHandle<Muon::RpcCoinDataContainer> rpc_coin_container(m_rpc_coin_key);
	  
	  if (Muons.cptr()){	
          // CombinedMuons Tight
	  for (const xAOD::Muon* muons: *Muons)
	  {
 
             
	    if ( muons->muonType()!=xAOD::Muon::Combined  || ( muons->quality()!=xAOD::Muon::Tight && m_requireMuonCombinedTight ))continue;
	     
	   
	    const xAOD::TrackParticle* metrack = muons->trackParticle( xAOD::Muon::ExtrapolatedMuonSpectrometerTrackParticle );
            
	    if( !metrack   ) continue;
	    if( std::abs(metrack->eta())>1) continue;
	    sc = rpc_triggerefficiency.getHist( m_hMEtracks  ,"hMEtracks" ) ;		  
	    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get " << " hMEtracks " );
	    if(m_hMEtracks)m_hMEtracks->Fill( metrack->pt() / 1000.);
	    
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
             if(m_idHelperSvc->rpcIdHelper().measuresPhi(prdcoll_id))continue;
	     int cointhr = (*it_collection)->threshold();
             descriptor_Atl = MuonDetMgr->getRpcReadoutElement( prdcoll_id );
             eta_atlas = descriptor_Atl->stripPos(prdcoll_id ).eta();
             phi_atlas = descriptor_Atl->stripPos(prdcoll_id ).phi();
	     
	     
             if(m_idHelperSvc->rpcIdHelper().measuresPhi(prdcoll_id))continue;
	     if( std::sqrt( std::abs(eta_atlas-metrack->eta())*std::abs(eta_atlas-metrack->eta()) +  std::abs(phi_atlas-metrack->phi())*std::abs(phi_atlas-metrack->phi()) ) < m_MuonDeltaRMatching) { 
	      //Second coin phi view
	      for( it_container_phi = rpc_coin_container->begin(); it_container_phi != rpc_coin_container->end(); ++it_container_phi ) {
               for ( Muon::RpcCoinDataCollection::const_iterator it_collection_phi = (*it_container_phi)->begin(); it_collection_phi != (*it_container_phi)->end(); ++it_collection_phi ) { // each collection is a trigger signal
	         if( (*it_collection_phi)->isLowPtCoin()  == 0  && (*it_collection_phi)->isHighPtCoin() == 0 ) continue ; ;
	         prdcoll_id_phi   = (*it_collection_phi)->identify(); 
	         if(m_idHelperSvc->rpcIdHelper().measuresPhi(prdcoll_id_phi)==0)continue;
		 int cointhrphi = (*it_collection)->threshold();
		 if(m_idHelperSvc->rpcIdHelper().stationPhi (prdcoll_id) != m_idHelperSvc->rpcIdHelper().stationPhi (prdcoll_id_phi))  continue ;	   
		 if(m_idHelperSvc->rpcIdHelper().stationName(prdcoll_id) != m_idHelperSvc->rpcIdHelper().stationName(prdcoll_id_phi))  continue ;	   
		 if(m_idHelperSvc->rpcIdHelper().stationEta (prdcoll_id) != m_idHelperSvc->rpcIdHelper().stationEta (prdcoll_id_phi))  continue ;		   
		 if(m_idHelperSvc->rpcIdHelper().doubletR   (prdcoll_id) != m_idHelperSvc->rpcIdHelper().doubletR	(prdcoll_id_phi))  continue ;
		 if(m_idHelperSvc->rpcIdHelper().doubletZ   (prdcoll_id) != m_idHelperSvc->rpcIdHelper().doubletZ	(prdcoll_id_phi))  continue ;
		 if(m_idHelperSvc->rpcIdHelper().doubletPhi (prdcoll_id) != m_idHelperSvc->rpcIdHelper().doubletPhi (prdcoll_id_phi))  continue ;
		 if(m_idHelperSvc->rpcIdHelper().gasGap     (prdcoll_id) != m_idHelperSvc->rpcIdHelper().gasGap	(prdcoll_id_phi))  continue ;  
            
		 if( std::abs((*it_collection)->time() -  (*it_collection_phi)->time()) > 50. ) continue ;  
		 if( (*it_collection)->isLowPtCoin() != (*it_collection_phi)->isLowPtCoin()  || (*it_collection)->isHighPtCoin() != (*it_collection_phi)->isHighPtCoin()) continue ; 
	     
	          
                 descriptor_Atl = MuonDetMgr->getRpcReadoutElement( prdcoll_id_phi );
                 eta_atlas = descriptor_Atl->stripPos(prdcoll_id_phi ).eta();
                 phi_atlas = descriptor_Atl->stripPos(prdcoll_id_phi ).phi(); 
		 if( std::sqrt( std::abs(eta_atlas-metrack->eta())*std::abs(eta_atlas-metrack->eta()) + std::abs(phi_atlas-metrack->phi())*std::abs(phi_atlas-metrack->phi()) ) < m_MuonDeltaRMatching) {		    
		    
		    int minthrview = cointhr ; if(cointhrphi<minthrview)minthrview = cointhrphi; 
		    if( (*it_collection)-> isLowPtCoin() &&  (*it_collection_phi)-> isLowPtCoin()){
		     if(minthrview>=0&&!foundmatchlowpt_thr0){
		      m_hRPCPhiEtaCoinThr[0]->Fill( metrack->pt() / 1000.);
		      foundmatchlowpt_thr0=true;
		     } 
		     if(minthrview>=1&&!foundmatchlowpt_thr1){
		      m_hRPCPhiEtaCoinThr[1]->Fill( metrack->pt() / 1000.);
		      foundmatchlowpt_thr1=true;
		     } 
		     if(minthrview>=2&&!foundmatchlowpt_thr2){
		      m_hRPCPhiEtaCoinThr[2]->Fill( metrack->pt() / 1000.);
		      foundmatchlowpt_thr2=true;
		     }
		    } 
		    if( (*it_collection)->isHighPtCoin() &&  (*it_collection_phi)-> isHighPtCoin()){
		     if(minthrview>=0&&!foundmatchhighpt_thr0){
		      m_hRPCPhiEtaCoinThr[3]->Fill( metrack->pt() / 1000.);
		      foundmatchhighpt_thr0=true;
		     } 
		     if(minthrview>=1&&!foundmatchhighpt_thr1){
		      m_hRPCPhiEtaCoinThr[4]->Fill( metrack->pt() / 1000.);
		      foundmatchhighpt_thr1=true;
		     } 
		     if(minthrview>=2&&!foundmatchhighpt_thr2){
		      m_hRPCPhiEtaCoinThr[5]->Fill( metrack->pt() / 1000.);
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
	   
		    if( ( metrack->eta()-etaminpad.at(i_etaphiPAD))*sign> -m_MuonDeltaRMatching ){
		    if( (-metrack->eta()+etamaxpad.at(i_etaphiPAD))*sign> -m_MuonDeltaRMatching ){
		    if(   metrack->phi()-phiminpad.at(i_etaphiPAD)      > -m_MuonDeltaRMatching ){
		    if(  -metrack->phi()+phiminpad.at(i_etaphiPAD)      > -m_MuonDeltaRMatching ){
		      
		      int thr = thresholdpad.at(i_etaphiPAD) ;
		      if(thr>=0&&!foundmatchpad_thr0){
		       m_hRPCPadThr[0]->Fill( metrack->pt() / 1000.);
		       foundmatchpad_thr0=true;
		      }
		      if(thr>=1&&!foundmatchpad_thr1){
		       m_hRPCPadThr[1]->Fill( metrack->pt() / 1000.);
		       foundmatchpad_thr1=true;
		      }
		      if(thr>=2&&!foundmatchpad_thr2){
		       m_hRPCPadThr[2]->Fill( metrack->pt() / 1000.);
		       foundmatchpad_thr2=true;
		      }
		      if(thr>=3&&!foundmatchpad_thr3){
		       m_hRPCPadThr[3]->Fill( metrack->pt() / 1000.);
		       foundmatchpad_thr3=true;
		      }
		      if(thr>=4&&!foundmatchpad_thr4){
		       m_hRPCPadThr[4]->Fill( metrack->pt() / 1000.);
		       foundmatchpad_thr4=true;
		      }
		      if(thr>=5&&!foundmatchpad_thr5){
		       m_hRPCPadThr[5]->Fill( metrack->pt() / 1000.);
		       foundmatchpad_thr5=true;
		      }
		    }}}}  
	   
	   }//end pad loop 
	   
	   //muctpi
	   for (auto i_muctpi_rdo_roi_list=muctpi_rdo_roi_list.begin();i_muctpi_rdo_roi_list!=muctpi_rdo_roi_list.end();i_muctpi_rdo_roi_list++) { // each collection is a trigger signal
	            
		    double deta =   metrack->eta()-i_muctpi_rdo_roi_list->eta ;
	            double dphi =   metrack->phi()-i_muctpi_rdo_roi_list->phi ;
		    
	            double dr   = std::sqrt(deta*deta+dphi*dphi);
		    int thr     = i_muctpi_rdo_roi_list->thrNumber;
		    if(dr<m_MuonDeltaRMatching ){
		      if(thr>=0&&!foundmatchmuctpi_thr0){
		       m_hRPCMuctpiThr[0]->Fill( metrack->pt() / 1000.);
		       foundmatchmuctpi_thr0=true;
		      }
		      if(thr>=1&&!foundmatchmuctpi_thr1){
		       m_hRPCMuctpiThr[1]->Fill( metrack->pt() / 1000.);
		       foundmatchmuctpi_thr1=true;
		      }
		      if(thr>=2&&!foundmatchmuctpi_thr2){
		       m_hRPCMuctpiThr[2]->Fill( metrack->pt() / 1000.);
		       foundmatchmuctpi_thr2=true;
		      }
		      if(thr>=3&&!foundmatchmuctpi_thr3){
		       m_hRPCMuctpiThr[3]->Fill( metrack->pt() / 1000.);
		       foundmatchmuctpi_thr3=true;
		      }
		      if(thr>=4&&!foundmatchmuctpi_thr4){
		       m_hRPCMuctpiThr[4]->Fill( metrack->pt() / 1000.);
		       foundmatchmuctpi_thr4=true;
		      }
		      if(thr>=5&&!foundmatchmuctpi_thr5){
		       m_hRPCMuctpiThr[5]->Fill( metrack->pt() / 1000.);
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
		  
	  int N_RpcTrack = 0 ;
          std::vector<Amg::Vector3D > RpcTrack_Vect3DCos  ;
          std::vector<Amg::Vector3D >  RpcTrack_Poin3RLin  ;
          std::vector<float>       RpcTrack_chi2dofeta ;
          std::vector<float>       RpcTrack_chi2dofphi ;
          std::vector<float>       RpcTrack_chi2dof    ;
		  
	  if( m_do_rpctracks && N_Rpc_Clusters3D<1000 )  {

            std::vector<int> Rpc_track (N_Rpc_Clusters3D);
			      
	    for (int ilayertype=0; ilayertype!=7; ilayertype++) { //ilayertype==6 do tracks with all 6 layers
	      ATH_MSG_DEBUG("---***---***---***--- Track Search iteration ilayertype="<<ilayertype<<" out of [0-6]");


	      for (int i_3D=0; i_3D!=N_Rpc_Clusters3D; i_3D++) { Rpc_track[ i_3D ] = 0; }
                                        
	      Amg::Vector3D  SegVector(0,0,0)	;
	      Amg::Vector3D   SegPoint(0,0,0)	;
	      Amg::Vector3D  SegVectorII(0,0,0)	;
	      Amg::Vector3D  ImpactVector(0,0,0)  	;
	      Amg::Vector3D  bImpactIP(0,0,0)     	;
      
	      ///multi tracks begin
	      //Pattern recognition: link 3D cluster with at least three different layer m_type two of each Pivot and LowPt 
	      int N_Rpc_Tracks   = 0 ;
 	      for (int i_3D0=0; i_3D0!=N_Rpc_Clusters3D; i_3D0++) {
	        if( !(Rpc_Matched_mu.at(i_3D0)) && m_StandAloneMatchedWithTrack )continue;
 		if(LayerType.at(i_3D0)==ilayertype &&  ilayertype!=6  )continue;
 		if(Rpc_track[ i_3D0 ]>0||PlaneType.at(i_3D0)!=0)continue;//Start always with no-track assigned LowPt plane
 		int PointperTrack = 1  ;
 		Rpc_track[ i_3D0 ] = N_Rpc_Tracks + 1 ;//preliminary assigned
 		int linkedtrack   = 0 ;
                PointperTrack = 0;
		for (int i_3DI=0;i_3DI!=N_Rpc_Clusters3D;i_3DI++) {
	          if( !(Rpc_Matched_mu.at(i_3DI))  && m_StandAloneMatchedWithTrack )continue;
		  if(linkedtrack == 1 ) continue ;
		  if(  std::abs(Rpc_Eta_3D.at(i_3DI)-Rpc_Eta_3D.at(i_3D0)) > EtaStationSpan )continue;
		  if(  std::abs(Rpc_Phi_3D.at(i_3DI)-Rpc_Phi_3D.at(i_3D0)) > DoublePhiSpan  )continue;
		  if(LayerType.at(i_3DI)==ilayertype &&  ilayertype!=6  )continue;
		  if(Rpc_track[ i_3DI ]>0||PlaneType.at(i_3DI)!=1)continue;//Second always no-track assigned Pivot plane
		  Rpc_track[ i_3DI ] = N_Rpc_Tracks + 1 ;
		  PointperTrack = 2 ;
		  SegVector = Rpc_Point.at(i_3DI)-Rpc_Point.at(i_3D0) ;	  
		  SegPoint  =			Rpc_Point.at(i_3D0) ;
		  int lookforthirdII   = 0 ;
		  int thirdlayertypeII = 0 ; 
		  int thirdlayerHPt    = 0 ;
		
		  for (int i_3DII=0;i_3DII!=N_Rpc_Clusters3D;i_3DII++) {
	            if( !(Rpc_Matched_mu.at(i_3DII)) && m_StandAloneMatchedWithTrack )continue;
		    if(  std::abs(Rpc_Eta_3D.at(i_3DII)-Rpc_Eta_3D.at(i_3DI)) > EtaStationSpan )continue;
		    if(  std::abs(Rpc_Phi_3D.at(i_3DII)-Rpc_Phi_3D.at(i_3DI)) > DoublePhiSpan  )continue;
		    if(LayerType.at(i_3DII)==ilayertype &&  ilayertype!=6  )continue;
		    if(Rpc_track[ i_3DII ]>0)continue;//Third no-track assigned LowPt or Pivot or HighPt plane
		  
		    ImpactVector = (SegPoint-Rpc_Point.at(i_3DII)).cross(SegVector);	    
		    if(SegVector.mag()!=0)ImpactVector = ImpactVector/ SegVector.mag();	   
		    
		    if(ilayertype<6){
		     m_f_rpcmergepointdistance -> Fill (ImpactVector.mag()) ;
		    } 
		    else {
		     m_rpcmergepointdistance -> Fill (ImpactVector.mag()) ;
		    }      
		    
		    if(ImpactVector.mag()<MergePointDistance){
		      Rpc_track[ i_3DII ] = N_Rpc_Tracks + 1 ;
		      PointperTrack++ ;
		      lookforthirdII = 1 ;
		      if (LayerType.at(i_3DII)!=LayerType.at(i_3DI) && LayerType.at(i_3DII)!=LayerType.at(i_3D0) ){ 
			thirdlayertypeII = 1 ;
		      }
		      if (LayerType.at(i_3DII)> 3 ){ 
			thirdlayerHPt = 1 ;
		      }
		     	    
		    }
		    else ATH_MSG_VERBOSE("Third (or following) cluster DO NOT match the segment built with 1st-2nd 3D cluster");

		          
		    
		    //merge in costheta not used
		    SegVectorII = Rpc_Point.at(i_3DII)-Rpc_Point.at(i_3D0) ;
		    float cosmerge = SegVector.dot(SegVectorII);
		    if(SegVector.mag()*SegVectorII.mag()!=0)cosmerge=cosmerge/(SegVector.mag()*SegVectorII.mag());
						
			
			
		  }//Third
		  //First and Second do not link with different layer m_type with any Third let free Second

		  if( (lookforthirdII==0||thirdlayertypeII==0)                  ||
		      (ilayertype==4&&thirdlayerHPt==0&&m_HPtPointForHPteff==1) ||
		      (ilayertype==5&&thirdlayerHPt==0&&m_HPtPointForHPteff==1) ||
		      (ilayertype <5&&thirdlayerHPt==0&&m_HPtPointForLPteff==1) || 
		      (ilayertype==6&&thirdlayerHPt==0&&m_HPtPointForTracks==1)    ){  
		    Rpc_track[ i_3DI  ] = 0 ;
		    for (int i_3Dx=0;i_3Dx!=N_Rpc_Clusters3D;i_3Dx++) { if((i_3Dx!=i_3D0)&&(Rpc_track[ i_3Dx ]==N_Rpc_Tracks + 1 )) Rpc_track[ i_3Dx ] = 0; }
		  }
		  else{ 
		    linkedtrack = 1 ; 
		  } 
  	
		}//Second
		//First, Second and Third do not link with different layer m_type let free First
		if(linkedtrack == 0 ){
		  Rpc_track[ i_3D0  ] = 0 ;
		  PointperTrack	  = 0 ;
		}
		else{
 		  if( ilayertype==6  ) { m_rpcPointPerTracks -> Fill ( PointperTrack ) ; }
		  else {  m_f_rpcPointPerTracks -> Fill ( PointperTrack ) ; }
		
		  //rosy decrease to 1000 instead of 10000 since RPC_track is a fixed to 1000 size array
		  if( N_Rpc_Tracks<1000 ) N_Rpc_Tracks ++ ;
 
		  float x0Phi = 0 ;
		  float xyPhi = 0 ;
		  float z0Eta = 0 ;
		  float zyEta = 0 ;
 
		  float xav=0;
                  float x2av=0;
                  float yav=0;
                  float y2av=0;
                  float zav=0;
                  float z2av=0;
                  float xyav=0;
                  float xzav=0;
                  float zyav=0 ;
 
		  int NplaneSmall0  = 0;
                  int NplaneSmall1  = 0;
                  int NplaneSmall2  = 0; 
		  int NplaneLarge0  = 0;
                  int NplaneLarge1  = 0;
                  int NplaneLarge2  = 0;
 
		  float mineta = 100;
                  float maxeta =-100;
		  float minphi = 100;
                  float maxphi =-100;
 
 
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
			
		  float TrackType = NplaneSmall0 + NplaneSmall1*2 + NplaneSmall2*4 + NplaneLarge0*8 + NplaneLarge1*16 + NplaneLarge2*32;
 
		  // linear regression with all points in (xy), (yx), (yz)
		  float deltay = PointperTrack * y2av - yav*yav ;
 
		  if(deltay!=0){
		    x0Phi = (xav*y2av-yav*xyav)/deltay;
		    xyPhi = (xyav*PointperTrack-xav*yav)/deltay;
		    z0Eta = (zav*y2av-yav*zyav)/deltay;
		    zyEta = (zyav*PointperTrack-zav*yav)/deltay;
		  }
		  else{
		    x0Phi = 0 ;
		    xyPhi = 0 ;
		    z0Eta = 0 ;
		    zyEta = 0 ;
		  }
			
		  float chi2dof = 0;
                  float chi2dofeta = 0;
                  float chi2dofphi=0;
		  float tav = 0;
                  float t2av = 0;
                  float tres = 0;
                  float trms = 0;
		                            
 
		  //time spread, residual and chi2 in (y,z), (y,x) view
		  if(deltay!=0){
		    for (int i_3D=0;i_3D!=N_Rpc_Clusters3D;i_3D++) {
		      if(Rpc_track[ i_3D ] != N_Rpc_Tracks) continue;
		      tav  += Rpc_t_3D.at(i_3D)/PointperTrack;
		      t2av += Rpc_t_3D.at(i_3D)*Rpc_t_3D.at(i_3D)/PointperTrack;
		      float projz   =  z0Eta + zyEta * Rpc_y_3D.at(i_3D)  ;
		      float res	  = (projz -Rpc_z_3D.at(i_3D))  	;
		                            
		      if ( m_rpcEtaResidual!=0  &&  ilayertype==6  ) {m_rpcEtaResidual->Fill( res );}
		      else {ATH_MSG_DEBUG ( "rpcEtaResidual not in hist list!" );}
			  
		      float residual2 = res*res				;
		      chi2dofeta  += residual2/(PointperTrack-2)/81.0   ;
 
		      float projx   =  x0Phi + xyPhi * Rpc_y_3D.at(i_3D);
		      res	  = (projx -Rpc_x_3D.at(i_3D))        ;
		      float cosyx = std::cos(( (2*(Rpc_Phi_3D.at(i_3D)-1) + SmallLarge.at(i_3D) -5) )*M_PI/8) ;
		      res = res * cosyx ;
		                            
		      if ( m_rpcPhiResidual!=0  &&  ilayertype==6  ) {m_rpcPhiResidual->Fill( res );}
		      else {ATH_MSG_DEBUG ( "rpcPhiResidual not in hist list!" );}
		  		  
		      residual2 = res*res				;
		      chi2dofphi  += residual2/(PointperTrack-2)/81.0   ;
		    }
 
		  
		    chi2dof = (chi2dofeta + chi2dofphi)/2. ;
		    if ( ilayertype==6  )  {
		      m_rpcchi2dof         -> Fill (chi2dof) ;
		      m_rpcetavsphichi2dof -> Fill (chi2dofphi,chi2dofeta) ;
		      trms = std::sqrt(std::abs(t2av - tav*tav));
		      m_rpcTimeTrackRMS -> Fill (trms) ;
		      for (int i_3D=0;i_3D!=N_Rpc_Clusters3D;i_3D++) {
			if(Rpc_track[ i_3D ] != N_Rpc_Tracks) continue;
			tres =  Rpc_t_3D.at(i_3D) - tav;
			m_rpcTimeTrackRes -> Fill (tres) ;
		      }
		  
		      m_rpcTrackType->Fill(TrackType);
 
		    }
		    else { 
		      m_f_rpcchi2dof -> Fill (chi2dof) ; 	  
		      m_f_rpcTrackType->Fill(TrackType);
		 
		    }
		  }
 		    
 		  if( chi2dof<Chi2dofCut ) {
		  
		    Amg::Vector3D Vect3DCos( xyPhi, 1 , zyEta );
		    Amg::Vector3D  Poin3RLin( x0Phi, 0 , z0Eta );
		  
		  
		    bImpactIP = Poin3RLin.cross(Vect3DCos);	    
		    if(Vect3DCos.mag()!=0)bImpactIP = bImpactIP/ Vect3DCos.mag();
		    float Impact_B_IP = bImpactIP.mag();
		        
		    float xsurface = Poin3RLin.x()+Vect3DCos.x()*81000.;
		    float zsurface = Poin3RLin.z()+Vect3DCos.z()*81000.;
		    float xPy0     = Poin3RLin.x() ;
		    float zPy0     = Poin3RLin.z() ;
                    float xPz0 = 0;
                    float yPz0 = 0;
                    float zPx0 = 0;
                    float yPx0 = 0;
		    if(Vect3DCos.z()!=0) {
		      yPz0     =  - Poin3RLin.z()/Vect3DCos.z()     ;
		      xPz0     =    Poin3RLin.x()+Vect3DCos.x()*yPz0;
		    }
		    if(Vect3DCos.x()!=0) {
		      yPx0     =  - Poin3RLin.x()/Vect3DCos.x()     ;
		      zPx0     =  Poin3RLin.z() +Vect3DCos.z()*yPx0 ;
		    }
		  		  

		    float anglephi = 90-std::atan(std::abs(xyPhi))*180/M_PI   ; //atan between -pi/2 and pi/2 , anglephi from 180 to 0	
		    if(xyPhi<0) anglephi = 180.-anglephi;	   
	    	      
		    float rho    = std::sqrt( xyPhi*xyPhi + 1 + zyEta*zyEta);
		    float costh  = zyEta/rho ;
		    float sinth  = std::sqrt(1-costh*costh);
		    float pseta  = sinth / (1+costh)  ;
		     
		    if(pseta>0) pseta = -std::log(pseta) ;
		      	 	      
		    		  
		    if( ilayertype==6  ){
		      m_rpczxSurfaceView       ->Fill(zsurface,xsurface);
		      m_rpcxSurfaceView        ->Fill(         xsurface);
		      m_rpczSurfaceView        ->Fill(zsurface         );
		      m_rpctrack_phivseta      ->Fill(pseta, anglephi  );
		      m_rpctrack_bvseta        ->Fill(pseta,Impact_B_IP); 
		      m_rpctrack_phi           ->Fill(anglephi         );
		      m_rpctrack_eta           ->Fill(pseta            );
		      m_rpctrack_b             ->Fill(Impact_B_IP      );
		      m_rpctrack_bVTXy0        ->Fill(zPy0,xPy0        );
		      m_rpctrack_bVTXx0        ->Fill(zPx0,yPx0        );
		      m_rpctrack_bVTXz0        ->Fill(xPz0,yPz0        );
		    		  
		    		  
		      N_RpcTrack++;
		      RpcTrack_Vect3DCos .push_back( Vect3DCos);		
		      RpcTrack_Poin3RLin .push_back( Poin3RLin);		
		      RpcTrack_chi2dofeta.push_back(chi2dofeta);		
		      RpcTrack_chi2dofphi.push_back(chi2dofeta);		
		      RpcTrack_chi2dof   .push_back(   chi2dof);
		    }
		    else{
		      m_f_rpczxSurfaceView->Fill(zsurface,xsurface);
		    }

		    float phipitch  = 0 ;
		    float etapitch  = 0 ;
	 		   
		  
		    //start loop on gaps by geomodel
		    if(ilayertype!=6){
		      for(int iname=      2; iname!=       10+1 ; iname++){
			for(int ieta = mineta; ieta != maxeta+1; ieta++ ){ 
			  for(int iphi = minphi; iphi != maxphi+1; iphi++ ){
			    for(int ir   =      1; ir   !=      2+1; ir++	){
			      for(int iz   =      1; iz   !=      3+1; iz++	){
				for(int idp = 1; idp != 2 + 1; idp++ ){
          // need the full check here, since id.is_valid() is not enough to avoid exception in getRpcReadoutElement
          bool isValid=false; 
          Identifier rpcId = m_idHelperSvc->rpcIdHelper().channelID(iname, ieta, iphi, ir, iz, idp, 1, 1, 1, true, &isValid, true); // last 6 arguments are: int doubletPhi, int gasGap, int measuresPhi, int strip, bool check, bool* isValid, bool noPrint
          if (!isValid) {
            ATH_MSG_DEBUG("Could not find valid Identifier for station="<<iname<<", eta="<<ieta<<", phi="<<iphi<<", doubletR="<<ir<<", doubletZ="<<iz<<", doubletPhi="<<idp<<", continuing...");
            continue;
          }
				  const MuonGM::RpcReadoutElement* rpc = MuonDetMgr->getRpcReadoutElement(rpcId);
	      
				  if(!rpc)continue;
	

                                  int small_large = 0;
				  if(iname==2||iname==4){
				    small_large = 1 ;
				  }
 		
				  Identifier idr = rpc->identify();
		
    
 				  for(int ig = 1; ig != 2 + 1; ig++ ){
    
				    int Layer = 4 ;
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
         
				    int phistripN = rpc->NphiStrips()   ;
				    int etastripN = rpc->NetaStrips()   ;
				    phipitch  = rpc->StripPitch(1)  ;
				    etapitch  = rpc->StripPitch(0)  ;
    
				    if(phistripN>80||etastripN>40) continue ;
				    if(phistripN< 1||etastripN< 1) continue ;
				    if(phipitch <10||etapitch <10) continue ;
				    if(phipitch >50||etapitch >50) continue ;
    
				    Identifier ideta1  =  m_idHelperSvc->rpcIdHelper().channelID(idr, iz, idp, ig, 0, 1)	 ;
				    Identifier idetaN  =  m_idHelperSvc->rpcIdHelper().channelID(idr, iz, idp, ig, 0, etastripN) ;
				    Identifier idphi1  =  m_idHelperSvc->rpcIdHelper().channelID(idr, iz, idp, ig, 1, 1)	 ;
				    Identifier idphiN  =  m_idHelperSvc->rpcIdHelper().channelID(idr, iz, idp, ig, 1, phistripN) ;
        
              
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
				    Poseta1L		    = ((rpc->transform(ideta1)).inverse())*Poseta1     ;
 				    PosetaN		    = rpc->stripPos(idetaN)			       ;
 				    PosetaNL		    = ((rpc->transform(ideta1)).inverse())*PosetaN     ;
 				    Inters3DLeta1	    = ((rpc->transform(ideta1)).inverse())*Inters3DG   ;
 				    Posphi1		    = rpc->stripPos(idphi1)			       ;
 				    Posphi1L		    = ((rpc->transform(idphi1)).inverse())*Posphi1     ;
 				    PosphiN		    = rpc->stripPos(idphiN)			       ;
 				    PosphiNL		    = ((rpc->transform(idphi1)).inverse())*PosphiN     ;
 				    Inters3DLphi1	    = ((rpc->transform(idphi1)).inverse())*Inters3DG   ;

		                
				    float hitstripphi = (( Inters3DLphi1.x()-Posphi1L.x() + phipitch  ))/phipitch;
				    if( Posphi1L.x() > PosphiNL.x() )hitstripphi=-hitstripphi;
				    float hitstripeta = (( Inters3DLeta1.x()-Poseta1L.x() + etapitch  ))/etapitch;
				    if( Poseta1L.x() > PosetaNL.x() )hitstripeta=-hitstripeta;

       
				    //look for gap intersection
				    if( !(hitstripphi >  float(nstripfiducial)) ) continue ;
				    if( !(hitstripeta >  float(nstripfiducial)) ) continue ;
				    if( !(hitstripphi <  float(rpc->NphiStrips()-nstripfiducial)) ) continue ;
				    if( !(hitstripeta <  float(rpc->NetaStrips()-nstripfiducial)) ) continue ;
       
        
       
				    int foundonehiteta =    0 ;
				    int foundonehitphi =    0 ;  
				    int foundEtaPhi    =    0 ;
				    int foundEtaOrPhi  =    0 ;
				    int PanelIndexeta  = 1000 ;
				    int PanelIndexphi  = 1000 ;
                                    int StripIndexeta =     0 ;
                                    int StripIndexphi =     0 ;
        
                                    int stripetaatlas = 0;
                                    int stripphisector = 0;

                                    for(int imeasphi = 0; imeasphi != 1 + 1; imeasphi++ ){
                                      int irpcstrip = 0;
				      if(imeasphi==0){
					irpcstrip = int(hitstripeta+1);
					if(irpcstrip<1||rpc->NetaStrips()<irpcstrip)continue;
				      }
				      else { 
					irpcstrip = int(hitstripphi+1);
					if(irpcstrip<1||rpc->NphiStrips()<irpcstrip)continue;
				      }
				      Identifier prdcoll_id  =  m_idHelperSvc->rpcIdHelper().channelID(idr, iz, idp, ig, imeasphi, irpcstrip) ;
       
				      if( prdcoll_id == 0 )continue;
        
				      //get information from geomodel to book and fill rpc histos with the right max strip number
				      std::vector<int>   rpcstripshift = RpcGM::RpcStripShift(MuonDetMgr,m_idHelperSvc->rpcIdHelper(),prdcoll_id, 0)  ;
 
				      int ShiftPhiStrips     =  rpcstripshift[1] ;
				      int ShiftStrips	     =  rpcstripshift[4] ;
				      int ShiftEtaStripsTot  =  rpcstripshift[8] ;
				      int Nbin  	     =  rpcstripshift[9] ;
				      int EtaStripSign       =  rpcstripshift[10];
				      int Side	       =  rpcstripshift[12]  ;
				      int PanelIndex	     =  rpcstripshift[13];
				      int Settore          =  rpcstripshift[14];
				      int strip_dbindex      =  rpcstripshift[16]  ; //ang cool

				      m_SummaryHist_Idx = (Settore-1)*m_SummaryHist_Size/16;
 
				      //get name for titles and labels 
				      std::vector<std::string>   rpclayersectorsidename = RpcGM::RpcLayerSectorSideName(m_idHelperSvc->rpcIdHelper(),prdcoll_id, 0)  ;
 
                                      std::string layer_name		   = rpclayersectorsidename[0] ;
                                      std::string layertodraw1_name	   = rpclayersectorsidename[1] ;
                                      std::string layertodraw2_name	   = rpclayersectorsidename[2] ;
                                      std::string layervslayer_name	   = rpclayersectorsidename[3] ;
                                      std::string layer0_name		   = rpclayersectorsidename[4] ;
                                      std::string layer1_name		   = rpclayersectorsidename[5] ;
                                      std::string layer2_name		   = rpclayersectorsidename[6] ;
                                      std::string layerPhivsEta_name	   = rpclayersectorsidename[7] ;
                                      std::string layerPhivsEtaSector_name = rpclayersectorsidename[8] ;
                                      std::string sector_name		   = rpclayersectorsidename[9] ;
                                      std::string layeronly_name	   = rpclayersectorsidename[10];
                                      std::string layer_name_panel	   = rpclayersectorsidename[11];
                                      std::string sector_dphi_layer        = rpclayersectorsidename[12]  ; //ang cool

				      if( (m_doRadiography==1) && (imeasphi==1) ){
					stripetaatlas  =  int( ( hitstripeta + ShiftEtaStripsTot )*EtaStripSign );	     
					stripphisector =  int(  hitstripphi + ShiftPhiStrips ); 
					sc = rpc_radiography.getHist(m_rpcSectorLayerTrackProj, sector_name+"_"+layeronly_name+"_TrackProj");                       
                
					if (m_rpcSectorLayerTrackProj) {
					  m_rpcSectorLayerTrackProj->Fill(stripetaatlas, stripphisector  );
					}
					else {ATH_MSG_DEBUG ( " rpcSectorLayerTrackProj not in hist list!" );}
				      }
		
				      //get chamber hardware name
				      std::string hardware_name=convertChamberName(iname,ieta,iphi,m_type) ;
          
				      if(m_rpcchamberhist){
 
					ATH_MSG_DEBUG ( "RPC Hardware chamber name in the selected area : " << hardware_name );	    
 
					bool histo_flag=true;
					for (std::vector<std::string>::const_iterator iter=m_layer_name_list.begin(); iter!=m_layer_name_list.end(); iter++){
					  if ( (hardware_name+layer_name)==*iter){histo_flag=false;}
					}
					if (histo_flag){ 
					  m_layer_name_list.push_back(hardware_name+layer_name); 
					  bookRPCTracksLayerHistograms(hardware_name, layer_name, layer0_name, Nbin, 0 , Nbin);
					}
          
					histo_flag=true ;
					for (std::vector<std::string>::const_iterator iter=m_layer_name_list_panel.begin(); iter!=m_layer_name_list_panel.end(); iter++){
					  if ( (hardware_name+layer_name_panel)==*iter){histo_flag=false;}
					}
					if (histo_flag){ 
					  m_layer_name_list_panel.push_back(hardware_name+layer_name_panel)  ; 
					  m_layer_name_bin_list_panel.push_back( PanelIndex ) 	       ; 
					}
                                   
				      }//end if on m_rpcchamberhist || ESD
				      else{
					bool histo_flag=true ;
					for (std::vector<std::string>::const_iterator iter=m_layer_name_list_panel.begin(); iter!=m_layer_name_list_panel.end(); iter++){
					  if ( (hardware_name+layer_name_panel)==*iter){histo_flag=false;}
					}
					if (histo_flag){ 
					  m_layer_name_list_panel.push_back(hardware_name+layer_name_panel)  ; 
					  m_layer_name_bin_list_panel.push_back( PanelIndex ) 	       ; 
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
                                        TH1*  rpcCool_TrackProj = nullptr;
					sc = rpcCoolDb.getHist( rpcCool_TrackProj, sector_dphi_layer+"_TrackProj" ) ;	    
					if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_TrackProj hist" );
					if(rpcCool_TrackProj)rpcCool_TrackProj->Fill( strip_dbindex );   
				      }
                 
				      if (m_LayerTrackProjection) {m_LayerTrackProjection->Fill( ilayertype + 6*imeasphi + 12*small_large);}
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
               
					    int irpc_clus_size	 = ((*rpcCollection)->rdoList()).size();
					    int irpc_clus_station  =  m_idHelperSvc->rpcIdHelper().stationName(prd_id) ;
					    int irpc_clus_eta	 =  m_idHelperSvc->rpcIdHelper().stationEta(prd_id)  ;
					    int irpc_clus_phi	 =  m_idHelperSvc->rpcIdHelper().stationPhi(prd_id)  ;
					    int irpc_clus_doublr   =  m_idHelperSvc->rpcIdHelper().doubletR(prd_id)    ;
					    int irpc_clus_doublz   =  m_idHelperSvc->rpcIdHelper().doubletZ(prd_id)    ;
					    int irpc_clus_doublphi =  m_idHelperSvc->rpcIdHelper().doubletPhi(prd_id)  ;
					    int irpc_clus_gasgap   =  m_idHelperSvc->rpcIdHelper().gasGap(prd_id)      ;
					    int irpc_clus_measphi  =  m_idHelperSvc->rpcIdHelper().measuresPhi(prd_id) ;
             	
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
					    int av_strip = 0 ;
					    for(int i=0; i!=irpc_clus_size ; i++){
					      Identifier id = ((*rpcCollection)->rdoList())[i]	     ;
					      const MuonGM::RpcReadoutElement* descriptor = MuonDetMgr->getRpcReadoutElement(id);
					      stripPosC += descriptor->stripPos(id)	   ;
					      int strip	   = int(m_idHelperSvc->rpcIdHelper().strip(id)) ;
					      av_strip  += float(strip)		   ;
					    }
					    if( irpc_clus_size != 0 ){ 
					      av_strip  = av_strip  / irpc_clus_size ;
					      stripPosC = stripPosC / irpc_clus_size ;
					    }
        
					    Amg::Vector3D Vect3DCosL  (0,0,0)  ;
					    Amg::Vector3D stripPosCL  (0,0,0)  ;
					    Amg::Vector3D Inters3DLCL (0,0,0)  ;
	  
					    Vect3DCosL   = ((rpc->transform(ideta1)).inverse())*Vect3DCosL  ;
					    stripPosCL   = ((rpc->transform(ideta1)).inverse())*stripPosC  ;
					    Inters3DLCL  = ((rpc->transform(ideta1)).inverse())*Inters3DG  ;
					    
					    if(imeasphi==1){
					      Vect3DCosL   = ((rpc->transform(idphi1)).inverse())*Vect3DCosL  ;
					      stripPosCL   = ((rpc->transform(idphi1)).inverse())*stripPosC  ;
					      Inters3DLCL  = ((rpc->transform(idphi1)).inverse())*Inters3DG  ;
					    }
					    float residuals    = stripPosCL.x()-Inters3DLCL.x();
             	
					    if (rpcresidualslayer) {rpcresidualslayer->Fill( residuals );}
					    else {  ATH_MSG_DEBUG ( "rpcstripresidualslayer not in hist list!" );}
             			 
					    if (m_f_rpcEtaResidual!=0 && imeasphi==0) {m_f_rpcEtaResidual->Fill( residuals );}
					    else {  ATH_MSG_DEBUG ( "f_rpcEtaResidual not in hist list!" );}
               
					    if (m_f_rpcPhiResidual!=0 && imeasphi==1) {m_f_rpcPhiResidual->Fill( residuals );}
					    else {  ATH_MSG_DEBUG ( "f_rpcPhiResidual not in hist list!" );} 
	
					    char ResidualVsCS_title_char[100] = "ResidualVsCS";
					    sprintf(ResidualVsCS_title_char , "ResidualVsCS%d", irpc_clus_size) ;
					    if(irpc_clus_size<maxCSres+1) { 
					      sc = rpcprd_shift.getHist(m_ResidualVsCS , ResidualVsCS_title_char );
					      m_ResidualVsCS -> Fill (residuals) ;
					    }
					    // summary residuals CS1
					    if ( irpc_clus_size==1 ) {
					      if(PanelIndex<1000) m_SummaryHist[enumSumRes_CS1_entries+m_SummaryHist_Idx]-> Fill( PanelIndex );
					      //
					      if ( m_doCoolDB ) {
                                                TH1*  rpcCool_Res_CS1_entries = nullptr;
						sc = rpcCoolDb.getHist( rpcCool_Res_CS1_entries, sector_dphi_layer+"_Res_CS1_entries" ) ;	    
						if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_Res_CS1_entries hist" );
						if(rpcCool_Res_CS1_entries)rpcCool_Res_CS1_entries->Fill( strip_dbindex ); 
					      }
	
					      if(PanelIndex<1000) m_SummaryHist[enumSumRes_CS1_NotNorm+m_SummaryHist_Idx]-> Fill( PanelIndex, residuals );
					      //
					      if ( m_doCoolDB ) {
                                                TH1*  rpcCool_Res_CS1_NotNorm = nullptr;
						sc = rpcCoolDb.getHist( rpcCool_Res_CS1_NotNorm, sector_dphi_layer+"_Res_CS1_NotNorm" ) ;	    
						if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_Res_CS1_NotNorm hist" );
						if(rpcCool_Res_CS1_NotNorm)rpcCool_Res_CS1_NotNorm->Fill( strip_dbindex, residuals );      
					      }  
    
					      if(PanelIndex<1000) m_SummaryHist[enumSumRes_CS1_square+m_SummaryHist_Idx]-> Fill( PanelIndex, residuals*residuals );
					      //
					      if ( m_doCoolDB ) {
                                                TH1*  rpcCool_Res_CS1_square = nullptr;
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
                                                TH1*  rpcCool_Res_CS2_entries = nullptr;		   ;
						sc = rpcCoolDb.getHist( rpcCool_Res_CS2_entries, sector_dphi_layer+"_Res_CS2_entries" ) ;	    
						if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_Res_CS2_entries hist" );
						if(rpcCool_Res_CS2_entries)rpcCool_Res_CS2_entries->Fill( strip_dbindex ); 
					      }  
 
					      if(PanelIndex<1000) m_SummaryHist[enumSumRes_CS2_NotNorm+m_SummaryHist_Idx]-> Fill( PanelIndex, residuals );
					      //
					      if ( m_doCoolDB ) {
                                                TH1*  rpcCool_Res_CS2_NotNorm = nullptr;
						sc = rpcCoolDb.getHist( rpcCool_Res_CS2_NotNorm, sector_dphi_layer+"_Res_CS2_NotNorm" ) ;	    
						if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_Res_CS2_NotNorm hist" );
						if(rpcCool_Res_CS2_NotNorm)rpcCool_Res_CS2_NotNorm->Fill( strip_dbindex, residuals );      
					      }
  
					      if(PanelIndex<1000) m_SummaryHist[enumSumRes_CS2_square+m_SummaryHist_Idx]-> Fill( PanelIndex, residuals*residuals );
					      //
					      if ( m_doCoolDB ) {
                                                TH1*  rpcCool_Res_CS2_square = nullptr;
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
                                                TH1* rpcCool_Res_CS2_entries = nullptr;
						sc = rpcCoolDb.getHist( rpcCool_Res_CS2_entries, sector_dphi_layer+"_Res_CS2_entries" ) ;	    
						if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_Res_CS2_entries hist" );
						if(rpcCool_Res_CS2_entries)rpcCool_Res_CS2_entries->Fill( strip_dbindex ); 
					      }
 
					      if(PanelIndex<1000) m_SummaryHist[enumSumRes_CSmore2_NotNorm+m_SummaryHist_Idx]-> Fill( PanelIndex, residuals );
					      //
					      if ( m_doCoolDB ) {
                                                TH1* rpcCool_Res_CS2_NotNorm = nullptr;
						sc = rpcCoolDb.getHist( rpcCool_Res_CS2_NotNorm, sector_dphi_layer+"_Res_CS2_NotNorm" ) ;
						if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_Res_CS2_NotNorm hist" );
						if(rpcCool_Res_CS2_NotNorm)rpcCool_Res_CS2_NotNorm->Fill( strip_dbindex, residuals );      
					      }
            
					      if(PanelIndex<1000) m_SummaryHist[enumSumRes_CSmore2_square+m_SummaryHist_Idx]-> Fill( PanelIndex, residuals*residuals );
					      //
					      if ( m_doCoolDB ) {
                                                TH1* rpcCool_Res_CS2_square = nullptr;
						sc = rpcCoolDb.getHist( rpcCool_Res_CS2_square, sector_dphi_layer+"_Res_CS2_square" ) ;	    
						if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_Res_CS2_square hist" );
						if(rpcCool_Res_CS2_square)rpcCool_Res_CS2_square->Fill( strip_dbindex, residuals*residuals  );     
					      }
					    }
					    //efficiency only if average strip +/-1 projected track
    

					    //eta view
					    if(irpc_clus_measphi==0 && foundonehiteta==0){
					      if(std::abs(residuals)<=m_resThEff){
						if (rpchitontracklayer) {rpchitontracklayer->Fill( float( irpcstrip + ShiftStrips)  -0.5 );}
						else {  ATH_MSG_DEBUG ( "rpcstriphitontracklayer not in hist list!" );}
						if (m_LayerHitOnTrack) {m_LayerHitOnTrack->Fill( ilayertype + 6*imeasphi + 12*small_large);}
						if ( Side == 1 ) {
						  if ( RPCBA_layerHitOnTrack )  RPCBA_layerHitOnTrack->Fill( ilayertype + 6*imeasphi + 12*small_large);
						}		
						else{
						  if ( RPCBC_layerHitOnTrack )  RPCBC_layerHitOnTrack->Fill( ilayertype + 6*imeasphi + 12*small_large);
						}
			  
			  
						// Summary Hit On Track
                                                TH1* SummaryHitOnTrack = nullptr;
						sc = rpcprd_expert_sum.getHist( SummaryHitOnTrack, "SummaryHitOnTrack_" + sector_name ) ;
						if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get SummaryHitOnTrack hist in EtaView" );		    
						if(PanelIndex<1000) SummaryHitOnTrack -> Fill( float(PanelIndex) ) ;
						//
						if ( m_doCoolDB ) {
                                                  TH1*  rpcCool_HitOnTrack = nullptr;
						  sc = rpcCoolDb.getHist( rpcCool_HitOnTrack, sector_dphi_layer+"_HitOnTrack" ) ;	    
						  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_HitOnTrack hist" );
						  if(rpcCool_HitOnTrack)rpcCool_HitOnTrack->Fill( strip_dbindex ); 
						}
			   
						//incidence angle of tracks on surface (z,rho)
			           
			                        float norm3DGaprho = std::sqrt(norm3DGap.x() *  norm3DGap.x() + norm3DGap.y() *  norm3DGap.y()) ;
			                        float Vect3DCosrho = std::sqrt(Vect3DCos.x() *  Vect3DCos.x() + Vect3DCos.y() *  Vect3DCos.y()) ;
						
						double incAngle = norm3DGaprho *  Vect3DCosrho + norm3DGap.z() *  Vect3DCos.z() ;
						float norm1 = std::sqrt(norm3DGaprho*norm3DGaprho+norm3DGap.z()*norm3DGap.z()) ;
						float norm2 = std::sqrt(Vect3DCosrho*Vect3DCosrho+Vect3DCos.z()*Vect3DCos.z()) ;
			  			   
						if(norm1*norm2>0)incAngle = incAngle / norm1 / norm2 ;
			  			  
						sc = rpcprd_shift.getHist(m_rpcCS_angleTrasv, "CSvsTrasversalAngleTrack") ;
						if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get CSvsTrasversalAngleTrack  hist in EtaView" );
						if ( m_rpcCS_angleTrasv ) m_rpcCS_angleTrasv->Fill( incAngle, irpc_clus_size );

						//incidence angle of tracks on surface (x,y)
			   
						incAngle = norm3DGap.x() *  Vect3DCos.x() + norm3DGap.y() *  Vect3DCos.y() ;
						norm1 = std::sqrt(norm3DGap.x()*norm3DGap.x()+norm3DGap.y()*norm3DGap.y()) ;
						norm2 = std::sqrt(Vect3DCos.x()*Vect3DCos.x()+Vect3DCos.y()*Vect3DCos.y()) ;
			   
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
					      if(std::abs(residuals)<=m_resThEff){
						if (rpchitontracklayer) {rpchitontracklayer->Fill( float( irpcstrip + ShiftStrips)  -0.5 );}
						else {  ATH_MSG_DEBUG ( "rpcstriphitontracklayer not in hist list!" );}
						if (m_LayerHitOnTrack) {m_LayerHitOnTrack->Fill( ilayertype + 6*imeasphi + 12*small_large);}
						if ( Side == 1 ) {
						  if ( RPCBA_layerHitOnTrack )  RPCBA_layerHitOnTrack->Fill( ilayertype + 6*imeasphi + 12*small_large);
						}		
						else{
						  if ( RPCBC_layerHitOnTrack )  RPCBC_layerHitOnTrack->Fill( ilayertype + 6*imeasphi + 12*small_large);
						}
                                                TH1* SummaryHitOnTrack = nullptr;
						sc = rpcprd_expert_sum.getHist( SummaryHitOnTrack, "SummaryHitOnTrack_" + sector_name ) ;
						if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get SummaryHitOnTrack hist in PhiView" );		    
						if(PanelIndex<1000)SummaryHitOnTrack -> Fill( float(PanelIndex) ) ;
						//
						if ( m_doCoolDB ) {
                                                  TH1*  rpcCool_HitOnTrack = nullptr;
						  sc = rpcCoolDb.getHist( rpcCool_HitOnTrack, sector_dphi_layer+"_HitOnTrack" ) ;	    
						  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_HitOnTrack hist" );
						  if(rpcCool_HitOnTrack)rpcCool_HitOnTrack->Fill( strip_dbindex ); 
						}  
                          
						//incidence angle of tracks on surface (x,y)
			   
						double incAngle = norm3DGap.x() *  Vect3DCos.x() + norm3DGap.y() *  Vect3DCos.y() ;
						float norm1 = std::sqrt(norm3DGap.x()*norm3DGap.x()+norm3DGap.y()*norm3DGap.y()) ;
						float norm2 = std::sqrt(Vect3DCos.x()*Vect3DCos.x()+Vect3DCos.y()*Vect3DCos.y()) ;
			   
						if(norm1*norm2>0)incAngle = incAngle / norm1 / norm2 ;
			                              		      
						sc = rpcprd_shift.getHist(m_rpcCS_angleTrasv, "CSvsTrasversalAngleTrack") ;
						if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get CSvsTrasversalAngleTrack  hist in EtaView" );
						if ( m_rpcCS_angleTrasv ) m_rpcCS_angleTrasv->Fill( incAngle, irpc_clus_size );
			   
						//incidence angle of tracks on surface (z,rho)
			                        
			           
			                        float norm3DGaprho = std::sqrt(norm3DGap.x() *  norm3DGap.x() + norm3DGap.y() *  norm3DGap.y()) ;
			                        float Vect3DCosrho = std::sqrt(Vect3DCos.x() *  Vect3DCos.x() + Vect3DCos.y() *  Vect3DCos.y()) ;
						incAngle = norm3DGaprho *  Vect3DCosrho + norm3DGap.z() *  Vect3DCos.z() ;
						norm1 = std::sqrt(norm3DGaprho*norm3DGaprho+norm3DGap.z()*norm3DGap.z()) ;
						norm2 = std::sqrt(Vect3DCosrho*Vect3DCosrho+Vect3DCos.z()*Vect3DCos.z()) ;
			 			   
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
					      sc = rpc_radiography.getHist(m_rpcSectorLayerResponse,  sector_name+"_"+layeronly_name+"_Response" ); 
					      if (m_rpcSectorLayerResponse) {
					       m_rpcSectorLayerResponse->Fill(stripetaatlas, stripphisector  );
					       float a1 =  m_rpcSectorLayerResponse->GetBinContent(stripetaatlas, stripphisector  );
					       float a2 =  m_rpcSectorLayerTrackProj->GetBinContent(stripetaatlas, stripphisector  );
					       if(a2<a1)ATH_MSG_WARNING(" WARNING DIAMOND AND");
					      }
					      else {  ATH_MSG_DEBUG ( "rpcSectorLayerResponse not in hist list!" );}
					      foundEtaPhi=1;
					     }
					     if ( (foundonehitphi==1 || foundonehiteta==1 ) && imeasphi==1  && foundEtaOrPhi==0 ) {
					      sc = rpc_radiography.getHist(m_rpcSectorLayerResponseOR, sector_name+"_"+layeronly_name+"_ResponseEtaOrPhi" ); 
					      if ( m_rpcSectorLayerResponseOR ) { 
					       m_rpcSectorLayerResponseOR->Fill(stripetaatlas, stripphisector  );
					       float a1 =  m_rpcSectorLayerResponseOR->GetBinContent(stripetaatlas, stripphisector  );
					       float a2 =  m_rpcSectorLayerTrackProj->GetBinContent(stripetaatlas, stripphisector  );
					       if(a2<a1)ATH_MSG_WARNING(" WARNING DIAMOND OR");
					      }
					      else {  ATH_MSG_DEBUG ( "rpcSectorLayerResponseEtaOrPhi not in hist list!" );}
					      foundEtaOrPhi=1;
					     }
					   }  
   
 					  }}}//end loop on cluster
//     
//   
 				    }//end loop on views

				    if(foundonehiteta==1&&foundonehitphi==1){	
				    			
				      if(PanelIndexeta<1000)m_SummaryHist[enumSumHitOnTrack_withCrossStrip+m_SummaryHist_Idx]-> Fill( float(PanelIndexeta) ) ;
				      if(PanelIndexphi<1000)m_SummaryHist[enumSumHitOnTrack_withCrossStrip+m_SummaryHist_Idx]-> Fill( float(PanelIndexphi) ) ;
				      //
				      if ( m_doCoolDB ) {
                                        TH1*  rpcCool_HitOnTrack_withCrossStrip = nullptr;
					sc = rpcCoolDb.getHist( rpcCool_HitOnTrack_withCrossStrip, sector_dphi_layer+"_HitOnTrack_withCrossStrip" ) ;		
					if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_HitOnTrack_withCrossStrip hist" );
					if(rpcCool_HitOnTrack_withCrossStrip)rpcCool_HitOnTrack_withCrossStrip->Fill( StripIndexeta );
					if(rpcCool_HitOnTrack_withCrossStrip)rpcCool_HitOnTrack_withCrossStrip->Fill( StripIndexphi );	
				      }
				    }  
				  
				  
				    if ( m_doRadiography ) {
				      if(foundonehitphi==1 && foundonehiteta==1 ) {
					sc = rpc_radiography.getHist(m_rpcSectorLayerResponse,  sector_name+"_"+layeronly_name+"_Response" ); 
					if (m_rpcSectorLayerResponse) {
					  m_rpcSectorLayerResponse->Fill(stripetaatlas, stripphisector  );
					}
					else {  ATH_MSG_DEBUG ( "rpcSectorLayerResponse not in hist list!" );}
					      
				      }
				      if ( foundonehitphi==1 || foundonehiteta==1 ) {
					sc = rpc_radiography.getHist(m_rpcSectorLayerResponseOR, sector_name+"_"+layeronly_name+"_ResponseEtaOrPhi" ); 
					if ( m_rpcSectorLayerResponseOR ) { 
					  m_rpcSectorLayerResponseOR->Fill(stripetaatlas, stripphisector  );
					}
					else {  ATH_MSG_DEBUG ( "rpcSectorLayerResponseEtaOrPhi not in hist list!" );}
					      
				      }
				    } // end  Radiography 
				  
				  
				  
    
				  } 
} 
				  } } } } } }//end look for gap intersection

         }}}//Chi2dofCut//First
 
 	      if( ilayertype==6  ) {  m_rpcNtracks -> Fill (N_Rpc_Tracks) ; }
 	      else { m_f_rpcNtracks -> Fill (N_Rpc_Tracks) ;  }
     
 	      ///multi tracks end
   
 	    }//for layertype
 	  }//if cluster more than
  
	  // Calculate Noise 
          int isNoiseCorr=0;
	  it = rpc_clusterContainer->begin();
	  for ( ; it != rpc_clusterContainer->end() ; ++it ) {

	    const Muon::RpcPrepDataCollection* clusterCollection = *it;
    
	    if (clusterCollection->size()>0) {
      
	      ATH_MSG_DEBUG ( "New Cluster collection" );

	      for (Muon::RpcPrepDataCollection::const_iterator rpcCollection = clusterCollection->begin(); 
		   rpcCollection != clusterCollection->end(); ++rpcCollection) {
	
		Identifier prd_id = (*rpcCollection)->identify();
	
	        ATH_MSG_DEBUG ( "Adding a new cluster " );
	     
		int irpc_clus_size     =  ((*rpcCollection)->rdoList()).size();
		int irpc_clus_station  =  m_idHelperSvc->rpcIdHelper().stationName(prd_id)  ;
		int irpc_clus_eta      =  m_idHelperSvc->rpcIdHelper().stationEta(prd_id)   ;
		int irpc_clus_phi      =  m_idHelperSvc->rpcIdHelper().stationPhi(prd_id)   ;
		int irpc_clus_doublr   =  m_idHelperSvc->rpcIdHelper().doubletR(prd_id)     ;
		int irpc_clus_doublz   =  m_idHelperSvc->rpcIdHelper().doubletZ(prd_id)     ;
		int irpc_clus_doublphi =  m_idHelperSvc->rpcIdHelper().doubletPhi(prd_id)   ;
		int irpc_clus_gasgap   =  m_idHelperSvc->rpcIdHelper().gasGap(prd_id)       ;
		int irpc_clus_measphi  =  m_idHelperSvc->rpcIdHelper().measuresPhi(prd_id)  ;

		// get the cluster position
    const MuonGM::RpcReadoutElement* rpc = MuonDetMgr->getRpcReadoutElement(prd_id);
    if(!rpc) continue;
    const Amg::Vector3D stripPosC = rpc->stripPos(prd_id);

		//get information from geomodel to book and fill rpc histos with the right max strip number
		std::vector<int>   rpcstripshift = RpcGM::RpcStripShift(MuonDetMgr,m_idHelperSvc->rpcIdHelper(),prd_id, 0)  ;

		int PanelIndex  	 =  rpcstripshift[13]  ;
		int Settore            =  rpcstripshift[14];
		int strip_dbindex      =  rpcstripshift[16]  ;
  
		m_SummaryHist_Idx = (Settore-1)*m_SummaryHist_Size/16;

		//get name for titles and labels
		std::vector<std::string>   rpclayersectorsidename = RpcGM::RpcLayerSectorSideName(m_idHelperSvc->rpcIdHelper(),prd_id, 0)  ;
  	       
                std::string layer_name	       = rpclayersectorsidename[ 0]  ;
                std::string layertodraw1_name	       = rpclayersectorsidename[ 1]  ;
                std::string layertodraw2_name	       = rpclayersectorsidename[ 2]  ;
                std::string layervslayer_name	       = rpclayersectorsidename[ 3]  ;
                std::string layer0_name 	       = rpclayersectorsidename[ 4]  ;
                std::string layer1_name 	       = rpclayersectorsidename[ 5]  ;
                std::string layer2_name 	       = rpclayersectorsidename[ 6]  ;
                std::string layerPhivsEta_name       = rpclayersectorsidename[ 7]  ;
                std::string layerPhivsEtaSector_name = rpclayersectorsidename[ 8]  ;
                std::string sector_name 	       = rpclayersectorsidename[ 9]  ;
		layeronly_name	       = rpclayersectorsidename[10]  ;
                std::string layer_name_panel         = rpclayersectorsidename[11]  ;	
                std::string ssector_dphi_layer        = rpclayersectorsidename[12]  ;

		irpc_clus_measphi  =  m_idHelperSvc->rpcIdHelper().measuresPhi(prd_id);
			  
		Amg::Vector3D  Vector3DGapG = rpc->gasGapPos(irpc_clus_doublz, irpc_clus_doublphi, irpc_clus_gasgap  );
		Amg::Vector3D norm3DGap   = rpc->normal   ( /*                 irpc_clus_doublphi, irpc_clus_gasgap  */);
		float invgasgaparea  = rpc->gasGapSsize() * rpc->gasGapZsize() / 100;
		
		if(invgasgaparea!=0)invgasgaparea=1./invgasgaparea;

                float invstriparea = 0;
		if(irpc_clus_measphi==0){
		  invstriparea=invgasgaparea*rpc->NetaStrips() ;
		}   
		else {
		  invstriparea=invgasgaparea*rpc->NphiStrips() ;
		}	
		
	        
		int isNoise=1;
		
		Amg::Vector3D Inters3DG  (0,0,0);
		Amg::Vector3D Inters3DL  (0,0,0);
                Amg::Vector3D stripPosCL (0,0,0);
	
		for (int i_RpcTrack=0; i_RpcTrack!=N_RpcTrack; i_RpcTrack++) {
 
		  Inters3DG   =  RpcTrack_Poin3RLin .at(i_RpcTrack) + 
		    RpcTrack_Vect3DCos .at(i_RpcTrack) *
		    (norm3DGap.dot(Vector3DGapG-RpcTrack_Poin3RLin .at(i_RpcTrack)))/
		    (norm3DGap.dot(RpcTrack_Vect3DCos .at(i_RpcTrack) ));
	       
		  Inters3DL	 = ((rpc->transform(prd_id)).inverse())* Inters3DG   ;  				    
		  stripPosCL	 = ((rpc->transform(prd_id)).inverse())* stripPosC   ;				    
                  
		  float distance = std::abs(Inters3DL.x() -  stripPosCL.x());
	        
		
		  if(distance<MergePointDistance) {
		    isNoise=0;
		  }   
		} // end loop on tracks
		
		if( isNoise==1 ) {
	        
		  if(PanelIndex<1000)m_SummaryHist[enumSumNoiseTot_NotNorm+m_SummaryHist_Idx]-> Fill( float(PanelIndex) , invgasgaparea) ;
		  //
		  if ( m_doCoolDB ) {
                    TH1*  rpcCool_NoiseTot_NotNorm = nullptr;
		    sc = rpcCoolDb.getHist( rpcCool_NoiseTot_NotNorm, sector_dphi_layer+"_NoiseTot_NotNorm" ) ;		    
		    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_NoiseTot_NotNorm hist" );
		    if(rpcCool_NoiseTot_NotNorm)rpcCool_NoiseTot_NotNorm->Fill( strip_dbindex, invstriparea);	  
		  }
		  // correlated noise
		  for (Muon::RpcPrepDataCollection::const_iterator rpcCollectionII = clusterCollection->begin(); 
		       rpcCollectionII != clusterCollection->end(); ++rpcCollectionII) {
		    Identifier prd_idII = (*rpcCollectionII)->identify();
	     
	            ATH_MSG_DEBUG ( "Adding a new cluster " );
	     
		    int irpc_clus_stationII  =  m_idHelperSvc->rpcIdHelper().stationName(prd_idII) ;
		    int irpc_clus_etaII      =  m_idHelperSvc->rpcIdHelper().stationEta(prd_idII)  ;
		    int irpc_clus_phiII      =  m_idHelperSvc->rpcIdHelper().stationPhi(prd_idII)  ;
		    int irpc_clus_doublrII   =  m_idHelperSvc->rpcIdHelper().doubletR(prd_idII)    ;
		    int irpc_clus_doublzII   =  m_idHelperSvc->rpcIdHelper().doubletZ(prd_idII)    ;
		    int irpc_clus_doublphiII =  m_idHelperSvc->rpcIdHelper().doubletPhi(prd_idII)  ;
		    int irpc_clus_gasgapII   =  m_idHelperSvc->rpcIdHelper().gasGap(prd_idII)      ; 
		    int irpc_clus_measphiII  =  m_idHelperSvc->rpcIdHelper().measuresPhi(prd_idII) ;
	   
	   
		    if( irpc_clus_stationII  != irpc_clus_station     ) continue ;		 
		    if( irpc_clus_etaII	   != irpc_clus_eta	    ) continue ;		 
		    if( irpc_clus_phiII	   != irpc_clus_phi	    ) continue ;		 
		    if( irpc_clus_doublrII   != irpc_clus_doublr      ) continue ;		 
		    if( irpc_clus_doublzII   != irpc_clus_doublz      ) continue ;		 
		    if( irpc_clus_doublphiII != irpc_clus_doublphi    ) continue ;		 
		    if( irpc_clus_gasgapII   == irpc_clus_gasgap      ) continue ;		 
		    if( irpc_clus_measphiII  != irpc_clus_measphi     ) continue ;
	   
		    // get the cluster position
		    const MuonGM::RpcReadoutElement* descriptorII = MuonDetMgr->getRpcReadoutElement(prd_idII);
	
		    const Amg::Vector3D stripPosCII = descriptorII->stripPos(prd_idII);
	          
	   
		    float distance =   (stripPosCII - stripPosC ).mag() ;
	   
		    if( distance < MergePointDistance ) {
		      isNoiseCorr = 1;
		    }
	
		  }
		  if ( isNoiseCorr==1) {
		    if(PanelIndex<1000)m_SummaryHist[enumSumNoiseCorr_NotNorm+m_SummaryHist_Idx]-> Fill( float(PanelIndex), invgasgaparea) ;
		    //
		    if ( m_doCoolDB ) {
                      TH1*  rpcCool_NoiseCorr_NotNorm = nullptr;
		      sc = rpcCoolDb.getHist( rpcCool_NoiseCorr_NotNorm, sector_dphi_layer+"_NoiseCorr_NotNorm" ) ;	      
		      if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get " << sector_dphi_layer << "_NoiseCorr_NotNorm hist" );
		      if(rpcCool_NoiseCorr_NotNorm)rpcCool_NoiseCorr_NotNorm->Fill( strip_dbindex, invstriparea );     
		    }
		    m_rpcCS_NoiseCorr->Fill( irpc_clus_size ) ;
		  }
		  else {
		    m_rpcCS_NoiseNotCorr->Fill( irpc_clus_size ) ;
		  }
		} // end if isNoise==1   
		else {
		  m_rpcCS_HitOnTrack->Fill( irpc_clus_size ) ;
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
 		
		  		 
	      int isNoise = 1 ;
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
		  float distance = ( Rpc_Point.at(i_3D0) - Rpc_Point.at(i_3DI) ).mag() ;
		  if ( distance < MergePointDistance ) {
		    isNoiseCorr = 1;
		  }
		}
	      
		int isNoisePhiStrip =  Rpc_avPhi_3D.at(i_3D0) ;
		if(Rpc_DBLphi_3D.at(i_3D0) == 2){
		  if(LayerType.at(i_3D0)<4&&Rpc_Station_3D.at(i_3D0)==2)isNoisePhiStrip += 64 ;
		  if(LayerType.at(i_3D0)<4&&(Rpc_Station_3D.at(i_3D0)==3||Rpc_Station_3D.at(i_3D0)==8))isNoisePhiStrip += 56 ;
		  if(LayerType.at(i_3D0)>3&&Rpc_Station_3D.at(i_3D0)==4)isNoisePhiStrip += 80 ;
		  if(LayerType.at(i_3D0)>3&&(Rpc_Station_3D.at(i_3D0)==5||Rpc_Station_3D.at(i_3D0)>8))isNoisePhiStrip += 64 ;
		}
		    
		if ( isNoiseCorr==1 ) {
		  sc = rpc_radiography.getHist(m_rpcSectorLayerNoiseCorr, sector_name+"_"+layeronly_name+"_NoiseCorr" );
		  if ( m_rpcSectorLayerNoiseCorr ) { m_rpcSectorLayerNoiseCorr->Fill( Rpc_avEta_3D.at(i_3D0),  isNoisePhiStrip ); }
		  else {  ATH_MSG_DEBUG ( "rpcSectorLayerNoiseCorr not in hist list!" );}		    
		}
		else {
		  sc = rpc_radiography.getHist(m_rpcSectorLayerNoiseNotCorr, sector_name+"_"+layeronly_name+"_NoiseNotCorr" );
		  if ( m_rpcSectorLayerNoiseNotCorr ) { m_rpcSectorLayerNoiseNotCorr->Fill( Rpc_avEta_3D.at(i_3D0), isNoisePhiStrip  ); }
		  else {  ATH_MSG_DEBUG ( "rpcSectorLayerNoiseNotCorr not in hist list!" );}
		}

	      }
	    }//calculate noise map end
	  }
  
     
	} // END IF (m_doClusters)  

	
    }}//m_doRpcESD // AthenaMonManager::tier0 || AthenaMonManager::tier0ESD        
      
  m_rpc_eventstotal++;

  return sc; // statuscode check 
  
}


/*----------------------------------------------------------------------------------*/
StatusCode RPCStandaloneTracksMon::bookHistogramsRecurrent( )
/*----------------------------------------------------------------------------------*/
{

  ATH_MSG_DEBUG ( "RPCStandaloneTracksMon Histograms being booked" );
 
  StatusCode sc = StatusCode::SUCCESS; 
  if( m_doRpcESD==true ) {if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online ) { 

      // MuonDetectorManager from the conditions store
      SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey};
      const MuonGM::MuonDetectorManager* MuonDetMgr = DetectorManagerHandle.cptr(); 
      if(!MuonDetMgr){
	ATH_MSG_ERROR("nullptr to the read MuonDetectorManager conditions object");
	return StatusCode::FAILURE; 
      } 
      
      //declare a group of histograms
      std::string generic_path_rpcmonitoring = "Muon/MuonRawDataMonitoring/RPCStandAloneTrackMon";
      MonGroup rpcprd_shift( this, generic_path_rpcmonitoring+"/Overview", run, ATTRIB_UNMANAGED  )      ;
      MonGroup rpcprd_expert( this, generic_path_rpcmonitoring+"/Overview", run, ATTRIB_UNMANAGED )     ;
      MonGroup rpcprd_expert_sum( this, generic_path_rpcmonitoring +"/Summary", run, ATTRIB_UNMANAGED ) ;
      MonGroup rpc_dqmf_global( this, generic_path_rpcmonitoring + "/GLOBAL", run, ATTRIB_UNMANAGED )    ;
      MonGroup rpcprd_dq_BA( this, generic_path_rpcmonitoring + "/RPCBA", run, ATTRIB_UNMANAGED  )       ;
      MonGroup rpcprd_dq_BC( this, generic_path_rpcmonitoring + "/RPCBC", run, ATTRIB_UNMANAGED )        ;
      MonGroup rpc_radiography(this, generic_path_rpcmonitoring +"/RPCRadiography",run, ATTRIB_UNMANAGED) ;
      MonGroup rpc_triggerefficiency(this, generic_path_rpcmonitoring +"/TriggerEfficiency",run, ATTRIB_UNMANAGED) ;
    
      if(newRunFlag())
	{      
	  ATH_MSG_INFO ( "RPCStandaloneTracksMon : begin of run" );
	
	  //book triggereff
	 
	 //PhiEtaCoin Thr_eff
	 m_hRPCPhiEtaCoinThr_eff.push_back( new TH1F("hRPCPhiEtaCoinThr_eff0","hRPCPhiEtaCoinThr_eff0" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCPhiEtaCoinThr_eff.back()) ; 
         m_hRPCPhiEtaCoinThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCPhiEtaCoinThr_eff.back()->GetYaxis()->SetTitle("RphiEtaCoin Thr_eff0");   
	 
	 m_hRPCPhiEtaCoinThr_eff.push_back( new TH1F("hRPCPhiEtaCoinThr_eff1","hRPCPhiEtaCoinThr_eff1" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCPhiEtaCoinThr_eff.back()) ; 
         m_hRPCPhiEtaCoinThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCPhiEtaCoinThr_eff.back()->GetYaxis()->SetTitle("RphiEtaCoin Thr_eff1");  
	 
	 m_hRPCPhiEtaCoinThr_eff.push_back( new TH1F("hRPCPhiEtaCoinThr_eff2","hRPCPhiEtaCoinThr_eff2" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCPhiEtaCoinThr_eff.back()) ; 
         m_hRPCPhiEtaCoinThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCPhiEtaCoinThr_eff.back()->GetYaxis()->SetTitle("RphiEtaCoin Thr_eff2");  
	 
	 m_hRPCPhiEtaCoinThr_eff.push_back( new TH1F("hRPCPhiEtaCoinThr_eff3","hRPCPhiEtaCoinThr_eff3" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCPhiEtaCoinThr_eff.back()) ; 
         m_hRPCPhiEtaCoinThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCPhiEtaCoinThr_eff.back()->GetYaxis()->SetTitle("RphiEtaCoin Thr_eff3");  
	 
	 m_hRPCPhiEtaCoinThr_eff.push_back( new TH1F("hRPCPhiEtaCoinThr_eff4","hRPCPhiEtaCoinThr_eff4" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCPhiEtaCoinThr_eff.back()) ; 
         m_hRPCPhiEtaCoinThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCPhiEtaCoinThr_eff.back()->GetYaxis()->SetTitle("RphiEtaCoin Thr_eff4"); 
	  
	 m_hRPCPhiEtaCoinThr_eff.push_back( new TH1F("hRPCPhiEtaCoinThr_eff5","hRPCPhiEtaCoinThr_eff5" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCPhiEtaCoinThr_eff.back()) ; 
         m_hRPCPhiEtaCoinThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCPhiEtaCoinThr_eff.back()->GetYaxis()->SetTitle("RphiEtaCoin Thr_eff5"); 
	 
	 //Pad Thr_eff
	 m_hRPCPadThr_eff.push_back( new TH1F("hRPCPadThr_eff0","hRPCPadThr_eff0" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCPadThr_eff.back()) ; 
         m_hRPCPadThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCPadThr_eff.back()->GetYaxis()->SetTitle("RPad Thr_eff0");   
	 
	 m_hRPCPadThr_eff.push_back( new TH1F("hRPCPadThr_eff1","hRPCPadThr_eff1" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCPadThr_eff.back()) ; 
         m_hRPCPadThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCPadThr_eff.back()->GetYaxis()->SetTitle("RPad Thr_eff1");  
	 
	 m_hRPCPadThr_eff.push_back( new TH1F("hRPCPadThr_eff2","hRPCPadThr_eff2" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCPadThr_eff.back()) ; 
         m_hRPCPadThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCPadThr_eff.back()->GetYaxis()->SetTitle("RPad Thr_eff2");  
	 
	 m_hRPCPadThr_eff.push_back( new TH1F("hRPCPadThr_eff3","hRPCPadThr_eff3" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCPadThr_eff.back()) ; 
         m_hRPCPadThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCPadThr_eff.back()->GetYaxis()->SetTitle("RPad Thr_eff3");  
	 
	 m_hRPCPadThr_eff.push_back( new TH1F("hRPCPadThr_eff4","hRPCPadThr_eff4" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCPadThr_eff.back()) ; 
         m_hRPCPadThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCPadThr_eff.back()->GetYaxis()->SetTitle("RPad Thr_eff4"); 
	  
	 m_hRPCPadThr_eff.push_back( new TH1F("hRPCPadThr_eff5","hRPCPadThr_eff5" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCPadThr_eff.back()) ; 
         m_hRPCPadThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCPadThr_eff.back()->GetYaxis()->SetTitle("RPad Thr_eff5");
	 
	 //Muctpi Thr_eff
	 m_hRPCMuctpiThr_eff.push_back( new TH1F("hRPCMuctpiThr_eff0","hRPCMuctpiThr_eff0" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCMuctpiThr_eff.back()) ; 
         m_hRPCMuctpiThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCMuctpiThr_eff.back()->GetYaxis()->SetTitle("RMuctpi Thr_eff0");   
	 
	 m_hRPCMuctpiThr_eff.push_back( new TH1F("hRPCMuctpiThr_eff1","hRPCMuctpiThr_eff1" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCMuctpiThr_eff.back()) ; 
         m_hRPCMuctpiThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCMuctpiThr_eff.back()->GetYaxis()->SetTitle("RMuctpi Thr_eff1");  
	 
	 m_hRPCMuctpiThr_eff.push_back( new TH1F("hRPCMuctpiThr_eff2","hRPCMuctpiThr_eff2" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCMuctpiThr_eff.back()) ; 
         m_hRPCMuctpiThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCMuctpiThr_eff.back()->GetYaxis()->SetTitle("RMuctpi Thr_eff2");  
	 
	 m_hRPCMuctpiThr_eff.push_back( new TH1F("hRPCMuctpiThr_eff3","hRPCMuctpiThr_eff3" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCMuctpiThr_eff.back()) ; 
         m_hRPCMuctpiThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCMuctpiThr_eff.back()->GetYaxis()->SetTitle("RMuctpi Thr_eff3");  
	 
	 m_hRPCMuctpiThr_eff.push_back( new TH1F("hRPCMuctpiThr_eff4","hRPCMuctpiThr_eff4" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCMuctpiThr_eff.back()) ; 
         m_hRPCMuctpiThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCMuctpiThr_eff.back()->GetYaxis()->SetTitle("RMuctpi Thr_eff4"); 
	  
	 m_hRPCMuctpiThr_eff.push_back( new TH1F("hRPCMuctpiThr_eff5","hRPCMuctpiThr_eff5" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCMuctpiThr_eff.back()) ; 
         m_hRPCMuctpiThr_eff.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCMuctpiThr_eff.back()->GetYaxis()->SetTitle("RMuctpi Thr_eff5");

	 
	 //PhiEtaCoin thr
	 m_hRPCPhiEtaCoinThr.push_back( new TH1F("hRPCPhiEtaCoinThr0","hRPCPhiEtaCoinThr0" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCPhiEtaCoinThr.back()) ; 
         m_hRPCPhiEtaCoinThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCPhiEtaCoinThr.back()->GetYaxis()->SetTitle("RphiEtaCoin Thr0");   
	 
	 m_hRPCPhiEtaCoinThr.push_back( new TH1F("hRPCPhiEtaCoinThr1","hRPCPhiEtaCoinThr1" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCPhiEtaCoinThr.back()) ; 
         m_hRPCPhiEtaCoinThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCPhiEtaCoinThr.back()->GetYaxis()->SetTitle("RphiEtaCoin Thr1");  
	 
	 m_hRPCPhiEtaCoinThr.push_back( new TH1F("hRPCPhiEtaCoinThr2","hRPCPhiEtaCoinThr2" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCPhiEtaCoinThr.back()) ; 
         m_hRPCPhiEtaCoinThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCPhiEtaCoinThr.back()->GetYaxis()->SetTitle("RphiEtaCoin Thr2");  
	 
	 m_hRPCPhiEtaCoinThr.push_back( new TH1F("hRPCPhiEtaCoinThr3","hRPCPhiEtaCoinThr3" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCPhiEtaCoinThr.back()) ; 
         m_hRPCPhiEtaCoinThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCPhiEtaCoinThr.back()->GetYaxis()->SetTitle("RphiEtaCoin Thr3");  
	 
	 m_hRPCPhiEtaCoinThr.push_back( new TH1F("hRPCPhiEtaCoinThr4","hRPCPhiEtaCoinThr4" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCPhiEtaCoinThr.back()) ; 
         m_hRPCPhiEtaCoinThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCPhiEtaCoinThr.back()->GetYaxis()->SetTitle("RphiEtaCoin Thr4"); 
	  
	 m_hRPCPhiEtaCoinThr.push_back( new TH1F("hRPCPhiEtaCoinThr5","hRPCPhiEtaCoinThr5" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCPhiEtaCoinThr.back()) ; 
         m_hRPCPhiEtaCoinThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCPhiEtaCoinThr.back()->GetYaxis()->SetTitle("RphiEtaCoin Thr5"); 
	 
	 //Pad thr
	 m_hRPCPadThr.push_back( new TH1F("hRPCPadThr0","hRPCPadThr0" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCPadThr.back()) ; 
         m_hRPCPadThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCPadThr.back()->GetYaxis()->SetTitle("RPad Thr0");   
	 
	 m_hRPCPadThr.push_back( new TH1F("hRPCPadThr1","hRPCPadThr1" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCPadThr.back()) ; 
         m_hRPCPadThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCPadThr.back()->GetYaxis()->SetTitle("RPad Thr1");  
	 
	 m_hRPCPadThr.push_back( new TH1F("hRPCPadThr2","hRPCPadThr2" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCPadThr.back()) ; 
         m_hRPCPadThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCPadThr.back()->GetYaxis()->SetTitle("RPad Thr2");  
	 
	 m_hRPCPadThr.push_back( new TH1F("hRPCPadThr3","hRPCPadThr3" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCPadThr.back()) ; 
         m_hRPCPadThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCPadThr.back()->GetYaxis()->SetTitle("RPad Thr3");  
	 
	 m_hRPCPadThr.push_back( new TH1F("hRPCPadThr4","hRPCPadThr4" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCPadThr.back()) ; 
         m_hRPCPadThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCPadThr.back()->GetYaxis()->SetTitle("RPad Thr4"); 
	  
	 m_hRPCPadThr.push_back( new TH1F("hRPCPadThr5","hRPCPadThr5" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCPadThr.back()) ; 
         m_hRPCPadThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCPadThr.back()->GetYaxis()->SetTitle("RPad Thr5");
	 
	 //Muctpi thr
	 m_hRPCMuctpiThr.push_back( new TH1F("hRPCMuctpiThr0","hRPCMuctpiThr0" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCMuctpiThr.back()) ; 
         m_hRPCMuctpiThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCMuctpiThr.back()->GetYaxis()->SetTitle("RMuctpi Thr0");   
	 
	 m_hRPCMuctpiThr.push_back( new TH1F("hRPCMuctpiThr1","hRPCMuctpiThr1" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCMuctpiThr.back()) ; 
         m_hRPCMuctpiThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCMuctpiThr.back()->GetYaxis()->SetTitle("RMuctpi Thr1");  
	 
	 m_hRPCMuctpiThr.push_back( new TH1F("hRPCMuctpiThr2","hRPCMuctpiThr2" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCMuctpiThr.back()) ; 
         m_hRPCMuctpiThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCMuctpiThr.back()->GetYaxis()->SetTitle("RMuctpi Thr2");  
	 
	 m_hRPCMuctpiThr.push_back( new TH1F("hRPCMuctpiThr3","hRPCMuctpiThr3" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCMuctpiThr.back()) ; 
         m_hRPCMuctpiThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCMuctpiThr.back()->GetYaxis()->SetTitle("RMuctpi Thr3");  
	 
	 m_hRPCMuctpiThr.push_back( new TH1F("hRPCMuctpiThr4","hRPCMuctpiThr4" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCMuctpiThr.back()) ; 
         m_hRPCMuctpiThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCMuctpiThr.back()->GetYaxis()->SetTitle("RMuctpi Thr4"); 
	  
	 m_hRPCMuctpiThr.push_back( new TH1F("hRPCMuctpiThr5","hRPCMuctpiThr5" , 400, 0., 100.));	    
         sc=rpc_triggerefficiency.regHist(m_hRPCMuctpiThr.back()) ; 
         m_hRPCMuctpiThr.back()->GetXaxis()->SetTitle("Pt[GeV]");
         m_hRPCMuctpiThr.back()->GetYaxis()->SetTitle("RMuctpi Thr5");
	  
	  
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
	
	  std::string generic_path_rpczxSurfaceView = generic_path_rpcmonitoring+"/Overview";
	  std:: string rpczxSurfaceView_title = "rpczxSurfaceView";
	  const char* rpczxSurfaceView_title_char = rpczxSurfaceView_title.c_str();  
	  	  
	  TH2 *rpczxSurfaceView=new TH2F(rpczxSurfaceView_title_char,rpczxSurfaceView_title_char,50,-50000,50000,50,-50000,50000);	  
	  sc = rpc_dqmf_global.regHist(rpczxSurfaceView) ;  
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpczxSurfaceView Failed to register histogram " );	    
	      return sc;
	    }
	  rpczxSurfaceView->SetFillColor(42);
	  rpczxSurfaceView->GetYaxis()->SetTitle("rpc track extrapolation x [mm]");
	  rpczxSurfaceView->GetXaxis()->SetTitle("rpc track extrapolation z [mm]"); 
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpczxSurfaceView << generic_path_rpczxSurfaceView.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );		 
	    ATH_MSG_DEBUG ( "Booked bookrpczxSurfaceView successfully" );}   
	  	  
	  
	  std::string generic_path_rpczSurfaceView = generic_path_rpcmonitoring+"/Overview";
	  std:: string rpczSurfaceView_title = "rpczSurfaceView";
	  const char* rpczSurfaceView_title_char = rpczSurfaceView_title.c_str();  

	  TH1 *rpczSurfaceView=new TH1F(rpczSurfaceView_title_char,rpczSurfaceView_title_char,50,-50000,50000);	  
	  sc=rpcprd_shift.regHist(rpczSurfaceView) ;  
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpczSurfaceView Failed to register histogram " );	    
	      return sc;
	    }
	  rpczSurfaceView->SetFillColor(42);
	  rpczSurfaceView->GetYaxis()->SetTitle("Counts");
	  rpczSurfaceView->GetXaxis()->SetTitle("rpc track extrapolation z [mm]"); 
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpczSurfaceView << generic_path_rpczSurfaceView.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );		 
	    ATH_MSG_DEBUG ( "Booked bookrpczSurfaceView successfully" );}   
	  	  
	  std::string generic_path_rpcxSurfaceView = generic_path_rpcmonitoring+"/Overview";
	  std:: string rpcxSurfaceView_title = "rpcxSurfaceView";
	  const char* rpcxSurfaceView_title_char = rpcxSurfaceView_title.c_str();  
	  
	  TH1 *rpcxSurfaceView=new TH1F(rpcxSurfaceView_title_char,rpcxSurfaceView_title_char,50,-50000,50000);	  
	  sc= rpcprd_shift.regHist(rpcxSurfaceView) ;  
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpcxSurfaceView Failed to register histogram " );	    
	      return sc;
	    }
	  rpcxSurfaceView->SetFillColor(42);
	  rpcxSurfaceView->GetYaxis()->SetTitle("counts");
	  rpcxSurfaceView->GetXaxis()->SetTitle("rpc track extrapolation x [mm]"); 
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpcxSurfaceView << generic_path_rpcxSurfaceView.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );		 
	    ATH_MSG_DEBUG ( "Booked bookrpcxSurfaceView successfully" );}   

    	    

	  std::string generic_path_f_rpczxSurfaceView = generic_path_rpcmonitoring+"/Overview";
	  std:: string f_rpczxSurfaceView_title = "f_rpczxSurfaceView";
	  const char* f_rpczxSurfaceView_title_char = f_rpczxSurfaceView_title.c_str();  

	  TH2 *f_rpczxSurfaceView=new TH2F(f_rpczxSurfaceView_title_char,f_rpczxSurfaceView_title_char,50,-50000,50000,50,-50000,50000);	  
	  sc=rpcprd_shift.regHist(f_rpczxSurfaceView) ;  
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "f_rpczxSurfaceView Failed to register histogram " );	    
	      return sc;
	    }
	  f_rpczxSurfaceView->SetFillColor(42);
	  f_rpczxSurfaceView->GetYaxis()->SetTitle("rpc track extrapolation x [mm]");
	  f_rpczxSurfaceView->GetXaxis()->SetTitle("rpc track extrapolation z [mm]"); 
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << f_rpczxSurfaceView << generic_path_f_rpczxSurfaceView.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookf_rpczxSurfaceView successfully" );}   
	
	
	  std::string generic_path_rpctrack_phivseta = generic_path_rpcmonitoring+"/Overview";
	  std:: string rpctrack_phivseta_title = "rpctrack_phivseta";
	  const char* rpctrack_phivseta_title_char = rpctrack_phivseta_title.c_str();
	
	  TH2 *rpctrack_phivseta=new TH2F(rpctrack_phivseta_title_char,rpctrack_phivseta_title_char,100,-1.5,1.5, 100, 0, 180); // 0-360
	  sc=rpcprd_shift.regHist(rpctrack_phivseta) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpctrack_phivseta Failed to register histogram " );
	      return sc;
	    }
	  rpctrack_phivseta->SetFillColor(42);
	  rpctrack_phivseta->GetXaxis()->SetTitle("pseudo-rapidity");
	  rpctrack_phivseta->GetYaxis()->SetTitle("phi angle [deg]");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpctrack_phivseta << generic_path_rpctrack_phivseta.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpctrack_phivseta successfully" );}
	

	  std::string generic_path_rpctrack_bvseta = generic_path_rpcmonitoring+"/Overview";
	  std:: string rpctrack_bvseta_title = "rpctrack_bvseta";
	  const char* rpctrack_bvseta_title_char = rpctrack_bvseta_title.c_str();

	  TH2 *rpctrack_bvseta=new TH2F(rpctrack_bvseta_title_char,rpctrack_bvseta_title_char, 100, -1.5, 1.5, 1000, 0, 50000);
	  sc=rpcprd_shift.regHist(rpctrack_bvseta) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpctrack_bvseta Failed to register histogram " );
	      return sc;
	    }
	  rpctrack_bvseta->SetFillColor(42);
	  rpctrack_bvseta->GetXaxis()->SetTitle("rpc track pseudo-rapidity");
	  rpctrack_bvseta->GetYaxis()->SetTitle("rpc track distance from IP [mm]");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpctrack_bvseta << generic_path_rpctrack_bvseta.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpctrack_bvseta successfully" );}



	  std::string generic_path_rpctrack_bVTXy0 = generic_path_rpcmonitoring+"/Overview";
	  std:: string rpctrack_bVTXy0_title = "rpctrack_bVTXy0";
	  const char* rpctrack_bVTXy0_title_char = rpctrack_bVTXy0_title.c_str();

	  TH2 *rpctrack_bVTXy0=new TH2F(rpctrack_bVTXy0_title_char,rpctrack_bVTXy0_title_char, 100, -5000, 5000, 100, -5000, 5000);
	  sc=rpcprd_shift.regHist(rpctrack_bVTXy0) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpctrack_bVTXy0 Failed to register histogram " );
	      return sc;
	    }
	  rpctrack_bVTXy0->SetFillColor(42);
	  rpctrack_bVTXy0->GetYaxis()->SetTitle("rpc track projection y=0, x [mm]");
	  rpctrack_bVTXy0->GetXaxis()->SetTitle("rpc track projection y=0, z [mm] ");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpctrack_bVTXy0 << generic_path_rpctrack_bVTXy0.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpctrack_bVTXy0 successfully" );}
     


	  std::string generic_path_rpctrack_bVTXz0 = generic_path_rpcmonitoring+"/Overview";
	  std:: string rpctrack_bVTXz0_title = "rpctrack_bVTXz0";
	  const char* rpctrack_bVTXz0_title_char = rpctrack_bVTXz0_title.c_str();

	  TH2 *rpctrack_bVTXz0=new TH2F(rpctrack_bVTXz0_title_char,rpctrack_bVTXz0_title_char, 100, -5000, 5000, 100, -5000, 5000);
	  sc=rpcprd_shift.regHist(rpctrack_bVTXz0) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpctrack_bVTXz0 Failed to register histogram " );
	      return sc;
	    }
	  rpctrack_bVTXz0->SetFillColor(42);
	  rpctrack_bVTXz0->GetXaxis()->SetTitle("rpc track projection z=0, x [mm]");
	  rpctrack_bVTXz0->GetYaxis()->SetTitle("rpc track projection z=0, y [mm] ");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpctrack_bVTXz0 << generic_path_rpctrack_bVTXz0.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpctrack_bVTXz0 successfully" );}
     
     
	  std::string generic_path_rpctrack_bVTXx0 = generic_path_rpcmonitoring+"/Overview";
	  std:: string rpctrack_bVTXx0_title = "rpctrack_bVTXx0";
	  const char* rpctrack_bVTXx0_title_char = rpctrack_bVTXx0_title.c_str();

	  TH2 *rpctrack_bVTXx0=new TH2F(rpctrack_bVTXx0_title_char,rpctrack_bVTXx0_title_char, 100, -5000, 5000, 100, -5000, 5000);
	  sc=rpcprd_shift.regHist(rpctrack_bVTXx0) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpctrack_bVTXx0 Failed to register histogram " );
	      return sc;
	    }
	  rpctrack_bVTXx0->SetFillColor(42);
	  rpctrack_bVTXx0->GetXaxis()->SetTitle("rpc track projection x=0, z [mm]");
	  rpctrack_bVTXx0->GetYaxis()->SetTitle("rpc track projection x=0, y [mm] ");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpctrack_bVTXx0 << generic_path_rpctrack_bVTXx0.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpctrack_bVTXx0 successfully" );}
     
	  std::string generic_path_rpctrack_phi = generic_path_rpcmonitoring+"/Overview";
	  std:: string rpctrack_phi_title = "rpctrack_phi";
	  const char* rpctrack_phi_title_char = rpctrack_phi_title.c_str();
     
	  TH1 *rpctrack_phi=new TH1F(rpctrack_phi_title_char,rpctrack_phi_title_char,100, 0, 360);
	  sc=rpcprd_shift.regHist(rpctrack_phi) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpctrack_phi Failed to register histogram " );
	      return sc;
	    }
	  rpctrack_phi->SetFillColor(42);
	  rpctrack_phi->GetYaxis()->SetTitle("Counts");
	  rpctrack_phi->GetXaxis()->SetTitle("phi angle [deg]");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpctrack_phi << generic_path_rpctrack_phi.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpctrack_phi successfully" );}
     
     
	  std::string generic_path_rpctrack_eta = generic_path_rpcmonitoring+"/Overview";
	  std:: string rpctrack_eta_title = "rpctrack_eta";
	  const char* rpctrack_eta_title_char = rpctrack_eta_title.c_str();
 
        
	  TH1 *rpctrack_eta=new TH1F(rpctrack_eta_title_char,rpctrack_eta_title_char,100,-1.5,1.5);
	  sc=rpcprd_shift.regHist(rpctrack_eta) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpctrack_eta Failed to register histogram " );
	      return sc;
	    }
	  rpctrack_eta->SetFillColor(42);
	  rpctrack_eta->GetXaxis()->SetTitle("pseudo-rapidity");
	  rpctrack_eta->GetYaxis()->SetTitle("Counts");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpctrack_eta << generic_path_rpctrack_eta.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpctrack_eta successfully" );}
	 
     
	  std::string generic_path_rpctrack_b = generic_path_rpcmonitoring+"/Overview";
	  std:: string rpctrack_b_title = "rpctrack_b";
	  const char* rpctrack_b_title_char = rpctrack_b_title.c_str();
     
	  TH1 *rpctrack_b=new TH1F(rpctrack_b_title_char,rpctrack_b_title_char,1000,0,50000);
	  sc=rpcprd_shift.regHist(rpctrack_b) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpctrack_b Failed to register histogram " );
	      return sc;
	    }
	  rpctrack_b->SetFillColor(42);
	  rpctrack_b->GetXaxis()->SetTitle("rpc track distance from IP [mm]");
	  rpctrack_b->GetXaxis()->SetTitle("Counts");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpctrack_b << generic_path_rpctrack_b.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpctrack_b successfully" );}
     
 
 
     
	  std::string generic_path_rpcmergepointdistance = generic_path_rpcmonitoring+"/Overview";
	  std:: string rpcmergepointdistance_title = "rpcmergepointdistance";
	  const char* rpcmergepointdistance_title_char = rpcmergepointdistance_title.c_str();
     
	  TH1 *rpcmergepointdistance=new TH1F(rpcmergepointdistance_title_char,rpcmergepointdistance_title_char,1000,0,1000);
	  sc=rpcprd_shift.regHist(rpcmergepointdistance) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpcmergepointdistance Failed to register histogram " );
	      return sc;
	    }
	  rpcmergepointdistance->SetFillColor(42);
	  rpcmergepointdistance->GetYaxis()->SetTitle("[counts]");
	  rpcmergepointdistance->GetXaxis()->SetTitle("merge point distance [mm]");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpcmergepointdistance << generic_path_rpcmergepointdistance.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpcmergepointdistance successfully" );}
     
     
     
 
	
	  std::string generic_path_f_rpcmergepointdistance = generic_path_rpcmonitoring+"/Overview";
	  std:: string f_rpcmergepointdistance_title = "f_rpcmergepointdistance";
	  const char* f_rpcmergepointdistance_title_char = f_rpcmergepointdistance_title.c_str();
	
	  TH1 *f_rpcmergepointdistance=new TH1F(f_rpcmergepointdistance_title_char,f_rpcmergepointdistance_title_char,1000,-0,1000);
	  sc=rpcprd_shift.regHist(f_rpcmergepointdistance) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "f_rpcmergepointdistance Failed to register histogram " );
	      return sc;
	    }
	  f_rpcmergepointdistance->SetFillColor(42);
	  f_rpcmergepointdistance->GetYaxis()->SetTitle("[counts]");
	  f_rpcmergepointdistance->GetXaxis()->SetTitle("merge point distance [mm]");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << f_rpcmergepointdistance << generic_path_f_rpcmergepointdistance.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookf_rpcmergepointdistance successfully" );}
	
    
 
 
     
	  std::string generic_path_rpcchi2dof = generic_path_rpcmonitoring+"/Overview";
	  std:: string rpcchi2dof_title = "rpcchi2dof";
	  const char* rpcchi2dof_title_char = rpcchi2dof_title.c_str();
     
	  TH1 *rpcchi2dof=new TH1F(rpcchi2dof_title_char,rpcchi2dof_title_char,200,0,20);
	  sc=rpcprd_shift.regHist(rpcchi2dof) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpcchi2dof Failed to register histogram " );
	      return sc;
	    }
	  rpcchi2dof->SetFillColor(42);
	  rpcchi2dof->GetYaxis()->SetTitle("[counts]");
	  rpcchi2dof->GetXaxis()->SetTitle("chi2/dof");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpcchi2dof << generic_path_rpcchi2dof.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpcchi2dof successfully" );}
     
     
     
 
	
	  std::string generic_path_f_rpcchi2dof = generic_path_rpcmonitoring+"/Overview";
	  std:: string f_rpcchi2dof_title = "f_rpcchi2dof";
	  const char* f_rpcchi2dof_title_char = f_rpcchi2dof_title.c_str();
	
	  TH1 *f_rpcchi2dof=new TH1F(f_rpcchi2dof_title_char,f_rpcchi2dof_title_char,200,0,20);
	  sc=rpcprd_shift.regHist(f_rpcchi2dof) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "f_rpcchi2dof Failed to register histogram " );
	      return sc;
	    }
	  f_rpcchi2dof->SetFillColor(42);
	  f_rpcchi2dof->GetYaxis()->SetTitle("[counts]");
	  f_rpcchi2dof->GetXaxis()->SetTitle("chi2/dof");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << f_rpcchi2dof << generic_path_f_rpcchi2dof.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookf_rpcchi2dof successfully" );}
        
	  std::string generic_path_rpcetavsphichi2dof = generic_path_rpcmonitoring+"/Overview";
	  std:: string rpcetavsphichi2dof_title = "rpcetavsphichi2dof";
	  const char* rpcetavsphichi2dof_title_char = rpcetavsphichi2dof_title.c_str();

	  TH1 *rpcetavsphichi2dof=new TH2F(rpcetavsphichi2dof_title_char,rpcetavsphichi2dof_title_char,200,0,20,200,0,20);
	  sc=rpcprd_shift.regHist(rpcetavsphichi2dof) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpcetavsphichi2dof Failed to register histogram " );
	      return sc;
	    }
	  rpcetavsphichi2dof->SetFillColor(42);
	  rpcetavsphichi2dof->GetYaxis()->SetTitle("etachi2/dof");
	  rpcetavsphichi2dof->GetXaxis()->SetTitle("phichi2/dof");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpcetavsphichi2dof << generic_path_rpcetavsphichi2dof.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpcetavsphichi2dof successfully" );}
	
	  
	  std::string generic_path_rpcNtracks = generic_path_rpcmonitoring+"/Overview";
	  std:: string rpcNtracks_title = "rpcNtracks";
	  const char* rpcNtracks_title_char = rpcNtracks_title.c_str();
	  
	  TH1 *rpcNtracks=new TH1F(rpcNtracks_title_char,rpcNtracks_title_char,10,0,10);
	  sc=rpc_dqmf_global.regHist(rpcNtracks) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpcNtracks Failed to register histogram " );
	      return sc;
	    }
	  rpcNtracks->SetFillColor(42);
	  rpcNtracks->GetYaxis()->SetTitle("[counts]");
	  rpcNtracks->GetXaxis()->SetTitle("Number of rpc tracks per event");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpcNtracks << generic_path_rpcNtracks.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpcNtracks successfully" );}
 
	  	  
	  std::string generic_path_f_rpcNtracks = generic_path_rpcmonitoring+"/Overview";
	  std:: string f_rpcNtracks_title = "f_rpcNtracks";
	  const char* f_rpcNtracks_title_char = f_rpcNtracks_title.c_str();
	          
	  TH1 *f_rpcNtracks=new TH1F(f_rpcNtracks_title_char,f_rpcNtracks_title_char,10,0,10);
	  sc=rpcprd_shift.regHist(f_rpcNtracks) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "f_rpcNtracks Failed to register histogram " );
	      return sc;
	    }
	  f_rpcNtracks->SetFillColor(42);
	  f_rpcNtracks->GetYaxis()->SetTitle("[counts]");
	  f_rpcNtracks->GetXaxis()->SetTitle("Number of rpc tracks per event");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << f_rpcNtracks << generic_path_f_rpcNtracks.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookf_rpcNtracks successfully" );}

 
	  std::string generic_path_rpcPointPerTracks = generic_path_rpcmonitoring+"/Overview";
	  std:: string rpcPointPerTracks_title = "rpcPointPerTracks";
	  const char* rpcPointPerTracks_title_char = rpcPointPerTracks_title.c_str();

	  TH1 *rpcPointPerTracks=new TH1F(rpcPointPerTracks_title_char,rpcPointPerTracks_title_char,10,0,10);
	  sc=rpcprd_shift.regHist(rpcPointPerTracks) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpcPointPerTracks Failed to register histogram " );
	      return sc;
	    }
	  rpcPointPerTracks->SetFillColor(42);
	  rpcPointPerTracks->GetYaxis()->SetTitle("[counts]");
	  rpcPointPerTracks->GetXaxis()->SetTitle("Number of 3D point per tracks");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpcPointPerTracks << generic_path_rpcPointPerTracks.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpcPointPerTracks successfully" );}
	  
	  
	  std::string generic_path_f_rpcPointPerTracks = generic_path_rpcmonitoring+"/Overview";
	  std:: string f_rpcPointPerTracks_title = "f_rpcPointPerTracks";
	  const char* f_rpcPointPerTracks_title_char = f_rpcPointPerTracks_title.c_str();
	
	  TH1 *f_rpcPointPerTracks=new TH1F(f_rpcPointPerTracks_title_char,f_rpcPointPerTracks_title_char,10,0,10);
	  sc=rpcprd_shift.regHist(f_rpcPointPerTracks) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "f_rpcPointPerTracks Failed to register histogram " );
	      return sc;
	    }
	  f_rpcPointPerTracks->SetFillColor(42);
	  f_rpcPointPerTracks->GetYaxis()->SetTitle("[counts]");
	  f_rpcPointPerTracks->GetXaxis()->SetTitle("Number of 3D point per tracks");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << f_rpcPointPerTracks << generic_path_f_rpcPointPerTracks.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookf_rpcPointPerTracks successfully" );}
	  
	
	  
	  std::string generic_path_rpcTrackType = generic_path_rpcmonitoring+"/Overview";
	  std:: string rpcTrackType_title = "rpcTrackType";
	  const char* rpcTrackType_title_char = rpcTrackType_title.c_str();
	
	  TH1 *rpcTrackType=new TH1F(rpcTrackType_title_char,rpcTrackType_title_char,64,0,64);
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
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpcTrackType << generic_path_rpcTrackType.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpcTrackType successfully" );}
	  
	  std::string generic_path_f_rpcTrackType = generic_path_rpcmonitoring+"/Overview";
	  std:: string f_rpcTrackType_title = "f_rpcTrackType";
	  const char* f_rpcTrackType_title_char = f_rpcTrackType_title.c_str();
	
	  TH1 *f_rpcTrackType=new TH1F(f_rpcTrackType_title_char,f_rpcTrackType_title_char,64,0,64);
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
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << f_rpcTrackType << generic_path_f_rpcTrackType.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookf_rpcTrackType successfully" );}
	
	  
	
	  
	  std::string generic_path_rpcPhiResidual = generic_path_rpcmonitoring+"/Overview";
	  std:: string rpcPhiResidual_title = "rpcPhiResidual";
	  const char* rpcPhiResidual_title_char = rpcPhiResidual_title.c_str();
	
	  TH1 *rpcPhiResidual=new TH1F(rpcPhiResidual_title_char,rpcPhiResidual_title_char,1000,-500,500);
	  sc=rpcprd_shift.regHist(rpcPhiResidual) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpcPhiResidual Failed to register histogram " );
	      return sc;
	    }
	  rpcPhiResidual->SetFillColor(42);
	  rpcPhiResidual->GetYaxis()->SetTitle("[counts]");
	  rpcPhiResidual->GetXaxis()->SetTitle("Phi residual [mm]");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpcPhiResidual << generic_path_rpcPhiResidual.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpcPhiResidual successfully" );}
	  
	  std::string generic_path_f_rpcPhiResidual = generic_path_rpcmonitoring+"/Overview";
	  std:: string f_rpcPhiResidual_title = "f_rpcPhiResidual";
	  const char* f_rpcPhiResidual_title_char = f_rpcPhiResidual_title.c_str();
	
	  TH1 *f_rpcPhiResidual=new TH1F(f_rpcPhiResidual_title_char,f_rpcPhiResidual_title_char,1000,-500,500);
	  sc=rpcprd_shift.regHist(f_rpcPhiResidual) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "f_rpcPhiResidual Failed to register histogram " );
	      return sc;
	    }
	  f_rpcPhiResidual->SetFillColor(42);
	  f_rpcPhiResidual->GetYaxis()->SetTitle("[counts]"); 

	  f_rpcPhiResidual->GetXaxis()->SetTitle("Phi spatial resolution");
 	  
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << f_rpcPhiResidual << generic_path_f_rpcPhiResidual.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookf_rpcPhiResidual successfully" );}
	

	  std::string generic_path_rpcEtaResidual = generic_path_rpcmonitoring+"/Overview";
	  std:: string rpcEtaResidual_title = "rpcEtaResidual";
	  const char* rpcEtaResidual_title_char = rpcEtaResidual_title.c_str();
	  
	  TH1 *rpcEtaResidual=new TH1F(rpcEtaResidual_title_char,rpcEtaResidual_title_char,1000,-500,500);
	  sc=rpcprd_shift.regHist(rpcEtaResidual) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "rpcEtaResidual Failed to register histogram " );
	      return sc;
	    }
	  rpcEtaResidual->SetFillColor(42);
	  rpcEtaResidual->GetYaxis()->SetTitle("[counts]");
	  rpcEtaResidual->GetXaxis()->SetTitle("Eta Residual [mm]");
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpcEtaResidual << generic_path_rpcEtaResidual.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpcEtaResidual successfully" );}
	
	  std::string generic_path_f_rpcEtaResidual = generic_path_rpcmonitoring+"/Overview";
	  std:: string f_rpcEtaResidual_title = "f_rpcEtaResidual";
	  const char* f_rpcEtaResidual_title_char = f_rpcEtaResidual_title.c_str();
	  
	  TH1 *f_rpcEtaResidual=new TH1F(f_rpcEtaResidual_title_char,f_rpcEtaResidual_title_char,1000,-500,500);
	  sc=rpcprd_shift.regHist(f_rpcEtaResidual) ;
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "f_rpcEtaResidual Failed to register histogram " );
	      return sc;
	    }
	  f_rpcEtaResidual->SetFillColor(42);
	  f_rpcEtaResidual->GetYaxis()->SetTitle("[counts]"); 
	  f_rpcEtaResidual->GetXaxis()->SetTitle("Eta spatial resolution [mm]");
	
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << f_rpcEtaResidual << generic_path_f_rpcEtaResidual.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookf_rpcEtaResidual successfully" );}

	 	  
	  std::string generic_path_LayerTrackProjection = generic_path_rpcmonitoring+"/Overview";
	  std::string LayerTrackProjection_title = "Track_Projected_on_Layer";
	  const char* LayerTrackProjection_title_char = LayerTrackProjection_title.c_str();  
	
	  m_LayerTrackProjection=new TH1I(LayerTrackProjection_title_char,LayerTrackProjection_title_char,24,0,24);	    
	  sc = rpc_dqmf_global.regHist(m_LayerTrackProjection) ;  
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "LayerTrackProjection Failed to register histogram " );       
	      return sc;
	    }
	  m_LayerTrackProjection->SetFillColor(42);
	  m_LayerTrackProjection->GetXaxis()->SetTitle("(0=LPt0,1=LPt1,2=Piv0,3=Piv1,4=HPt0,5=HPt1)+(eta=0,1=phi)*6+(0=Small,1=Large)*12");
	  m_LayerTrackProjection->GetYaxis()->SetTitle("Number of Tracks"); 
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << m_LayerTrackProjection << generic_path_LayerTrackProjection.c_str() );
	    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
	    ATH_MSG_DEBUG ( "RUN : " << run );	       
	    ATH_MSG_DEBUG ( "Booked bookLayerTrackProjection successfully" );}   
	

	  std::string generic_path_LayerHitOnTrack = generic_path_rpcmonitoring+"/Overview";
	  std::string LayerHitOnTrack_title = "Hit_on_Track_Layer";
	  const char* LayerHitOnTrack_title_char = LayerHitOnTrack_title.c_str();  
	 	  
	  m_LayerHitOnTrack=new TH1I(LayerHitOnTrack_title_char,LayerHitOnTrack_title_char,24,0,24);	    
	  sc = rpc_dqmf_global.regHist(m_LayerHitOnTrack) ;  
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "LayerHitOnTrack Failed to register histogram " );       
	      return sc;
	    }
	  m_LayerHitOnTrack->SetFillColor(42);
	  m_LayerHitOnTrack->GetXaxis()->SetTitle("LayerType(LPt0,LPt1,Piv0,Piv1,HPt0,HPt1) + ( Eta or Phi)*6 + (Small or Large)*12");
	  m_LayerHitOnTrack->GetXaxis()->SetTitleSize(0.03)        ;
	  m_LayerHitOnTrack->GetXaxis()->SetTitleOffset(1.2)       ;
	  m_LayerHitOnTrack->GetYaxis()->SetTitle("Number of Hits"); 
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << m_LayerHitOnTrack << generic_path_LayerHitOnTrack.c_str() );
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
	    int n_eta_station = ( (i_sec+1) %2 ) + 6 + 1 ;  // small: 1-6, large: 1-7, sec 12+14: 0-6, sec 13: 1-7 (8->7)	    
	    int n_bin = 72 * n_eta_station   ;  	    
	    //  72 = 2*2*2*3*3  measPhi + gap + dblPhi + dblZ + stationName + stationEta	  
	    std::string PanelIndexTitle = "# Panel = View + (gap-1)*2 + (dblPhi-1)*4 + (dblZ-1)*8 + (LowPt=0,Pivot=1,HighPt=2)*24 + (Eta)*72";
	  
	    // book panelID histograms
	    std::string SummaryPanelID_title      = "SummaryPanelID_" + sector_name ;
	    const char* SummaryPanelID_title_char = SummaryPanelID_title.c_str()  ;
	  
	    TH1* SummaryPanelID = new TH1F(SummaryPanelID_title_char, SummaryPanelID_title_char, 2*n_bin, -n_bin, n_bin);
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
	  	  
	    int panelBin   = 0   ;
	    int indexplane = 0   ;
	    Identifier panelId ;
	    
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
        // need to pay attention to BME case - not yet considered here .... 
        // need the full check here, since id.is_valid() is not enough to avoid exception in getRpcReadoutElement
        bool isValid=false; 
        Identifier rpcId = m_idHelperSvc->rpcIdHelper().channelID(iname, (ieta-8), int(i_sec/2)+1, ir+1, idbz+1, idbphi+1, 1, 1, 1, true, &isValid, true); // last 6 arguments are: int doubletPhi, int gasGap, int measuresPhi, int strip, bool check, bool* isValid, bool noPrint
        if (!isValid) {
          ATH_MSG_DEBUG("Could not find valid Identifier for station="<<iname<<", eta="<<(ieta-8)<<", phi="<<int(i_sec/2)+1<<", doubletR="<<ir+1<<", doubletZ="<<idbz+1<<", doubletPhi="<<idbphi+1<<", continuing...");
          continue;
        }
        const MuonGM::RpcReadoutElement* rpc = MuonDetMgr->getRpcReadoutElement(rpcId);
        if (rpc) {
			    
			    panelId = m_idHelperSvc->rpcIdHelper().panelID(iname, ieta-8, int(i_sec/2)+1, ir+1, idbz+1, idbphi+1, igap+1, imeasphi) ; 
			    
			    indexplane = ir ;
			    
			    if ( ir==0 && (iname==4 || iname==5 || iname == 9 || iname == 10) )  indexplane = 2 ;
			    
			    
			    panelBin = imeasphi + igap*2 + idbphi*4 + idbz*8 + indexplane*24 + std::abs(ieta-8)*72 ;
			    
			    //BOG0-4 one panel, BOF1-3 two panels, BOF4 one panel 
			    if ( ir==0 && iname == 10 ) {
			      // convention: BOG <-> doubletZ=4 <-> (3-1)*8=16
			      panelBin = imeasphi + igap*2 + idbphi*4 + 16 + indexplane*24 + std::abs(ieta-8)*72 ;
			    }
			    if ( ir==1 && iname == 10 ) {//feet extension BOG3-4
			      // convention: BOG with ir=1 <-> doubletZ=3 <-> (3-1)*8=16
			      panelBin = imeasphi + igap*2 + idbphi*4 + 16 + indexplane*24 + std::abs(ieta-8)*72 ;
			    }
			    if ( ir==1 && iname ==  9 ) {//feet extension BOF2-3 and BOF4
			      // convention: BOF with ir=1 <-> eta->eta+2
			      panelBin = imeasphi + igap*2 + idbphi*4 + idbz*8 + indexplane*24 + (std::abs(ieta-8)+2)*72 ;
			    }
			    //sector 13 BME
			    if ( iname==53 ) {
			      // convention: BME <-> doubletZ=3 <-> (3-1)*8=16
			      panelBin = imeasphi + igap*2 + idbphi*4 + 16 + indexplane*24 + std::abs(ieta-8)*72 ;
			    }
			    //sector 13 BOL8
			    if ( std::abs(ieta-8)==8 ) {
			      // convention: BOL8 <-> eta->eta-1
			      panelBin = imeasphi + igap*2 + idbphi*4 + 16 + indexplane*24 + 7*72 ;
			    }
	          
			    if ( (ieta-8) <0 ) panelBin = - panelBin; 
			    SummaryPanelID->Fill(panelBin, panelId.get_identifier32().get_compact() );
			    
			  }
			}
		      }
		    }
		  }
		}
	      }
	    }
	  
	    // 1) panel efficiency distribution per sector
	    std::string generic_path_SummaryEffDistriPerSector = generic_path_rpcmonitoring+"/Summary";
	    generic_path_SummaryEffDistriPerSector += sector_name;
	    std::string SummaryEffDistriPerSector_title      = "SummaryEffDistriPerSector_" + sector_name ;
	    const char* SummaryEffDistriPerSector_title_char = SummaryEffDistriPerSector_title.c_str()  ;
	    TH1* SummaryEffDistriPerSector = new TH1F ( SummaryEffDistriPerSector_title_char, SummaryEffDistriPerSector_title_char, 110, 0, 1.1 ) ;
	    SummaryEffDistriPerSector->SetFillColor(2);
	    SummaryEffDistriPerSector->GetXaxis()-> SetTitle("Panel Efficiency");
	    SummaryEffDistriPerSector->GetXaxis()-> SetTitleSize(0.03)	  ;
	    SummaryEffDistriPerSector->GetXaxis()-> SetTitleOffset(1.2)	  ;
	    SummaryEffDistriPerSector->GetYaxis()-> SetTitle("Counts/0.01");
	    {
	      ATH_MSG_DEBUG ( "INSIDE bookRPCHistograms : " << SummaryEffDistriPerSector << generic_path_SummaryEffDistriPerSector.c_str() );
	      //ATH_MSG_DEBUG ( "EXPERT : " << expert );
	      ATH_MSG_DEBUG ( "RUN : " << run );}
	
	    sc = rpcprd_expert_sum.regHist( SummaryEffDistriPerSector ) ;
	    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register  SummaryEffDistriPerSector hist to MonGroup" );    
	  
	    // 2) panel GapEfficiency distribution per sector
	    std::string generic_path_SummaryGapEffDistriPerSector = generic_path_rpcmonitoring+"/Summary";
	    generic_path_SummaryGapEffDistriPerSector += sector_name;
	    std::string SummaryGapEffDistriPerSector_title      = "SummaryGapEffDistriPerSector_" + sector_name ;
	    const char* SummaryGapEffDistriPerSector_title_char = SummaryGapEffDistriPerSector_title.c_str()  ;
	    TH1* SummaryGapEffDistriPerSector = new TH1F ( SummaryGapEffDistriPerSector_title_char, SummaryGapEffDistriPerSector_title_char, 110, 0, 1.1 ) ;
	    SummaryGapEffDistriPerSector->SetFillColor(2);
	    SummaryGapEffDistriPerSector->GetXaxis()-> SetTitle("GasGap Efficiency");
	    SummaryGapEffDistriPerSector->GetXaxis()-> SetTitleSize(0.03)	  ;
	    SummaryGapEffDistriPerSector->GetXaxis()-> SetTitleOffset(1.2)	  ;
	    SummaryGapEffDistriPerSector->GetYaxis()-> SetTitle("Counts/0.01");
	    {
	      ATH_MSG_DEBUG ( "INSIDE bookRPCHistograms : " << SummaryGapEffDistriPerSector << generic_path_SummaryGapEffDistriPerSector.c_str() );
	      //ATH_MSG_DEBUG ( "EXPERT : " << expert );
	      ATH_MSG_DEBUG ( "RUN : " << run );}
	
	    sc = rpcprd_expert_sum.regHist( SummaryGapEffDistriPerSector ) ;
	    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register  SummaryGapEffDistriPerSector hist to MonGroup" );    
	  
	    // 3) panel NoiseCorr distribution per sector
	    std::string generic_path_SummaryNoiseCorrDistriPerSector = generic_path_rpcmonitoring+"/Summary";
	    generic_path_SummaryNoiseCorrDistriPerSector += sector_name;
	    std::string SummaryNoiseCorrDistriPerSector_title      = "SummaryNoiseCorrDistriPerSector_" + sector_name ;
	    const char* SummaryNoiseCorrDistriPerSector_title_char = SummaryNoiseCorrDistriPerSector_title.c_str()  ;
	    TH1* SummaryNoiseCorrDistriPerSector = new TH1F ( SummaryNoiseCorrDistriPerSector_title_char, SummaryNoiseCorrDistriPerSector_title_char, 1000, 0, 100 ) ;
	    SummaryNoiseCorrDistriPerSector->SetFillColor(2);
	    SummaryNoiseCorrDistriPerSector->GetXaxis()-> SetTitle("Panel Correlated Noise");
	    SummaryNoiseCorrDistriPerSector->GetXaxis()-> SetTitleSize(0.03)	  ;
	    SummaryNoiseCorrDistriPerSector->GetXaxis()-> SetTitleOffset(1.2)	  ;
	    SummaryNoiseCorrDistriPerSector->GetYaxis()-> SetTitle("Counts/0.1Hz/cm2");
	    {
	      ATH_MSG_DEBUG ( "INSIDE bookRPCHistograms : " << SummaryNoiseCorrDistriPerSector << generic_path_SummaryNoiseCorrDistriPerSector.c_str() );
	      //ATH_MSG_DEBUG ( "EXPERT : " << expert );
	      ATH_MSG_DEBUG ( "RUN : " << run );}
	
	    sc = rpcprd_expert_sum.regHist( SummaryNoiseCorrDistriPerSector ) ;
	    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register  SummaryNoiseCorrDistriPerSector hist to MonGroup" );    
	  
	    // 4) panel NoiseTot distribution per sector
	    std::string generic_path_SummaryNoiseTotDistriPerSector = generic_path_rpcmonitoring+"/Summary";
	    generic_path_SummaryNoiseTotDistriPerSector += sector_name;
	    std::string SummaryNoiseTotDistriPerSector_title      = "SummaryNoiseTotDistriPerSector_" + sector_name ;
	    const char* SummaryNoiseTotDistriPerSector_title_char = SummaryNoiseTotDistriPerSector_title.c_str()  ;
	    TH1* SummaryNoiseTotDistriPerSector = new TH1F ( SummaryNoiseTotDistriPerSector_title_char,  SummaryNoiseTotDistriPerSector_title_char, 1000, 0, 100 ) ;
	    SummaryNoiseTotDistriPerSector->SetFillColor(2);
	    SummaryNoiseTotDistriPerSector->GetXaxis()-> SetTitle("Panel Noise Total");
	    SummaryNoiseTotDistriPerSector->GetXaxis()-> SetTitleSize(0.03)	  ;
	    SummaryNoiseTotDistriPerSector->GetXaxis()-> SetTitleOffset(1.2)	  ;
	    SummaryNoiseTotDistriPerSector->GetYaxis()-> SetTitle("Counts/0.1Hz/cm2");
	    {
	      ATH_MSG_DEBUG ( "INSIDE bookRPCHistograms : " << SummaryNoiseTotDistriPerSector << generic_path_SummaryNoiseTotDistriPerSector.c_str() );
	      //ATH_MSG_DEBUG ( "EXPERT : " << expert );
	      ATH_MSG_DEBUG ( "RUN : " << run );}
	
	    sc = rpcprd_expert_sum.regHist( SummaryNoiseTotDistriPerSector ) ;
	    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register  SummaryNoiseTotDistriPerSector hist to MonGroup" );    
	  
	    //    panel NoiseNotCorr distribution per sector
	    std::string generic_path_SummaryNoiseNotCorrDistriPerSector = generic_path_rpcmonitoring+"/Summary";
	    generic_path_SummaryNoiseNotCorrDistriPerSector += sector_name;
	    std::string SummaryNoiseNotCorrDistriPerSector_title      = "SummaryNoiseNotCorrDistriPerSector_" + sector_name ;
	    const char* SummaryNoiseNotCorrDistriPerSector_title_char = SummaryNoiseNotCorrDistriPerSector_title.c_str()  ;
	    TH1* SummaryNoiseNotCorrDistriPerSector = new TH1F ( SummaryNoiseNotCorrDistriPerSector_title_char,  SummaryNoiseNotCorrDistriPerSector_title_char, 1000, 0, 100 ) ;
	    SummaryNoiseNotCorrDistriPerSector->SetFillColor(2);
	    SummaryNoiseNotCorrDistriPerSector->GetXaxis()-> SetTitle("Panel Noise Not Corr");
	    SummaryNoiseNotCorrDistriPerSector->GetXaxis()-> SetTitleSize(0.03)	  ;
	    SummaryNoiseNotCorrDistriPerSector->GetXaxis()-> SetTitleOffset(1.2)	  ;
	    SummaryNoiseNotCorrDistriPerSector->GetYaxis()-> SetTitle("Counts/0.1Hz/cm2");
	    {
	      ATH_MSG_DEBUG ( "INSIDE bookRPCHistograms : " << SummaryNoiseNotCorrDistriPerSector << generic_path_SummaryNoiseNotCorrDistriPerSector.c_str() );
	      //ATH_MSG_DEBUG ( "EXPERT : " << expert );
	      ATH_MSG_DEBUG ( "RUN : " << run );}
	
	    sc = rpcprd_expert_sum.regHist( SummaryNoiseNotCorrDistriPerSector ) ;
	    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register  SummaryNoiseNotCorrDistriPerSector hist to MonGroup" );    
	  
	  
	    // 5) panel CS distribution per sector
	    std::string generic_path_SummaryCSDistriPerSector = generic_path_rpcmonitoring+"/Summary";
	    generic_path_SummaryCSDistriPerSector += sector_name;
	    std::string SummaryCSDistriPerSector_title      = "SummaryCSDistriPerSector_" + sector_name ;
	    const char* SummaryCSDistriPerSector_title_char = SummaryCSDistriPerSector_title.c_str()  ;
	    TH1* SummaryCSDistriPerSector = new TH1F ( SummaryCSDistriPerSector_title_char, SummaryCSDistriPerSector_title_char, 200, 0, 20);
	    SummaryCSDistriPerSector->SetFillColor(2);
	    SummaryCSDistriPerSector->GetXaxis()-> SetTitle("Panel  CS");
	    SummaryCSDistriPerSector->GetXaxis()-> SetTitleSize(0.03)	  ;
	    SummaryCSDistriPerSector->GetXaxis()-> SetTitleOffset(1.2)	  ;
	    SummaryCSDistriPerSector->GetYaxis()-> SetTitle("Counts/0.1");
	    {
	      ATH_MSG_DEBUG ( "INSIDE bookRPCHistograms : " << SummaryCSDistriPerSector << generic_path_SummaryCSDistriPerSector.c_str() );
	      //ATH_MSG_DEBUG ( "EXPERT : " << expert );
	      ATH_MSG_DEBUG ( "RUN : " << run );}
	
	    sc = rpcprd_expert_sum.regHist( SummaryCSDistriPerSector ) ;
	    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register  SummaryCSDistriPerSector hist to MonGroup" );    
	  
	    // 6) panel res_CS1 distribution per sector
	    std::string generic_path_SummaryRes_CS1DistriPerSector = generic_path_rpcmonitoring+"/Summary";
	    generic_path_SummaryRes_CS1DistriPerSector += sector_name;
	    std::string SummaryRes_CS1DistriPerSector_title      = "SummaryRes_CS1DistriPerSector_" + sector_name ;
	    const char* SummaryRes_CS1DistriPerSector_title_char = SummaryRes_CS1DistriPerSector_title.c_str()  ;
	    TH1* SummaryRes_CS1DistriPerSector = new TH1F ( SummaryRes_CS1DistriPerSector_title_char, SummaryRes_CS1DistriPerSector_title_char, 1000, -500, 500 ) ;
	    SummaryRes_CS1DistriPerSector->SetFillColor(2);
	    SummaryRes_CS1DistriPerSector->GetXaxis()-> SetTitle("Average Strip Panel res_CS1 [mm]");
	    SummaryRes_CS1DistriPerSector->GetXaxis()-> SetTitleSize(0.03)	  ;
	    SummaryRes_CS1DistriPerSector->GetXaxis()-> SetTitleOffset(1.2)	  ;
	    SummaryRes_CS1DistriPerSector->GetYaxis()-> SetTitle("Counts/1mm");
	    {
	      ATH_MSG_DEBUG ( "INSIDE bookRPCHistograms : " << SummaryRes_CS1DistriPerSector << generic_path_SummaryRes_CS1DistriPerSector.c_str() );
	      //ATH_MSG_DEBUG ( "EXPERT : " << expert );
	      ATH_MSG_DEBUG ( "RUN : " << run );}
	
	    sc = rpcprd_expert_sum.regHist( SummaryRes_CS1DistriPerSector ) ;
	    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register  SummaryRes_CS1DistriPerSector hist to MonGroup" );    
	  
	    // 7) panel res_CS2 distribution per sector
	    std::string generic_path_SummaryRes_CS2DistriPerSector = generic_path_rpcmonitoring+"/Summary";
	    generic_path_SummaryRes_CS2DistriPerSector += sector_name;
	    std::string SummaryRes_CS2DistriPerSector_title      = "SummaryRes_CS2DistriPerSector_" + sector_name ;
	    const char* SummaryRes_CS2DistriPerSector_title_char = SummaryRes_CS2DistriPerSector_title.c_str()  ;
	    TH1* SummaryRes_CS2DistriPerSector = new TH1F ( SummaryRes_CS2DistriPerSector_title_char, SummaryRes_CS2DistriPerSector_title_char, 1000, -500, 500 ) ;
	    SummaryRes_CS2DistriPerSector->SetFillColor(2);
	    SummaryRes_CS2DistriPerSector->GetXaxis()-> SetTitle("Average Strip Panel res_CS2 [mm]");
	    SummaryRes_CS2DistriPerSector->GetXaxis()-> SetTitleSize(0.03)	  ;
	    SummaryRes_CS2DistriPerSector->GetXaxis()-> SetTitleOffset(1.2)	  ;
	    SummaryRes_CS2DistriPerSector->GetYaxis()-> SetTitle("Counts/1mm");
	    {
	      ATH_MSG_DEBUG ( "INSIDE bookRPCHistograms : " << SummaryRes_CS2DistriPerSector << generic_path_SummaryRes_CS2DistriPerSector.c_str() );
	      //ATH_MSG_DEBUG ( "EXPERT : " << expert );
	      ATH_MSG_DEBUG ( "RUN : " << run );}
	
	    sc = rpcprd_expert_sum.regHist( SummaryRes_CS2DistriPerSector ) ;
	    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register  SummaryRes_CS2DistriPerSector hist to MonGroup" );    
	  
	    // 8) panel res_CSmore2 distribution per sector
	    std::string generic_path_SummaryRes_CSmore2DistriPerSector = generic_path_rpcmonitoring+"/Summary";
	    generic_path_SummaryRes_CSmore2DistriPerSector += sector_name;
	    std::string SummaryRes_CSmore2DistriPerSector_title      = "SummaryRes_CSmore2DistriPerSector_" + sector_name ;
	    const char* SummaryRes_CSmore2DistriPerSector_title_char = SummaryRes_CSmore2DistriPerSector_title.c_str()  ;
	    TH1* SummaryRes_CSmore2DistriPerSector = new TH1F ( SummaryRes_CSmore2DistriPerSector_title_char, SummaryRes_CSmore2DistriPerSector_title_char,1000, -500, 500 ) ;
	    SummaryRes_CSmore2DistriPerSector->SetFillColor(2);
	    SummaryRes_CSmore2DistriPerSector->GetXaxis()-> SetTitle("Average Strip Panel res_CSmore2 [mm]");
	    SummaryRes_CSmore2DistriPerSector->GetXaxis()-> SetTitleSize(0.03)	  ;
	    SummaryRes_CSmore2DistriPerSector->GetXaxis()-> SetTitleOffset(1.2)	  ;
	    SummaryRes_CSmore2DistriPerSector->GetYaxis()-> SetTitle("Counts/1mm");
	    {
	      ATH_MSG_DEBUG ( "INSIDE bookRPCHistograms : " << SummaryRes_CSmore2DistriPerSector << generic_path_SummaryRes_CSmore2DistriPerSector.c_str() );
	      //ATH_MSG_DEBUG ( "EXPERT : " << expert );
	      ATH_MSG_DEBUG ( "RUN : " << run );}
	
	    sc = rpcprd_expert_sum.regHist( SummaryRes_CSmore2DistriPerSector ) ;
	    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register  SummaryRes_CSmore2DistriPerSector hist to MonGroup" );    

 	  
	    // 9) panel Occupancy distribution per sector
	    std::string generic_path_SummaryOccupancyDistriPerSector = generic_path_rpcmonitoring+"/Summary";
	    generic_path_SummaryOccupancyDistriPerSector += sector_name;
	    std::string SummaryOccupancyDistriPerSector_title      = "SummaryOccupancyDistriPerSector_" + sector_name ;
	    const char* SummaryOccupancyDistriPerSector_title_char = SummaryOccupancyDistriPerSector_title.c_str()  ;
	    TH1* SummaryOccupancyDistriPerSector = new TH1F ( SummaryOccupancyDistriPerSector_title_char, SummaryOccupancyDistriPerSector_title_char, 100, -10., 2. ) ;
	    SummaryOccupancyDistriPerSector->SetFillColor(2);
	    SummaryOccupancyDistriPerSector->GetXaxis()-> SetTitle("Occupancy");
	    SummaryOccupancyDistriPerSector->GetXaxis()-> SetTitleSize(0.03)	  ;
	    SummaryOccupancyDistriPerSector->GetXaxis()-> SetTitleOffset(1.2)	  ;
	    SummaryOccupancyDistriPerSector->GetYaxis()-> SetTitle("Counts");
	    {
	      ATH_MSG_DEBUG ( "INSIDE bookRPCHistograms : " << SummaryOccupancyDistriPerSector << generic_path_SummaryOccupancyDistriPerSector.c_str() );
	      //ATH_MSG_DEBUG ( "EXPERT : " << expert );
	      ATH_MSG_DEBUG ( "RUN : " << run );}
	
	    sc = rpcprd_expert_sum.regHist( SummaryOccupancyDistriPerSector ) ;
	    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register  SummaryOccupancyDistriPerSector hist to MonGroup" );    

	  
	    // 10) panel res_CS1rms distribution per sector
	    std::string generic_path_SummaryRes_CS1rmsDistriPerSector = generic_path_rpcmonitoring+"/Summary";
	    generic_path_SummaryRes_CS1rmsDistriPerSector += sector_name;
	    std::string SummaryRes_CS1rmsDistriPerSector_title      = "SummaryRes_CS1rmsDistriPerSector_" + sector_name ;
	    const char* SummaryRes_CS1rmsDistriPerSector_title_char = SummaryRes_CS1rmsDistriPerSector_title.c_str()  ;
	    TH1* SummaryRes_CS1rmsDistriPerSector = new TH1F ( SummaryRes_CS1rmsDistriPerSector_title_char,  SummaryRes_CS1rmsDistriPerSector_title_char, 50, 0, 50 ) ;
	    SummaryRes_CS1rmsDistriPerSector->SetFillColor(2);
	    SummaryRes_CS1rmsDistriPerSector->GetXaxis()-> SetTitle("Average Strip Panel res_CS1 rms [mm]");
	    SummaryRes_CS1rmsDistriPerSector->GetXaxis()-> SetTitleSize(0.03)	  ;
	    SummaryRes_CS1rmsDistriPerSector->GetXaxis()-> SetTitleOffset(1.2)	  ;
	    SummaryRes_CS1rmsDistriPerSector->GetYaxis()-> SetTitle("Counts/1mm");
	    {
	      ATH_MSG_DEBUG ( "INSIDE bookRPCHistograms : " << SummaryRes_CS1rmsDistriPerSector << generic_path_SummaryRes_CS1rmsDistriPerSector.c_str() );
	      //ATH_MSG_DEBUG ( "EXPERT : " << expert );
	      ATH_MSG_DEBUG ( "RUN : " << run );}
	
	    sc = rpcprd_expert_sum.regHist( SummaryRes_CS1rmsDistriPerSector ) ;
	    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register  SummaryRes_CS1rmsDistriPerSector hist to MonGroup" );    
	  
	    // 11) panel res_CS2rms distribution per sector
	    std::string generic_path_SummaryRes_CS2rmsDistriPerSector = generic_path_rpcmonitoring+"/Summary";
	    generic_path_SummaryRes_CS2rmsDistriPerSector += sector_name;
	    std::string SummaryRes_CS2rmsDistriPerSector_title      = "SummaryRes_CS2rmsDistriPerSector_" + sector_name ;
	    const char* SummaryRes_CS2rmsDistriPerSector_title_char = SummaryRes_CS2rmsDistriPerSector_title.c_str()  ;
	    TH1* SummaryRes_CS2rmsDistriPerSector = new TH1F ( SummaryRes_CS2rmsDistriPerSector_title_char, SummaryRes_CS2rmsDistriPerSector_title_char,  50, 0, 50 ) ;
	    SummaryRes_CS2rmsDistriPerSector->SetFillColor(2);
	    SummaryRes_CS2rmsDistriPerSector->GetXaxis()-> SetTitle("Average Strip Panel res_CS2 rms [mm]");
	    SummaryRes_CS2rmsDistriPerSector->GetXaxis()-> SetTitleSize(0.03)	  ;
	    SummaryRes_CS2rmsDistriPerSector->GetXaxis()-> SetTitleOffset(1.2)	  ;
	    SummaryRes_CS2rmsDistriPerSector->GetYaxis()-> SetTitle("Counts/1mm");
	    {
	      ATH_MSG_DEBUG ( "INSIDE bookRPCHistograms : " << SummaryRes_CS2rmsDistriPerSector << generic_path_SummaryRes_CS2rmsDistriPerSector.c_str() );
	      //ATH_MSG_DEBUG ( "EXPERT : " << expert );
	      ATH_MSG_DEBUG ( "RUN : " << run );}
	
	    sc = rpcprd_expert_sum.regHist( SummaryRes_CS2rmsDistriPerSector ) ;
	    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register  SummaryRes_CS2rmsDistriPerSector hist to MonGroup" );    

	 	  
	    // 12) panel res_CSmore2rms distribution per sector
	    std::string generic_path_SummaryRes_CSmore2rmsDistriPerSector = generic_path_rpcmonitoring+"/Summary";
	    generic_path_SummaryRes_CSmore2rmsDistriPerSector += sector_name;
	    std::string SummaryRes_CSmore2rmsDistriPerSector_title      = "SummaryRes_CSmore2rmsDistriPerSector_" + sector_name ;
	    const char* SummaryRes_CSmore2rmsDistriPerSector_title_char = SummaryRes_CSmore2rmsDistriPerSector_title.c_str()  ;
	    TH1* SummaryRes_CSmore2rmsDistriPerSector = new TH1F ( SummaryRes_CSmore2rmsDistriPerSector_title_char, SummaryRes_CSmore2rmsDistriPerSector_title_char, 50, 0, 50 ) ;
	    SummaryRes_CSmore2rmsDistriPerSector->SetFillColor(2);
	    SummaryRes_CSmore2rmsDistriPerSector->GetXaxis()-> SetTitle("Average Strip Panel res_CSmore2 rms [mm]");
	    SummaryRes_CSmore2rmsDistriPerSector->GetXaxis()-> SetTitleSize(0.03)	  ;
	    SummaryRes_CSmore2rmsDistriPerSector->GetXaxis()-> SetTitleOffset(1.2)	  ;
	    SummaryRes_CSmore2rmsDistriPerSector->GetYaxis()-> SetTitle("Counts/1mm");
	    {
	      ATH_MSG_DEBUG ( "INSIDE bookRPCHistograms : " << SummaryRes_CSmore2rmsDistriPerSector << generic_path_SummaryRes_CSmore2rmsDistriPerSector.c_str() );
	      //ATH_MSG_DEBUG ( "EXPERT : " << expert );
	      ATH_MSG_DEBUG ( "RUN : " << run );}
	
	    sc = rpcprd_expert_sum.regHist( SummaryRes_CSmore2rmsDistriPerSector ) ;
	    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register  SummaryRes_CSmore2rmsDistriPerSector hist to MonGroup" );    
	

	 	  
	    // 13) panel Time distribution per sector
	    std::string generic_path_SummaryTimeDistriPerSector = generic_path_rpcmonitoring+"/Summary";
	    generic_path_SummaryTimeDistriPerSector += sector_name;
	    std::string SummaryTimeDistriPerSector_title      = "SummaryTimeDistriPerSector_" + sector_name ;
	    const char* SummaryTimeDistriPerSector_title_char = SummaryTimeDistriPerSector_title.c_str()  ;
	    TH1* SummaryTimeDistriPerSector = new TH1F ( SummaryTimeDistriPerSector_title_char, SummaryTimeDistriPerSector_title_char, 64, 0, 200 ) ;
	    SummaryTimeDistriPerSector->SetFillColor(2);
	    SummaryTimeDistriPerSector->GetXaxis()-> SetTitle("Average Strip Panel Time");
	    SummaryTimeDistriPerSector->GetXaxis()-> SetTitleSize(0.03)	  ;
	    SummaryTimeDistriPerSector->GetXaxis()-> SetTitleOffset(1.2)	  ;
	    SummaryTimeDistriPerSector->GetYaxis()-> SetTitle("Counts/3.125ns");
	    {
	      ATH_MSG_DEBUG ( "INSIDE bookRPCHistograms : " << SummaryTimeDistriPerSector << generic_path_SummaryTimeDistriPerSector.c_str() );
	      //ATH_MSG_DEBUG ( "EXPERT : " << expert );
	      ATH_MSG_DEBUG ( "RUN : " << run );}
	
	    sc = rpcprd_expert_sum.regHist( SummaryTimeDistriPerSector ) ;
	    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register  SummaryTimeDistriPerSector hist to MonGroup" ); 
	    
	    //TOTPanelsSummary += SummaryPanelID->GetEntries(); 
	 	  
	  }
	  m_SummaryHist_Size =  m_SummaryHist.size() ;
	
	
	  //////////////////////////////
	  // DQMF shift histograms   
        
	  // track time residual : dqmf shifter histogram
	  std::string generic_path_rpcTimeTrackRes = generic_path_rpcmonitoring+"/GLOBAL";
	  const char* rpcTimeTrackRes_title_char = "TimeTrackResidual";
	  TH1 *rpcTimeTrackRes = new TH1I(rpcTimeTrackRes_title_char, rpcTimeTrackRes_title_char, timeNbin, -timemaxrange/2, timemaxrange/2);
	  sc=rpc_dqmf_global.regHist( rpcTimeTrackRes ) ; 
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "TimeTrackResidulal Failed to register histogram " );       
	      return sc;
	    }
	  rpcTimeTrackRes->SetFillColor(42);
	  rpcTimeTrackRes->GetXaxis()->SetTitle("Track Time Residual [ns]");
	  rpcTimeTrackRes->GetYaxis()->SetTitle("Counts/(3.125ns)");
	
	  // track time RMS :  dqmf shifter histogram
	  std::string generic_path_rpcTimeTrackRMS = generic_path_rpcmonitoring+"/GLOBAL";
	  const char* rpcTimeTrackRMS_title_char = "TimeTrackRMS";
	  TH1 *rpcTimeTrackRMS = new TH1I(rpcTimeTrackRMS_title_char, rpcTimeTrackRMS_title_char, timeNbin, timeminrange, timemaxrange);
	  sc=rpcprd_shift.regHist( rpcTimeTrackRMS ) ; 
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "TimeTrackRMS Failed to register histogram " );       
	      return sc;
	    }
	  rpcTimeTrackRMS->SetFillColor(42);
	  rpcTimeTrackRMS->GetXaxis()->SetTitle("Track Time RMS [ns]");
	  rpcTimeTrackRMS->GetYaxis()->SetTitle("Counts/(3.125ns)");

	
	  // average efficiency per layer : dqmf shifter histogram	 (under evaluation)  	    
	  std::string generic_path_LayerEff = generic_path_rpcmonitoring+"/Overview";
	  std::string LayerEff_title = "Layer_Efficiency";
	  const char* LayerEff_title_char = LayerEff_title.c_str();  

	  m_LayerEff=new TH1F(LayerEff_title_char,LayerEff_title_char,24,0,24);	    
	  sc = rpc_dqmf_global.regHist(m_LayerEff) ;  
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "LayerEff Failed to register histogram " );       
	      return sc;
	    }
	  m_LayerEff->SetFillColor(42);
	  m_LayerEff->GetXaxis()->SetTitle("LayerType(LPt0,LPt1,Piv0,Piv1,HPt0,HPt1) + ( Eta or Phi)*6 + (Small or Large)*12");
	  m_LayerEff->GetXaxis()->SetTitleSize(0.03) ;
	  m_LayerEff->GetXaxis()->SetTitleOffset(1.2);
	  m_LayerEff->GetYaxis()->SetTitle("Average Layer Efficiency"); 
	  m_LayerEff->SetMarkerStyle(20)      ;
	  m_LayerEff->SetMarkerColor(2)       ;
	  {
	    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << m_LayerEff << generic_path_LayerEff.c_str() );
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
	  const char* rpcCS_HitOnTrack_title_char = "CS_HitOnTrack_Distribution";
	  TH1 *rpcCS_HitOnTrack=new TH1I(rpcCS_HitOnTrack_title_char, rpcCS_HitOnTrack_title_char, 20, 0.5, 20.5);
	  sc=rpcprd_shift.regHist(rpcCS_HitOnTrack) ;  
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "CS_HitOnTrack_Distribution Failed to register histogram " );	    
	      return sc;
	    }					
	  rpcCS_HitOnTrack->SetFillColor(42);
	  rpcCS_HitOnTrack->GetXaxis()->SetTitle("Cluster Size on correlated noise ");
	  rpcCS_HitOnTrack->GetYaxis()->SetTitle("Counts");      
	
	  // CS of noise hit distribution
	  const char* rpcCS_NoiseCorr_title_char = "CS_NoiseCorr_Distribution";
	  TH1 *rpcCS_NoiseCorr=new TH1I(rpcCS_NoiseCorr_title_char, rpcCS_NoiseCorr_title_char, 20, 0.5, 20.5);
	  sc=rpcprd_shift.regHist(rpcCS_NoiseCorr) ;  
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "CS_NoiseCorr_Distribution Failed to register histogram " );	    
	      return sc;
	    }					
	  rpcCS_NoiseCorr->SetFillColor(42);
	  rpcCS_NoiseCorr->GetXaxis()->SetTitle("Cluster Size on correlated noise ");
	  rpcCS_NoiseCorr->GetYaxis()->SetTitle("Counts");      
	
	  const char* rpcCS_NoiseNotCorr_title_char = "CS_NoiseNotCorr_Distribution";
	  TH1 *rpcCS_NoiseNotCorr=new TH1I(rpcCS_NoiseNotCorr_title_char, rpcCS_NoiseNotCorr_title_char, 20, 0.5, 20.5);
	  sc=rpcprd_shift.regHist(rpcCS_NoiseNotCorr) ;  
	  if(sc.isFailure())
	    {  ATH_MSG_FATAL ( "CS_NoiseNotCorr_Distribution Failed to register histogram " );	    
	      return sc;
	    }					
	  rpcCS_NoiseNotCorr->SetFillColor(42);
	  rpcCS_NoiseNotCorr->GetXaxis()->SetTitle("Cluster Size on correlated noise ");
	  rpcCS_NoiseNotCorr->GetYaxis()->SetTitle("Counts");      
	
	  // CS vs angle track 
	  const char* rpcCS_EtavsPhi_title_char = "CS_EtavsPhi";
	  TH1 *rpcCS_EtavsPhi=new TH2I(rpcCS_EtavsPhi_title_char, rpcCS_EtavsPhi_title_char, 20, 0.5, 20.5, 20, 0.5, 20.5);
	  sc=rpcprd_shift.regHist( rpcCS_EtavsPhi ) ;  
	  if(sc.isFailure())
	    { 
	      ATH_MSG_FATAL ( "CS_EtavsPhi Failed to register histogram " );	    
	      return sc;
	    }
	  rpcCS_EtavsPhi->GetXaxis()->SetTitle("CS Phi");     
	  rpcCS_EtavsPhi->GetYaxis()->SetTitle("CS Eta");
          
	
	  // CS vs angle track 
	  const char* rpcCS_angleLong_title_char = "CSvsLongitudinalAngleTrack";
	  TH1 *rpcCS_angleLong=new TH2I(rpcCS_angleLong_title_char, rpcCS_angleLong_title_char, 50, 0, 1, 20, 0.5, 20.5);
	  sc=rpcprd_shift.regHist( rpcCS_angleLong ) ;  
	  if(sc.isFailure())
	    { 
	      ATH_MSG_FATAL ( "CSvsLongitudinalAngleTrack Failed to register histogram " );	    
	      return sc;
	    }
	  rpcCS_angleLong->GetXaxis()->SetTitle("Longitudinal Incidence angle of tracks [degrees]");     
	  rpcCS_angleLong->GetYaxis()->SetTitle("Cluster Size");
          	
	  // CS vs angle track
	  const char* rpcCS_angleTrasv_title_char = "CSvsTrasversalAngleTrack";
	  TH1 *rpcCS_angleTrasv=new TH2I(rpcCS_angleTrasv_title_char, rpcCS_angleTrasv_title_char, 50, 0, 1, 20, 0.5, 20.5);
	  sc=rpcprd_shift.regHist( rpcCS_angleTrasv ) ;  
	  if(sc.isFailure())
	    { 
	      ATH_MSG_FATAL ( "CSvsTrasversalAngleTrack Failed to register histogram " );	    
	      return sc;
	    }
	  rpcCS_angleTrasv->GetXaxis()->SetTitle("trasversal Incidence angle of tracks [degrees]");  
	  rpcCS_angleTrasv->GetYaxis()->SetTitle("Cluster Size");
             
	  //Residual for CS
	  for ( int iCS=1; iCS!=maxCSres+1; iCS++ ) {
	         
	    char ResidualVsCS_title_char[100] = "ResidualVsCS";
	    sprintf(ResidualVsCS_title_char , "ResidualVsCS%d", iCS) ;  
	  
	    TH1 *ResidualVsCS=new TH1I(ResidualVsCS_title_char, ResidualVsCS_title_char, 2*(50+iCS*30) , -50-iCS*30, 50+iCS*30);
	    sc=rpcprd_shift.regHist( ResidualVsCS ) ;  
	    if(sc.isFailure())
	      { 
		ATH_MSG_FATAL ( "ResidualVsCS Failed to register histogram " );	    
		return sc;
	      }
	    ResidualVsCS->GetXaxis()->SetTitle("Residual [mm]");  
	    ResidualVsCS->GetYaxis()->SetTitle("Counts/[mm]");	
	 
	  }
         
	
	  if ( m_doCoolDB ) {
            std::vector<std::string> DB_NotNorm_list;
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
            std::vector<std::string> DB_list;
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
  std::string generic_path_rpcmonitoring = "Muon/MuonRawDataMonitoring/RPCStandAloneTrackMon";
     
  MonGroup rpcprd_dq_BA      ( this, generic_path_rpcmonitoring + "/RPCBA" , run, ATTRIB_UNMANAGED );
  MonGroup rpcprd_dq_BC      ( this, generic_path_rpcmonitoring + "/RPCBC" , run, ATTRIB_UNMANAGED );
		
  //1) distribution of average panel Efficiency
  std::string rpcAverageEff_C_title      = "Efficiency_Distribution_sideC"          ;
  const char* rpcAverageEff_C_title_char = rpcAverageEff_C_title.c_str();  

  m_rpcAverageSide_C.push_back( new TH1F(rpcAverageEff_C_title_char, rpcAverageEff_C_title_char, 110, -0.01, 1.09));	    
  sc=rpcprd_dq_BC.regHist(m_rpcAverageSide_C.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageEff_C Failed to register histogram " );       
      return ;
    }
  m_rpcAverageSide_C.back()->SetFillColor(42);
  m_rpcAverageSide_C.back()->GetXaxis()->SetTitle("Average Strip Panel Efficiency Side C");
  m_rpcAverageSide_C.back()->GetYaxis()->SetTitle("Counts/0.01"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " <<  rpcAverageEff_C_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageEff_C successfully" );}   
  
  std::string rpcAverageEff_A_title      = "Efficiency_Distribution_sideA"          ;
  const char* rpcAverageEff_A_title_char = rpcAverageEff_A_title.c_str();  
		
  m_rpcAverageSide_A.push_back( new TH1F(rpcAverageEff_A_title_char, rpcAverageEff_A_title_char, 110, -0.01, 1.09));	    
  sc=rpcprd_dq_BA.regHist(m_rpcAverageSide_A.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageEff_A Failed to register histogram " );       
      return;
    }
  m_rpcAverageSide_A.back()->SetFillColor(42);
  m_rpcAverageSide_A.back()->GetXaxis()->SetTitle("Average Strip Panel Efficiency Side A");
  m_rpcAverageSide_A.back()->GetYaxis()->SetTitle("Counts/0.01"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpcAverageEff_A_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageEff_A successfully" );}           

  //2) distribution of average panel GapEfficiency
  std::string rpcAverageGapEff_C_title      = "GapEfficiency_Distribution_sideC"          ;
  const char* rpcAverageGapEff_C_title_char = rpcAverageGapEff_C_title.c_str();  

  m_rpcAverageSide_C.push_back( new TH1F(rpcAverageGapEff_C_title_char, rpcAverageGapEff_C_title_char, 110, -0.01, 1.09));	    
  sc=rpcprd_dq_BC.regHist(m_rpcAverageSide_C.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageGasGapEff_C Failed to register histogram " );       
      return;
    }
  m_rpcAverageSide_C.back()->SetFillColor(42);
  m_rpcAverageSide_C.back()->GetXaxis()->SetTitle("Average Strip Panel GapEfficiency Side C");
  m_rpcAverageSide_C.back()->GetYaxis()->SetTitle("Counts/0.01"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpcAverageGapEff_C_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageGapEff_C successfully" );}
   		     	
  std::string rpcAverageGapEff_A_title      = "GapEfficiency_Distribution_sideA"          ;
  const char* rpcAverageGapEff_A_title_char = rpcAverageGapEff_A_title.c_str();  

  m_rpcAverageSide_A.push_back( new TH1F(rpcAverageGapEff_A_title_char, rpcAverageGapEff_A_title_char, 110, -0.01, 1.09));	    
  sc=rpcprd_dq_BA.regHist(m_rpcAverageSide_A.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageGasGapEfficiency Failed to register histogram " );       
      return;
    }
  m_rpcAverageSide_A.back()->SetFillColor(42);
  m_rpcAverageSide_A.back()->GetXaxis()->SetTitle("Average Strip Panel GapEfficiency Side A");
  m_rpcAverageSide_A.back()->GetYaxis()->SetTitle("Counts/0.01"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpcAverageGapEff_A_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageGapEff_A successfully" );}         
    
  //3) distribution of average panel NoiseCorr
  std::string rpcAverageNoiseCorr_C_title      = "NoiseCorr_Distribution_sideC"          ;
  const char* rpcAverageNoiseCorr_C_title_char = rpcAverageNoiseCorr_C_title.c_str();  
  
  m_rpcAverageSide_C.push_back( new TH1F(rpcAverageNoiseCorr_C_title_char, rpcAverageNoiseCorr_C_title_char, 1000, 0, 100));	    
  sc=rpcprd_dq_BC.regHist(m_rpcAverageSide_C.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageNoiseCorr_Ciciency Failed to register histogram " );       
      return;
    }
  m_rpcAverageSide_C.back()->SetFillColor(42);
  m_rpcAverageSide_C.back()->GetXaxis()->SetTitle("Average Strip Panel NoiseCorr Side C");
  m_rpcAverageSide_C.back()->GetYaxis()->SetTitle("Counts/0.1/Hz/cm2"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : "  << rpcAverageNoiseCorr_C_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageNoiseCorr_C successfully" );}

  std::string rpcAverageNoiseCorr_A_title      = "NoiseCorr_Distribution_sideA"          ;
  const char* rpcAverageNoiseCorr_A_title_char = rpcAverageNoiseCorr_A_title.c_str();  

  m_rpcAverageSide_A.push_back( new TH1F(rpcAverageNoiseCorr_A_title_char, rpcAverageNoiseCorr_A_title_char, 1000, 0, 100));	    
  sc=rpcprd_dq_BA.regHist(m_rpcAverageSide_A.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageNoiseCorr_A Failed to register histogram " );       
      return;
    }
  m_rpcAverageSide_A.back()->SetFillColor(42);
  m_rpcAverageSide_A.back()->GetXaxis()->SetTitle("Average Strip Panel NoiseCorr Side A");
  m_rpcAverageSide_A.back()->GetYaxis()->SetTitle("Counts/0.1/Hz/cm2"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " <<  rpcAverageNoiseCorr_A_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageNoiseCorr_A successfully" );}          
  
  
  //4) distribution of average panel NoiseTot
  std::string rpcAverageNoiseTot_C_title      = "NoiseTot_Distribution_sideC"          ;
  const char* rpcAverageNoiseTot_C_title_char = rpcAverageNoiseTot_C_title.c_str();  
 
  m_rpcAverageSide_C.push_back( new TH1F(rpcAverageNoiseTot_C_title_char, rpcAverageNoiseTot_C_title_char, 1000, 0, 100));	    
  sc=rpcprd_dq_BC.regHist(m_rpcAverageSide_C.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageNoiseTot_C Failed to register histogram " );       
      return;
    }
  m_rpcAverageSide_C.back()->SetFillColor(42);
  m_rpcAverageSide_C.back()->GetXaxis()->SetTitle("Average Strip Panel NoiseTot Side C");
  m_rpcAverageSide_C.back()->GetYaxis()->SetTitle("Counts/0.1/Hz/cm2"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " <<  rpcAverageNoiseTot_C_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageNoiseTot_C successfully" );}      
		
  std::string rpcAverageNoiseTot_A_title      = "NoiseTot_Distribution_sideA"          ;
  const char* rpcAverageNoiseTot_A_title_char = rpcAverageNoiseTot_A_title.c_str()   ;  

  m_rpcAverageSide_A.push_back( new TH1F(rpcAverageNoiseTot_A_title_char, rpcAverageNoiseTot_A_title_char, 1000, 0, 100));	    
  sc=rpcprd_dq_BA.regHist(m_rpcAverageSide_A.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageNoiseTot_A Failed to register histogram " );       
      return;
    }
  m_rpcAverageSide_A.back()->SetFillColor(42);
  m_rpcAverageSide_A.back()->GetXaxis()->SetTitle("Average Strip Panel NoiseTot Side A");
  m_rpcAverageSide_A.back()->GetYaxis()->SetTitle("Counts/0.1/Hz/cm2"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " <<  rpcAverageNoiseTot_A_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageNoiseTot_A successfully" );}            
   		     	

  //5)   distribution of average panel NoiseNotCorr
  std::string rpcAverageNoiseNotCorr_C_title      = "NoiseNotCorr_Distribution_sideC"          ;
  const char* rpcAverageNoiseNotCorr_C_title_char = rpcAverageNoiseNotCorr_C_title.c_str();  
 
  m_rpcAverageSide_C.push_back( new TH1F(rpcAverageNoiseNotCorr_C_title_char, rpcAverageNoiseNotCorr_C_title_char, 1000, 0, 100));	    
  sc=rpcprd_dq_BC.regHist(m_rpcAverageSide_C.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageNoiseNotCorr_C Failed to register histogram " );       
      return;
    }
  m_rpcAverageSide_C.back()->SetFillColor(42);
  m_rpcAverageSide_C.back()->GetXaxis()->SetTitle("Average Strip Panel NoiseNotCorr Side C");
  m_rpcAverageSide_C.back()->GetYaxis()->SetTitle("Counts/0.1/Hz/cm2"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " <<  rpcAverageNoiseNotCorr_C_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageNoiseNotCorr_C successfully" );}      

  std::string rpcAverageNoiseNotCorr_A_title      = "NoiseNotCorr_Distribution_sideA"          ;
  const char* rpcAverageNoiseNotCorr_A_title_char = rpcAverageNoiseNotCorr_A_title.c_str();  

  m_rpcAverageSide_A.push_back( new TH1F(rpcAverageNoiseNotCorr_A_title_char, rpcAverageNoiseNotCorr_A_title_char, 1000, 0, 100));	    
  sc=rpcprd_dq_BA.regHist(m_rpcAverageSide_A.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageNoiseNotCorr_A Failed to register histogram " );       
      return;
    }
  m_rpcAverageSide_A.back()->SetFillColor(42);
  m_rpcAverageSide_A.back()->GetXaxis()->SetTitle("Average Strip Panel NoiseNotCorr Side A");
  m_rpcAverageSide_A.back()->GetYaxis()->SetTitle("Counts/0.1/Hz/cm2"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " <<  rpcAverageNoiseNotCorr_A_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageNoiseNotCorr_A successfully" );}      

  
  //5) distribution of average panel CS
  std::string rpcAverageCS_C_title      = "CS_Distribution_sideC"          ;
  const char* rpcAverageCS_C_title_char = rpcAverageCS_C_title.c_str();  
  
  m_rpcAverageSide_C.push_back( new TH1F(rpcAverageCS_C_title_char, rpcAverageCS_C_title_char, 200, 0.1, 19.9) );	    
  sc=rpcprd_dq_BC.regHist(m_rpcAverageSide_C.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageCS_C Failed to register histogram " );       
      return;
    }
  m_rpcAverageSide_C.back()->SetFillColor(42);
  m_rpcAverageSide_C.back()->GetXaxis()->SetTitle("Average Strip Panel CS Side C");
  m_rpcAverageSide_C.back()->GetYaxis()->SetTitle("Counts/0.1"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " <<  rpcAverageCS_C_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageCS_C successfully" );}  
		
  std::string rpcAverageCS_A_title      = "CS_Distribution_sideA"          ;
  const char* rpcAverageCS_A_title_char = rpcAverageCS_A_title.c_str();  
  
  m_rpcAverageSide_A.push_back( new TH1F(rpcAverageCS_A_title_char, rpcAverageCS_A_title_char, 200, 0.1, 19.9) );	    
  sc=rpcprd_dq_BA.regHist(m_rpcAverageSide_A.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageCS_A Failed to register histogram " );       
      return;
    }
  m_rpcAverageSide_A.back()->SetFillColor(42);
  m_rpcAverageSide_A.back()->GetXaxis()->SetTitle("Average Strip Panel CS Side A");
  m_rpcAverageSide_A.back()->GetYaxis()->SetTitle("Counts/0.1"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " <<  rpcAverageCS_A_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageCS_A successfully" );}  
  

  //6) distribution of average panel res_CS1
  std::string rpcAverageRes_CS1_C_title      = "Res_CS1_Distribution_sideC"          ;
  const char* rpcAverageRes_CS1_C_title_char = rpcAverageRes_CS1_C_title.c_str();  

  m_rpcAverageSide_C.push_back( new TH1F(rpcAverageRes_CS1_C_title_char, rpcAverageRes_CS1_C_title_char, 1000, -500, 500));	    
  sc=rpcprd_dq_BC.regHist(m_rpcAverageSide_C.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageRes_CS1_C Failed to register histogram " );       
      return;
    }
  m_rpcAverageSide_C.back()->SetFillColor(42);
  m_rpcAverageSide_C.back()->GetXaxis()->SetTitle("Average Strip Panel res_CS1 Side C [mm]");
  m_rpcAverageSide_C.back()->GetYaxis()->SetTitle("Counts/1mm"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " <<  rpcAverageRes_CS1_C_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageRes_CS1_C successfully" );} 

  std::string rpcAverageRes_CS1_A_title      = "Res_CS1_Distribution_sideA"          ;
  const char* rpcAverageRes_CS1_A_title_char = rpcAverageRes_CS1_A_title.c_str();  

  m_rpcAverageSide_A.push_back( new TH1F(rpcAverageRes_CS1_A_title_char, rpcAverageRes_CS1_A_title_char, 1000, -500, 500));	    
  sc=rpcprd_dq_BA.regHist(m_rpcAverageSide_A.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageRes_CS1_A Failed to register histogram " );       
      return;
    }
  m_rpcAverageSide_A.back()->SetFillColor(42);
  m_rpcAverageSide_A.back()->GetXaxis()->SetTitle("Average Strip Panel res_CS1 Side A");
  m_rpcAverageSide_A.back()->GetYaxis()->SetTitle("Counts/1mm"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " <<  rpcAverageRes_CS1_A_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageRes_CS1_A successfully" );}        
		
  //7) distribution of average panel res_CS2
  std::string rpcAverageRes_CS2_C_title      = "Res_CS2_Distribution_sideC"          ;
  const char* rpcAverageRes_CS2_C_title_char = rpcAverageRes_CS2_C_title.c_str();  

  m_rpcAverageSide_C.push_back( new TH1F(rpcAverageRes_CS2_C_title_char, rpcAverageRes_CS2_C_title_char, 1000, -500, 500));	    
  sc=rpcprd_dq_BC.regHist(m_rpcAverageSide_C.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageRes_CS2_C Failed to register histogram " );       
      return;
    }
  m_rpcAverageSide_C.back()->SetFillColor(42);
  m_rpcAverageSide_C.back()->GetXaxis()->SetTitle("Average Strip Panel res_CS2 Side C [mm]");
  m_rpcAverageSide_C.back()->GetYaxis()->SetTitle("Counts/1mm"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpcAverageRes_CS2_C_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageRes_CS2_C successfully" );}         

  std::string rpcAverageRes_CS2_A_title      = "Res_CS2_Distribution_sideA"          ;
  const char* rpcAverageRes_CS2_A_title_char = rpcAverageRes_CS2_A_title.c_str();  

  m_rpcAverageSide_A.push_back( new TH1F(rpcAverageRes_CS2_A_title_char, rpcAverageRes_CS2_A_title_char, 1000, -500, 500));	    
  sc=rpcprd_dq_BA.regHist(m_rpcAverageSide_A.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageRes_CS2_A Failed to register histogram " );       
      return;
    }
  m_rpcAverageSide_A.back()->SetFillColor(42);
  m_rpcAverageSide_A.back()->GetXaxis()->SetTitle("Average Strip Panel res_CS2 Side A [mm]");
  m_rpcAverageSide_A.back()->GetYaxis()->SetTitle("Counts/1mm"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " <<  rpcAverageRes_CS2_A_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageRes_CS2_A successfully" );}

  //8) distribution of average panel res_CSmore2
  std::string rpcAverageRes_CSmore2_C_title      = "Res_CSmore2_Distribution_sideC"          ;
  const char* rpcAverageRes_CSmore2_C_title_char = rpcAverageRes_CSmore2_C_title.c_str();  
  
  m_rpcAverageSide_C.push_back( new TH1F(rpcAverageRes_CSmore2_C_title_char, rpcAverageRes_CSmore2_C_title_char, 1000, -500, 500));	    
  sc=rpcprd_dq_BC.regHist(m_rpcAverageSide_C.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageRes_CSmore2_C Failed to register histogram " );       
      return;
    }
  m_rpcAverageSide_C.back()->SetFillColor(42);
  m_rpcAverageSide_C.back()->GetXaxis()->SetTitle("Average Strip Panel res_CSmore2 Side C [mm]");
  m_rpcAverageSide_C.back()->GetYaxis()->SetTitle("Counts/1mm"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpcAverageRes_CSmore2_C_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageRes_CSmore2_C successfully" );}         
   		    
  std::string rpcAverageRes_CSmore2_A_title      = "Res_CSmore2_Distribution_sideA"          ;
  const char* rpcAverageRes_CSmore2_A_title_char = rpcAverageRes_CSmore2_A_title.c_str();  
  
  m_rpcAverageSide_A.push_back( new TH1F(rpcAverageRes_CSmore2_A_title_char, rpcAverageRes_CSmore2_A_title_char, 1000, -500, 500));	    
  sc=rpcprd_dq_BA.regHist(m_rpcAverageSide_A.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageRes_CSmore2_A Failed to register histogram " );       
      return;
    }
  m_rpcAverageSide_A.back()->SetFillColor(42);
  m_rpcAverageSide_A.back()->GetXaxis()->SetTitle("Average Strip Panel res_CSmore2 Side A [mm]");
  m_rpcAverageSide_A.back()->GetYaxis()->SetTitle("Counts/1mm"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpcAverageRes_CSmore2_A_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageRes_CSmore2_A successfully" );} 

   		     	
  //9) distribution of average panel res_CS1rms
  std::string rpcAverageRes_CS1_rms_C_title      = "Res_CS1rms_Distribution_sideC"          ;
  const char* rpcAverageRes_CS1_rms_C_title_char = rpcAverageRes_CS1_rms_C_title.c_str();  

  m_rpcAverageSide_C.push_back( new TH1F(rpcAverageRes_CS1_rms_C_title_char, rpcAverageRes_CS1_rms_C_title_char, 1000, 0, 500));	    
  sc=rpcprd_dq_BC.regHist(m_rpcAverageSide_C.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageRes_CS1_rms_C Failed to register histogram " );       
      return;
    }
  m_rpcAverageSide_C.back()->SetFillColor(42);
  m_rpcAverageSide_C.back()->GetXaxis()->SetTitle("Average Strip Panel res_CS1rms Side C [mm]");
  m_rpcAverageSide_C.back()->GetYaxis()->SetTitle("Counts/1mm"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpcAverageRes_CS1_rms_C_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageRes_CS1_rms_C successfully" );}         
   		     	
  
  std::string rpcAverageRes_CS1_rms_A_title      = "Res_CS1rms_Distribution_sideA"          ;
  const char* rpcAverageRes_CS1_rms_A_title_char = rpcAverageRes_CS1_rms_A_title.c_str();  
  
  m_rpcAverageSide_A.push_back( new TH1F(rpcAverageRes_CS1_rms_A_title_char, rpcAverageRes_CS1_rms_A_title_char, 1000, 0, 500));	    
  sc=rpcprd_dq_BA.regHist(m_rpcAverageSide_A.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageRes_CS1_rms_A Failed to register histogram " );       
      return;
    }
  m_rpcAverageSide_A.back()->SetFillColor(42);
  m_rpcAverageSide_A.back()->GetXaxis()->SetTitle("Average Strip Panel res_CS1rms SideA");
  m_rpcAverageSide_A.back()->GetYaxis()->SetTitle("Counts/1mm"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpcAverageRes_CS1_rms_A_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageRes_CS1_rms_A successfully" );} 
	   
  //10) distribution of average panel res_CS2rms
  std::string rpcAverageRes_CS2_rms_C_title      = "Res_CS2rms_Distribution_sideC"          ;
  const char* rpcAverageRes_CS2_rms_C_title_char = rpcAverageRes_CS2_rms_C_title.c_str();  

  m_rpcAverageSide_C.push_back( new TH1F(rpcAverageRes_CS2_rms_C_title_char, rpcAverageRes_CS2_rms_C_title_char, 1000, 0, 500));	    
  sc=rpcprd_dq_BC.regHist(m_rpcAverageSide_C.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageRes_CS2_rms_C Failed to register histogram " );       
      return;
    }
  m_rpcAverageSide_C.back()->SetFillColor(42);
  m_rpcAverageSide_C.back()->GetXaxis()->SetTitle("Average Strip Panel res_CS2rms Side C [mm]");
  m_rpcAverageSide_C.back()->GetYaxis()->SetTitle("Counts/1mm"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpcAverageRes_CS2_rms_C_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageRes_CS2_rms_C successfully" );}         
   		     	

  std::string rpcAverageRes_CS2_rms_A_title      = "Res_CS2rms_Distribution_sideA"          ;
  const char* rpcAverageRes_CS2_rms_A_title_char = rpcAverageRes_CS2_rms_A_title.c_str();  

  m_rpcAverageSide_A.push_back( new TH1F(rpcAverageRes_CS2_rms_A_title_char, rpcAverageRes_CS2_rms_A_title_char, 1000, 0, 500));	    
  sc=rpcprd_dq_BA.regHist(m_rpcAverageSide_A.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageRes_CS2_rms_A Failed to register histogram " );       
      return;
    }
  m_rpcAverageSide_A.back()->SetFillColor(42);
  m_rpcAverageSide_A.back()->GetXaxis()->SetTitle("Average Strip Panel res_CS2rms Side A [mm]");
  m_rpcAverageSide_A.back()->GetYaxis()->SetTitle("Counts/1mm"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " <<  rpcAverageRes_CS2_rms_A_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageRes_CS2_rms_A successfully" );} 

  //11) distribution of average panel res_CSmore2rms
  std::string rpcAverageRes_CSmore2_rms_C_title      = "Res_CSmore2rms_Distribution_sideC"          ;
  const char* rpcAverageRes_CSmore2_rms_C_title_char = rpcAverageRes_CSmore2_rms_C_title.c_str();  
 
  m_rpcAverageSide_C.push_back( new TH1F(rpcAverageRes_CSmore2_rms_C_title_char, rpcAverageRes_CSmore2_rms_C_title_char, 1000, 0, 500));	    
  sc=rpcprd_dq_BC.regHist(m_rpcAverageSide_C.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageRes_CSmore2_rms_C Failed to register histogram " );       
      return;
    }
  m_rpcAverageSide_C.back()->SetFillColor(42);
  m_rpcAverageSide_C.back()->GetXaxis()->SetTitle("Average Strip Panel res_CSmore2rms Side C [mm]");
  m_rpcAverageSide_C.back()->GetYaxis()->SetTitle("Counts/1mm"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpcAverageRes_CSmore2_rms_C_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageRes_CSmore2_rms_C successfully" );} 
 
 
  std::string rpcAverageRes_CSmore2_rms_A_title      = "Res_CSmore2rms_Distribution_sideA"          ;
  const char* rpcAverageRes_CSmore2_rms_A_title_char = rpcAverageRes_CSmore2_rms_A_title.c_str();  
    
  m_rpcAverageSide_A.push_back( new TH1F(rpcAverageRes_CSmore2_rms_A_title_char, rpcAverageRes_CSmore2_rms_A_title_char, 1000, 0, 500));	    
  sc=rpcprd_dq_BA.regHist(m_rpcAverageSide_A.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageRes_CSmore2_rms_A Failed to register histogram " );       
      return;
    }
  m_rpcAverageSide_A.back()->SetFillColor(42);
  m_rpcAverageSide_A.back()->GetXaxis()->SetTitle("Average Strip Panel res_CSmore2rms Side A [mm]");
  m_rpcAverageSide_A.back()->GetYaxis()->SetTitle("Counts/1mm"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpcAverageRes_CSmore2_rms_A_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageRes_CSmore2_rms_A successfully" );} 
    
  //12) distribution of average panel Occupancy
  std::string generic_path_rpcAverageOccupancy_C = generic_path_rpcmonitoring+"/GLOBAL";
  std::string rpcAverageOccupancy_C_title      = "Occupancy_Distribution_sideC"          ;
  const char* rpcAverageOccupancy_C_title_char = rpcAverageOccupancy_C_title.c_str();  
  
  m_rpcAverageSide_C.push_back( new TH1F(rpcAverageOccupancy_C_title_char, rpcAverageOccupancy_C_title_char, 100, -10, 2));	    
  sc=rpcprd_dq_BC.regHist(m_rpcAverageSide_C.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpc_Occupancy_distribution Failed to register histogram " );       
      return;
    }
  m_rpcAverageSide_C.back()->SetFillColor(42);
  m_rpcAverageSide_C.back()->GetXaxis()->SetTitle("log10(Average Strip Panel Occupancy Side C)");
  m_rpcAverageSide_C.back()->GetYaxis()->SetTitle("Counts/0.01"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpcAverageOccupancy_C_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageOccupancy_C distribution successfully" );}  

  std::string generic_path_rpcAverageOccupancy_A = generic_path_rpcmonitoring+"/GLOBAL";
  std::string rpcAverageOccupancy_A_title      = "Occupancy_Distribution_sideA"          ;
  const char* rpcAverageOccupancy_A_title_char = rpcAverageOccupancy_A_title.c_str();  
  
  m_rpcAverageSide_A.push_back( new TH1F(rpcAverageOccupancy_A_title_char, rpcAverageOccupancy_A_title_char, 100, -10, 2));	    
  sc=rpcprd_dq_BA.regHist(m_rpcAverageSide_A.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpc_Occupancy_distribution Failed to register histogram " );       
      return;
    }
  m_rpcAverageSide_A.back()->SetFillColor(42);
  m_rpcAverageSide_A.back()->GetXaxis()->SetTitle("log10(Average Strip Panel Occupancy Side A)");
  m_rpcAverageSide_A.back()->GetYaxis()->SetTitle("Counts/0.01"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " <<  rpcAverageOccupancy_A_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageOccupancy_A distribution successfully" );}
		
     
  //13) distribution of average panel Time
  std::string rpcAverageTime_C_title      = "Time_Distribution_sideC"          ;
  const char* rpcAverageTime_C_title_char = rpcAverageTime_C_title.c_str();  
 
  m_rpcAverageSide_C.push_back( new TH1F(rpcAverageTime_C_title_char, rpcAverageTime_C_title_char, 64, 0, 200));	    
  sc=rpcprd_dq_BC.regHist(m_rpcAverageSide_C.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageTime_C Failed to register histogram " );       
      return;
    }
  m_rpcAverageSide_C.back()->SetFillColor(42);
  m_rpcAverageSide_C.back()->GetXaxis()->SetTitle("Average Strip Panel Time Side C");
  m_rpcAverageSide_C.back()->GetYaxis()->SetTitle("Counts/3.125ns"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpcAverageTime_C_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageTime_C successfully" );}      
    
  std::string rpcAverageTime_A_title      = "Time_Distribution_sideA"          ;
  const char* rpcAverageTime_A_title_char = rpcAverageTime_A_title.c_str();  
    
  m_rpcAverageSide_A.push_back( new TH1F(rpcAverageTime_A_title_char, rpcAverageTime_A_title_char, 64, 0, 200));	    
  sc=rpcprd_dq_BA.regHist(m_rpcAverageSide_A.back()) ;  
  if(sc.isFailure())
    {  ATH_MSG_FATAL ( "rpcAverageTime_A Failed to register histogram " );       
      return;
    }
  m_rpcAverageSide_A.back()->SetFillColor(42);
  m_rpcAverageSide_A.back()->GetXaxis()->SetTitle("Average Strip Panel Time Side A");
  m_rpcAverageSide_A.back()->GetYaxis()->SetTitle("Counts/3.125ns"); 
  {
    ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpcAverageTime_A_title.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageTime_A successfully" );}      
    
}
    
// Radiography histograms
void RPCStandaloneTracksMon::bookRPCLayerRadiographyHistograms( int isec, std::string layer_name )
{

  ATH_MSG_DEBUG ( "bookRPCLayerRadiographyHistograms" );
  
  gErrorIgnoreLevel=kInfo;
  StatusCode sc = StatusCode::SUCCESS;      
  if(sc.isFailure() )  return ;
   
  if( m_doRpcESD==true ) {if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD ) { 

      // MuonDetectorManager from the conditions store
      SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey};
      const MuonGM::MuonDetectorManager* MuonDetMgr = DetectorManagerHandle.cptr(); 
      if(!MuonDetMgr){
	ATH_MSG_ERROR("nullptr to the read MuonDetectorManager conditions object");
	return; 
      } 

      char histName_char[100];
      sprintf(histName_char,"Sector%.2d", isec+1 ) ;
      std::string sector_name = histName_char      ;
      int istatPhi = int( isec/2) + 1;
      int iName    = 0              ;
      int ir = 0;
      if ( isec!=11 &&  isec!=13) {
	// if ( m_layer.find("Pivot",0) )
	if ( layer_name == "Pivot0" || layer_name == "Pivot1" )   {
	  iName = 2 + (isec%2 ) ;
	  ir    = 2 	        ;
	}
	if ( layer_name == "LowPt0" || layer_name == "LowPt1" )   {
	  iName = 2 + (isec%2 ) ;
	  ir    = 1 	        ;
	}
	if ( layer_name == "HighPt0" || layer_name == "HighPt1" ) {
	  iName = 4 + (isec%2 ) ;
	  ir    = 1 	        ;
	}
      }
      else {
	if ( layer_name == "Pivot0" || layer_name == "Pivot1" )   {
	  iName = 8 ;
	  ir    = 2 ;
	}
	if ( layer_name == "LowPt0" || layer_name == "LowPt1" )   {
	  iName = 8 ;
	  ir    = 1 ;
	}
	if ( layer_name == "HighPt0" || layer_name == "HighPt1" ) {
	  iName = 9 ; // or 10 ;
	  ir    = 1 ; // doubletR=2 -> upgrade of Atlas
	}   
      }

      Identifier rpcId = m_idHelperSvc->rpcIdHelper().channelID(iName, 1, istatPhi, ir, 1, 1, 1, 1, 1); // last 5 arguments are: int doubletZ, int doubletPhi, int gasGap, int measuresPhi, int strip
      if (!rpcId.is_valid()) {
        ATH_MSG_WARNING("Could not get valid Identifier for stationName="<<iName<<", eta=1, phi="<<istatPhi<<", doubletR="<<ir);
      }
      const MuonGM::RpcReadoutElement* rpc = MuonDetMgr->getRpcReadoutElement(rpcId);
 
      int NphiStrips         =  0;
      int NetaStripsTotSideA =  0;
      int NetaStripsTotSideC =  0;
      if(rpc){
	Identifier idr = rpc->identify();
	std::vector<int>   rpcstripshift = RpcGM::RpcStripShift(MuonDetMgr,m_idHelperSvc->rpcIdHelper(),idr, 0)  ;
        NphiStrips         =  rpcstripshift[0] ;
        NetaStripsTotSideA =  rpcstripshift[6] ;
        NetaStripsTotSideC =  rpcstripshift[7] ;
      }
    
      std::string generic_path_rpcmonitoring = "Muon/MuonRawDataMonitoring/RPCStandAloneTrackMon";
      MonGroup rpc_radiography(this, generic_path_rpcmonitoring + "/RPCRadiography", run, ATTRIB_UNMANAGED ) ;
    
      int binz    = (NetaStripsTotSideA+NetaStripsTotSideC)/m_rpcreducenbinsstrip ;
      int binminz = - NetaStripsTotSideC ;
      int binmaxz =	NetaStripsTotSideA ;
      int binx    = NphiStrips/m_rpcreducenbinsstrip ;
      int binminx = 0 ;
      int binmaxx = NphiStrips ;
   
 
      std::string rpcSectorLayerProj_title	= sector_name + "_" + layer_name + "_TrackProj" ;
      const char* rpcSectorLayerProj_title_char = rpcSectorLayerProj_title.c_str();
    
      TH2 * rpcSectorLayerTrackProj = new TH2I( rpcSectorLayerProj_title_char, rpcSectorLayerProj_title_char, binz, binminz, binmaxz, binx, binminx, binmaxx);
    
      sc = rpc_radiography.regHist( rpcSectorLayerTrackProj );
      if(sc.isFailure() ){  
        ATH_MSG_WARNING ( "couldn't register " << rpcSectorLayerProj_title << " hist to MonGroup" );
        return ;
      }
    
      rpcSectorLayerTrackProj->SetOption("COLZ");
      rpcSectorLayerTrackProj->GetXaxis()->SetTitle("<--- SIDE C      Rpc Eta strip      SIDE A --->");
      if ( (isec%2)== 0 ) { rpcSectorLayerTrackProj->GetYaxis()->SetTitle("<--- HV side     Rpc Phi strip     RO side --->"); }  // large
      else		  { rpcSectorLayerTrackProj->GetYaxis()->SetTitle("<--- RO side     Rpc Phi strip     HV side --->"); }
    
    
      std::string rpcSectorLayerResponse_title      = sector_name + "_" + layer_name + "_Response" ;
      const char* rpcSectorLayerResponse_title_char = rpcSectorLayerResponse_title.c_str();
 
      TH2 * rpcSectorLayerResponse = new TH2I( rpcSectorLayerResponse_title_char, rpcSectorLayerResponse_title_char ,binz, binminz, binmaxz, binx, binminx, binmaxx);
 
      sc = rpc_radiography.regHist( rpcSectorLayerResponse );
      if(sc.isFailure() )  
      {  
        ATH_MSG_WARNING ( "couldn't register " << rpcSectorLayerResponse_title << " hist to MonGroup" );
        return ;
      }
      rpcSectorLayerResponse->SetOption("COLZ");
      rpcSectorLayerResponse->GetXaxis()->SetTitle("<--- SIDE C	   Rpc Eta strip      SIDE A --->");
      if ( (isec%2) == 0) { rpcSectorLayerResponse->GetYaxis()->SetTitle("<--- HV side     Rpc Phi strip     RO side --->"); }  // large
      else	          { rpcSectorLayerResponse->GetYaxis()->SetTitle("<--- RO side     Rpc Phi strip     HV side --->"); }
    
      // eta OR phi response
      std::string rpcSectorLayerResponseOR_title      = sector_name + "_" + layer_name + "_ResponseEtaOrPhi" ;
      const char* rpcSectorLayerResponseOR_title_char = rpcSectorLayerResponseOR_title.c_str();
 
      TH2 * rpcSectorLayerResponseOR = new TH2I( rpcSectorLayerResponseOR_title_char, rpcSectorLayerResponseOR_title_char ,binz, binminz, binmaxz, binx, binminx, binmaxx);
 
      sc = rpc_radiography.regHist( rpcSectorLayerResponseOR );
      if(sc.isFailure() ) 
      { 
        ATH_MSG_WARNING ( "couldn't register " << rpcSectorLayerResponseOR_title << " hist to MonGroup" );
        return ;
      }
      
      
      rpcSectorLayerResponseOR->SetOption("COLZ");
      rpcSectorLayerResponseOR->GetXaxis()->SetTitle("<--- SIDE C	   Rpc Eta strip      SIDE A --->");
      if ( (isec%2) == 0) { rpcSectorLayerResponseOR->GetYaxis()->SetTitle("<--- HV side     Rpc Phi strip     RO side --->"); }  // large
      else	          { rpcSectorLayerResponse->GetYaxis()->SetTitle("<--- RO side     Rpc Phi strip     HV side --->"); }
    
    
    
      //ATLAS noise 
      std::string rpcSectorLayerNoiseCorr_title      = sector_name + "_" + layer_name + "_NoiseCorr" ;
      const char* rpcSectorLayerNoiseCorr_title_char = rpcSectorLayerNoiseCorr_title.c_str();
   
      TH2 * rpcSectorLayerNoiseCorr = new TH2I( rpcSectorLayerNoiseCorr_title_char, rpcSectorLayerNoiseCorr_title_char ,binz, binminz, binmaxz, binx, binminx, binmaxx);
 
      sc = rpc_radiography.regHist( rpcSectorLayerNoiseCorr );
      if(sc.isFailure() )  
      {  
       ATH_MSG_WARNING ( "couldn't register " << rpcSectorLayerNoiseCorr_title << " hist to MonGroup" );
       return ;
      }
      
      rpcSectorLayerNoiseCorr->SetOption("COLZ");
      rpcSectorLayerNoiseCorr->GetXaxis()->SetTitle("<--- SIDE C	   Rpc Eta strip      SIDE A --->");
      if ( (isec%2) == 0) { rpcSectorLayerNoiseCorr->GetYaxis()->SetTitle("<--- HV side     Rpc Phi strip     RO side --->"); }  // large
      else	          { rpcSectorLayerNoiseCorr->GetYaxis()->SetTitle("<--- RO side     Rpc Phi strip     HV side --->"); }
    
    
      std::string rpcSectorLayerNoiseNotCorr_title      = sector_name + "_" + layer_name + "_NoiseNotCorr" ;
      const char* rpcSectorLayerNoiseNotCorr_title_char = rpcSectorLayerNoiseNotCorr_title.c_str();
   
      TH2 * rpcSectorLayerNoiseNotCorr = new TH2I( rpcSectorLayerNoiseNotCorr_title_char, rpcSectorLayerNoiseNotCorr_title_char ,binz, binminz, binmaxz, binx, binminx, binmaxx);
 
      sc = rpc_radiography.regHist( rpcSectorLayerNoiseNotCorr );
      if(sc.isFailure() )  
      {  
      ATH_MSG_WARNING ( "couldn't register " << rpcSectorLayerNoiseNotCorr_title << " hist to MonGroup" ); 
       return ;
      }
      rpcSectorLayerNoiseNotCorr->SetOption("COLZ");
      rpcSectorLayerNoiseNotCorr->GetXaxis()->SetTitle("<--- SIDE C	   Rpc Eta strip      SIDE A --->");
      if ( (isec%2) == 0) { rpcSectorLayerNoiseNotCorr->GetYaxis()->SetTitle("<--- HV side     Rpc Phi strip     RO side --->"); }  // large
      else	          { rpcSectorLayerNoiseNotCorr->GetYaxis()->SetTitle("<--- RO side     Rpc Phi strip     HV side --->"); }
    
    
    }}  
  
}

// BOOK COOL STRIP HISTOGRAMSS
void RPCStandaloneTracksMon::bookRPCCoolHistograms_NotNorm( std::vector<std::string>::const_iterator & iter, int isec, int idblPhi, std::string layer ) 
{
  gErrorIgnoreLevel=kInfo;
  ATH_MSG_DEBUG ( "RPCStandaloneTracksMon: bookRPCCoolHistograms_NotNorm" );
  
  StatusCode sc = StatusCode::SUCCESS ;
  if(sc.isFailure() )  
  {
       return ;
  }
  std::string generic_path_rpcmonitoring = "Muon/MuonRawDataMonitoring/RPCStandAloneTrackMon";
  MonGroup rpcCoolDb( this, generic_path_rpcmonitoring+"/CoolDB", run, ATTRIB_UNMANAGED );
  
  char histName_char[100];
  sprintf(histName_char,"Sector%.2d_%s_dblPhi%d", isec+1, layer.c_str(), idblPhi+1) ;
  // example: Sector01_Pivot0_dblPhi1_StripId
  
  std::string histName  = histName_char  ;
  histName += "_"            ;
  histName += *iter        ;  //histName += m_coolQuantity ;
  int istatPhi  = int( isec/2) ;
  int iName     = 0              ;
  int ir = 0;
  
  //BML7(dr=1) is associated to LowPt and not Pivot
  if ( isec<11 ||  isec>13) {
    // if ( layer.find("Pivot",0) )
    if ( layer == "Pivot0" || layer == "Pivot1" )   {
      iName = 2 + (isec%2 ) ;
      ir    = 2 	      ; 
    }
    if ( layer == "LowPt0" || layer == "LowPt1" )   {
      iName = 2 + (isec%2 ) ;
      ir    = 1 	      ; 
    }
    if ( layer == "HighPt0" || layer == "HighPt1" ) {
      iName = 4 + (isec%2 ) ;
      ir    = 1   ;
    }
  }  
  else if ( isec==12) {
    // if ( layer.find("Pivot",0) )
    if ( layer == "Pivot0" || layer == "Pivot1" )   {
      iName     =  1      ; 
      ir    = 2 	      ;	 
    }
    if ( layer == "LowPt0" || layer == "LowPt1" )   {
      iName     =  1      ; 
      ir    = 1 	      ; 
    }
    if ( layer == "HighPt0" || layer == "HighPt1" ) {
      iName = 4               ;
      ir    = 1 	      ;   
    }
     
  }
  else {
    if ( layer == "Pivot0" || layer == "Pivot1" )   {
      iName     =   8  ; 
      ir    = 2 ;     
    }
    if ( layer == "LowPt0" || layer == "LowPt1" )   {
      iName = 8 ; 
      ir    = 1 ; 
    }
    if ( layer == "HighPt0" || layer == "HighPt1" ) {
      iName = 9 ; // or 10 ; 
      ir    = 1 ; // doubletR=2 -> upgrade of Atlas 
    }
  } // end sectors 12 and 14
  
  int NTotStripsSideA = 1;
  int NTotStripsSideC = 1;

  
  // MuonDetectorManager from the conditions store
  SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey};
  const MuonGM::MuonDetectorManager* MuonDetMgr = DetectorManagerHandle.cptr(); 
  if(!MuonDetMgr){
    ATH_MSG_ERROR("nullptr to the read MuonDetectorManager conditions object");
    return; 
  } 
 
  int kName = iName ;
  if(kName==1)kName=53;//BMLE

  Identifier rpcId = m_idHelperSvc->rpcIdHelper().channelID(kName, 1 , istatPhi+1, ir, 1, idblPhi+1, 1, 1, 1); // last 3 arguments are: int doubletPhi, int gasGap, int measuresPhi, int strip
  if (!rpcId.is_valid()) {
    ATH_MSG_WARNING("Could not get valid Identifier for stationName="<<kName<<", eta=1, phi="<<istatPhi+1<<", doubletR="<<ir<<", doubletZ=1, doubletPhi="<<idblPhi+1);
  }
  Identifier rpcId_c = m_idHelperSvc->rpcIdHelper().channelID(kName, -1 , istatPhi+1, ir, 1, idblPhi+1, 1, 1, 1); // last 3 arguments are: int doubletPhi, int gasGap, int measuresPhi, int strip
  if (!rpcId_c.is_valid()) {
    ATH_MSG_WARNING("Could not get valid Identifier for stationName="<<kName<<", eta=-1, phi="<<istatPhi+1<<", doubletR="<<ir<<", doubletZ=1, doubletPhi="<<idblPhi+1);
  }
  const MuonGM::RpcReadoutElement* rpc   = MuonDetMgr->getRpcReadoutElement(rpcId);   
  const MuonGM::RpcReadoutElement* rpc_c = MuonDetMgr->getRpcReadoutElement(rpcId_c);  
  
  if(rpc){  
    Identifier idr = rpc->identify();
    std::vector<int>   rpcstripshift = RpcGM::RpcStripShift(MuonDetMgr,m_idHelperSvc->rpcIdHelper(), idr, 0)  ;
    NTotStripsSideA = rpcstripshift[6]+rpcstripshift[17];
    Identifier idr_c = rpc_c->identify();
    std::vector<int>   rpcstripshift_c = RpcGM::RpcStripShift(MuonDetMgr,m_idHelperSvc->rpcIdHelper(), idr_c, 0)  ;
    NTotStripsSideC = rpcstripshift_c[7]+rpcstripshift_c[18];
   
  } 
   
  TH1 *rpcCoolHisto = new TH1F(histName.c_str(), histName.c_str(), NTotStripsSideC+NTotStripsSideA, -NTotStripsSideC, NTotStripsSideA );

  sc=rpcCoolDb.regHist(rpcCoolHisto) ;
  if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register " << histName << "hist to MonGroup" );
  rpcCoolHisto->GetXaxis()->SetTitle("strip");  
  
 
  
}
  
  
void RPCStandaloneTracksMon::bookRPCCoolHistograms( std::vector<std::string>::const_iterator & iter, int isec, int idblPhi, std::string layer ) 
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
  std::string generic_path_rpcmonitoring = "Muon/MuonRawDataMonitoring/RPCStandAloneTrackMon";
  MonGroup rpcCoolDb( this, generic_path_rpcmonitoring+"/CoolDB", run, ATTRIB_UNMANAGED );
    
  char histName_char[100];
  sprintf(histName_char,"Sector%.2d_%s_dblPhi%d", isec+1, layer.c_str(), idblPhi+1) ;
  // example: Sector01_Pivot0_dblPhi1_StripId
  
  std::string histName  = histName_char  ;
  histName += "_"            ;
  histName += *iter        ;  //histName += m_coolQuantity ;
  int istatPhi  = int( isec/2) ;
  int iName     = 0              ;
  int ir = 0;
  
  //BML7(dr=1) is associated to LowPt and not Pivot
  if ( isec<11 ||  isec>13) {
    // if ( layer.find("Pivot",0) )
    if ( layer == "Pivot0" || layer == "Pivot1" )   {
      iName = 2 + (isec%2 ) ;
      ir    = 2 	      ;	 
    }
    if ( layer == "LowPt0" || layer == "LowPt1" )   {
      iName = 2 + (isec%2 ) ;
      ir    = 1 	      ; 
    }
    if ( layer == "HighPt0" || layer == "HighPt1" ) {
      iName = 4 + (isec%2 ) ;
      ir    = 1 	      ; 
    } 
  }  
  else if ( isec==12) {
    // if ( layer.find("Pivot",0) )
    if ( layer == "Pivot0" || layer == "Pivot1" )   {
      iName     =  1      ; 
      ir    = 2 	      ;	 
    }
    if ( layer == "LowPt0" || layer == "LowPt1" )   {
      iName     =  1      ; 
      ir    = 1 	      ; 
    }
    if ( layer == "HighPt0" || layer == "HighPt1" ) {
      iName = 4               ;
      ir    = 1 	      ;  
    }
     
  }
  else {
    if ( layer == "Pivot0" || layer == "Pivot1" )   {
      iName     =   8  ; 
      ir    = 2 ;      
    }
    if ( layer == "LowPt0" || layer == "LowPt1" )   {
      iName = 8 ; 
      ir    = 1 ; 
    }
    if ( layer == "HighPt0" || layer == "HighPt1" ) {
      iName = 9 ; // or 10 ; 
      ir    = 1 ; // doubletR=2 -> upgrade of Atlas 
    }
  } // end sectors 12 and 14
  
  int NTotStripsSideA = 1;
  int NTotStripsSideC = 1;     

  // MuonDetectorManager from the conditions store
  SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey};
  const MuonGM::MuonDetectorManager* MuonDetMgr = DetectorManagerHandle.cptr(); 
  if(!MuonDetMgr){
    ATH_MSG_ERROR("nullptr to the read MuonDetectorManager conditions object");
    return; 
  } 


  int kName = iName ;
  if(kName==1)kName=53;//BMLE

  Identifier rpcId = m_idHelperSvc->rpcIdHelper().channelID(kName, 1 , istatPhi+1, ir, 1, idblPhi+1, 1, 1, 1); // last 3 arguments are: int doubletPhi, int gasGap, int measuresPhi, int strip
  if (!rpcId.is_valid()) {
    ATH_MSG_WARNING("Could not get valid Identifier for stationName="<<kName<<", eta=1, phi="<<istatPhi+1<<", doubletR="<<ir<<", doubletZ=1, doubletPhi="<<idblPhi+1);
  }
  Identifier rpcId_c = m_idHelperSvc->rpcIdHelper().channelID(kName, -1 , istatPhi+1, ir, 1, idblPhi+1, 1, 1, 1); // last 3 arguments are: int doubletPhi, int gasGap, int measuresPhi, int strip
  if (!rpcId_c.is_valid()) {
    ATH_MSG_WARNING("Could not get valid Identifier for stationName="<<kName<<", eta=-1, phi="<<istatPhi+1<<", doubletR="<<ir<<", doubletZ=1, doubletPhi="<<idblPhi+1);
  }
  const MuonGM::RpcReadoutElement* rpc   = MuonDetMgr->getRpcReadoutElement(rpcId);
  const MuonGM::RpcReadoutElement* rpc_c = MuonDetMgr->getRpcReadoutElement(rpcId_c);  
  
  if(rpc){  
    Identifier idr = rpc->identify();
    std::vector<int>   rpcstripshift = RpcGM::RpcStripShift(MuonDetMgr,m_idHelperSvc->rpcIdHelper(), idr, 0)  ;
    NTotStripsSideA = rpcstripshift[6]+rpcstripshift[17];
    Identifier idr_c = rpc_c->identify();
    std::vector<int>   rpcstripshift_c = RpcGM::RpcStripShift(MuonDetMgr,m_idHelperSvc->rpcIdHelper(), idr_c, 0)  ;
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
  
void RPCStandaloneTracksMon::bookRPCTracksLayerHistograms(std::string hardware_name, std::string layer_name, std::string layer0_name, int bin, int binmin, int binmax )
{
  gErrorIgnoreLevel=kError;
  StatusCode sc = StatusCode::SUCCESS;
  if(sc.isFailure() )  
  {  
    return ;
  }
  if( m_doRpcESD==true ) {if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online ) {    		    
      //declare a group of histograms
      std::string generic_path_rpcmonitoring = "Muon/MuonRawDataMonitoring/RPCStandAloneTrackMon";
      MonGroup rpcprd_expert( this, generic_path_rpcmonitoring+"/Chambers/"+hardware_name+"/Tracks/", run, ATTRIB_UNMANAGED );   
      MuonDQAHistList& lst = m_stationHists.getList( hardware_name +"/Tracks/" + layer_name);    
  
  
      //histo path for rpc trackproj
      //
      std::string generic_path_rpctrackprojlayer = generic_path_rpcmonitoring+"/Chambers/"+hardware_name+"/Projection/";
      generic_path_rpctrackprojlayer += hardware_name + "_" + layer_name + "_trackproj";
      std::string rpctrackprojlayer_title = hardware_name + "_" + layer_name + "_trackproj";	
      const char* rpctrackprojlayer_title_char = rpctrackprojlayer_title.c_str();
  
      TH1 *rpctrackprojlayer=new TH1I(rpctrackprojlayer_title_char, rpctrackprojlayer_title_char, bin/m_rpcreducenbins, binmin, binmax ); 			
      lst.addHist(rpctrackprojlayer);  
      rpctrackprojlayer->SetFillColor(42);  
      //rpctrackprojlayer->SetOption("COLZ");  
      rpctrackprojlayer->GetXaxis()->SetTitle(layer0_name.c_str());
      rpctrackprojlayer->GetYaxis()->SetTitle("trackproj ");

      {
	ATH_MSG_DEBUG ( "INSIDE bookRPCLayerHistograms : " << rpctrackprojlayer << generic_path_rpctrackprojlayer.c_str() );
	//ATH_MSG_DEBUG ( "EXPERT : " << expert );
	ATH_MSG_DEBUG ( "RUN : " << run );}    
   		     	
      sc = rpcprd_expert.regHist( rpctrackprojlayer );
      if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpctrackprojlayer hist to MonGroup" ); 
        
      //histo path for rpc hitontrack
      //
      std::string generic_path_rpchitontracklayer = generic_path_rpcmonitoring+"/Chambers/"+hardware_name+"/Projection/";
      generic_path_rpchitontracklayer += hardware_name + "_" + layer_name + "_hitontrack";
      std::string rpchitontracklayer_title = hardware_name + "_" + layer_name + "_hitontrack";	
      const char* rpchitontracklayer_title_char = rpchitontracklayer_title.c_str();
		
      TH1 *rpchitontracklayer=new TH1I(rpchitontracklayer_title_char, rpchitontracklayer_title_char, bin/m_rpcreducenbins, binmin, binmax ); 			
      lst.addHist(rpchitontracklayer);  
      rpchitontracklayer->SetFillColor(42);  
      //rpchitontracklayer->SetOption("COLZ");  
      rpchitontracklayer->GetXaxis()->SetTitle(layer0_name.c_str());
      rpchitontracklayer->GetYaxis()->SetTitle("hitontrack ");

      {
	ATH_MSG_DEBUG ( "INSIDE bookRPCLayerHistograms : " << rpchitontracklayer << generic_path_rpchitontracklayer.c_str() );
	//ATH_MSG_DEBUG ( "EXPERT : " << expert );
	ATH_MSG_DEBUG ( "RUN : " << run );}    
   		     	
      sc = rpcprd_expert.regHist( rpchitontracklayer );
      if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpchitontracklayer hist to MonGroup" ); 
   
  
      //histo path for rpc residuals
      //
      std::string generic_path_rpcresidualslayer = generic_path_rpcmonitoring+"/Chambers/"+hardware_name+"/Tracks/";
      generic_path_rpcresidualslayer += hardware_name + "_" + layer_name + "_residuals";
      std::string rpcresidualslayer_title = hardware_name + "_" + layer_name + "_residuals";	
      const char* rpcresidualslayer_title_char = rpcresidualslayer_title.c_str();
		
      TH1 *rpcresidualslayer=new TH1F(rpcresidualslayer_title_char, rpcresidualslayer_title_char, 1000, -500, 500); 			
      lst.addHist(rpcresidualslayer);  
      rpcresidualslayer->SetFillColor(42);   
      rpcresidualslayer->GetXaxis()->SetTitle(layer0_name.c_str());
      rpcresidualslayer->GetYaxis()->SetTitle("Counts/Strip");

      {
	ATH_MSG_DEBUG ( "INSIDE bookRPCLayerHistograms : " << rpcresidualslayer << generic_path_rpcresidualslayer.c_str() );
	//ATH_MSG_DEBUG ( "EXPERT : " << expert );
	ATH_MSG_DEBUG ( "RUN : " << run );}    
   		     	
      sc = rpcprd_expert.regHist( rpcresidualslayer );
      if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register rpcresidualslayer hist to MonGroup" );  
         
	 
    
    }}//m_doRpcESD // AthenaMonManager::tier0 || AthenaMonManager::tier0ESD  
  
}


//void RPCStandaloneTracksMon::bookRPCSummaryHistograms( int i_sec, std::vector<std::string>::const_iterator & iter )
void RPCStandaloneTracksMon::bookRPCSummaryHistograms( int i_sec, const std::string & quantity)
{
  StatusCode sc = StatusCode::SUCCESS; 
  
  ATH_MSG_DEBUG ( "Start bookRPCSummaryHistograms: " << quantity ); 
  
  std::string generic_path_rpcmonitoring = "Muon/MuonRawDataMonitoring/RPCStandAloneTrackMon";
  MonGroup rpcprd_expert_sum( this, generic_path_rpcmonitoring +"/Summary", run, ATTRIB_UNMANAGED ) ;
   
  char sector_char[20]    ;
  std::string sector_name ;
  sprintf(sector_char, "Sector%.2d", i_sec+1) ;  // sector number with 2 digits
  sector_name = sector_char ;
	
  // large sectors: (i_sec+1)%2 = 1;  small (i_sec+1)%2 = 0
  int n_eta_station = ( (i_sec+1) %2 ) + 6 + 1 ;  // small: 1-6, large: 1-7, sec 12+14: 0-6, sec 13: 1-7 (8->7)	  
  int n_bin = 72 * n_eta_station   ;   
  //  72 = 2*2*2*3*3  measPhi + gap + dblPhi + dblZ + stationName + stationEta
  std::string PanelIndexTitle = "# Panel = View + (gap-1)*2 + (dblPhi-1)*4 + (dblZ-1)*8 + (LowPt=0,Pivot=1,HighPt=2)*24 + (Eta)*72";
 
  std::string SummaryHist_title = "Summary" + quantity + "_" + sector_name ;
  const char* SummaryHist_title_char = SummaryHist_title.c_str(); 
  m_SummaryHist.push_back (new TH1F(SummaryHist_title_char, SummaryHist_title_char, 2*n_bin, -n_bin, n_bin));
  m_SummaryHist.back()->SetFillColor(2);
  m_SummaryHist.back()->GetXaxis()-> SetTitle(PanelIndexTitle.c_str() );
  m_SummaryHist.back()->GetXaxis()-> SetTitleSize(0.03)  	      ;
  m_SummaryHist.back()->GetXaxis()-> SetTitleOffset(1.2) 	      ;
  m_SummaryHist.back()->GetYaxis()-> SetTitle((quantity).c_str() )   ;
   
  sc = rpcprd_expert_sum.regHist( m_SummaryHist.back() ); 
  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register "<< SummaryHist_title <<" hist to MonGroup" );
  
 
  //
  
  MonGroup rpcprd_dq_BA      ( this, generic_path_rpcmonitoring + "/RPCBA" , run, ATTRIB_UNMANAGED );
  MonGroup rpcprd_dq_BC      ( this, generic_path_rpcmonitoring + "/RPCBC" , run, ATTRIB_UNMANAGED ); 
     
 
  if(quantity == "Time_NotNorm"){
  
    std::string SummaryHistRPCBA_title = "StripTimeDistribution_" + sector_name +"_SideA" ;
    const char* SummaryHistRPCBA_title_char = SummaryHistRPCBA_title.c_str();
  
    m_SummaryHistRPCBA.push_back( new TH1F(SummaryHistRPCBA_title_char, SummaryHistRPCBA_title_char, timeNbin, timeminrange, timemaxrange));
    m_SummaryHistRPCBA.back()->SetFillColor(2);
    m_SummaryHistRPCBA.back()->GetXaxis()-> SetTitle(PanelIndexTitle.c_str() );
    m_SummaryHistRPCBA.back()->GetXaxis()-> SetTitleSize(0.03)	        ;
    m_SummaryHistRPCBA.back()->GetXaxis()-> SetTitleOffset(1.2)	        ;
    m_SummaryHistRPCBA.back()->GetYaxis()-> SetTitle((quantity).c_str() )   ;
  
    sc = rpcprd_dq_BA.regHist( m_SummaryHistRPCBA.back() );
    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register "<< SummaryHistRPCBA_title <<" hist to MonGroup" ); 
   
    std::string SummaryHistRPCBC_title = "StripTimeDistribution_" + sector_name +"_SideC"  ;
    const char* SummaryHistRPCBC_title_char = SummaryHistRPCBC_title.c_str();
  
    m_SummaryHistRPCBC.push_back( new TH1F(SummaryHistRPCBC_title_char, SummaryHistRPCBC_title_char, timeNbin, timeminrange, timemaxrange));
    m_SummaryHistRPCBC.back()->SetFillColor(2);
    m_SummaryHistRPCBC.back()->GetXaxis()-> SetTitle(PanelIndexTitle.c_str() );
    m_SummaryHistRPCBC.back()->GetXaxis()-> SetTitleSize(0.03)	        ;
    m_SummaryHistRPCBC.back()->GetXaxis()-> SetTitleOffset(1.2)	        ;
    m_SummaryHistRPCBC.back()->GetYaxis()-> SetTitle((quantity).c_str() )   ;
  
    sc = rpcprd_dq_BC.regHist( m_SummaryHistRPCBC.back() );
    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register "<< SummaryHistRPCBC_title <<" hist to MonGroup" ); 
  
  }
     
 
  if(i_sec == 0 ){  
    std::string SummaryHistRPCBA_title = "Layer_" + quantity+"_sideA";
    const char* SummaryHistRPCBA_title_char = SummaryHistRPCBA_title.c_str();
  
    m_SummaryHistRPCBA.push_back(  new TH1F(SummaryHistRPCBA_title_char, SummaryHistRPCBA_title_char,24,0,24) );
    m_SummaryHistRPCBA.back()->SetFillColor(42);
    m_SummaryHistRPCBA.back()->GetXaxis()-> SetTitle( "LayerType(LPt0,LPt1,Piv0,Piv1,HPt0,HPt1) + ( Eta or Phi)*6 + (Small or Large)*12" );
    m_SummaryHistRPCBA.back()->GetXaxis()-> SetTitleSize(0.03)	         ;
    m_SummaryHistRPCBA.back()->GetXaxis()-> SetTitleOffset(1.2)	         ;
    m_SummaryHistRPCBA.back()->GetYaxis()-> SetTitle(("RPCBA_Layer_"+(quantity)).c_str() )   ;    
    m_SummaryHistRPCBA.back()->SetMarkerStyle(20)	  ;
    m_SummaryHistRPCBA.back()->SetMarkerColor(2)  ;
 
    sc = rpcprd_dq_BA.regHist( m_SummaryHistRPCBA.back() );
    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register "<< SummaryHistRPCBA_title <<" hist to MonGroup" ); 
     
    std::string SummaryHistRPCBC_title = "Layer_" + quantity+"_sideC";
    const char* SummaryHistRPCBC_title_char = SummaryHistRPCBC_title.c_str();
  
    m_SummaryHistRPCBC.push_back(  new TH1F(SummaryHistRPCBC_title_char, SummaryHistRPCBC_title_char,24,0,24) );
    m_SummaryHistRPCBC.back()->SetFillColor(42);
    m_SummaryHistRPCBC.back()->GetXaxis()-> SetTitle( "LayerType(LPt0,LPt1,Piv0,Piv1,HPt0,HPt1) + ( Eta or Phi)*6 + (Small or Large)*12" );
    m_SummaryHistRPCBC.back()->GetXaxis()-> SetTitleSize(0.03)	         ;
    m_SummaryHistRPCBC.back()->GetXaxis()-> SetTitleOffset(1.2)	         ;
    m_SummaryHistRPCBC.back()->GetYaxis()-> SetTitle(("RPCBC_Layer_"+(quantity)).c_str() )   ;       
    m_SummaryHistRPCBC.back()-> SetMarkerStyle(20)	   ;
    m_SummaryHistRPCBC.back()-> SetMarkerColor(2)  ;
  
    sc = rpcprd_dq_BC.regHist( m_SummaryHistRPCBC.back() );
    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register "<< SummaryHistRPCBC_title <<" hist to MonGroup" ); 
      
 
  } 
  
}

void RPCStandaloneTracksMon::bookRPCSummaryFinalHistograms( int i_sec, const std::string & quantity)
{
  StatusCode sc = StatusCode::SUCCESS; 
  std::string generic_path_rpcmonitoring = "Muon/MuonRawDataMonitoring/RPCStandAloneTrackMon";
  MonGroup rpcprd_expert_sum( this, generic_path_rpcmonitoring +"/Summary", run,  ATTRIB_UNMANAGED) ;
  
  ATH_MSG_DEBUG ( "Start bookRPCSummaryFinalHistograms: " << i_sec << " " << quantity ); 
  
  char sector_char[20]    ;
  std::string sector_name ;
  sprintf(sector_char, "Sector%.2d", i_sec+1) ;  // sector number with 2 digits
  sector_name = sector_char ;
	
  // large sectors: (i_sec+1)%2 = 1;  small (i_sec+1)%2 = 0
  int n_eta_station = ( (i_sec+1) %2 ) + 6 + 1 ;  // small: 1-6, large: 1-7, sec 12+14: 0-6, sec 13: 1-7 (8->7)	  
  int n_bin = 72 * n_eta_station   ;   
  //  72 = 2*2*2*3*3  measPhi + gap + dblPhi + dblZ + stationName + stationEta
  std::string PanelIndexTitle = "# Panel = View + (gap-1)*2 + (dblPhi-1)*4 + (dblZ-1)*8 + (LowPt=0,Pivot=1,HighPt=2)*24 + (Eta)*72";
  
   
  std::string SummaryHist_title = "Summary" + quantity + "_" + sector_name ;
  const char* SummaryHist_title_char = SummaryHist_title.c_str();
  bool doRecursiveReferenceDelete = gROOT->MustClean();
  gROOT->SetMustClean(false);
  m_sumTmpHist.push_back ( new TH1F(SummaryHist_title_char, SummaryHist_title_char, 2*n_bin, -n_bin, n_bin) );

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
  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register "<< SummaryHist_title <<" hist to MonGroup" );
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode RPCStandaloneTracksMon::procHistograms( )
{
  StatusCode sc = StatusCode::SUCCESS;
  
  ATH_MSG_DEBUG ( "********Reached Last Event in RPCStandaloneTracksMon !!!" );	  
  ATH_MSG_DEBUG ( "RPCStandaloneTracksMon finalize()" ); 	  
  
  if( m_doRpcESD==true ) {if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online ) {    
      std::string generic_path_rpcmonitoring = "Muon/MuonRawDataMonitoring/RPCStandAloneTrackMon" ;	
      MonGroup rpcprd_expert     ( this, generic_path_rpcmonitoring+"/Overview"  ,  run, ATTRIB_UNMANAGED );
      MonGroup rpcprd_shift      ( this, generic_path_rpcmonitoring+"/Overview"  ,  run, ATTRIB_UNMANAGED );
      MonGroup rpc_dqmf_global   ( this, generic_path_rpcmonitoring+"/GLOBAL"    ,  run, ATTRIB_UNMANAGED );
      MonGroup rpcprd_expert_sum ( this, generic_path_rpcmonitoring + "/Summary" ,  run, ATTRIB_UNMANAGED );
      MonGroup rpcprd_dq_BA      ( this, generic_path_rpcmonitoring + "/RPCBA"   ,  run, ATTRIB_UNMANAGED );
      MonGroup rpcprd_dq_BC      ( this, generic_path_rpcmonitoring + "/RPCBC"   ,  run, ATTRIB_UNMANAGED );
    
      if(endOfRunFlag()){        

        TH1* RPCBA_layerTrackProj = nullptr;
	sc = rpcprd_dq_BA.getHist(RPCBA_layerTrackProj,"Layer_TrackProj_sideA");	      
	if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get Layer_TrackProj_sideA  hist" );  

        TH1* RPCBC_layerTrackProj = nullptr;
	sc = rpcprd_dq_BC.getHist(RPCBC_layerTrackProj,"Layer_TrackProj_sideC");	      
	if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get Layer_TrackProj_sideC  hist" );  
         
        TH1* RPCBA_layerHitOnTrack = nullptr;
	sc = rpcprd_dq_BA.getHist(RPCBA_layerHitOnTrack,"Layer_HitOnTrack_sideA");	   
	if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get Layer_HitOnTrack_sideA  hist" );  
     	  
        TH1* RPCBC_layerHitOnTrack = nullptr;
	sc = rpcprd_dq_BC.getHist(RPCBC_layerHitOnTrack,"Layer_HitOnTrack_sideC");	   
	if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get Layer_HitOnTrack_sideC  hist" );  
	 	         
        TH1* RPCBA_layerEfficiency = nullptr;
	sc = rpcprd_dq_BA.getHist(RPCBA_layerEfficiency,"Layer_Efficiency_sideA");	   
	if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get Layer_Efficiency_sideA  hist" );  
     	  
        TH1* RPCBC_layerEfficiency = nullptr;
	sc = rpcprd_dq_BC.getHist(RPCBC_layerEfficiency,"Layer_Efficiency_sideC");	   
	if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get Layer_Efficiency_sideC  hist" );
      
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	/**Writes hits per harware chamber in a txt file*/
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *   
	std::ofstream myfile;
	if(m_rpcfile){
	  myfile.open ("rpccosmic.txt",std::ios::out); 
	  myfile << "-------- Counts per Chamber Statistics--------\n";} //only if m_rpcfile==true

    
	if(m_rpcfile){
	  myfile << "----Total events / Events in selected area----\n";
	  // myfile << m_rpc_eventstotal << "     /     " << rpc_event_inarea << "\n";
	  myfile.close();}  //only if m_rpcfile==true
      
	float RPCLyHitOnTr	      = 0 ;
	float RPCLyTrkPrj	      = 0 ;
	float RPCLyEff 		      = 0 ;
	float RPCLyEff_err	      = 0 ;
	float PanelVal 		      = 0 ; 
	float PanelVal_err	      = 0 ;
	float PanelVal_entries 	      = 0 ; 
	float PanelVal_square  	      = 0 ; 
	float PanelHitOnTrack  	      = 0 ;
	float PanelTrackProj	      = 0 ;
	  

	for (int ibin=0; ibin!=24; ibin++) {
	  RPCLyTrkPrj = m_LayerTrackProjection ->GetBinContent ( ibin + 1 ) ;
	  if ( RPCLyTrkPrj>0 ) {
	    RPCLyHitOnTr = m_LayerHitOnTrack ->GetBinContent ( ibin + 1 ) ;
	    RPCLyEff     = RPCLyHitOnTr / RPCLyTrkPrj ;
	    RPCLyEff_err = std::sqrt( std::abs( RPCLyHitOnTr-0.5*0) / RPCLyTrkPrj ) *
	      std::sqrt( 1. - std::abs( RPCLyHitOnTr-0.5*0) / RPCLyTrkPrj ) /
	      std::sqrt( RPCLyTrkPrj ) ;
	  
	    m_LayerEff->SetBinContent ( ibin + 1 , RPCLyEff     ) ;
	    m_LayerEff->SetBinError   ( ibin + 1 , RPCLyEff_err ) ;
	  }	
	  RPCLyTrkPrj = RPCBA_layerTrackProj ->GetBinContent ( ibin + 1 ) ;
	  if ( RPCLyTrkPrj>0 ) {
	    RPCLyHitOnTr = RPCBA_layerHitOnTrack ->GetBinContent ( ibin + 1 ) ;
	    RPCLyEff     = RPCLyHitOnTr / RPCLyTrkPrj ;
	    RPCLyEff_err = std::sqrt( std::abs( RPCLyHitOnTr-0.5*0) / RPCLyTrkPrj ) *
	      std::sqrt( 1. - std::abs( RPCLyHitOnTr-0.5*0) / RPCLyTrkPrj ) /
	      std::sqrt( RPCLyTrkPrj ) ;
	  
	    RPCBA_layerEfficiency->SetBinContent ( ibin + 1 , RPCLyEff     ) ;
	    RPCBA_layerEfficiency->SetBinError   ( ibin + 1 , RPCLyEff_err ) ;
	  }	
	  RPCLyTrkPrj = RPCBC_layerTrackProj ->GetBinContent ( ibin + 1 ) ;
	  if ( RPCLyTrkPrj>0 ) {
	    RPCLyHitOnTr = RPCBC_layerHitOnTrack ->GetBinContent ( ibin + 1 ) ;
	    RPCLyEff     = RPCLyHitOnTr / RPCLyTrkPrj ;
	    RPCLyEff_err = std::sqrt( std::abs( RPCLyHitOnTr-0.5*0) / RPCLyTrkPrj ) *
	      std::sqrt( 1. - std::abs( RPCLyHitOnTr-0.5*0) / RPCLyTrkPrj ) /
	      std::sqrt( RPCLyTrkPrj ) ;
	  
	    RPCBC_layerEfficiency->SetBinContent ( ibin + 1 , RPCLyEff     ) ;
	    RPCBC_layerEfficiency->SetBinError   ( ibin + 1 , RPCLyEff_err ) ;
	  }
	
	
	}
	
	//TriggerEfficiency
	int nb = m_hMEtracks->GetNbinsX() ;
	for (int ibin=0; ibin!=nb; ibin++) {
	  int TrkPrj = m_hMEtracks ->GetBinContent ( ibin + 1 ) ;
	  if ( TrkPrj>0 ) {
	    //hRPCPhiEtaCoinThr0
	      int   RPCOnTr    = m_hRPCPhiEtaCoinThr[0] ->GetBinContent ( ibin + 1 ) ;
	      float RPCEff     = RPCOnTr / TrkPrj ;
	      float RPCEff_err = std::sqrt( std::abs( RPCOnTr-0.5*0) / TrkPrj ) *
	      std::sqrt( 1. - std::abs( RPCOnTr-0.5*0) / TrkPrj ) /
	      std::sqrt( TrkPrj ) ;	  
	      m_hRPCPhiEtaCoinThr_eff[0]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      m_hRPCPhiEtaCoinThr_eff[0]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	      
	    //hRPCPhiEtaCoinThr1
	      RPCOnTr      = m_hRPCPhiEtaCoinThr[1] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff     = RPCOnTr / TrkPrj ;
	      RPCEff_err = std::sqrt( std::abs( RPCOnTr-0.5*0) / TrkPrj ) *
	      std::sqrt( 1. - std::abs( RPCOnTr-0.5*0) / TrkPrj ) /
	      std::sqrt( TrkPrj ) ;	  
	      m_hRPCPhiEtaCoinThr_eff[1]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      m_hRPCPhiEtaCoinThr_eff[1]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	      
	    //hRPCPhiEtaCoinThr2
	      RPCOnTr      = m_hRPCPhiEtaCoinThr[2] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff     = RPCOnTr / TrkPrj ;
	      RPCEff_err = std::sqrt( std::abs( RPCOnTr-0.5*0) / TrkPrj ) *
	      std::sqrt( 1. - std::abs( RPCOnTr-0.5*0) / TrkPrj ) /
	      std::sqrt( TrkPrj ) ;	  
	      m_hRPCPhiEtaCoinThr_eff[2]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      m_hRPCPhiEtaCoinThr_eff[2]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	      
	    //hRPCPhiEtaCoinThr3
	      RPCOnTr      = m_hRPCPhiEtaCoinThr[3] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff     = RPCOnTr / TrkPrj ;
	      RPCEff_err = std::sqrt( std::abs( RPCOnTr-0.5*0) / TrkPrj ) *
	      std::sqrt( 1. - std::abs( RPCOnTr-0.5*0) / TrkPrj ) /
	      std::sqrt( TrkPrj ) ;	  
	      m_hRPCPhiEtaCoinThr_eff[3]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      m_hRPCPhiEtaCoinThr_eff[3]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	      
	    //hRPCPhiEtaCoinThr4
	      RPCOnTr      = m_hRPCPhiEtaCoinThr[4] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff     = RPCOnTr / TrkPrj ;
	      RPCEff_err = std::sqrt( std::abs( RPCOnTr-0.5*0) / TrkPrj ) *
	      std::sqrt( 1. - std::abs( RPCOnTr-0.5*0) / TrkPrj ) /
	      std::sqrt( TrkPrj ) ;	  
	      m_hRPCPhiEtaCoinThr_eff[4]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      m_hRPCPhiEtaCoinThr_eff[4]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	      
	    //hRPCPhiEtaCoinThr5
	      RPCOnTr      = m_hRPCPhiEtaCoinThr[5] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff     = RPCOnTr / TrkPrj ;
	      RPCEff_err = std::sqrt( std::abs( RPCOnTr-0.5*0) / TrkPrj ) *
	      std::sqrt( 1. - std::abs( RPCOnTr-0.5*0) / TrkPrj ) /
	      std::sqrt( TrkPrj ) ;	  
	      m_hRPCPhiEtaCoinThr_eff[5]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      m_hRPCPhiEtaCoinThr_eff[5]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	    //hRPCPadThr0
	      RPCOnTr	 = m_hRPCPadThr[0] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff	 = RPCOnTr / TrkPrj ;
	      RPCEff_err = std::sqrt( std::abs( RPCOnTr-0.5*0) / TrkPrj ) *
	      std::sqrt( 1. - std::abs( RPCOnTr-0.5*0) / TrkPrj ) /
	      std::sqrt( TrkPrj ) ;	  
	      m_hRPCPadThr_eff[0]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      m_hRPCPadThr_eff[0]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	      
	    //hRPCPadThr1
	      RPCOnTr      = m_hRPCPadThr[1] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff     = RPCOnTr / TrkPrj ;
	      RPCEff_err = std::sqrt( std::abs( RPCOnTr-0.5*0) / TrkPrj ) *
	      std::sqrt( 1. - std::abs( RPCOnTr-0.5*0) / TrkPrj ) /
	      std::sqrt( TrkPrj ) ;	  
	      m_hRPCPadThr_eff[1]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      m_hRPCPadThr_eff[1]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	      
	    //hRPCPadThr2
	      RPCOnTr      = m_hRPCPadThr[2] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff     = RPCOnTr / TrkPrj ;
	      RPCEff_err = std::sqrt( std::abs( RPCOnTr-0.5*0) / TrkPrj ) *
	      std::sqrt( 1. - std::abs( RPCOnTr-0.5*0) / TrkPrj ) /
	      std::sqrt( TrkPrj ) ;	  
	      m_hRPCPadThr_eff[2]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      m_hRPCPadThr_eff[2]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	      
	    //hRPCPadThr3
	      RPCOnTr      = m_hRPCPadThr[3] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff     = RPCOnTr / TrkPrj ;
	      RPCEff_err = std::sqrt( std::abs( RPCOnTr-0.5*0) / TrkPrj ) *
	      std::sqrt( 1. - std::abs( RPCOnTr-0.5*0) / TrkPrj ) /
	      std::sqrt( TrkPrj ) ;	  
	      m_hRPCPadThr_eff[3]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      m_hRPCPadThr_eff[3]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	      
	    //hRPCPadThr4
	      RPCOnTr      = m_hRPCPadThr[4] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff     = RPCOnTr / TrkPrj ;
	      RPCEff_err = std::sqrt( std::abs( RPCOnTr-0.5*0) / TrkPrj ) *
	      std::sqrt( 1. - std::abs( RPCOnTr-0.5*0) / TrkPrj ) /
	      std::sqrt( TrkPrj ) ;	  
	      m_hRPCPadThr_eff[4]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      m_hRPCPadThr_eff[4]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	      
	    //hRPCPadThr5
	      RPCOnTr      = m_hRPCPadThr[5] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff     = RPCOnTr / TrkPrj ;
	      RPCEff_err = std::sqrt( std::abs( RPCOnTr-0.5*0) / TrkPrj ) *
	      std::sqrt( 1. - std::abs( RPCOnTr-0.5*0) / TrkPrj ) /
	      std::sqrt( TrkPrj ) ;	  
	      m_hRPCPadThr_eff[5]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      m_hRPCPadThr_eff[5]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	    //hRPCMuctpiThr0
	      RPCOnTr	 = m_hRPCMuctpiThr[0] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff	 = RPCOnTr / TrkPrj ;
	      RPCEff_err = std::sqrt( std::abs( RPCOnTr-0.5*0) / TrkPrj ) *
	      std::sqrt( 1. - std::abs( RPCOnTr-0.5*0) / TrkPrj ) /
	      std::sqrt( TrkPrj ) ;	  
	      m_hRPCMuctpiThr_eff[0]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      m_hRPCMuctpiThr_eff[0]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	      
	    //hRPCMuctpiThr1
	      RPCOnTr      = m_hRPCMuctpiThr[1] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff     = RPCOnTr / TrkPrj ;
	      RPCEff_err = std::sqrt( std::abs( RPCOnTr-0.5*0) / TrkPrj ) *
	      std::sqrt( 1. - std::abs( RPCOnTr-0.5*0) / TrkPrj ) /
	      std::sqrt( TrkPrj ) ;	  
	      m_hRPCMuctpiThr_eff[1]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      m_hRPCMuctpiThr_eff[1]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	      
	    //hRPCMuctpiThr2
	      RPCOnTr      = m_hRPCMuctpiThr[2] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff     = RPCOnTr / TrkPrj ;
	      RPCEff_err = std::sqrt( std::abs( RPCOnTr-0.5*0) / TrkPrj ) *
	      std::sqrt( 1. - std::abs( RPCOnTr-0.5*0) / TrkPrj ) /
	      std::sqrt( TrkPrj ) ;	  
	      m_hRPCMuctpiThr_eff[2]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      m_hRPCMuctpiThr_eff[2]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	      
	    //hRPCMuctpiThr3
	      RPCOnTr      = m_hRPCMuctpiThr[3] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff     = RPCOnTr / TrkPrj ;
	      RPCEff_err = std::sqrt( std::abs( RPCOnTr-0.5*0) / TrkPrj ) *
	      std::sqrt( 1. - std::abs( RPCOnTr-0.5*0) / TrkPrj ) /
	      std::sqrt( TrkPrj ) ;	  
	      m_hRPCMuctpiThr_eff[3]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      m_hRPCMuctpiThr_eff[3]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	      
	    //hRPCMuctpiThr4
	      RPCOnTr      = m_hRPCMuctpiThr[4] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff     = RPCOnTr / TrkPrj ;
	      RPCEff_err = std::sqrt( std::abs( RPCOnTr-0.5*0) / TrkPrj ) *
	      std::sqrt( 1. - std::abs( RPCOnTr-0.5*0) / TrkPrj ) /
	      std::sqrt( TrkPrj ) ;	  
	      m_hRPCMuctpiThr_eff[4]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      m_hRPCMuctpiThr_eff[4]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	      
	    //hRPCMuctpiThr5
	      RPCOnTr      = m_hRPCMuctpiThr[5] ->GetBinContent ( ibin + 1 ) ;
	      RPCEff     = RPCOnTr / TrkPrj ;
	      RPCEff_err = std::sqrt( std::abs( RPCOnTr-0.5*0) / TrkPrj ) *
	      std::sqrt( 1. - std::abs( RPCOnTr-0.5*0) / TrkPrj ) /
	      std::sqrt( TrkPrj ) ;	  
	      m_hRPCMuctpiThr_eff[5]->SetBinContent ( ibin + 1 , RPCEff     ) ;
	      m_hRPCMuctpiThr_eff[5]->SetBinError   ( ibin + 1 , RPCEff_err ) ;
	  }	
	
	}
	
	
	//TriggerEfficiency end

    
	std::vector<int>::const_iterator iter_bin=m_layer_name_bin_list_panel.begin() ;
	for (std::vector<std::string>::const_iterator iter=m_layer_name_list_panel.begin(); iter!=m_layer_name_list_panel.end(); iter++) {
	  int pos = *iter_bin;
	  iter_bin++ ;
	  if(pos>1000)continue; //skip fake trigger panels
      
	  std::string name = *iter          ;      
	  std::string list_name  = *iter    ;
	  std::string panel_name = *iter    ;
	  list_name.insert(7, "/Panels/")	  ; 
	  panel_name.insert(7, "_")         ;
          std::string sector_num  = name.substr(5, 2)   ;
          std::string sector_name = "Sector"+sector_num ;
	  m_SummaryHist_Idx = (atoi(sector_num.c_str())-1)*m_SummaryHist_Size/16;
            
      
	  //1) Efficiency
          TH1* SummaryHitOnTrack = nullptr;
          TH1* SummaryEfficiency = nullptr;
          TH1* SummaryEffDistriPerSector = nullptr;
	  sc = rpcprd_expert_sum.getHist ( SummaryHitOnTrack        ,"SummaryHitOnTrack_"         + sector_name ) ;
	  sc = rpcprd_expert_sum.getHist ( SummaryEfficiency        ,"SummaryEfficiency_"         + sector_name ) ;
	  sc = rpcprd_expert_sum.getHist ( SummaryEffDistriPerSector,"SummaryEffDistriPerSector_" + sector_name ) ;
	        
	  // fix 23 ott 2008
	  int shift_pos = m_SummaryHist[enumSumTrackProj+m_SummaryHist_Idx]-> GetNbinsX()/2 + 1;
	
	  PanelHitOnTrack = float ( SummaryHitOnTrack -> GetBinContent( pos + shift_pos ) ) ;
	  PanelTrackProj  = float ( m_SummaryHist[enumSumTrackProj+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;
	  PanelVal = 0 ;
	  if ( PanelTrackProj != 0 )  {
	    PanelVal     = PanelHitOnTrack/PanelTrackProj ;
	    PanelVal_err = std::sqrt( std::abs( PanelHitOnTrack-0.5*0) / PanelTrackProj ) *
	      std::sqrt( 1. - std::abs( PanelHitOnTrack-0.5*0) / PanelTrackProj ) /
	      std::sqrt( PanelTrackProj ) ;
	    SummaryEfficiency 	   -> SetBinContent ( pos + shift_pos , PanelVal     ) ;
	    SummaryEfficiency 	   -> SetBinError   ( pos + shift_pos , PanelVal_err ) ;
	    if ( pos>0 ) { m_rpcAverageSide_A[enumAvEff] -> Fill(PanelVal) ;  } 
	    else         { m_rpcAverageSide_C[enumAvEff] -> Fill(PanelVal) ;  }
	    SummaryEffDistriPerSector-> Fill(PanelVal) ; 
	  }
             
      
	  //2) GapEfficiency
          TH1* SummaryGapEffDistriPerSector = nullptr;
	  sc = rpcprd_expert_sum.getHist ( SummaryGapEffDistriPerSector    ,"SummaryGapEffDistriPerSector_"     + sector_name ) ;
	
	  PanelHitOnTrack                = float ( SummaryHitOnTrack -> GetBinContent( pos + shift_pos ) ) ;
	  PanelTrackProj                 = float ( m_SummaryHist[enumSumTrackProj+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;
	  PanelVal = 0 ;
	  if ( PanelTrackProj != 0 )  {
	    PanelVal     = PanelHitOnTrack/PanelTrackProj ;
	    PanelVal_err = std::sqrt( std::abs( PanelHitOnTrack-0.5*0) / PanelTrackProj ) *
	      std::sqrt( 1. - std::abs( PanelHitOnTrack-0.5*0) / PanelTrackProj ) /
	      std::sqrt( PanelTrackProj ) ;
	    m_SummaryHist[enumSumGapEfficiency+m_SummaryHist_Idx]-> SetBinContent ( pos + shift_pos , PanelVal	 ) ;
	    m_SummaryHist[enumSumGapEfficiency+m_SummaryHist_Idx]-> SetBinError	( pos + shift_pos , PanelVal_err ) ;	      
	    if ( pos>0 ) { m_rpcAverageSide_A[enumAvGapEff] -> Fill(PanelVal) ;  } 
	    else         { m_rpcAverageSide_C[enumAvGapEff] -> Fill(PanelVal) ;  }
	    SummaryGapEffDistriPerSector-> Fill(PanelVal) ; 
	  }


    
	  //3) NoiseCorr
          TH1* SummaryNoiseCorrDistriPerSector = nullptr;
	  sc = rpcprd_expert_sum.getHist ( SummaryNoiseCorrDistriPerSector,"SummaryNoiseCorrDistriPerSector_" + sector_name ) ;
	
	  PanelVal = float ( m_SummaryHist[enumSumNoiseCorr_NotNorm+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;
	  PanelVal_err = std::sqrt( PanelVal );
	  if( m_rpc_readout_window * m_rpc_eventstotal !=0 ) { 
	    PanelVal     = PanelVal     / m_rpc_readout_window  ;
	    PanelVal_err = PanelVal_err / m_rpc_readout_window  ; 
	
	    m_SummaryHist[enumSumNoiseCorr_NotNorm+m_SummaryHist_Idx]-> SetBinContent ( pos + shift_pos , PanelVal     ) ;
	    m_SummaryHist[enumSumNoiseCorr_NotNorm+m_SummaryHist_Idx]-> SetBinError   ( pos + shift_pos , PanelVal_err ) ;	  
	    if ( pos>0 ) { m_rpcAverageSide_A[enumAvNoiseCorr] -> Fill(PanelVal/m_rpc_eventstotal) ;  } 
	    else         { m_rpcAverageSide_C[enumAvNoiseCorr] -> Fill(PanelVal/m_rpc_eventstotal) ;  } 
	    SummaryNoiseCorrDistriPerSector-> Fill(PanelVal/ m_rpc_eventstotal) ; 
	  }
	
	  //4) NoiseTot
          TH1* SummaryNoiseTotDistriPerSector = nullptr;
	  sc = rpcprd_expert_sum.getHist ( SummaryNoiseTotDistriPerSector,"SummaryNoiseTotDistriPerSector_" + sector_name ) ;

	  PanelVal = float ( m_SummaryHist[enumSumNoiseTot_NotNorm+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;
	  PanelVal_err = std::sqrt( PanelVal );	

	  if( m_rpc_readout_window * m_rpc_eventstotal !=0 ) { 
	    PanelVal     = PanelVal     / m_rpc_readout_window ;
	    PanelVal_err = PanelVal_err / m_rpc_readout_window ;
	  
	    m_SummaryHist[enumSumNoiseTot_NotNorm+m_SummaryHist_Idx]-> SetBinContent ( pos + shift_pos , PanelVal     ) ;
	    m_SummaryHist[enumSumNoiseTot_NotNorm+m_SummaryHist_Idx]-> SetBinError   ( pos + shift_pos , PanelVal_err ) ;
	    if ( pos>0 ) { m_rpcAverageSide_A[enumAvNoiseTot] -> Fill(PanelVal/ m_rpc_eventstotal) ; } 
	    else         { m_rpcAverageSide_C[enumAvNoiseTot] -> Fill(PanelVal/ m_rpc_eventstotal) ; }
	    SummaryNoiseTotDistriPerSector-> Fill(PanelVal/ m_rpc_eventstotal) ; 
	  }
             
      
	  //5) AverageCS
          TH1* SummaryCSDistriPerSector = nullptr;
	  sc = rpcprd_expert_sum.getHist ( SummaryCSDistriPerSector ,"SummaryCSDistriPerSector_" + sector_name  );
	
	  PanelVal_entries = float ( m_SummaryHist[enumSumCS_entries+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;	
	  PanelVal         = float ( m_SummaryHist[enumSumCS_NotNorm+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;
	  PanelVal_square  = float ( m_SummaryHist[enumSumCS_square+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;	 
	  if(PanelVal_entries>0){
	    PanelVal=PanelVal/PanelVal_entries;
	    PanelVal_square=PanelVal_square/PanelVal_entries;	
	  }
	  PanelVal_err     = std::sqrt  ( std::abs(PanelVal_square - PanelVal*PanelVal) ) ;
	  // rpcAverageCS	 -> Fill(PanelVal) ; 
	  if ( pos>0 ) { m_rpcAverageSide_A[enumAvCS] -> Fill(PanelVal) ; } 
	  else         { m_rpcAverageSide_C[enumAvCS] -> Fill(PanelVal) ; }
	  SummaryCSDistriPerSector-> Fill(PanelVal) ; 
	
	
	  //6) Res_CS1
          TH1* SummaryRes_CS1DistriPerSector = nullptr;
          TH1* SummaryRes_CS1rmsDistriPerSector = nullptr;
	  sc = rpcprd_expert_sum.getHist ( SummaryRes_CS1DistriPerSector    ,"SummaryRes_CS1DistriPerSector_"    + sector_name ) ;
	  sc = rpcprd_expert_sum.getHist ( SummaryRes_CS1rmsDistriPerSector ,"SummaryRes_CS1rmsDistriPerSector_" + sector_name		  ) ;
     
	  PanelVal_entries = float ( m_SummaryHist[enumSumRes_CS1_entries+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;	
	  PanelVal         = float ( m_SummaryHist[enumSumRes_CS1_NotNorm+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;	
	  PanelVal_square  = float ( m_SummaryHist[enumSumRes_CS1_square+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;
	  if(PanelVal_entries>0){
	    PanelVal=PanelVal/PanelVal_entries;
	    PanelVal_square=PanelVal_square/PanelVal_entries;	
	  }
	  PanelVal_err     = std::sqrt  ( std::abs(PanelVal_square - PanelVal*PanelVal) ) ;
	  if ( pos>0 ) { m_rpcAverageSide_A[enumAvRes_CS1] -> Fill(PanelVal) ;  } 
	  else         { m_rpcAverageSide_C[enumAvRes_CS1] -> Fill(PanelVal) ;  } 
	  SummaryRes_CS1DistriPerSector    -> Fill(PanelVal) ;
	  if ( pos>0 ) { m_rpcAverageSide_A[enumAvRes_CS1rms] -> Fill(PanelVal_err) ;  } 
	  else         { m_rpcAverageSide_C[enumAvRes_CS1rms] -> Fill(PanelVal_err) ;  }  
	  SummaryRes_CS1rmsDistriPerSector -> Fill(PanelVal_err) ;
	
	  //7) Res_CS2
          TH1* SummaryRes_CS2DistriPerSector = nullptr;
          TH1* SummaryRes_CS2rmsDistriPerSector = nullptr;
	  sc = rpcprd_expert_sum.getHist ( SummaryRes_CS2DistriPerSector    ,"SummaryRes_CS2DistriPerSector_"    + sector_name ) ;
	  sc = rpcprd_expert_sum.getHist ( SummaryRes_CS2rmsDistriPerSector ,"SummaryRes_CS2rmsDistriPerSector_" + sector_name ) ;
	
	  PanelVal_entries = float ( m_SummaryHist[enumSumRes_CS2_entries+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;	
	  PanelVal         = float ( m_SummaryHist[enumSumRes_CS2_NotNorm+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;	
	  PanelVal_square  = float ( m_SummaryHist[enumSumRes_CS2_square+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;	 
	  if(PanelVal_entries>0){
	    PanelVal=PanelVal/PanelVal_entries;
	    PanelVal_square=PanelVal_square/PanelVal_entries;	
	  }
	  PanelVal_err     = std::sqrt  ( std::abs(PanelVal_square - PanelVal*PanelVal) ) ;
	  if ( pos>0 ) { m_rpcAverageSide_A[enumAvRes_CS2] -> Fill(PanelVal) ;  } 
	  else         { m_rpcAverageSide_C[enumAvRes_CS2] -> Fill(PanelVal) ;  }  
	  SummaryRes_CS2DistriPerSector   -> Fill(PanelVal) ;
	  if ( pos>0 ) { m_rpcAverageSide_A[enumAvRes_CS2rms] -> Fill(PanelVal_err) ;  } 
	  else         { m_rpcAverageSide_C[enumAvRes_CS2rms] -> Fill(PanelVal_err) ;  } 
	  SummaryRes_CS2rmsDistriPerSector-> Fill(PanelVal_err) ;
     
    
	  //8) Res_CSmore2
          TH1* SummaryRes_CSmore2DistriPerSector = nullptr;
          TH1* SummaryRes_CSmore2rmsDistriPerSector = nullptr;
	  sc = rpcprd_expert_sum.getHist ( SummaryRes_CSmore2DistriPerSector    ,"SummaryRes_CSmore2DistriPerSector_"    + sector_name ) ;
	  sc = rpcprd_expert_sum.getHist ( SummaryRes_CSmore2rmsDistriPerSector ,"SummaryRes_CSmore2rmsDistriPerSector_" + sector_name ) ;
	
	  PanelVal_entries = float ( m_SummaryHist[enumSumRes_CSmore2_entries+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;	
	  PanelVal         = float ( m_SummaryHist[enumSumRes_CSmore2_NotNorm+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;	
	  PanelVal_square  = float ( m_SummaryHist[enumSumRes_CSmore2_square+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;
	  if(PanelVal_entries>0){
	    PanelVal=PanelVal/PanelVal_entries;
	    PanelVal_square=PanelVal_square/PanelVal_entries;	
	  }
	  PanelVal_err     = std::sqrt  ( std::abs(PanelVal_square - PanelVal*PanelVal) ) ;
	  if ( pos>0 ) { m_rpcAverageSide_A[enumAvRes_CSmore2] -> Fill(PanelVal) ;  } 
	  else         { m_rpcAverageSide_C[enumAvRes_CSmore2] -> Fill(PanelVal) ;  }  
	  SummaryRes_CSmore2DistriPerSector   -> Fill(PanelVal) ;
	  if ( pos>0 ) { m_rpcAverageSide_A[enumAvRes_CSmore2rms] -> Fill(PanelVal_err) ;  } 
	  else         { m_rpcAverageSide_C[enumAvRes_CSmore2rms] -> Fill(PanelVal_err) ;  }  
	  SummaryRes_CSmore2rmsDistriPerSector-> Fill(PanelVal_err) ;
	    
	        
	  //9) Occupancy
          TH1* SummaryOccupancyDistriPerSector = nullptr;
	  sc = rpcprd_expert_sum.getHist ( SummaryOccupancyDistriPerSector ,"SummaryOccupancyDistriPerSector_" + sector_name	   );
	
	  PanelVal         = float ( m_SummaryHist[enumSumOccupancy_NotNorm+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ; 
	  PanelVal_err     = std::sqrt(PanelVal);
	
	  m_SummaryHist[enumSumOccupancy_NotNorm+m_SummaryHist_Idx]-> SetBinContent ( pos + shift_pos , PanelVal    ) ;
	  m_SummaryHist[enumSumOccupancy_NotNorm+m_SummaryHist_Idx]-> SetBinError   ( pos + shift_pos , PanelVal    ) ;
	
	  if(m_rpc_eventstotal>0) {
	    PanelVal = PanelVal / m_rpc_eventstotal ;
	    if(PanelVal>0) {
	      PanelVal     = std::log10(PanelVal     ) ;
	      PanelVal_err = std::log10(PanelVal_err ) ;
     
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

	  if ( pos>0 ) { m_rpcAverageSide_A[enumAvOccupancy] -> Fill(PanelVal) ;  } 
	  else         { m_rpcAverageSide_C[enumAvOccupancy] -> Fill(PanelVal) ;  }  
	  SummaryOccupancyDistriPerSector-> Fill(PanelVal) ;	    
	        
	  //10) Time
          TH1* SummaryTimeDistriPerSector = nullptr;
	  sc = rpcprd_expert_sum.getHist ( SummaryTimeDistriPerSector ,"SummaryTimeDistriPerSector_" + sector_name );
	
	  PanelVal_entries = float ( m_SummaryHist[enumSumCS_entries+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;	
	  PanelVal         = float ( m_SummaryHist[enumSumTime_NotNorm+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;	
	  PanelVal_square  = float ( m_SummaryHist[enumSumTime_square+m_SummaryHist_Idx]-> GetBinContent( pos + shift_pos ) ) ;	
	  if(PanelVal_entries>0){
	    PanelVal=PanelVal/PanelVal_entries;
	    PanelVal_square=PanelVal_square/PanelVal_entries;	
	  }
	  PanelVal_err     = std::sqrt  ( std::abs(PanelVal_square - PanelVal*PanelVal) ) ;
	  if ( pos>0 ) { m_rpcAverageSide_A[enumAvTime] -> Fill(PanelVal) ;  } 
	  else         { m_rpcAverageSide_C[enumAvTime] -> Fill(PanelVal) ;  }  
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
  m_rpcAverageSide_A.clear(); 
  m_rpcAverageSide_C.clear(); 
  
  return sc;
}
 
//======================================================================================//
/**  finalize */
//======================================================================================//
StatusCode RPCStandaloneTracksMon:: RPC_ROI_Mapping() 
{ 
 
  StatusCode sc = StatusCode::SUCCESS; 
  ATH_MSG_DEBUG ( "RPCStandaloneTracksMon::RPC_ROI_Mapping()" );
     m_EtaROImin[0][ 0][ 0]=  -0.238757;m_EtaROImax[0][ 0][ 0]=   -0.13107;m_PhiROImin[0][ 0][ 0]= 0.00296767;m_PhiROImax[0][ 0][ 0]=	0.113212;
     m_EtaROImin[0][ 0][ 1]=	    0;m_EtaROImax[0][ 0][ 1]=	     0;m_PhiROImin[0][ 0][ 1]=	      0;m_PhiROImax[0][ 0][ 1]=	       0;
     m_EtaROImin[0][ 0][ 2]=  -0.238757;m_EtaROImax[0][ 0][ 2]=   -0.13107;m_PhiROImin[0][ 0][ 2]=   0.116737;m_PhiROImax[0][ 0][ 2]=	0.224169;
     m_EtaROImin[0][ 0][ 3]=	    0;m_EtaROImax[0][ 0][ 3]=	     0;m_PhiROImin[0][ 0][ 3]=	      0;m_PhiROImax[0][ 0][ 3]=	       0;
     m_EtaROImin[0][ 0][ 4]=   -0.35049;m_EtaROImax[0][ 0][ 4]=  -0.246207;m_PhiROImin[0][ 0][ 4]= 0.00295181;m_PhiROImax[0][ 0][ 4]=	0.112612;
     m_EtaROImin[0][ 0][ 5]=  -0.452099;m_EtaROImax[0][ 0][ 5]=  -0.350831;m_PhiROImin[0][ 0][ 5]= 0.00295181;m_PhiROImax[0][ 0][ 5]=	0.112612;
     m_EtaROImin[0][ 0][ 6]=   -0.35049;m_EtaROImax[0][ 0][ 6]=  -0.246207;m_PhiROImin[0][ 0][ 6]=   0.116119;m_PhiROImax[0][ 0][ 6]=	0.223011;
     m_EtaROImin[0][ 0][ 7]=  -0.452099;m_EtaROImax[0][ 0][ 7]=  -0.350831;m_PhiROImin[0][ 0][ 7]=   0.116119;m_PhiROImax[0][ 0][ 7]=	0.223011;
     m_EtaROImin[0][ 0][ 8]=  -0.554444;m_EtaROImax[0][ 0][ 8]=  -0.458091;m_PhiROImin[0][ 0][ 8]= 0.00295181;m_PhiROImax[0][ 0][ 8]=	0.112612;
     m_EtaROImin[0][ 0][ 9]=  -0.648101;m_EtaROImax[0][ 0][ 9]=  -0.555789;m_PhiROImin[0][ 0][ 9]= 0.00295181;m_PhiROImax[0][ 0][ 9]=	0.112612;
     m_EtaROImin[0][ 0][10]=  -0.554444;m_EtaROImax[0][ 0][10]=  -0.458091;m_PhiROImin[0][ 0][10]=   0.116119;m_PhiROImax[0][ 0][10]=	0.223011;
     m_EtaROImin[0][ 0][11]=  -0.648101;m_EtaROImax[0][ 0][11]=  -0.555789;m_PhiROImin[0][ 0][11]=   0.116119;m_PhiROImax[0][ 0][11]=	0.223011;
     m_EtaROImin[0][ 0][12]=  -0.703298;m_EtaROImax[0][ 0][12]=  -0.655857;m_PhiROImin[0][ 0][12]= 0.00296767;m_PhiROImax[0][ 0][12]=	0.113212;
     m_EtaROImin[0][ 0][13]=  -0.776449;m_EtaROImax[0][ 0][13]=  -0.706413;m_PhiROImin[0][ 0][13]= 0.00296767;m_PhiROImax[0][ 0][13]=	0.113212;
     m_EtaROImin[0][ 0][14]=  -0.703298;m_EtaROImax[0][ 0][14]=  -0.655857;m_PhiROImin[0][ 0][14]=   0.116737;m_PhiROImax[0][ 0][14]=	0.224169;
     m_EtaROImin[0][ 0][15]=  -0.776449;m_EtaROImax[0][ 0][15]=  -0.706413;m_PhiROImin[0][ 0][15]=   0.116737;m_PhiROImax[0][ 0][15]=	0.224169;
     m_EtaROImin[0][ 0][16]=   -0.82768;m_EtaROImax[0][ 0][16]=  -0.783867;m_PhiROImin[0][ 0][16]= 0.00296767;m_PhiROImax[0][ 0][16]=	0.113212;
     m_EtaROImin[0][ 0][17]=  -0.895163;m_EtaROImax[0][ 0][17]=  -0.830556;m_PhiROImin[0][ 0][17]= 0.00296767;m_PhiROImax[0][ 0][17]=	0.113212;
     m_EtaROImin[0][ 0][18]=   -0.82768;m_EtaROImax[0][ 0][18]=  -0.783867;m_PhiROImin[0][ 0][18]=   0.116737;m_PhiROImax[0][ 0][18]=	0.224169;
     m_EtaROImin[0][ 0][19]=  -0.895163;m_EtaROImax[0][ 0][19]=  -0.830556;m_PhiROImin[0][ 0][19]=   0.116737;m_PhiROImax[0][ 0][19]=	0.224169;
     m_EtaROImin[0][ 0][20]=  -0.961344;m_EtaROImax[0][ 0][20]=  -0.898201;m_PhiROImin[0][ 0][20]= 0.00295181;m_PhiROImax[0][ 0][20]=	0.112612;
     m_EtaROImin[0][ 0][21]=   -1.02514;m_EtaROImax[0][ 0][21]=  -0.964674;m_PhiROImin[0][ 0][21]= 0.00295181;m_PhiROImax[0][ 0][21]=	0.112612;
     m_EtaROImin[0][ 0][22]=  -0.961344;m_EtaROImax[0][ 0][22]=  -0.898201;m_PhiROImin[0][ 0][22]=   0.116119;m_PhiROImax[0][ 0][22]=	0.223011;
     m_EtaROImin[0][ 0][23]=   -1.02514;m_EtaROImax[0][ 0][23]=  -0.964674;m_PhiROImin[0][ 0][23]=   0.116119;m_PhiROImax[0][ 0][23]=	0.223011;
     m_EtaROImin[0][ 0][24]=   -1.06547;m_EtaROImax[0][ 0][24]=   -1.03003;m_PhiROImin[0][ 0][24]= 0.00306876;m_PhiROImax[0][ 0][24]=	0.113429;
     m_EtaROImin[0][ 0][25]=	    0;m_EtaROImax[0][ 0][25]=	     0;m_PhiROImin[0][ 0][25]=	      0;m_PhiROImax[0][ 0][25]=	       0;
     m_EtaROImin[0][ 0][26]=   -1.06547;m_EtaROImax[0][ 0][26]=   -1.03003;m_PhiROImin[0][ 0][26]=   0.116958;m_PhiROImax[0][ 0][26]=	0.197143;
     m_EtaROImin[0][ 0][27]=	    0;m_EtaROImax[0][ 0][27]=	     0;m_PhiROImin[0][ 0][27]=	      0;m_PhiROImax[0][ 0][27]=	       0;
     m_EtaROImin[0][ 0][28]=	    0;m_EtaROImax[0][ 0][28]=	     0;m_PhiROImin[0][ 0][28]=	      0;m_PhiROImax[0][ 0][28]=	       0;
     m_EtaROImin[0][ 0][29]=	    0;m_EtaROImax[0][ 0][29]=	     0;m_PhiROImin[0][ 0][29]=	      0;m_PhiROImax[0][ 0][29]=	       0;
     m_EtaROImin[0][ 0][30]=	    0;m_EtaROImax[0][ 0][30]=	     0;m_PhiROImin[0][ 0][30]=	      0;m_PhiROImax[0][ 0][30]=	       0;
     m_EtaROImin[0][ 0][31]=	    0;m_EtaROImax[0][ 0][31]=	     0;m_PhiROImin[0][ 0][31]=	      0;m_PhiROImax[0][ 0][31]=	       0;
     m_EtaROImin[0][ 1][ 0]=  -0.213185;m_EtaROImax[0][ 1][ 0]=  -0.116816;m_PhiROImin[0][ 1][ 0]=   0.305953;m_PhiROImax[0][ 1][ 0]=	0.389909;
     m_EtaROImin[0][ 1][ 1]=  -0.118734;m_EtaROImax[0][ 1][ 1]= -0.0208251;m_PhiROImin[0][ 1][ 1]=   0.305953;m_PhiROImax[0][ 1][ 1]=	0.389909;
     m_EtaROImin[0][ 1][ 2]=  -0.213185;m_EtaROImax[0][ 1][ 2]=  -0.116816;m_PhiROImin[0][ 1][ 2]=   0.219649;m_PhiROImax[0][ 1][ 2]=	0.302322;
     m_EtaROImin[0][ 1][ 3]=  -0.118734;m_EtaROImax[0][ 1][ 3]= -0.0208251;m_PhiROImin[0][ 1][ 3]=   0.219649;m_PhiROImax[0][ 1][ 3]=	0.302322;
     m_EtaROImin[0][ 1][ 4]=  -0.416721;m_EtaROImax[0][ 1][ 4]=   -0.30075;m_PhiROImin[0][ 1][ 4]=   0.305953;m_PhiROImax[0][ 1][ 4]=	0.389909;
     m_EtaROImin[0][ 1][ 5]=  -0.302928;m_EtaROImax[0][ 1][ 5]=  -0.222449;m_PhiROImin[0][ 1][ 5]=   0.305953;m_PhiROImax[0][ 1][ 5]=	0.389909;
     m_EtaROImin[0][ 1][ 6]=  -0.416721;m_EtaROImax[0][ 1][ 6]=   -0.30075;m_PhiROImin[0][ 1][ 6]=   0.219649;m_PhiROImax[0][ 1][ 6]=	0.302322;
     m_EtaROImin[0][ 1][ 7]=  -0.302928;m_EtaROImax[0][ 1][ 7]=  -0.222449;m_PhiROImin[0][ 1][ 7]=   0.219649;m_PhiROImax[0][ 1][ 7]=	0.302322;
     m_EtaROImin[0][ 1][ 8]=  -0.573871;m_EtaROImax[0][ 1][ 8]=  -0.501681;m_PhiROImin[0][ 1][ 8]=   0.305953;m_PhiROImax[0][ 1][ 8]=	0.389909;
     m_EtaROImin[0][ 1][ 9]=  -0.504617;m_EtaROImax[0][ 1][ 9]=   -0.42967;m_PhiROImin[0][ 1][ 9]=   0.305953;m_PhiROImax[0][ 1][ 9]=	0.389909;
     m_EtaROImin[0][ 1][10]=  -0.573871;m_EtaROImax[0][ 1][10]=  -0.501681;m_PhiROImin[0][ 1][10]=   0.219649;m_PhiROImax[0][ 1][10]=	0.302322;
     m_EtaROImin[0][ 1][11]=  -0.504617;m_EtaROImax[0][ 1][11]=   -0.42967;m_PhiROImin[0][ 1][11]=   0.219649;m_PhiROImax[0][ 1][11]=	0.302322;
     m_EtaROImin[0][ 1][12]=  -0.741516;m_EtaROImax[0][ 1][12]=  -0.649933;m_PhiROImin[0][ 1][12]=   0.305953;m_PhiROImax[0][ 1][12]=	0.389909;
     m_EtaROImin[0][ 1][13]=  -0.653329;m_EtaROImax[0][ 1][13]=  -0.583785;m_PhiROImin[0][ 1][13]=   0.305953;m_PhiROImax[0][ 1][13]=	0.389909;
     m_EtaROImin[0][ 1][14]=  -0.741516;m_EtaROImax[0][ 1][14]=  -0.649933;m_PhiROImin[0][ 1][14]=   0.219649;m_PhiROImax[0][ 1][14]=	0.302322;
     m_EtaROImin[0][ 1][15]=  -0.653329;m_EtaROImax[0][ 1][15]=  -0.583785;m_PhiROImin[0][ 1][15]=   0.219649;m_PhiROImax[0][ 1][15]=	0.302322;
     m_EtaROImin[0][ 1][16]=	    0;m_EtaROImax[0][ 1][16]=	     0;m_PhiROImin[0][ 1][16]=	      0;m_PhiROImax[0][ 1][16]=	       0;
     m_EtaROImin[0][ 1][17]=  -0.837822;m_EtaROImax[0][ 1][17]=  -0.756521;m_PhiROImin[0][ 1][17]=   0.305953;m_PhiROImax[0][ 1][17]=	0.389909;
     m_EtaROImin[0][ 1][18]=	    0;m_EtaROImax[0][ 1][18]=	     0;m_PhiROImin[0][ 1][18]=	      0;m_PhiROImax[0][ 1][18]=	       0;
     m_EtaROImin[0][ 1][19]=  -0.837822;m_EtaROImax[0][ 1][19]=  -0.756521;m_PhiROImin[0][ 1][19]=   0.219649;m_PhiROImax[0][ 1][19]=	0.302322;
     m_EtaROImin[0][ 1][20]=  -0.956037;m_EtaROImax[0][ 1][20]=  -0.899344;m_PhiROImin[0][ 1][20]=   0.305953;m_PhiROImax[0][ 1][20]=	0.389909;
     m_EtaROImin[0][ 1][21]=  -0.903324;m_EtaROImax[0][ 1][21]=  -0.844116;m_PhiROImin[0][ 1][21]=   0.305953;m_PhiROImax[0][ 1][21]=	0.389909;
     m_EtaROImin[0][ 1][22]=  -0.956037;m_EtaROImax[0][ 1][22]=  -0.899344;m_PhiROImin[0][ 1][22]=   0.219649;m_PhiROImax[0][ 1][22]=	0.302322;
     m_EtaROImin[0][ 1][23]=  -0.903324;m_EtaROImax[0][ 1][23]=  -0.844116;m_PhiROImin[0][ 1][23]=   0.219649;m_PhiROImax[0][ 1][23]=	0.302322;
     m_EtaROImin[0][ 1][24]=	    0;m_EtaROImax[0][ 1][24]=	     0;m_PhiROImin[0][ 1][24]=	      0;m_PhiROImax[0][ 1][24]=	       0;
     m_EtaROImin[0][ 1][25]=	    0;m_EtaROImax[0][ 1][25]=	     0;m_PhiROImin[0][ 1][25]=	      0;m_PhiROImax[0][ 1][25]=	       0;
     m_EtaROImin[0][ 1][26]=	    0;m_EtaROImax[0][ 1][26]=	     0;m_PhiROImin[0][ 1][26]=	      0;m_PhiROImax[0][ 1][26]=	       0;
     m_EtaROImin[0][ 1][27]=	    0;m_EtaROImax[0][ 1][27]=	     0;m_PhiROImin[0][ 1][27]=	      0;m_PhiROImax[0][ 1][27]=	       0;
     m_EtaROImin[0][ 1][28]=	    0;m_EtaROImax[0][ 1][28]=	     0;m_PhiROImin[0][ 1][28]=	      0;m_PhiROImax[0][ 1][28]=	       0;
     m_EtaROImin[0][ 1][29]=	    0;m_EtaROImax[0][ 1][29]=	     0;m_PhiROImin[0][ 1][29]=	      0;m_PhiROImax[0][ 1][29]=	       0;
     m_EtaROImin[0][ 1][30]=	    0;m_EtaROImax[0][ 1][30]=	     0;m_PhiROImin[0][ 1][30]=	      0;m_PhiROImax[0][ 1][30]=	       0;
     m_EtaROImin[0][ 1][31]=	    0;m_EtaROImax[0][ 1][31]=	     0;m_PhiROImin[0][ 1][31]=	      0;m_PhiROImax[0][ 1][31]=	       0;
     m_EtaROImin[0][ 2][ 0]=  -0.118734;m_EtaROImax[0][ 2][ 0]= -0.0208251;m_PhiROImin[0][ 2][ 0]=   0.395489;m_PhiROImax[0][ 2][ 0]=	0.479445;
     m_EtaROImin[0][ 2][ 1]=  -0.213185;m_EtaROImax[0][ 2][ 1]=  -0.116816;m_PhiROImin[0][ 2][ 1]=   0.395489;m_PhiROImax[0][ 2][ 1]=	0.479445;
     m_EtaROImin[0][ 2][ 2]=  -0.118734;m_EtaROImax[0][ 2][ 2]= -0.0208251;m_PhiROImin[0][ 2][ 2]=   0.483076;m_PhiROImax[0][ 2][ 2]=	0.565749;
     m_EtaROImin[0][ 2][ 3]=  -0.213185;m_EtaROImax[0][ 2][ 3]=  -0.116816;m_PhiROImin[0][ 2][ 3]=   0.483076;m_PhiROImax[0][ 2][ 3]=	0.565749;
     m_EtaROImin[0][ 2][ 4]=  -0.302928;m_EtaROImax[0][ 2][ 4]=  -0.222449;m_PhiROImin[0][ 2][ 4]=   0.395489;m_PhiROImax[0][ 2][ 4]=	0.479445;
     m_EtaROImin[0][ 2][ 5]=  -0.416721;m_EtaROImax[0][ 2][ 5]=   -0.30075;m_PhiROImin[0][ 2][ 5]=   0.395489;m_PhiROImax[0][ 2][ 5]=	0.479445;
     m_EtaROImin[0][ 2][ 6]=  -0.302928;m_EtaROImax[0][ 2][ 6]=  -0.222449;m_PhiROImin[0][ 2][ 6]=   0.483076;m_PhiROImax[0][ 2][ 6]=	0.565749;
     m_EtaROImin[0][ 2][ 7]=  -0.416721;m_EtaROImax[0][ 2][ 7]=   -0.30075;m_PhiROImin[0][ 2][ 7]=   0.483076;m_PhiROImax[0][ 2][ 7]=	0.565749;
     m_EtaROImin[0][ 2][ 8]=  -0.504617;m_EtaROImax[0][ 2][ 8]=   -0.42967;m_PhiROImin[0][ 2][ 8]=   0.395489;m_PhiROImax[0][ 2][ 8]=	0.479445;
     m_EtaROImin[0][ 2][ 9]=  -0.573871;m_EtaROImax[0][ 2][ 9]=  -0.501681;m_PhiROImin[0][ 2][ 9]=   0.395489;m_PhiROImax[0][ 2][ 9]=	0.479445;
     m_EtaROImin[0][ 2][10]=  -0.504617;m_EtaROImax[0][ 2][10]=   -0.42967;m_PhiROImin[0][ 2][10]=   0.483076;m_PhiROImax[0][ 2][10]=	0.565749;
     m_EtaROImin[0][ 2][11]=  -0.573871;m_EtaROImax[0][ 2][11]=  -0.501681;m_PhiROImin[0][ 2][11]=   0.483076;m_PhiROImax[0][ 2][11]=	0.565749;
     m_EtaROImin[0][ 2][12]=  -0.653329;m_EtaROImax[0][ 2][12]=  -0.583785;m_PhiROImin[0][ 2][12]=   0.395489;m_PhiROImax[0][ 2][12]=	0.479445;
     m_EtaROImin[0][ 2][13]=  -0.741516;m_EtaROImax[0][ 2][13]=  -0.649933;m_PhiROImin[0][ 2][13]=   0.395489;m_PhiROImax[0][ 2][13]=	0.479445;
     m_EtaROImin[0][ 2][14]=  -0.653329;m_EtaROImax[0][ 2][14]=  -0.583785;m_PhiROImin[0][ 2][14]=   0.483076;m_PhiROImax[0][ 2][14]=	0.565749;
     m_EtaROImin[0][ 2][15]=  -0.741516;m_EtaROImax[0][ 2][15]=  -0.649933;m_PhiROImin[0][ 2][15]=   0.483076;m_PhiROImax[0][ 2][15]=	0.565749;
     m_EtaROImin[0][ 2][16]=  -0.837822;m_EtaROImax[0][ 2][16]=  -0.756521;m_PhiROImin[0][ 2][16]=   0.395489;m_PhiROImax[0][ 2][16]=	0.479445;
     m_EtaROImin[0][ 2][17]=	    0;m_EtaROImax[0][ 2][17]=	     0;m_PhiROImin[0][ 2][17]=	      0;m_PhiROImax[0][ 2][17]=	       0;
     m_EtaROImin[0][ 2][18]=  -0.837822;m_EtaROImax[0][ 2][18]=  -0.756521;m_PhiROImin[0][ 2][18]=   0.483076;m_PhiROImax[0][ 2][18]=	0.565749;
     m_EtaROImin[0][ 2][19]=	    0;m_EtaROImax[0][ 2][19]=	     0;m_PhiROImin[0][ 2][19]=	      0;m_PhiROImax[0][ 2][19]=	       0;
     m_EtaROImin[0][ 2][20]=  -0.903324;m_EtaROImax[0][ 2][20]=  -0.844116;m_PhiROImin[0][ 2][20]=   0.395489;m_PhiROImax[0][ 2][20]=	0.479445;
     m_EtaROImin[0][ 2][21]=  -0.956037;m_EtaROImax[0][ 2][21]=  -0.899344;m_PhiROImin[0][ 2][21]=   0.395489;m_PhiROImax[0][ 2][21]=	0.479445;
     m_EtaROImin[0][ 2][22]=  -0.903324;m_EtaROImax[0][ 2][22]=  -0.844116;m_PhiROImin[0][ 2][22]=   0.483076;m_PhiROImax[0][ 2][22]=	0.565749;
     m_EtaROImin[0][ 2][23]=  -0.956037;m_EtaROImax[0][ 2][23]=  -0.899344;m_PhiROImin[0][ 2][23]=   0.483076;m_PhiROImax[0][ 2][23]=	0.565749;
     m_EtaROImin[0][ 2][24]=	    0;m_EtaROImax[0][ 2][24]=	     0;m_PhiROImin[0][ 2][24]=	      0;m_PhiROImax[0][ 2][24]=	       0;
     m_EtaROImin[0][ 2][25]=	    0;m_EtaROImax[0][ 2][25]=	     0;m_PhiROImin[0][ 2][25]=	      0;m_PhiROImax[0][ 2][25]=	       0;
     m_EtaROImin[0][ 2][26]=	    0;m_EtaROImax[0][ 2][26]=	     0;m_PhiROImin[0][ 2][26]=	      0;m_PhiROImax[0][ 2][26]=	       0;
     m_EtaROImin[0][ 2][27]=	    0;m_EtaROImax[0][ 2][27]=	     0;m_PhiROImin[0][ 2][27]=	      0;m_PhiROImax[0][ 2][27]=	       0;
     m_EtaROImin[0][ 2][28]=	    0;m_EtaROImax[0][ 2][28]=	     0;m_PhiROImin[0][ 2][28]=	      0;m_PhiROImax[0][ 2][28]=	       0;
     m_EtaROImin[0][ 2][29]=	    0;m_EtaROImax[0][ 2][29]=	     0;m_PhiROImin[0][ 2][29]=	      0;m_PhiROImax[0][ 2][29]=	       0;
     m_EtaROImin[0][ 2][30]=	    0;m_EtaROImax[0][ 2][30]=	     0;m_PhiROImin[0][ 2][30]=	      0;m_PhiROImax[0][ 2][30]=	       0;
     m_EtaROImin[0][ 2][31]=	    0;m_EtaROImax[0][ 2][31]=	     0;m_PhiROImin[0][ 2][31]=	      0;m_PhiROImax[0][ 2][31]=	       0;
     m_EtaROImin[0][ 3][ 0]=  -0.238532;m_EtaROImax[0][ 3][ 0]=  -0.147302;m_PhiROImin[0][ 3][ 0]=   0.672786;m_PhiROImax[0][ 3][ 0]=	0.782446;
     m_EtaROImin[0][ 3][ 1]=  -0.147437;m_EtaROImax[0][ 3][ 1]= -0.0554683;m_PhiROImin[0][ 3][ 1]=   0.672786;m_PhiROImax[0][ 3][ 1]=	0.782446;
     m_EtaROImin[0][ 3][ 2]=  -0.238532;m_EtaROImax[0][ 3][ 2]=  -0.147302;m_PhiROImin[0][ 3][ 2]=   0.562387;m_PhiROImax[0][ 3][ 2]=	0.669279;
     m_EtaROImin[0][ 3][ 3]=  -0.147437;m_EtaROImax[0][ 3][ 3]= -0.0554683;m_PhiROImin[0][ 3][ 3]=   0.562387;m_PhiROImax[0][ 3][ 3]=	0.669279;
     m_EtaROImin[0][ 3][ 4]=  -0.452099;m_EtaROImax[0][ 3][ 4]=  -0.350831;m_PhiROImin[0][ 3][ 4]=   0.672786;m_PhiROImax[0][ 3][ 4]=	0.782446;
     m_EtaROImin[0][ 3][ 5]=   -0.35049;m_EtaROImax[0][ 3][ 5]=  -0.246207;m_PhiROImin[0][ 3][ 5]=   0.672786;m_PhiROImax[0][ 3][ 5]=	0.782446;
     m_EtaROImin[0][ 3][ 6]=  -0.452099;m_EtaROImax[0][ 3][ 6]=  -0.350831;m_PhiROImin[0][ 3][ 6]=   0.562387;m_PhiROImax[0][ 3][ 6]=	0.669279;
     m_EtaROImin[0][ 3][ 7]=   -0.35049;m_EtaROImax[0][ 3][ 7]=  -0.246207;m_PhiROImin[0][ 3][ 7]=   0.562387;m_PhiROImax[0][ 3][ 7]=	0.669279;
     m_EtaROImin[0][ 3][ 8]=  -0.648101;m_EtaROImax[0][ 3][ 8]=  -0.555789;m_PhiROImin[0][ 3][ 8]=   0.672786;m_PhiROImax[0][ 3][ 8]=	0.782446;
     m_EtaROImin[0][ 3][ 9]=  -0.554444;m_EtaROImax[0][ 3][ 9]=  -0.458091;m_PhiROImin[0][ 3][ 9]=   0.672786;m_PhiROImax[0][ 3][ 9]=	0.782446;
     m_EtaROImin[0][ 3][10]=  -0.648101;m_EtaROImax[0][ 3][10]=  -0.555789;m_PhiROImin[0][ 3][10]=   0.562387;m_PhiROImax[0][ 3][10]=	0.669279;
     m_EtaROImin[0][ 3][11]=  -0.554444;m_EtaROImax[0][ 3][11]=  -0.458091;m_PhiROImin[0][ 3][11]=   0.562387;m_PhiROImax[0][ 3][11]=	0.669279;
     m_EtaROImin[0][ 3][12]=  -0.776449;m_EtaROImax[0][ 3][12]=  -0.731124;m_PhiROImin[0][ 3][12]=   0.672186;m_PhiROImax[0][ 3][12]=	 0.78243;
     m_EtaROImin[0][ 3][13]=  -0.728056;m_EtaROImax[0][ 3][13]=  -0.655857;m_PhiROImin[0][ 3][13]=   0.672186;m_PhiROImax[0][ 3][13]=	 0.78243;
     m_EtaROImin[0][ 3][14]=  -0.776449;m_EtaROImax[0][ 3][14]=  -0.731124;m_PhiROImin[0][ 3][14]=   0.561229;m_PhiROImax[0][ 3][14]=	0.668661;
     m_EtaROImin[0][ 3][15]=  -0.728056;m_EtaROImax[0][ 3][15]=  -0.655857;m_PhiROImin[0][ 3][15]=   0.561229;m_PhiROImax[0][ 3][15]=	0.668661;
     m_EtaROImin[0][ 3][16]=  -0.895163;m_EtaROImax[0][ 3][16]=  -0.853359;m_PhiROImin[0][ 3][16]=   0.672186;m_PhiROImax[0][ 3][16]=	 0.78243;
     m_EtaROImin[0][ 3][17]=  -0.850528;m_EtaROImax[0][ 3][17]=  -0.783867;m_PhiROImin[0][ 3][17]=   0.672186;m_PhiROImax[0][ 3][17]=	 0.78243;
     m_EtaROImin[0][ 3][18]=  -0.895163;m_EtaROImax[0][ 3][18]=  -0.853359;m_PhiROImin[0][ 3][18]=   0.561229;m_PhiROImax[0][ 3][18]=	0.668661;
     m_EtaROImin[0][ 3][19]=  -0.850528;m_EtaROImax[0][ 3][19]=  -0.783867;m_PhiROImin[0][ 3][19]=   0.561229;m_PhiROImax[0][ 3][19]=	0.668661;
     m_EtaROImin[0][ 3][20]=   -1.02514;m_EtaROImax[0][ 3][20]=  -0.964674;m_PhiROImin[0][ 3][20]=   0.672786;m_PhiROImax[0][ 3][20]=	0.782446;
     m_EtaROImin[0][ 3][21]=  -0.961344;m_EtaROImax[0][ 3][21]=  -0.898201;m_PhiROImin[0][ 3][21]=   0.672786;m_PhiROImax[0][ 3][21]=	0.782446;
     m_EtaROImin[0][ 3][22]=   -1.02514;m_EtaROImax[0][ 3][22]=  -0.964674;m_PhiROImin[0][ 3][22]=   0.562387;m_PhiROImax[0][ 3][22]=	0.669279;
     m_EtaROImin[0][ 3][23]=  -0.961344;m_EtaROImax[0][ 3][23]=  -0.898201;m_PhiROImin[0][ 3][23]=   0.562387;m_PhiROImax[0][ 3][23]=	0.669279;
     m_EtaROImin[0][ 3][24]=	    0;m_EtaROImax[0][ 3][24]=	     0;m_PhiROImin[0][ 3][24]=	      0;m_PhiROImax[0][ 3][24]=	       0;
     m_EtaROImin[0][ 3][25]=   -1.06547;m_EtaROImax[0][ 3][25]=   -1.03003;m_PhiROImin[0][ 3][25]=   0.671969;m_PhiROImax[0][ 3][25]=	0.782329;
     m_EtaROImin[0][ 3][26]=	    0;m_EtaROImax[0][ 3][26]=	     0;m_PhiROImin[0][ 3][26]=	      0;m_PhiROImax[0][ 3][26]=	       0;
     m_EtaROImin[0][ 3][27]=   -1.06547;m_EtaROImax[0][ 3][27]=   -1.03003;m_PhiROImin[0][ 3][27]=   0.588255;m_PhiROImax[0][ 3][27]=	 0.66844;
     m_EtaROImin[0][ 3][28]=	    0;m_EtaROImax[0][ 3][28]=	     0;m_PhiROImin[0][ 3][28]=	      0;m_PhiROImax[0][ 3][28]=	       0;
     m_EtaROImin[0][ 3][29]=	    0;m_EtaROImax[0][ 3][29]=	     0;m_PhiROImin[0][ 3][29]=	      0;m_PhiROImax[0][ 3][29]=	       0;
     m_EtaROImin[0][ 3][30]=	    0;m_EtaROImax[0][ 3][30]=	     0;m_PhiROImin[0][ 3][30]=	      0;m_PhiROImax[0][ 3][30]=	       0;
     m_EtaROImin[0][ 3][31]=	    0;m_EtaROImax[0][ 3][31]=	     0;m_PhiROImin[0][ 3][31]=	      0;m_PhiROImax[0][ 3][31]=	       0;
     m_EtaROImin[0][ 4][ 0]=  -0.147437;m_EtaROImax[0][ 4][ 0]= -0.0554683;m_PhiROImin[0][ 4][ 0]=	0.78835;m_PhiROImax[0][ 4][ 0]=	0.898011;
     m_EtaROImin[0][ 4][ 1]=  -0.238532;m_EtaROImax[0][ 4][ 1]=  -0.147302;m_PhiROImin[0][ 4][ 1]=	0.78835;m_PhiROImax[0][ 4][ 1]=	0.898011;
     m_EtaROImin[0][ 4][ 2]=  -0.147437;m_EtaROImax[0][ 4][ 2]= -0.0554683;m_PhiROImin[0][ 4][ 2]=   0.901517;m_PhiROImax[0][ 4][ 2]=	 1.00841;
     m_EtaROImin[0][ 4][ 3]=  -0.238532;m_EtaROImax[0][ 4][ 3]=  -0.147302;m_PhiROImin[0][ 4][ 3]=   0.901517;m_PhiROImax[0][ 4][ 3]=	 1.00841;
     m_EtaROImin[0][ 4][ 4]=   -0.35049;m_EtaROImax[0][ 4][ 4]=  -0.246207;m_PhiROImin[0][ 4][ 4]=	0.78835;m_PhiROImax[0][ 4][ 4]=	0.898011;
     m_EtaROImin[0][ 4][ 5]=  -0.452099;m_EtaROImax[0][ 4][ 5]=  -0.350831;m_PhiROImin[0][ 4][ 5]=	0.78835;m_PhiROImax[0][ 4][ 5]=	0.898011;
     m_EtaROImin[0][ 4][ 6]=   -0.35049;m_EtaROImax[0][ 4][ 6]=  -0.246207;m_PhiROImin[0][ 4][ 6]=   0.901517;m_PhiROImax[0][ 4][ 6]=	 1.00841;
     m_EtaROImin[0][ 4][ 7]=  -0.452099;m_EtaROImax[0][ 4][ 7]=  -0.350831;m_PhiROImin[0][ 4][ 7]=   0.901517;m_PhiROImax[0][ 4][ 7]=	 1.00841;
     m_EtaROImin[0][ 4][ 8]=  -0.554444;m_EtaROImax[0][ 4][ 8]=  -0.458091;m_PhiROImin[0][ 4][ 8]=	0.78835;m_PhiROImax[0][ 4][ 8]=	0.898011;
     m_EtaROImin[0][ 4][ 9]=  -0.648101;m_EtaROImax[0][ 4][ 9]=  -0.555789;m_PhiROImin[0][ 4][ 9]=	0.78835;m_PhiROImax[0][ 4][ 9]=	0.898011;
     m_EtaROImin[0][ 4][10]=  -0.554444;m_EtaROImax[0][ 4][10]=  -0.458091;m_PhiROImin[0][ 4][10]=   0.901517;m_PhiROImax[0][ 4][10]=	 1.00841;
     m_EtaROImin[0][ 4][11]=  -0.648101;m_EtaROImax[0][ 4][11]=  -0.555789;m_PhiROImin[0][ 4][11]=   0.901517;m_PhiROImax[0][ 4][11]=	 1.00841;
     m_EtaROImin[0][ 4][12]=  -0.703298;m_EtaROImax[0][ 4][12]=  -0.655857;m_PhiROImin[0][ 4][12]=   0.788366;m_PhiROImax[0][ 4][12]=	 0.89861;
     m_EtaROImin[0][ 4][13]=  -0.776449;m_EtaROImax[0][ 4][13]=  -0.706413;m_PhiROImin[0][ 4][13]=   0.788366;m_PhiROImax[0][ 4][13]=	 0.89861;
     m_EtaROImin[0][ 4][14]=  -0.703298;m_EtaROImax[0][ 4][14]=  -0.655857;m_PhiROImin[0][ 4][14]=   0.902135;m_PhiROImax[0][ 4][14]=	 1.00957;
     m_EtaROImin[0][ 4][15]=  -0.776449;m_EtaROImax[0][ 4][15]=  -0.706413;m_PhiROImin[0][ 4][15]=   0.902135;m_PhiROImax[0][ 4][15]=	 1.00957;
     m_EtaROImin[0][ 4][16]=   -0.82768;m_EtaROImax[0][ 4][16]=  -0.783867;m_PhiROImin[0][ 4][16]=   0.788366;m_PhiROImax[0][ 4][16]=	 0.89861;
     m_EtaROImin[0][ 4][17]=  -0.895163;m_EtaROImax[0][ 4][17]=  -0.830556;m_PhiROImin[0][ 4][17]=   0.788366;m_PhiROImax[0][ 4][17]=	 0.89861;
     m_EtaROImin[0][ 4][18]=   -0.82768;m_EtaROImax[0][ 4][18]=  -0.783867;m_PhiROImin[0][ 4][18]=   0.902135;m_PhiROImax[0][ 4][18]=	 1.00957;
     m_EtaROImin[0][ 4][19]=  -0.895163;m_EtaROImax[0][ 4][19]=  -0.830556;m_PhiROImin[0][ 4][19]=   0.902135;m_PhiROImax[0][ 4][19]=	 1.00957;
     m_EtaROImin[0][ 4][20]=  -0.961344;m_EtaROImax[0][ 4][20]=  -0.898201;m_PhiROImin[0][ 4][20]=	0.78835;m_PhiROImax[0][ 4][20]=	0.898011;
     m_EtaROImin[0][ 4][21]=   -1.02514;m_EtaROImax[0][ 4][21]=  -0.964674;m_PhiROImin[0][ 4][21]=	0.78835;m_PhiROImax[0][ 4][21]=	0.898011;
     m_EtaROImin[0][ 4][22]=  -0.961344;m_EtaROImax[0][ 4][22]=  -0.898201;m_PhiROImin[0][ 4][22]=   0.901517;m_PhiROImax[0][ 4][22]=	 1.00841;
     m_EtaROImin[0][ 4][23]=   -1.02514;m_EtaROImax[0][ 4][23]=  -0.964674;m_PhiROImin[0][ 4][23]=   0.901517;m_PhiROImax[0][ 4][23]=	 1.00841;
     m_EtaROImin[0][ 4][24]=   -1.06547;m_EtaROImax[0][ 4][24]=   -1.03003;m_PhiROImin[0][ 4][24]=   0.788467;m_PhiROImax[0][ 4][24]=	0.898827;
     m_EtaROImin[0][ 4][25]=	    0;m_EtaROImax[0][ 4][25]=	     0;m_PhiROImin[0][ 4][25]=	      0;m_PhiROImax[0][ 4][25]=	       0;
     m_EtaROImin[0][ 4][26]=   -1.06547;m_EtaROImax[0][ 4][26]=   -1.03003;m_PhiROImin[0][ 4][26]=   0.902356;m_PhiROImax[0][ 4][26]=	0.982541;
     m_EtaROImin[0][ 4][27]=	    0;m_EtaROImax[0][ 4][27]=	     0;m_PhiROImin[0][ 4][27]=	      0;m_PhiROImax[0][ 4][27]=	       0;
     m_EtaROImin[0][ 4][28]=	    0;m_EtaROImax[0][ 4][28]=	     0;m_PhiROImin[0][ 4][28]=	      0;m_PhiROImax[0][ 4][28]=	       0;
     m_EtaROImin[0][ 4][29]=	    0;m_EtaROImax[0][ 4][29]=	     0;m_PhiROImin[0][ 4][29]=	      0;m_PhiROImax[0][ 4][29]=	       0;
     m_EtaROImin[0][ 4][30]=	    0;m_EtaROImax[0][ 4][30]=	     0;m_PhiROImin[0][ 4][30]=	      0;m_PhiROImax[0][ 4][30]=	       0;
     m_EtaROImin[0][ 4][31]=	    0;m_EtaROImax[0][ 4][31]=	     0;m_PhiROImin[0][ 4][31]=	      0;m_PhiROImax[0][ 4][31]=	       0;
     m_EtaROImin[0][ 5][ 0]=  -0.213185;m_EtaROImax[0][ 5][ 0]=  -0.116816;m_PhiROImin[0][ 5][ 0]=	1.09135;m_PhiROImax[0][ 5][ 0]=	 1.17531;
     m_EtaROImin[0][ 5][ 1]=  -0.118734;m_EtaROImax[0][ 5][ 1]= -0.0208251;m_PhiROImin[0][ 5][ 1]=	1.09135;m_PhiROImax[0][ 5][ 1]=	 1.17531;
     m_EtaROImin[0][ 5][ 2]=  -0.213185;m_EtaROImax[0][ 5][ 2]=  -0.116816;m_PhiROImin[0][ 5][ 2]=	1.00505;m_PhiROImax[0][ 5][ 2]=	 1.08772;
     m_EtaROImin[0][ 5][ 3]=  -0.118734;m_EtaROImax[0][ 5][ 3]= -0.0208251;m_PhiROImin[0][ 5][ 3]=	1.00505;m_PhiROImax[0][ 5][ 3]=	 1.08772;
     m_EtaROImin[0][ 5][ 4]=  -0.416721;m_EtaROImax[0][ 5][ 4]=   -0.30075;m_PhiROImin[0][ 5][ 4]=	1.09135;m_PhiROImax[0][ 5][ 4]=	 1.17531;
     m_EtaROImin[0][ 5][ 5]=  -0.302928;m_EtaROImax[0][ 5][ 5]=  -0.222449;m_PhiROImin[0][ 5][ 5]=	1.09135;m_PhiROImax[0][ 5][ 5]=	 1.17531;
     m_EtaROImin[0][ 5][ 6]=  -0.416721;m_EtaROImax[0][ 5][ 6]=   -0.30075;m_PhiROImin[0][ 5][ 6]=	1.00505;m_PhiROImax[0][ 5][ 6]=	 1.08772;
     m_EtaROImin[0][ 5][ 7]=  -0.302928;m_EtaROImax[0][ 5][ 7]=  -0.222449;m_PhiROImin[0][ 5][ 7]=	1.00505;m_PhiROImax[0][ 5][ 7]=	 1.08772;
     m_EtaROImin[0][ 5][ 8]=  -0.573871;m_EtaROImax[0][ 5][ 8]=  -0.501681;m_PhiROImin[0][ 5][ 8]=	1.09135;m_PhiROImax[0][ 5][ 8]=	 1.17531;
     m_EtaROImin[0][ 5][ 9]=  -0.504617;m_EtaROImax[0][ 5][ 9]=   -0.42967;m_PhiROImin[0][ 5][ 9]=	1.09135;m_PhiROImax[0][ 5][ 9]=	 1.17531;
     m_EtaROImin[0][ 5][10]=  -0.573871;m_EtaROImax[0][ 5][10]=  -0.501681;m_PhiROImin[0][ 5][10]=	1.00505;m_PhiROImax[0][ 5][10]=	 1.08772;
     m_EtaROImin[0][ 5][11]=  -0.504617;m_EtaROImax[0][ 5][11]=   -0.42967;m_PhiROImin[0][ 5][11]=	1.00505;m_PhiROImax[0][ 5][11]=	 1.08772;
     m_EtaROImin[0][ 5][12]=  -0.741516;m_EtaROImax[0][ 5][12]=  -0.649933;m_PhiROImin[0][ 5][12]=	1.09135;m_PhiROImax[0][ 5][12]=	 1.17531;
     m_EtaROImin[0][ 5][13]=  -0.653329;m_EtaROImax[0][ 5][13]=  -0.583785;m_PhiROImin[0][ 5][13]=	1.09135;m_PhiROImax[0][ 5][13]=	 1.17531;
     m_EtaROImin[0][ 5][14]=  -0.741516;m_EtaROImax[0][ 5][14]=  -0.649933;m_PhiROImin[0][ 5][14]=	1.00505;m_PhiROImax[0][ 5][14]=	 1.08772;
     m_EtaROImin[0][ 5][15]=  -0.653329;m_EtaROImax[0][ 5][15]=  -0.583785;m_PhiROImin[0][ 5][15]=	1.00505;m_PhiROImax[0][ 5][15]=	 1.08772;
     m_EtaROImin[0][ 5][16]=	    0;m_EtaROImax[0][ 5][16]=	     0;m_PhiROImin[0][ 5][16]=	      0;m_PhiROImax[0][ 5][16]=	       0;
     m_EtaROImin[0][ 5][17]=  -0.837822;m_EtaROImax[0][ 5][17]=  -0.756521;m_PhiROImin[0][ 5][17]=	1.09135;m_PhiROImax[0][ 5][17]=	 1.17531;
     m_EtaROImin[0][ 5][18]=	    0;m_EtaROImax[0][ 5][18]=	     0;m_PhiROImin[0][ 5][18]=	      0;m_PhiROImax[0][ 5][18]=	       0;
     m_EtaROImin[0][ 5][19]=  -0.837822;m_EtaROImax[0][ 5][19]=  -0.756521;m_PhiROImin[0][ 5][19]=	1.00505;m_PhiROImax[0][ 5][19]=	 1.08772;
     m_EtaROImin[0][ 5][20]=  -0.956037;m_EtaROImax[0][ 5][20]=  -0.899344;m_PhiROImin[0][ 5][20]=	1.09135;m_PhiROImax[0][ 5][20]=	 1.17531;
     m_EtaROImin[0][ 5][21]=  -0.903324;m_EtaROImax[0][ 5][21]=  -0.844116;m_PhiROImin[0][ 5][21]=	1.09135;m_PhiROImax[0][ 5][21]=	 1.17531;
     m_EtaROImin[0][ 5][22]=  -0.956037;m_EtaROImax[0][ 5][22]=  -0.899344;m_PhiROImin[0][ 5][22]=	1.00505;m_PhiROImax[0][ 5][22]=	 1.08772;
     m_EtaROImin[0][ 5][23]=  -0.903324;m_EtaROImax[0][ 5][23]=  -0.844116;m_PhiROImin[0][ 5][23]=	1.00505;m_PhiROImax[0][ 5][23]=	 1.08772;
     m_EtaROImin[0][ 5][24]=	    0;m_EtaROImax[0][ 5][24]=	     0;m_PhiROImin[0][ 5][24]=	      0;m_PhiROImax[0][ 5][24]=	       0;
     m_EtaROImin[0][ 5][25]=	    0;m_EtaROImax[0][ 5][25]=	     0;m_PhiROImin[0][ 5][25]=	      0;m_PhiROImax[0][ 5][25]=	       0;
     m_EtaROImin[0][ 5][26]=	    0;m_EtaROImax[0][ 5][26]=	     0;m_PhiROImin[0][ 5][26]=	      0;m_PhiROImax[0][ 5][26]=	       0;
     m_EtaROImin[0][ 5][27]=	    0;m_EtaROImax[0][ 5][27]=	     0;m_PhiROImin[0][ 5][27]=	      0;m_PhiROImax[0][ 5][27]=	       0;
     m_EtaROImin[0][ 5][28]=	    0;m_EtaROImax[0][ 5][28]=	     0;m_PhiROImin[0][ 5][28]=	      0;m_PhiROImax[0][ 5][28]=	       0;
     m_EtaROImin[0][ 5][29]=	    0;m_EtaROImax[0][ 5][29]=	     0;m_PhiROImin[0][ 5][29]=	      0;m_PhiROImax[0][ 5][29]=	       0;
     m_EtaROImin[0][ 5][30]=	    0;m_EtaROImax[0][ 5][30]=	     0;m_PhiROImin[0][ 5][30]=	      0;m_PhiROImax[0][ 5][30]=	       0;
     m_EtaROImin[0][ 5][31]=	    0;m_EtaROImax[0][ 5][31]=	     0;m_PhiROImin[0][ 5][31]=	      0;m_PhiROImax[0][ 5][31]=	       0;
     m_EtaROImin[0][ 6][ 0]=  -0.118734;m_EtaROImax[0][ 6][ 0]= -0.0208251;m_PhiROImin[0][ 6][ 0]=	1.18089;m_PhiROImax[0][ 6][ 0]=	 1.26484;
     m_EtaROImin[0][ 6][ 1]=  -0.213185;m_EtaROImax[0][ 6][ 1]=  -0.116816;m_PhiROImin[0][ 6][ 1]=	1.18089;m_PhiROImax[0][ 6][ 1]=	 1.26484;
     m_EtaROImin[0][ 6][ 2]=  -0.118734;m_EtaROImax[0][ 6][ 2]= -0.0208251;m_PhiROImin[0][ 6][ 2]=	1.26847;m_PhiROImax[0][ 6][ 2]=	 1.35115;
     m_EtaROImin[0][ 6][ 3]=  -0.213185;m_EtaROImax[0][ 6][ 3]=  -0.116816;m_PhiROImin[0][ 6][ 3]=	1.26847;m_PhiROImax[0][ 6][ 3]=	 1.35115;
     m_EtaROImin[0][ 6][ 4]=  -0.302928;m_EtaROImax[0][ 6][ 4]=  -0.222449;m_PhiROImin[0][ 6][ 4]=	1.18089;m_PhiROImax[0][ 6][ 4]=	 1.26484;
     m_EtaROImin[0][ 6][ 5]=  -0.416721;m_EtaROImax[0][ 6][ 5]=   -0.30075;m_PhiROImin[0][ 6][ 5]=	1.18089;m_PhiROImax[0][ 6][ 5]=	 1.26484;
     m_EtaROImin[0][ 6][ 6]=  -0.302928;m_EtaROImax[0][ 6][ 6]=  -0.222449;m_PhiROImin[0][ 6][ 6]=	1.26847;m_PhiROImax[0][ 6][ 6]=	 1.35115;
     m_EtaROImin[0][ 6][ 7]=  -0.416721;m_EtaROImax[0][ 6][ 7]=   -0.30075;m_PhiROImin[0][ 6][ 7]=	1.26847;m_PhiROImax[0][ 6][ 7]=	 1.35115;
     m_EtaROImin[0][ 6][ 8]=  -0.504617;m_EtaROImax[0][ 6][ 8]=   -0.42967;m_PhiROImin[0][ 6][ 8]=	1.18089;m_PhiROImax[0][ 6][ 8]=	 1.26484;
     m_EtaROImin[0][ 6][ 9]=  -0.573871;m_EtaROImax[0][ 6][ 9]=  -0.501681;m_PhiROImin[0][ 6][ 9]=	1.18089;m_PhiROImax[0][ 6][ 9]=	 1.26484;
     m_EtaROImin[0][ 6][10]=  -0.504617;m_EtaROImax[0][ 6][10]=   -0.42967;m_PhiROImin[0][ 6][10]=	1.26847;m_PhiROImax[0][ 6][10]=	 1.35115;
     m_EtaROImin[0][ 6][11]=  -0.573871;m_EtaROImax[0][ 6][11]=  -0.501681;m_PhiROImin[0][ 6][11]=	1.26847;m_PhiROImax[0][ 6][11]=	 1.35115;
     m_EtaROImin[0][ 6][12]=  -0.653329;m_EtaROImax[0][ 6][12]=  -0.583785;m_PhiROImin[0][ 6][12]=	1.18089;m_PhiROImax[0][ 6][12]=	 1.26484;
     m_EtaROImin[0][ 6][13]=  -0.741516;m_EtaROImax[0][ 6][13]=  -0.649933;m_PhiROImin[0][ 6][13]=	1.18089;m_PhiROImax[0][ 6][13]=	 1.26484;
     m_EtaROImin[0][ 6][14]=  -0.653329;m_EtaROImax[0][ 6][14]=  -0.583785;m_PhiROImin[0][ 6][14]=	1.26847;m_PhiROImax[0][ 6][14]=	 1.35115;
     m_EtaROImin[0][ 6][15]=  -0.741516;m_EtaROImax[0][ 6][15]=  -0.649933;m_PhiROImin[0][ 6][15]=	1.26847;m_PhiROImax[0][ 6][15]=	 1.35115;
     m_EtaROImin[0][ 6][16]=  -0.837822;m_EtaROImax[0][ 6][16]=  -0.756521;m_PhiROImin[0][ 6][16]=	1.18089;m_PhiROImax[0][ 6][16]=	 1.26484;
     m_EtaROImin[0][ 6][17]=	    0;m_EtaROImax[0][ 6][17]=	     0;m_PhiROImin[0][ 6][17]=	      0;m_PhiROImax[0][ 6][17]=	       0;
     m_EtaROImin[0][ 6][18]=  -0.837822;m_EtaROImax[0][ 6][18]=  -0.756521;m_PhiROImin[0][ 6][18]=	1.26847;m_PhiROImax[0][ 6][18]=	 1.35115;
     m_EtaROImin[0][ 6][19]=	    0;m_EtaROImax[0][ 6][19]=	     0;m_PhiROImin[0][ 6][19]=	      0;m_PhiROImax[0][ 6][19]=	       0;
     m_EtaROImin[0][ 6][20]=  -0.903324;m_EtaROImax[0][ 6][20]=  -0.844116;m_PhiROImin[0][ 6][20]=	1.18089;m_PhiROImax[0][ 6][20]=	 1.26484;
     m_EtaROImin[0][ 6][21]=  -0.956037;m_EtaROImax[0][ 6][21]=  -0.899344;m_PhiROImin[0][ 6][21]=	1.18089;m_PhiROImax[0][ 6][21]=	 1.26484;
     m_EtaROImin[0][ 6][22]=  -0.903324;m_EtaROImax[0][ 6][22]=  -0.844116;m_PhiROImin[0][ 6][22]=	1.26847;m_PhiROImax[0][ 6][22]=	 1.35115;
     m_EtaROImin[0][ 6][23]=  -0.956037;m_EtaROImax[0][ 6][23]=  -0.899344;m_PhiROImin[0][ 6][23]=	1.26847;m_PhiROImax[0][ 6][23]=	 1.35115;
     m_EtaROImin[0][ 6][24]=	    0;m_EtaROImax[0][ 6][24]=	     0;m_PhiROImin[0][ 6][24]=	      0;m_PhiROImax[0][ 6][24]=	       0;
     m_EtaROImin[0][ 6][25]=	    0;m_EtaROImax[0][ 6][25]=	     0;m_PhiROImin[0][ 6][25]=	      0;m_PhiROImax[0][ 6][25]=	       0;
     m_EtaROImin[0][ 6][26]=	    0;m_EtaROImax[0][ 6][26]=	     0;m_PhiROImin[0][ 6][26]=	      0;m_PhiROImax[0][ 6][26]=	       0;
     m_EtaROImin[0][ 6][27]=	    0;m_EtaROImax[0][ 6][27]=	     0;m_PhiROImin[0][ 6][27]=	      0;m_PhiROImax[0][ 6][27]=	       0;
     m_EtaROImin[0][ 6][28]=	    0;m_EtaROImax[0][ 6][28]=	     0;m_PhiROImin[0][ 6][28]=	      0;m_PhiROImax[0][ 6][28]=	       0;
     m_EtaROImin[0][ 6][29]=	    0;m_EtaROImax[0][ 6][29]=	     0;m_PhiROImin[0][ 6][29]=	      0;m_PhiROImax[0][ 6][29]=	       0;
     m_EtaROImin[0][ 6][30]=	    0;m_EtaROImax[0][ 6][30]=	     0;m_PhiROImin[0][ 6][30]=	      0;m_PhiROImax[0][ 6][30]=	       0;
     m_EtaROImin[0][ 6][31]=	    0;m_EtaROImax[0][ 6][31]=	     0;m_PhiROImin[0][ 6][31]=	      0;m_PhiROImax[0][ 6][31]=	       0;
     m_EtaROImin[0][ 7][ 0]=  -0.238313;m_EtaROImax[0][ 7][ 0]=   -0.14708;m_PhiROImin[0][ 7][ 0]=	1.45818;m_PhiROImax[0][ 7][ 0]=	 1.56784;
     m_EtaROImin[0][ 7][ 1]=  -0.147216;m_EtaROImax[0][ 7][ 1]= -0.0552456;m_PhiROImin[0][ 7][ 1]=	1.45818;m_PhiROImax[0][ 7][ 1]=	 1.56784;
     m_EtaROImin[0][ 7][ 2]=  -0.238313;m_EtaROImax[0][ 7][ 2]=   -0.14708;m_PhiROImin[0][ 7][ 2]=	1.34779;m_PhiROImax[0][ 7][ 2]=	 1.45468;
     m_EtaROImin[0][ 7][ 3]=  -0.147216;m_EtaROImax[0][ 7][ 3]= -0.0552456;m_PhiROImin[0][ 7][ 3]=	1.34779;m_PhiROImax[0][ 7][ 3]=	 1.45468;
     m_EtaROImin[0][ 7][ 4]=  -0.452099;m_EtaROImax[0][ 7][ 4]=  -0.350831;m_PhiROImin[0][ 7][ 4]=	1.45818;m_PhiROImax[0][ 7][ 4]=	 1.56784;
     m_EtaROImin[0][ 7][ 5]=   -0.35049;m_EtaROImax[0][ 7][ 5]=  -0.246207;m_PhiROImin[0][ 7][ 5]=	1.45818;m_PhiROImax[0][ 7][ 5]=	 1.56784;
     m_EtaROImin[0][ 7][ 6]=  -0.452099;m_EtaROImax[0][ 7][ 6]=  -0.350831;m_PhiROImin[0][ 7][ 6]=	1.34779;m_PhiROImax[0][ 7][ 6]=	 1.45468;
     m_EtaROImin[0][ 7][ 7]=   -0.35049;m_EtaROImax[0][ 7][ 7]=  -0.246207;m_PhiROImin[0][ 7][ 7]=	1.34779;m_PhiROImax[0][ 7][ 7]=	 1.45468;
     m_EtaROImin[0][ 7][ 8]=  -0.648101;m_EtaROImax[0][ 7][ 8]=  -0.555789;m_PhiROImin[0][ 7][ 8]=	1.45818;m_PhiROImax[0][ 7][ 8]=	 1.56784;
     m_EtaROImin[0][ 7][ 9]=  -0.554444;m_EtaROImax[0][ 7][ 9]=  -0.458091;m_PhiROImin[0][ 7][ 9]=	1.45818;m_PhiROImax[0][ 7][ 9]=	 1.56784;
     m_EtaROImin[0][ 7][10]=  -0.648101;m_EtaROImax[0][ 7][10]=  -0.555789;m_PhiROImin[0][ 7][10]=	1.34779;m_PhiROImax[0][ 7][10]=	 1.45468;
     m_EtaROImin[0][ 7][11]=  -0.554444;m_EtaROImax[0][ 7][11]=  -0.458091;m_PhiROImin[0][ 7][11]=	1.34779;m_PhiROImax[0][ 7][11]=	 1.45468;
     m_EtaROImin[0][ 7][12]=  -0.776449;m_EtaROImax[0][ 7][12]=  -0.731124;m_PhiROImin[0][ 7][12]=	1.45758;m_PhiROImax[0][ 7][12]=	 1.56783;
     m_EtaROImin[0][ 7][13]=  -0.728056;m_EtaROImax[0][ 7][13]=  -0.655857;m_PhiROImin[0][ 7][13]=	1.45758;m_PhiROImax[0][ 7][13]=	 1.56783;
     m_EtaROImin[0][ 7][14]=  -0.776449;m_EtaROImax[0][ 7][14]=  -0.731124;m_PhiROImin[0][ 7][14]=	1.34663;m_PhiROImax[0][ 7][14]=	 1.45406;
     m_EtaROImin[0][ 7][15]=  -0.728056;m_EtaROImax[0][ 7][15]=  -0.655857;m_PhiROImin[0][ 7][15]=	1.34663;m_PhiROImax[0][ 7][15]=	 1.45406;
     m_EtaROImin[0][ 7][16]=  -0.895163;m_EtaROImax[0][ 7][16]=  -0.853359;m_PhiROImin[0][ 7][16]=	1.45758;m_PhiROImax[0][ 7][16]=	 1.56783;
     m_EtaROImin[0][ 7][17]=  -0.850528;m_EtaROImax[0][ 7][17]=  -0.783867;m_PhiROImin[0][ 7][17]=	1.45758;m_PhiROImax[0][ 7][17]=	 1.56783;
     m_EtaROImin[0][ 7][18]=  -0.895163;m_EtaROImax[0][ 7][18]=  -0.853359;m_PhiROImin[0][ 7][18]=	1.34663;m_PhiROImax[0][ 7][18]=	 1.45406;
     m_EtaROImin[0][ 7][19]=  -0.850528;m_EtaROImax[0][ 7][19]=  -0.783867;m_PhiROImin[0][ 7][19]=	1.34663;m_PhiROImax[0][ 7][19]=	 1.45406;
     m_EtaROImin[0][ 7][20]=   -1.02514;m_EtaROImax[0][ 7][20]=  -0.964674;m_PhiROImin[0][ 7][20]=	1.45818;m_PhiROImax[0][ 7][20]=	 1.56784;
     m_EtaROImin[0][ 7][21]=  -0.961344;m_EtaROImax[0][ 7][21]=  -0.898201;m_PhiROImin[0][ 7][21]=	1.45818;m_PhiROImax[0][ 7][21]=	 1.56784;
     m_EtaROImin[0][ 7][22]=   -1.02514;m_EtaROImax[0][ 7][22]=  -0.964674;m_PhiROImin[0][ 7][22]=	1.34779;m_PhiROImax[0][ 7][22]=	 1.45468;
     m_EtaROImin[0][ 7][23]=  -0.961344;m_EtaROImax[0][ 7][23]=  -0.898201;m_PhiROImin[0][ 7][23]=	1.34779;m_PhiROImax[0][ 7][23]=	 1.45468;
     m_EtaROImin[0][ 7][24]=	    0;m_EtaROImax[0][ 7][24]=	     0;m_PhiROImin[0][ 7][24]=	      0;m_PhiROImax[0][ 7][24]=	       0;
     m_EtaROImin[0][ 7][25]=   -1.06547;m_EtaROImax[0][ 7][25]=   -1.03003;m_PhiROImin[0][ 7][25]=	1.45737;m_PhiROImax[0][ 7][25]=	 1.56773;
     m_EtaROImin[0][ 7][26]=	    0;m_EtaROImax[0][ 7][26]=	     0;m_PhiROImin[0][ 7][26]=	      0;m_PhiROImax[0][ 7][26]=	       0;
     m_EtaROImin[0][ 7][27]=   -1.06547;m_EtaROImax[0][ 7][27]=   -1.03003;m_PhiROImin[0][ 7][27]=	1.37365;m_PhiROImax[0][ 7][27]=	 1.45384;
     m_EtaROImin[0][ 7][28]=	    0;m_EtaROImax[0][ 7][28]=	     0;m_PhiROImin[0][ 7][28]=	      0;m_PhiROImax[0][ 7][28]=	       0;
     m_EtaROImin[0][ 7][29]=	    0;m_EtaROImax[0][ 7][29]=	     0;m_PhiROImin[0][ 7][29]=	      0;m_PhiROImax[0][ 7][29]=	       0;
     m_EtaROImin[0][ 7][30]=	    0;m_EtaROImax[0][ 7][30]=	     0;m_PhiROImin[0][ 7][30]=	      0;m_PhiROImax[0][ 7][30]=	       0;
     m_EtaROImin[0][ 7][31]=	    0;m_EtaROImax[0][ 7][31]=	     0;m_PhiROImin[0][ 7][31]=	      0;m_PhiROImax[0][ 7][31]=	       0;
     m_EtaROImin[0][ 8][ 0]=  -0.147216;m_EtaROImax[0][ 8][ 0]= -0.0552456;m_PhiROImin[0][ 8][ 0]=	1.57375;m_PhiROImax[0][ 8][ 0]=	 1.68341;
     m_EtaROImin[0][ 8][ 1]=  -0.238313;m_EtaROImax[0][ 8][ 1]=   -0.14708;m_PhiROImin[0][ 8][ 1]=	1.57375;m_PhiROImax[0][ 8][ 1]=	 1.68341;
     m_EtaROImin[0][ 8][ 2]=  -0.147216;m_EtaROImax[0][ 8][ 2]= -0.0552456;m_PhiROImin[0][ 8][ 2]=	1.68692;m_PhiROImax[0][ 8][ 2]=	 1.79381;
     m_EtaROImin[0][ 8][ 3]=  -0.238313;m_EtaROImax[0][ 8][ 3]=   -0.14708;m_PhiROImin[0][ 8][ 3]=	1.68692;m_PhiROImax[0][ 8][ 3]=	 1.79381;
     m_EtaROImin[0][ 8][ 4]=   -0.35049;m_EtaROImax[0][ 8][ 4]=  -0.246207;m_PhiROImin[0][ 8][ 4]=	1.57375;m_PhiROImax[0][ 8][ 4]=	 1.68341;
     m_EtaROImin[0][ 8][ 5]=  -0.452099;m_EtaROImax[0][ 8][ 5]=  -0.350831;m_PhiROImin[0][ 8][ 5]=	1.57375;m_PhiROImax[0][ 8][ 5]=	 1.68341;
     m_EtaROImin[0][ 8][ 6]=   -0.35049;m_EtaROImax[0][ 8][ 6]=  -0.246207;m_PhiROImin[0][ 8][ 6]=	1.68692;m_PhiROImax[0][ 8][ 6]=	 1.79381;
     m_EtaROImin[0][ 8][ 7]=  -0.452099;m_EtaROImax[0][ 8][ 7]=  -0.350831;m_PhiROImin[0][ 8][ 7]=	1.68692;m_PhiROImax[0][ 8][ 7]=	 1.79381;
     m_EtaROImin[0][ 8][ 8]=  -0.554444;m_EtaROImax[0][ 8][ 8]=  -0.458091;m_PhiROImin[0][ 8][ 8]=	1.57375;m_PhiROImax[0][ 8][ 8]=	 1.68341;
     m_EtaROImin[0][ 8][ 9]=  -0.648101;m_EtaROImax[0][ 8][ 9]=  -0.555789;m_PhiROImin[0][ 8][ 9]=	1.57375;m_PhiROImax[0][ 8][ 9]=	 1.68341;
     m_EtaROImin[0][ 8][10]=  -0.554444;m_EtaROImax[0][ 8][10]=  -0.458091;m_PhiROImin[0][ 8][10]=	1.68692;m_PhiROImax[0][ 8][10]=	 1.79381;
     m_EtaROImin[0][ 8][11]=  -0.648101;m_EtaROImax[0][ 8][11]=  -0.555789;m_PhiROImin[0][ 8][11]=	1.68692;m_PhiROImax[0][ 8][11]=	 1.79381;
     m_EtaROImin[0][ 8][12]=  -0.703298;m_EtaROImax[0][ 8][12]=  -0.655857;m_PhiROImin[0][ 8][12]=	1.57376;m_PhiROImax[0][ 8][12]=	 1.68401;
     m_EtaROImin[0][ 8][13]=  -0.776449;m_EtaROImax[0][ 8][13]=  -0.706413;m_PhiROImin[0][ 8][13]=	1.57376;m_PhiROImax[0][ 8][13]=	 1.68401;
     m_EtaROImin[0][ 8][14]=  -0.703298;m_EtaROImax[0][ 8][14]=  -0.655857;m_PhiROImin[0][ 8][14]=	1.68753;m_PhiROImax[0][ 8][14]=	 1.79497;
     m_EtaROImin[0][ 8][15]=  -0.776449;m_EtaROImax[0][ 8][15]=  -0.706413;m_PhiROImin[0][ 8][15]=	1.68753;m_PhiROImax[0][ 8][15]=	 1.79497;
     m_EtaROImin[0][ 8][16]=   -0.82768;m_EtaROImax[0][ 8][16]=  -0.783867;m_PhiROImin[0][ 8][16]=	1.57376;m_PhiROImax[0][ 8][16]=	 1.68401;
     m_EtaROImin[0][ 8][17]=  -0.895163;m_EtaROImax[0][ 8][17]=  -0.830556;m_PhiROImin[0][ 8][17]=	1.57376;m_PhiROImax[0][ 8][17]=	 1.68401;
     m_EtaROImin[0][ 8][18]=   -0.82768;m_EtaROImax[0][ 8][18]=  -0.783867;m_PhiROImin[0][ 8][18]=	1.68753;m_PhiROImax[0][ 8][18]=	 1.79497;
     m_EtaROImin[0][ 8][19]=  -0.895163;m_EtaROImax[0][ 8][19]=  -0.830556;m_PhiROImin[0][ 8][19]=	1.68753;m_PhiROImax[0][ 8][19]=	 1.79497;
     m_EtaROImin[0][ 8][20]=  -0.961344;m_EtaROImax[0][ 8][20]=  -0.898201;m_PhiROImin[0][ 8][20]=	1.57375;m_PhiROImax[0][ 8][20]=	 1.68341;
     m_EtaROImin[0][ 8][21]=   -1.02514;m_EtaROImax[0][ 8][21]=  -0.964674;m_PhiROImin[0][ 8][21]=	1.57375;m_PhiROImax[0][ 8][21]=	 1.68341;
     m_EtaROImin[0][ 8][22]=  -0.961344;m_EtaROImax[0][ 8][22]=  -0.898201;m_PhiROImin[0][ 8][22]=	1.68692;m_PhiROImax[0][ 8][22]=	 1.79381;
     m_EtaROImin[0][ 8][23]=   -1.02514;m_EtaROImax[0][ 8][23]=  -0.964674;m_PhiROImin[0][ 8][23]=	1.68692;m_PhiROImax[0][ 8][23]=	 1.79381;
     m_EtaROImin[0][ 8][24]=   -1.06547;m_EtaROImax[0][ 8][24]=   -1.03003;m_PhiROImin[0][ 8][24]=	1.57387;m_PhiROImax[0][ 8][24]=	 1.68423;
     m_EtaROImin[0][ 8][25]=	    0;m_EtaROImax[0][ 8][25]=	     0;m_PhiROImin[0][ 8][25]=	      0;m_PhiROImax[0][ 8][25]=	       0;
     m_EtaROImin[0][ 8][26]=   -1.06547;m_EtaROImax[0][ 8][26]=   -1.03003;m_PhiROImin[0][ 8][26]=	1.68775;m_PhiROImax[0][ 8][26]=	 1.76794;
     m_EtaROImin[0][ 8][27]=	    0;m_EtaROImax[0][ 8][27]=	     0;m_PhiROImin[0][ 8][27]=	      0;m_PhiROImax[0][ 8][27]=	       0;
     m_EtaROImin[0][ 8][28]=	    0;m_EtaROImax[0][ 8][28]=	     0;m_PhiROImin[0][ 8][28]=	      0;m_PhiROImax[0][ 8][28]=	       0;
     m_EtaROImin[0][ 8][29]=	    0;m_EtaROImax[0][ 8][29]=	     0;m_PhiROImin[0][ 8][29]=	      0;m_PhiROImax[0][ 8][29]=	       0;
     m_EtaROImin[0][ 8][30]=	    0;m_EtaROImax[0][ 8][30]=	     0;m_PhiROImin[0][ 8][30]=	      0;m_PhiROImax[0][ 8][30]=	       0;
     m_EtaROImin[0][ 8][31]=	    0;m_EtaROImax[0][ 8][31]=	     0;m_PhiROImin[0][ 8][31]=	      0;m_PhiROImax[0][ 8][31]=	       0;
     m_EtaROImin[0][ 9][ 0]=  -0.213185;m_EtaROImax[0][ 9][ 0]=  -0.116816;m_PhiROImin[0][ 9][ 0]=	1.87675;m_PhiROImax[0][ 9][ 0]=	 1.96071;
     m_EtaROImin[0][ 9][ 1]=  -0.118734;m_EtaROImax[0][ 9][ 1]= -0.0208251;m_PhiROImin[0][ 9][ 1]=	1.87675;m_PhiROImax[0][ 9][ 1]=	 1.96071;
     m_EtaROImin[0][ 9][ 2]=  -0.213185;m_EtaROImax[0][ 9][ 2]=  -0.116816;m_PhiROImin[0][ 9][ 2]=	1.79045;m_PhiROImax[0][ 9][ 2]=	 1.87312;
     m_EtaROImin[0][ 9][ 3]=  -0.118734;m_EtaROImax[0][ 9][ 3]= -0.0208251;m_PhiROImin[0][ 9][ 3]=	1.79045;m_PhiROImax[0][ 9][ 3]=	 1.87312;
     m_EtaROImin[0][ 9][ 4]=  -0.416721;m_EtaROImax[0][ 9][ 4]=   -0.30075;m_PhiROImin[0][ 9][ 4]=	1.87675;m_PhiROImax[0][ 9][ 4]=	 1.96071;
     m_EtaROImin[0][ 9][ 5]=  -0.302928;m_EtaROImax[0][ 9][ 5]=  -0.222449;m_PhiROImin[0][ 9][ 5]=	1.87675;m_PhiROImax[0][ 9][ 5]=	 1.96071;
     m_EtaROImin[0][ 9][ 6]=  -0.416721;m_EtaROImax[0][ 9][ 6]=   -0.30075;m_PhiROImin[0][ 9][ 6]=	1.79045;m_PhiROImax[0][ 9][ 6]=	 1.87312;
     m_EtaROImin[0][ 9][ 7]=  -0.302928;m_EtaROImax[0][ 9][ 7]=  -0.222449;m_PhiROImin[0][ 9][ 7]=	1.79045;m_PhiROImax[0][ 9][ 7]=	 1.87312;
     m_EtaROImin[0][ 9][ 8]=  -0.573871;m_EtaROImax[0][ 9][ 8]=  -0.501681;m_PhiROImin[0][ 9][ 8]=	1.87675;m_PhiROImax[0][ 9][ 8]=	 1.96071;
     m_EtaROImin[0][ 9][ 9]=  -0.504617;m_EtaROImax[0][ 9][ 9]=   -0.42967;m_PhiROImin[0][ 9][ 9]=	1.87675;m_PhiROImax[0][ 9][ 9]=	 1.96071;
     m_EtaROImin[0][ 9][10]=  -0.573871;m_EtaROImax[0][ 9][10]=  -0.501681;m_PhiROImin[0][ 9][10]=	1.79045;m_PhiROImax[0][ 9][10]=	 1.87312;
     m_EtaROImin[0][ 9][11]=  -0.504617;m_EtaROImax[0][ 9][11]=   -0.42967;m_PhiROImin[0][ 9][11]=	1.79045;m_PhiROImax[0][ 9][11]=	 1.87312;
     m_EtaROImin[0][ 9][12]=  -0.741516;m_EtaROImax[0][ 9][12]=  -0.649933;m_PhiROImin[0][ 9][12]=	1.87675;m_PhiROImax[0][ 9][12]=	 1.96071;
     m_EtaROImin[0][ 9][13]=  -0.653329;m_EtaROImax[0][ 9][13]=  -0.583785;m_PhiROImin[0][ 9][13]=	1.87675;m_PhiROImax[0][ 9][13]=	 1.96071;
     m_EtaROImin[0][ 9][14]=  -0.741516;m_EtaROImax[0][ 9][14]=  -0.649933;m_PhiROImin[0][ 9][14]=	1.79045;m_PhiROImax[0][ 9][14]=	 1.87312;
     m_EtaROImin[0][ 9][15]=  -0.653329;m_EtaROImax[0][ 9][15]=  -0.583785;m_PhiROImin[0][ 9][15]=	1.79045;m_PhiROImax[0][ 9][15]=	 1.87312;
     m_EtaROImin[0][ 9][16]=	    0;m_EtaROImax[0][ 9][16]=	     0;m_PhiROImin[0][ 9][16]=	      0;m_PhiROImax[0][ 9][16]=	       0;
     m_EtaROImin[0][ 9][17]=  -0.837822;m_EtaROImax[0][ 9][17]=  -0.756521;m_PhiROImin[0][ 9][17]=	1.87675;m_PhiROImax[0][ 9][17]=	 1.96071;
     m_EtaROImin[0][ 9][18]=	    0;m_EtaROImax[0][ 9][18]=	     0;m_PhiROImin[0][ 9][18]=	      0;m_PhiROImax[0][ 9][18]=	       0;
     m_EtaROImin[0][ 9][19]=  -0.837822;m_EtaROImax[0][ 9][19]=  -0.756521;m_PhiROImin[0][ 9][19]=	1.79045;m_PhiROImax[0][ 9][19]=	 1.87312;
     m_EtaROImin[0][ 9][20]=  -0.956037;m_EtaROImax[0][ 9][20]=  -0.899344;m_PhiROImin[0][ 9][20]=	1.87675;m_PhiROImax[0][ 9][20]=	 1.96071;
     m_EtaROImin[0][ 9][21]=  -0.903324;m_EtaROImax[0][ 9][21]=  -0.844116;m_PhiROImin[0][ 9][21]=	1.87675;m_PhiROImax[0][ 9][21]=	 1.96071;
     m_EtaROImin[0][ 9][22]=  -0.956037;m_EtaROImax[0][ 9][22]=  -0.899344;m_PhiROImin[0][ 9][22]=	1.79045;m_PhiROImax[0][ 9][22]=	 1.87312;
     m_EtaROImin[0][ 9][23]=  -0.903324;m_EtaROImax[0][ 9][23]=  -0.844116;m_PhiROImin[0][ 9][23]=	1.79045;m_PhiROImax[0][ 9][23]=	 1.87312;
     m_EtaROImin[0][ 9][24]=	    0;m_EtaROImax[0][ 9][24]=	     0;m_PhiROImin[0][ 9][24]=	      0;m_PhiROImax[0][ 9][24]=	       0;
     m_EtaROImin[0][ 9][25]=	    0;m_EtaROImax[0][ 9][25]=	     0;m_PhiROImin[0][ 9][25]=	      0;m_PhiROImax[0][ 9][25]=	       0;
     m_EtaROImin[0][ 9][26]=	    0;m_EtaROImax[0][ 9][26]=	     0;m_PhiROImin[0][ 9][26]=	      0;m_PhiROImax[0][ 9][26]=	       0;
     m_EtaROImin[0][ 9][27]=	    0;m_EtaROImax[0][ 9][27]=	     0;m_PhiROImin[0][ 9][27]=	      0;m_PhiROImax[0][ 9][27]=	       0;
     m_EtaROImin[0][ 9][28]=	    0;m_EtaROImax[0][ 9][28]=	     0;m_PhiROImin[0][ 9][28]=	      0;m_PhiROImax[0][ 9][28]=	       0;
     m_EtaROImin[0][ 9][29]=	    0;m_EtaROImax[0][ 9][29]=	     0;m_PhiROImin[0][ 9][29]=	      0;m_PhiROImax[0][ 9][29]=	       0;
     m_EtaROImin[0][ 9][30]=	    0;m_EtaROImax[0][ 9][30]=	     0;m_PhiROImin[0][ 9][30]=	      0;m_PhiROImax[0][ 9][30]=	       0;
     m_EtaROImin[0][ 9][31]=	    0;m_EtaROImax[0][ 9][31]=	     0;m_PhiROImin[0][ 9][31]=	      0;m_PhiROImax[0][ 9][31]=	       0;
     m_EtaROImin[0][10][ 0]=  -0.118734;m_EtaROImax[0][10][ 0]= -0.0208251;m_PhiROImin[0][10][ 0]=	1.96629;m_PhiROImax[0][10][ 0]=	 2.05024;
     m_EtaROImin[0][10][ 1]=  -0.213185;m_EtaROImax[0][10][ 1]=  -0.116816;m_PhiROImin[0][10][ 1]=	1.96629;m_PhiROImax[0][10][ 1]=	 2.05024;
     m_EtaROImin[0][10][ 2]=  -0.118734;m_EtaROImax[0][10][ 2]= -0.0208251;m_PhiROImin[0][10][ 2]=	2.05387;m_PhiROImax[0][10][ 2]=	 2.13655;
     m_EtaROImin[0][10][ 3]=  -0.213185;m_EtaROImax[0][10][ 3]=  -0.116816;m_PhiROImin[0][10][ 3]=	2.05387;m_PhiROImax[0][10][ 3]=	 2.13655;
     m_EtaROImin[0][10][ 4]=  -0.302928;m_EtaROImax[0][10][ 4]=  -0.222449;m_PhiROImin[0][10][ 4]=	1.96629;m_PhiROImax[0][10][ 4]=	 2.05024;
     m_EtaROImin[0][10][ 5]=  -0.416721;m_EtaROImax[0][10][ 5]=   -0.30075;m_PhiROImin[0][10][ 5]=	1.96629;m_PhiROImax[0][10][ 5]=	 2.05024;
     m_EtaROImin[0][10][ 6]=  -0.302928;m_EtaROImax[0][10][ 6]=  -0.222449;m_PhiROImin[0][10][ 6]=	2.05387;m_PhiROImax[0][10][ 6]=	 2.13655;
     m_EtaROImin[0][10][ 7]=  -0.416721;m_EtaROImax[0][10][ 7]=   -0.30075;m_PhiROImin[0][10][ 7]=	2.05387;m_PhiROImax[0][10][ 7]=	 2.13655;
     m_EtaROImin[0][10][ 8]=  -0.504617;m_EtaROImax[0][10][ 8]=   -0.42967;m_PhiROImin[0][10][ 8]=	1.96629;m_PhiROImax[0][10][ 8]=	 2.05024;
     m_EtaROImin[0][10][ 9]=  -0.573871;m_EtaROImax[0][10][ 9]=  -0.501681;m_PhiROImin[0][10][ 9]=	1.96629;m_PhiROImax[0][10][ 9]=	 2.05024;
     m_EtaROImin[0][10][10]=  -0.504617;m_EtaROImax[0][10][10]=   -0.42967;m_PhiROImin[0][10][10]=	2.05387;m_PhiROImax[0][10][10]=	 2.13655;
     m_EtaROImin[0][10][11]=  -0.573871;m_EtaROImax[0][10][11]=  -0.501681;m_PhiROImin[0][10][11]=	2.05387;m_PhiROImax[0][10][11]=	 2.13655;
     m_EtaROImin[0][10][12]=  -0.653329;m_EtaROImax[0][10][12]=  -0.583785;m_PhiROImin[0][10][12]=	1.96629;m_PhiROImax[0][10][12]=	 2.05024;
     m_EtaROImin[0][10][13]=  -0.741516;m_EtaROImax[0][10][13]=  -0.649933;m_PhiROImin[0][10][13]=	1.96629;m_PhiROImax[0][10][13]=	 2.05024;
     m_EtaROImin[0][10][14]=  -0.653329;m_EtaROImax[0][10][14]=  -0.583785;m_PhiROImin[0][10][14]=	2.05387;m_PhiROImax[0][10][14]=	 2.13655;
     m_EtaROImin[0][10][15]=  -0.741516;m_EtaROImax[0][10][15]=  -0.649933;m_PhiROImin[0][10][15]=	2.05387;m_PhiROImax[0][10][15]=	 2.13655;
     m_EtaROImin[0][10][16]=  -0.837822;m_EtaROImax[0][10][16]=  -0.756521;m_PhiROImin[0][10][16]=	1.96629;m_PhiROImax[0][10][16]=	 2.05024;
     m_EtaROImin[0][10][17]=	    0;m_EtaROImax[0][10][17]=	     0;m_PhiROImin[0][10][17]=	      0;m_PhiROImax[0][10][17]=	       0;
     m_EtaROImin[0][10][18]=  -0.837822;m_EtaROImax[0][10][18]=  -0.756521;m_PhiROImin[0][10][18]=	2.05387;m_PhiROImax[0][10][18]=	 2.13655;
     m_EtaROImin[0][10][19]=	    0;m_EtaROImax[0][10][19]=	     0;m_PhiROImin[0][10][19]=	      0;m_PhiROImax[0][10][19]=	       0;
     m_EtaROImin[0][10][20]=  -0.903324;m_EtaROImax[0][10][20]=  -0.844116;m_PhiROImin[0][10][20]=	1.96629;m_PhiROImax[0][10][20]=	 2.05024;
     m_EtaROImin[0][10][21]=  -0.956037;m_EtaROImax[0][10][21]=  -0.899344;m_PhiROImin[0][10][21]=	1.96629;m_PhiROImax[0][10][21]=	 2.05024;
     m_EtaROImin[0][10][22]=  -0.903324;m_EtaROImax[0][10][22]=  -0.844116;m_PhiROImin[0][10][22]=	2.05387;m_PhiROImax[0][10][22]=	 2.13655;
     m_EtaROImin[0][10][23]=  -0.956037;m_EtaROImax[0][10][23]=  -0.899344;m_PhiROImin[0][10][23]=	2.05387;m_PhiROImax[0][10][23]=	 2.13655;
     m_EtaROImin[0][10][24]=	    0;m_EtaROImax[0][10][24]=	     0;m_PhiROImin[0][10][24]=	      0;m_PhiROImax[0][10][24]=	       0;
     m_EtaROImin[0][10][25]=	    0;m_EtaROImax[0][10][25]=	     0;m_PhiROImin[0][10][25]=	      0;m_PhiROImax[0][10][25]=	       0;
     m_EtaROImin[0][10][26]=	    0;m_EtaROImax[0][10][26]=	     0;m_PhiROImin[0][10][26]=	      0;m_PhiROImax[0][10][26]=	       0;
     m_EtaROImin[0][10][27]=	    0;m_EtaROImax[0][10][27]=	     0;m_PhiROImin[0][10][27]=	      0;m_PhiROImax[0][10][27]=	       0;
     m_EtaROImin[0][10][28]=	    0;m_EtaROImax[0][10][28]=	     0;m_PhiROImin[0][10][28]=	      0;m_PhiROImax[0][10][28]=	       0;
     m_EtaROImin[0][10][29]=	    0;m_EtaROImax[0][10][29]=	     0;m_PhiROImin[0][10][29]=	      0;m_PhiROImax[0][10][29]=	       0;
     m_EtaROImin[0][10][30]=	    0;m_EtaROImax[0][10][30]=	     0;m_PhiROImin[0][10][30]=	      0;m_PhiROImax[0][10][30]=	       0;
     m_EtaROImin[0][10][31]=	    0;m_EtaROImax[0][10][31]=	     0;m_PhiROImin[0][10][31]=	      0;m_PhiROImax[0][10][31]=	       0;
     m_EtaROImin[0][11][ 0]=	    0;m_EtaROImax[0][11][ 0]=	     0;m_PhiROImin[0][11][ 0]=	      0;m_PhiROImax[0][11][ 0]=	       0;
     m_EtaROImin[0][11][ 1]=  -0.238612;m_EtaROImax[0][11][ 1]=  -0.130922;m_PhiROImin[0][11][ 1]=	2.24298;m_PhiROImax[0][11][ 1]=	 2.35323;
     m_EtaROImin[0][11][ 2]=	    0;m_EtaROImax[0][11][ 2]=	     0;m_PhiROImin[0][11][ 2]=	      0;m_PhiROImax[0][11][ 2]=	       0;
     m_EtaROImin[0][11][ 3]=  -0.238612;m_EtaROImax[0][11][ 3]=  -0.130922;m_PhiROImin[0][11][ 3]=	2.13203;m_PhiROImax[0][11][ 3]=	 2.23946;
     m_EtaROImin[0][11][ 4]=  -0.452099;m_EtaROImax[0][11][ 4]=  -0.350831;m_PhiROImin[0][11][ 4]=	2.24358;m_PhiROImax[0][11][ 4]=	 2.35324;
     m_EtaROImin[0][11][ 5]=   -0.35049;m_EtaROImax[0][11][ 5]=  -0.246207;m_PhiROImin[0][11][ 5]=	2.24358;m_PhiROImax[0][11][ 5]=	 2.35324;
     m_EtaROImin[0][11][ 6]=  -0.452099;m_EtaROImax[0][11][ 6]=  -0.350831;m_PhiROImin[0][11][ 6]=	2.13318;m_PhiROImax[0][11][ 6]=	 2.24008;
     m_EtaROImin[0][11][ 7]=   -0.35049;m_EtaROImax[0][11][ 7]=  -0.246207;m_PhiROImin[0][11][ 7]=	2.13318;m_PhiROImax[0][11][ 7]=	 2.24008;
     m_EtaROImin[0][11][ 8]=  -0.648101;m_EtaROImax[0][11][ 8]=  -0.555789;m_PhiROImin[0][11][ 8]=	2.24358;m_PhiROImax[0][11][ 8]=	 2.35324;
     m_EtaROImin[0][11][ 9]=  -0.554444;m_EtaROImax[0][11][ 9]=  -0.458091;m_PhiROImin[0][11][ 9]=	2.24358;m_PhiROImax[0][11][ 9]=	 2.35324;
     m_EtaROImin[0][11][10]=  -0.648101;m_EtaROImax[0][11][10]=  -0.555789;m_PhiROImin[0][11][10]=	2.13318;m_PhiROImax[0][11][10]=	 2.24008;
     m_EtaROImin[0][11][11]=  -0.554444;m_EtaROImax[0][11][11]=  -0.458091;m_PhiROImin[0][11][11]=	2.13318;m_PhiROImax[0][11][11]=	 2.24008;
     m_EtaROImin[0][11][12]=  -0.776449;m_EtaROImax[0][11][12]=  -0.731124;m_PhiROImin[0][11][12]=	2.24298;m_PhiROImax[0][11][12]=	 2.35323;
     m_EtaROImin[0][11][13]=  -0.728056;m_EtaROImax[0][11][13]=  -0.655857;m_PhiROImin[0][11][13]=	2.24298;m_PhiROImax[0][11][13]=	 2.35323;
     m_EtaROImin[0][11][14]=  -0.776449;m_EtaROImax[0][11][14]=  -0.731124;m_PhiROImin[0][11][14]=	2.13203;m_PhiROImax[0][11][14]=	 2.23946;
     m_EtaROImin[0][11][15]=  -0.728056;m_EtaROImax[0][11][15]=  -0.655857;m_PhiROImin[0][11][15]=	2.13203;m_PhiROImax[0][11][15]=	 2.23946;
     m_EtaROImin[0][11][16]=  -0.895163;m_EtaROImax[0][11][16]=  -0.853359;m_PhiROImin[0][11][16]=	2.24298;m_PhiROImax[0][11][16]=	 2.35323;
     m_EtaROImin[0][11][17]=  -0.850528;m_EtaROImax[0][11][17]=  -0.783867;m_PhiROImin[0][11][17]=	2.24298;m_PhiROImax[0][11][17]=	 2.35323;
     m_EtaROImin[0][11][18]=  -0.895163;m_EtaROImax[0][11][18]=  -0.853359;m_PhiROImin[0][11][18]=	2.13203;m_PhiROImax[0][11][18]=	 2.23946;
     m_EtaROImin[0][11][19]=  -0.850528;m_EtaROImax[0][11][19]=  -0.783867;m_PhiROImin[0][11][19]=	2.13203;m_PhiROImax[0][11][19]=	 2.23946;
     m_EtaROImin[0][11][20]=   -1.02514;m_EtaROImax[0][11][20]=  -0.964674;m_PhiROImin[0][11][20]=	2.24358;m_PhiROImax[0][11][20]=	 2.35324;
     m_EtaROImin[0][11][21]=  -0.961344;m_EtaROImax[0][11][21]=  -0.898201;m_PhiROImin[0][11][21]=	2.24358;m_PhiROImax[0][11][21]=	 2.35324;
     m_EtaROImin[0][11][22]=   -1.02514;m_EtaROImax[0][11][22]=  -0.964674;m_PhiROImin[0][11][22]=	2.13318;m_PhiROImax[0][11][22]=	 2.24008;
     m_EtaROImin[0][11][23]=  -0.961344;m_EtaROImax[0][11][23]=  -0.898201;m_PhiROImin[0][11][23]=	2.13318;m_PhiROImax[0][11][23]=	 2.24008;
     m_EtaROImin[0][11][24]=	    0;m_EtaROImax[0][11][24]=	     0;m_PhiROImin[0][11][24]=	      0;m_PhiROImax[0][11][24]=	       0;
     m_EtaROImin[0][11][25]=   -1.06547;m_EtaROImax[0][11][25]=   -1.03003;m_PhiROImin[0][11][25]=	2.24277;m_PhiROImax[0][11][25]=	 2.35313;
     m_EtaROImin[0][11][26]=	    0;m_EtaROImax[0][11][26]=	     0;m_PhiROImin[0][11][26]=	      0;m_PhiROImax[0][11][26]=	       0;
     m_EtaROImin[0][11][27]=   -1.06547;m_EtaROImax[0][11][27]=   -1.03003;m_PhiROImin[0][11][27]=	2.15905;m_PhiROImax[0][11][27]=	 2.23924;
     m_EtaROImin[0][11][28]=	    0;m_EtaROImax[0][11][28]=	     0;m_PhiROImin[0][11][28]=	      0;m_PhiROImax[0][11][28]=	       0;
     m_EtaROImin[0][11][29]=	    0;m_EtaROImax[0][11][29]=	     0;m_PhiROImin[0][11][29]=	      0;m_PhiROImax[0][11][29]=	       0;
     m_EtaROImin[0][11][30]=	    0;m_EtaROImax[0][11][30]=	     0;m_PhiROImin[0][11][30]=	      0;m_PhiROImax[0][11][30]=	       0;
     m_EtaROImin[0][11][31]=	    0;m_EtaROImax[0][11][31]=	     0;m_PhiROImin[0][11][31]=	      0;m_PhiROImax[0][11][31]=	       0;
     m_EtaROImin[0][12][ 0]=  -0.238612;m_EtaROImax[0][12][ 0]=  -0.130922;m_PhiROImin[0][12][ 0]=	2.35916;m_PhiROImax[0][12][ 0]=	 2.46941;
     m_EtaROImin[0][12][ 1]=	    0;m_EtaROImax[0][12][ 1]=	     0;m_PhiROImin[0][12][ 1]=	      0;m_PhiROImax[0][12][ 1]=	       0;
     m_EtaROImin[0][12][ 2]=  -0.238612;m_EtaROImax[0][12][ 2]=  -0.130922;m_PhiROImin[0][12][ 2]=	2.47293;m_PhiROImax[0][12][ 2]=	 2.58036;
     m_EtaROImin[0][12][ 3]=	    0;m_EtaROImax[0][12][ 3]=	     0;m_PhiROImin[0][12][ 3]=	      0;m_PhiROImax[0][12][ 3]=	       0;
     m_EtaROImin[0][12][ 4]=   -0.35049;m_EtaROImax[0][12][ 4]=  -0.246207;m_PhiROImin[0][12][ 4]=	2.35915;m_PhiROImax[0][12][ 4]=	 2.46881;
     m_EtaROImin[0][12][ 5]=  -0.452099;m_EtaROImax[0][12][ 5]=  -0.350831;m_PhiROImin[0][12][ 5]=	2.35915;m_PhiROImax[0][12][ 5]=	 2.46881;
     m_EtaROImin[0][12][ 6]=   -0.35049;m_EtaROImax[0][12][ 6]=  -0.246207;m_PhiROImin[0][12][ 6]=	2.47231;m_PhiROImax[0][12][ 6]=	 2.57921;
     m_EtaROImin[0][12][ 7]=  -0.452099;m_EtaROImax[0][12][ 7]=  -0.350831;m_PhiROImin[0][12][ 7]=	2.47231;m_PhiROImax[0][12][ 7]=	 2.57921;
     m_EtaROImin[0][12][ 8]=  -0.554444;m_EtaROImax[0][12][ 8]=  -0.458091;m_PhiROImin[0][12][ 8]=	2.35915;m_PhiROImax[0][12][ 8]=	 2.46881;
     m_EtaROImin[0][12][ 9]=  -0.648101;m_EtaROImax[0][12][ 9]=  -0.555789;m_PhiROImin[0][12][ 9]=	2.35915;m_PhiROImax[0][12][ 9]=	 2.46881;
     m_EtaROImin[0][12][10]=  -0.554444;m_EtaROImax[0][12][10]=  -0.458091;m_PhiROImin[0][12][10]=	2.47231;m_PhiROImax[0][12][10]=	 2.57921;
     m_EtaROImin[0][12][11]=  -0.648101;m_EtaROImax[0][12][11]=  -0.555789;m_PhiROImin[0][12][11]=	2.47231;m_PhiROImax[0][12][11]=	 2.57921;
     m_EtaROImin[0][12][12]=  -0.703298;m_EtaROImax[0][12][12]=  -0.655857;m_PhiROImin[0][12][12]=	2.35916;m_PhiROImax[0][12][12]=	 2.46941;
     m_EtaROImin[0][12][13]=  -0.776449;m_EtaROImax[0][12][13]=  -0.706413;m_PhiROImin[0][12][13]=	2.35916;m_PhiROImax[0][12][13]=	 2.46941;
     m_EtaROImin[0][12][14]=  -0.703298;m_EtaROImax[0][12][14]=  -0.655857;m_PhiROImin[0][12][14]=	2.47293;m_PhiROImax[0][12][14]=	 2.58036;
     m_EtaROImin[0][12][15]=  -0.776449;m_EtaROImax[0][12][15]=  -0.706413;m_PhiROImin[0][12][15]=	2.47293;m_PhiROImax[0][12][15]=	 2.58036;
     m_EtaROImin[0][12][16]=   -0.82768;m_EtaROImax[0][12][16]=  -0.783867;m_PhiROImin[0][12][16]=	2.35916;m_PhiROImax[0][12][16]=	 2.46941;
     m_EtaROImin[0][12][17]=  -0.895163;m_EtaROImax[0][12][17]=  -0.830556;m_PhiROImin[0][12][17]=	2.35916;m_PhiROImax[0][12][17]=	 2.46941;
     m_EtaROImin[0][12][18]=   -0.82768;m_EtaROImax[0][12][18]=  -0.783867;m_PhiROImin[0][12][18]=	2.47293;m_PhiROImax[0][12][18]=	 2.58036;
     m_EtaROImin[0][12][19]=  -0.895163;m_EtaROImax[0][12][19]=  -0.830556;m_PhiROImin[0][12][19]=	2.47293;m_PhiROImax[0][12][19]=	 2.58036;
     m_EtaROImin[0][12][20]=  -0.961344;m_EtaROImax[0][12][20]=  -0.898201;m_PhiROImin[0][12][20]=	2.35915;m_PhiROImax[0][12][20]=	 2.46881;
     m_EtaROImin[0][12][21]=   -1.02514;m_EtaROImax[0][12][21]=  -0.964674;m_PhiROImin[0][12][21]=	2.35915;m_PhiROImax[0][12][21]=	 2.46881;
     m_EtaROImin[0][12][22]=  -0.961344;m_EtaROImax[0][12][22]=  -0.898201;m_PhiROImin[0][12][22]=	2.47231;m_PhiROImax[0][12][22]=	 2.57921;
     m_EtaROImin[0][12][23]=   -1.02514;m_EtaROImax[0][12][23]=  -0.964674;m_PhiROImin[0][12][23]=	2.47231;m_PhiROImax[0][12][23]=	 2.57921;
     m_EtaROImin[0][12][24]=   -1.06547;m_EtaROImax[0][12][24]=   -1.03003;m_PhiROImin[0][12][24]=	2.35926;m_PhiROImax[0][12][24]=	 2.46962;
     m_EtaROImin[0][12][25]=	    0;m_EtaROImax[0][12][25]=	     0;m_PhiROImin[0][12][25]=	      0;m_PhiROImax[0][12][25]=	       0;
     m_EtaROImin[0][12][26]=   -1.06547;m_EtaROImax[0][12][26]=   -1.03003;m_PhiROImin[0][12][26]=	2.47315;m_PhiROImax[0][12][26]=	 2.55334;
     m_EtaROImin[0][12][27]=	    0;m_EtaROImax[0][12][27]=	     0;m_PhiROImin[0][12][27]=	      0;m_PhiROImax[0][12][27]=	       0;
     m_EtaROImin[0][12][28]=	    0;m_EtaROImax[0][12][28]=	     0;m_PhiROImin[0][12][28]=	      0;m_PhiROImax[0][12][28]=	       0;
     m_EtaROImin[0][12][29]=	    0;m_EtaROImax[0][12][29]=	     0;m_PhiROImin[0][12][29]=	      0;m_PhiROImax[0][12][29]=	       0;
     m_EtaROImin[0][12][30]=	    0;m_EtaROImax[0][12][30]=	     0;m_PhiROImin[0][12][30]=	      0;m_PhiROImax[0][12][30]=	       0;
     m_EtaROImin[0][12][31]=	    0;m_EtaROImax[0][12][31]=	     0;m_PhiROImin[0][12][31]=	      0;m_PhiROImax[0][12][31]=	       0;
     m_EtaROImin[0][13][ 0]=  -0.214185;m_EtaROImax[0][13][ 0]=  -0.117369;m_PhiROImin[0][13][ 0]=	2.66256;m_PhiROImax[0][13][ 0]=	 2.74612;
     m_EtaROImin[0][13][ 1]=  -0.118174;m_EtaROImax[0][13][ 1]= -0.0207264;m_PhiROImin[0][13][ 1]=	2.66256;m_PhiROImax[0][13][ 1]=	 2.74612;
     m_EtaROImin[0][13][ 2]=  -0.214185;m_EtaROImax[0][13][ 2]=  -0.117369;m_PhiROImin[0][13][ 2]=	2.57665;m_PhiROImax[0][13][ 2]=	 2.65895;
     m_EtaROImin[0][13][ 3]=  -0.118174;m_EtaROImax[0][13][ 3]= -0.0207264;m_PhiROImin[0][13][ 3]=	2.57665;m_PhiROImax[0][13][ 3]=	 2.65895;
     m_EtaROImin[0][13][ 4]=  -0.416721;m_EtaROImax[0][13][ 4]=   -0.30075;m_PhiROImin[0][13][ 4]=	2.66215;m_PhiROImax[0][13][ 4]=	  2.7461;
     m_EtaROImin[0][13][ 5]=  -0.302928;m_EtaROImax[0][13][ 5]=  -0.222449;m_PhiROImin[0][13][ 5]=	2.66215;m_PhiROImax[0][13][ 5]=	  2.7461;
     m_EtaROImin[0][13][ 6]=  -0.416721;m_EtaROImax[0][13][ 6]=   -0.30075;m_PhiROImin[0][13][ 6]=	2.57584;m_PhiROImax[0][13][ 6]=	 2.65852;
     m_EtaROImin[0][13][ 7]=  -0.302928;m_EtaROImax[0][13][ 7]=  -0.222449;m_PhiROImin[0][13][ 7]=	2.57584;m_PhiROImax[0][13][ 7]=	 2.65852;
     m_EtaROImin[0][13][ 8]=  -0.573871;m_EtaROImax[0][13][ 8]=  -0.501681;m_PhiROImin[0][13][ 8]=	2.66215;m_PhiROImax[0][13][ 8]=	  2.7461;
     m_EtaROImin[0][13][ 9]=  -0.504617;m_EtaROImax[0][13][ 9]=   -0.42967;m_PhiROImin[0][13][ 9]=	2.66215;m_PhiROImax[0][13][ 9]=	  2.7461;
     m_EtaROImin[0][13][10]=  -0.573871;m_EtaROImax[0][13][10]=  -0.501681;m_PhiROImin[0][13][10]=	2.57584;m_PhiROImax[0][13][10]=	 2.65852;
     m_EtaROImin[0][13][11]=  -0.504617;m_EtaROImax[0][13][11]=   -0.42967;m_PhiROImin[0][13][11]=	2.57584;m_PhiROImax[0][13][11]=	 2.65852;
     m_EtaROImin[0][13][12]=  -0.741516;m_EtaROImax[0][13][12]=  -0.649933;m_PhiROImin[0][13][12]=	2.66215;m_PhiROImax[0][13][12]=	  2.7461;
     m_EtaROImin[0][13][13]=  -0.653329;m_EtaROImax[0][13][13]=  -0.583785;m_PhiROImin[0][13][13]=	2.66215;m_PhiROImax[0][13][13]=	  2.7461;
     m_EtaROImin[0][13][14]=  -0.741516;m_EtaROImax[0][13][14]=  -0.649933;m_PhiROImin[0][13][14]=	2.57584;m_PhiROImax[0][13][14]=	 2.65852;
     m_EtaROImin[0][13][15]=  -0.653329;m_EtaROImax[0][13][15]=  -0.583785;m_PhiROImin[0][13][15]=	2.57584;m_PhiROImax[0][13][15]=	 2.65852;
     m_EtaROImin[0][13][16]=	    0;m_EtaROImax[0][13][16]=	     0;m_PhiROImin[0][13][16]=	      0;m_PhiROImax[0][13][16]=	       0;
     m_EtaROImin[0][13][17]=  -0.837822;m_EtaROImax[0][13][17]=  -0.756521;m_PhiROImin[0][13][17]=	2.66215;m_PhiROImax[0][13][17]=	  2.7461;
     m_EtaROImin[0][13][18]=	    0;m_EtaROImax[0][13][18]=	     0;m_PhiROImin[0][13][18]=	      0;m_PhiROImax[0][13][18]=	       0;
     m_EtaROImin[0][13][19]=  -0.837822;m_EtaROImax[0][13][19]=  -0.756521;m_PhiROImin[0][13][19]=	2.57584;m_PhiROImax[0][13][19]=	 2.65852;
     m_EtaROImin[0][13][20]=  -0.956037;m_EtaROImax[0][13][20]=  -0.899344;m_PhiROImin[0][13][20]=	2.66215;m_PhiROImax[0][13][20]=	  2.7461;
     m_EtaROImin[0][13][21]=  -0.903324;m_EtaROImax[0][13][21]=  -0.844116;m_PhiROImin[0][13][21]=	2.66215;m_PhiROImax[0][13][21]=	  2.7461;
     m_EtaROImin[0][13][22]=  -0.956037;m_EtaROImax[0][13][22]=  -0.899344;m_PhiROImin[0][13][22]=	2.57584;m_PhiROImax[0][13][22]=	 2.65852;
     m_EtaROImin[0][13][23]=  -0.903324;m_EtaROImax[0][13][23]=  -0.844116;m_PhiROImin[0][13][23]=	2.57584;m_PhiROImax[0][13][23]=	 2.65852;
     m_EtaROImin[0][13][24]=	    0;m_EtaROImax[0][13][24]=	     0;m_PhiROImin[0][13][24]=	      0;m_PhiROImax[0][13][24]=	       0;
     m_EtaROImin[0][13][25]=	    0;m_EtaROImax[0][13][25]=	     0;m_PhiROImin[0][13][25]=	      0;m_PhiROImax[0][13][25]=	       0;
     m_EtaROImin[0][13][26]=	    0;m_EtaROImax[0][13][26]=	     0;m_PhiROImin[0][13][26]=	      0;m_PhiROImax[0][13][26]=	       0;
     m_EtaROImin[0][13][27]=	    0;m_EtaROImax[0][13][27]=	     0;m_PhiROImin[0][13][27]=	      0;m_PhiROImax[0][13][27]=	       0;
     m_EtaROImin[0][13][28]=	    0;m_EtaROImax[0][13][28]=	     0;m_PhiROImin[0][13][28]=	      0;m_PhiROImax[0][13][28]=	       0;
     m_EtaROImin[0][13][29]=	    0;m_EtaROImax[0][13][29]=	     0;m_PhiROImin[0][13][29]=	      0;m_PhiROImax[0][13][29]=	       0;
     m_EtaROImin[0][13][30]=	    0;m_EtaROImax[0][13][30]=	     0;m_PhiROImin[0][13][30]=	      0;m_PhiROImax[0][13][30]=	       0;
     m_EtaROImin[0][13][31]=	    0;m_EtaROImax[0][13][31]=	     0;m_PhiROImin[0][13][31]=	      0;m_PhiROImax[0][13][31]=	       0;
     m_EtaROImin[0][14][ 0]=  -0.118174;m_EtaROImax[0][14][ 0]= -0.0207264;m_PhiROImin[0][14][ 0]=	2.75167;m_PhiROImax[0][14][ 0]=	 2.83523;
     m_EtaROImin[0][14][ 1]=  -0.214185;m_EtaROImax[0][14][ 1]=  -0.117369;m_PhiROImin[0][14][ 1]=	2.75167;m_PhiROImax[0][14][ 1]=	 2.83523;
     m_EtaROImin[0][14][ 2]=  -0.118174;m_EtaROImax[0][14][ 2]= -0.0207264;m_PhiROImin[0][14][ 2]=	2.83884;m_PhiROImax[0][14][ 2]=	 2.92113;
     m_EtaROImin[0][14][ 3]=  -0.214185;m_EtaROImax[0][14][ 3]=  -0.117369;m_PhiROImin[0][14][ 3]=	2.83884;m_PhiROImax[0][14][ 3]=	 2.92113;
     m_EtaROImin[0][14][ 4]=  -0.302928;m_EtaROImax[0][14][ 4]=  -0.222449;m_PhiROImin[0][14][ 4]=	2.75168;m_PhiROImax[0][14][ 4]=	 2.83564;
     m_EtaROImin[0][14][ 5]=  -0.416721;m_EtaROImax[0][14][ 5]=   -0.30075;m_PhiROImin[0][14][ 5]=	2.75168;m_PhiROImax[0][14][ 5]=	 2.83564;
     m_EtaROImin[0][14][ 6]=  -0.302928;m_EtaROImax[0][14][ 6]=  -0.222449;m_PhiROImin[0][14][ 6]=	2.83927;m_PhiROImax[0][14][ 6]=	 2.92194;
     m_EtaROImin[0][14][ 7]=  -0.416721;m_EtaROImax[0][14][ 7]=   -0.30075;m_PhiROImin[0][14][ 7]=	2.83927;m_PhiROImax[0][14][ 7]=	 2.92194;
     m_EtaROImin[0][14][ 8]=  -0.504617;m_EtaROImax[0][14][ 8]=   -0.42967;m_PhiROImin[0][14][ 8]=	2.75168;m_PhiROImax[0][14][ 8]=	 2.83564;
     m_EtaROImin[0][14][ 9]=  -0.573871;m_EtaROImax[0][14][ 9]=  -0.501681;m_PhiROImin[0][14][ 9]=	2.75168;m_PhiROImax[0][14][ 9]=	 2.83564;
     m_EtaROImin[0][14][10]=  -0.504617;m_EtaROImax[0][14][10]=   -0.42967;m_PhiROImin[0][14][10]=	2.83927;m_PhiROImax[0][14][10]=	 2.92194;
     m_EtaROImin[0][14][11]=  -0.573871;m_EtaROImax[0][14][11]=  -0.501681;m_PhiROImin[0][14][11]=	2.83927;m_PhiROImax[0][14][11]=	 2.92194;
     m_EtaROImin[0][14][12]=  -0.653329;m_EtaROImax[0][14][12]=  -0.583785;m_PhiROImin[0][14][12]=	2.75168;m_PhiROImax[0][14][12]=	 2.83564;
     m_EtaROImin[0][14][13]=  -0.741516;m_EtaROImax[0][14][13]=  -0.649933;m_PhiROImin[0][14][13]=	2.75168;m_PhiROImax[0][14][13]=	 2.83564;
     m_EtaROImin[0][14][14]=  -0.653329;m_EtaROImax[0][14][14]=  -0.583785;m_PhiROImin[0][14][14]=	2.83927;m_PhiROImax[0][14][14]=	 2.92194;
     m_EtaROImin[0][14][15]=  -0.741516;m_EtaROImax[0][14][15]=  -0.649933;m_PhiROImin[0][14][15]=	2.83927;m_PhiROImax[0][14][15]=	 2.92194;
     m_EtaROImin[0][14][16]=  -0.837822;m_EtaROImax[0][14][16]=  -0.756521;m_PhiROImin[0][14][16]=	2.75168;m_PhiROImax[0][14][16]=	 2.83564;
     m_EtaROImin[0][14][17]=	    0;m_EtaROImax[0][14][17]=	     0;m_PhiROImin[0][14][17]=	      0;m_PhiROImax[0][14][17]=	       0;
     m_EtaROImin[0][14][18]=  -0.837822;m_EtaROImax[0][14][18]=  -0.756521;m_PhiROImin[0][14][18]=	2.83927;m_PhiROImax[0][14][18]=	 2.92194;
     m_EtaROImin[0][14][19]=	    0;m_EtaROImax[0][14][19]=	     0;m_PhiROImin[0][14][19]=	      0;m_PhiROImax[0][14][19]=	       0;
     m_EtaROImin[0][14][20]=  -0.903324;m_EtaROImax[0][14][20]=  -0.844116;m_PhiROImin[0][14][20]=	2.75168;m_PhiROImax[0][14][20]=	 2.83564;
     m_EtaROImin[0][14][21]=  -0.956037;m_EtaROImax[0][14][21]=  -0.899344;m_PhiROImin[0][14][21]=	2.75168;m_PhiROImax[0][14][21]=	 2.83564;
     m_EtaROImin[0][14][22]=  -0.903324;m_EtaROImax[0][14][22]=  -0.844116;m_PhiROImin[0][14][22]=	2.83927;m_PhiROImax[0][14][22]=	 2.92194;
     m_EtaROImin[0][14][23]=  -0.956037;m_EtaROImax[0][14][23]=  -0.899344;m_PhiROImin[0][14][23]=	2.83927;m_PhiROImax[0][14][23]=	 2.92194;
     m_EtaROImin[0][14][24]=	    0;m_EtaROImax[0][14][24]=	     0;m_PhiROImin[0][14][24]=	      0;m_PhiROImax[0][14][24]=	       0;
     m_EtaROImin[0][14][25]=	    0;m_EtaROImax[0][14][25]=	     0;m_PhiROImin[0][14][25]=	      0;m_PhiROImax[0][14][25]=	       0;
     m_EtaROImin[0][14][26]=	    0;m_EtaROImax[0][14][26]=	     0;m_PhiROImin[0][14][26]=	      0;m_PhiROImax[0][14][26]=	       0;
     m_EtaROImin[0][14][27]=	    0;m_EtaROImax[0][14][27]=	     0;m_PhiROImin[0][14][27]=	      0;m_PhiROImax[0][14][27]=	       0;
     m_EtaROImin[0][14][28]=	    0;m_EtaROImax[0][14][28]=	     0;m_PhiROImin[0][14][28]=	      0;m_PhiROImax[0][14][28]=	       0;
     m_EtaROImin[0][14][29]=	    0;m_EtaROImax[0][14][29]=	     0;m_PhiROImin[0][14][29]=	      0;m_PhiROImax[0][14][29]=	       0;
     m_EtaROImin[0][14][30]=	    0;m_EtaROImax[0][14][30]=	     0;m_PhiROImin[0][14][30]=	      0;m_PhiROImax[0][14][30]=	       0;
     m_EtaROImin[0][14][31]=	    0;m_EtaROImax[0][14][31]=	     0;m_PhiROImin[0][14][31]=	      0;m_PhiROImax[0][14][31]=	       0;
     m_EtaROImin[0][15][ 0]=  -0.238476;m_EtaROImax[0][15][ 0]=  -0.180926;m_PhiROImin[0][15][ 0]=	3.02838;m_PhiROImax[0][15][ 0]=	 3.13862;
     m_EtaROImin[0][15][ 1]=  -0.177066;m_EtaROImax[0][15][ 1]= -0.0876766;m_PhiROImin[0][15][ 1]=	3.02838;m_PhiROImax[0][15][ 1]=	 3.13862;
     m_EtaROImin[0][15][ 2]=  -0.238476;m_EtaROImax[0][15][ 2]=  -0.180926;m_PhiROImin[0][15][ 2]=	2.91742;m_PhiROImax[0][15][ 2]=	 3.02486;
     m_EtaROImin[0][15][ 3]=  -0.177066;m_EtaROImax[0][15][ 3]= -0.0876766;m_PhiROImin[0][15][ 3]=	2.91742;m_PhiROImax[0][15][ 3]=	 3.02486;
     m_EtaROImin[0][15][ 4]=  -0.452099;m_EtaROImax[0][15][ 4]=  -0.350831;m_PhiROImin[0][15][ 4]=	3.02898;m_PhiROImax[0][15][ 4]=	 3.13864;
     m_EtaROImin[0][15][ 5]=   -0.35049;m_EtaROImax[0][15][ 5]=  -0.246207;m_PhiROImin[0][15][ 5]=	3.02898;m_PhiROImax[0][15][ 5]=	 3.13864;
     m_EtaROImin[0][15][ 6]=  -0.452099;m_EtaROImax[0][15][ 6]=  -0.350831;m_PhiROImin[0][15][ 6]=	2.91858;m_PhiROImax[0][15][ 6]=	 3.02547;
     m_EtaROImin[0][15][ 7]=   -0.35049;m_EtaROImax[0][15][ 7]=  -0.246207;m_PhiROImin[0][15][ 7]=	2.91858;m_PhiROImax[0][15][ 7]=	 3.02547;
     m_EtaROImin[0][15][ 8]=  -0.648101;m_EtaROImax[0][15][ 8]=  -0.555789;m_PhiROImin[0][15][ 8]=	3.02898;m_PhiROImax[0][15][ 8]=	 3.13864;
     m_EtaROImin[0][15][ 9]=  -0.554444;m_EtaROImax[0][15][ 9]=  -0.458091;m_PhiROImin[0][15][ 9]=	3.02898;m_PhiROImax[0][15][ 9]=	 3.13864;
     m_EtaROImin[0][15][10]=  -0.648101;m_EtaROImax[0][15][10]=  -0.555789;m_PhiROImin[0][15][10]=	2.91858;m_PhiROImax[0][15][10]=	 3.02547;
     m_EtaROImin[0][15][11]=  -0.554444;m_EtaROImax[0][15][11]=  -0.458091;m_PhiROImin[0][15][11]=	2.91858;m_PhiROImax[0][15][11]=	 3.02547;
     m_EtaROImin[0][15][12]=  -0.776449;m_EtaROImax[0][15][12]=  -0.731124;m_PhiROImin[0][15][12]=	3.02838;m_PhiROImax[0][15][12]=	 3.13862;
     m_EtaROImin[0][15][13]=  -0.728056;m_EtaROImax[0][15][13]=  -0.655857;m_PhiROImin[0][15][13]=	3.02838;m_PhiROImax[0][15][13]=	 3.13862;
     m_EtaROImin[0][15][14]=  -0.776449;m_EtaROImax[0][15][14]=  -0.731124;m_PhiROImin[0][15][14]=	2.91742;m_PhiROImax[0][15][14]=	 3.02486;
     m_EtaROImin[0][15][15]=  -0.728056;m_EtaROImax[0][15][15]=  -0.655857;m_PhiROImin[0][15][15]=	2.91742;m_PhiROImax[0][15][15]=	 3.02486;
     m_EtaROImin[0][15][16]=  -0.895163;m_EtaROImax[0][15][16]=  -0.853359;m_PhiROImin[0][15][16]=	3.02838;m_PhiROImax[0][15][16]=	 3.13862;
     m_EtaROImin[0][15][17]=  -0.850528;m_EtaROImax[0][15][17]=  -0.783867;m_PhiROImin[0][15][17]=	3.02838;m_PhiROImax[0][15][17]=	 3.13862;
     m_EtaROImin[0][15][18]=  -0.895163;m_EtaROImax[0][15][18]=  -0.853359;m_PhiROImin[0][15][18]=	2.91742;m_PhiROImax[0][15][18]=	 3.02486;
     m_EtaROImin[0][15][19]=  -0.850528;m_EtaROImax[0][15][19]=  -0.783867;m_PhiROImin[0][15][19]=	2.91742;m_PhiROImax[0][15][19]=	 3.02486;
     m_EtaROImin[0][15][20]=   -1.02514;m_EtaROImax[0][15][20]=  -0.964674;m_PhiROImin[0][15][20]=	3.02898;m_PhiROImax[0][15][20]=	 3.13864;
     m_EtaROImin[0][15][21]=  -0.961344;m_EtaROImax[0][15][21]=  -0.898201;m_PhiROImin[0][15][21]=	3.02898;m_PhiROImax[0][15][21]=	 3.13864;
     m_EtaROImin[0][15][22]=   -1.02514;m_EtaROImax[0][15][22]=  -0.964674;m_PhiROImin[0][15][22]=	2.91858;m_PhiROImax[0][15][22]=	 3.02547;
     m_EtaROImin[0][15][23]=  -0.961344;m_EtaROImax[0][15][23]=  -0.898201;m_PhiROImin[0][15][23]=	2.91858;m_PhiROImax[0][15][23]=	 3.02547;
     m_EtaROImin[0][15][24]=	    0;m_EtaROImax[0][15][24]=	     0;m_PhiROImin[0][15][24]=	      0;m_PhiROImax[0][15][24]=	       0;
     m_EtaROImin[0][15][25]=   -1.06547;m_EtaROImax[0][15][25]=   -1.03003;m_PhiROImin[0][15][25]=	3.02816;m_PhiROImax[0][15][25]=	 3.13852;
     m_EtaROImin[0][15][26]=	    0;m_EtaROImax[0][15][26]=	     0;m_PhiROImin[0][15][26]=	      0;m_PhiROImax[0][15][26]=	       0;
     m_EtaROImin[0][15][27]=   -1.06547;m_EtaROImax[0][15][27]=   -1.03003;m_PhiROImin[0][15][27]=	2.94445;m_PhiROImax[0][15][27]=	 3.02463;
     m_EtaROImin[0][15][28]=	    0;m_EtaROImax[0][15][28]=	     0;m_PhiROImin[0][15][28]=	      0;m_PhiROImax[0][15][28]=	       0;
     m_EtaROImin[0][15][29]=	    0;m_EtaROImax[0][15][29]=	     0;m_PhiROImin[0][15][29]=	      0;m_PhiROImax[0][15][29]=	       0;
     m_EtaROImin[0][15][30]=	    0;m_EtaROImax[0][15][30]=	     0;m_PhiROImin[0][15][30]=	      0;m_PhiROImax[0][15][30]=	       0;
     m_EtaROImin[0][15][31]=	    0;m_EtaROImax[0][15][31]=	     0;m_PhiROImin[0][15][31]=	      0;m_PhiROImax[0][15][31]=	       0;
     m_EtaROImin[0][16][ 0]=  -0.146097;m_EtaROImax[0][16][ 0]= -0.0876766;m_PhiROImin[0][16][ 0]=   -3.13862;m_PhiROImax[0][16][ 0]=	-3.02838;
     m_EtaROImin[0][16][ 1]=  -0.238476;m_EtaROImax[0][16][ 1]=  -0.149976;m_PhiROImin[0][16][ 1]=   -3.13862;m_PhiROImax[0][16][ 1]=	-3.02838;
     m_EtaROImin[0][16][ 2]=  -0.146097;m_EtaROImax[0][16][ 2]= -0.0876766;m_PhiROImin[0][16][ 2]=   -3.02486;m_PhiROImax[0][16][ 2]=	-2.91742;
     m_EtaROImin[0][16][ 3]=  -0.238476;m_EtaROImax[0][16][ 3]=  -0.149976;m_PhiROImin[0][16][ 3]=   -3.02486;m_PhiROImax[0][16][ 3]=	-2.91742;
     m_EtaROImin[0][16][ 4]=   -0.35049;m_EtaROImax[0][16][ 4]=  -0.246207;m_PhiROImin[0][16][ 4]=   -3.13864;m_PhiROImax[0][16][ 4]=	-3.02898;
     m_EtaROImin[0][16][ 5]=  -0.452099;m_EtaROImax[0][16][ 5]=  -0.350831;m_PhiROImin[0][16][ 5]=   -3.13864;m_PhiROImax[0][16][ 5]=	-3.02898;
     m_EtaROImin[0][16][ 6]=   -0.35049;m_EtaROImax[0][16][ 6]=  -0.246207;m_PhiROImin[0][16][ 6]=   -3.02547;m_PhiROImax[0][16][ 6]=	-2.91858;
     m_EtaROImin[0][16][ 7]=  -0.452099;m_EtaROImax[0][16][ 7]=  -0.350831;m_PhiROImin[0][16][ 7]=   -3.02547;m_PhiROImax[0][16][ 7]=	-2.91858;
     m_EtaROImin[0][16][ 8]=  -0.554444;m_EtaROImax[0][16][ 8]=  -0.458091;m_PhiROImin[0][16][ 8]=   -3.13864;m_PhiROImax[0][16][ 8]=	-3.02898;
     m_EtaROImin[0][16][ 9]=  -0.648101;m_EtaROImax[0][16][ 9]=  -0.555789;m_PhiROImin[0][16][ 9]=   -3.13864;m_PhiROImax[0][16][ 9]=	-3.02898;
     m_EtaROImin[0][16][10]=  -0.554444;m_EtaROImax[0][16][10]=  -0.458091;m_PhiROImin[0][16][10]=   -3.02547;m_PhiROImax[0][16][10]=	-2.91858;
     m_EtaROImin[0][16][11]=  -0.648101;m_EtaROImax[0][16][11]=  -0.555789;m_PhiROImin[0][16][11]=   -3.02547;m_PhiROImax[0][16][11]=	-2.91858;
     m_EtaROImin[0][16][12]=  -0.703298;m_EtaROImax[0][16][12]=  -0.655857;m_PhiROImin[0][16][12]=   -3.13862;m_PhiROImax[0][16][12]=	-3.02838;
     m_EtaROImin[0][16][13]=  -0.776449;m_EtaROImax[0][16][13]=  -0.706413;m_PhiROImin[0][16][13]=   -3.13862;m_PhiROImax[0][16][13]=	-3.02838;
     m_EtaROImin[0][16][14]=  -0.703298;m_EtaROImax[0][16][14]=  -0.655857;m_PhiROImin[0][16][14]=   -3.02486;m_PhiROImax[0][16][14]=	-2.91742;
     m_EtaROImin[0][16][15]=  -0.776449;m_EtaROImax[0][16][15]=  -0.706413;m_PhiROImin[0][16][15]=   -3.02486;m_PhiROImax[0][16][15]=	-2.91742;
     m_EtaROImin[0][16][16]=   -0.82768;m_EtaROImax[0][16][16]=  -0.783867;m_PhiROImin[0][16][16]=   -3.13862;m_PhiROImax[0][16][16]=	-3.02838;
     m_EtaROImin[0][16][17]=  -0.895163;m_EtaROImax[0][16][17]=  -0.830556;m_PhiROImin[0][16][17]=   -3.13862;m_PhiROImax[0][16][17]=	-3.02838;
     m_EtaROImin[0][16][18]=   -0.82768;m_EtaROImax[0][16][18]=  -0.783867;m_PhiROImin[0][16][18]=   -3.02486;m_PhiROImax[0][16][18]=	-2.91742;
     m_EtaROImin[0][16][19]=  -0.895163;m_EtaROImax[0][16][19]=  -0.830556;m_PhiROImin[0][16][19]=   -3.02486;m_PhiROImax[0][16][19]=	-2.91742;
     m_EtaROImin[0][16][20]=  -0.961344;m_EtaROImax[0][16][20]=  -0.898201;m_PhiROImin[0][16][20]=   -3.13864;m_PhiROImax[0][16][20]=	-3.02898;
     m_EtaROImin[0][16][21]=   -1.02514;m_EtaROImax[0][16][21]=  -0.964674;m_PhiROImin[0][16][21]=   -3.13864;m_PhiROImax[0][16][21]=	-3.02898;
     m_EtaROImin[0][16][22]=  -0.961344;m_EtaROImax[0][16][22]=  -0.898201;m_PhiROImin[0][16][22]=   -3.02547;m_PhiROImax[0][16][22]=	-2.91858;
     m_EtaROImin[0][16][23]=   -1.02514;m_EtaROImax[0][16][23]=  -0.964674;m_PhiROImin[0][16][23]=   -3.02547;m_PhiROImax[0][16][23]=	-2.91858;
     m_EtaROImin[0][16][24]=   -1.06547;m_EtaROImax[0][16][24]=   -1.03003;m_PhiROImin[0][16][24]=   -3.13852;m_PhiROImax[0][16][24]=	-3.02816;
     m_EtaROImin[0][16][25]=	    0;m_EtaROImax[0][16][25]=	     0;m_PhiROImin[0][16][25]=	      0;m_PhiROImax[0][16][25]=	       0;
     m_EtaROImin[0][16][26]=   -1.06547;m_EtaROImax[0][16][26]=   -1.03003;m_PhiROImin[0][16][26]=   -3.02463;m_PhiROImax[0][16][26]=	-2.94445;
     m_EtaROImin[0][16][27]=	    0;m_EtaROImax[0][16][27]=	     0;m_PhiROImin[0][16][27]=	      0;m_PhiROImax[0][16][27]=	       0;
     m_EtaROImin[0][16][28]=	    0;m_EtaROImax[0][16][28]=	     0;m_PhiROImin[0][16][28]=	      0;m_PhiROImax[0][16][28]=	       0;
     m_EtaROImin[0][16][29]=	    0;m_EtaROImax[0][16][29]=	     0;m_PhiROImin[0][16][29]=	      0;m_PhiROImax[0][16][29]=	       0;
     m_EtaROImin[0][16][30]=	    0;m_EtaROImax[0][16][30]=	     0;m_PhiROImin[0][16][30]=	      0;m_PhiROImax[0][16][30]=	       0;
     m_EtaROImin[0][16][31]=	    0;m_EtaROImax[0][16][31]=	     0;m_PhiROImin[0][16][31]=	      0;m_PhiROImax[0][16][31]=	       0;
     m_EtaROImin[0][17][ 0]=  -0.213185;m_EtaROImax[0][17][ 0]=  -0.116816;m_PhiROImin[0][17][ 0]=   -2.83564;m_PhiROImax[0][17][ 0]=	-2.75168;
     m_EtaROImin[0][17][ 1]=  -0.118734;m_EtaROImax[0][17][ 1]= -0.0208251;m_PhiROImin[0][17][ 1]=   -2.83564;m_PhiROImax[0][17][ 1]=	-2.75168;
     m_EtaROImin[0][17][ 2]=  -0.213185;m_EtaROImax[0][17][ 2]=  -0.116816;m_PhiROImin[0][17][ 2]=   -2.92194;m_PhiROImax[0][17][ 2]=	-2.83927;
     m_EtaROImin[0][17][ 3]=  -0.118734;m_EtaROImax[0][17][ 3]= -0.0208251;m_PhiROImin[0][17][ 3]=   -2.92194;m_PhiROImax[0][17][ 3]=	-2.83927;
     m_EtaROImin[0][17][ 4]=  -0.416721;m_EtaROImax[0][17][ 4]=   -0.30075;m_PhiROImin[0][17][ 4]=   -2.83564;m_PhiROImax[0][17][ 4]=	-2.75168;
     m_EtaROImin[0][17][ 5]=  -0.302928;m_EtaROImax[0][17][ 5]=  -0.222449;m_PhiROImin[0][17][ 5]=   -2.83564;m_PhiROImax[0][17][ 5]=	-2.75168;
     m_EtaROImin[0][17][ 6]=  -0.416721;m_EtaROImax[0][17][ 6]=   -0.30075;m_PhiROImin[0][17][ 6]=   -2.92194;m_PhiROImax[0][17][ 6]=	-2.83927;
     m_EtaROImin[0][17][ 7]=  -0.302928;m_EtaROImax[0][17][ 7]=  -0.222449;m_PhiROImin[0][17][ 7]=   -2.92194;m_PhiROImax[0][17][ 7]=	-2.83927;
     m_EtaROImin[0][17][ 8]=  -0.573871;m_EtaROImax[0][17][ 8]=  -0.501681;m_PhiROImin[0][17][ 8]=   -2.83564;m_PhiROImax[0][17][ 8]=	-2.75168;
     m_EtaROImin[0][17][ 9]=  -0.504617;m_EtaROImax[0][17][ 9]=   -0.42967;m_PhiROImin[0][17][ 9]=   -2.83564;m_PhiROImax[0][17][ 9]=	-2.75168;
     m_EtaROImin[0][17][10]=  -0.573871;m_EtaROImax[0][17][10]=  -0.501681;m_PhiROImin[0][17][10]=   -2.92194;m_PhiROImax[0][17][10]=	-2.83927;
     m_EtaROImin[0][17][11]=  -0.504617;m_EtaROImax[0][17][11]=   -0.42967;m_PhiROImin[0][17][11]=   -2.92194;m_PhiROImax[0][17][11]=	-2.83927;
     m_EtaROImin[0][17][12]=  -0.741516;m_EtaROImax[0][17][12]=  -0.649933;m_PhiROImin[0][17][12]=   -2.83564;m_PhiROImax[0][17][12]=	-2.75168;
     m_EtaROImin[0][17][13]=  -0.653329;m_EtaROImax[0][17][13]=  -0.583785;m_PhiROImin[0][17][13]=   -2.83564;m_PhiROImax[0][17][13]=	-2.75168;
     m_EtaROImin[0][17][14]=  -0.741516;m_EtaROImax[0][17][14]=  -0.649933;m_PhiROImin[0][17][14]=   -2.92194;m_PhiROImax[0][17][14]=	-2.83927;
     m_EtaROImin[0][17][15]=  -0.653329;m_EtaROImax[0][17][15]=  -0.583785;m_PhiROImin[0][17][15]=   -2.92194;m_PhiROImax[0][17][15]=	-2.83927;
     m_EtaROImin[0][17][16]=	    0;m_EtaROImax[0][17][16]=	     0;m_PhiROImin[0][17][16]=	      0;m_PhiROImax[0][17][16]=	       0;
     m_EtaROImin[0][17][17]=  -0.837822;m_EtaROImax[0][17][17]=  -0.756521;m_PhiROImin[0][17][17]=   -2.83564;m_PhiROImax[0][17][17]=	-2.75168;
     m_EtaROImin[0][17][18]=	    0;m_EtaROImax[0][17][18]=	     0;m_PhiROImin[0][17][18]=	      0;m_PhiROImax[0][17][18]=	       0;
     m_EtaROImin[0][17][19]=  -0.837822;m_EtaROImax[0][17][19]=  -0.756521;m_PhiROImin[0][17][19]=   -2.92194;m_PhiROImax[0][17][19]=	-2.83927;
     m_EtaROImin[0][17][20]=  -0.956037;m_EtaROImax[0][17][20]=  -0.899344;m_PhiROImin[0][17][20]=   -2.83564;m_PhiROImax[0][17][20]=	-2.75168;
     m_EtaROImin[0][17][21]=  -0.903324;m_EtaROImax[0][17][21]=  -0.844116;m_PhiROImin[0][17][21]=   -2.83564;m_PhiROImax[0][17][21]=	-2.75168;
     m_EtaROImin[0][17][22]=  -0.956037;m_EtaROImax[0][17][22]=  -0.899344;m_PhiROImin[0][17][22]=   -2.92194;m_PhiROImax[0][17][22]=	-2.83927;
     m_EtaROImin[0][17][23]=  -0.903324;m_EtaROImax[0][17][23]=  -0.844116;m_PhiROImin[0][17][23]=   -2.92194;m_PhiROImax[0][17][23]=	-2.83927;
     m_EtaROImin[0][17][24]=	    0;m_EtaROImax[0][17][24]=	     0;m_PhiROImin[0][17][24]=	      0;m_PhiROImax[0][17][24]=	       0;
     m_EtaROImin[0][17][25]=	    0;m_EtaROImax[0][17][25]=	     0;m_PhiROImin[0][17][25]=	      0;m_PhiROImax[0][17][25]=	       0;
     m_EtaROImin[0][17][26]=	    0;m_EtaROImax[0][17][26]=	     0;m_PhiROImin[0][17][26]=	      0;m_PhiROImax[0][17][26]=	       0;
     m_EtaROImin[0][17][27]=	    0;m_EtaROImax[0][17][27]=	     0;m_PhiROImin[0][17][27]=	      0;m_PhiROImax[0][17][27]=	       0;
     m_EtaROImin[0][17][28]=	    0;m_EtaROImax[0][17][28]=	     0;m_PhiROImin[0][17][28]=	      0;m_PhiROImax[0][17][28]=	       0;
     m_EtaROImin[0][17][29]=	    0;m_EtaROImax[0][17][29]=	     0;m_PhiROImin[0][17][29]=	      0;m_PhiROImax[0][17][29]=	       0;
     m_EtaROImin[0][17][30]=	    0;m_EtaROImax[0][17][30]=	     0;m_PhiROImin[0][17][30]=	      0;m_PhiROImax[0][17][30]=	       0;
     m_EtaROImin[0][17][31]=	    0;m_EtaROImax[0][17][31]=	     0;m_PhiROImin[0][17][31]=	      0;m_PhiROImax[0][17][31]=	       0;
     m_EtaROImin[0][18][ 0]=  -0.118734;m_EtaROImax[0][18][ 0]= -0.0208251;m_PhiROImin[0][18][ 0]=	-2.7461;m_PhiROImax[0][18][ 0]=	-2.66215;
     m_EtaROImin[0][18][ 1]=  -0.213185;m_EtaROImax[0][18][ 1]=  -0.116816;m_PhiROImin[0][18][ 1]=	-2.7461;m_PhiROImax[0][18][ 1]=	-2.66215;
     m_EtaROImin[0][18][ 2]=  -0.118734;m_EtaROImax[0][18][ 2]= -0.0208251;m_PhiROImin[0][18][ 2]=   -2.65852;m_PhiROImax[0][18][ 2]=	-2.57584;
     m_EtaROImin[0][18][ 3]=  -0.213185;m_EtaROImax[0][18][ 3]=  -0.116816;m_PhiROImin[0][18][ 3]=   -2.65852;m_PhiROImax[0][18][ 3]=	-2.57584;
     m_EtaROImin[0][18][ 4]=  -0.302928;m_EtaROImax[0][18][ 4]=  -0.222449;m_PhiROImin[0][18][ 4]=	-2.7461;m_PhiROImax[0][18][ 4]=	-2.66215;
     m_EtaROImin[0][18][ 5]=  -0.416721;m_EtaROImax[0][18][ 5]=   -0.30075;m_PhiROImin[0][18][ 5]=	-2.7461;m_PhiROImax[0][18][ 5]=	-2.66215;
     m_EtaROImin[0][18][ 6]=  -0.302928;m_EtaROImax[0][18][ 6]=  -0.222449;m_PhiROImin[0][18][ 6]=   -2.65852;m_PhiROImax[0][18][ 6]=	-2.57584;
     m_EtaROImin[0][18][ 7]=  -0.416721;m_EtaROImax[0][18][ 7]=   -0.30075;m_PhiROImin[0][18][ 7]=   -2.65852;m_PhiROImax[0][18][ 7]=	-2.57584;
     m_EtaROImin[0][18][ 8]=  -0.504617;m_EtaROImax[0][18][ 8]=   -0.42967;m_PhiROImin[0][18][ 8]=	-2.7461;m_PhiROImax[0][18][ 8]=	-2.66215;
     m_EtaROImin[0][18][ 9]=  -0.573871;m_EtaROImax[0][18][ 9]=  -0.501681;m_PhiROImin[0][18][ 9]=	-2.7461;m_PhiROImax[0][18][ 9]=	-2.66215;
     m_EtaROImin[0][18][10]=  -0.504617;m_EtaROImax[0][18][10]=   -0.42967;m_PhiROImin[0][18][10]=   -2.65852;m_PhiROImax[0][18][10]=	-2.57584;
     m_EtaROImin[0][18][11]=  -0.573871;m_EtaROImax[0][18][11]=  -0.501681;m_PhiROImin[0][18][11]=   -2.65852;m_PhiROImax[0][18][11]=	-2.57584;
     m_EtaROImin[0][18][12]=  -0.653329;m_EtaROImax[0][18][12]=  -0.583785;m_PhiROImin[0][18][12]=	-2.7461;m_PhiROImax[0][18][12]=	-2.66215;
     m_EtaROImin[0][18][13]=  -0.741516;m_EtaROImax[0][18][13]=  -0.649933;m_PhiROImin[0][18][13]=	-2.7461;m_PhiROImax[0][18][13]=	-2.66215;
     m_EtaROImin[0][18][14]=  -0.653329;m_EtaROImax[0][18][14]=  -0.583785;m_PhiROImin[0][18][14]=   -2.65852;m_PhiROImax[0][18][14]=	-2.57584;
     m_EtaROImin[0][18][15]=  -0.741516;m_EtaROImax[0][18][15]=  -0.649933;m_PhiROImin[0][18][15]=   -2.65852;m_PhiROImax[0][18][15]=	-2.57584;
     m_EtaROImin[0][18][16]=  -0.837822;m_EtaROImax[0][18][16]=  -0.756521;m_PhiROImin[0][18][16]=	-2.7461;m_PhiROImax[0][18][16]=	-2.66215;
     m_EtaROImin[0][18][17]=	    0;m_EtaROImax[0][18][17]=	     0;m_PhiROImin[0][18][17]=	      0;m_PhiROImax[0][18][17]=	       0;
     m_EtaROImin[0][18][18]=  -0.837822;m_EtaROImax[0][18][18]=  -0.756521;m_PhiROImin[0][18][18]=   -2.65852;m_PhiROImax[0][18][18]=	-2.57584;
     m_EtaROImin[0][18][19]=	    0;m_EtaROImax[0][18][19]=	     0;m_PhiROImin[0][18][19]=	      0;m_PhiROImax[0][18][19]=	       0;
     m_EtaROImin[0][18][20]=  -0.903324;m_EtaROImax[0][18][20]=  -0.844116;m_PhiROImin[0][18][20]=	-2.7461;m_PhiROImax[0][18][20]=	-2.66215;
     m_EtaROImin[0][18][21]=  -0.956037;m_EtaROImax[0][18][21]=  -0.899344;m_PhiROImin[0][18][21]=	-2.7461;m_PhiROImax[0][18][21]=	-2.66215;
     m_EtaROImin[0][18][22]=  -0.903324;m_EtaROImax[0][18][22]=  -0.844116;m_PhiROImin[0][18][22]=   -2.65852;m_PhiROImax[0][18][22]=	-2.57584;
     m_EtaROImin[0][18][23]=  -0.956037;m_EtaROImax[0][18][23]=  -0.899344;m_PhiROImin[0][18][23]=   -2.65852;m_PhiROImax[0][18][23]=	-2.57584;
     m_EtaROImin[0][18][24]=	    0;m_EtaROImax[0][18][24]=	     0;m_PhiROImin[0][18][24]=	      0;m_PhiROImax[0][18][24]=	       0;
     m_EtaROImin[0][18][25]=	    0;m_EtaROImax[0][18][25]=	     0;m_PhiROImin[0][18][25]=	      0;m_PhiROImax[0][18][25]=	       0;
     m_EtaROImin[0][18][26]=	    0;m_EtaROImax[0][18][26]=	     0;m_PhiROImin[0][18][26]=	      0;m_PhiROImax[0][18][26]=	       0;
     m_EtaROImin[0][18][27]=	    0;m_EtaROImax[0][18][27]=	     0;m_PhiROImin[0][18][27]=	      0;m_PhiROImax[0][18][27]=	       0;
     m_EtaROImin[0][18][28]=	    0;m_EtaROImax[0][18][28]=	     0;m_PhiROImin[0][18][28]=	      0;m_PhiROImax[0][18][28]=	       0;
     m_EtaROImin[0][18][29]=	    0;m_EtaROImax[0][18][29]=	     0;m_PhiROImin[0][18][29]=	      0;m_PhiROImax[0][18][29]=	       0;
     m_EtaROImin[0][18][30]=	    0;m_EtaROImax[0][18][30]=	     0;m_PhiROImin[0][18][30]=	      0;m_PhiROImax[0][18][30]=	       0;
     m_EtaROImin[0][18][31]=	    0;m_EtaROImax[0][18][31]=	     0;m_PhiROImin[0][18][31]=	      0;m_PhiROImax[0][18][31]=	       0;
     m_EtaROImin[0][19][ 0]=  -0.238294;m_EtaROImax[0][19][ 0]=  -0.180742;m_PhiROImin[0][19][ 0]=   -2.46941;m_PhiROImax[0][19][ 0]=	-2.35916;
     m_EtaROImin[0][19][ 1]=  -0.176882;m_EtaROImax[0][19][ 1]= -0.0874905;m_PhiROImin[0][19][ 1]=   -2.46941;m_PhiROImax[0][19][ 1]=	-2.35916;
     m_EtaROImin[0][19][ 2]=  -0.238294;m_EtaROImax[0][19][ 2]=  -0.180742;m_PhiROImin[0][19][ 2]=   -2.58036;m_PhiROImax[0][19][ 2]=	-2.47293;
     m_EtaROImin[0][19][ 3]=  -0.176882;m_EtaROImax[0][19][ 3]= -0.0874905;m_PhiROImin[0][19][ 3]=   -2.58036;m_PhiROImax[0][19][ 3]=	-2.47293;
     m_EtaROImin[0][19][ 4]=  -0.452099;m_EtaROImax[0][19][ 4]=  -0.350831;m_PhiROImin[0][19][ 4]=   -2.46881;m_PhiROImax[0][19][ 4]=	-2.35915;
     m_EtaROImin[0][19][ 5]=   -0.35049;m_EtaROImax[0][19][ 5]=  -0.246207;m_PhiROImin[0][19][ 5]=   -2.46881;m_PhiROImax[0][19][ 5]=	-2.35915;
     m_EtaROImin[0][19][ 6]=  -0.452099;m_EtaROImax[0][19][ 6]=  -0.350831;m_PhiROImin[0][19][ 6]=   -2.57921;m_PhiROImax[0][19][ 6]=	-2.47231;
     m_EtaROImin[0][19][ 7]=   -0.35049;m_EtaROImax[0][19][ 7]=  -0.246207;m_PhiROImin[0][19][ 7]=   -2.57921;m_PhiROImax[0][19][ 7]=	-2.47231;
     m_EtaROImin[0][19][ 8]=  -0.648101;m_EtaROImax[0][19][ 8]=  -0.555789;m_PhiROImin[0][19][ 8]=   -2.46881;m_PhiROImax[0][19][ 8]=	-2.35915;
     m_EtaROImin[0][19][ 9]=  -0.554444;m_EtaROImax[0][19][ 9]=  -0.458091;m_PhiROImin[0][19][ 9]=   -2.46881;m_PhiROImax[0][19][ 9]=	-2.35915;
     m_EtaROImin[0][19][10]=  -0.648101;m_EtaROImax[0][19][10]=  -0.555789;m_PhiROImin[0][19][10]=   -2.57921;m_PhiROImax[0][19][10]=	-2.47231;
     m_EtaROImin[0][19][11]=  -0.554444;m_EtaROImax[0][19][11]=  -0.458091;m_PhiROImin[0][19][11]=   -2.57921;m_PhiROImax[0][19][11]=	-2.47231;
     m_EtaROImin[0][19][12]=  -0.776449;m_EtaROImax[0][19][12]=  -0.731124;m_PhiROImin[0][19][12]=   -2.46941;m_PhiROImax[0][19][12]=	-2.35916;
     m_EtaROImin[0][19][13]=  -0.728056;m_EtaROImax[0][19][13]=  -0.655857;m_PhiROImin[0][19][13]=   -2.46941;m_PhiROImax[0][19][13]=	-2.35916;
     m_EtaROImin[0][19][14]=  -0.776449;m_EtaROImax[0][19][14]=  -0.731124;m_PhiROImin[0][19][14]=   -2.58036;m_PhiROImax[0][19][14]=	-2.47293;
     m_EtaROImin[0][19][15]=  -0.728056;m_EtaROImax[0][19][15]=  -0.655857;m_PhiROImin[0][19][15]=   -2.58036;m_PhiROImax[0][19][15]=	-2.47293;
     m_EtaROImin[0][19][16]=  -0.895163;m_EtaROImax[0][19][16]=  -0.853359;m_PhiROImin[0][19][16]=   -2.46941;m_PhiROImax[0][19][16]=	-2.35916;
     m_EtaROImin[0][19][17]=  -0.850528;m_EtaROImax[0][19][17]=  -0.783867;m_PhiROImin[0][19][17]=   -2.46941;m_PhiROImax[0][19][17]=	-2.35916;
     m_EtaROImin[0][19][18]=  -0.895163;m_EtaROImax[0][19][18]=  -0.853359;m_PhiROImin[0][19][18]=   -2.58036;m_PhiROImax[0][19][18]=	-2.47293;
     m_EtaROImin[0][19][19]=  -0.850528;m_EtaROImax[0][19][19]=  -0.783867;m_PhiROImin[0][19][19]=   -2.58036;m_PhiROImax[0][19][19]=	-2.47293;
     m_EtaROImin[0][19][20]=   -1.02514;m_EtaROImax[0][19][20]=  -0.964674;m_PhiROImin[0][19][20]=   -2.46881;m_PhiROImax[0][19][20]=	-2.35915;
     m_EtaROImin[0][19][21]=  -0.961344;m_EtaROImax[0][19][21]=  -0.898201;m_PhiROImin[0][19][21]=   -2.46881;m_PhiROImax[0][19][21]=	-2.35915;
     m_EtaROImin[0][19][22]=   -1.02514;m_EtaROImax[0][19][22]=  -0.964674;m_PhiROImin[0][19][22]=   -2.57921;m_PhiROImax[0][19][22]=	-2.47231;
     m_EtaROImin[0][19][23]=  -0.961344;m_EtaROImax[0][19][23]=  -0.898201;m_PhiROImin[0][19][23]=   -2.57921;m_PhiROImax[0][19][23]=	-2.47231;
     m_EtaROImin[0][19][24]=	    0;m_EtaROImax[0][19][24]=	     0;m_PhiROImin[0][19][24]=	      0;m_PhiROImax[0][19][24]=	       0;
     m_EtaROImin[0][19][25]=   -1.06547;m_EtaROImax[0][19][25]=   -1.03003;m_PhiROImin[0][19][25]=   -2.46962;m_PhiROImax[0][19][25]=	-2.35926;
     m_EtaROImin[0][19][26]=	    0;m_EtaROImax[0][19][26]=	     0;m_PhiROImin[0][19][26]=	      0;m_PhiROImax[0][19][26]=	       0;
     m_EtaROImin[0][19][27]=   -1.06547;m_EtaROImax[0][19][27]=   -1.03003;m_PhiROImin[0][19][27]=   -2.55334;m_PhiROImax[0][19][27]=	-2.47315;
     m_EtaROImin[0][19][28]=	    0;m_EtaROImax[0][19][28]=	     0;m_PhiROImin[0][19][28]=	      0;m_PhiROImax[0][19][28]=	       0;
     m_EtaROImin[0][19][29]=	    0;m_EtaROImax[0][19][29]=	     0;m_PhiROImin[0][19][29]=	      0;m_PhiROImax[0][19][29]=	       0;
     m_EtaROImin[0][19][30]=	    0;m_EtaROImax[0][19][30]=	     0;m_PhiROImin[0][19][30]=	      0;m_PhiROImax[0][19][30]=	       0;
     m_EtaROImin[0][19][31]=	    0;m_EtaROImax[0][19][31]=	     0;m_PhiROImin[0][19][31]=	      0;m_PhiROImax[0][19][31]=	       0;
     m_EtaROImin[0][20][ 0]=  -0.145912;m_EtaROImax[0][20][ 0]= -0.0874905;m_PhiROImin[0][20][ 0]=   -2.35323;m_PhiROImax[0][20][ 0]=	-2.24298;
     m_EtaROImin[0][20][ 1]=  -0.238294;m_EtaROImax[0][20][ 1]=  -0.149792;m_PhiROImin[0][20][ 1]=   -2.35323;m_PhiROImax[0][20][ 1]=	-2.24298;
     m_EtaROImin[0][20][ 2]=  -0.145912;m_EtaROImax[0][20][ 2]= -0.0874905;m_PhiROImin[0][20][ 2]=   -2.23946;m_PhiROImax[0][20][ 2]=	-2.13203;
     m_EtaROImin[0][20][ 3]=  -0.238294;m_EtaROImax[0][20][ 3]=  -0.149792;m_PhiROImin[0][20][ 3]=   -2.23946;m_PhiROImax[0][20][ 3]=	-2.13203;
     m_EtaROImin[0][20][ 4]=   -0.35049;m_EtaROImax[0][20][ 4]=  -0.246207;m_PhiROImin[0][20][ 4]=   -2.35324;m_PhiROImax[0][20][ 4]=	-2.24358;
     m_EtaROImin[0][20][ 5]=  -0.452099;m_EtaROImax[0][20][ 5]=  -0.350831;m_PhiROImin[0][20][ 5]=   -2.35324;m_PhiROImax[0][20][ 5]=	-2.24358;
     m_EtaROImin[0][20][ 6]=   -0.35049;m_EtaROImax[0][20][ 6]=  -0.246207;m_PhiROImin[0][20][ 6]=   -2.24008;m_PhiROImax[0][20][ 6]=	-2.13318;
     m_EtaROImin[0][20][ 7]=  -0.452099;m_EtaROImax[0][20][ 7]=  -0.350831;m_PhiROImin[0][20][ 7]=   -2.24008;m_PhiROImax[0][20][ 7]=	-2.13318;
     m_EtaROImin[0][20][ 8]=  -0.554444;m_EtaROImax[0][20][ 8]=  -0.458091;m_PhiROImin[0][20][ 8]=   -2.35324;m_PhiROImax[0][20][ 8]=	-2.24358;
     m_EtaROImin[0][20][ 9]=  -0.648101;m_EtaROImax[0][20][ 9]=  -0.555789;m_PhiROImin[0][20][ 9]=   -2.35324;m_PhiROImax[0][20][ 9]=	-2.24358;
     m_EtaROImin[0][20][10]=  -0.554444;m_EtaROImax[0][20][10]=  -0.458091;m_PhiROImin[0][20][10]=   -2.24008;m_PhiROImax[0][20][10]=	-2.13318;
     m_EtaROImin[0][20][11]=  -0.648101;m_EtaROImax[0][20][11]=  -0.555789;m_PhiROImin[0][20][11]=   -2.24008;m_PhiROImax[0][20][11]=	-2.13318;
     m_EtaROImin[0][20][12]=  -0.703298;m_EtaROImax[0][20][12]=  -0.655857;m_PhiROImin[0][20][12]=   -2.35323;m_PhiROImax[0][20][12]=	-2.24298;
     m_EtaROImin[0][20][13]=  -0.776449;m_EtaROImax[0][20][13]=  -0.706413;m_PhiROImin[0][20][13]=   -2.35323;m_PhiROImax[0][20][13]=	-2.24298;
     m_EtaROImin[0][20][14]=  -0.703298;m_EtaROImax[0][20][14]=  -0.655857;m_PhiROImin[0][20][14]=   -2.23946;m_PhiROImax[0][20][14]=	-2.13203;
     m_EtaROImin[0][20][15]=  -0.776449;m_EtaROImax[0][20][15]=  -0.706413;m_PhiROImin[0][20][15]=   -2.23946;m_PhiROImax[0][20][15]=	-2.13203;
     m_EtaROImin[0][20][16]=   -0.82768;m_EtaROImax[0][20][16]=  -0.783867;m_PhiROImin[0][20][16]=   -2.35323;m_PhiROImax[0][20][16]=	-2.24298;
     m_EtaROImin[0][20][17]=  -0.895163;m_EtaROImax[0][20][17]=  -0.830556;m_PhiROImin[0][20][17]=   -2.35323;m_PhiROImax[0][20][17]=	-2.24298;
     m_EtaROImin[0][20][18]=   -0.82768;m_EtaROImax[0][20][18]=  -0.783867;m_PhiROImin[0][20][18]=   -2.23946;m_PhiROImax[0][20][18]=	-2.13203;
     m_EtaROImin[0][20][19]=  -0.895163;m_EtaROImax[0][20][19]=  -0.830556;m_PhiROImin[0][20][19]=   -2.23946;m_PhiROImax[0][20][19]=	-2.13203;
     m_EtaROImin[0][20][20]=  -0.961344;m_EtaROImax[0][20][20]=  -0.898201;m_PhiROImin[0][20][20]=   -2.35324;m_PhiROImax[0][20][20]=	-2.24358;
     m_EtaROImin[0][20][21]=   -1.02514;m_EtaROImax[0][20][21]=  -0.964674;m_PhiROImin[0][20][21]=   -2.35324;m_PhiROImax[0][20][21]=	-2.24358;
     m_EtaROImin[0][20][22]=  -0.961344;m_EtaROImax[0][20][22]=  -0.898201;m_PhiROImin[0][20][22]=   -2.24008;m_PhiROImax[0][20][22]=	-2.13318;
     m_EtaROImin[0][20][23]=   -1.02514;m_EtaROImax[0][20][23]=  -0.964674;m_PhiROImin[0][20][23]=   -2.24008;m_PhiROImax[0][20][23]=	-2.13318;
     m_EtaROImin[0][20][24]=   -1.06547;m_EtaROImax[0][20][24]=   -1.03003;m_PhiROImin[0][20][24]=   -2.35313;m_PhiROImax[0][20][24]=	-2.24277;
     m_EtaROImin[0][20][25]=	    0;m_EtaROImax[0][20][25]=	     0;m_PhiROImin[0][20][25]=	      0;m_PhiROImax[0][20][25]=	       0;
     m_EtaROImin[0][20][26]=   -1.06547;m_EtaROImax[0][20][26]=   -1.03003;m_PhiROImin[0][20][26]=   -2.23924;m_PhiROImax[0][20][26]=	-2.15905;
     m_EtaROImin[0][20][27]=	    0;m_EtaROImax[0][20][27]=	     0;m_PhiROImin[0][20][27]=	      0;m_PhiROImax[0][20][27]=	       0;
     m_EtaROImin[0][20][28]=	    0;m_EtaROImax[0][20][28]=	     0;m_PhiROImin[0][20][28]=	      0;m_PhiROImax[0][20][28]=	       0;
     m_EtaROImin[0][20][29]=	    0;m_EtaROImax[0][20][29]=	     0;m_PhiROImin[0][20][29]=	      0;m_PhiROImax[0][20][29]=	       0;
     m_EtaROImin[0][20][30]=	    0;m_EtaROImax[0][20][30]=	     0;m_PhiROImin[0][20][30]=	      0;m_PhiROImax[0][20][30]=	       0;
     m_EtaROImin[0][20][31]=	    0;m_EtaROImax[0][20][31]=	     0;m_PhiROImin[0][20][31]=	      0;m_PhiROImax[0][20][31]=	       0;
     m_EtaROImin[0][21][ 0]=  -0.203032;m_EtaROImax[0][21][ 0]=  -0.155043;m_PhiROImin[0][21][ 0]=   -2.05024;m_PhiROImax[0][21][ 0]=	-1.96629;
     m_EtaROImin[0][21][ 1]=   -0.15183;m_EtaROImax[0][21][ 1]= -0.0775634;m_PhiROImin[0][21][ 1]=   -2.05024;m_PhiROImax[0][21][ 1]=	-1.96629;
     m_EtaROImin[0][21][ 2]=  -0.203032;m_EtaROImax[0][21][ 2]=  -0.155043;m_PhiROImin[0][21][ 2]=   -2.13655;m_PhiROImax[0][21][ 2]=	-2.05387;
     m_EtaROImin[0][21][ 3]=   -0.15183;m_EtaROImax[0][21][ 3]= -0.0775634;m_PhiROImin[0][21][ 3]=   -2.13655;m_PhiROImax[0][21][ 3]=	-2.05387;
     m_EtaROImin[0][21][ 4]=  -0.322732;m_EtaROImax[0][21][ 4]=  -0.276301;m_PhiROImin[0][21][ 4]=   -2.04983;m_PhiROImax[0][21][ 4]=	-1.96627;
     m_EtaROImin[0][21][ 5]=  -0.273184;m_EtaROImax[0][21][ 5]=  -0.200823;m_PhiROImin[0][21][ 5]=   -2.04983;m_PhiROImax[0][21][ 5]=	-1.96627;
     m_EtaROImin[0][21][ 6]=  -0.322732;m_EtaROImax[0][21][ 6]=  -0.276301;m_PhiROImin[0][21][ 6]=   -2.13573;m_PhiROImax[0][21][ 6]=	-2.05344;
     m_EtaROImin[0][21][ 7]=  -0.273184;m_EtaROImax[0][21][ 7]=  -0.200823;m_PhiROImin[0][21][ 7]=   -2.13573;m_PhiROImax[0][21][ 7]=	-2.05344;
     m_EtaROImin[0][21][ 8]=   -0.51487;m_EtaROImax[0][21][ 8]=  -0.439011;m_PhiROImin[0][21][ 8]=   -2.04536;m_PhiROImax[0][21][ 8]=	-1.96564;
     m_EtaROImin[0][21][ 9]=  -0.441017;m_EtaROImax[0][21][ 9]=  -0.362484;m_PhiROImin[0][21][ 9]=   -2.04536;m_PhiROImax[0][21][ 9]=	-1.96564;
     m_EtaROImin[0][21][10]=   -0.51487;m_EtaROImax[0][21][10]=  -0.439011;m_PhiROImin[0][21][10]=   -2.12657;m_PhiROImax[0][21][10]=	-2.04792;
     m_EtaROImin[0][21][11]=  -0.441017;m_EtaROImax[0][21][11]=  -0.362484;m_PhiROImin[0][21][11]=   -2.12657;m_PhiROImax[0][21][11]=	-2.04792;
     m_EtaROImin[0][21][12]=  -0.659332;m_EtaROImax[0][21][12]=  -0.566067;m_PhiROImin[0][21][12]=   -2.05024;m_PhiROImax[0][21][12]=	-1.96629;
     m_EtaROImin[0][21][13]=  -0.569141;m_EtaROImax[0][21][13]=  -0.470878;m_PhiROImin[0][21][13]=   -2.05024;m_PhiROImax[0][21][13]=	-1.96629;
     m_EtaROImin[0][21][14]=  -0.659332;m_EtaROImax[0][21][14]=  -0.566067;m_PhiROImin[0][21][14]=   -2.13655;m_PhiROImax[0][21][14]=	-2.05387;
     m_EtaROImin[0][21][15]=  -0.569141;m_EtaROImax[0][21][15]=  -0.470878;m_PhiROImin[0][21][15]=   -2.13655;m_PhiROImax[0][21][15]=	-2.05387;
     m_EtaROImin[0][21][16]=   -0.72184;m_EtaROImax[0][21][16]=  -0.671529;m_PhiROImin[0][21][16]=   -2.04536;m_PhiROImax[0][21][16]=	-1.96564;
     m_EtaROImin[0][21][17]=  -0.674155;m_EtaROImax[0][21][17]=  -0.622205;m_PhiROImin[0][21][17]=   -2.04536;m_PhiROImax[0][21][17]=	-1.96564;
     m_EtaROImin[0][21][18]=   -0.72184;m_EtaROImax[0][21][18]=  -0.671529;m_PhiROImin[0][21][18]=   -2.12657;m_PhiROImax[0][21][18]=	-2.04792;
     m_EtaROImin[0][21][19]=  -0.674155;m_EtaROImax[0][21][19]=  -0.622205;m_PhiROImin[0][21][19]=   -2.12657;m_PhiROImax[0][21][19]=	-2.04792;
     m_EtaROImin[0][21][20]=  -0.893028;m_EtaROImax[0][21][20]=  -0.833797;m_PhiROImin[0][21][20]=   -2.04536;m_PhiROImax[0][21][20]=	-1.96564;
     m_EtaROImin[0][21][21]=  -0.831846;m_EtaROImax[0][21][21]=  -0.744499;m_PhiROImin[0][21][21]=   -2.04536;m_PhiROImax[0][21][21]=	-1.96564;
     m_EtaROImin[0][21][22]=  -0.893028;m_EtaROImax[0][21][22]=  -0.833797;m_PhiROImin[0][21][22]=   -2.12657;m_PhiROImax[0][21][22]=	-2.04792;
     m_EtaROImin[0][21][23]=  -0.831846;m_EtaROImax[0][21][23]=  -0.744499;m_PhiROImin[0][21][23]=   -2.12657;m_PhiROImax[0][21][23]=	-2.04792;
     m_EtaROImin[0][21][24]=  -0.905303;m_EtaROImax[0][21][24]=  -0.846464;m_PhiROImin[0][21][24]=   -2.05024;m_PhiROImax[0][21][24]=	-1.96629;
     m_EtaROImin[0][21][25]=  -0.850339;m_EtaROImax[0][21][25]=  -0.788882;m_PhiROImin[0][21][25]=   -2.05024;m_PhiROImax[0][21][25]=	-1.96629;
     m_EtaROImin[0][21][26]=  -0.905303;m_EtaROImax[0][21][26]=  -0.846464;m_PhiROImin[0][21][26]=   -2.13655;m_PhiROImax[0][21][26]=	-2.05387;
     m_EtaROImin[0][21][27]=  -0.850339;m_EtaROImax[0][21][27]=  -0.788882;m_PhiROImin[0][21][27]=   -2.13655;m_PhiROImax[0][21][27]=	-2.05387;
     m_EtaROImin[0][21][28]=   -0.98143;m_EtaROImax[0][21][28]=  -0.954883;m_PhiROImin[0][21][28]=   -2.03358;m_PhiROImax[0][21][28]=	-1.98444;
     m_EtaROImin[0][21][29]=  -0.953094;m_EtaROImax[0][21][29]=  -0.911293;m_PhiROImin[0][21][29]=   -2.03358;m_PhiROImax[0][21][29]=	-1.98444;
     m_EtaROImin[0][21][30]=   -0.98143;m_EtaROImax[0][21][30]=  -0.954883;m_PhiROImin[0][21][30]=   -2.12127;m_PhiROImax[0][21][30]=	-2.03635;
     m_EtaROImin[0][21][31]=  -0.953094;m_EtaROImax[0][21][31]=  -0.911293;m_PhiROImin[0][21][31]=   -2.12127;m_PhiROImax[0][21][31]=	-2.03635;
     m_EtaROImin[0][22][ 0]=  -0.126072;m_EtaROImax[0][22][ 0]= -0.0775634;m_PhiROImin[0][22][ 0]=   -1.96071;m_PhiROImax[0][22][ 0]=	-1.87675;
     m_EtaROImin[0][22][ 1]=  -0.203032;m_EtaROImax[0][22][ 1]=  -0.129296;m_PhiROImin[0][22][ 1]=   -1.96071;m_PhiROImax[0][22][ 1]=	-1.87675;
     m_EtaROImin[0][22][ 2]=  -0.126072;m_EtaROImax[0][22][ 2]= -0.0775634;m_PhiROImin[0][22][ 2]=   -1.87312;m_PhiROImax[0][22][ 2]=	-1.79045;
     m_EtaROImin[0][22][ 3]=  -0.203032;m_EtaROImax[0][22][ 3]=  -0.129296;m_PhiROImin[0][22][ 3]=   -1.87312;m_PhiROImax[0][22][ 3]=	-1.79045;
     m_EtaROImin[0][22][ 4]=  -0.248155;m_EtaROImax[0][22][ 4]=  -0.200823;m_PhiROImin[0][22][ 4]=   -1.96072;m_PhiROImax[0][22][ 4]=	-1.87716;
     m_EtaROImin[0][22][ 5]=  -0.322732;m_EtaROImax[0][22][ 5]=  -0.251292;m_PhiROImin[0][22][ 5]=   -1.96072;m_PhiROImax[0][22][ 5]=	-1.87716;
     m_EtaROImin[0][22][ 6]=  -0.248155;m_EtaROImax[0][22][ 6]=  -0.200823;m_PhiROImin[0][22][ 6]=   -1.87355;m_PhiROImax[0][22][ 6]=	-1.79126;
     m_EtaROImin[0][22][ 7]=  -0.322732;m_EtaROImax[0][22][ 7]=  -0.251292;m_PhiROImin[0][22][ 7]=   -1.87355;m_PhiROImax[0][22][ 7]=	-1.79126;
     m_EtaROImin[0][22][ 8]=  -0.441017;m_EtaROImax[0][22][ 8]=  -0.362484;m_PhiROImin[0][22][ 8]=   -1.96135;m_PhiROImax[0][22][ 8]=	-1.88163;
     m_EtaROImin[0][22][ 9]=   -0.51487;m_EtaROImax[0][22][ 9]=  -0.439011;m_PhiROImin[0][22][ 9]=   -1.96135;m_PhiROImax[0][22][ 9]=	-1.88163;
     m_EtaROImin[0][22][10]=  -0.441017;m_EtaROImax[0][22][10]=  -0.362484;m_PhiROImin[0][22][10]=   -1.87907;m_PhiROImax[0][22][10]=	-1.80042;
     m_EtaROImin[0][22][11]=   -0.51487;m_EtaROImax[0][22][11]=  -0.439011;m_PhiROImin[0][22][11]=   -1.87907;m_PhiROImax[0][22][11]=	-1.80042;
     m_EtaROImin[0][22][12]=  -0.569141;m_EtaROImax[0][22][12]=  -0.470878;m_PhiROImin[0][22][12]=   -1.96071;m_PhiROImax[0][22][12]=	-1.87675;
     m_EtaROImin[0][22][13]=  -0.659332;m_EtaROImax[0][22][13]=  -0.566067;m_PhiROImin[0][22][13]=   -1.96071;m_PhiROImax[0][22][13]=	-1.87675;
     m_EtaROImin[0][22][14]=  -0.569141;m_EtaROImax[0][22][14]=  -0.470878;m_PhiROImin[0][22][14]=   -1.87312;m_PhiROImax[0][22][14]=	-1.79045;
     m_EtaROImin[0][22][15]=  -0.659332;m_EtaROImax[0][22][15]=  -0.566067;m_PhiROImin[0][22][15]=   -1.87312;m_PhiROImax[0][22][15]=	-1.79045;
     m_EtaROImin[0][22][16]=  -0.674155;m_EtaROImax[0][22][16]=  -0.622205;m_PhiROImin[0][22][16]=   -1.96135;m_PhiROImax[0][22][16]=	-1.88163;
     m_EtaROImin[0][22][17]=   -0.72184;m_EtaROImax[0][22][17]=  -0.671529;m_PhiROImin[0][22][17]=   -1.96135;m_PhiROImax[0][22][17]=	-1.88163;
     m_EtaROImin[0][22][18]=  -0.674155;m_EtaROImax[0][22][18]=  -0.622205;m_PhiROImin[0][22][18]=   -1.87907;m_PhiROImax[0][22][18]=	-1.80042;
     m_EtaROImin[0][22][19]=   -0.72184;m_EtaROImax[0][22][19]=  -0.671529;m_PhiROImin[0][22][19]=   -1.87907;m_PhiROImax[0][22][19]=	-1.80042;
     m_EtaROImin[0][22][20]=  -0.831846;m_EtaROImax[0][22][20]=  -0.744499;m_PhiROImin[0][22][20]=   -1.96135;m_PhiROImax[0][22][20]=	-1.88163;
     m_EtaROImin[0][22][21]=  -0.893028;m_EtaROImax[0][22][21]=  -0.833797;m_PhiROImin[0][22][21]=   -1.96135;m_PhiROImax[0][22][21]=	-1.88163;
     m_EtaROImin[0][22][22]=  -0.831846;m_EtaROImax[0][22][22]=  -0.744499;m_PhiROImin[0][22][22]=   -1.87907;m_PhiROImax[0][22][22]=	-1.80042;
     m_EtaROImin[0][22][23]=  -0.893028;m_EtaROImax[0][22][23]=  -0.833797;m_PhiROImin[0][22][23]=   -1.87907;m_PhiROImax[0][22][23]=	-1.80042;
     m_EtaROImin[0][22][24]=  -0.850339;m_EtaROImax[0][22][24]=  -0.788882;m_PhiROImin[0][22][24]=   -1.96071;m_PhiROImax[0][22][24]=	-1.87675;
     m_EtaROImin[0][22][25]=  -0.905303;m_EtaROImax[0][22][25]=  -0.846464;m_PhiROImin[0][22][25]=   -1.96071;m_PhiROImax[0][22][25]=	-1.87675;
     m_EtaROImin[0][22][26]=  -0.850339;m_EtaROImax[0][22][26]=  -0.788882;m_PhiROImin[0][22][26]=   -1.87312;m_PhiROImax[0][22][26]=	-1.79045;
     m_EtaROImin[0][22][27]=  -0.905303;m_EtaROImax[0][22][27]=  -0.846464;m_PhiROImin[0][22][27]=   -1.87312;m_PhiROImax[0][22][27]=	-1.79045;
     m_EtaROImin[0][22][28]=  -0.940931;m_EtaROImax[0][22][28]=  -0.913486;m_PhiROImin[0][22][28]=   -1.97686;m_PhiROImax[0][22][28]=	-1.89087;
     m_EtaROImin[0][22][29]=  -0.983719;m_EtaROImax[0][22][29]=  -0.942741;m_PhiROImin[0][22][29]=   -1.97686;m_PhiROImax[0][22][29]=	-1.89087;
     m_EtaROImin[0][22][30]=  -0.940931;m_EtaROImax[0][22][30]=  -0.913486;m_PhiROImin[0][22][30]=   -1.98444;m_PhiROImax[0][22][30]=	-1.88811;
     m_EtaROImin[0][22][31]=  -0.983719;m_EtaROImax[0][22][31]=  -0.942741;m_PhiROImin[0][22][31]=   -1.98444;m_PhiROImax[0][22][31]=	-1.88811;
     m_EtaROImin[0][23][ 0]=  -0.238866;m_EtaROImax[0][23][ 0]=  -0.131182;m_PhiROImin[0][23][ 0]=   -1.68341;m_PhiROImax[0][23][ 0]=	-1.57375;
     m_EtaROImin[0][23][ 1]=  -0.132007;m_EtaROImax[0][23][ 1]=   -0.02338;m_PhiROImin[0][23][ 1]=   -1.68341;m_PhiROImax[0][23][ 1]=	-1.57375;
     m_EtaROImin[0][23][ 2]=  -0.238866;m_EtaROImax[0][23][ 2]=  -0.131182;m_PhiROImin[0][23][ 2]=   -1.79381;m_PhiROImax[0][23][ 2]=	-1.68692;
     m_EtaROImin[0][23][ 3]=  -0.132007;m_EtaROImax[0][23][ 3]=   -0.02338;m_PhiROImin[0][23][ 3]=   -1.79381;m_PhiROImax[0][23][ 3]=	-1.68692;
     m_EtaROImin[0][23][ 4]=  -0.452099;m_EtaROImax[0][23][ 4]=  -0.350831;m_PhiROImin[0][23][ 4]=   -1.68341;m_PhiROImax[0][23][ 4]=	-1.57375;
     m_EtaROImin[0][23][ 5]=   -0.35049;m_EtaROImax[0][23][ 5]=  -0.246207;m_PhiROImin[0][23][ 5]=   -1.68341;m_PhiROImax[0][23][ 5]=	-1.57375;
     m_EtaROImin[0][23][ 6]=  -0.452099;m_EtaROImax[0][23][ 6]=  -0.350831;m_PhiROImin[0][23][ 6]=   -1.79381;m_PhiROImax[0][23][ 6]=	-1.68692;
     m_EtaROImin[0][23][ 7]=   -0.35049;m_EtaROImax[0][23][ 7]=  -0.246207;m_PhiROImin[0][23][ 7]=   -1.79381;m_PhiROImax[0][23][ 7]=	-1.68692;
     m_EtaROImin[0][23][ 8]=  -0.648101;m_EtaROImax[0][23][ 8]=  -0.555789;m_PhiROImin[0][23][ 8]=   -1.68341;m_PhiROImax[0][23][ 8]=	-1.57375;
     m_EtaROImin[0][23][ 9]=  -0.554444;m_EtaROImax[0][23][ 9]=  -0.458091;m_PhiROImin[0][23][ 9]=   -1.68341;m_PhiROImax[0][23][ 9]=	-1.57375;
     m_EtaROImin[0][23][10]=  -0.648101;m_EtaROImax[0][23][10]=  -0.555789;m_PhiROImin[0][23][10]=   -1.79381;m_PhiROImax[0][23][10]=	-1.68692;
     m_EtaROImin[0][23][11]=  -0.554444;m_EtaROImax[0][23][11]=  -0.458091;m_PhiROImin[0][23][11]=   -1.79381;m_PhiROImax[0][23][11]=	-1.68692;
     m_EtaROImin[0][23][12]=  -0.786462;m_EtaROImax[0][23][12]=  -0.745327;m_PhiROImin[0][23][12]=   -1.62692;m_PhiROImax[0][23][12]=	 -1.5708;
     m_EtaROImin[0][23][13]=  -0.742545;m_EtaROImax[0][23][13]=  -0.677222;m_PhiROImin[0][23][13]=   -1.62692;m_PhiROImax[0][23][13]=	 -1.5708;
     m_EtaROImin[0][23][14]=  -0.786462;m_EtaROImax[0][23][14]=  -0.745327;m_PhiROImin[0][23][14]=   -1.69744;m_PhiROImax[0][23][14]=	-1.63046;
     m_EtaROImin[0][23][15]=  -0.742545;m_EtaROImax[0][23][15]=  -0.677222;m_PhiROImin[0][23][15]=   -1.69744;m_PhiROImax[0][23][15]=	-1.63046;
     m_EtaROImin[0][23][16]=  -0.895163;m_EtaROImax[0][23][16]=  -0.853359;m_PhiROImin[0][23][16]=   -1.68401;m_PhiROImax[0][23][16]=	-1.57376;
     m_EtaROImin[0][23][17]=  -0.850528;m_EtaROImax[0][23][17]=  -0.783867;m_PhiROImin[0][23][17]=   -1.68401;m_PhiROImax[0][23][17]=	-1.57376;
     m_EtaROImin[0][23][18]=  -0.895163;m_EtaROImax[0][23][18]=  -0.853359;m_PhiROImin[0][23][18]=   -1.79497;m_PhiROImax[0][23][18]=	-1.68753;
     m_EtaROImin[0][23][19]=  -0.850528;m_EtaROImax[0][23][19]=  -0.783867;m_PhiROImin[0][23][19]=   -1.79497;m_PhiROImax[0][23][19]=	-1.68753;
     m_EtaROImin[0][23][20]=   -1.02514;m_EtaROImax[0][23][20]=  -0.964674;m_PhiROImin[0][23][20]=   -1.68341;m_PhiROImax[0][23][20]=	-1.57375;
     m_EtaROImin[0][23][21]=  -0.961344;m_EtaROImax[0][23][21]=  -0.898201;m_PhiROImin[0][23][21]=   -1.68341;m_PhiROImax[0][23][21]=	-1.57375;
     m_EtaROImin[0][23][22]=   -1.02514;m_EtaROImax[0][23][22]=  -0.964674;m_PhiROImin[0][23][22]=   -1.79381;m_PhiROImax[0][23][22]=	-1.68692;
     m_EtaROImin[0][23][23]=  -0.961344;m_EtaROImax[0][23][23]=  -0.898201;m_PhiROImin[0][23][23]=   -1.79381;m_PhiROImax[0][23][23]=	-1.68692;
     m_EtaROImin[0][23][24]=	    0;m_EtaROImax[0][23][24]=	     0;m_PhiROImin[0][23][24]=	      0;m_PhiROImax[0][23][24]=	       0;
     m_EtaROImin[0][23][25]=   -1.06547;m_EtaROImax[0][23][25]=   -1.03003;m_PhiROImin[0][23][25]=   -1.68423;m_PhiROImax[0][23][25]=	-1.57387;
     m_EtaROImin[0][23][26]=	    0;m_EtaROImax[0][23][26]=	     0;m_PhiROImin[0][23][26]=	      0;m_PhiROImax[0][23][26]=	       0;
     m_EtaROImin[0][23][27]=   -1.06547;m_EtaROImax[0][23][27]=   -1.03003;m_PhiROImin[0][23][27]=   -1.76794;m_PhiROImax[0][23][27]=	-1.68775;
     m_EtaROImin[0][23][28]=	    0;m_EtaROImax[0][23][28]=	     0;m_PhiROImin[0][23][28]=	      0;m_PhiROImax[0][23][28]=	       0;
     m_EtaROImin[0][23][29]=	    0;m_EtaROImax[0][23][29]=	     0;m_PhiROImin[0][23][29]=	      0;m_PhiROImax[0][23][29]=	       0;
     m_EtaROImin[0][23][30]=	    0;m_EtaROImax[0][23][30]=	     0;m_PhiROImin[0][23][30]=	      0;m_PhiROImax[0][23][30]=	       0;
     m_EtaROImin[0][23][31]=	    0;m_EtaROImax[0][23][31]=	     0;m_PhiROImin[0][23][31]=	      0;m_PhiROImax[0][23][31]=	       0;
     m_EtaROImin[0][24][ 0]=  -0.132007;m_EtaROImax[0][24][ 0]=   -0.02338;m_PhiROImin[0][24][ 0]=   -1.56784;m_PhiROImax[0][24][ 0]=	-1.45818;
     m_EtaROImin[0][24][ 1]=  -0.238866;m_EtaROImax[0][24][ 1]=  -0.131182;m_PhiROImin[0][24][ 1]=   -1.56784;m_PhiROImax[0][24][ 1]=	-1.45818;
     m_EtaROImin[0][24][ 2]=  -0.132007;m_EtaROImax[0][24][ 2]=   -0.02338;m_PhiROImin[0][24][ 2]=   -1.45468;m_PhiROImax[0][24][ 2]=	-1.34779;
     m_EtaROImin[0][24][ 3]=  -0.238866;m_EtaROImax[0][24][ 3]=  -0.131182;m_PhiROImin[0][24][ 3]=   -1.45468;m_PhiROImax[0][24][ 3]=	-1.34779;
     m_EtaROImin[0][24][ 4]=   -0.35049;m_EtaROImax[0][24][ 4]=  -0.246207;m_PhiROImin[0][24][ 4]=   -1.56784;m_PhiROImax[0][24][ 4]=	-1.45818;
     m_EtaROImin[0][24][ 5]=  -0.452099;m_EtaROImax[0][24][ 5]=  -0.350831;m_PhiROImin[0][24][ 5]=   -1.56784;m_PhiROImax[0][24][ 5]=	-1.45818;
     m_EtaROImin[0][24][ 6]=   -0.35049;m_EtaROImax[0][24][ 6]=  -0.246207;m_PhiROImin[0][24][ 6]=   -1.45468;m_PhiROImax[0][24][ 6]=	-1.34779;
     m_EtaROImin[0][24][ 7]=  -0.452099;m_EtaROImax[0][24][ 7]=  -0.350831;m_PhiROImin[0][24][ 7]=   -1.45468;m_PhiROImax[0][24][ 7]=	-1.34779;
     m_EtaROImin[0][24][ 8]=  -0.554444;m_EtaROImax[0][24][ 8]=  -0.458091;m_PhiROImin[0][24][ 8]=   -1.56784;m_PhiROImax[0][24][ 8]=	-1.45818;
     m_EtaROImin[0][24][ 9]=  -0.648101;m_EtaROImax[0][24][ 9]=  -0.555789;m_PhiROImin[0][24][ 9]=   -1.56784;m_PhiROImax[0][24][ 9]=	-1.45818;
     m_EtaROImin[0][24][10]=  -0.554444;m_EtaROImax[0][24][10]=  -0.458091;m_PhiROImin[0][24][10]=   -1.45468;m_PhiROImax[0][24][10]=	-1.34779;
     m_EtaROImin[0][24][11]=  -0.648101;m_EtaROImax[0][24][11]=  -0.555789;m_PhiROImin[0][24][11]=   -1.45468;m_PhiROImax[0][24][11]=	-1.34779;
     m_EtaROImin[0][24][12]=  -0.720119;m_EtaROImax[0][24][12]=  -0.677222;m_PhiROImin[0][24][12]=   -1.56798;m_PhiROImax[0][24][12]=	 -1.5005;
     m_EtaROImin[0][24][13]=  -0.786462;m_EtaROImax[0][24][13]=   -0.72294;m_PhiROImin[0][24][13]=   -1.56798;m_PhiROImax[0][24][13]=	 -1.5005;
     m_EtaROImin[0][24][14]=  -0.720119;m_EtaROImax[0][24][14]=  -0.677222;m_PhiROImin[0][24][14]=	-1.5708;m_PhiROImax[0][24][14]=	-1.49696;
     m_EtaROImin[0][24][15]=  -0.786462;m_EtaROImax[0][24][15]=   -0.72294;m_PhiROImin[0][24][15]=	-1.5708;m_PhiROImax[0][24][15]=	-1.49696;
     m_EtaROImin[0][24][16]=   -0.82768;m_EtaROImax[0][24][16]=  -0.783867;m_PhiROImin[0][24][16]=   -1.56783;m_PhiROImax[0][24][16]=	-1.45758;
     m_EtaROImin[0][24][17]=  -0.895163;m_EtaROImax[0][24][17]=  -0.830556;m_PhiROImin[0][24][17]=   -1.56783;m_PhiROImax[0][24][17]=	-1.45758;
     m_EtaROImin[0][24][18]=   -0.82768;m_EtaROImax[0][24][18]=  -0.783867;m_PhiROImin[0][24][18]=   -1.45406;m_PhiROImax[0][24][18]=	-1.34663;
     m_EtaROImin[0][24][19]=  -0.895163;m_EtaROImax[0][24][19]=  -0.830556;m_PhiROImin[0][24][19]=   -1.45406;m_PhiROImax[0][24][19]=	-1.34663;
     m_EtaROImin[0][24][20]=  -0.961344;m_EtaROImax[0][24][20]=  -0.898201;m_PhiROImin[0][24][20]=   -1.56784;m_PhiROImax[0][24][20]=	-1.45818;
     m_EtaROImin[0][24][21]=   -1.02514;m_EtaROImax[0][24][21]=  -0.964674;m_PhiROImin[0][24][21]=   -1.56784;m_PhiROImax[0][24][21]=	-1.45818;
     m_EtaROImin[0][24][22]=  -0.961344;m_EtaROImax[0][24][22]=  -0.898201;m_PhiROImin[0][24][22]=   -1.45468;m_PhiROImax[0][24][22]=	-1.34779;
     m_EtaROImin[0][24][23]=   -1.02514;m_EtaROImax[0][24][23]=  -0.964674;m_PhiROImin[0][24][23]=   -1.45468;m_PhiROImax[0][24][23]=	-1.34779;
     m_EtaROImin[0][24][24]=   -1.06547;m_EtaROImax[0][24][24]=   -1.03003;m_PhiROImin[0][24][24]=   -1.56773;m_PhiROImax[0][24][24]=	-1.45737;
     m_EtaROImin[0][24][25]=	    0;m_EtaROImax[0][24][25]=	     0;m_PhiROImin[0][24][25]=	      0;m_PhiROImax[0][24][25]=	       0;
     m_EtaROImin[0][24][26]=   -1.06547;m_EtaROImax[0][24][26]=   -1.03003;m_PhiROImin[0][24][26]=   -1.45384;m_PhiROImax[0][24][26]=	-1.37365;
     m_EtaROImin[0][24][27]=	    0;m_EtaROImax[0][24][27]=	     0;m_PhiROImin[0][24][27]=	      0;m_PhiROImax[0][24][27]=	       0;
     m_EtaROImin[0][24][28]=	    0;m_EtaROImax[0][24][28]=	     0;m_PhiROImin[0][24][28]=	      0;m_PhiROImax[0][24][28]=	       0;
     m_EtaROImin[0][24][29]=	    0;m_EtaROImax[0][24][29]=	     0;m_PhiROImin[0][24][29]=	      0;m_PhiROImax[0][24][29]=	       0;
     m_EtaROImin[0][24][30]=	    0;m_EtaROImax[0][24][30]=	     0;m_PhiROImin[0][24][30]=	      0;m_PhiROImax[0][24][30]=	       0;
     m_EtaROImin[0][24][31]=	    0;m_EtaROImax[0][24][31]=	     0;m_PhiROImin[0][24][31]=	      0;m_PhiROImax[0][24][31]=	       0;
     m_EtaROImin[0][25][ 0]=  -0.203032;m_EtaROImax[0][25][ 0]=  -0.155043;m_PhiROImin[0][25][ 0]=   -1.26484;m_PhiROImax[0][25][ 0]=	-1.18089;
     m_EtaROImin[0][25][ 1]=   -0.15183;m_EtaROImax[0][25][ 1]= -0.0775634;m_PhiROImin[0][25][ 1]=   -1.26484;m_PhiROImax[0][25][ 1]=	-1.18089;
     m_EtaROImin[0][25][ 2]=  -0.203032;m_EtaROImax[0][25][ 2]=  -0.155043;m_PhiROImin[0][25][ 2]=   -1.35115;m_PhiROImax[0][25][ 2]=	-1.26847;
     m_EtaROImin[0][25][ 3]=   -0.15183;m_EtaROImax[0][25][ 3]= -0.0775634;m_PhiROImin[0][25][ 3]=   -1.35115;m_PhiROImax[0][25][ 3]=	-1.26847;
     m_EtaROImin[0][25][ 4]=  -0.322732;m_EtaROImax[0][25][ 4]=  -0.276301;m_PhiROImin[0][25][ 4]=   -1.26443;m_PhiROImax[0][25][ 4]=	-1.18087;
     m_EtaROImin[0][25][ 5]=  -0.273184;m_EtaROImax[0][25][ 5]=  -0.200823;m_PhiROImin[0][25][ 5]=   -1.26443;m_PhiROImax[0][25][ 5]=	-1.18087;
     m_EtaROImin[0][25][ 6]=  -0.322732;m_EtaROImax[0][25][ 6]=  -0.276301;m_PhiROImin[0][25][ 6]=   -1.35034;m_PhiROImax[0][25][ 6]=	-1.26804;
     m_EtaROImin[0][25][ 7]=  -0.273184;m_EtaROImax[0][25][ 7]=  -0.200823;m_PhiROImin[0][25][ 7]=   -1.35034;m_PhiROImax[0][25][ 7]=	-1.26804;
     m_EtaROImin[0][25][ 8]=  -0.516583;m_EtaROImax[0][25][ 8]=  -0.440504;m_PhiROImin[0][25][ 8]=   -1.25967;m_PhiROImax[0][25][ 8]=	-1.18023;
     m_EtaROImin[0][25][ 9]=  -0.439522;m_EtaROImax[0][25][ 9]=  -0.361231;m_PhiROImin[0][25][ 9]=   -1.25967;m_PhiROImax[0][25][ 9]=	-1.18023;
     m_EtaROImin[0][25][10]=  -0.516583;m_EtaROImax[0][25][10]=  -0.440504;m_PhiROImin[0][25][10]=   -1.34059;m_PhiROImax[0][25][10]=	-1.26222;
     m_EtaROImin[0][25][11]=  -0.439522;m_EtaROImax[0][25][11]=  -0.361231;m_PhiROImin[0][25][11]=   -1.34059;m_PhiROImax[0][25][11]=	-1.26222;
     m_EtaROImin[0][25][12]=  -0.659332;m_EtaROImax[0][25][12]=  -0.566067;m_PhiROImin[0][25][12]=   -1.26484;m_PhiROImax[0][25][12]=	-1.18089;
     m_EtaROImin[0][25][13]=  -0.569141;m_EtaROImax[0][25][13]=  -0.470878;m_PhiROImin[0][25][13]=   -1.26484;m_PhiROImax[0][25][13]=	-1.18089;
     m_EtaROImin[0][25][14]=  -0.659332;m_EtaROImax[0][25][14]=  -0.566067;m_PhiROImin[0][25][14]=   -1.35115;m_PhiROImax[0][25][14]=	-1.26847;
     m_EtaROImin[0][25][15]=  -0.569141;m_EtaROImax[0][25][15]=  -0.470878;m_PhiROImin[0][25][15]=   -1.35115;m_PhiROImax[0][25][15]=	-1.26847;
     m_EtaROImin[0][25][16]=  -0.724075;m_EtaROImax[0][25][16]=  -0.673648;m_PhiROImin[0][25][16]=   -1.25967;m_PhiROImax[0][25][16]=	-1.18023;
     m_EtaROImin[0][25][17]=  -0.672035;m_EtaROImax[0][25][17]=  -0.620212;m_PhiROImin[0][25][17]=   -1.25967;m_PhiROImax[0][25][17]=	-1.18023;
     m_EtaROImin[0][25][18]=  -0.724075;m_EtaROImax[0][25][18]=  -0.673648;m_PhiROImin[0][25][18]=   -1.34059;m_PhiROImax[0][25][18]=	-1.26222;
     m_EtaROImin[0][25][19]=  -0.672035;m_EtaROImax[0][25][19]=  -0.620212;m_PhiROImin[0][25][19]=   -1.34059;m_PhiROImax[0][25][19]=	-1.26222;
     m_EtaROImin[0][25][20]=  -0.893028;m_EtaROImax[0][25][20]=  -0.833797;m_PhiROImin[0][25][20]=   -1.25997;m_PhiROImax[0][25][20]=	-1.18024;
     m_EtaROImin[0][25][21]=  -0.831846;m_EtaROImax[0][25][21]=  -0.744499;m_PhiROImin[0][25][21]=   -1.25997;m_PhiROImax[0][25][21]=	-1.18024;
     m_EtaROImin[0][25][22]=  -0.893028;m_EtaROImax[0][25][22]=  -0.833797;m_PhiROImin[0][25][22]=   -1.34117;m_PhiROImax[0][25][22]=	-1.26253;
     m_EtaROImin[0][25][23]=  -0.831846;m_EtaROImax[0][25][23]=  -0.744499;m_PhiROImin[0][25][23]=   -1.34117;m_PhiROImax[0][25][23]=	-1.26253;
     m_EtaROImin[0][25][24]=  -0.905303;m_EtaROImax[0][25][24]=  -0.846464;m_PhiROImin[0][25][24]=   -1.26484;m_PhiROImax[0][25][24]=	-1.18089;
     m_EtaROImin[0][25][25]=  -0.850339;m_EtaROImax[0][25][25]=  -0.788882;m_PhiROImin[0][25][25]=   -1.26484;m_PhiROImax[0][25][25]=	-1.18089;
     m_EtaROImin[0][25][26]=  -0.905303;m_EtaROImax[0][25][26]=  -0.846464;m_PhiROImin[0][25][26]=   -1.35115;m_PhiROImax[0][25][26]=	-1.26847;
     m_EtaROImin[0][25][27]=  -0.850339;m_EtaROImax[0][25][27]=  -0.788882;m_PhiROImin[0][25][27]=   -1.35115;m_PhiROImax[0][25][27]=	-1.26847;
     m_EtaROImin[0][25][28]=  -0.984405;m_EtaROImax[0][25][28]=  -0.957812;m_PhiROImin[0][25][28]=   -1.20642;m_PhiROImax[0][25][28]=	-1.15713;
     m_EtaROImin[0][25][29]=   -0.95602;m_EtaROImax[0][25][29]=  -0.914143;m_PhiROImin[0][25][29]=   -1.20642;m_PhiROImax[0][25][29]=	-1.15713;
     m_EtaROImin[0][25][30]=  -0.984405;m_EtaROImax[0][25][30]=  -0.957812;m_PhiROImin[0][25][30]=   -1.29487;m_PhiROImax[0][25][30]=	 -1.2092;
     m_EtaROImin[0][25][31]=   -0.95602;m_EtaROImax[0][25][31]=  -0.914143;m_PhiROImin[0][25][31]=   -1.29487;m_PhiROImax[0][25][31]=	 -1.2092;
     m_EtaROImin[0][26][ 0]=  -0.126072;m_EtaROImax[0][26][ 0]= -0.0775634;m_PhiROImin[0][26][ 0]=   -1.17531;m_PhiROImax[0][26][ 0]=	-1.09135;
     m_EtaROImin[0][26][ 1]=  -0.203032;m_EtaROImax[0][26][ 1]=  -0.129296;m_PhiROImin[0][26][ 1]=   -1.17531;m_PhiROImax[0][26][ 1]=	-1.09135;
     m_EtaROImin[0][26][ 2]=  -0.126072;m_EtaROImax[0][26][ 2]= -0.0775634;m_PhiROImin[0][26][ 2]=   -1.08772;m_PhiROImax[0][26][ 2]=	-1.00505;
     m_EtaROImin[0][26][ 3]=  -0.203032;m_EtaROImax[0][26][ 3]=  -0.129296;m_PhiROImin[0][26][ 3]=   -1.08772;m_PhiROImax[0][26][ 3]=	-1.00505;
     m_EtaROImin[0][26][ 4]=  -0.248155;m_EtaROImax[0][26][ 4]=  -0.200823;m_PhiROImin[0][26][ 4]=   -1.17532;m_PhiROImax[0][26][ 4]=	-1.09176;
     m_EtaROImin[0][26][ 5]=  -0.322732;m_EtaROImax[0][26][ 5]=  -0.251292;m_PhiROImin[0][26][ 5]=   -1.17532;m_PhiROImax[0][26][ 5]=	-1.09176;
     m_EtaROImin[0][26][ 6]=  -0.248155;m_EtaROImax[0][26][ 6]=  -0.200823;m_PhiROImin[0][26][ 6]=   -1.08815;m_PhiROImax[0][26][ 6]=	-1.00586;
     m_EtaROImin[0][26][ 7]=  -0.322732;m_EtaROImax[0][26][ 7]=  -0.251292;m_PhiROImin[0][26][ 7]=   -1.08815;m_PhiROImax[0][26][ 7]=	-1.00586;
     m_EtaROImin[0][26][ 8]=  -0.439522;m_EtaROImax[0][26][ 8]=  -0.361231;m_PhiROImin[0][26][ 8]=   -1.17596;m_PhiROImax[0][26][ 8]=	-1.09652;
     m_EtaROImin[0][26][ 9]=  -0.516583;m_EtaROImax[0][26][ 9]=  -0.440504;m_PhiROImin[0][26][ 9]=   -1.17596;m_PhiROImax[0][26][ 9]=	-1.09652;
     m_EtaROImin[0][26][10]=  -0.439522;m_EtaROImax[0][26][10]=  -0.361231;m_PhiROImin[0][26][10]=   -1.09397;m_PhiROImax[0][26][10]=	 -1.0156;
     m_EtaROImin[0][26][11]=  -0.516583;m_EtaROImax[0][26][11]=  -0.440504;m_PhiROImin[0][26][11]=   -1.09397;m_PhiROImax[0][26][11]=	 -1.0156;
     m_EtaROImin[0][26][12]=  -0.569141;m_EtaROImax[0][26][12]=  -0.470878;m_PhiROImin[0][26][12]=   -1.17531;m_PhiROImax[0][26][12]=	-1.09135;
     m_EtaROImin[0][26][13]=  -0.659332;m_EtaROImax[0][26][13]=  -0.566067;m_PhiROImin[0][26][13]=   -1.17531;m_PhiROImax[0][26][13]=	-1.09135;
     m_EtaROImin[0][26][14]=  -0.569141;m_EtaROImax[0][26][14]=  -0.470878;m_PhiROImin[0][26][14]=   -1.08772;m_PhiROImax[0][26][14]=	-1.00505;
     m_EtaROImin[0][26][15]=  -0.659332;m_EtaROImax[0][26][15]=  -0.566067;m_PhiROImin[0][26][15]=   -1.08772;m_PhiROImax[0][26][15]=	-1.00505;
     m_EtaROImin[0][26][16]=  -0.672035;m_EtaROImax[0][26][16]=  -0.620212;m_PhiROImin[0][26][16]=   -1.17596;m_PhiROImax[0][26][16]=	-1.09652;
     m_EtaROImin[0][26][17]=  -0.724075;m_EtaROImax[0][26][17]=  -0.673648;m_PhiROImin[0][26][17]=   -1.17596;m_PhiROImax[0][26][17]=	-1.09652;
     m_EtaROImin[0][26][18]=  -0.672035;m_EtaROImax[0][26][18]=  -0.620212;m_PhiROImin[0][26][18]=   -1.09397;m_PhiROImax[0][26][18]=	 -1.0156;
     m_EtaROImin[0][26][19]=  -0.724075;m_EtaROImax[0][26][19]=  -0.673648;m_PhiROImin[0][26][19]=   -1.09397;m_PhiROImax[0][26][19]=	 -1.0156;
     m_EtaROImin[0][26][20]=  -0.831846;m_EtaROImax[0][26][20]=  -0.744499;m_PhiROImin[0][26][20]=   -1.17595;m_PhiROImax[0][26][20]=	-1.09623;
     m_EtaROImin[0][26][21]=  -0.893028;m_EtaROImax[0][26][21]=  -0.833797;m_PhiROImin[0][26][21]=   -1.17595;m_PhiROImax[0][26][21]=	-1.09623;
     m_EtaROImin[0][26][22]=  -0.831846;m_EtaROImax[0][26][22]=  -0.744499;m_PhiROImin[0][26][22]=   -1.09367;m_PhiROImax[0][26][22]=	-1.01502;
     m_EtaROImin[0][26][23]=  -0.893028;m_EtaROImax[0][26][23]=  -0.833797;m_PhiROImin[0][26][23]=   -1.09367;m_PhiROImax[0][26][23]=	-1.01502;
     m_EtaROImin[0][26][24]=  -0.850339;m_EtaROImax[0][26][24]=  -0.788882;m_PhiROImin[0][26][24]=   -1.17531;m_PhiROImax[0][26][24]=	-1.09135;
     m_EtaROImin[0][26][25]=  -0.905303;m_EtaROImax[0][26][25]=  -0.846464;m_PhiROImin[0][26][25]=   -1.17531;m_PhiROImax[0][26][25]=	-1.09135;
     m_EtaROImin[0][26][26]=  -0.850339;m_EtaROImax[0][26][26]=  -0.788882;m_PhiROImin[0][26][26]=   -1.08772;m_PhiROImax[0][26][26]=	-1.00505;
     m_EtaROImin[0][26][27]=  -0.905303;m_EtaROImax[0][26][27]=  -0.846464;m_PhiROImin[0][26][27]=   -1.08772;m_PhiROImax[0][26][27]=	-1.00505;
     m_EtaROImin[0][26][28]=  -0.939363;m_EtaROImax[0][26][28]=  -0.911945;m_PhiROImin[0][26][28]=   -1.14955;m_PhiROImax[0][26][28]=	-1.06384;
     m_EtaROImin[0][26][29]=   -0.98211;m_EtaROImax[0][26][29]=  -0.941171;m_PhiROImin[0][26][29]=   -1.14955;m_PhiROImax[0][26][29]=	-1.06384;
     m_EtaROImin[0][26][30]=  -0.939363;m_EtaROImax[0][26][30]=  -0.911945;m_PhiROImin[0][26][30]=   -1.15713;m_PhiROImax[0][26][30]=	 -1.0611;
     m_EtaROImin[0][26][31]=   -0.98211;m_EtaROImax[0][26][31]=  -0.941171;m_PhiROImin[0][26][31]=   -1.15713;m_PhiROImax[0][26][31]=	 -1.0611;
     m_EtaROImin[0][27][ 0]=  -0.238294;m_EtaROImax[0][27][ 0]=  -0.180742;m_PhiROImin[0][27][ 0]=   -0.89861;m_PhiROImax[0][27][ 0]=  -0.788366;
     m_EtaROImin[0][27][ 1]=  -0.176882;m_EtaROImax[0][27][ 1]= -0.0874905;m_PhiROImin[0][27][ 1]=   -0.89861;m_PhiROImax[0][27][ 1]=  -0.788366;
     m_EtaROImin[0][27][ 2]=  -0.238294;m_EtaROImax[0][27][ 2]=  -0.180742;m_PhiROImin[0][27][ 2]=   -1.00957;m_PhiROImax[0][27][ 2]=  -0.902135;
     m_EtaROImin[0][27][ 3]=  -0.176882;m_EtaROImax[0][27][ 3]= -0.0874905;m_PhiROImin[0][27][ 3]=   -1.00957;m_PhiROImax[0][27][ 3]=  -0.902135;
     m_EtaROImin[0][27][ 4]=  -0.452099;m_EtaROImax[0][27][ 4]=  -0.350831;m_PhiROImin[0][27][ 4]=  -0.898011;m_PhiROImax[0][27][ 4]=	-0.78835;
     m_EtaROImin[0][27][ 5]=   -0.35049;m_EtaROImax[0][27][ 5]=  -0.246207;m_PhiROImin[0][27][ 5]=  -0.898011;m_PhiROImax[0][27][ 5]=	-0.78835;
     m_EtaROImin[0][27][ 6]=  -0.452099;m_EtaROImax[0][27][ 6]=  -0.350831;m_PhiROImin[0][27][ 6]=   -1.00841;m_PhiROImax[0][27][ 6]=  -0.901517;
     m_EtaROImin[0][27][ 7]=   -0.35049;m_EtaROImax[0][27][ 7]=  -0.246207;m_PhiROImin[0][27][ 7]=   -1.00841;m_PhiROImax[0][27][ 7]=  -0.901517;
     m_EtaROImin[0][27][ 8]=  -0.648101;m_EtaROImax[0][27][ 8]=  -0.555789;m_PhiROImin[0][27][ 8]=  -0.898011;m_PhiROImax[0][27][ 8]=	-0.78835;
     m_EtaROImin[0][27][ 9]=  -0.554444;m_EtaROImax[0][27][ 9]=  -0.458091;m_PhiROImin[0][27][ 9]=  -0.898011;m_PhiROImax[0][27][ 9]=	-0.78835;
     m_EtaROImin[0][27][10]=  -0.648101;m_EtaROImax[0][27][10]=  -0.555789;m_PhiROImin[0][27][10]=   -1.00841;m_PhiROImax[0][27][10]=  -0.901517;
     m_EtaROImin[0][27][11]=  -0.554444;m_EtaROImax[0][27][11]=  -0.458091;m_PhiROImin[0][27][11]=   -1.00841;m_PhiROImax[0][27][11]=  -0.901517;
     m_EtaROImin[0][27][12]=  -0.776449;m_EtaROImax[0][27][12]=  -0.731124;m_PhiROImin[0][27][12]=   -0.89861;m_PhiROImax[0][27][12]=  -0.788366;
     m_EtaROImin[0][27][13]=  -0.728056;m_EtaROImax[0][27][13]=  -0.655857;m_PhiROImin[0][27][13]=   -0.89861;m_PhiROImax[0][27][13]=  -0.788366;
     m_EtaROImin[0][27][14]=  -0.776449;m_EtaROImax[0][27][14]=  -0.731124;m_PhiROImin[0][27][14]=   -1.00957;m_PhiROImax[0][27][14]=  -0.902135;
     m_EtaROImin[0][27][15]=  -0.728056;m_EtaROImax[0][27][15]=  -0.655857;m_PhiROImin[0][27][15]=   -1.00957;m_PhiROImax[0][27][15]=  -0.902135;
     m_EtaROImin[0][27][16]=  -0.895163;m_EtaROImax[0][27][16]=  -0.853359;m_PhiROImin[0][27][16]=   -0.89861;m_PhiROImax[0][27][16]=  -0.788366;
     m_EtaROImin[0][27][17]=  -0.850528;m_EtaROImax[0][27][17]=  -0.783867;m_PhiROImin[0][27][17]=   -0.89861;m_PhiROImax[0][27][17]=  -0.788366;
     m_EtaROImin[0][27][18]=  -0.895163;m_EtaROImax[0][27][18]=  -0.853359;m_PhiROImin[0][27][18]=   -1.00957;m_PhiROImax[0][27][18]=  -0.902135;
     m_EtaROImin[0][27][19]=  -0.850528;m_EtaROImax[0][27][19]=  -0.783867;m_PhiROImin[0][27][19]=   -1.00957;m_PhiROImax[0][27][19]=  -0.902135;
     m_EtaROImin[0][27][20]=   -1.02514;m_EtaROImax[0][27][20]=  -0.964674;m_PhiROImin[0][27][20]=  -0.898011;m_PhiROImax[0][27][20]=	-0.78835;
     m_EtaROImin[0][27][21]=  -0.961344;m_EtaROImax[0][27][21]=  -0.898201;m_PhiROImin[0][27][21]=  -0.898011;m_PhiROImax[0][27][21]=	-0.78835;
     m_EtaROImin[0][27][22]=   -1.02514;m_EtaROImax[0][27][22]=  -0.964674;m_PhiROImin[0][27][22]=   -1.00841;m_PhiROImax[0][27][22]=  -0.901517;
     m_EtaROImin[0][27][23]=  -0.961344;m_EtaROImax[0][27][23]=  -0.898201;m_PhiROImin[0][27][23]=   -1.00841;m_PhiROImax[0][27][23]=  -0.901517;
     m_EtaROImin[0][27][24]=	    0;m_EtaROImax[0][27][24]=	     0;m_PhiROImin[0][27][24]=	      0;m_PhiROImax[0][27][24]=	       0;
     m_EtaROImin[0][27][25]=   -1.06547;m_EtaROImax[0][27][25]=   -1.03003;m_PhiROImin[0][27][25]=  -0.898827;m_PhiROImax[0][27][25]=  -0.788467;
     m_EtaROImin[0][27][26]=	    0;m_EtaROImax[0][27][26]=	     0;m_PhiROImin[0][27][26]=	      0;m_PhiROImax[0][27][26]=	       0;
     m_EtaROImin[0][27][27]=   -1.06547;m_EtaROImax[0][27][27]=   -1.03003;m_PhiROImin[0][27][27]=  -0.982541;m_PhiROImax[0][27][27]=  -0.902356;
     m_EtaROImin[0][27][28]=	    0;m_EtaROImax[0][27][28]=	     0;m_PhiROImin[0][27][28]=	      0;m_PhiROImax[0][27][28]=	       0;
     m_EtaROImin[0][27][29]=	    0;m_EtaROImax[0][27][29]=	     0;m_PhiROImin[0][27][29]=	      0;m_PhiROImax[0][27][29]=	       0;
     m_EtaROImin[0][27][30]=	    0;m_EtaROImax[0][27][30]=	     0;m_PhiROImin[0][27][30]=	      0;m_PhiROImax[0][27][30]=	       0;
     m_EtaROImin[0][27][31]=	    0;m_EtaROImax[0][27][31]=	     0;m_PhiROImin[0][27][31]=	      0;m_PhiROImax[0][27][31]=	       0;
     m_EtaROImin[0][28][ 0]=  -0.145912;m_EtaROImax[0][28][ 0]= -0.0874905;m_PhiROImin[0][28][ 0]=   -0.78243;m_PhiROImax[0][28][ 0]=  -0.672186;
     m_EtaROImin[0][28][ 1]=  -0.238294;m_EtaROImax[0][28][ 1]=  -0.149792;m_PhiROImin[0][28][ 1]=   -0.78243;m_PhiROImax[0][28][ 1]=  -0.672186;
     m_EtaROImin[0][28][ 2]=  -0.145912;m_EtaROImax[0][28][ 2]= -0.0874905;m_PhiROImin[0][28][ 2]=  -0.668661;m_PhiROImax[0][28][ 2]=  -0.561229;
     m_EtaROImin[0][28][ 3]=  -0.238294;m_EtaROImax[0][28][ 3]=  -0.149792;m_PhiROImin[0][28][ 3]=  -0.668661;m_PhiROImax[0][28][ 3]=  -0.561229;
     m_EtaROImin[0][28][ 4]=   -0.35049;m_EtaROImax[0][28][ 4]=  -0.246207;m_PhiROImin[0][28][ 4]=  -0.782446;m_PhiROImax[0][28][ 4]=  -0.672786;
     m_EtaROImin[0][28][ 5]=  -0.452099;m_EtaROImax[0][28][ 5]=  -0.350831;m_PhiROImin[0][28][ 5]=  -0.782446;m_PhiROImax[0][28][ 5]=  -0.672786;
     m_EtaROImin[0][28][ 6]=   -0.35049;m_EtaROImax[0][28][ 6]=  -0.246207;m_PhiROImin[0][28][ 6]=  -0.669279;m_PhiROImax[0][28][ 6]=  -0.562387;
     m_EtaROImin[0][28][ 7]=  -0.452099;m_EtaROImax[0][28][ 7]=  -0.350831;m_PhiROImin[0][28][ 7]=  -0.669279;m_PhiROImax[0][28][ 7]=  -0.562387;
     m_EtaROImin[0][28][ 8]=  -0.554444;m_EtaROImax[0][28][ 8]=  -0.458091;m_PhiROImin[0][28][ 8]=  -0.782446;m_PhiROImax[0][28][ 8]=  -0.672786;
     m_EtaROImin[0][28][ 9]=  -0.648101;m_EtaROImax[0][28][ 9]=  -0.555789;m_PhiROImin[0][28][ 9]=  -0.782446;m_PhiROImax[0][28][ 9]=  -0.672786;
     m_EtaROImin[0][28][10]=  -0.554444;m_EtaROImax[0][28][10]=  -0.458091;m_PhiROImin[0][28][10]=  -0.669279;m_PhiROImax[0][28][10]=  -0.562387;
     m_EtaROImin[0][28][11]=  -0.648101;m_EtaROImax[0][28][11]=  -0.555789;m_PhiROImin[0][28][11]=  -0.669279;m_PhiROImax[0][28][11]=  -0.562387;
     m_EtaROImin[0][28][12]=  -0.703298;m_EtaROImax[0][28][12]=  -0.655857;m_PhiROImin[0][28][12]=   -0.78243;m_PhiROImax[0][28][12]=  -0.672186;
     m_EtaROImin[0][28][13]=  -0.776449;m_EtaROImax[0][28][13]=  -0.706413;m_PhiROImin[0][28][13]=   -0.78243;m_PhiROImax[0][28][13]=  -0.672186;
     m_EtaROImin[0][28][14]=  -0.703298;m_EtaROImax[0][28][14]=  -0.655857;m_PhiROImin[0][28][14]=  -0.668661;m_PhiROImax[0][28][14]=  -0.561229;
     m_EtaROImin[0][28][15]=  -0.776449;m_EtaROImax[0][28][15]=  -0.706413;m_PhiROImin[0][28][15]=  -0.668661;m_PhiROImax[0][28][15]=  -0.561229;
     m_EtaROImin[0][28][16]=   -0.82768;m_EtaROImax[0][28][16]=  -0.783867;m_PhiROImin[0][28][16]=   -0.78243;m_PhiROImax[0][28][16]=  -0.672186;
     m_EtaROImin[0][28][17]=  -0.895163;m_EtaROImax[0][28][17]=  -0.830556;m_PhiROImin[0][28][17]=   -0.78243;m_PhiROImax[0][28][17]=  -0.672186;
     m_EtaROImin[0][28][18]=   -0.82768;m_EtaROImax[0][28][18]=  -0.783867;m_PhiROImin[0][28][18]=  -0.668661;m_PhiROImax[0][28][18]=  -0.561229;
     m_EtaROImin[0][28][19]=  -0.895163;m_EtaROImax[0][28][19]=  -0.830556;m_PhiROImin[0][28][19]=  -0.668661;m_PhiROImax[0][28][19]=  -0.561229;
     m_EtaROImin[0][28][20]=  -0.961344;m_EtaROImax[0][28][20]=  -0.898201;m_PhiROImin[0][28][20]=  -0.782446;m_PhiROImax[0][28][20]=  -0.672786;
     m_EtaROImin[0][28][21]=   -1.02514;m_EtaROImax[0][28][21]=  -0.964674;m_PhiROImin[0][28][21]=  -0.782446;m_PhiROImax[0][28][21]=  -0.672786;
     m_EtaROImin[0][28][22]=  -0.961344;m_EtaROImax[0][28][22]=  -0.898201;m_PhiROImin[0][28][22]=  -0.669279;m_PhiROImax[0][28][22]=  -0.562387;
     m_EtaROImin[0][28][23]=   -1.02514;m_EtaROImax[0][28][23]=  -0.964674;m_PhiROImin[0][28][23]=  -0.669279;m_PhiROImax[0][28][23]=  -0.562387;
     m_EtaROImin[0][28][24]=   -1.06547;m_EtaROImax[0][28][24]=   -1.03003;m_PhiROImin[0][28][24]=  -0.782329;m_PhiROImax[0][28][24]=  -0.671969;
     m_EtaROImin[0][28][25]=	    0;m_EtaROImax[0][28][25]=	     0;m_PhiROImin[0][28][25]=	      0;m_PhiROImax[0][28][25]=	       0;
     m_EtaROImin[0][28][26]=   -1.06547;m_EtaROImax[0][28][26]=   -1.03003;m_PhiROImin[0][28][26]=   -0.66844;m_PhiROImax[0][28][26]=  -0.588255;
     m_EtaROImin[0][28][27]=	    0;m_EtaROImax[0][28][27]=	     0;m_PhiROImin[0][28][27]=	      0;m_PhiROImax[0][28][27]=	       0;
     m_EtaROImin[0][28][28]=	    0;m_EtaROImax[0][28][28]=	     0;m_PhiROImin[0][28][28]=	      0;m_PhiROImax[0][28][28]=	       0;
     m_EtaROImin[0][28][29]=	    0;m_EtaROImax[0][28][29]=	     0;m_PhiROImin[0][28][29]=	      0;m_PhiROImax[0][28][29]=	       0;
     m_EtaROImin[0][28][30]=	    0;m_EtaROImax[0][28][30]=	     0;m_PhiROImin[0][28][30]=	      0;m_PhiROImax[0][28][30]=	       0;
     m_EtaROImin[0][28][31]=	    0;m_EtaROImax[0][28][31]=	     0;m_PhiROImin[0][28][31]=	      0;m_PhiROImax[0][28][31]=	       0;
     m_EtaROImin[0][29][ 0]=  -0.213185;m_EtaROImax[0][29][ 0]=  -0.116816;m_PhiROImin[0][29][ 0]=  -0.479445;m_PhiROImax[0][29][ 0]=  -0.395489;
     m_EtaROImin[0][29][ 1]=  -0.118734;m_EtaROImax[0][29][ 1]= -0.0208251;m_PhiROImin[0][29][ 1]=  -0.479445;m_PhiROImax[0][29][ 1]=  -0.395489;
     m_EtaROImin[0][29][ 2]=  -0.213185;m_EtaROImax[0][29][ 2]=  -0.116816;m_PhiROImin[0][29][ 2]=  -0.565749;m_PhiROImax[0][29][ 2]=  -0.483076;
     m_EtaROImin[0][29][ 3]=  -0.118734;m_EtaROImax[0][29][ 3]= -0.0208251;m_PhiROImin[0][29][ 3]=  -0.565749;m_PhiROImax[0][29][ 3]=  -0.483076;
     m_EtaROImin[0][29][ 4]=  -0.416721;m_EtaROImax[0][29][ 4]=   -0.30075;m_PhiROImin[0][29][ 4]=  -0.479445;m_PhiROImax[0][29][ 4]=  -0.395489;
     m_EtaROImin[0][29][ 5]=  -0.302928;m_EtaROImax[0][29][ 5]=  -0.222449;m_PhiROImin[0][29][ 5]=  -0.479445;m_PhiROImax[0][29][ 5]=  -0.395489;
     m_EtaROImin[0][29][ 6]=  -0.416721;m_EtaROImax[0][29][ 6]=   -0.30075;m_PhiROImin[0][29][ 6]=  -0.565749;m_PhiROImax[0][29][ 6]=  -0.483076;
     m_EtaROImin[0][29][ 7]=  -0.302928;m_EtaROImax[0][29][ 7]=  -0.222449;m_PhiROImin[0][29][ 7]=  -0.565749;m_PhiROImax[0][29][ 7]=  -0.483076;
     m_EtaROImin[0][29][ 8]=  -0.573871;m_EtaROImax[0][29][ 8]=  -0.501681;m_PhiROImin[0][29][ 8]=  -0.479445;m_PhiROImax[0][29][ 8]=  -0.395489;
     m_EtaROImin[0][29][ 9]=  -0.504617;m_EtaROImax[0][29][ 9]=   -0.42967;m_PhiROImin[0][29][ 9]=  -0.479445;m_PhiROImax[0][29][ 9]=  -0.395489;
     m_EtaROImin[0][29][10]=  -0.573871;m_EtaROImax[0][29][10]=  -0.501681;m_PhiROImin[0][29][10]=  -0.565749;m_PhiROImax[0][29][10]=  -0.483076;
     m_EtaROImin[0][29][11]=  -0.504617;m_EtaROImax[0][29][11]=   -0.42967;m_PhiROImin[0][29][11]=  -0.565749;m_PhiROImax[0][29][11]=  -0.483076;
     m_EtaROImin[0][29][12]=  -0.741516;m_EtaROImax[0][29][12]=  -0.649933;m_PhiROImin[0][29][12]=  -0.479445;m_PhiROImax[0][29][12]=  -0.395489;
     m_EtaROImin[0][29][13]=  -0.653329;m_EtaROImax[0][29][13]=  -0.583785;m_PhiROImin[0][29][13]=  -0.479445;m_PhiROImax[0][29][13]=  -0.395489;
     m_EtaROImin[0][29][14]=  -0.741516;m_EtaROImax[0][29][14]=  -0.649933;m_PhiROImin[0][29][14]=  -0.565749;m_PhiROImax[0][29][14]=  -0.483076;
     m_EtaROImin[0][29][15]=  -0.653329;m_EtaROImax[0][29][15]=  -0.583785;m_PhiROImin[0][29][15]=  -0.565749;m_PhiROImax[0][29][15]=  -0.483076;
     m_EtaROImin[0][29][16]=	    0;m_EtaROImax[0][29][16]=	     0;m_PhiROImin[0][29][16]=	      0;m_PhiROImax[0][29][16]=	       0;
     m_EtaROImin[0][29][17]=  -0.837822;m_EtaROImax[0][29][17]=  -0.756521;m_PhiROImin[0][29][17]=  -0.479445;m_PhiROImax[0][29][17]=  -0.395489;
     m_EtaROImin[0][29][18]=	    0;m_EtaROImax[0][29][18]=	     0;m_PhiROImin[0][29][18]=	      0;m_PhiROImax[0][29][18]=	       0;
     m_EtaROImin[0][29][19]=  -0.837822;m_EtaROImax[0][29][19]=  -0.756521;m_PhiROImin[0][29][19]=  -0.565749;m_PhiROImax[0][29][19]=  -0.483076;
     m_EtaROImin[0][29][20]=  -0.956037;m_EtaROImax[0][29][20]=  -0.899344;m_PhiROImin[0][29][20]=  -0.479445;m_PhiROImax[0][29][20]=  -0.395489;
     m_EtaROImin[0][29][21]=  -0.903324;m_EtaROImax[0][29][21]=  -0.844116;m_PhiROImin[0][29][21]=  -0.479445;m_PhiROImax[0][29][21]=  -0.395489;
     m_EtaROImin[0][29][22]=  -0.956037;m_EtaROImax[0][29][22]=  -0.899344;m_PhiROImin[0][29][22]=  -0.565749;m_PhiROImax[0][29][22]=  -0.483076;
     m_EtaROImin[0][29][23]=  -0.903324;m_EtaROImax[0][29][23]=  -0.844116;m_PhiROImin[0][29][23]=  -0.565749;m_PhiROImax[0][29][23]=  -0.483076;
     m_EtaROImin[0][29][24]=	    0;m_EtaROImax[0][29][24]=	     0;m_PhiROImin[0][29][24]=	      0;m_PhiROImax[0][29][24]=	       0;
     m_EtaROImin[0][29][25]=	    0;m_EtaROImax[0][29][25]=	     0;m_PhiROImin[0][29][25]=	      0;m_PhiROImax[0][29][25]=	       0;
     m_EtaROImin[0][29][26]=	    0;m_EtaROImax[0][29][26]=	     0;m_PhiROImin[0][29][26]=	      0;m_PhiROImax[0][29][26]=	       0;
     m_EtaROImin[0][29][27]=	    0;m_EtaROImax[0][29][27]=	     0;m_PhiROImin[0][29][27]=	      0;m_PhiROImax[0][29][27]=	       0;
     m_EtaROImin[0][29][28]=	    0;m_EtaROImax[0][29][28]=	     0;m_PhiROImin[0][29][28]=	      0;m_PhiROImax[0][29][28]=	       0;
     m_EtaROImin[0][29][29]=	    0;m_EtaROImax[0][29][29]=	     0;m_PhiROImin[0][29][29]=	      0;m_PhiROImax[0][29][29]=	       0;
     m_EtaROImin[0][29][30]=	    0;m_EtaROImax[0][29][30]=	     0;m_PhiROImin[0][29][30]=	      0;m_PhiROImax[0][29][30]=	       0;
     m_EtaROImin[0][29][31]=	    0;m_EtaROImax[0][29][31]=	     0;m_PhiROImin[0][29][31]=	      0;m_PhiROImax[0][29][31]=	       0;
     m_EtaROImin[0][30][ 0]=  -0.118734;m_EtaROImax[0][30][ 0]= -0.0208251;m_PhiROImin[0][30][ 0]=  -0.389909;m_PhiROImax[0][30][ 0]=  -0.305953;
     m_EtaROImin[0][30][ 1]=  -0.213185;m_EtaROImax[0][30][ 1]=  -0.116816;m_PhiROImin[0][30][ 1]=  -0.389909;m_PhiROImax[0][30][ 1]=  -0.305953;
     m_EtaROImin[0][30][ 2]=  -0.118734;m_EtaROImax[0][30][ 2]= -0.0208251;m_PhiROImin[0][30][ 2]=  -0.302322;m_PhiROImax[0][30][ 2]=  -0.219649;
     m_EtaROImin[0][30][ 3]=  -0.213185;m_EtaROImax[0][30][ 3]=  -0.116816;m_PhiROImin[0][30][ 3]=  -0.302322;m_PhiROImax[0][30][ 3]=  -0.219649;
     m_EtaROImin[0][30][ 4]=  -0.302928;m_EtaROImax[0][30][ 4]=  -0.222449;m_PhiROImin[0][30][ 4]=  -0.389909;m_PhiROImax[0][30][ 4]=  -0.305953;
     m_EtaROImin[0][30][ 5]=  -0.416721;m_EtaROImax[0][30][ 5]=   -0.30075;m_PhiROImin[0][30][ 5]=  -0.389909;m_PhiROImax[0][30][ 5]=  -0.305953;
     m_EtaROImin[0][30][ 6]=  -0.302928;m_EtaROImax[0][30][ 6]=  -0.222449;m_PhiROImin[0][30][ 6]=  -0.302322;m_PhiROImax[0][30][ 6]=  -0.219649;
     m_EtaROImin[0][30][ 7]=  -0.416721;m_EtaROImax[0][30][ 7]=   -0.30075;m_PhiROImin[0][30][ 7]=  -0.302322;m_PhiROImax[0][30][ 7]=  -0.219649;
     m_EtaROImin[0][30][ 8]=  -0.504617;m_EtaROImax[0][30][ 8]=   -0.42967;m_PhiROImin[0][30][ 8]=  -0.389909;m_PhiROImax[0][30][ 8]=  -0.305953;
     m_EtaROImin[0][30][ 9]=  -0.573871;m_EtaROImax[0][30][ 9]=  -0.501681;m_PhiROImin[0][30][ 9]=  -0.389909;m_PhiROImax[0][30][ 9]=  -0.305953;
     m_EtaROImin[0][30][10]=  -0.504617;m_EtaROImax[0][30][10]=   -0.42967;m_PhiROImin[0][30][10]=  -0.302322;m_PhiROImax[0][30][10]=  -0.219649;
     m_EtaROImin[0][30][11]=  -0.573871;m_EtaROImax[0][30][11]=  -0.501681;m_PhiROImin[0][30][11]=  -0.302322;m_PhiROImax[0][30][11]=  -0.219649;
     m_EtaROImin[0][30][12]=  -0.653329;m_EtaROImax[0][30][12]=  -0.583785;m_PhiROImin[0][30][12]=  -0.389909;m_PhiROImax[0][30][12]=  -0.305953;
     m_EtaROImin[0][30][13]=  -0.741516;m_EtaROImax[0][30][13]=  -0.649933;m_PhiROImin[0][30][13]=  -0.389909;m_PhiROImax[0][30][13]=  -0.305953;
     m_EtaROImin[0][30][14]=  -0.653329;m_EtaROImax[0][30][14]=  -0.583785;m_PhiROImin[0][30][14]=  -0.302322;m_PhiROImax[0][30][14]=  -0.219649;
     m_EtaROImin[0][30][15]=  -0.741516;m_EtaROImax[0][30][15]=  -0.649933;m_PhiROImin[0][30][15]=  -0.302322;m_PhiROImax[0][30][15]=  -0.219649;
     m_EtaROImin[0][30][16]=  -0.837822;m_EtaROImax[0][30][16]=  -0.756521;m_PhiROImin[0][30][16]=  -0.389909;m_PhiROImax[0][30][16]=  -0.305953;
     m_EtaROImin[0][30][17]=	    0;m_EtaROImax[0][30][17]=	     0;m_PhiROImin[0][30][17]=	      0;m_PhiROImax[0][30][17]=	       0;
     m_EtaROImin[0][30][18]=  -0.837822;m_EtaROImax[0][30][18]=  -0.756521;m_PhiROImin[0][30][18]=  -0.302322;m_PhiROImax[0][30][18]=  -0.219649;
     m_EtaROImin[0][30][19]=	    0;m_EtaROImax[0][30][19]=	     0;m_PhiROImin[0][30][19]=	      0;m_PhiROImax[0][30][19]=	       0;
     m_EtaROImin[0][30][20]=  -0.903324;m_EtaROImax[0][30][20]=  -0.844116;m_PhiROImin[0][30][20]=  -0.389909;m_PhiROImax[0][30][20]=  -0.305953;
     m_EtaROImin[0][30][21]=  -0.956037;m_EtaROImax[0][30][21]=  -0.899344;m_PhiROImin[0][30][21]=  -0.389909;m_PhiROImax[0][30][21]=  -0.305953;
     m_EtaROImin[0][30][22]=  -0.903324;m_EtaROImax[0][30][22]=  -0.844116;m_PhiROImin[0][30][22]=  -0.302322;m_PhiROImax[0][30][22]=  -0.219649;
     m_EtaROImin[0][30][23]=  -0.956037;m_EtaROImax[0][30][23]=  -0.899344;m_PhiROImin[0][30][23]=  -0.302322;m_PhiROImax[0][30][23]=  -0.219649;
     m_EtaROImin[0][30][24]=	    0;m_EtaROImax[0][30][24]=	     0;m_PhiROImin[0][30][24]=	      0;m_PhiROImax[0][30][24]=	       0;
     m_EtaROImin[0][30][25]=	    0;m_EtaROImax[0][30][25]=	     0;m_PhiROImin[0][30][25]=	      0;m_PhiROImax[0][30][25]=	       0;
     m_EtaROImin[0][30][26]=	    0;m_EtaROImax[0][30][26]=	     0;m_PhiROImin[0][30][26]=	      0;m_PhiROImax[0][30][26]=	       0;
     m_EtaROImin[0][30][27]=	    0;m_EtaROImax[0][30][27]=	     0;m_PhiROImin[0][30][27]=	      0;m_PhiROImax[0][30][27]=	       0;
     m_EtaROImin[0][30][28]=	    0;m_EtaROImax[0][30][28]=	     0;m_PhiROImin[0][30][28]=	      0;m_PhiROImax[0][30][28]=	       0;
     m_EtaROImin[0][30][29]=	    0;m_EtaROImax[0][30][29]=	     0;m_PhiROImin[0][30][29]=	      0;m_PhiROImax[0][30][29]=	       0;
     m_EtaROImin[0][30][30]=	    0;m_EtaROImax[0][30][30]=	     0;m_PhiROImin[0][30][30]=	      0;m_PhiROImax[0][30][30]=	       0;
     m_EtaROImin[0][30][31]=	    0;m_EtaROImax[0][30][31]=	     0;m_PhiROImin[0][30][31]=	      0;m_PhiROImax[0][30][31]=	       0;
     m_EtaROImin[0][31][ 0]=	    0;m_EtaROImax[0][31][ 0]=	     0;m_PhiROImin[0][31][ 0]=	      0;m_PhiROImax[0][31][ 0]=	       0;
     m_EtaROImin[0][31][ 1]=  -0.238757;m_EtaROImax[0][31][ 1]=   -0.13107;m_PhiROImin[0][31][ 1]=  -0.113212;m_PhiROImax[0][31][ 1]=-0.00296767;
     m_EtaROImin[0][31][ 2]=	    0;m_EtaROImax[0][31][ 2]=	     0;m_PhiROImin[0][31][ 2]=	      0;m_PhiROImax[0][31][ 2]=	       0;
     m_EtaROImin[0][31][ 3]=  -0.238757;m_EtaROImax[0][31][ 3]=   -0.13107;m_PhiROImin[0][31][ 3]=  -0.224169;m_PhiROImax[0][31][ 3]=  -0.116737;
     m_EtaROImin[0][31][ 4]=  -0.452099;m_EtaROImax[0][31][ 4]=  -0.350831;m_PhiROImin[0][31][ 4]=  -0.112612;m_PhiROImax[0][31][ 4]=-0.00295181;
     m_EtaROImin[0][31][ 5]=   -0.35049;m_EtaROImax[0][31][ 5]=  -0.246207;m_PhiROImin[0][31][ 5]=  -0.112612;m_PhiROImax[0][31][ 5]=-0.00295181;
     m_EtaROImin[0][31][ 6]=  -0.452099;m_EtaROImax[0][31][ 6]=  -0.350831;m_PhiROImin[0][31][ 6]=  -0.223011;m_PhiROImax[0][31][ 6]=  -0.116119;
     m_EtaROImin[0][31][ 7]=   -0.35049;m_EtaROImax[0][31][ 7]=  -0.246207;m_PhiROImin[0][31][ 7]=  -0.223011;m_PhiROImax[0][31][ 7]=  -0.116119;
     m_EtaROImin[0][31][ 8]=  -0.648101;m_EtaROImax[0][31][ 8]=  -0.555789;m_PhiROImin[0][31][ 8]=  -0.112612;m_PhiROImax[0][31][ 8]=-0.00295181;
     m_EtaROImin[0][31][ 9]=  -0.554444;m_EtaROImax[0][31][ 9]=  -0.458091;m_PhiROImin[0][31][ 9]=  -0.112612;m_PhiROImax[0][31][ 9]=-0.00295181;
     m_EtaROImin[0][31][10]=  -0.648101;m_EtaROImax[0][31][10]=  -0.555789;m_PhiROImin[0][31][10]=  -0.223011;m_PhiROImax[0][31][10]=  -0.116119;
     m_EtaROImin[0][31][11]=  -0.554444;m_EtaROImax[0][31][11]=  -0.458091;m_PhiROImin[0][31][11]=  -0.223011;m_PhiROImax[0][31][11]=  -0.116119;
     m_EtaROImin[0][31][12]=  -0.776449;m_EtaROImax[0][31][12]=  -0.731124;m_PhiROImin[0][31][12]=  -0.113212;m_PhiROImax[0][31][12]=-0.00296767;
     m_EtaROImin[0][31][13]=  -0.728056;m_EtaROImax[0][31][13]=  -0.655857;m_PhiROImin[0][31][13]=  -0.113212;m_PhiROImax[0][31][13]=-0.00296767;
     m_EtaROImin[0][31][14]=  -0.776449;m_EtaROImax[0][31][14]=  -0.731124;m_PhiROImin[0][31][14]=  -0.224169;m_PhiROImax[0][31][14]=  -0.116737;
     m_EtaROImin[0][31][15]=  -0.728056;m_EtaROImax[0][31][15]=  -0.655857;m_PhiROImin[0][31][15]=  -0.224169;m_PhiROImax[0][31][15]=  -0.116737;
     m_EtaROImin[0][31][16]=  -0.895163;m_EtaROImax[0][31][16]=  -0.853359;m_PhiROImin[0][31][16]=  -0.113212;m_PhiROImax[0][31][16]=-0.00296767;
     m_EtaROImin[0][31][17]=  -0.850528;m_EtaROImax[0][31][17]=  -0.783867;m_PhiROImin[0][31][17]=  -0.113212;m_PhiROImax[0][31][17]=-0.00296767;
     m_EtaROImin[0][31][18]=  -0.895163;m_EtaROImax[0][31][18]=  -0.853359;m_PhiROImin[0][31][18]=  -0.224169;m_PhiROImax[0][31][18]=  -0.116737;
     m_EtaROImin[0][31][19]=  -0.850528;m_EtaROImax[0][31][19]=  -0.783867;m_PhiROImin[0][31][19]=  -0.224169;m_PhiROImax[0][31][19]=  -0.116737;
     m_EtaROImin[0][31][20]=   -1.02514;m_EtaROImax[0][31][20]=  -0.964674;m_PhiROImin[0][31][20]=  -0.112612;m_PhiROImax[0][31][20]=-0.00295181;
     m_EtaROImin[0][31][21]=  -0.961344;m_EtaROImax[0][31][21]=  -0.898201;m_PhiROImin[0][31][21]=  -0.112612;m_PhiROImax[0][31][21]=-0.00295181;
     m_EtaROImin[0][31][22]=   -1.02514;m_EtaROImax[0][31][22]=  -0.964674;m_PhiROImin[0][31][22]=  -0.223011;m_PhiROImax[0][31][22]=  -0.116119;
     m_EtaROImin[0][31][23]=  -0.961344;m_EtaROImax[0][31][23]=  -0.898201;m_PhiROImin[0][31][23]=  -0.223011;m_PhiROImax[0][31][23]=  -0.116119;
     m_EtaROImin[0][31][24]=	    0;m_EtaROImax[0][31][24]=	     0;m_PhiROImin[0][31][24]=	      0;m_PhiROImax[0][31][24]=	       0;
     m_EtaROImin[0][31][25]=   -1.06547;m_EtaROImax[0][31][25]=   -1.03003;m_PhiROImin[0][31][25]=  -0.113429;m_PhiROImax[0][31][25]=-0.00306876;
     m_EtaROImin[0][31][26]=	    0;m_EtaROImax[0][31][26]=	     0;m_PhiROImin[0][31][26]=	      0;m_PhiROImax[0][31][26]=	       0;
     m_EtaROImin[0][31][27]=   -1.06547;m_EtaROImax[0][31][27]=   -1.03003;m_PhiROImin[0][31][27]=  -0.197143;m_PhiROImax[0][31][27]=  -0.116958;
     m_EtaROImin[0][31][28]=	    0;m_EtaROImax[0][31][28]=	     0;m_PhiROImin[0][31][28]=	      0;m_PhiROImax[0][31][28]=	       0;
     m_EtaROImin[0][31][29]=	    0;m_EtaROImax[0][31][29]=	     0;m_PhiROImin[0][31][29]=	      0;m_PhiROImax[0][31][29]=	       0;
     m_EtaROImin[0][31][30]=	    0;m_EtaROImax[0][31][30]=	     0;m_PhiROImin[0][31][30]=	      0;m_PhiROImax[0][31][30]=	       0;
     m_EtaROImin[0][31][31]=	    0;m_EtaROImax[0][31][31]=	     0;m_PhiROImin[0][31][31]=	      0;m_PhiROImax[0][31][31]=	       0;
     m_EtaROImin[1][ 0][ 0]=   0.130922;m_EtaROImax[1][ 0][ 0]=   0.238612;m_PhiROImin[1][ 0][ 0]= 0.00295181;m_PhiROImax[1][ 0][ 0]=	0.112612;
     m_EtaROImin[1][ 0][ 1]=  0.0231199;m_EtaROImax[1][ 0][ 1]=   0.131749;m_PhiROImin[1][ 0][ 1]= 0.00295181;m_PhiROImax[1][ 0][ 1]=	0.112612;
     m_EtaROImin[1][ 0][ 2]=   0.130922;m_EtaROImax[1][ 0][ 2]=   0.238612;m_PhiROImin[1][ 0][ 2]=   0.116119;m_PhiROImax[1][ 0][ 2]=	0.223011;
     m_EtaROImin[1][ 0][ 3]=  0.0231199;m_EtaROImax[1][ 0][ 3]=   0.131749;m_PhiROImin[1][ 0][ 3]=   0.116119;m_PhiROImax[1][ 0][ 3]=	0.223011;
     m_EtaROImin[1][ 0][ 4]=   0.350831;m_EtaROImax[1][ 0][ 4]=   0.452099;m_PhiROImin[1][ 0][ 4]= 0.00295181;m_PhiROImax[1][ 0][ 4]=	0.112612;
     m_EtaROImin[1][ 0][ 5]=   0.246207;m_EtaROImax[1][ 0][ 5]=    0.35049;m_PhiROImin[1][ 0][ 5]= 0.00295181;m_PhiROImax[1][ 0][ 5]=	0.112612;
     m_EtaROImin[1][ 0][ 6]=   0.350831;m_EtaROImax[1][ 0][ 6]=   0.452099;m_PhiROImin[1][ 0][ 6]=   0.116119;m_PhiROImax[1][ 0][ 6]=	0.223011;
     m_EtaROImin[1][ 0][ 7]=   0.246207;m_EtaROImax[1][ 0][ 7]=    0.35049;m_PhiROImin[1][ 0][ 7]=   0.116119;m_PhiROImax[1][ 0][ 7]=	0.223011;
     m_EtaROImin[1][ 0][ 8]=   0.555789;m_EtaROImax[1][ 0][ 8]=   0.648101;m_PhiROImin[1][ 0][ 8]= 0.00295181;m_PhiROImax[1][ 0][ 8]=	0.112612;
     m_EtaROImin[1][ 0][ 9]=   0.458091;m_EtaROImax[1][ 0][ 9]=   0.554444;m_PhiROImin[1][ 0][ 9]= 0.00295181;m_PhiROImax[1][ 0][ 9]=	0.112612;
     m_EtaROImin[1][ 0][10]=   0.555789;m_EtaROImax[1][ 0][10]=   0.648101;m_PhiROImin[1][ 0][10]=   0.116119;m_PhiROImax[1][ 0][10]=	0.223011;
     m_EtaROImin[1][ 0][11]=   0.458091;m_EtaROImax[1][ 0][11]=   0.554444;m_PhiROImin[1][ 0][11]=   0.116119;m_PhiROImax[1][ 0][11]=	0.223011;
     m_EtaROImin[1][ 0][12]=   0.731124;m_EtaROImax[1][ 0][12]=   0.776449;m_PhiROImin[1][ 0][12]= 0.00296767;m_PhiROImax[1][ 0][12]=	0.113212;
     m_EtaROImin[1][ 0][13]=   0.655857;m_EtaROImax[1][ 0][13]=   0.728056;m_PhiROImin[1][ 0][13]= 0.00296767;m_PhiROImax[1][ 0][13]=	0.113212;
     m_EtaROImin[1][ 0][14]=   0.731124;m_EtaROImax[1][ 0][14]=   0.776449;m_PhiROImin[1][ 0][14]=   0.116737;m_PhiROImax[1][ 0][14]=	0.224169;
     m_EtaROImin[1][ 0][15]=   0.655857;m_EtaROImax[1][ 0][15]=   0.728056;m_PhiROImin[1][ 0][15]=   0.116737;m_PhiROImax[1][ 0][15]=	0.224169;
     m_EtaROImin[1][ 0][16]=   0.853359;m_EtaROImax[1][ 0][16]=   0.895163;m_PhiROImin[1][ 0][16]= 0.00296767;m_PhiROImax[1][ 0][16]=	0.113212;
     m_EtaROImin[1][ 0][17]=   0.783867;m_EtaROImax[1][ 0][17]=   0.850528;m_PhiROImin[1][ 0][17]= 0.00296767;m_PhiROImax[1][ 0][17]=	0.113212;
     m_EtaROImin[1][ 0][18]=   0.853359;m_EtaROImax[1][ 0][18]=   0.895163;m_PhiROImin[1][ 0][18]=   0.116737;m_PhiROImax[1][ 0][18]=	0.224169;
     m_EtaROImin[1][ 0][19]=   0.783867;m_EtaROImax[1][ 0][19]=   0.850528;m_PhiROImin[1][ 0][19]=   0.116737;m_PhiROImax[1][ 0][19]=	0.224169;
     m_EtaROImin[1][ 0][20]=   0.964674;m_EtaROImax[1][ 0][20]=    1.02514;m_PhiROImin[1][ 0][20]= 0.00295181;m_PhiROImax[1][ 0][20]=	0.112612;
     m_EtaROImin[1][ 0][21]=   0.898201;m_EtaROImax[1][ 0][21]=   0.961344;m_PhiROImin[1][ 0][21]= 0.00295181;m_PhiROImax[1][ 0][21]=	0.112612;
     m_EtaROImin[1][ 0][22]=   0.964674;m_EtaROImax[1][ 0][22]=    1.02514;m_PhiROImin[1][ 0][22]=   0.116119;m_PhiROImax[1][ 0][22]=	0.223011;
     m_EtaROImin[1][ 0][23]=   0.898201;m_EtaROImax[1][ 0][23]=   0.961344;m_PhiROImin[1][ 0][23]=   0.116119;m_PhiROImax[1][ 0][23]=	0.223011;
     m_EtaROImin[1][ 0][24]=	    0;m_EtaROImax[1][ 0][24]=	     0;m_PhiROImin[1][ 0][24]=	      0;m_PhiROImax[1][ 0][24]=	       0;
     m_EtaROImin[1][ 0][25]=    1.03003;m_EtaROImax[1][ 0][25]=    1.06547;m_PhiROImin[1][ 0][25]= 0.00306876;m_PhiROImax[1][ 0][25]=	0.113429;
     m_EtaROImin[1][ 0][26]=	    0;m_EtaROImax[1][ 0][26]=	     0;m_PhiROImin[1][ 0][26]=	      0;m_PhiROImax[1][ 0][26]=	       0;
     m_EtaROImin[1][ 0][27]=    1.03003;m_EtaROImax[1][ 0][27]=    1.06547;m_PhiROImin[1][ 0][27]=   0.116958;m_PhiROImax[1][ 0][27]=	0.197143;
     m_EtaROImin[1][ 0][28]=	    0;m_EtaROImax[1][ 0][28]=	     0;m_PhiROImin[1][ 0][28]=	      0;m_PhiROImax[1][ 0][28]=	       0;
     m_EtaROImin[1][ 0][29]=	    0;m_EtaROImax[1][ 0][29]=	     0;m_PhiROImin[1][ 0][29]=	      0;m_PhiROImax[1][ 0][29]=	       0;
     m_EtaROImin[1][ 0][30]=	    0;m_EtaROImax[1][ 0][30]=	     0;m_PhiROImin[1][ 0][30]=	      0;m_PhiROImax[1][ 0][30]=	       0;
     m_EtaROImin[1][ 0][31]=	    0;m_EtaROImax[1][ 0][31]=	     0;m_PhiROImin[1][ 0][31]=	      0;m_PhiROImax[1][ 0][31]=	       0;
     m_EtaROImin[1][ 1][ 0]=  0.0208251;m_EtaROImax[1][ 1][ 0]=   0.118734;m_PhiROImin[1][ 1][ 0]=   0.305953;m_PhiROImax[1][ 1][ 0]=	0.389909;
     m_EtaROImin[1][ 1][ 1]=   0.116816;m_EtaROImax[1][ 1][ 1]=   0.213185;m_PhiROImin[1][ 1][ 1]=   0.305953;m_PhiROImax[1][ 1][ 1]=	0.389909;
     m_EtaROImin[1][ 1][ 2]=  0.0208251;m_EtaROImax[1][ 1][ 2]=   0.118734;m_PhiROImin[1][ 1][ 2]=   0.219649;m_PhiROImax[1][ 1][ 2]=	0.302322;
     m_EtaROImin[1][ 1][ 3]=   0.116816;m_EtaROImax[1][ 1][ 3]=   0.213185;m_PhiROImin[1][ 1][ 3]=   0.219649;m_PhiROImax[1][ 1][ 3]=	0.302322;
     m_EtaROImin[1][ 1][ 4]=   0.222449;m_EtaROImax[1][ 1][ 4]=   0.302928;m_PhiROImin[1][ 1][ 4]=   0.305953;m_PhiROImax[1][ 1][ 4]=	0.389909;
     m_EtaROImin[1][ 1][ 5]=    0.30075;m_EtaROImax[1][ 1][ 5]=   0.416721;m_PhiROImin[1][ 1][ 5]=   0.305953;m_PhiROImax[1][ 1][ 5]=	0.389909;
     m_EtaROImin[1][ 1][ 6]=   0.222449;m_EtaROImax[1][ 1][ 6]=   0.302928;m_PhiROImin[1][ 1][ 6]=   0.219649;m_PhiROImax[1][ 1][ 6]=	0.302322;
     m_EtaROImin[1][ 1][ 7]=    0.30075;m_EtaROImax[1][ 1][ 7]=   0.416721;m_PhiROImin[1][ 1][ 7]=   0.219649;m_PhiROImax[1][ 1][ 7]=	0.302322;
     m_EtaROImin[1][ 1][ 8]=    0.42967;m_EtaROImax[1][ 1][ 8]=   0.504617;m_PhiROImin[1][ 1][ 8]=   0.305953;m_PhiROImax[1][ 1][ 8]=	0.389909;
     m_EtaROImin[1][ 1][ 9]=   0.501681;m_EtaROImax[1][ 1][ 9]=   0.573871;m_PhiROImin[1][ 1][ 9]=   0.305953;m_PhiROImax[1][ 1][ 9]=	0.389909;
     m_EtaROImin[1][ 1][10]=    0.42967;m_EtaROImax[1][ 1][10]=   0.504617;m_PhiROImin[1][ 1][10]=   0.219649;m_PhiROImax[1][ 1][10]=	0.302322;
     m_EtaROImin[1][ 1][11]=   0.501681;m_EtaROImax[1][ 1][11]=   0.573871;m_PhiROImin[1][ 1][11]=   0.219649;m_PhiROImax[1][ 1][11]=	0.302322;
     m_EtaROImin[1][ 1][12]=   0.583785;m_EtaROImax[1][ 1][12]=   0.653329;m_PhiROImin[1][ 1][12]=   0.305953;m_PhiROImax[1][ 1][12]=	0.389909;
     m_EtaROImin[1][ 1][13]=   0.649934;m_EtaROImax[1][ 1][13]=   0.741516;m_PhiROImin[1][ 1][13]=   0.305953;m_PhiROImax[1][ 1][13]=	0.389909;
     m_EtaROImin[1][ 1][14]=   0.583785;m_EtaROImax[1][ 1][14]=   0.653329;m_PhiROImin[1][ 1][14]=   0.219649;m_PhiROImax[1][ 1][14]=	0.302322;
     m_EtaROImin[1][ 1][15]=   0.649934;m_EtaROImax[1][ 1][15]=   0.741516;m_PhiROImin[1][ 1][15]=   0.219649;m_PhiROImax[1][ 1][15]=	0.302322;
     m_EtaROImin[1][ 1][16]=   0.756521;m_EtaROImax[1][ 1][16]=   0.837822;m_PhiROImin[1][ 1][16]=   0.305953;m_PhiROImax[1][ 1][16]=	0.389909;
     m_EtaROImin[1][ 1][17]=	    0;m_EtaROImax[1][ 1][17]=	     0;m_PhiROImin[1][ 1][17]=	      0;m_PhiROImax[1][ 1][17]=	       0;
     m_EtaROImin[1][ 1][18]=   0.756521;m_EtaROImax[1][ 1][18]=   0.837822;m_PhiROImin[1][ 1][18]=   0.219649;m_PhiROImax[1][ 1][18]=	0.302322;
     m_EtaROImin[1][ 1][19]=	    0;m_EtaROImax[1][ 1][19]=	     0;m_PhiROImin[1][ 1][19]=	      0;m_PhiROImax[1][ 1][19]=	       0;
     m_EtaROImin[1][ 1][20]=   0.844116;m_EtaROImax[1][ 1][20]=   0.903324;m_PhiROImin[1][ 1][20]=   0.305953;m_PhiROImax[1][ 1][20]=	0.389909;
     m_EtaROImin[1][ 1][21]=   0.899344;m_EtaROImax[1][ 1][21]=   0.956037;m_PhiROImin[1][ 1][21]=   0.305953;m_PhiROImax[1][ 1][21]=	0.389909;
     m_EtaROImin[1][ 1][22]=   0.844116;m_EtaROImax[1][ 1][22]=   0.903324;m_PhiROImin[1][ 1][22]=   0.219649;m_PhiROImax[1][ 1][22]=	0.302322;
     m_EtaROImin[1][ 1][23]=   0.899344;m_EtaROImax[1][ 1][23]=   0.956037;m_PhiROImin[1][ 1][23]=   0.219649;m_PhiROImax[1][ 1][23]=	0.302322;
     m_EtaROImin[1][ 1][24]=	    0;m_EtaROImax[1][ 1][24]=	     0;m_PhiROImin[1][ 1][24]=	      0;m_PhiROImax[1][ 1][24]=	       0;
     m_EtaROImin[1][ 1][25]=	    0;m_EtaROImax[1][ 1][25]=	     0;m_PhiROImin[1][ 1][25]=	      0;m_PhiROImax[1][ 1][25]=	       0;
     m_EtaROImin[1][ 1][26]=	    0;m_EtaROImax[1][ 1][26]=	     0;m_PhiROImin[1][ 1][26]=	      0;m_PhiROImax[1][ 1][26]=	       0;
     m_EtaROImin[1][ 1][27]=	    0;m_EtaROImax[1][ 1][27]=	     0;m_PhiROImin[1][ 1][27]=	      0;m_PhiROImax[1][ 1][27]=	       0;
     m_EtaROImin[1][ 1][28]=	    0;m_EtaROImax[1][ 1][28]=	     0;m_PhiROImin[1][ 1][28]=	      0;m_PhiROImax[1][ 1][28]=	       0;
     m_EtaROImin[1][ 1][29]=	    0;m_EtaROImax[1][ 1][29]=	     0;m_PhiROImin[1][ 1][29]=	      0;m_PhiROImax[1][ 1][29]=	       0;
     m_EtaROImin[1][ 1][30]=	    0;m_EtaROImax[1][ 1][30]=	     0;m_PhiROImin[1][ 1][30]=	      0;m_PhiROImax[1][ 1][30]=	       0;
     m_EtaROImin[1][ 1][31]=	    0;m_EtaROImax[1][ 1][31]=	     0;m_PhiROImin[1][ 1][31]=	      0;m_PhiROImax[1][ 1][31]=	       0;
     m_EtaROImin[1][ 2][ 0]=   0.116816;m_EtaROImax[1][ 2][ 0]=   0.213185;m_PhiROImin[1][ 2][ 0]=   0.395489;m_PhiROImax[1][ 2][ 0]=	0.479445;
     m_EtaROImin[1][ 2][ 1]=  0.0208251;m_EtaROImax[1][ 2][ 1]=   0.118734;m_PhiROImin[1][ 2][ 1]=   0.395489;m_PhiROImax[1][ 2][ 1]=	0.479445;
     m_EtaROImin[1][ 2][ 2]=   0.116816;m_EtaROImax[1][ 2][ 2]=   0.213185;m_PhiROImin[1][ 2][ 2]=   0.483076;m_PhiROImax[1][ 2][ 2]=	0.565749;
     m_EtaROImin[1][ 2][ 3]=  0.0208251;m_EtaROImax[1][ 2][ 3]=   0.118734;m_PhiROImin[1][ 2][ 3]=   0.483076;m_PhiROImax[1][ 2][ 3]=	0.565749;
     m_EtaROImin[1][ 2][ 4]=    0.30075;m_EtaROImax[1][ 2][ 4]=   0.416721;m_PhiROImin[1][ 2][ 4]=   0.395489;m_PhiROImax[1][ 2][ 4]=	0.479445;
     m_EtaROImin[1][ 2][ 5]=   0.222449;m_EtaROImax[1][ 2][ 5]=   0.302928;m_PhiROImin[1][ 2][ 5]=   0.395489;m_PhiROImax[1][ 2][ 5]=	0.479445;
     m_EtaROImin[1][ 2][ 6]=    0.30075;m_EtaROImax[1][ 2][ 6]=   0.416721;m_PhiROImin[1][ 2][ 6]=   0.483076;m_PhiROImax[1][ 2][ 6]=	0.565749;
     m_EtaROImin[1][ 2][ 7]=   0.222449;m_EtaROImax[1][ 2][ 7]=   0.302928;m_PhiROImin[1][ 2][ 7]=   0.483076;m_PhiROImax[1][ 2][ 7]=	0.565749;
     m_EtaROImin[1][ 2][ 8]=   0.501681;m_EtaROImax[1][ 2][ 8]=   0.573871;m_PhiROImin[1][ 2][ 8]=   0.395489;m_PhiROImax[1][ 2][ 8]=	0.479445;
     m_EtaROImin[1][ 2][ 9]=    0.42967;m_EtaROImax[1][ 2][ 9]=   0.504617;m_PhiROImin[1][ 2][ 9]=   0.395489;m_PhiROImax[1][ 2][ 9]=	0.479445;
     m_EtaROImin[1][ 2][10]=   0.501681;m_EtaROImax[1][ 2][10]=   0.573871;m_PhiROImin[1][ 2][10]=   0.483076;m_PhiROImax[1][ 2][10]=	0.565749;
     m_EtaROImin[1][ 2][11]=    0.42967;m_EtaROImax[1][ 2][11]=   0.504617;m_PhiROImin[1][ 2][11]=   0.483076;m_PhiROImax[1][ 2][11]=	0.565749;
     m_EtaROImin[1][ 2][12]=   0.649934;m_EtaROImax[1][ 2][12]=   0.741516;m_PhiROImin[1][ 2][12]=   0.395489;m_PhiROImax[1][ 2][12]=	0.479445;
     m_EtaROImin[1][ 2][13]=   0.583785;m_EtaROImax[1][ 2][13]=   0.653329;m_PhiROImin[1][ 2][13]=   0.395489;m_PhiROImax[1][ 2][13]=	0.479445;
     m_EtaROImin[1][ 2][14]=   0.649934;m_EtaROImax[1][ 2][14]=   0.741516;m_PhiROImin[1][ 2][14]=   0.483076;m_PhiROImax[1][ 2][14]=	0.565749;
     m_EtaROImin[1][ 2][15]=   0.583785;m_EtaROImax[1][ 2][15]=   0.653329;m_PhiROImin[1][ 2][15]=   0.483076;m_PhiROImax[1][ 2][15]=	0.565749;
     m_EtaROImin[1][ 2][16]=	    0;m_EtaROImax[1][ 2][16]=	     0;m_PhiROImin[1][ 2][16]=	      0;m_PhiROImax[1][ 2][16]=	       0;
     m_EtaROImin[1][ 2][17]=   0.756521;m_EtaROImax[1][ 2][17]=   0.837822;m_PhiROImin[1][ 2][17]=   0.395489;m_PhiROImax[1][ 2][17]=	0.479445;
     m_EtaROImin[1][ 2][18]=	    0;m_EtaROImax[1][ 2][18]=	     0;m_PhiROImin[1][ 2][18]=	      0;m_PhiROImax[1][ 2][18]=	       0;
     m_EtaROImin[1][ 2][19]=   0.756521;m_EtaROImax[1][ 2][19]=   0.837822;m_PhiROImin[1][ 2][19]=   0.483076;m_PhiROImax[1][ 2][19]=	0.565749;
     m_EtaROImin[1][ 2][20]=   0.899344;m_EtaROImax[1][ 2][20]=   0.956037;m_PhiROImin[1][ 2][20]=   0.395489;m_PhiROImax[1][ 2][20]=	0.479445;
     m_EtaROImin[1][ 2][21]=   0.844116;m_EtaROImax[1][ 2][21]=   0.903324;m_PhiROImin[1][ 2][21]=   0.395489;m_PhiROImax[1][ 2][21]=	0.479445;
     m_EtaROImin[1][ 2][22]=   0.899344;m_EtaROImax[1][ 2][22]=   0.956037;m_PhiROImin[1][ 2][22]=   0.483076;m_PhiROImax[1][ 2][22]=	0.565749;
     m_EtaROImin[1][ 2][23]=   0.844116;m_EtaROImax[1][ 2][23]=   0.903324;m_PhiROImin[1][ 2][23]=   0.483076;m_PhiROImax[1][ 2][23]=	0.565749;
     m_EtaROImin[1][ 2][24]=	    0;m_EtaROImax[1][ 2][24]=	     0;m_PhiROImin[1][ 2][24]=	      0;m_PhiROImax[1][ 2][24]=	       0;
     m_EtaROImin[1][ 2][25]=	    0;m_EtaROImax[1][ 2][25]=	     0;m_PhiROImin[1][ 2][25]=	      0;m_PhiROImax[1][ 2][25]=	       0;
     m_EtaROImin[1][ 2][26]=	    0;m_EtaROImax[1][ 2][26]=	     0;m_PhiROImin[1][ 2][26]=	      0;m_PhiROImax[1][ 2][26]=	       0;
     m_EtaROImin[1][ 2][27]=	    0;m_EtaROImax[1][ 2][27]=	     0;m_PhiROImin[1][ 2][27]=	      0;m_PhiROImax[1][ 2][27]=	       0;
     m_EtaROImin[1][ 2][28]=	    0;m_EtaROImax[1][ 2][28]=	     0;m_PhiROImin[1][ 2][28]=	      0;m_PhiROImax[1][ 2][28]=	       0;
     m_EtaROImin[1][ 2][29]=	    0;m_EtaROImax[1][ 2][29]=	     0;m_PhiROImin[1][ 2][29]=	      0;m_PhiROImax[1][ 2][29]=	       0;
     m_EtaROImin[1][ 2][30]=	    0;m_EtaROImax[1][ 2][30]=	     0;m_PhiROImin[1][ 2][30]=	      0;m_PhiROImax[1][ 2][30]=	       0;
     m_EtaROImin[1][ 2][31]=	    0;m_EtaROImax[1][ 2][31]=	     0;m_PhiROImin[1][ 2][31]=	      0;m_PhiROImax[1][ 2][31]=	       0;
     m_EtaROImin[1][ 3][ 0]=  0.0231199;m_EtaROImax[1][ 3][ 0]=   0.131749;m_PhiROImin[1][ 3][ 0]=   0.672786;m_PhiROImax[1][ 3][ 0]=	0.782446;
     m_EtaROImin[1][ 3][ 1]=   0.130922;m_EtaROImax[1][ 3][ 1]=   0.238612;m_PhiROImin[1][ 3][ 1]=   0.672786;m_PhiROImax[1][ 3][ 1]=	0.782446;
     m_EtaROImin[1][ 3][ 2]=  0.0231199;m_EtaROImax[1][ 3][ 2]=   0.131749;m_PhiROImin[1][ 3][ 2]=   0.562387;m_PhiROImax[1][ 3][ 2]=	0.669279;
     m_EtaROImin[1][ 3][ 3]=   0.130922;m_EtaROImax[1][ 3][ 3]=   0.238612;m_PhiROImin[1][ 3][ 3]=   0.562387;m_PhiROImax[1][ 3][ 3]=	0.669279;
     m_EtaROImin[1][ 3][ 4]=   0.246207;m_EtaROImax[1][ 3][ 4]=    0.35049;m_PhiROImin[1][ 3][ 4]=   0.672786;m_PhiROImax[1][ 3][ 4]=	0.782446;
     m_EtaROImin[1][ 3][ 5]=   0.350831;m_EtaROImax[1][ 3][ 5]=   0.452099;m_PhiROImin[1][ 3][ 5]=   0.672786;m_PhiROImax[1][ 3][ 5]=	0.782446;
     m_EtaROImin[1][ 3][ 6]=   0.246207;m_EtaROImax[1][ 3][ 6]=    0.35049;m_PhiROImin[1][ 3][ 6]=   0.562387;m_PhiROImax[1][ 3][ 6]=	0.669279;
     m_EtaROImin[1][ 3][ 7]=   0.350831;m_EtaROImax[1][ 3][ 7]=   0.452099;m_PhiROImin[1][ 3][ 7]=   0.562387;m_PhiROImax[1][ 3][ 7]=	0.669279;
     m_EtaROImin[1][ 3][ 8]=   0.458091;m_EtaROImax[1][ 3][ 8]=   0.554444;m_PhiROImin[1][ 3][ 8]=   0.672786;m_PhiROImax[1][ 3][ 8]=	0.782446;
     m_EtaROImin[1][ 3][ 9]=   0.555789;m_EtaROImax[1][ 3][ 9]=   0.648101;m_PhiROImin[1][ 3][ 9]=   0.672786;m_PhiROImax[1][ 3][ 9]=	0.782446;
     m_EtaROImin[1][ 3][10]=   0.458091;m_EtaROImax[1][ 3][10]=   0.554444;m_PhiROImin[1][ 3][10]=   0.562387;m_PhiROImax[1][ 3][10]=	0.669279;
     m_EtaROImin[1][ 3][11]=   0.555789;m_EtaROImax[1][ 3][11]=   0.648101;m_PhiROImin[1][ 3][11]=   0.562387;m_PhiROImax[1][ 3][11]=	0.669279;
     m_EtaROImin[1][ 3][12]=   0.655857;m_EtaROImax[1][ 3][12]=   0.703298;m_PhiROImin[1][ 3][12]=   0.672186;m_PhiROImax[1][ 3][12]=	 0.78243;
     m_EtaROImin[1][ 3][13]=   0.706413;m_EtaROImax[1][ 3][13]=   0.776449;m_PhiROImin[1][ 3][13]=   0.672186;m_PhiROImax[1][ 3][13]=	 0.78243;
     m_EtaROImin[1][ 3][14]=   0.655857;m_EtaROImax[1][ 3][14]=   0.703298;m_PhiROImin[1][ 3][14]=   0.561229;m_PhiROImax[1][ 3][14]=	0.668661;
     m_EtaROImin[1][ 3][15]=   0.706413;m_EtaROImax[1][ 3][15]=   0.776449;m_PhiROImin[1][ 3][15]=   0.561229;m_PhiROImax[1][ 3][15]=	0.668661;
     m_EtaROImin[1][ 3][16]=   0.783867;m_EtaROImax[1][ 3][16]=    0.82768;m_PhiROImin[1][ 3][16]=   0.672186;m_PhiROImax[1][ 3][16]=	 0.78243;
     m_EtaROImin[1][ 3][17]=   0.830556;m_EtaROImax[1][ 3][17]=   0.895163;m_PhiROImin[1][ 3][17]=   0.672186;m_PhiROImax[1][ 3][17]=	 0.78243;
     m_EtaROImin[1][ 3][18]=   0.783867;m_EtaROImax[1][ 3][18]=    0.82768;m_PhiROImin[1][ 3][18]=   0.561229;m_PhiROImax[1][ 3][18]=	0.668661;
     m_EtaROImin[1][ 3][19]=   0.830556;m_EtaROImax[1][ 3][19]=   0.895163;m_PhiROImin[1][ 3][19]=   0.561229;m_PhiROImax[1][ 3][19]=	0.668661;
     m_EtaROImin[1][ 3][20]=   0.898201;m_EtaROImax[1][ 3][20]=   0.961344;m_PhiROImin[1][ 3][20]=   0.672786;m_PhiROImax[1][ 3][20]=	0.782446;
     m_EtaROImin[1][ 3][21]=   0.964674;m_EtaROImax[1][ 3][21]=    1.02514;m_PhiROImin[1][ 3][21]=   0.672786;m_PhiROImax[1][ 3][21]=	0.782446;
     m_EtaROImin[1][ 3][22]=   0.898201;m_EtaROImax[1][ 3][22]=   0.961344;m_PhiROImin[1][ 3][22]=   0.562387;m_PhiROImax[1][ 3][22]=	0.669279;
     m_EtaROImin[1][ 3][23]=   0.964674;m_EtaROImax[1][ 3][23]=    1.02514;m_PhiROImin[1][ 3][23]=   0.562387;m_PhiROImax[1][ 3][23]=	0.669279;
     m_EtaROImin[1][ 3][24]=    1.03003;m_EtaROImax[1][ 3][24]=    1.06547;m_PhiROImin[1][ 3][24]=   0.671969;m_PhiROImax[1][ 3][24]=	0.782329;
     m_EtaROImin[1][ 3][25]=	    0;m_EtaROImax[1][ 3][25]=	     0;m_PhiROImin[1][ 3][25]=	      0;m_PhiROImax[1][ 3][25]=	       0;
     m_EtaROImin[1][ 3][26]=    1.03003;m_EtaROImax[1][ 3][26]=    1.06547;m_PhiROImin[1][ 3][26]=   0.588255;m_PhiROImax[1][ 3][26]=	 0.66844;
     m_EtaROImin[1][ 3][27]=	    0;m_EtaROImax[1][ 3][27]=	     0;m_PhiROImin[1][ 3][27]=	      0;m_PhiROImax[1][ 3][27]=	       0;
     m_EtaROImin[1][ 3][28]=	    0;m_EtaROImax[1][ 3][28]=	     0;m_PhiROImin[1][ 3][28]=	      0;m_PhiROImax[1][ 3][28]=	       0;
     m_EtaROImin[1][ 3][29]=	    0;m_EtaROImax[1][ 3][29]=	     0;m_PhiROImin[1][ 3][29]=	      0;m_PhiROImax[1][ 3][29]=	       0;
     m_EtaROImin[1][ 3][30]=	    0;m_EtaROImax[1][ 3][30]=	     0;m_PhiROImin[1][ 3][30]=	      0;m_PhiROImax[1][ 3][30]=	       0;
     m_EtaROImin[1][ 3][31]=	    0;m_EtaROImax[1][ 3][31]=	     0;m_PhiROImin[1][ 3][31]=	      0;m_PhiROImax[1][ 3][31]=	       0;
     m_EtaROImin[1][ 4][ 0]=   0.130922;m_EtaROImax[1][ 4][ 0]=   0.238612;m_PhiROImin[1][ 4][ 0]=	0.78835;m_PhiROImax[1][ 4][ 0]=	0.898011;
     m_EtaROImin[1][ 4][ 1]=  0.0231199;m_EtaROImax[1][ 4][ 1]=   0.131749;m_PhiROImin[1][ 4][ 1]=	0.78835;m_PhiROImax[1][ 4][ 1]=	0.898011;
     m_EtaROImin[1][ 4][ 2]=   0.130922;m_EtaROImax[1][ 4][ 2]=   0.238612;m_PhiROImin[1][ 4][ 2]=   0.901517;m_PhiROImax[1][ 4][ 2]=	 1.00841;
     m_EtaROImin[1][ 4][ 3]=  0.0231199;m_EtaROImax[1][ 4][ 3]=   0.131749;m_PhiROImin[1][ 4][ 3]=   0.901517;m_PhiROImax[1][ 4][ 3]=	 1.00841;
     m_EtaROImin[1][ 4][ 4]=   0.350831;m_EtaROImax[1][ 4][ 4]=   0.452099;m_PhiROImin[1][ 4][ 4]=	0.78835;m_PhiROImax[1][ 4][ 4]=	0.898011;
     m_EtaROImin[1][ 4][ 5]=   0.246207;m_EtaROImax[1][ 4][ 5]=    0.35049;m_PhiROImin[1][ 4][ 5]=	0.78835;m_PhiROImax[1][ 4][ 5]=	0.898011;
     m_EtaROImin[1][ 4][ 6]=   0.350831;m_EtaROImax[1][ 4][ 6]=   0.452099;m_PhiROImin[1][ 4][ 6]=   0.901517;m_PhiROImax[1][ 4][ 6]=	 1.00841;
     m_EtaROImin[1][ 4][ 7]=   0.246207;m_EtaROImax[1][ 4][ 7]=    0.35049;m_PhiROImin[1][ 4][ 7]=   0.901517;m_PhiROImax[1][ 4][ 7]=	 1.00841;
     m_EtaROImin[1][ 4][ 8]=   0.555789;m_EtaROImax[1][ 4][ 8]=   0.648101;m_PhiROImin[1][ 4][ 8]=	0.78835;m_PhiROImax[1][ 4][ 8]=	0.898011;
     m_EtaROImin[1][ 4][ 9]=   0.458091;m_EtaROImax[1][ 4][ 9]=   0.554444;m_PhiROImin[1][ 4][ 9]=	0.78835;m_PhiROImax[1][ 4][ 9]=	0.898011;
     m_EtaROImin[1][ 4][10]=   0.555789;m_EtaROImax[1][ 4][10]=   0.648101;m_PhiROImin[1][ 4][10]=   0.901517;m_PhiROImax[1][ 4][10]=	 1.00841;
     m_EtaROImin[1][ 4][11]=   0.458091;m_EtaROImax[1][ 4][11]=   0.554444;m_PhiROImin[1][ 4][11]=   0.901517;m_PhiROImax[1][ 4][11]=	 1.00841;
     m_EtaROImin[1][ 4][12]=   0.731124;m_EtaROImax[1][ 4][12]=   0.776449;m_PhiROImin[1][ 4][12]=   0.788366;m_PhiROImax[1][ 4][12]=	 0.89861;
     m_EtaROImin[1][ 4][13]=   0.655857;m_EtaROImax[1][ 4][13]=   0.728056;m_PhiROImin[1][ 4][13]=   0.788366;m_PhiROImax[1][ 4][13]=	 0.89861;
     m_EtaROImin[1][ 4][14]=   0.731124;m_EtaROImax[1][ 4][14]=   0.776449;m_PhiROImin[1][ 4][14]=   0.902135;m_PhiROImax[1][ 4][14]=	 1.00957;
     m_EtaROImin[1][ 4][15]=   0.655857;m_EtaROImax[1][ 4][15]=   0.728056;m_PhiROImin[1][ 4][15]=   0.902135;m_PhiROImax[1][ 4][15]=	 1.00957;
     m_EtaROImin[1][ 4][16]=   0.853359;m_EtaROImax[1][ 4][16]=   0.895163;m_PhiROImin[1][ 4][16]=   0.788366;m_PhiROImax[1][ 4][16]=	 0.89861;
     m_EtaROImin[1][ 4][17]=   0.783867;m_EtaROImax[1][ 4][17]=   0.850528;m_PhiROImin[1][ 4][17]=   0.788366;m_PhiROImax[1][ 4][17]=	 0.89861;
     m_EtaROImin[1][ 4][18]=   0.853359;m_EtaROImax[1][ 4][18]=   0.895163;m_PhiROImin[1][ 4][18]=   0.902135;m_PhiROImax[1][ 4][18]=	 1.00957;
     m_EtaROImin[1][ 4][19]=   0.783867;m_EtaROImax[1][ 4][19]=   0.850528;m_PhiROImin[1][ 4][19]=   0.902135;m_PhiROImax[1][ 4][19]=	 1.00957;
     m_EtaROImin[1][ 4][20]=   0.964674;m_EtaROImax[1][ 4][20]=    1.02514;m_PhiROImin[1][ 4][20]=	0.78835;m_PhiROImax[1][ 4][20]=	0.898011;
     m_EtaROImin[1][ 4][21]=   0.898201;m_EtaROImax[1][ 4][21]=   0.961344;m_PhiROImin[1][ 4][21]=	0.78835;m_PhiROImax[1][ 4][21]=	0.898011;
     m_EtaROImin[1][ 4][22]=   0.964674;m_EtaROImax[1][ 4][22]=    1.02514;m_PhiROImin[1][ 4][22]=   0.901517;m_PhiROImax[1][ 4][22]=	 1.00841;
     m_EtaROImin[1][ 4][23]=   0.898201;m_EtaROImax[1][ 4][23]=   0.961344;m_PhiROImin[1][ 4][23]=   0.901517;m_PhiROImax[1][ 4][23]=	 1.00841;
     m_EtaROImin[1][ 4][24]=	    0;m_EtaROImax[1][ 4][24]=	     0;m_PhiROImin[1][ 4][24]=	      0;m_PhiROImax[1][ 4][24]=	       0;
     m_EtaROImin[1][ 4][25]=    1.03003;m_EtaROImax[1][ 4][25]=    1.06547;m_PhiROImin[1][ 4][25]=   0.788467;m_PhiROImax[1][ 4][25]=	0.898827;
     m_EtaROImin[1][ 4][26]=	    0;m_EtaROImax[1][ 4][26]=	     0;m_PhiROImin[1][ 4][26]=	      0;m_PhiROImax[1][ 4][26]=	       0;
     m_EtaROImin[1][ 4][27]=    1.03003;m_EtaROImax[1][ 4][27]=    1.06547;m_PhiROImin[1][ 4][27]=   0.902356;m_PhiROImax[1][ 4][27]=	0.982541;
     m_EtaROImin[1][ 4][28]=	    0;m_EtaROImax[1][ 4][28]=	     0;m_PhiROImin[1][ 4][28]=	      0;m_PhiROImax[1][ 4][28]=	       0;
     m_EtaROImin[1][ 4][29]=	    0;m_EtaROImax[1][ 4][29]=	     0;m_PhiROImin[1][ 4][29]=	      0;m_PhiROImax[1][ 4][29]=	       0;
     m_EtaROImin[1][ 4][30]=	    0;m_EtaROImax[1][ 4][30]=	     0;m_PhiROImin[1][ 4][30]=	      0;m_PhiROImax[1][ 4][30]=	       0;
     m_EtaROImin[1][ 4][31]=	    0;m_EtaROImax[1][ 4][31]=	     0;m_PhiROImin[1][ 4][31]=	      0;m_PhiROImax[1][ 4][31]=	       0;
     m_EtaROImin[1][ 5][ 0]=  0.0208251;m_EtaROImax[1][ 5][ 0]=   0.118734;m_PhiROImin[1][ 5][ 0]=	1.09135;m_PhiROImax[1][ 5][ 0]=	 1.17531;
     m_EtaROImin[1][ 5][ 1]=   0.116816;m_EtaROImax[1][ 5][ 1]=   0.213185;m_PhiROImin[1][ 5][ 1]=	1.09135;m_PhiROImax[1][ 5][ 1]=	 1.17531;
     m_EtaROImin[1][ 5][ 2]=  0.0208251;m_EtaROImax[1][ 5][ 2]=   0.118734;m_PhiROImin[1][ 5][ 2]=	1.00505;m_PhiROImax[1][ 5][ 2]=	 1.08772;
     m_EtaROImin[1][ 5][ 3]=   0.116816;m_EtaROImax[1][ 5][ 3]=   0.213185;m_PhiROImin[1][ 5][ 3]=	1.00505;m_PhiROImax[1][ 5][ 3]=	 1.08772;
     m_EtaROImin[1][ 5][ 4]=   0.222449;m_EtaROImax[1][ 5][ 4]=   0.302928;m_PhiROImin[1][ 5][ 4]=	1.09135;m_PhiROImax[1][ 5][ 4]=	 1.17531;
     m_EtaROImin[1][ 5][ 5]=    0.30075;m_EtaROImax[1][ 5][ 5]=   0.416721;m_PhiROImin[1][ 5][ 5]=	1.09135;m_PhiROImax[1][ 5][ 5]=	 1.17531;
     m_EtaROImin[1][ 5][ 6]=   0.222449;m_EtaROImax[1][ 5][ 6]=   0.302928;m_PhiROImin[1][ 5][ 6]=	1.00505;m_PhiROImax[1][ 5][ 6]=	 1.08772;
     m_EtaROImin[1][ 5][ 7]=    0.30075;m_EtaROImax[1][ 5][ 7]=   0.416721;m_PhiROImin[1][ 5][ 7]=	1.00505;m_PhiROImax[1][ 5][ 7]=	 1.08772;
     m_EtaROImin[1][ 5][ 8]=    0.42967;m_EtaROImax[1][ 5][ 8]=   0.504617;m_PhiROImin[1][ 5][ 8]=	1.09135;m_PhiROImax[1][ 5][ 8]=	 1.17531;
     m_EtaROImin[1][ 5][ 9]=   0.501681;m_EtaROImax[1][ 5][ 9]=   0.573871;m_PhiROImin[1][ 5][ 9]=	1.09135;m_PhiROImax[1][ 5][ 9]=	 1.17531;
     m_EtaROImin[1][ 5][10]=    0.42967;m_EtaROImax[1][ 5][10]=   0.504617;m_PhiROImin[1][ 5][10]=	1.00505;m_PhiROImax[1][ 5][10]=	 1.08772;
     m_EtaROImin[1][ 5][11]=   0.501681;m_EtaROImax[1][ 5][11]=   0.573871;m_PhiROImin[1][ 5][11]=	1.00505;m_PhiROImax[1][ 5][11]=	 1.08772;
     m_EtaROImin[1][ 5][12]=   0.583785;m_EtaROImax[1][ 5][12]=   0.653329;m_PhiROImin[1][ 5][12]=	1.09135;m_PhiROImax[1][ 5][12]=	 1.17531;
     m_EtaROImin[1][ 5][13]=   0.649934;m_EtaROImax[1][ 5][13]=   0.741516;m_PhiROImin[1][ 5][13]=	1.09135;m_PhiROImax[1][ 5][13]=	 1.17531;
     m_EtaROImin[1][ 5][14]=   0.583785;m_EtaROImax[1][ 5][14]=   0.653329;m_PhiROImin[1][ 5][14]=	1.00505;m_PhiROImax[1][ 5][14]=	 1.08772;
     m_EtaROImin[1][ 5][15]=   0.649934;m_EtaROImax[1][ 5][15]=   0.741516;m_PhiROImin[1][ 5][15]=	1.00505;m_PhiROImax[1][ 5][15]=	 1.08772;
     m_EtaROImin[1][ 5][16]=   0.756521;m_EtaROImax[1][ 5][16]=   0.837822;m_PhiROImin[1][ 5][16]=	1.09135;m_PhiROImax[1][ 5][16]=	 1.17531;
     m_EtaROImin[1][ 5][17]=	    0;m_EtaROImax[1][ 5][17]=	     0;m_PhiROImin[1][ 5][17]=	      0;m_PhiROImax[1][ 5][17]=	       0;
     m_EtaROImin[1][ 5][18]=   0.756521;m_EtaROImax[1][ 5][18]=   0.837822;m_PhiROImin[1][ 5][18]=	1.00505;m_PhiROImax[1][ 5][18]=	 1.08772;
     m_EtaROImin[1][ 5][19]=	    0;m_EtaROImax[1][ 5][19]=	     0;m_PhiROImin[1][ 5][19]=	      0;m_PhiROImax[1][ 5][19]=	       0;
     m_EtaROImin[1][ 5][20]=   0.844116;m_EtaROImax[1][ 5][20]=   0.903324;m_PhiROImin[1][ 5][20]=	1.09135;m_PhiROImax[1][ 5][20]=	 1.17531;
     m_EtaROImin[1][ 5][21]=   0.899344;m_EtaROImax[1][ 5][21]=   0.956037;m_PhiROImin[1][ 5][21]=	1.09135;m_PhiROImax[1][ 5][21]=	 1.17531;
     m_EtaROImin[1][ 5][22]=   0.844116;m_EtaROImax[1][ 5][22]=   0.903324;m_PhiROImin[1][ 5][22]=	1.00505;m_PhiROImax[1][ 5][22]=	 1.08772;
     m_EtaROImin[1][ 5][23]=   0.899344;m_EtaROImax[1][ 5][23]=   0.956037;m_PhiROImin[1][ 5][23]=	1.00505;m_PhiROImax[1][ 5][23]=	 1.08772;
     m_EtaROImin[1][ 5][24]=	    0;m_EtaROImax[1][ 5][24]=	     0;m_PhiROImin[1][ 5][24]=	      0;m_PhiROImax[1][ 5][24]=	       0;
     m_EtaROImin[1][ 5][25]=	    0;m_EtaROImax[1][ 5][25]=	     0;m_PhiROImin[1][ 5][25]=	      0;m_PhiROImax[1][ 5][25]=	       0;
     m_EtaROImin[1][ 5][26]=	    0;m_EtaROImax[1][ 5][26]=	     0;m_PhiROImin[1][ 5][26]=	      0;m_PhiROImax[1][ 5][26]=	       0;
     m_EtaROImin[1][ 5][27]=	    0;m_EtaROImax[1][ 5][27]=	     0;m_PhiROImin[1][ 5][27]=	      0;m_PhiROImax[1][ 5][27]=	       0;
     m_EtaROImin[1][ 5][28]=	    0;m_EtaROImax[1][ 5][28]=	     0;m_PhiROImin[1][ 5][28]=	      0;m_PhiROImax[1][ 5][28]=	       0;
     m_EtaROImin[1][ 5][29]=	    0;m_EtaROImax[1][ 5][29]=	     0;m_PhiROImin[1][ 5][29]=	      0;m_PhiROImax[1][ 5][29]=	       0;
     m_EtaROImin[1][ 5][30]=	    0;m_EtaROImax[1][ 5][30]=	     0;m_PhiROImin[1][ 5][30]=	      0;m_PhiROImax[1][ 5][30]=	       0;
     m_EtaROImin[1][ 5][31]=	    0;m_EtaROImax[1][ 5][31]=	     0;m_PhiROImin[1][ 5][31]=	      0;m_PhiROImax[1][ 5][31]=	       0;
     m_EtaROImin[1][ 6][ 0]=   0.116816;m_EtaROImax[1][ 6][ 0]=   0.213185;m_PhiROImin[1][ 6][ 0]=	1.18089;m_PhiROImax[1][ 6][ 0]=	 1.26484;
     m_EtaROImin[1][ 6][ 1]=  0.0208251;m_EtaROImax[1][ 6][ 1]=   0.118734;m_PhiROImin[1][ 6][ 1]=	1.18089;m_PhiROImax[1][ 6][ 1]=	 1.26484;
     m_EtaROImin[1][ 6][ 2]=   0.116816;m_EtaROImax[1][ 6][ 2]=   0.213185;m_PhiROImin[1][ 6][ 2]=	1.26847;m_PhiROImax[1][ 6][ 2]=	 1.35115;
     m_EtaROImin[1][ 6][ 3]=  0.0208251;m_EtaROImax[1][ 6][ 3]=   0.118734;m_PhiROImin[1][ 6][ 3]=	1.26847;m_PhiROImax[1][ 6][ 3]=	 1.35115;
     m_EtaROImin[1][ 6][ 4]=    0.30075;m_EtaROImax[1][ 6][ 4]=   0.416721;m_PhiROImin[1][ 6][ 4]=	1.18089;m_PhiROImax[1][ 6][ 4]=	 1.26484;
     m_EtaROImin[1][ 6][ 5]=   0.222449;m_EtaROImax[1][ 6][ 5]=   0.302928;m_PhiROImin[1][ 6][ 5]=	1.18089;m_PhiROImax[1][ 6][ 5]=	 1.26484;
     m_EtaROImin[1][ 6][ 6]=    0.30075;m_EtaROImax[1][ 6][ 6]=   0.416721;m_PhiROImin[1][ 6][ 6]=	1.26847;m_PhiROImax[1][ 6][ 6]=	 1.35115;
     m_EtaROImin[1][ 6][ 7]=   0.222449;m_EtaROImax[1][ 6][ 7]=   0.302928;m_PhiROImin[1][ 6][ 7]=	1.26847;m_PhiROImax[1][ 6][ 7]=	 1.35115;
     m_EtaROImin[1][ 6][ 8]=   0.501681;m_EtaROImax[1][ 6][ 8]=   0.573871;m_PhiROImin[1][ 6][ 8]=	1.18089;m_PhiROImax[1][ 6][ 8]=	 1.26484;
     m_EtaROImin[1][ 6][ 9]=    0.42967;m_EtaROImax[1][ 6][ 9]=   0.504617;m_PhiROImin[1][ 6][ 9]=	1.18089;m_PhiROImax[1][ 6][ 9]=	 1.26484;
     m_EtaROImin[1][ 6][10]=   0.501681;m_EtaROImax[1][ 6][10]=   0.573871;m_PhiROImin[1][ 6][10]=	1.26847;m_PhiROImax[1][ 6][10]=	 1.35115;
     m_EtaROImin[1][ 6][11]=    0.42967;m_EtaROImax[1][ 6][11]=   0.504617;m_PhiROImin[1][ 6][11]=	1.26847;m_PhiROImax[1][ 6][11]=	 1.35115;
     m_EtaROImin[1][ 6][12]=   0.649934;m_EtaROImax[1][ 6][12]=   0.741516;m_PhiROImin[1][ 6][12]=	1.18089;m_PhiROImax[1][ 6][12]=	 1.26484;
     m_EtaROImin[1][ 6][13]=   0.583785;m_EtaROImax[1][ 6][13]=   0.653329;m_PhiROImin[1][ 6][13]=	1.18089;m_PhiROImax[1][ 6][13]=	 1.26484;
     m_EtaROImin[1][ 6][14]=   0.649934;m_EtaROImax[1][ 6][14]=   0.741516;m_PhiROImin[1][ 6][14]=	1.26847;m_PhiROImax[1][ 6][14]=	 1.35115;
     m_EtaROImin[1][ 6][15]=   0.583785;m_EtaROImax[1][ 6][15]=   0.653329;m_PhiROImin[1][ 6][15]=	1.26847;m_PhiROImax[1][ 6][15]=	 1.35115;
     m_EtaROImin[1][ 6][16]=	    0;m_EtaROImax[1][ 6][16]=	     0;m_PhiROImin[1][ 6][16]=	      0;m_PhiROImax[1][ 6][16]=	       0;
     m_EtaROImin[1][ 6][17]=   0.756521;m_EtaROImax[1][ 6][17]=   0.837822;m_PhiROImin[1][ 6][17]=	1.18089;m_PhiROImax[1][ 6][17]=	 1.26484;
     m_EtaROImin[1][ 6][18]=	    0;m_EtaROImax[1][ 6][18]=	     0;m_PhiROImin[1][ 6][18]=	      0;m_PhiROImax[1][ 6][18]=	       0;
     m_EtaROImin[1][ 6][19]=   0.756521;m_EtaROImax[1][ 6][19]=   0.837822;m_PhiROImin[1][ 6][19]=	1.26847;m_PhiROImax[1][ 6][19]=	 1.35115;
     m_EtaROImin[1][ 6][20]=   0.899344;m_EtaROImax[1][ 6][20]=   0.956037;m_PhiROImin[1][ 6][20]=	1.18089;m_PhiROImax[1][ 6][20]=	 1.26484;
     m_EtaROImin[1][ 6][21]=   0.844116;m_EtaROImax[1][ 6][21]=   0.903324;m_PhiROImin[1][ 6][21]=	1.18089;m_PhiROImax[1][ 6][21]=	 1.26484;
     m_EtaROImin[1][ 6][22]=   0.899344;m_EtaROImax[1][ 6][22]=   0.956037;m_PhiROImin[1][ 6][22]=	1.26847;m_PhiROImax[1][ 6][22]=	 1.35115;
     m_EtaROImin[1][ 6][23]=   0.844116;m_EtaROImax[1][ 6][23]=   0.903324;m_PhiROImin[1][ 6][23]=	1.26847;m_PhiROImax[1][ 6][23]=	 1.35115;
     m_EtaROImin[1][ 6][24]=	    0;m_EtaROImax[1][ 6][24]=	     0;m_PhiROImin[1][ 6][24]=	      0;m_PhiROImax[1][ 6][24]=	       0;
     m_EtaROImin[1][ 6][25]=	    0;m_EtaROImax[1][ 6][25]=	     0;m_PhiROImin[1][ 6][25]=	      0;m_PhiROImax[1][ 6][25]=	       0;
     m_EtaROImin[1][ 6][26]=	    0;m_EtaROImax[1][ 6][26]=	     0;m_PhiROImin[1][ 6][26]=	      0;m_PhiROImax[1][ 6][26]=	       0;
     m_EtaROImin[1][ 6][27]=	    0;m_EtaROImax[1][ 6][27]=	     0;m_PhiROImin[1][ 6][27]=	      0;m_PhiROImax[1][ 6][27]=	       0;
     m_EtaROImin[1][ 6][28]=	    0;m_EtaROImax[1][ 6][28]=	     0;m_PhiROImin[1][ 6][28]=	      0;m_PhiROImax[1][ 6][28]=	       0;
     m_EtaROImin[1][ 6][29]=	    0;m_EtaROImax[1][ 6][29]=	     0;m_PhiROImin[1][ 6][29]=	      0;m_PhiROImax[1][ 6][29]=	       0;
     m_EtaROImin[1][ 6][30]=	    0;m_EtaROImax[1][ 6][30]=	     0;m_PhiROImin[1][ 6][30]=	      0;m_PhiROImax[1][ 6][30]=	       0;
     m_EtaROImin[1][ 6][31]=	    0;m_EtaROImax[1][ 6][31]=	     0;m_PhiROImin[1][ 6][31]=	      0;m_PhiROImax[1][ 6][31]=	       0;
     m_EtaROImin[1][ 7][ 0]=  0.0552456;m_EtaROImax[1][ 7][ 0]=   0.147216;m_PhiROImin[1][ 7][ 0]=	1.45818;m_PhiROImax[1][ 7][ 0]=	 1.56784;
     m_EtaROImin[1][ 7][ 1]=    0.14708;m_EtaROImax[1][ 7][ 1]=   0.238313;m_PhiROImin[1][ 7][ 1]=	1.45818;m_PhiROImax[1][ 7][ 1]=	 1.56784;
     m_EtaROImin[1][ 7][ 2]=  0.0552456;m_EtaROImax[1][ 7][ 2]=   0.147216;m_PhiROImin[1][ 7][ 2]=	1.34779;m_PhiROImax[1][ 7][ 2]=	 1.45468;
     m_EtaROImin[1][ 7][ 3]=    0.14708;m_EtaROImax[1][ 7][ 3]=   0.238313;m_PhiROImin[1][ 7][ 3]=	1.34779;m_PhiROImax[1][ 7][ 3]=	 1.45468;
     m_EtaROImin[1][ 7][ 4]=   0.246207;m_EtaROImax[1][ 7][ 4]=    0.35049;m_PhiROImin[1][ 7][ 4]=	1.45818;m_PhiROImax[1][ 7][ 4]=	 1.56784;
     m_EtaROImin[1][ 7][ 5]=   0.350831;m_EtaROImax[1][ 7][ 5]=   0.452099;m_PhiROImin[1][ 7][ 5]=	1.45818;m_PhiROImax[1][ 7][ 5]=	 1.56784;
     m_EtaROImin[1][ 7][ 6]=   0.246207;m_EtaROImax[1][ 7][ 6]=    0.35049;m_PhiROImin[1][ 7][ 6]=	1.34779;m_PhiROImax[1][ 7][ 6]=	 1.45468;
     m_EtaROImin[1][ 7][ 7]=   0.350831;m_EtaROImax[1][ 7][ 7]=   0.452099;m_PhiROImin[1][ 7][ 7]=	1.34779;m_PhiROImax[1][ 7][ 7]=	 1.45468;
     m_EtaROImin[1][ 7][ 8]=   0.458091;m_EtaROImax[1][ 7][ 8]=   0.554444;m_PhiROImin[1][ 7][ 8]=	1.45818;m_PhiROImax[1][ 7][ 8]=	 1.56784;
     m_EtaROImin[1][ 7][ 9]=   0.555789;m_EtaROImax[1][ 7][ 9]=   0.648101;m_PhiROImin[1][ 7][ 9]=	1.45818;m_PhiROImax[1][ 7][ 9]=	 1.56784;
     m_EtaROImin[1][ 7][10]=   0.458091;m_EtaROImax[1][ 7][10]=   0.554444;m_PhiROImin[1][ 7][10]=	1.34779;m_PhiROImax[1][ 7][10]=	 1.45468;
     m_EtaROImin[1][ 7][11]=   0.555789;m_EtaROImax[1][ 7][11]=   0.648101;m_PhiROImin[1][ 7][11]=	1.34779;m_PhiROImax[1][ 7][11]=	 1.45468;
     m_EtaROImin[1][ 7][12]=   0.655857;m_EtaROImax[1][ 7][12]=   0.703298;m_PhiROImin[1][ 7][12]=	1.45758;m_PhiROImax[1][ 7][12]=	 1.56783;
     m_EtaROImin[1][ 7][13]=   0.706413;m_EtaROImax[1][ 7][13]=   0.776449;m_PhiROImin[1][ 7][13]=	1.45758;m_PhiROImax[1][ 7][13]=	 1.56783;
     m_EtaROImin[1][ 7][14]=   0.655857;m_EtaROImax[1][ 7][14]=   0.703298;m_PhiROImin[1][ 7][14]=	1.34663;m_PhiROImax[1][ 7][14]=	 1.45406;
     m_EtaROImin[1][ 7][15]=   0.706413;m_EtaROImax[1][ 7][15]=   0.776449;m_PhiROImin[1][ 7][15]=	1.34663;m_PhiROImax[1][ 7][15]=	 1.45406;
     m_EtaROImin[1][ 7][16]=   0.783867;m_EtaROImax[1][ 7][16]=    0.82768;m_PhiROImin[1][ 7][16]=	1.45758;m_PhiROImax[1][ 7][16]=	 1.56783;
     m_EtaROImin[1][ 7][17]=   0.830556;m_EtaROImax[1][ 7][17]=   0.895163;m_PhiROImin[1][ 7][17]=	1.45758;m_PhiROImax[1][ 7][17]=	 1.56783;
     m_EtaROImin[1][ 7][18]=   0.783867;m_EtaROImax[1][ 7][18]=    0.82768;m_PhiROImin[1][ 7][18]=	1.34663;m_PhiROImax[1][ 7][18]=	 1.45406;
     m_EtaROImin[1][ 7][19]=   0.830556;m_EtaROImax[1][ 7][19]=   0.895163;m_PhiROImin[1][ 7][19]=	1.34663;m_PhiROImax[1][ 7][19]=	 1.45406;
     m_EtaROImin[1][ 7][20]=   0.898201;m_EtaROImax[1][ 7][20]=   0.961344;m_PhiROImin[1][ 7][20]=	1.45818;m_PhiROImax[1][ 7][20]=	 1.56784;
     m_EtaROImin[1][ 7][21]=   0.964674;m_EtaROImax[1][ 7][21]=    1.02514;m_PhiROImin[1][ 7][21]=	1.45818;m_PhiROImax[1][ 7][21]=	 1.56784;
     m_EtaROImin[1][ 7][22]=   0.898201;m_EtaROImax[1][ 7][22]=   0.961344;m_PhiROImin[1][ 7][22]=	1.34779;m_PhiROImax[1][ 7][22]=	 1.45468;
     m_EtaROImin[1][ 7][23]=   0.964674;m_EtaROImax[1][ 7][23]=    1.02514;m_PhiROImin[1][ 7][23]=	1.34779;m_PhiROImax[1][ 7][23]=	 1.45468;
     m_EtaROImin[1][ 7][24]=    1.03003;m_EtaROImax[1][ 7][24]=    1.06547;m_PhiROImin[1][ 7][24]=	1.45737;m_PhiROImax[1][ 7][24]=	 1.56773;
     m_EtaROImin[1][ 7][25]=	    0;m_EtaROImax[1][ 7][25]=	     0;m_PhiROImin[1][ 7][25]=	      0;m_PhiROImax[1][ 7][25]=	       0;
     m_EtaROImin[1][ 7][26]=    1.03003;m_EtaROImax[1][ 7][26]=    1.06547;m_PhiROImin[1][ 7][26]=	1.37365;m_PhiROImax[1][ 7][26]=	 1.45384;
     m_EtaROImin[1][ 7][27]=	    0;m_EtaROImax[1][ 7][27]=	     0;m_PhiROImin[1][ 7][27]=	      0;m_PhiROImax[1][ 7][27]=	       0;
     m_EtaROImin[1][ 7][28]=	    0;m_EtaROImax[1][ 7][28]=	     0;m_PhiROImin[1][ 7][28]=	      0;m_PhiROImax[1][ 7][28]=	       0;
     m_EtaROImin[1][ 7][29]=	    0;m_EtaROImax[1][ 7][29]=	     0;m_PhiROImin[1][ 7][29]=	      0;m_PhiROImax[1][ 7][29]=	       0;
     m_EtaROImin[1][ 7][30]=	    0;m_EtaROImax[1][ 7][30]=	     0;m_PhiROImin[1][ 7][30]=	      0;m_PhiROImax[1][ 7][30]=	       0;
     m_EtaROImin[1][ 7][31]=	    0;m_EtaROImax[1][ 7][31]=	     0;m_PhiROImin[1][ 7][31]=	      0;m_PhiROImax[1][ 7][31]=	       0;
     m_EtaROImin[1][ 8][ 0]=    0.14708;m_EtaROImax[1][ 8][ 0]=   0.238313;m_PhiROImin[1][ 8][ 0]=	1.57375;m_PhiROImax[1][ 8][ 0]=	 1.68341;
     m_EtaROImin[1][ 8][ 1]=  0.0552456;m_EtaROImax[1][ 8][ 1]=   0.147216;m_PhiROImin[1][ 8][ 1]=	1.57375;m_PhiROImax[1][ 8][ 1]=	 1.68341;
     m_EtaROImin[1][ 8][ 2]=    0.14708;m_EtaROImax[1][ 8][ 2]=   0.238313;m_PhiROImin[1][ 8][ 2]=	1.68692;m_PhiROImax[1][ 8][ 2]=	 1.79381;
     m_EtaROImin[1][ 8][ 3]=  0.0552456;m_EtaROImax[1][ 8][ 3]=   0.147216;m_PhiROImin[1][ 8][ 3]=	1.68692;m_PhiROImax[1][ 8][ 3]=	 1.79381;
     m_EtaROImin[1][ 8][ 4]=   0.350831;m_EtaROImax[1][ 8][ 4]=   0.452099;m_PhiROImin[1][ 8][ 4]=	1.57375;m_PhiROImax[1][ 8][ 4]=	 1.68341;
     m_EtaROImin[1][ 8][ 5]=   0.246207;m_EtaROImax[1][ 8][ 5]=    0.35049;m_PhiROImin[1][ 8][ 5]=	1.57375;m_PhiROImax[1][ 8][ 5]=	 1.68341;
     m_EtaROImin[1][ 8][ 6]=   0.350831;m_EtaROImax[1][ 8][ 6]=   0.452099;m_PhiROImin[1][ 8][ 6]=	1.68692;m_PhiROImax[1][ 8][ 6]=	 1.79381;
     m_EtaROImin[1][ 8][ 7]=   0.246207;m_EtaROImax[1][ 8][ 7]=    0.35049;m_PhiROImin[1][ 8][ 7]=	1.68692;m_PhiROImax[1][ 8][ 7]=	 1.79381;
     m_EtaROImin[1][ 8][ 8]=   0.555789;m_EtaROImax[1][ 8][ 8]=   0.648101;m_PhiROImin[1][ 8][ 8]=	1.57375;m_PhiROImax[1][ 8][ 8]=	 1.68341;
     m_EtaROImin[1][ 8][ 9]=   0.458091;m_EtaROImax[1][ 8][ 9]=   0.554444;m_PhiROImin[1][ 8][ 9]=	1.57375;m_PhiROImax[1][ 8][ 9]=	 1.68341;
     m_EtaROImin[1][ 8][10]=   0.555789;m_EtaROImax[1][ 8][10]=   0.648101;m_PhiROImin[1][ 8][10]=	1.68692;m_PhiROImax[1][ 8][10]=	 1.79381;
     m_EtaROImin[1][ 8][11]=   0.458091;m_EtaROImax[1][ 8][11]=   0.554444;m_PhiROImin[1][ 8][11]=	1.68692;m_PhiROImax[1][ 8][11]=	 1.79381;
     m_EtaROImin[1][ 8][12]=   0.731124;m_EtaROImax[1][ 8][12]=   0.776449;m_PhiROImin[1][ 8][12]=	1.57376;m_PhiROImax[1][ 8][12]=	 1.68401;
     m_EtaROImin[1][ 8][13]=   0.655857;m_EtaROImax[1][ 8][13]=   0.728056;m_PhiROImin[1][ 8][13]=	1.57376;m_PhiROImax[1][ 8][13]=	 1.68401;
     m_EtaROImin[1][ 8][14]=   0.731124;m_EtaROImax[1][ 8][14]=   0.776449;m_PhiROImin[1][ 8][14]=	1.68753;m_PhiROImax[1][ 8][14]=	 1.79497;
     m_EtaROImin[1][ 8][15]=   0.655857;m_EtaROImax[1][ 8][15]=   0.728056;m_PhiROImin[1][ 8][15]=	1.68753;m_PhiROImax[1][ 8][15]=	 1.79497;
     m_EtaROImin[1][ 8][16]=   0.853359;m_EtaROImax[1][ 8][16]=   0.895163;m_PhiROImin[1][ 8][16]=	1.57376;m_PhiROImax[1][ 8][16]=	 1.68401;
     m_EtaROImin[1][ 8][17]=   0.783867;m_EtaROImax[1][ 8][17]=   0.850528;m_PhiROImin[1][ 8][17]=	1.57376;m_PhiROImax[1][ 8][17]=	 1.68401;
     m_EtaROImin[1][ 8][18]=   0.853359;m_EtaROImax[1][ 8][18]=   0.895163;m_PhiROImin[1][ 8][18]=	1.68753;m_PhiROImax[1][ 8][18]=	 1.79497;
     m_EtaROImin[1][ 8][19]=   0.783867;m_EtaROImax[1][ 8][19]=   0.850528;m_PhiROImin[1][ 8][19]=	1.68753;m_PhiROImax[1][ 8][19]=	 1.79497;
     m_EtaROImin[1][ 8][20]=   0.964674;m_EtaROImax[1][ 8][20]=    1.02514;m_PhiROImin[1][ 8][20]=	1.57375;m_PhiROImax[1][ 8][20]=	 1.68341;
     m_EtaROImin[1][ 8][21]=   0.898201;m_EtaROImax[1][ 8][21]=   0.961344;m_PhiROImin[1][ 8][21]=	1.57375;m_PhiROImax[1][ 8][21]=	 1.68341;
     m_EtaROImin[1][ 8][22]=   0.964674;m_EtaROImax[1][ 8][22]=    1.02514;m_PhiROImin[1][ 8][22]=	1.68692;m_PhiROImax[1][ 8][22]=	 1.79381;
     m_EtaROImin[1][ 8][23]=   0.898201;m_EtaROImax[1][ 8][23]=   0.961344;m_PhiROImin[1][ 8][23]=	1.68692;m_PhiROImax[1][ 8][23]=	 1.79381;
     m_EtaROImin[1][ 8][24]=	    0;m_EtaROImax[1][ 8][24]=	     0;m_PhiROImin[1][ 8][24]=	      0;m_PhiROImax[1][ 8][24]=	       0;
     m_EtaROImin[1][ 8][25]=    1.03003;m_EtaROImax[1][ 8][25]=    1.06547;m_PhiROImin[1][ 8][25]=	1.57387;m_PhiROImax[1][ 8][25]=	 1.68423;
     m_EtaROImin[1][ 8][26]=	    0;m_EtaROImax[1][ 8][26]=	     0;m_PhiROImin[1][ 8][26]=	      0;m_PhiROImax[1][ 8][26]=	       0;
     m_EtaROImin[1][ 8][27]=    1.03003;m_EtaROImax[1][ 8][27]=    1.06547;m_PhiROImin[1][ 8][27]=	1.68775;m_PhiROImax[1][ 8][27]=	 1.76794;
     m_EtaROImin[1][ 8][28]=	    0;m_EtaROImax[1][ 8][28]=	     0;m_PhiROImin[1][ 8][28]=	      0;m_PhiROImax[1][ 8][28]=	       0;
     m_EtaROImin[1][ 8][29]=	    0;m_EtaROImax[1][ 8][29]=	     0;m_PhiROImin[1][ 8][29]=	      0;m_PhiROImax[1][ 8][29]=	       0;
     m_EtaROImin[1][ 8][30]=	    0;m_EtaROImax[1][ 8][30]=	     0;m_PhiROImin[1][ 8][30]=	      0;m_PhiROImax[1][ 8][30]=	       0;
     m_EtaROImin[1][ 8][31]=	    0;m_EtaROImax[1][ 8][31]=	     0;m_PhiROImin[1][ 8][31]=	      0;m_PhiROImax[1][ 8][31]=	       0;
     m_EtaROImin[1][ 9][ 0]=  0.0208251;m_EtaROImax[1][ 9][ 0]=   0.118734;m_PhiROImin[1][ 9][ 0]=	1.87675;m_PhiROImax[1][ 9][ 0]=	 1.96071;
     m_EtaROImin[1][ 9][ 1]=   0.116816;m_EtaROImax[1][ 9][ 1]=   0.213185;m_PhiROImin[1][ 9][ 1]=	1.87675;m_PhiROImax[1][ 9][ 1]=	 1.96071;
     m_EtaROImin[1][ 9][ 2]=  0.0208251;m_EtaROImax[1][ 9][ 2]=   0.118734;m_PhiROImin[1][ 9][ 2]=	1.79045;m_PhiROImax[1][ 9][ 2]=	 1.87312;
     m_EtaROImin[1][ 9][ 3]=   0.116816;m_EtaROImax[1][ 9][ 3]=   0.213185;m_PhiROImin[1][ 9][ 3]=	1.79045;m_PhiROImax[1][ 9][ 3]=	 1.87312;
     m_EtaROImin[1][ 9][ 4]=   0.222449;m_EtaROImax[1][ 9][ 4]=   0.302928;m_PhiROImin[1][ 9][ 4]=	1.87675;m_PhiROImax[1][ 9][ 4]=	 1.96071;
     m_EtaROImin[1][ 9][ 5]=    0.30075;m_EtaROImax[1][ 9][ 5]=   0.416721;m_PhiROImin[1][ 9][ 5]=	1.87675;m_PhiROImax[1][ 9][ 5]=	 1.96071;
     m_EtaROImin[1][ 9][ 6]=   0.222449;m_EtaROImax[1][ 9][ 6]=   0.302928;m_PhiROImin[1][ 9][ 6]=	1.79045;m_PhiROImax[1][ 9][ 6]=	 1.87312;
     m_EtaROImin[1][ 9][ 7]=    0.30075;m_EtaROImax[1][ 9][ 7]=   0.416721;m_PhiROImin[1][ 9][ 7]=	1.79045;m_PhiROImax[1][ 9][ 7]=	 1.87312;
     m_EtaROImin[1][ 9][ 8]=    0.42967;m_EtaROImax[1][ 9][ 8]=   0.504617;m_PhiROImin[1][ 9][ 8]=	1.87675;m_PhiROImax[1][ 9][ 8]=	 1.96071;
     m_EtaROImin[1][ 9][ 9]=   0.501681;m_EtaROImax[1][ 9][ 9]=   0.573871;m_PhiROImin[1][ 9][ 9]=	1.87675;m_PhiROImax[1][ 9][ 9]=	 1.96071;
     m_EtaROImin[1][ 9][10]=    0.42967;m_EtaROImax[1][ 9][10]=   0.504617;m_PhiROImin[1][ 9][10]=	1.79045;m_PhiROImax[1][ 9][10]=	 1.87312;
     m_EtaROImin[1][ 9][11]=   0.501681;m_EtaROImax[1][ 9][11]=   0.573871;m_PhiROImin[1][ 9][11]=	1.79045;m_PhiROImax[1][ 9][11]=	 1.87312;
     m_EtaROImin[1][ 9][12]=   0.583785;m_EtaROImax[1][ 9][12]=   0.653329;m_PhiROImin[1][ 9][12]=	1.87675;m_PhiROImax[1][ 9][12]=	 1.96071;
     m_EtaROImin[1][ 9][13]=   0.649934;m_EtaROImax[1][ 9][13]=   0.741516;m_PhiROImin[1][ 9][13]=	1.87675;m_PhiROImax[1][ 9][13]=	 1.96071;
     m_EtaROImin[1][ 9][14]=   0.583785;m_EtaROImax[1][ 9][14]=   0.653329;m_PhiROImin[1][ 9][14]=	1.79045;m_PhiROImax[1][ 9][14]=	 1.87312;
     m_EtaROImin[1][ 9][15]=   0.649934;m_EtaROImax[1][ 9][15]=   0.741516;m_PhiROImin[1][ 9][15]=	1.79045;m_PhiROImax[1][ 9][15]=	 1.87312;
     m_EtaROImin[1][ 9][16]=   0.756521;m_EtaROImax[1][ 9][16]=   0.837822;m_PhiROImin[1][ 9][16]=	1.87675;m_PhiROImax[1][ 9][16]=	 1.96071;
     m_EtaROImin[1][ 9][17]=	    0;m_EtaROImax[1][ 9][17]=	     0;m_PhiROImin[1][ 9][17]=	      0;m_PhiROImax[1][ 9][17]=	       0;
     m_EtaROImin[1][ 9][18]=   0.756521;m_EtaROImax[1][ 9][18]=   0.837822;m_PhiROImin[1][ 9][18]=	1.79045;m_PhiROImax[1][ 9][18]=	 1.87312;
     m_EtaROImin[1][ 9][19]=	    0;m_EtaROImax[1][ 9][19]=	     0;m_PhiROImin[1][ 9][19]=	      0;m_PhiROImax[1][ 9][19]=	       0;
     m_EtaROImin[1][ 9][20]=   0.844116;m_EtaROImax[1][ 9][20]=   0.903324;m_PhiROImin[1][ 9][20]=	1.87675;m_PhiROImax[1][ 9][20]=	 1.96071;
     m_EtaROImin[1][ 9][21]=   0.899344;m_EtaROImax[1][ 9][21]=   0.956037;m_PhiROImin[1][ 9][21]=	1.87675;m_PhiROImax[1][ 9][21]=	 1.96071;
     m_EtaROImin[1][ 9][22]=   0.844116;m_EtaROImax[1][ 9][22]=   0.903324;m_PhiROImin[1][ 9][22]=	1.79045;m_PhiROImax[1][ 9][22]=	 1.87312;
     m_EtaROImin[1][ 9][23]=   0.899344;m_EtaROImax[1][ 9][23]=   0.956037;m_PhiROImin[1][ 9][23]=	1.79045;m_PhiROImax[1][ 9][23]=	 1.87312;
     m_EtaROImin[1][ 9][24]=	    0;m_EtaROImax[1][ 9][24]=	     0;m_PhiROImin[1][ 9][24]=	      0;m_PhiROImax[1][ 9][24]=	       0;
     m_EtaROImin[1][ 9][25]=	    0;m_EtaROImax[1][ 9][25]=	     0;m_PhiROImin[1][ 9][25]=	      0;m_PhiROImax[1][ 9][25]=	       0;
     m_EtaROImin[1][ 9][26]=	    0;m_EtaROImax[1][ 9][26]=	     0;m_PhiROImin[1][ 9][26]=	      0;m_PhiROImax[1][ 9][26]=	       0;
     m_EtaROImin[1][ 9][27]=	    0;m_EtaROImax[1][ 9][27]=	     0;m_PhiROImin[1][ 9][27]=	      0;m_PhiROImax[1][ 9][27]=	       0;
     m_EtaROImin[1][ 9][28]=	    0;m_EtaROImax[1][ 9][28]=	     0;m_PhiROImin[1][ 9][28]=	      0;m_PhiROImax[1][ 9][28]=	       0;
     m_EtaROImin[1][ 9][29]=	    0;m_EtaROImax[1][ 9][29]=	     0;m_PhiROImin[1][ 9][29]=	      0;m_PhiROImax[1][ 9][29]=	       0;
     m_EtaROImin[1][ 9][30]=	    0;m_EtaROImax[1][ 9][30]=	     0;m_PhiROImin[1][ 9][30]=	      0;m_PhiROImax[1][ 9][30]=	       0;
     m_EtaROImin[1][ 9][31]=	    0;m_EtaROImax[1][ 9][31]=	     0;m_PhiROImin[1][ 9][31]=	      0;m_PhiROImax[1][ 9][31]=	       0;
     m_EtaROImin[1][10][ 0]=   0.116816;m_EtaROImax[1][10][ 0]=   0.213185;m_PhiROImin[1][10][ 0]=	1.96629;m_PhiROImax[1][10][ 0]=	 2.05024;
     m_EtaROImin[1][10][ 1]=  0.0208251;m_EtaROImax[1][10][ 1]=   0.118734;m_PhiROImin[1][10][ 1]=	1.96629;m_PhiROImax[1][10][ 1]=	 2.05024;
     m_EtaROImin[1][10][ 2]=   0.116816;m_EtaROImax[1][10][ 2]=   0.213185;m_PhiROImin[1][10][ 2]=	2.05387;m_PhiROImax[1][10][ 2]=	 2.13655;
     m_EtaROImin[1][10][ 3]=  0.0208251;m_EtaROImax[1][10][ 3]=   0.118734;m_PhiROImin[1][10][ 3]=	2.05387;m_PhiROImax[1][10][ 3]=	 2.13655;
     m_EtaROImin[1][10][ 4]=    0.30075;m_EtaROImax[1][10][ 4]=   0.416721;m_PhiROImin[1][10][ 4]=	1.96629;m_PhiROImax[1][10][ 4]=	 2.05024;
     m_EtaROImin[1][10][ 5]=   0.222449;m_EtaROImax[1][10][ 5]=   0.302928;m_PhiROImin[1][10][ 5]=	1.96629;m_PhiROImax[1][10][ 5]=	 2.05024;
     m_EtaROImin[1][10][ 6]=    0.30075;m_EtaROImax[1][10][ 6]=   0.416721;m_PhiROImin[1][10][ 6]=	2.05387;m_PhiROImax[1][10][ 6]=	 2.13655;
     m_EtaROImin[1][10][ 7]=   0.222449;m_EtaROImax[1][10][ 7]=   0.302928;m_PhiROImin[1][10][ 7]=	2.05387;m_PhiROImax[1][10][ 7]=	 2.13655;
     m_EtaROImin[1][10][ 8]=   0.501681;m_EtaROImax[1][10][ 8]=   0.573871;m_PhiROImin[1][10][ 8]=	1.96629;m_PhiROImax[1][10][ 8]=	 2.05024;
     m_EtaROImin[1][10][ 9]=    0.42967;m_EtaROImax[1][10][ 9]=   0.504617;m_PhiROImin[1][10][ 9]=	1.96629;m_PhiROImax[1][10][ 9]=	 2.05024;
     m_EtaROImin[1][10][10]=   0.501681;m_EtaROImax[1][10][10]=   0.573871;m_PhiROImin[1][10][10]=	2.05387;m_PhiROImax[1][10][10]=	 2.13655;
     m_EtaROImin[1][10][11]=    0.42967;m_EtaROImax[1][10][11]=   0.504617;m_PhiROImin[1][10][11]=	2.05387;m_PhiROImax[1][10][11]=	 2.13655;
     m_EtaROImin[1][10][12]=   0.649934;m_EtaROImax[1][10][12]=   0.741516;m_PhiROImin[1][10][12]=	1.96629;m_PhiROImax[1][10][12]=	 2.05024;
     m_EtaROImin[1][10][13]=   0.583785;m_EtaROImax[1][10][13]=   0.653329;m_PhiROImin[1][10][13]=	1.96629;m_PhiROImax[1][10][13]=	 2.05024;
     m_EtaROImin[1][10][14]=   0.649934;m_EtaROImax[1][10][14]=   0.741516;m_PhiROImin[1][10][14]=	2.05387;m_PhiROImax[1][10][14]=	 2.13655;
     m_EtaROImin[1][10][15]=   0.583785;m_EtaROImax[1][10][15]=   0.653329;m_PhiROImin[1][10][15]=	2.05387;m_PhiROImax[1][10][15]=	 2.13655;
     m_EtaROImin[1][10][16]=	    0;m_EtaROImax[1][10][16]=	     0;m_PhiROImin[1][10][16]=	      0;m_PhiROImax[1][10][16]=	       0;
     m_EtaROImin[1][10][17]=   0.756521;m_EtaROImax[1][10][17]=   0.837822;m_PhiROImin[1][10][17]=	1.96629;m_PhiROImax[1][10][17]=	 2.05024;
     m_EtaROImin[1][10][18]=	    0;m_EtaROImax[1][10][18]=	     0;m_PhiROImin[1][10][18]=	      0;m_PhiROImax[1][10][18]=	       0;
     m_EtaROImin[1][10][19]=   0.756521;m_EtaROImax[1][10][19]=   0.837822;m_PhiROImin[1][10][19]=	2.05387;m_PhiROImax[1][10][19]=	 2.13655;
     m_EtaROImin[1][10][20]=   0.899344;m_EtaROImax[1][10][20]=   0.956037;m_PhiROImin[1][10][20]=	1.96629;m_PhiROImax[1][10][20]=	 2.05024;
     m_EtaROImin[1][10][21]=   0.844116;m_EtaROImax[1][10][21]=   0.903324;m_PhiROImin[1][10][21]=	1.96629;m_PhiROImax[1][10][21]=	 2.05024;
     m_EtaROImin[1][10][22]=   0.899344;m_EtaROImax[1][10][22]=   0.956037;m_PhiROImin[1][10][22]=	2.05387;m_PhiROImax[1][10][22]=	 2.13655;
     m_EtaROImin[1][10][23]=   0.844116;m_EtaROImax[1][10][23]=   0.903324;m_PhiROImin[1][10][23]=	2.05387;m_PhiROImax[1][10][23]=	 2.13655;
     m_EtaROImin[1][10][24]=	    0;m_EtaROImax[1][10][24]=	     0;m_PhiROImin[1][10][24]=	      0;m_PhiROImax[1][10][24]=	       0;
     m_EtaROImin[1][10][25]=	    0;m_EtaROImax[1][10][25]=	     0;m_PhiROImin[1][10][25]=	      0;m_PhiROImax[1][10][25]=	       0;
     m_EtaROImin[1][10][26]=	    0;m_EtaROImax[1][10][26]=	     0;m_PhiROImin[1][10][26]=	      0;m_PhiROImax[1][10][26]=	       0;
     m_EtaROImin[1][10][27]=	    0;m_EtaROImax[1][10][27]=	     0;m_PhiROImin[1][10][27]=	      0;m_PhiROImax[1][10][27]=	       0;
     m_EtaROImin[1][10][28]=	    0;m_EtaROImax[1][10][28]=	     0;m_PhiROImin[1][10][28]=	      0;m_PhiROImax[1][10][28]=	       0;
     m_EtaROImin[1][10][29]=	    0;m_EtaROImax[1][10][29]=	     0;m_PhiROImin[1][10][29]=	      0;m_PhiROImax[1][10][29]=	       0;
     m_EtaROImin[1][10][30]=	    0;m_EtaROImax[1][10][30]=	     0;m_PhiROImin[1][10][30]=	      0;m_PhiROImax[1][10][30]=	       0;
     m_EtaROImin[1][10][31]=	    0;m_EtaROImax[1][10][31]=	     0;m_PhiROImin[1][10][31]=	      0;m_PhiROImax[1][10][31]=	       0;
     m_EtaROImin[1][11][ 0]=   0.130922;m_EtaROImax[1][11][ 0]=   0.238612;m_PhiROImin[1][11][ 0]=	2.24298;m_PhiROImax[1][11][ 0]=	 2.35323;
     m_EtaROImin[1][11][ 1]=	    0;m_EtaROImax[1][11][ 1]=	     0;m_PhiROImin[1][11][ 1]=	      0;m_PhiROImax[1][11][ 1]=	       0;
     m_EtaROImin[1][11][ 2]=   0.130922;m_EtaROImax[1][11][ 2]=   0.238612;m_PhiROImin[1][11][ 2]=	2.13203;m_PhiROImax[1][11][ 2]=	 2.23946;
     m_EtaROImin[1][11][ 3]=	    0;m_EtaROImax[1][11][ 3]=	     0;m_PhiROImin[1][11][ 3]=	      0;m_PhiROImax[1][11][ 3]=	       0;
     m_EtaROImin[1][11][ 4]=   0.246207;m_EtaROImax[1][11][ 4]=    0.35049;m_PhiROImin[1][11][ 4]=	2.24358;m_PhiROImax[1][11][ 4]=	 2.35324;
     m_EtaROImin[1][11][ 5]=   0.350831;m_EtaROImax[1][11][ 5]=   0.452099;m_PhiROImin[1][11][ 5]=	2.24358;m_PhiROImax[1][11][ 5]=	 2.35324;
     m_EtaROImin[1][11][ 6]=   0.246207;m_EtaROImax[1][11][ 6]=    0.35049;m_PhiROImin[1][11][ 6]=	2.13318;m_PhiROImax[1][11][ 6]=	 2.24008;
     m_EtaROImin[1][11][ 7]=   0.350831;m_EtaROImax[1][11][ 7]=   0.452099;m_PhiROImin[1][11][ 7]=	2.13318;m_PhiROImax[1][11][ 7]=	 2.24008;
     m_EtaROImin[1][11][ 8]=   0.458091;m_EtaROImax[1][11][ 8]=   0.554444;m_PhiROImin[1][11][ 8]=	2.24358;m_PhiROImax[1][11][ 8]=	 2.35324;
     m_EtaROImin[1][11][ 9]=   0.555789;m_EtaROImax[1][11][ 9]=   0.648101;m_PhiROImin[1][11][ 9]=	2.24358;m_PhiROImax[1][11][ 9]=	 2.35324;
     m_EtaROImin[1][11][10]=   0.458091;m_EtaROImax[1][11][10]=   0.554444;m_PhiROImin[1][11][10]=	2.13318;m_PhiROImax[1][11][10]=	 2.24008;
     m_EtaROImin[1][11][11]=   0.555789;m_EtaROImax[1][11][11]=   0.648101;m_PhiROImin[1][11][11]=	2.13318;m_PhiROImax[1][11][11]=	 2.24008;
     m_EtaROImin[1][11][12]=   0.655857;m_EtaROImax[1][11][12]=   0.703298;m_PhiROImin[1][11][12]=	2.24298;m_PhiROImax[1][11][12]=	 2.35323;
     m_EtaROImin[1][11][13]=   0.706413;m_EtaROImax[1][11][13]=   0.776449;m_PhiROImin[1][11][13]=	2.24298;m_PhiROImax[1][11][13]=	 2.35323;
     m_EtaROImin[1][11][14]=   0.655857;m_EtaROImax[1][11][14]=   0.703298;m_PhiROImin[1][11][14]=	2.13203;m_PhiROImax[1][11][14]=	 2.23946;
     m_EtaROImin[1][11][15]=   0.706413;m_EtaROImax[1][11][15]=   0.776449;m_PhiROImin[1][11][15]=	2.13203;m_PhiROImax[1][11][15]=	 2.23946;
     m_EtaROImin[1][11][16]=   0.783867;m_EtaROImax[1][11][16]=    0.82768;m_PhiROImin[1][11][16]=	2.24298;m_PhiROImax[1][11][16]=	 2.35323;
     m_EtaROImin[1][11][17]=   0.830556;m_EtaROImax[1][11][17]=   0.895163;m_PhiROImin[1][11][17]=	2.24298;m_PhiROImax[1][11][17]=	 2.35323;
     m_EtaROImin[1][11][18]=   0.783867;m_EtaROImax[1][11][18]=    0.82768;m_PhiROImin[1][11][18]=	2.13203;m_PhiROImax[1][11][18]=	 2.23946;
     m_EtaROImin[1][11][19]=   0.830556;m_EtaROImax[1][11][19]=   0.895163;m_PhiROImin[1][11][19]=	2.13203;m_PhiROImax[1][11][19]=	 2.23946;
     m_EtaROImin[1][11][20]=   0.898201;m_EtaROImax[1][11][20]=   0.961344;m_PhiROImin[1][11][20]=	2.24358;m_PhiROImax[1][11][20]=	 2.35324;
     m_EtaROImin[1][11][21]=   0.964674;m_EtaROImax[1][11][21]=    1.02514;m_PhiROImin[1][11][21]=	2.24358;m_PhiROImax[1][11][21]=	 2.35324;
     m_EtaROImin[1][11][22]=   0.898201;m_EtaROImax[1][11][22]=   0.961344;m_PhiROImin[1][11][22]=	2.13318;m_PhiROImax[1][11][22]=	 2.24008;
     m_EtaROImin[1][11][23]=   0.964674;m_EtaROImax[1][11][23]=    1.02514;m_PhiROImin[1][11][23]=	2.13318;m_PhiROImax[1][11][23]=	 2.24008;
     m_EtaROImin[1][11][24]=    1.03003;m_EtaROImax[1][11][24]=    1.06547;m_PhiROImin[1][11][24]=	2.24277;m_PhiROImax[1][11][24]=	 2.35313;
     m_EtaROImin[1][11][25]=	    0;m_EtaROImax[1][11][25]=	     0;m_PhiROImin[1][11][25]=	      0;m_PhiROImax[1][11][25]=	       0;
     m_EtaROImin[1][11][26]=    1.03003;m_EtaROImax[1][11][26]=    1.06547;m_PhiROImin[1][11][26]=	2.15905;m_PhiROImax[1][11][26]=	 2.23924;
     m_EtaROImin[1][11][27]=	    0;m_EtaROImax[1][11][27]=	     0;m_PhiROImin[1][11][27]=	      0;m_PhiROImax[1][11][27]=	       0;
     m_EtaROImin[1][11][28]=	    0;m_EtaROImax[1][11][28]=	     0;m_PhiROImin[1][11][28]=	      0;m_PhiROImax[1][11][28]=	       0;
     m_EtaROImin[1][11][29]=	    0;m_EtaROImax[1][11][29]=	     0;m_PhiROImin[1][11][29]=	      0;m_PhiROImax[1][11][29]=	       0;
     m_EtaROImin[1][11][30]=	    0;m_EtaROImax[1][11][30]=	     0;m_PhiROImin[1][11][30]=	      0;m_PhiROImax[1][11][30]=	       0;
     m_EtaROImin[1][11][31]=	    0;m_EtaROImax[1][11][31]=	     0;m_PhiROImin[1][11][31]=	      0;m_PhiROImax[1][11][31]=	       0;
     m_EtaROImin[1][12][ 0]=	    0;m_EtaROImax[1][12][ 0]=	     0;m_PhiROImin[1][12][ 0]=	      0;m_PhiROImax[1][12][ 0]=	       0;
     m_EtaROImin[1][12][ 1]=   0.130922;m_EtaROImax[1][12][ 1]=   0.238612;m_PhiROImin[1][12][ 1]=	2.35916;m_PhiROImax[1][12][ 1]=	 2.46941;
     m_EtaROImin[1][12][ 2]=	    0;m_EtaROImax[1][12][ 2]=	     0;m_PhiROImin[1][12][ 2]=	      0;m_PhiROImax[1][12][ 2]=	       0;
     m_EtaROImin[1][12][ 3]=   0.130922;m_EtaROImax[1][12][ 3]=   0.238612;m_PhiROImin[1][12][ 3]=	2.47293;m_PhiROImax[1][12][ 3]=	 2.58036;
     m_EtaROImin[1][12][ 4]=   0.350831;m_EtaROImax[1][12][ 4]=   0.452099;m_PhiROImin[1][12][ 4]=	2.35915;m_PhiROImax[1][12][ 4]=	 2.46881;
     m_EtaROImin[1][12][ 5]=   0.246207;m_EtaROImax[1][12][ 5]=    0.35049;m_PhiROImin[1][12][ 5]=	2.35915;m_PhiROImax[1][12][ 5]=	 2.46881;
     m_EtaROImin[1][12][ 6]=   0.350831;m_EtaROImax[1][12][ 6]=   0.452099;m_PhiROImin[1][12][ 6]=	2.47231;m_PhiROImax[1][12][ 6]=	 2.57921;
     m_EtaROImin[1][12][ 7]=   0.246207;m_EtaROImax[1][12][ 7]=    0.35049;m_PhiROImin[1][12][ 7]=	2.47231;m_PhiROImax[1][12][ 7]=	 2.57921;
     m_EtaROImin[1][12][ 8]=   0.555789;m_EtaROImax[1][12][ 8]=   0.648101;m_PhiROImin[1][12][ 8]=	2.35915;m_PhiROImax[1][12][ 8]=	 2.46881;
     m_EtaROImin[1][12][ 9]=   0.458091;m_EtaROImax[1][12][ 9]=   0.554444;m_PhiROImin[1][12][ 9]=	2.35915;m_PhiROImax[1][12][ 9]=	 2.46881;
     m_EtaROImin[1][12][10]=   0.555789;m_EtaROImax[1][12][10]=   0.648101;m_PhiROImin[1][12][10]=	2.47231;m_PhiROImax[1][12][10]=	 2.57921;
     m_EtaROImin[1][12][11]=   0.458091;m_EtaROImax[1][12][11]=   0.554444;m_PhiROImin[1][12][11]=	2.47231;m_PhiROImax[1][12][11]=	 2.57921;
     m_EtaROImin[1][12][12]=   0.731124;m_EtaROImax[1][12][12]=   0.776449;m_PhiROImin[1][12][12]=	2.35916;m_PhiROImax[1][12][12]=	 2.46941;
     m_EtaROImin[1][12][13]=   0.655857;m_EtaROImax[1][12][13]=   0.728056;m_PhiROImin[1][12][13]=	2.35916;m_PhiROImax[1][12][13]=	 2.46941;
     m_EtaROImin[1][12][14]=   0.731124;m_EtaROImax[1][12][14]=   0.776449;m_PhiROImin[1][12][14]=	2.47293;m_PhiROImax[1][12][14]=	 2.58036;
     m_EtaROImin[1][12][15]=   0.655857;m_EtaROImax[1][12][15]=   0.728056;m_PhiROImin[1][12][15]=	2.47293;m_PhiROImax[1][12][15]=	 2.58036;
     m_EtaROImin[1][12][16]=   0.853359;m_EtaROImax[1][12][16]=   0.895163;m_PhiROImin[1][12][16]=	2.35916;m_PhiROImax[1][12][16]=	 2.46941;
     m_EtaROImin[1][12][17]=   0.783867;m_EtaROImax[1][12][17]=   0.850528;m_PhiROImin[1][12][17]=	2.35916;m_PhiROImax[1][12][17]=	 2.46941;
     m_EtaROImin[1][12][18]=   0.853359;m_EtaROImax[1][12][18]=   0.895163;m_PhiROImin[1][12][18]=	2.47293;m_PhiROImax[1][12][18]=	 2.58036;
     m_EtaROImin[1][12][19]=   0.783867;m_EtaROImax[1][12][19]=   0.850528;m_PhiROImin[1][12][19]=	2.47293;m_PhiROImax[1][12][19]=	 2.58036;
     m_EtaROImin[1][12][20]=   0.964674;m_EtaROImax[1][12][20]=    1.02514;m_PhiROImin[1][12][20]=	2.35915;m_PhiROImax[1][12][20]=	 2.46881;
     m_EtaROImin[1][12][21]=   0.898201;m_EtaROImax[1][12][21]=   0.961344;m_PhiROImin[1][12][21]=	2.35915;m_PhiROImax[1][12][21]=	 2.46881;
     m_EtaROImin[1][12][22]=   0.964674;m_EtaROImax[1][12][22]=    1.02514;m_PhiROImin[1][12][22]=	2.47231;m_PhiROImax[1][12][22]=	 2.57921;
     m_EtaROImin[1][12][23]=   0.898201;m_EtaROImax[1][12][23]=   0.961344;m_PhiROImin[1][12][23]=	2.47231;m_PhiROImax[1][12][23]=	 2.57921;
     m_EtaROImin[1][12][24]=	    0;m_EtaROImax[1][12][24]=	     0;m_PhiROImin[1][12][24]=	      0;m_PhiROImax[1][12][24]=	       0;
     m_EtaROImin[1][12][25]=    1.03003;m_EtaROImax[1][12][25]=    1.06547;m_PhiROImin[1][12][25]=	2.35926;m_PhiROImax[1][12][25]=	 2.46962;
     m_EtaROImin[1][12][26]=	    0;m_EtaROImax[1][12][26]=	     0;m_PhiROImin[1][12][26]=	      0;m_PhiROImax[1][12][26]=	       0;
     m_EtaROImin[1][12][27]=    1.03003;m_EtaROImax[1][12][27]=    1.06547;m_PhiROImin[1][12][27]=	2.47315;m_PhiROImax[1][12][27]=	 2.55334;
     m_EtaROImin[1][12][28]=	    0;m_EtaROImax[1][12][28]=	     0;m_PhiROImin[1][12][28]=	      0;m_PhiROImax[1][12][28]=	       0;
     m_EtaROImin[1][12][29]=	    0;m_EtaROImax[1][12][29]=	     0;m_PhiROImin[1][12][29]=	      0;m_PhiROImax[1][12][29]=	       0;
     m_EtaROImin[1][12][30]=	    0;m_EtaROImax[1][12][30]=	     0;m_PhiROImin[1][12][30]=	      0;m_PhiROImax[1][12][30]=	       0;
     m_EtaROImin[1][12][31]=	    0;m_EtaROImax[1][12][31]=	     0;m_PhiROImin[1][12][31]=	      0;m_PhiROImax[1][12][31]=	       0;
     m_EtaROImin[1][13][ 0]=  0.0208251;m_EtaROImax[1][13][ 0]=   0.118734;m_PhiROImin[1][13][ 0]=	2.66215;m_PhiROImax[1][13][ 0]=	  2.7461;
     m_EtaROImin[1][13][ 1]=   0.116816;m_EtaROImax[1][13][ 1]=   0.213185;m_PhiROImin[1][13][ 1]=	2.66215;m_PhiROImax[1][13][ 1]=	  2.7461;
     m_EtaROImin[1][13][ 2]=  0.0208251;m_EtaROImax[1][13][ 2]=   0.118734;m_PhiROImin[1][13][ 2]=	2.57584;m_PhiROImax[1][13][ 2]=	 2.65852;
     m_EtaROImin[1][13][ 3]=   0.116816;m_EtaROImax[1][13][ 3]=   0.213185;m_PhiROImin[1][13][ 3]=	2.57584;m_PhiROImax[1][13][ 3]=	 2.65852;
     m_EtaROImin[1][13][ 4]=   0.222449;m_EtaROImax[1][13][ 4]=   0.302928;m_PhiROImin[1][13][ 4]=	2.66215;m_PhiROImax[1][13][ 4]=	  2.7461;
     m_EtaROImin[1][13][ 5]=    0.30075;m_EtaROImax[1][13][ 5]=   0.416721;m_PhiROImin[1][13][ 5]=	2.66215;m_PhiROImax[1][13][ 5]=	  2.7461;
     m_EtaROImin[1][13][ 6]=   0.222449;m_EtaROImax[1][13][ 6]=   0.302928;m_PhiROImin[1][13][ 6]=	2.57584;m_PhiROImax[1][13][ 6]=	 2.65852;
     m_EtaROImin[1][13][ 7]=    0.30075;m_EtaROImax[1][13][ 7]=   0.416721;m_PhiROImin[1][13][ 7]=	2.57584;m_PhiROImax[1][13][ 7]=	 2.65852;
     m_EtaROImin[1][13][ 8]=    0.42967;m_EtaROImax[1][13][ 8]=   0.504617;m_PhiROImin[1][13][ 8]=	2.66215;m_PhiROImax[1][13][ 8]=	  2.7461;
     m_EtaROImin[1][13][ 9]=   0.501681;m_EtaROImax[1][13][ 9]=   0.573871;m_PhiROImin[1][13][ 9]=	2.66215;m_PhiROImax[1][13][ 9]=	  2.7461;
     m_EtaROImin[1][13][10]=    0.42967;m_EtaROImax[1][13][10]=   0.504617;m_PhiROImin[1][13][10]=	2.57584;m_PhiROImax[1][13][10]=	 2.65852;
     m_EtaROImin[1][13][11]=   0.501681;m_EtaROImax[1][13][11]=   0.573871;m_PhiROImin[1][13][11]=	2.57584;m_PhiROImax[1][13][11]=	 2.65852;
     m_EtaROImin[1][13][12]=   0.583785;m_EtaROImax[1][13][12]=   0.653329;m_PhiROImin[1][13][12]=	2.66215;m_PhiROImax[1][13][12]=	  2.7461;
     m_EtaROImin[1][13][13]=   0.649934;m_EtaROImax[1][13][13]=   0.741516;m_PhiROImin[1][13][13]=	2.66215;m_PhiROImax[1][13][13]=	  2.7461;
     m_EtaROImin[1][13][14]=   0.583785;m_EtaROImax[1][13][14]=   0.653329;m_PhiROImin[1][13][14]=	2.57584;m_PhiROImax[1][13][14]=	 2.65852;
     m_EtaROImin[1][13][15]=   0.649934;m_EtaROImax[1][13][15]=   0.741516;m_PhiROImin[1][13][15]=	2.57584;m_PhiROImax[1][13][15]=	 2.65852;
     m_EtaROImin[1][13][16]=   0.756521;m_EtaROImax[1][13][16]=   0.837822;m_PhiROImin[1][13][16]=	2.66215;m_PhiROImax[1][13][16]=	  2.7461;
     m_EtaROImin[1][13][17]=	    0;m_EtaROImax[1][13][17]=	     0;m_PhiROImin[1][13][17]=	      0;m_PhiROImax[1][13][17]=	       0;
     m_EtaROImin[1][13][18]=   0.756521;m_EtaROImax[1][13][18]=   0.837822;m_PhiROImin[1][13][18]=	2.57584;m_PhiROImax[1][13][18]=	 2.65852;
     m_EtaROImin[1][13][19]=	    0;m_EtaROImax[1][13][19]=	     0;m_PhiROImin[1][13][19]=	      0;m_PhiROImax[1][13][19]=	       0;
     m_EtaROImin[1][13][20]=   0.844116;m_EtaROImax[1][13][20]=   0.903324;m_PhiROImin[1][13][20]=	2.66215;m_PhiROImax[1][13][20]=	  2.7461;
     m_EtaROImin[1][13][21]=   0.899344;m_EtaROImax[1][13][21]=   0.956037;m_PhiROImin[1][13][21]=	2.66215;m_PhiROImax[1][13][21]=	  2.7461;
     m_EtaROImin[1][13][22]=   0.844116;m_EtaROImax[1][13][22]=   0.903324;m_PhiROImin[1][13][22]=	2.57584;m_PhiROImax[1][13][22]=	 2.65852;
     m_EtaROImin[1][13][23]=   0.899344;m_EtaROImax[1][13][23]=   0.956037;m_PhiROImin[1][13][23]=	2.57584;m_PhiROImax[1][13][23]=	 2.65852;
     m_EtaROImin[1][13][24]=	    0;m_EtaROImax[1][13][24]=	     0;m_PhiROImin[1][13][24]=	      0;m_PhiROImax[1][13][24]=	       0;
     m_EtaROImin[1][13][25]=	    0;m_EtaROImax[1][13][25]=	     0;m_PhiROImin[1][13][25]=	      0;m_PhiROImax[1][13][25]=	       0;
     m_EtaROImin[1][13][26]=	    0;m_EtaROImax[1][13][26]=	     0;m_PhiROImin[1][13][26]=	      0;m_PhiROImax[1][13][26]=	       0;
     m_EtaROImin[1][13][27]=	    0;m_EtaROImax[1][13][27]=	     0;m_PhiROImin[1][13][27]=	      0;m_PhiROImax[1][13][27]=	       0;
     m_EtaROImin[1][13][28]=	    0;m_EtaROImax[1][13][28]=	     0;m_PhiROImin[1][13][28]=	      0;m_PhiROImax[1][13][28]=	       0;
     m_EtaROImin[1][13][29]=	    0;m_EtaROImax[1][13][29]=	     0;m_PhiROImin[1][13][29]=	      0;m_PhiROImax[1][13][29]=	       0;
     m_EtaROImin[1][13][30]=	    0;m_EtaROImax[1][13][30]=	     0;m_PhiROImin[1][13][30]=	      0;m_PhiROImax[1][13][30]=	       0;
     m_EtaROImin[1][13][31]=	    0;m_EtaROImax[1][13][31]=	     0;m_PhiROImin[1][13][31]=	      0;m_PhiROImax[1][13][31]=	       0;
     m_EtaROImin[1][14][ 0]=   0.116816;m_EtaROImax[1][14][ 0]=   0.213185;m_PhiROImin[1][14][ 0]=	2.75168;m_PhiROImax[1][14][ 0]=	 2.83564;
     m_EtaROImin[1][14][ 1]=  0.0208251;m_EtaROImax[1][14][ 1]=   0.118734;m_PhiROImin[1][14][ 1]=	2.75168;m_PhiROImax[1][14][ 1]=	 2.83564;
     m_EtaROImin[1][14][ 2]=   0.116816;m_EtaROImax[1][14][ 2]=   0.213185;m_PhiROImin[1][14][ 2]=	2.83927;m_PhiROImax[1][14][ 2]=	 2.92194;
     m_EtaROImin[1][14][ 3]=  0.0208251;m_EtaROImax[1][14][ 3]=   0.118734;m_PhiROImin[1][14][ 3]=	2.83927;m_PhiROImax[1][14][ 3]=	 2.92194;
     m_EtaROImin[1][14][ 4]=    0.30075;m_EtaROImax[1][14][ 4]=   0.416721;m_PhiROImin[1][14][ 4]=	2.75168;m_PhiROImax[1][14][ 4]=	 2.83564;
     m_EtaROImin[1][14][ 5]=   0.222449;m_EtaROImax[1][14][ 5]=   0.302928;m_PhiROImin[1][14][ 5]=	2.75168;m_PhiROImax[1][14][ 5]=	 2.83564;
     m_EtaROImin[1][14][ 6]=    0.30075;m_EtaROImax[1][14][ 6]=   0.416721;m_PhiROImin[1][14][ 6]=	2.83927;m_PhiROImax[1][14][ 6]=	 2.92194;
     m_EtaROImin[1][14][ 7]=   0.222449;m_EtaROImax[1][14][ 7]=   0.302928;m_PhiROImin[1][14][ 7]=	2.83927;m_PhiROImax[1][14][ 7]=	 2.92194;
     m_EtaROImin[1][14][ 8]=   0.501681;m_EtaROImax[1][14][ 8]=   0.573871;m_PhiROImin[1][14][ 8]=	2.75168;m_PhiROImax[1][14][ 8]=	 2.83564;
     m_EtaROImin[1][14][ 9]=    0.42967;m_EtaROImax[1][14][ 9]=   0.504617;m_PhiROImin[1][14][ 9]=	2.75168;m_PhiROImax[1][14][ 9]=	 2.83564;
     m_EtaROImin[1][14][10]=   0.501681;m_EtaROImax[1][14][10]=   0.573871;m_PhiROImin[1][14][10]=	2.83927;m_PhiROImax[1][14][10]=	 2.92194;
     m_EtaROImin[1][14][11]=    0.42967;m_EtaROImax[1][14][11]=   0.504617;m_PhiROImin[1][14][11]=	2.83927;m_PhiROImax[1][14][11]=	 2.92194;
     m_EtaROImin[1][14][12]=   0.649934;m_EtaROImax[1][14][12]=   0.741516;m_PhiROImin[1][14][12]=	2.75168;m_PhiROImax[1][14][12]=	 2.83564;
     m_EtaROImin[1][14][13]=   0.583785;m_EtaROImax[1][14][13]=   0.653329;m_PhiROImin[1][14][13]=	2.75168;m_PhiROImax[1][14][13]=	 2.83564;
     m_EtaROImin[1][14][14]=   0.649934;m_EtaROImax[1][14][14]=   0.741516;m_PhiROImin[1][14][14]=	2.83927;m_PhiROImax[1][14][14]=	 2.92194;
     m_EtaROImin[1][14][15]=   0.583785;m_EtaROImax[1][14][15]=   0.653329;m_PhiROImin[1][14][15]=	2.83927;m_PhiROImax[1][14][15]=	 2.92194;
     m_EtaROImin[1][14][16]=	    0;m_EtaROImax[1][14][16]=	     0;m_PhiROImin[1][14][16]=	      0;m_PhiROImax[1][14][16]=	       0;
     m_EtaROImin[1][14][17]=   0.756521;m_EtaROImax[1][14][17]=   0.837822;m_PhiROImin[1][14][17]=	2.75168;m_PhiROImax[1][14][17]=	 2.83564;
     m_EtaROImin[1][14][18]=	    0;m_EtaROImax[1][14][18]=	     0;m_PhiROImin[1][14][18]=	      0;m_PhiROImax[1][14][18]=	       0;
     m_EtaROImin[1][14][19]=   0.756521;m_EtaROImax[1][14][19]=   0.837822;m_PhiROImin[1][14][19]=	2.83927;m_PhiROImax[1][14][19]=	 2.92194;
     m_EtaROImin[1][14][20]=   0.899344;m_EtaROImax[1][14][20]=   0.956037;m_PhiROImin[1][14][20]=	2.75168;m_PhiROImax[1][14][20]=	 2.83564;
     m_EtaROImin[1][14][21]=   0.844116;m_EtaROImax[1][14][21]=   0.903324;m_PhiROImin[1][14][21]=	2.75168;m_PhiROImax[1][14][21]=	 2.83564;
     m_EtaROImin[1][14][22]=   0.899344;m_EtaROImax[1][14][22]=   0.956037;m_PhiROImin[1][14][22]=	2.83927;m_PhiROImax[1][14][22]=	 2.92194;
     m_EtaROImin[1][14][23]=   0.844116;m_EtaROImax[1][14][23]=   0.903324;m_PhiROImin[1][14][23]=	2.83927;m_PhiROImax[1][14][23]=	 2.92194;
     m_EtaROImin[1][14][24]=	    0;m_EtaROImax[1][14][24]=	     0;m_PhiROImin[1][14][24]=	      0;m_PhiROImax[1][14][24]=	       0;
     m_EtaROImin[1][14][25]=	    0;m_EtaROImax[1][14][25]=	     0;m_PhiROImin[1][14][25]=	      0;m_PhiROImax[1][14][25]=	       0;
     m_EtaROImin[1][14][26]=	    0;m_EtaROImax[1][14][26]=	     0;m_PhiROImin[1][14][26]=	      0;m_PhiROImax[1][14][26]=	       0;
     m_EtaROImin[1][14][27]=	    0;m_EtaROImax[1][14][27]=	     0;m_PhiROImin[1][14][27]=	      0;m_PhiROImax[1][14][27]=	       0;
     m_EtaROImin[1][14][28]=	    0;m_EtaROImax[1][14][28]=	     0;m_PhiROImin[1][14][28]=	      0;m_PhiROImax[1][14][28]=	       0;
     m_EtaROImin[1][14][29]=	    0;m_EtaROImax[1][14][29]=	     0;m_PhiROImin[1][14][29]=	      0;m_PhiROImax[1][14][29]=	       0;
     m_EtaROImin[1][14][30]=	    0;m_EtaROImax[1][14][30]=	     0;m_PhiROImin[1][14][30]=	      0;m_PhiROImax[1][14][30]=	       0;
     m_EtaROImin[1][14][31]=	    0;m_EtaROImax[1][14][31]=	     0;m_PhiROImin[1][14][31]=	      0;m_PhiROImax[1][14][31]=	       0;
     m_EtaROImin[1][15][ 0]=   0.130922;m_EtaROImax[1][15][ 0]=   0.238612;m_PhiROImin[1][15][ 0]=	3.02838;m_PhiROImax[1][15][ 0]=	 3.13862;
     m_EtaROImin[1][15][ 1]=	    0;m_EtaROImax[1][15][ 1]=	     0;m_PhiROImin[1][15][ 1]=	      0;m_PhiROImax[1][15][ 1]=	       0;
     m_EtaROImin[1][15][ 2]=   0.130922;m_EtaROImax[1][15][ 2]=   0.238612;m_PhiROImin[1][15][ 2]=	2.91742;m_PhiROImax[1][15][ 2]=	 3.02486;
     m_EtaROImin[1][15][ 3]=	    0;m_EtaROImax[1][15][ 3]=	     0;m_PhiROImin[1][15][ 3]=	      0;m_PhiROImax[1][15][ 3]=	       0;
     m_EtaROImin[1][15][ 4]=   0.246207;m_EtaROImax[1][15][ 4]=    0.35049;m_PhiROImin[1][15][ 4]=	3.02898;m_PhiROImax[1][15][ 4]=	 3.13864;
     m_EtaROImin[1][15][ 5]=   0.350831;m_EtaROImax[1][15][ 5]=   0.452099;m_PhiROImin[1][15][ 5]=	3.02898;m_PhiROImax[1][15][ 5]=	 3.13864;
     m_EtaROImin[1][15][ 6]=   0.246207;m_EtaROImax[1][15][ 6]=    0.35049;m_PhiROImin[1][15][ 6]=	2.91858;m_PhiROImax[1][15][ 6]=	 3.02547;
     m_EtaROImin[1][15][ 7]=   0.350831;m_EtaROImax[1][15][ 7]=   0.452099;m_PhiROImin[1][15][ 7]=	2.91858;m_PhiROImax[1][15][ 7]=	 3.02547;
     m_EtaROImin[1][15][ 8]=   0.458091;m_EtaROImax[1][15][ 8]=   0.554444;m_PhiROImin[1][15][ 8]=	3.02898;m_PhiROImax[1][15][ 8]=	 3.13864;
     m_EtaROImin[1][15][ 9]=   0.555789;m_EtaROImax[1][15][ 9]=   0.648101;m_PhiROImin[1][15][ 9]=	3.02898;m_PhiROImax[1][15][ 9]=	 3.13864;
     m_EtaROImin[1][15][10]=   0.458091;m_EtaROImax[1][15][10]=   0.554444;m_PhiROImin[1][15][10]=	2.91858;m_PhiROImax[1][15][10]=	 3.02547;
     m_EtaROImin[1][15][11]=   0.555789;m_EtaROImax[1][15][11]=   0.648101;m_PhiROImin[1][15][11]=	2.91858;m_PhiROImax[1][15][11]=	 3.02547;
     m_EtaROImin[1][15][12]=   0.655857;m_EtaROImax[1][15][12]=   0.703298;m_PhiROImin[1][15][12]=	3.02838;m_PhiROImax[1][15][12]=	 3.13862;
     m_EtaROImin[1][15][13]=   0.706413;m_EtaROImax[1][15][13]=   0.776449;m_PhiROImin[1][15][13]=	3.02838;m_PhiROImax[1][15][13]=	 3.13862;
     m_EtaROImin[1][15][14]=   0.655857;m_EtaROImax[1][15][14]=   0.703298;m_PhiROImin[1][15][14]=	2.91742;m_PhiROImax[1][15][14]=	 3.02486;
     m_EtaROImin[1][15][15]=   0.706413;m_EtaROImax[1][15][15]=   0.776449;m_PhiROImin[1][15][15]=	2.91742;m_PhiROImax[1][15][15]=	 3.02486;
     m_EtaROImin[1][15][16]=   0.783867;m_EtaROImax[1][15][16]=    0.82768;m_PhiROImin[1][15][16]=	3.02838;m_PhiROImax[1][15][16]=	 3.13862;
     m_EtaROImin[1][15][17]=   0.830556;m_EtaROImax[1][15][17]=   0.895163;m_PhiROImin[1][15][17]=	3.02838;m_PhiROImax[1][15][17]=	 3.13862;
     m_EtaROImin[1][15][18]=   0.783867;m_EtaROImax[1][15][18]=    0.82768;m_PhiROImin[1][15][18]=	2.91742;m_PhiROImax[1][15][18]=	 3.02486;
     m_EtaROImin[1][15][19]=   0.830556;m_EtaROImax[1][15][19]=   0.895163;m_PhiROImin[1][15][19]=	2.91742;m_PhiROImax[1][15][19]=	 3.02486;
     m_EtaROImin[1][15][20]=   0.898201;m_EtaROImax[1][15][20]=   0.961344;m_PhiROImin[1][15][20]=	3.02898;m_PhiROImax[1][15][20]=	 3.13864;
     m_EtaROImin[1][15][21]=   0.964674;m_EtaROImax[1][15][21]=    1.02514;m_PhiROImin[1][15][21]=	3.02898;m_PhiROImax[1][15][21]=	 3.13864;
     m_EtaROImin[1][15][22]=   0.898201;m_EtaROImax[1][15][22]=   0.961344;m_PhiROImin[1][15][22]=	2.91858;m_PhiROImax[1][15][22]=	 3.02547;
     m_EtaROImin[1][15][23]=   0.964674;m_EtaROImax[1][15][23]=    1.02514;m_PhiROImin[1][15][23]=	2.91858;m_PhiROImax[1][15][23]=	 3.02547;
     m_EtaROImin[1][15][24]=    1.03003;m_EtaROImax[1][15][24]=    1.06547;m_PhiROImin[1][15][24]=	3.02816;m_PhiROImax[1][15][24]=	 3.13852;
     m_EtaROImin[1][15][25]=	    0;m_EtaROImax[1][15][25]=	     0;m_PhiROImin[1][15][25]=	      0;m_PhiROImax[1][15][25]=	       0;
     m_EtaROImin[1][15][26]=    1.03003;m_EtaROImax[1][15][26]=    1.06547;m_PhiROImin[1][15][26]=	2.94445;m_PhiROImax[1][15][26]=	 3.02463;
     m_EtaROImin[1][15][27]=	    0;m_EtaROImax[1][15][27]=	     0;m_PhiROImin[1][15][27]=	      0;m_PhiROImax[1][15][27]=	       0;
     m_EtaROImin[1][15][28]=	    0;m_EtaROImax[1][15][28]=	     0;m_PhiROImin[1][15][28]=	      0;m_PhiROImax[1][15][28]=	       0;
     m_EtaROImin[1][15][29]=	    0;m_EtaROImax[1][15][29]=	     0;m_PhiROImin[1][15][29]=	      0;m_PhiROImax[1][15][29]=	       0;
     m_EtaROImin[1][15][30]=	    0;m_EtaROImax[1][15][30]=	     0;m_PhiROImin[1][15][30]=	      0;m_PhiROImax[1][15][30]=	       0;
     m_EtaROImin[1][15][31]=	    0;m_EtaROImax[1][15][31]=	     0;m_PhiROImin[1][15][31]=	      0;m_PhiROImax[1][15][31]=	       0;
     m_EtaROImin[1][16][ 0]=	    0;m_EtaROImax[1][16][ 0]=	     0;m_PhiROImin[1][16][ 0]=	      0;m_PhiROImax[1][16][ 0]=	       0;
     m_EtaROImin[1][16][ 1]=   0.130922;m_EtaROImax[1][16][ 1]=   0.238612;m_PhiROImin[1][16][ 1]=   -3.13862;m_PhiROImax[1][16][ 1]=	-3.02838;
     m_EtaROImin[1][16][ 2]=	    0;m_EtaROImax[1][16][ 2]=	     0;m_PhiROImin[1][16][ 2]=	      0;m_PhiROImax[1][16][ 2]=	       0;
     m_EtaROImin[1][16][ 3]=   0.130922;m_EtaROImax[1][16][ 3]=   0.238612;m_PhiROImin[1][16][ 3]=   -3.02486;m_PhiROImax[1][16][ 3]=	-2.91742;
     m_EtaROImin[1][16][ 4]=   0.350831;m_EtaROImax[1][16][ 4]=   0.452099;m_PhiROImin[1][16][ 4]=   -3.13864;m_PhiROImax[1][16][ 4]=	-3.02898;
     m_EtaROImin[1][16][ 5]=   0.246207;m_EtaROImax[1][16][ 5]=    0.35049;m_PhiROImin[1][16][ 5]=   -3.13864;m_PhiROImax[1][16][ 5]=	-3.02898;
     m_EtaROImin[1][16][ 6]=   0.350831;m_EtaROImax[1][16][ 6]=   0.452099;m_PhiROImin[1][16][ 6]=   -3.02547;m_PhiROImax[1][16][ 6]=	-2.91858;
     m_EtaROImin[1][16][ 7]=   0.246207;m_EtaROImax[1][16][ 7]=    0.35049;m_PhiROImin[1][16][ 7]=   -3.02547;m_PhiROImax[1][16][ 7]=	-2.91858;
     m_EtaROImin[1][16][ 8]=   0.555789;m_EtaROImax[1][16][ 8]=   0.648101;m_PhiROImin[1][16][ 8]=   -3.13864;m_PhiROImax[1][16][ 8]=	-3.02898;
     m_EtaROImin[1][16][ 9]=   0.458091;m_EtaROImax[1][16][ 9]=   0.554444;m_PhiROImin[1][16][ 9]=   -3.13864;m_PhiROImax[1][16][ 9]=	-3.02898;
     m_EtaROImin[1][16][10]=   0.555789;m_EtaROImax[1][16][10]=   0.648101;m_PhiROImin[1][16][10]=   -3.02547;m_PhiROImax[1][16][10]=	-2.91858;
     m_EtaROImin[1][16][11]=   0.458091;m_EtaROImax[1][16][11]=   0.554444;m_PhiROImin[1][16][11]=   -3.02547;m_PhiROImax[1][16][11]=	-2.91858;
     m_EtaROImin[1][16][12]=   0.731124;m_EtaROImax[1][16][12]=   0.776449;m_PhiROImin[1][16][12]=   -3.13862;m_PhiROImax[1][16][12]=	-3.02838;
     m_EtaROImin[1][16][13]=   0.655857;m_EtaROImax[1][16][13]=   0.728056;m_PhiROImin[1][16][13]=   -3.13862;m_PhiROImax[1][16][13]=	-3.02838;
     m_EtaROImin[1][16][14]=   0.731124;m_EtaROImax[1][16][14]=   0.776449;m_PhiROImin[1][16][14]=   -3.02486;m_PhiROImax[1][16][14]=	-2.91742;
     m_EtaROImin[1][16][15]=   0.655857;m_EtaROImax[1][16][15]=   0.728056;m_PhiROImin[1][16][15]=   -3.02486;m_PhiROImax[1][16][15]=	-2.91742;
     m_EtaROImin[1][16][16]=   0.853359;m_EtaROImax[1][16][16]=   0.895163;m_PhiROImin[1][16][16]=   -3.13862;m_PhiROImax[1][16][16]=	-3.02838;
     m_EtaROImin[1][16][17]=   0.783867;m_EtaROImax[1][16][17]=   0.850528;m_PhiROImin[1][16][17]=   -3.13862;m_PhiROImax[1][16][17]=	-3.02838;
     m_EtaROImin[1][16][18]=   0.853359;m_EtaROImax[1][16][18]=   0.895163;m_PhiROImin[1][16][18]=   -3.02486;m_PhiROImax[1][16][18]=	-2.91742;
     m_EtaROImin[1][16][19]=   0.783867;m_EtaROImax[1][16][19]=   0.850528;m_PhiROImin[1][16][19]=   -3.02486;m_PhiROImax[1][16][19]=	-2.91742;
     m_EtaROImin[1][16][20]=   0.964674;m_EtaROImax[1][16][20]=    1.02514;m_PhiROImin[1][16][20]=   -3.13864;m_PhiROImax[1][16][20]=	-3.02898;
     m_EtaROImin[1][16][21]=   0.898201;m_EtaROImax[1][16][21]=   0.961344;m_PhiROImin[1][16][21]=   -3.13864;m_PhiROImax[1][16][21]=	-3.02898;
     m_EtaROImin[1][16][22]=   0.964674;m_EtaROImax[1][16][22]=    1.02514;m_PhiROImin[1][16][22]=   -3.02547;m_PhiROImax[1][16][22]=	-2.91858;
     m_EtaROImin[1][16][23]=   0.898201;m_EtaROImax[1][16][23]=   0.961344;m_PhiROImin[1][16][23]=   -3.02547;m_PhiROImax[1][16][23]=	-2.91858;
     m_EtaROImin[1][16][24]=	    0;m_EtaROImax[1][16][24]=	     0;m_PhiROImin[1][16][24]=	      0;m_PhiROImax[1][16][24]=	       0;
     m_EtaROImin[1][16][25]=    1.03003;m_EtaROImax[1][16][25]=    1.06547;m_PhiROImin[1][16][25]=   -3.13852;m_PhiROImax[1][16][25]=	-3.02816;
     m_EtaROImin[1][16][26]=	    0;m_EtaROImax[1][16][26]=	     0;m_PhiROImin[1][16][26]=	      0;m_PhiROImax[1][16][26]=	       0;
     m_EtaROImin[1][16][27]=    1.03003;m_EtaROImax[1][16][27]=    1.06547;m_PhiROImin[1][16][27]=   -3.02463;m_PhiROImax[1][16][27]=	-2.94445;
     m_EtaROImin[1][16][28]=	    0;m_EtaROImax[1][16][28]=	     0;m_PhiROImin[1][16][28]=	      0;m_PhiROImax[1][16][28]=	       0;
     m_EtaROImin[1][16][29]=	    0;m_EtaROImax[1][16][29]=	     0;m_PhiROImin[1][16][29]=	      0;m_PhiROImax[1][16][29]=	       0;
     m_EtaROImin[1][16][30]=	    0;m_EtaROImax[1][16][30]=	     0;m_PhiROImin[1][16][30]=	      0;m_PhiROImax[1][16][30]=	       0;
     m_EtaROImin[1][16][31]=	    0;m_EtaROImax[1][16][31]=	     0;m_PhiROImin[1][16][31]=	      0;m_PhiROImax[1][16][31]=	       0;
     m_EtaROImin[1][17][ 0]=  0.0208251;m_EtaROImax[1][17][ 0]=   0.118734;m_PhiROImin[1][17][ 0]=   -2.83564;m_PhiROImax[1][17][ 0]=	-2.75168;
     m_EtaROImin[1][17][ 1]=   0.116816;m_EtaROImax[1][17][ 1]=   0.213185;m_PhiROImin[1][17][ 1]=   -2.83564;m_PhiROImax[1][17][ 1]=	-2.75168;
     m_EtaROImin[1][17][ 2]=  0.0208251;m_EtaROImax[1][17][ 2]=   0.118734;m_PhiROImin[1][17][ 2]=   -2.92194;m_PhiROImax[1][17][ 2]=	-2.83927;
     m_EtaROImin[1][17][ 3]=   0.116816;m_EtaROImax[1][17][ 3]=   0.213185;m_PhiROImin[1][17][ 3]=   -2.92194;m_PhiROImax[1][17][ 3]=	-2.83927;
     m_EtaROImin[1][17][ 4]=   0.222449;m_EtaROImax[1][17][ 4]=   0.302928;m_PhiROImin[1][17][ 4]=   -2.83564;m_PhiROImax[1][17][ 4]=	-2.75168;
     m_EtaROImin[1][17][ 5]=    0.30075;m_EtaROImax[1][17][ 5]=   0.416721;m_PhiROImin[1][17][ 5]=   -2.83564;m_PhiROImax[1][17][ 5]=	-2.75168;
     m_EtaROImin[1][17][ 6]=   0.222449;m_EtaROImax[1][17][ 6]=   0.302928;m_PhiROImin[1][17][ 6]=   -2.92194;m_PhiROImax[1][17][ 6]=	-2.83927;
     m_EtaROImin[1][17][ 7]=    0.30075;m_EtaROImax[1][17][ 7]=   0.416721;m_PhiROImin[1][17][ 7]=   -2.92194;m_PhiROImax[1][17][ 7]=	-2.83927;
     m_EtaROImin[1][17][ 8]=    0.42967;m_EtaROImax[1][17][ 8]=   0.504617;m_PhiROImin[1][17][ 8]=   -2.83564;m_PhiROImax[1][17][ 8]=	-2.75168;
     m_EtaROImin[1][17][ 9]=   0.501681;m_EtaROImax[1][17][ 9]=   0.573871;m_PhiROImin[1][17][ 9]=   -2.83564;m_PhiROImax[1][17][ 9]=	-2.75168;
     m_EtaROImin[1][17][10]=    0.42967;m_EtaROImax[1][17][10]=   0.504617;m_PhiROImin[1][17][10]=   -2.92194;m_PhiROImax[1][17][10]=	-2.83927;
     m_EtaROImin[1][17][11]=   0.501681;m_EtaROImax[1][17][11]=   0.573871;m_PhiROImin[1][17][11]=   -2.92194;m_PhiROImax[1][17][11]=	-2.83927;
     m_EtaROImin[1][17][12]=   0.583785;m_EtaROImax[1][17][12]=   0.653329;m_PhiROImin[1][17][12]=   -2.83564;m_PhiROImax[1][17][12]=	-2.75168;
     m_EtaROImin[1][17][13]=   0.649934;m_EtaROImax[1][17][13]=   0.741516;m_PhiROImin[1][17][13]=   -2.83564;m_PhiROImax[1][17][13]=	-2.75168;
     m_EtaROImin[1][17][14]=   0.583785;m_EtaROImax[1][17][14]=   0.653329;m_PhiROImin[1][17][14]=   -2.92194;m_PhiROImax[1][17][14]=	-2.83927;
     m_EtaROImin[1][17][15]=   0.649934;m_EtaROImax[1][17][15]=   0.741516;m_PhiROImin[1][17][15]=   -2.92194;m_PhiROImax[1][17][15]=	-2.83927;
     m_EtaROImin[1][17][16]=   0.756521;m_EtaROImax[1][17][16]=   0.837822;m_PhiROImin[1][17][16]=   -2.83564;m_PhiROImax[1][17][16]=	-2.75168;
     m_EtaROImin[1][17][17]=	    0;m_EtaROImax[1][17][17]=	     0;m_PhiROImin[1][17][17]=	      0;m_PhiROImax[1][17][17]=	       0;
     m_EtaROImin[1][17][18]=   0.756521;m_EtaROImax[1][17][18]=   0.837822;m_PhiROImin[1][17][18]=   -2.92194;m_PhiROImax[1][17][18]=	-2.83927;
     m_EtaROImin[1][17][19]=	    0;m_EtaROImax[1][17][19]=	     0;m_PhiROImin[1][17][19]=	      0;m_PhiROImax[1][17][19]=	       0;
     m_EtaROImin[1][17][20]=   0.844116;m_EtaROImax[1][17][20]=   0.903324;m_PhiROImin[1][17][20]=   -2.83564;m_PhiROImax[1][17][20]=	-2.75168;
     m_EtaROImin[1][17][21]=   0.899344;m_EtaROImax[1][17][21]=   0.956037;m_PhiROImin[1][17][21]=   -2.83564;m_PhiROImax[1][17][21]=	-2.75168;
     m_EtaROImin[1][17][22]=   0.844116;m_EtaROImax[1][17][22]=   0.903324;m_PhiROImin[1][17][22]=   -2.92194;m_PhiROImax[1][17][22]=	-2.83927;
     m_EtaROImin[1][17][23]=   0.899344;m_EtaROImax[1][17][23]=   0.956037;m_PhiROImin[1][17][23]=   -2.92194;m_PhiROImax[1][17][23]=	-2.83927;
     m_EtaROImin[1][17][24]=	    0;m_EtaROImax[1][17][24]=	     0;m_PhiROImin[1][17][24]=	      0;m_PhiROImax[1][17][24]=	       0;
     m_EtaROImin[1][17][25]=	    0;m_EtaROImax[1][17][25]=	     0;m_PhiROImin[1][17][25]=	      0;m_PhiROImax[1][17][25]=	       0;
     m_EtaROImin[1][17][26]=	    0;m_EtaROImax[1][17][26]=	     0;m_PhiROImin[1][17][26]=	      0;m_PhiROImax[1][17][26]=	       0;
     m_EtaROImin[1][17][27]=	    0;m_EtaROImax[1][17][27]=	     0;m_PhiROImin[1][17][27]=	      0;m_PhiROImax[1][17][27]=	       0;
     m_EtaROImin[1][17][28]=	    0;m_EtaROImax[1][17][28]=	     0;m_PhiROImin[1][17][28]=	      0;m_PhiROImax[1][17][28]=	       0;
     m_EtaROImin[1][17][29]=	    0;m_EtaROImax[1][17][29]=	     0;m_PhiROImin[1][17][29]=	      0;m_PhiROImax[1][17][29]=	       0;
     m_EtaROImin[1][17][30]=	    0;m_EtaROImax[1][17][30]=	     0;m_PhiROImin[1][17][30]=	      0;m_PhiROImax[1][17][30]=	       0;
     m_EtaROImin[1][17][31]=	    0;m_EtaROImax[1][17][31]=	     0;m_PhiROImin[1][17][31]=	      0;m_PhiROImax[1][17][31]=	       0;
     m_EtaROImin[1][18][ 0]=   0.116816;m_EtaROImax[1][18][ 0]=   0.213185;m_PhiROImin[1][18][ 0]=	-2.7461;m_PhiROImax[1][18][ 0]=	-2.66215;
     m_EtaROImin[1][18][ 1]=  0.0208251;m_EtaROImax[1][18][ 1]=   0.118734;m_PhiROImin[1][18][ 1]=	-2.7461;m_PhiROImax[1][18][ 1]=	-2.66215;
     m_EtaROImin[1][18][ 2]=   0.116816;m_EtaROImax[1][18][ 2]=   0.213185;m_PhiROImin[1][18][ 2]=   -2.65852;m_PhiROImax[1][18][ 2]=	-2.57584;
     m_EtaROImin[1][18][ 3]=  0.0208251;m_EtaROImax[1][18][ 3]=   0.118734;m_PhiROImin[1][18][ 3]=   -2.65852;m_PhiROImax[1][18][ 3]=	-2.57584;
     m_EtaROImin[1][18][ 4]=    0.30075;m_EtaROImax[1][18][ 4]=   0.416721;m_PhiROImin[1][18][ 4]=	-2.7461;m_PhiROImax[1][18][ 4]=	-2.66215;
     m_EtaROImin[1][18][ 5]=   0.222449;m_EtaROImax[1][18][ 5]=   0.302928;m_PhiROImin[1][18][ 5]=	-2.7461;m_PhiROImax[1][18][ 5]=	-2.66215;
     m_EtaROImin[1][18][ 6]=    0.30075;m_EtaROImax[1][18][ 6]=   0.416721;m_PhiROImin[1][18][ 6]=   -2.65852;m_PhiROImax[1][18][ 6]=	-2.57584;
     m_EtaROImin[1][18][ 7]=   0.222449;m_EtaROImax[1][18][ 7]=   0.302928;m_PhiROImin[1][18][ 7]=   -2.65852;m_PhiROImax[1][18][ 7]=	-2.57584;
     m_EtaROImin[1][18][ 8]=   0.501681;m_EtaROImax[1][18][ 8]=   0.573871;m_PhiROImin[1][18][ 8]=	-2.7461;m_PhiROImax[1][18][ 8]=	-2.66215;
     m_EtaROImin[1][18][ 9]=    0.42967;m_EtaROImax[1][18][ 9]=   0.504617;m_PhiROImin[1][18][ 9]=	-2.7461;m_PhiROImax[1][18][ 9]=	-2.66215;
     m_EtaROImin[1][18][10]=   0.501681;m_EtaROImax[1][18][10]=   0.573871;m_PhiROImin[1][18][10]=   -2.65852;m_PhiROImax[1][18][10]=	-2.57584;
     m_EtaROImin[1][18][11]=    0.42967;m_EtaROImax[1][18][11]=   0.504617;m_PhiROImin[1][18][11]=   -2.65852;m_PhiROImax[1][18][11]=	-2.57584;
     m_EtaROImin[1][18][12]=   0.649934;m_EtaROImax[1][18][12]=   0.741516;m_PhiROImin[1][18][12]=	-2.7461;m_PhiROImax[1][18][12]=	-2.66215;
     m_EtaROImin[1][18][13]=   0.583785;m_EtaROImax[1][18][13]=   0.653329;m_PhiROImin[1][18][13]=	-2.7461;m_PhiROImax[1][18][13]=	-2.66215;
     m_EtaROImin[1][18][14]=   0.649934;m_EtaROImax[1][18][14]=   0.741516;m_PhiROImin[1][18][14]=   -2.65852;m_PhiROImax[1][18][14]=	-2.57584;
     m_EtaROImin[1][18][15]=   0.583785;m_EtaROImax[1][18][15]=   0.653329;m_PhiROImin[1][18][15]=   -2.65852;m_PhiROImax[1][18][15]=	-2.57584;
     m_EtaROImin[1][18][16]=	    0;m_EtaROImax[1][18][16]=	     0;m_PhiROImin[1][18][16]=	      0;m_PhiROImax[1][18][16]=	       0;
     m_EtaROImin[1][18][17]=   0.756521;m_EtaROImax[1][18][17]=   0.837822;m_PhiROImin[1][18][17]=	-2.7461;m_PhiROImax[1][18][17]=	-2.66215;
     m_EtaROImin[1][18][18]=	    0;m_EtaROImax[1][18][18]=	     0;m_PhiROImin[1][18][18]=	      0;m_PhiROImax[1][18][18]=	       0;
     m_EtaROImin[1][18][19]=   0.756521;m_EtaROImax[1][18][19]=   0.837822;m_PhiROImin[1][18][19]=   -2.65852;m_PhiROImax[1][18][19]=	-2.57584;
     m_EtaROImin[1][18][20]=   0.899344;m_EtaROImax[1][18][20]=   0.956037;m_PhiROImin[1][18][20]=	-2.7461;m_PhiROImax[1][18][20]=	-2.66215;
     m_EtaROImin[1][18][21]=   0.844116;m_EtaROImax[1][18][21]=   0.903324;m_PhiROImin[1][18][21]=	-2.7461;m_PhiROImax[1][18][21]=	-2.66215;
     m_EtaROImin[1][18][22]=   0.899344;m_EtaROImax[1][18][22]=   0.956037;m_PhiROImin[1][18][22]=   -2.65852;m_PhiROImax[1][18][22]=	-2.57584;
     m_EtaROImin[1][18][23]=   0.844116;m_EtaROImax[1][18][23]=   0.903324;m_PhiROImin[1][18][23]=   -2.65852;m_PhiROImax[1][18][23]=	-2.57584;
     m_EtaROImin[1][18][24]=	    0;m_EtaROImax[1][18][24]=	     0;m_PhiROImin[1][18][24]=	      0;m_PhiROImax[1][18][24]=	       0;
     m_EtaROImin[1][18][25]=	    0;m_EtaROImax[1][18][25]=	     0;m_PhiROImin[1][18][25]=	      0;m_PhiROImax[1][18][25]=	       0;
     m_EtaROImin[1][18][26]=	    0;m_EtaROImax[1][18][26]=	     0;m_PhiROImin[1][18][26]=	      0;m_PhiROImax[1][18][26]=	       0;
     m_EtaROImin[1][18][27]=	    0;m_EtaROImax[1][18][27]=	     0;m_PhiROImin[1][18][27]=	      0;m_PhiROImax[1][18][27]=	       0;
     m_EtaROImin[1][18][28]=	    0;m_EtaROImax[1][18][28]=	     0;m_PhiROImin[1][18][28]=	      0;m_PhiROImax[1][18][28]=	       0;
     m_EtaROImin[1][18][29]=	    0;m_EtaROImax[1][18][29]=	     0;m_PhiROImin[1][18][29]=	      0;m_PhiROImax[1][18][29]=	       0;
     m_EtaROImin[1][18][30]=	    0;m_EtaROImax[1][18][30]=	     0;m_PhiROImin[1][18][30]=	      0;m_PhiROImax[1][18][30]=	       0;
     m_EtaROImin[1][18][31]=	    0;m_EtaROImax[1][18][31]=	     0;m_PhiROImin[1][18][31]=	      0;m_PhiROImax[1][18][31]=	       0;
     m_EtaROImin[1][19][ 0]=  0.0874905;m_EtaROImax[1][19][ 0]=   0.145912;m_PhiROImin[1][19][ 0]=   -2.46941;m_PhiROImax[1][19][ 0]=	-2.35916;
     m_EtaROImin[1][19][ 1]=   0.149792;m_EtaROImax[1][19][ 1]=   0.238294;m_PhiROImin[1][19][ 1]=   -2.46941;m_PhiROImax[1][19][ 1]=	-2.35916;
     m_EtaROImin[1][19][ 2]=  0.0874905;m_EtaROImax[1][19][ 2]=   0.145912;m_PhiROImin[1][19][ 2]=   -2.58036;m_PhiROImax[1][19][ 2]=	-2.47293;
     m_EtaROImin[1][19][ 3]=   0.149792;m_EtaROImax[1][19][ 3]=   0.238294;m_PhiROImin[1][19][ 3]=   -2.58036;m_PhiROImax[1][19][ 3]=	-2.47293;
     m_EtaROImin[1][19][ 4]=   0.246207;m_EtaROImax[1][19][ 4]=    0.35049;m_PhiROImin[1][19][ 4]=   -2.46881;m_PhiROImax[1][19][ 4]=	-2.35915;
     m_EtaROImin[1][19][ 5]=   0.350831;m_EtaROImax[1][19][ 5]=   0.452099;m_PhiROImin[1][19][ 5]=   -2.46881;m_PhiROImax[1][19][ 5]=	-2.35915;
     m_EtaROImin[1][19][ 6]=   0.246207;m_EtaROImax[1][19][ 6]=    0.35049;m_PhiROImin[1][19][ 6]=   -2.57921;m_PhiROImax[1][19][ 6]=	-2.47231;
     m_EtaROImin[1][19][ 7]=   0.350831;m_EtaROImax[1][19][ 7]=   0.452099;m_PhiROImin[1][19][ 7]=   -2.57921;m_PhiROImax[1][19][ 7]=	-2.47231;
     m_EtaROImin[1][19][ 8]=   0.458091;m_EtaROImax[1][19][ 8]=   0.554444;m_PhiROImin[1][19][ 8]=   -2.46881;m_PhiROImax[1][19][ 8]=	-2.35915;
     m_EtaROImin[1][19][ 9]=   0.555789;m_EtaROImax[1][19][ 9]=   0.648101;m_PhiROImin[1][19][ 9]=   -2.46881;m_PhiROImax[1][19][ 9]=	-2.35915;
     m_EtaROImin[1][19][10]=   0.458091;m_EtaROImax[1][19][10]=   0.554444;m_PhiROImin[1][19][10]=   -2.57921;m_PhiROImax[1][19][10]=	-2.47231;
     m_EtaROImin[1][19][11]=   0.555789;m_EtaROImax[1][19][11]=   0.648101;m_PhiROImin[1][19][11]=   -2.57921;m_PhiROImax[1][19][11]=	-2.47231;
     m_EtaROImin[1][19][12]=   0.655857;m_EtaROImax[1][19][12]=   0.703298;m_PhiROImin[1][19][12]=   -2.46941;m_PhiROImax[1][19][12]=	-2.35916;
     m_EtaROImin[1][19][13]=   0.706413;m_EtaROImax[1][19][13]=   0.776449;m_PhiROImin[1][19][13]=   -2.46941;m_PhiROImax[1][19][13]=	-2.35916;
     m_EtaROImin[1][19][14]=   0.655857;m_EtaROImax[1][19][14]=   0.703298;m_PhiROImin[1][19][14]=   -2.58036;m_PhiROImax[1][19][14]=	-2.47293;
     m_EtaROImin[1][19][15]=   0.706413;m_EtaROImax[1][19][15]=   0.776449;m_PhiROImin[1][19][15]=   -2.58036;m_PhiROImax[1][19][15]=	-2.47293;
     m_EtaROImin[1][19][16]=   0.783867;m_EtaROImax[1][19][16]=    0.82768;m_PhiROImin[1][19][16]=   -2.46941;m_PhiROImax[1][19][16]=	-2.35916;
     m_EtaROImin[1][19][17]=   0.830556;m_EtaROImax[1][19][17]=   0.895163;m_PhiROImin[1][19][17]=   -2.46941;m_PhiROImax[1][19][17]=	-2.35916;
     m_EtaROImin[1][19][18]=   0.783867;m_EtaROImax[1][19][18]=    0.82768;m_PhiROImin[1][19][18]=   -2.58036;m_PhiROImax[1][19][18]=	-2.47293;
     m_EtaROImin[1][19][19]=   0.830556;m_EtaROImax[1][19][19]=   0.895163;m_PhiROImin[1][19][19]=   -2.58036;m_PhiROImax[1][19][19]=	-2.47293;
     m_EtaROImin[1][19][20]=   0.898201;m_EtaROImax[1][19][20]=   0.961344;m_PhiROImin[1][19][20]=   -2.46881;m_PhiROImax[1][19][20]=	-2.35915;
     m_EtaROImin[1][19][21]=   0.964674;m_EtaROImax[1][19][21]=    1.02514;m_PhiROImin[1][19][21]=   -2.46881;m_PhiROImax[1][19][21]=	-2.35915;
     m_EtaROImin[1][19][22]=   0.898201;m_EtaROImax[1][19][22]=   0.961344;m_PhiROImin[1][19][22]=   -2.57921;m_PhiROImax[1][19][22]=	-2.47231;
     m_EtaROImin[1][19][23]=   0.964674;m_EtaROImax[1][19][23]=    1.02514;m_PhiROImin[1][19][23]=   -2.57921;m_PhiROImax[1][19][23]=	-2.47231;
     m_EtaROImin[1][19][24]=    1.03003;m_EtaROImax[1][19][24]=    1.06547;m_PhiROImin[1][19][24]=   -2.46962;m_PhiROImax[1][19][24]=	-2.35926;
     m_EtaROImin[1][19][25]=	    0;m_EtaROImax[1][19][25]=	     0;m_PhiROImin[1][19][25]=	      0;m_PhiROImax[1][19][25]=	       0;
     m_EtaROImin[1][19][26]=    1.03003;m_EtaROImax[1][19][26]=    1.06547;m_PhiROImin[1][19][26]=   -2.55334;m_PhiROImax[1][19][26]=	-2.47315;
     m_EtaROImin[1][19][27]=	    0;m_EtaROImax[1][19][27]=	     0;m_PhiROImin[1][19][27]=	      0;m_PhiROImax[1][19][27]=	       0;
     m_EtaROImin[1][19][28]=	    0;m_EtaROImax[1][19][28]=	     0;m_PhiROImin[1][19][28]=	      0;m_PhiROImax[1][19][28]=	       0;
     m_EtaROImin[1][19][29]=	    0;m_EtaROImax[1][19][29]=	     0;m_PhiROImin[1][19][29]=	      0;m_PhiROImax[1][19][29]=	       0;
     m_EtaROImin[1][19][30]=	    0;m_EtaROImax[1][19][30]=	     0;m_PhiROImin[1][19][30]=	      0;m_PhiROImax[1][19][30]=	       0;
     m_EtaROImin[1][19][31]=	    0;m_EtaROImax[1][19][31]=	     0;m_PhiROImin[1][19][31]=	      0;m_PhiROImax[1][19][31]=	       0;
     m_EtaROImin[1][20][ 0]=   0.180742;m_EtaROImax[1][20][ 0]=   0.238294;m_PhiROImin[1][20][ 0]=   -2.35323;m_PhiROImax[1][20][ 0]=	-2.24298;
     m_EtaROImin[1][20][ 1]=  0.0874905;m_EtaROImax[1][20][ 1]=   0.176882;m_PhiROImin[1][20][ 1]=   -2.35323;m_PhiROImax[1][20][ 1]=	-2.24298;
     m_EtaROImin[1][20][ 2]=   0.180742;m_EtaROImax[1][20][ 2]=   0.238294;m_PhiROImin[1][20][ 2]=   -2.23946;m_PhiROImax[1][20][ 2]=	-2.13203;
     m_EtaROImin[1][20][ 3]=  0.0874905;m_EtaROImax[1][20][ 3]=   0.176882;m_PhiROImin[1][20][ 3]=   -2.23946;m_PhiROImax[1][20][ 3]=	-2.13203;
     m_EtaROImin[1][20][ 4]=   0.350831;m_EtaROImax[1][20][ 4]=   0.452099;m_PhiROImin[1][20][ 4]=   -2.35324;m_PhiROImax[1][20][ 4]=	-2.24358;
     m_EtaROImin[1][20][ 5]=   0.246207;m_EtaROImax[1][20][ 5]=    0.35049;m_PhiROImin[1][20][ 5]=   -2.35324;m_PhiROImax[1][20][ 5]=	-2.24358;
     m_EtaROImin[1][20][ 6]=   0.350831;m_EtaROImax[1][20][ 6]=   0.452099;m_PhiROImin[1][20][ 6]=   -2.24008;m_PhiROImax[1][20][ 6]=	-2.13318;
     m_EtaROImin[1][20][ 7]=   0.246207;m_EtaROImax[1][20][ 7]=    0.35049;m_PhiROImin[1][20][ 7]=   -2.24008;m_PhiROImax[1][20][ 7]=	-2.13318;
     m_EtaROImin[1][20][ 8]=   0.555789;m_EtaROImax[1][20][ 8]=   0.648101;m_PhiROImin[1][20][ 8]=   -2.35324;m_PhiROImax[1][20][ 8]=	-2.24358;
     m_EtaROImin[1][20][ 9]=   0.458091;m_EtaROImax[1][20][ 9]=   0.554444;m_PhiROImin[1][20][ 9]=   -2.35324;m_PhiROImax[1][20][ 9]=	-2.24358;
     m_EtaROImin[1][20][10]=   0.555789;m_EtaROImax[1][20][10]=   0.648101;m_PhiROImin[1][20][10]=   -2.24008;m_PhiROImax[1][20][10]=	-2.13318;
     m_EtaROImin[1][20][11]=   0.458091;m_EtaROImax[1][20][11]=   0.554444;m_PhiROImin[1][20][11]=   -2.24008;m_PhiROImax[1][20][11]=	-2.13318;
     m_EtaROImin[1][20][12]=   0.731124;m_EtaROImax[1][20][12]=   0.776449;m_PhiROImin[1][20][12]=   -2.35323;m_PhiROImax[1][20][12]=	-2.24298;
     m_EtaROImin[1][20][13]=   0.655857;m_EtaROImax[1][20][13]=   0.728056;m_PhiROImin[1][20][13]=   -2.35323;m_PhiROImax[1][20][13]=	-2.24298;
     m_EtaROImin[1][20][14]=   0.731124;m_EtaROImax[1][20][14]=   0.776449;m_PhiROImin[1][20][14]=   -2.23946;m_PhiROImax[1][20][14]=	-2.13203;
     m_EtaROImin[1][20][15]=   0.655857;m_EtaROImax[1][20][15]=   0.728056;m_PhiROImin[1][20][15]=   -2.23946;m_PhiROImax[1][20][15]=	-2.13203;
     m_EtaROImin[1][20][16]=   0.853359;m_EtaROImax[1][20][16]=   0.895163;m_PhiROImin[1][20][16]=   -2.35323;m_PhiROImax[1][20][16]=	-2.24298;
     m_EtaROImin[1][20][17]=   0.783867;m_EtaROImax[1][20][17]=   0.850528;m_PhiROImin[1][20][17]=   -2.35323;m_PhiROImax[1][20][17]=	-2.24298;
     m_EtaROImin[1][20][18]=   0.853359;m_EtaROImax[1][20][18]=   0.895163;m_PhiROImin[1][20][18]=   -2.23946;m_PhiROImax[1][20][18]=	-2.13203;
     m_EtaROImin[1][20][19]=   0.783867;m_EtaROImax[1][20][19]=   0.850528;m_PhiROImin[1][20][19]=   -2.23946;m_PhiROImax[1][20][19]=	-2.13203;
     m_EtaROImin[1][20][20]=   0.964674;m_EtaROImax[1][20][20]=    1.02514;m_PhiROImin[1][20][20]=   -2.35324;m_PhiROImax[1][20][20]=	-2.24358;
     m_EtaROImin[1][20][21]=   0.898201;m_EtaROImax[1][20][21]=   0.961344;m_PhiROImin[1][20][21]=   -2.35324;m_PhiROImax[1][20][21]=	-2.24358;
     m_EtaROImin[1][20][22]=   0.964674;m_EtaROImax[1][20][22]=    1.02514;m_PhiROImin[1][20][22]=   -2.24008;m_PhiROImax[1][20][22]=	-2.13318;
     m_EtaROImin[1][20][23]=   0.898201;m_EtaROImax[1][20][23]=   0.961344;m_PhiROImin[1][20][23]=   -2.24008;m_PhiROImax[1][20][23]=	-2.13318;
     m_EtaROImin[1][20][24]=	    0;m_EtaROImax[1][20][24]=	     0;m_PhiROImin[1][20][24]=	      0;m_PhiROImax[1][20][24]=	       0;
     m_EtaROImin[1][20][25]=    1.03003;m_EtaROImax[1][20][25]=    1.06547;m_PhiROImin[1][20][25]=   -2.35313;m_PhiROImax[1][20][25]=	-2.24277;
     m_EtaROImin[1][20][26]=	    0;m_EtaROImax[1][20][26]=	     0;m_PhiROImin[1][20][26]=	      0;m_PhiROImax[1][20][26]=	       0;
     m_EtaROImin[1][20][27]=    1.03003;m_EtaROImax[1][20][27]=    1.06547;m_PhiROImin[1][20][27]=   -2.23924;m_PhiROImax[1][20][27]=	-2.15905;
     m_EtaROImin[1][20][28]=	    0;m_EtaROImax[1][20][28]=	     0;m_PhiROImin[1][20][28]=	      0;m_PhiROImax[1][20][28]=	       0;
     m_EtaROImin[1][20][29]=	    0;m_EtaROImax[1][20][29]=	     0;m_PhiROImin[1][20][29]=	      0;m_PhiROImax[1][20][29]=	       0;
     m_EtaROImin[1][20][30]=	    0;m_EtaROImax[1][20][30]=	     0;m_PhiROImin[1][20][30]=	      0;m_PhiROImax[1][20][30]=	       0;
     m_EtaROImin[1][20][31]=	    0;m_EtaROImax[1][20][31]=	     0;m_PhiROImin[1][20][31]=	      0;m_PhiROImax[1][20][31]=	       0;
     m_EtaROImin[1][21][ 0]=  0.0775634;m_EtaROImax[1][21][ 0]=   0.126072;m_PhiROImin[1][21][ 0]=   -2.05024;m_PhiROImax[1][21][ 0]=	-1.96629;
     m_EtaROImin[1][21][ 1]=   0.129296;m_EtaROImax[1][21][ 1]=   0.203032;m_PhiROImin[1][21][ 1]=   -2.05024;m_PhiROImax[1][21][ 1]=	-1.96629;
     m_EtaROImin[1][21][ 2]=  0.0775634;m_EtaROImax[1][21][ 2]=   0.126072;m_PhiROImin[1][21][ 2]=   -2.13655;m_PhiROImax[1][21][ 2]=	-2.05387;
     m_EtaROImin[1][21][ 3]=   0.129296;m_EtaROImax[1][21][ 3]=   0.203032;m_PhiROImin[1][21][ 3]=   -2.13655;m_PhiROImax[1][21][ 3]=	-2.05387;
     m_EtaROImin[1][21][ 4]=   0.200823;m_EtaROImax[1][21][ 4]=   0.248155;m_PhiROImin[1][21][ 4]=   -2.04983;m_PhiROImax[1][21][ 4]=	-1.96627;
     m_EtaROImin[1][21][ 5]=   0.251292;m_EtaROImax[1][21][ 5]=   0.322732;m_PhiROImin[1][21][ 5]=   -2.04983;m_PhiROImax[1][21][ 5]=	-1.96627;
     m_EtaROImin[1][21][ 6]=   0.200823;m_EtaROImax[1][21][ 6]=   0.248155;m_PhiROImin[1][21][ 6]=   -2.13573;m_PhiROImax[1][21][ 6]=	-2.05344;
     m_EtaROImin[1][21][ 7]=   0.251292;m_EtaROImax[1][21][ 7]=   0.322732;m_PhiROImin[1][21][ 7]=   -2.13573;m_PhiROImax[1][21][ 7]=	-2.05344;
     m_EtaROImin[1][21][ 8]=   0.362484;m_EtaROImax[1][21][ 8]=   0.441017;m_PhiROImin[1][21][ 8]=   -2.04536;m_PhiROImax[1][21][ 8]=	-1.96564;
     m_EtaROImin[1][21][ 9]=   0.439011;m_EtaROImax[1][21][ 9]=    0.51487;m_PhiROImin[1][21][ 9]=   -2.04536;m_PhiROImax[1][21][ 9]=	-1.96564;
     m_EtaROImin[1][21][10]=   0.362484;m_EtaROImax[1][21][10]=   0.441017;m_PhiROImin[1][21][10]=   -2.12657;m_PhiROImax[1][21][10]=	-2.04792;
     m_EtaROImin[1][21][11]=   0.439011;m_EtaROImax[1][21][11]=    0.51487;m_PhiROImin[1][21][11]=   -2.12657;m_PhiROImax[1][21][11]=	-2.04792;
     m_EtaROImin[1][21][12]=   0.470878;m_EtaROImax[1][21][12]=   0.569141;m_PhiROImin[1][21][12]=   -2.05024;m_PhiROImax[1][21][12]=	-1.96629;
     m_EtaROImin[1][21][13]=   0.566067;m_EtaROImax[1][21][13]=   0.659332;m_PhiROImin[1][21][13]=   -2.05024;m_PhiROImax[1][21][13]=	-1.96629;
     m_EtaROImin[1][21][14]=   0.470878;m_EtaROImax[1][21][14]=   0.569141;m_PhiROImin[1][21][14]=   -2.13655;m_PhiROImax[1][21][14]=	-2.05387;
     m_EtaROImin[1][21][15]=   0.566067;m_EtaROImax[1][21][15]=   0.659332;m_PhiROImin[1][21][15]=   -2.13655;m_PhiROImax[1][21][15]=	-2.05387;
     m_EtaROImin[1][21][16]=   0.622205;m_EtaROImax[1][21][16]=   0.674155;m_PhiROImin[1][21][16]=   -2.04536;m_PhiROImax[1][21][16]=	-1.96564;
     m_EtaROImin[1][21][17]=   0.671529;m_EtaROImax[1][21][17]=    0.72184;m_PhiROImin[1][21][17]=   -2.04536;m_PhiROImax[1][21][17]=	-1.96564;
     m_EtaROImin[1][21][18]=   0.622205;m_EtaROImax[1][21][18]=   0.674155;m_PhiROImin[1][21][18]=   -2.12657;m_PhiROImax[1][21][18]=	-2.04792;
     m_EtaROImin[1][21][19]=   0.671529;m_EtaROImax[1][21][19]=    0.72184;m_PhiROImin[1][21][19]=   -2.12657;m_PhiROImax[1][21][19]=	-2.04792;
     m_EtaROImin[1][21][20]=   0.744499;m_EtaROImax[1][21][20]=   0.831846;m_PhiROImin[1][21][20]=   -2.04536;m_PhiROImax[1][21][20]=	-1.96564;
     m_EtaROImin[1][21][21]=   0.833797;m_EtaROImax[1][21][21]=   0.893028;m_PhiROImin[1][21][21]=   -2.04536;m_PhiROImax[1][21][21]=	-1.96564;
     m_EtaROImin[1][21][22]=   0.744499;m_EtaROImax[1][21][22]=   0.831846;m_PhiROImin[1][21][22]=   -2.12657;m_PhiROImax[1][21][22]=	-2.04792;
     m_EtaROImin[1][21][23]=   0.833797;m_EtaROImax[1][21][23]=   0.893028;m_PhiROImin[1][21][23]=   -2.12657;m_PhiROImax[1][21][23]=	-2.04792;
     m_EtaROImin[1][21][24]=   0.788882;m_EtaROImax[1][21][24]=   0.850339;m_PhiROImin[1][21][24]=   -2.05024;m_PhiROImax[1][21][24]=	-1.96629;
     m_EtaROImin[1][21][25]=   0.846464;m_EtaROImax[1][21][25]=   0.905303;m_PhiROImin[1][21][25]=   -2.05024;m_PhiROImax[1][21][25]=	-1.96629;
     m_EtaROImin[1][21][26]=   0.788882;m_EtaROImax[1][21][26]=   0.850339;m_PhiROImin[1][21][26]=   -2.13655;m_PhiROImax[1][21][26]=	-2.05387;
     m_EtaROImin[1][21][27]=   0.846464;m_EtaROImax[1][21][27]=   0.905303;m_PhiROImin[1][21][27]=   -2.13655;m_PhiROImax[1][21][27]=	-2.05387;
     m_EtaROImin[1][21][28]=   0.911293;m_EtaROImax[1][21][28]=   0.938699;m_PhiROImin[1][21][28]=   -2.03358;m_PhiROImax[1][21][28]=	-1.98444;
     m_EtaROImin[1][21][29]=   0.940507;m_EtaROImax[1][21][29]=    0.98143;m_PhiROImin[1][21][29]=   -2.03358;m_PhiROImax[1][21][29]=	-1.98444;
     m_EtaROImin[1][21][30]=   0.911293;m_EtaROImax[1][21][30]=   0.938699;m_PhiROImin[1][21][30]=   -2.12127;m_PhiROImax[1][21][30]=	-2.03635;
     m_EtaROImin[1][21][31]=   0.940507;m_EtaROImax[1][21][31]=    0.98143;m_PhiROImin[1][21][31]=   -2.12127;m_PhiROImax[1][21][31]=	-2.03635;
     m_EtaROImin[1][22][ 0]=   0.155043;m_EtaROImax[1][22][ 0]=   0.203032;m_PhiROImin[1][22][ 0]=   -1.96071;m_PhiROImax[1][22][ 0]=	-1.87675;
     m_EtaROImin[1][22][ 1]=  0.0775634;m_EtaROImax[1][22][ 1]=    0.15183;m_PhiROImin[1][22][ 1]=   -1.96071;m_PhiROImax[1][22][ 1]=	-1.87675;
     m_EtaROImin[1][22][ 2]=   0.155043;m_EtaROImax[1][22][ 2]=   0.203032;m_PhiROImin[1][22][ 2]=   -1.87312;m_PhiROImax[1][22][ 2]=	-1.79045;
     m_EtaROImin[1][22][ 3]=  0.0775634;m_EtaROImax[1][22][ 3]=    0.15183;m_PhiROImin[1][22][ 3]=   -1.87312;m_PhiROImax[1][22][ 3]=	-1.79045;
     m_EtaROImin[1][22][ 4]=   0.276301;m_EtaROImax[1][22][ 4]=   0.322732;m_PhiROImin[1][22][ 4]=   -1.96072;m_PhiROImax[1][22][ 4]=	-1.87716;
     m_EtaROImin[1][22][ 5]=   0.200823;m_EtaROImax[1][22][ 5]=   0.273184;m_PhiROImin[1][22][ 5]=   -1.96072;m_PhiROImax[1][22][ 5]=	-1.87716;
     m_EtaROImin[1][22][ 6]=   0.276301;m_EtaROImax[1][22][ 6]=   0.322732;m_PhiROImin[1][22][ 6]=   -1.87355;m_PhiROImax[1][22][ 6]=	-1.79126;
     m_EtaROImin[1][22][ 7]=   0.200823;m_EtaROImax[1][22][ 7]=   0.273184;m_PhiROImin[1][22][ 7]=   -1.87355;m_PhiROImax[1][22][ 7]=	-1.79126;
     m_EtaROImin[1][22][ 8]=   0.439011;m_EtaROImax[1][22][ 8]=    0.51487;m_PhiROImin[1][22][ 8]=   -1.96135;m_PhiROImax[1][22][ 8]=	-1.88163;
     m_EtaROImin[1][22][ 9]=   0.362484;m_EtaROImax[1][22][ 9]=   0.441017;m_PhiROImin[1][22][ 9]=   -1.96135;m_PhiROImax[1][22][ 9]=	-1.88163;
     m_EtaROImin[1][22][10]=   0.439011;m_EtaROImax[1][22][10]=    0.51487;m_PhiROImin[1][22][10]=   -1.87907;m_PhiROImax[1][22][10]=	-1.80042;
     m_EtaROImin[1][22][11]=   0.362484;m_EtaROImax[1][22][11]=   0.441017;m_PhiROImin[1][22][11]=   -1.87907;m_PhiROImax[1][22][11]=	-1.80042;
     m_EtaROImin[1][22][12]=   0.566067;m_EtaROImax[1][22][12]=   0.659332;m_PhiROImin[1][22][12]=   -1.96071;m_PhiROImax[1][22][12]=	-1.87675;
     m_EtaROImin[1][22][13]=   0.470878;m_EtaROImax[1][22][13]=   0.569141;m_PhiROImin[1][22][13]=   -1.96071;m_PhiROImax[1][22][13]=	-1.87675;
     m_EtaROImin[1][22][14]=   0.566067;m_EtaROImax[1][22][14]=   0.659332;m_PhiROImin[1][22][14]=   -1.87312;m_PhiROImax[1][22][14]=	-1.79045;
     m_EtaROImin[1][22][15]=   0.470878;m_EtaROImax[1][22][15]=   0.569141;m_PhiROImin[1][22][15]=   -1.87312;m_PhiROImax[1][22][15]=	-1.79045;
     m_EtaROImin[1][22][16]=   0.671529;m_EtaROImax[1][22][16]=    0.72184;m_PhiROImin[1][22][16]=   -1.96135;m_PhiROImax[1][22][16]=	-1.88163;
     m_EtaROImin[1][22][17]=   0.622205;m_EtaROImax[1][22][17]=   0.674155;m_PhiROImin[1][22][17]=   -1.96135;m_PhiROImax[1][22][17]=	-1.88163;
     m_EtaROImin[1][22][18]=   0.671529;m_EtaROImax[1][22][18]=    0.72184;m_PhiROImin[1][22][18]=   -1.87907;m_PhiROImax[1][22][18]=	-1.80042;
     m_EtaROImin[1][22][19]=   0.622205;m_EtaROImax[1][22][19]=   0.674155;m_PhiROImin[1][22][19]=   -1.87907;m_PhiROImax[1][22][19]=	-1.80042;
     m_EtaROImin[1][22][20]=   0.833797;m_EtaROImax[1][22][20]=   0.893028;m_PhiROImin[1][22][20]=   -1.96135;m_PhiROImax[1][22][20]=	-1.88163;
     m_EtaROImin[1][22][21]=   0.744499;m_EtaROImax[1][22][21]=   0.831846;m_PhiROImin[1][22][21]=   -1.96135;m_PhiROImax[1][22][21]=	-1.88163;
     m_EtaROImin[1][22][22]=   0.833797;m_EtaROImax[1][22][22]=   0.893028;m_PhiROImin[1][22][22]=   -1.87907;m_PhiROImax[1][22][22]=	-1.80042;
     m_EtaROImin[1][22][23]=   0.744499;m_EtaROImax[1][22][23]=   0.831846;m_PhiROImin[1][22][23]=   -1.87907;m_PhiROImax[1][22][23]=	-1.80042;
     m_EtaROImin[1][22][24]=   0.846464;m_EtaROImax[1][22][24]=   0.905303;m_PhiROImin[1][22][24]=   -1.96071;m_PhiROImax[1][22][24]=	-1.87675;
     m_EtaROImin[1][22][25]=   0.788882;m_EtaROImax[1][22][25]=   0.850339;m_PhiROImin[1][22][25]=   -1.96071;m_PhiROImax[1][22][25]=	-1.87675;
     m_EtaROImin[1][22][26]=   0.846464;m_EtaROImax[1][22][26]=   0.905303;m_PhiROImin[1][22][26]=   -1.87312;m_PhiROImax[1][22][26]=	-1.79045;
     m_EtaROImin[1][22][27]=   0.788882;m_EtaROImax[1][22][27]=   0.850339;m_PhiROImin[1][22][27]=   -1.87312;m_PhiROImax[1][22][27]=	-1.79045;
     m_EtaROImin[1][22][28]=   0.957137;m_EtaROImax[1][22][28]=   0.983719;m_PhiROImin[1][22][28]=   -1.97686;m_PhiROImax[1][22][28]=	-1.89087;
     m_EtaROImin[1][22][29]=   0.913486;m_EtaROImax[1][22][29]=   0.955345;m_PhiROImin[1][22][29]=   -1.97686;m_PhiROImax[1][22][29]=	-1.89087;
     m_EtaROImin[1][22][30]=   0.957137;m_EtaROImax[1][22][30]=   0.983719;m_PhiROImin[1][22][30]=   -1.98444;m_PhiROImax[1][22][30]=	-1.88811;
     m_EtaROImin[1][22][31]=   0.913486;m_EtaROImax[1][22][31]=   0.955345;m_PhiROImin[1][22][31]=   -1.98444;m_PhiROImax[1][22][31]=	-1.88811;
     m_EtaROImin[1][23][ 0]=  0.0552456;m_EtaROImax[1][23][ 0]=   0.147216;m_PhiROImin[1][23][ 0]=   -1.68341;m_PhiROImax[1][23][ 0]=	-1.57375;
     m_EtaROImin[1][23][ 1]=    0.14708;m_EtaROImax[1][23][ 1]=   0.238313;m_PhiROImin[1][23][ 1]=   -1.68341;m_PhiROImax[1][23][ 1]=	-1.57375;
     m_EtaROImin[1][23][ 2]=  0.0552456;m_EtaROImax[1][23][ 2]=   0.147216;m_PhiROImin[1][23][ 2]=   -1.79381;m_PhiROImax[1][23][ 2]=	-1.68692;
     m_EtaROImin[1][23][ 3]=    0.14708;m_EtaROImax[1][23][ 3]=   0.238313;m_PhiROImin[1][23][ 3]=   -1.79381;m_PhiROImax[1][23][ 3]=	-1.68692;
     m_EtaROImin[1][23][ 4]=   0.246207;m_EtaROImax[1][23][ 4]=    0.35049;m_PhiROImin[1][23][ 4]=   -1.68341;m_PhiROImax[1][23][ 4]=	-1.57375;
     m_EtaROImin[1][23][ 5]=   0.350831;m_EtaROImax[1][23][ 5]=   0.452099;m_PhiROImin[1][23][ 5]=   -1.68341;m_PhiROImax[1][23][ 5]=	-1.57375;
     m_EtaROImin[1][23][ 6]=   0.246207;m_EtaROImax[1][23][ 6]=    0.35049;m_PhiROImin[1][23][ 6]=   -1.79381;m_PhiROImax[1][23][ 6]=	-1.68692;
     m_EtaROImin[1][23][ 7]=   0.350831;m_EtaROImax[1][23][ 7]=   0.452099;m_PhiROImin[1][23][ 7]=   -1.79381;m_PhiROImax[1][23][ 7]=	-1.68692;
     m_EtaROImin[1][23][ 8]=   0.458091;m_EtaROImax[1][23][ 8]=   0.554444;m_PhiROImin[1][23][ 8]=   -1.68341;m_PhiROImax[1][23][ 8]=	-1.57375;
     m_EtaROImin[1][23][ 9]=   0.555789;m_EtaROImax[1][23][ 9]=   0.648101;m_PhiROImin[1][23][ 9]=   -1.68341;m_PhiROImax[1][23][ 9]=	-1.57375;
     m_EtaROImin[1][23][10]=   0.458091;m_EtaROImax[1][23][10]=   0.554444;m_PhiROImin[1][23][10]=   -1.79381;m_PhiROImax[1][23][10]=	-1.68692;
     m_EtaROImin[1][23][11]=   0.555789;m_EtaROImax[1][23][11]=   0.648101;m_PhiROImin[1][23][11]=   -1.79381;m_PhiROImax[1][23][11]=	-1.68692;
     m_EtaROImin[1][23][12]=   0.677222;m_EtaROImax[1][23][12]=   0.720119;m_PhiROImin[1][23][12]=   -1.62692;m_PhiROImax[1][23][12]=	 -1.5708;
     m_EtaROImin[1][23][13]=    0.72294;m_EtaROImax[1][23][13]=   0.786462;m_PhiROImin[1][23][13]=   -1.62692;m_PhiROImax[1][23][13]=	 -1.5708;
     m_EtaROImin[1][23][14]=   0.677222;m_EtaROImax[1][23][14]=   0.720119;m_PhiROImin[1][23][14]=   -1.69744;m_PhiROImax[1][23][14]=	-1.63046;
     m_EtaROImin[1][23][15]=    0.72294;m_EtaROImax[1][23][15]=   0.786462;m_PhiROImin[1][23][15]=   -1.69744;m_PhiROImax[1][23][15]=	-1.63046;
     m_EtaROImin[1][23][16]=   0.783867;m_EtaROImax[1][23][16]=    0.82768;m_PhiROImin[1][23][16]=   -1.68401;m_PhiROImax[1][23][16]=	-1.57376;
     m_EtaROImin[1][23][17]=   0.830556;m_EtaROImax[1][23][17]=   0.895163;m_PhiROImin[1][23][17]=   -1.68401;m_PhiROImax[1][23][17]=	-1.57376;
     m_EtaROImin[1][23][18]=   0.783867;m_EtaROImax[1][23][18]=    0.82768;m_PhiROImin[1][23][18]=   -1.79497;m_PhiROImax[1][23][18]=	-1.68753;
     m_EtaROImin[1][23][19]=   0.830556;m_EtaROImax[1][23][19]=   0.895163;m_PhiROImin[1][23][19]=   -1.79497;m_PhiROImax[1][23][19]=	-1.68753;
     m_EtaROImin[1][23][20]=   0.898201;m_EtaROImax[1][23][20]=   0.961344;m_PhiROImin[1][23][20]=   -1.68341;m_PhiROImax[1][23][20]=	-1.57375;
     m_EtaROImin[1][23][21]=   0.964674;m_EtaROImax[1][23][21]=    1.02514;m_PhiROImin[1][23][21]=   -1.68341;m_PhiROImax[1][23][21]=	-1.57375;
     m_EtaROImin[1][23][22]=   0.898201;m_EtaROImax[1][23][22]=   0.961344;m_PhiROImin[1][23][22]=   -1.79381;m_PhiROImax[1][23][22]=	-1.68692;
     m_EtaROImin[1][23][23]=   0.964674;m_EtaROImax[1][23][23]=    1.02514;m_PhiROImin[1][23][23]=   -1.79381;m_PhiROImax[1][23][23]=	-1.68692;
     m_EtaROImin[1][23][24]=    1.03003;m_EtaROImax[1][23][24]=    1.06547;m_PhiROImin[1][23][24]=   -1.68423;m_PhiROImax[1][23][24]=	-1.57387;
     m_EtaROImin[1][23][25]=	    0;m_EtaROImax[1][23][25]=	     0;m_PhiROImin[1][23][25]=	      0;m_PhiROImax[1][23][25]=	       0;
     m_EtaROImin[1][23][26]=    1.03003;m_EtaROImax[1][23][26]=    1.06547;m_PhiROImin[1][23][26]=   -1.76794;m_PhiROImax[1][23][26]=	-1.68775;
     m_EtaROImin[1][23][27]=	    0;m_EtaROImax[1][23][27]=	     0;m_PhiROImin[1][23][27]=	      0;m_PhiROImax[1][23][27]=	       0;
     m_EtaROImin[1][23][28]=	    0;m_EtaROImax[1][23][28]=	     0;m_PhiROImin[1][23][28]=	      0;m_PhiROImax[1][23][28]=	       0;
     m_EtaROImin[1][23][29]=	    0;m_EtaROImax[1][23][29]=	     0;m_PhiROImin[1][23][29]=	      0;m_PhiROImax[1][23][29]=	       0;
     m_EtaROImin[1][23][30]=	    0;m_EtaROImax[1][23][30]=	     0;m_PhiROImin[1][23][30]=	      0;m_PhiROImax[1][23][30]=	       0;
     m_EtaROImin[1][23][31]=	    0;m_EtaROImax[1][23][31]=	     0;m_PhiROImin[1][23][31]=	      0;m_PhiROImax[1][23][31]=	       0;
     m_EtaROImin[1][24][ 0]=    0.14708;m_EtaROImax[1][24][ 0]=   0.238313;m_PhiROImin[1][24][ 0]=   -1.56784;m_PhiROImax[1][24][ 0]=	-1.45818;
     m_EtaROImin[1][24][ 1]=  0.0552456;m_EtaROImax[1][24][ 1]=   0.147216;m_PhiROImin[1][24][ 1]=   -1.56784;m_PhiROImax[1][24][ 1]=	-1.45818;
     m_EtaROImin[1][24][ 2]=    0.14708;m_EtaROImax[1][24][ 2]=   0.238313;m_PhiROImin[1][24][ 2]=   -1.45468;m_PhiROImax[1][24][ 2]=	-1.34779;
     m_EtaROImin[1][24][ 3]=  0.0552456;m_EtaROImax[1][24][ 3]=   0.147216;m_PhiROImin[1][24][ 3]=   -1.45468;m_PhiROImax[1][24][ 3]=	-1.34779;
     m_EtaROImin[1][24][ 4]=   0.350831;m_EtaROImax[1][24][ 4]=   0.452099;m_PhiROImin[1][24][ 4]=   -1.56784;m_PhiROImax[1][24][ 4]=	-1.45818;
     m_EtaROImin[1][24][ 5]=   0.246207;m_EtaROImax[1][24][ 5]=    0.35049;m_PhiROImin[1][24][ 5]=   -1.56784;m_PhiROImax[1][24][ 5]=	-1.45818;
     m_EtaROImin[1][24][ 6]=   0.350831;m_EtaROImax[1][24][ 6]=   0.452099;m_PhiROImin[1][24][ 6]=   -1.45468;m_PhiROImax[1][24][ 6]=	-1.34779;
     m_EtaROImin[1][24][ 7]=   0.246207;m_EtaROImax[1][24][ 7]=    0.35049;m_PhiROImin[1][24][ 7]=   -1.45468;m_PhiROImax[1][24][ 7]=	-1.34779;
     m_EtaROImin[1][24][ 8]=   0.555789;m_EtaROImax[1][24][ 8]=   0.648101;m_PhiROImin[1][24][ 8]=   -1.56784;m_PhiROImax[1][24][ 8]=	-1.45818;
     m_EtaROImin[1][24][ 9]=   0.458091;m_EtaROImax[1][24][ 9]=   0.554444;m_PhiROImin[1][24][ 9]=   -1.56784;m_PhiROImax[1][24][ 9]=	-1.45818;
     m_EtaROImin[1][24][10]=   0.555789;m_EtaROImax[1][24][10]=   0.648101;m_PhiROImin[1][24][10]=   -1.45468;m_PhiROImax[1][24][10]=	-1.34779;
     m_EtaROImin[1][24][11]=   0.458091;m_EtaROImax[1][24][11]=   0.554444;m_PhiROImin[1][24][11]=   -1.45468;m_PhiROImax[1][24][11]=	-1.34779;
     m_EtaROImin[1][24][12]=   0.745327;m_EtaROImax[1][24][12]=   0.786462;m_PhiROImin[1][24][12]=   -1.56798;m_PhiROImax[1][24][12]=	 -1.5005;
     m_EtaROImin[1][24][13]=   0.677222;m_EtaROImax[1][24][13]=   0.742545;m_PhiROImin[1][24][13]=   -1.56798;m_PhiROImax[1][24][13]=	 -1.5005;
     m_EtaROImin[1][24][14]=   0.745327;m_EtaROImax[1][24][14]=   0.786462;m_PhiROImin[1][24][14]=	-1.5708;m_PhiROImax[1][24][14]=	-1.49696;
     m_EtaROImin[1][24][15]=   0.677222;m_EtaROImax[1][24][15]=   0.742545;m_PhiROImin[1][24][15]=	-1.5708;m_PhiROImax[1][24][15]=	-1.49696;
     m_EtaROImin[1][24][16]=   0.853359;m_EtaROImax[1][24][16]=   0.895163;m_PhiROImin[1][24][16]=   -1.56783;m_PhiROImax[1][24][16]=	-1.45758;
     m_EtaROImin[1][24][17]=   0.783867;m_EtaROImax[1][24][17]=   0.850528;m_PhiROImin[1][24][17]=   -1.56783;m_PhiROImax[1][24][17]=	-1.45758;
     m_EtaROImin[1][24][18]=   0.853359;m_EtaROImax[1][24][18]=   0.895163;m_PhiROImin[1][24][18]=   -1.45406;m_PhiROImax[1][24][18]=	-1.34663;
     m_EtaROImin[1][24][19]=   0.783867;m_EtaROImax[1][24][19]=   0.850528;m_PhiROImin[1][24][19]=   -1.45406;m_PhiROImax[1][24][19]=	-1.34663;
     m_EtaROImin[1][24][20]=   0.964674;m_EtaROImax[1][24][20]=    1.02514;m_PhiROImin[1][24][20]=   -1.56784;m_PhiROImax[1][24][20]=	-1.45818;
     m_EtaROImin[1][24][21]=   0.898201;m_EtaROImax[1][24][21]=   0.961344;m_PhiROImin[1][24][21]=   -1.56784;m_PhiROImax[1][24][21]=	-1.45818;
     m_EtaROImin[1][24][22]=   0.964674;m_EtaROImax[1][24][22]=    1.02514;m_PhiROImin[1][24][22]=   -1.45468;m_PhiROImax[1][24][22]=	-1.34779;
     m_EtaROImin[1][24][23]=   0.898201;m_EtaROImax[1][24][23]=   0.961344;m_PhiROImin[1][24][23]=   -1.45468;m_PhiROImax[1][24][23]=	-1.34779;
     m_EtaROImin[1][24][24]=	    0;m_EtaROImax[1][24][24]=	     0;m_PhiROImin[1][24][24]=	      0;m_PhiROImax[1][24][24]=	       0;
     m_EtaROImin[1][24][25]=    1.03003;m_EtaROImax[1][24][25]=    1.06547;m_PhiROImin[1][24][25]=   -1.56773;m_PhiROImax[1][24][25]=	-1.45737;
     m_EtaROImin[1][24][26]=	    0;m_EtaROImax[1][24][26]=	     0;m_PhiROImin[1][24][26]=	      0;m_PhiROImax[1][24][26]=	       0;
     m_EtaROImin[1][24][27]=    1.03003;m_EtaROImax[1][24][27]=    1.06547;m_PhiROImin[1][24][27]=   -1.45384;m_PhiROImax[1][24][27]=	-1.37365;
     m_EtaROImin[1][24][28]=	    0;m_EtaROImax[1][24][28]=	     0;m_PhiROImin[1][24][28]=	      0;m_PhiROImax[1][24][28]=	       0;
     m_EtaROImin[1][24][29]=	    0;m_EtaROImax[1][24][29]=	     0;m_PhiROImin[1][24][29]=	      0;m_PhiROImax[1][24][29]=	       0;
     m_EtaROImin[1][24][30]=	    0;m_EtaROImax[1][24][30]=	     0;m_PhiROImin[1][24][30]=	      0;m_PhiROImax[1][24][30]=	       0;
     m_EtaROImin[1][24][31]=	    0;m_EtaROImax[1][24][31]=	     0;m_PhiROImin[1][24][31]=	      0;m_PhiROImax[1][24][31]=	       0;
     m_EtaROImin[1][25][ 0]=  0.0775634;m_EtaROImax[1][25][ 0]=   0.126072;m_PhiROImin[1][25][ 0]=   -1.26484;m_PhiROImax[1][25][ 0]=	-1.18089;
     m_EtaROImin[1][25][ 1]=   0.129296;m_EtaROImax[1][25][ 1]=   0.203032;m_PhiROImin[1][25][ 1]=   -1.26484;m_PhiROImax[1][25][ 1]=	-1.18089;
     m_EtaROImin[1][25][ 2]=  0.0775634;m_EtaROImax[1][25][ 2]=   0.126072;m_PhiROImin[1][25][ 2]=   -1.35115;m_PhiROImax[1][25][ 2]=	-1.26847;
     m_EtaROImin[1][25][ 3]=   0.129296;m_EtaROImax[1][25][ 3]=   0.203032;m_PhiROImin[1][25][ 3]=   -1.35115;m_PhiROImax[1][25][ 3]=	-1.26847;
     m_EtaROImin[1][25][ 4]=   0.200823;m_EtaROImax[1][25][ 4]=   0.248155;m_PhiROImin[1][25][ 4]=   -1.26443;m_PhiROImax[1][25][ 4]=	-1.18087;
     m_EtaROImin[1][25][ 5]=   0.251292;m_EtaROImax[1][25][ 5]=   0.322732;m_PhiROImin[1][25][ 5]=   -1.26443;m_PhiROImax[1][25][ 5]=	-1.18087;
     m_EtaROImin[1][25][ 6]=   0.200823;m_EtaROImax[1][25][ 6]=   0.248155;m_PhiROImin[1][25][ 6]=   -1.35034;m_PhiROImax[1][25][ 6]=	-1.26804;
     m_EtaROImin[1][25][ 7]=   0.251292;m_EtaROImax[1][25][ 7]=   0.322732;m_PhiROImin[1][25][ 7]=   -1.35034;m_PhiROImax[1][25][ 7]=	-1.26804;
     m_EtaROImin[1][25][ 8]=   0.361231;m_EtaROImax[1][25][ 8]=   0.439522;m_PhiROImin[1][25][ 8]=   -1.25967;m_PhiROImax[1][25][ 8]=	-1.18023;
     m_EtaROImin[1][25][ 9]=   0.440504;m_EtaROImax[1][25][ 9]=   0.516583;m_PhiROImin[1][25][ 9]=   -1.25967;m_PhiROImax[1][25][ 9]=	-1.18023;
     m_EtaROImin[1][25][10]=   0.361231;m_EtaROImax[1][25][10]=   0.439522;m_PhiROImin[1][25][10]=   -1.34059;m_PhiROImax[1][25][10]=	-1.26222;
     m_EtaROImin[1][25][11]=   0.440504;m_EtaROImax[1][25][11]=   0.516583;m_PhiROImin[1][25][11]=   -1.34059;m_PhiROImax[1][25][11]=	-1.26222;
     m_EtaROImin[1][25][12]=   0.470878;m_EtaROImax[1][25][12]=   0.569141;m_PhiROImin[1][25][12]=   -1.26484;m_PhiROImax[1][25][12]=	-1.18089;
     m_EtaROImin[1][25][13]=   0.566067;m_EtaROImax[1][25][13]=   0.659332;m_PhiROImin[1][25][13]=   -1.26484;m_PhiROImax[1][25][13]=	-1.18089;
     m_EtaROImin[1][25][14]=   0.470878;m_EtaROImax[1][25][14]=   0.569141;m_PhiROImin[1][25][14]=   -1.35115;m_PhiROImax[1][25][14]=	-1.26847;
     m_EtaROImin[1][25][15]=   0.566067;m_EtaROImax[1][25][15]=   0.659332;m_PhiROImin[1][25][15]=   -1.35115;m_PhiROImax[1][25][15]=	-1.26847;
     m_EtaROImin[1][25][16]=   0.620212;m_EtaROImax[1][25][16]=   0.672035;m_PhiROImin[1][25][16]=   -1.25967;m_PhiROImax[1][25][16]=	-1.18023;
     m_EtaROImin[1][25][17]=   0.673648;m_EtaROImax[1][25][17]=   0.724075;m_PhiROImin[1][25][17]=   -1.25967;m_PhiROImax[1][25][17]=	-1.18023;
     m_EtaROImin[1][25][18]=   0.620212;m_EtaROImax[1][25][18]=   0.672035;m_PhiROImin[1][25][18]=   -1.34059;m_PhiROImax[1][25][18]=	-1.26222;
     m_EtaROImin[1][25][19]=   0.673648;m_EtaROImax[1][25][19]=   0.724075;m_PhiROImin[1][25][19]=   -1.34059;m_PhiROImax[1][25][19]=	-1.26222;
     m_EtaROImin[1][25][20]=   0.744499;m_EtaROImax[1][25][20]=   0.831846;m_PhiROImin[1][25][20]=   -1.25997;m_PhiROImax[1][25][20]=	-1.18024;
     m_EtaROImin[1][25][21]=   0.833797;m_EtaROImax[1][25][21]=   0.893028;m_PhiROImin[1][25][21]=   -1.25997;m_PhiROImax[1][25][21]=	-1.18024;
     m_EtaROImin[1][25][22]=   0.744499;m_EtaROImax[1][25][22]=   0.831846;m_PhiROImin[1][25][22]=   -1.34117;m_PhiROImax[1][25][22]=	-1.26253;
     m_EtaROImin[1][25][23]=   0.833797;m_EtaROImax[1][25][23]=   0.893028;m_PhiROImin[1][25][23]=   -1.34117;m_PhiROImax[1][25][23]=	-1.26253;
     m_EtaROImin[1][25][24]=   0.788882;m_EtaROImax[1][25][24]=   0.850339;m_PhiROImin[1][25][24]=   -1.26484;m_PhiROImax[1][25][24]=	-1.18089;
     m_EtaROImin[1][25][25]=   0.846464;m_EtaROImax[1][25][25]=   0.905303;m_PhiROImin[1][25][25]=   -1.26484;m_PhiROImax[1][25][25]=	-1.18089;
     m_EtaROImin[1][25][26]=   0.788882;m_EtaROImax[1][25][26]=   0.850339;m_PhiROImin[1][25][26]=   -1.35115;m_PhiROImax[1][25][26]=	-1.26847;
     m_EtaROImin[1][25][27]=   0.846464;m_EtaROImax[1][25][27]=   0.905303;m_PhiROImin[1][25][27]=   -1.35115;m_PhiROImax[1][25][27]=	-1.26847;
     m_EtaROImin[1][25][28]=   0.914143;m_EtaROImax[1][25][28]=	0.9416;m_PhiROImin[1][25][28]=   -1.20642;m_PhiROImax[1][25][28]=	-1.15713;
     m_EtaROImin[1][25][29]=   0.943411;m_EtaROImax[1][25][29]=   0.984405;m_PhiROImin[1][25][29]=   -1.20642;m_PhiROImax[1][25][29]=	-1.15713;
     m_EtaROImin[1][25][30]=   0.914143;m_EtaROImax[1][25][30]=	0.9416;m_PhiROImin[1][25][30]=   -1.29487;m_PhiROImax[1][25][30]=	 -1.2092;
     m_EtaROImin[1][25][31]=   0.943411;m_EtaROImax[1][25][31]=   0.984405;m_PhiROImin[1][25][31]=   -1.29487;m_PhiROImax[1][25][31]=	 -1.2092;
     m_EtaROImin[1][26][ 0]=   0.155043;m_EtaROImax[1][26][ 0]=   0.203032;m_PhiROImin[1][26][ 0]=   -1.17531;m_PhiROImax[1][26][ 0]=	-1.09135;
     m_EtaROImin[1][26][ 1]=  0.0775634;m_EtaROImax[1][26][ 1]=    0.15183;m_PhiROImin[1][26][ 1]=   -1.17531;m_PhiROImax[1][26][ 1]=	-1.09135;
     m_EtaROImin[1][26][ 2]=   0.155043;m_EtaROImax[1][26][ 2]=   0.203032;m_PhiROImin[1][26][ 2]=   -1.08772;m_PhiROImax[1][26][ 2]=	-1.00505;
     m_EtaROImin[1][26][ 3]=  0.0775634;m_EtaROImax[1][26][ 3]=    0.15183;m_PhiROImin[1][26][ 3]=   -1.08772;m_PhiROImax[1][26][ 3]=	-1.00505;
     m_EtaROImin[1][26][ 4]=   0.276301;m_EtaROImax[1][26][ 4]=   0.322732;m_PhiROImin[1][26][ 4]=   -1.17532;m_PhiROImax[1][26][ 4]=	-1.09176;
     m_EtaROImin[1][26][ 5]=   0.200823;m_EtaROImax[1][26][ 5]=   0.273184;m_PhiROImin[1][26][ 5]=   -1.17532;m_PhiROImax[1][26][ 5]=	-1.09176;
     m_EtaROImin[1][26][ 6]=   0.276301;m_EtaROImax[1][26][ 6]=   0.322732;m_PhiROImin[1][26][ 6]=   -1.08815;m_PhiROImax[1][26][ 6]=	-1.00586;
     m_EtaROImin[1][26][ 7]=   0.200823;m_EtaROImax[1][26][ 7]=   0.273184;m_PhiROImin[1][26][ 7]=   -1.08815;m_PhiROImax[1][26][ 7]=	-1.00586;
     m_EtaROImin[1][26][ 8]=   0.440504;m_EtaROImax[1][26][ 8]=   0.516583;m_PhiROImin[1][26][ 8]=   -1.17596;m_PhiROImax[1][26][ 8]=	-1.09652;
     m_EtaROImin[1][26][ 9]=   0.361231;m_EtaROImax[1][26][ 9]=   0.439522;m_PhiROImin[1][26][ 9]=   -1.17596;m_PhiROImax[1][26][ 9]=	-1.09652;
     m_EtaROImin[1][26][10]=   0.440504;m_EtaROImax[1][26][10]=   0.516583;m_PhiROImin[1][26][10]=   -1.09397;m_PhiROImax[1][26][10]=	 -1.0156;
     m_EtaROImin[1][26][11]=   0.361231;m_EtaROImax[1][26][11]=   0.439522;m_PhiROImin[1][26][11]=   -1.09397;m_PhiROImax[1][26][11]=	 -1.0156;
     m_EtaROImin[1][26][12]=   0.566067;m_EtaROImax[1][26][12]=   0.659332;m_PhiROImin[1][26][12]=   -1.17531;m_PhiROImax[1][26][12]=	-1.09135;
     m_EtaROImin[1][26][13]=   0.470878;m_EtaROImax[1][26][13]=   0.569141;m_PhiROImin[1][26][13]=   -1.17531;m_PhiROImax[1][26][13]=	-1.09135;
     m_EtaROImin[1][26][14]=   0.566067;m_EtaROImax[1][26][14]=   0.659332;m_PhiROImin[1][26][14]=   -1.08772;m_PhiROImax[1][26][14]=	-1.00505;
     m_EtaROImin[1][26][15]=   0.470878;m_EtaROImax[1][26][15]=   0.569141;m_PhiROImin[1][26][15]=   -1.08772;m_PhiROImax[1][26][15]=	-1.00505;
     m_EtaROImin[1][26][16]=   0.673648;m_EtaROImax[1][26][16]=   0.724075;m_PhiROImin[1][26][16]=   -1.17596;m_PhiROImax[1][26][16]=	-1.09652;
     m_EtaROImin[1][26][17]=   0.620212;m_EtaROImax[1][26][17]=   0.672035;m_PhiROImin[1][26][17]=   -1.17596;m_PhiROImax[1][26][17]=	-1.09652;
     m_EtaROImin[1][26][18]=   0.673648;m_EtaROImax[1][26][18]=   0.724075;m_PhiROImin[1][26][18]=   -1.09397;m_PhiROImax[1][26][18]=	 -1.0156;
     m_EtaROImin[1][26][19]=   0.620212;m_EtaROImax[1][26][19]=   0.672035;m_PhiROImin[1][26][19]=   -1.09397;m_PhiROImax[1][26][19]=	 -1.0156;
     m_EtaROImin[1][26][20]=   0.833797;m_EtaROImax[1][26][20]=   0.893028;m_PhiROImin[1][26][20]=   -1.17595;m_PhiROImax[1][26][20]=	-1.09623;
     m_EtaROImin[1][26][21]=   0.744499;m_EtaROImax[1][26][21]=   0.831846;m_PhiROImin[1][26][21]=   -1.17595;m_PhiROImax[1][26][21]=	-1.09623;
     m_EtaROImin[1][26][22]=   0.833797;m_EtaROImax[1][26][22]=   0.893028;m_PhiROImin[1][26][22]=   -1.09367;m_PhiROImax[1][26][22]=	-1.01502;
     m_EtaROImin[1][26][23]=   0.744499;m_EtaROImax[1][26][23]=   0.831846;m_PhiROImin[1][26][23]=   -1.09367;m_PhiROImax[1][26][23]=	-1.01502;
     m_EtaROImin[1][26][24]=   0.846464;m_EtaROImax[1][26][24]=   0.905303;m_PhiROImin[1][26][24]=   -1.17531;m_PhiROImax[1][26][24]=	-1.09135;
     m_EtaROImin[1][26][25]=   0.788882;m_EtaROImax[1][26][25]=   0.850339;m_PhiROImin[1][26][25]=   -1.17531;m_PhiROImax[1][26][25]=	-1.09135;
     m_EtaROImin[1][26][26]=   0.846464;m_EtaROImax[1][26][26]=   0.905303;m_PhiROImin[1][26][26]=   -1.08772;m_PhiROImax[1][26][26]=	-1.00505;
     m_EtaROImin[1][26][27]=   0.788882;m_EtaROImax[1][26][27]=   0.850339;m_PhiROImin[1][26][27]=   -1.08772;m_PhiROImax[1][26][27]=	-1.00505;
     m_EtaROImin[1][26][28]=   0.955553;m_EtaROImax[1][26][28]=    0.98211;m_PhiROImin[1][26][28]=   -1.14955;m_PhiROImax[1][26][28]=	-1.06384;
     m_EtaROImin[1][26][29]=   0.911945;m_EtaROImax[1][26][29]=   0.953764;m_PhiROImin[1][26][29]=   -1.14955;m_PhiROImax[1][26][29]=	-1.06384;
     m_EtaROImin[1][26][30]=   0.955553;m_EtaROImax[1][26][30]=    0.98211;m_PhiROImin[1][26][30]=   -1.15713;m_PhiROImax[1][26][30]=	 -1.0611;
     m_EtaROImin[1][26][31]=   0.911945;m_EtaROImax[1][26][31]=   0.953764;m_PhiROImin[1][26][31]=   -1.15713;m_PhiROImax[1][26][31]=	 -1.0611;
     m_EtaROImin[1][27][ 0]=  0.0874905;m_EtaROImax[1][27][ 0]=   0.145912;m_PhiROImin[1][27][ 0]=   -0.89861;m_PhiROImax[1][27][ 0]=  -0.788366;
     m_EtaROImin[1][27][ 1]=   0.149792;m_EtaROImax[1][27][ 1]=   0.238294;m_PhiROImin[1][27][ 1]=   -0.89861;m_PhiROImax[1][27][ 1]=  -0.788366;
     m_EtaROImin[1][27][ 2]=  0.0874905;m_EtaROImax[1][27][ 2]=   0.145912;m_PhiROImin[1][27][ 2]=   -1.00957;m_PhiROImax[1][27][ 2]=  -0.902135;
     m_EtaROImin[1][27][ 3]=   0.149792;m_EtaROImax[1][27][ 3]=   0.238294;m_PhiROImin[1][27][ 3]=   -1.00957;m_PhiROImax[1][27][ 3]=  -0.902135;
     m_EtaROImin[1][27][ 4]=   0.246207;m_EtaROImax[1][27][ 4]=    0.35049;m_PhiROImin[1][27][ 4]=  -0.898011;m_PhiROImax[1][27][ 4]=	-0.78835;
     m_EtaROImin[1][27][ 5]=   0.350831;m_EtaROImax[1][27][ 5]=   0.452099;m_PhiROImin[1][27][ 5]=  -0.898011;m_PhiROImax[1][27][ 5]=	-0.78835;
     m_EtaROImin[1][27][ 6]=   0.246207;m_EtaROImax[1][27][ 6]=    0.35049;m_PhiROImin[1][27][ 6]=   -1.00841;m_PhiROImax[1][27][ 6]=  -0.901517;
     m_EtaROImin[1][27][ 7]=   0.350831;m_EtaROImax[1][27][ 7]=   0.452099;m_PhiROImin[1][27][ 7]=   -1.00841;m_PhiROImax[1][27][ 7]=  -0.901517;
     m_EtaROImin[1][27][ 8]=   0.458091;m_EtaROImax[1][27][ 8]=   0.554444;m_PhiROImin[1][27][ 8]=  -0.898011;m_PhiROImax[1][27][ 8]=	-0.78835;
     m_EtaROImin[1][27][ 9]=   0.555789;m_EtaROImax[1][27][ 9]=   0.648101;m_PhiROImin[1][27][ 9]=  -0.898011;m_PhiROImax[1][27][ 9]=	-0.78835;
     m_EtaROImin[1][27][10]=   0.458091;m_EtaROImax[1][27][10]=   0.554444;m_PhiROImin[1][27][10]=   -1.00841;m_PhiROImax[1][27][10]=  -0.901517;
     m_EtaROImin[1][27][11]=   0.555789;m_EtaROImax[1][27][11]=   0.648101;m_PhiROImin[1][27][11]=   -1.00841;m_PhiROImax[1][27][11]=  -0.901517;
     m_EtaROImin[1][27][12]=   0.655857;m_EtaROImax[1][27][12]=   0.703298;m_PhiROImin[1][27][12]=   -0.89861;m_PhiROImax[1][27][12]=  -0.788366;
     m_EtaROImin[1][27][13]=   0.706413;m_EtaROImax[1][27][13]=   0.776449;m_PhiROImin[1][27][13]=   -0.89861;m_PhiROImax[1][27][13]=  -0.788366;
     m_EtaROImin[1][27][14]=   0.655857;m_EtaROImax[1][27][14]=   0.703298;m_PhiROImin[1][27][14]=   -1.00957;m_PhiROImax[1][27][14]=  -0.902135;
     m_EtaROImin[1][27][15]=   0.706413;m_EtaROImax[1][27][15]=   0.776449;m_PhiROImin[1][27][15]=   -1.00957;m_PhiROImax[1][27][15]=  -0.902135;
     m_EtaROImin[1][27][16]=   0.783867;m_EtaROImax[1][27][16]=    0.82768;m_PhiROImin[1][27][16]=   -0.89861;m_PhiROImax[1][27][16]=  -0.788366;
     m_EtaROImin[1][27][17]=   0.830556;m_EtaROImax[1][27][17]=   0.895163;m_PhiROImin[1][27][17]=   -0.89861;m_PhiROImax[1][27][17]=  -0.788366;
     m_EtaROImin[1][27][18]=   0.783867;m_EtaROImax[1][27][18]=    0.82768;m_PhiROImin[1][27][18]=   -1.00957;m_PhiROImax[1][27][18]=  -0.902135;
     m_EtaROImin[1][27][19]=   0.830556;m_EtaROImax[1][27][19]=   0.895163;m_PhiROImin[1][27][19]=   -1.00957;m_PhiROImax[1][27][19]=  -0.902135;
     m_EtaROImin[1][27][20]=   0.898201;m_EtaROImax[1][27][20]=   0.961344;m_PhiROImin[1][27][20]=  -0.898011;m_PhiROImax[1][27][20]=	-0.78835;
     m_EtaROImin[1][27][21]=   0.964674;m_EtaROImax[1][27][21]=    1.02514;m_PhiROImin[1][27][21]=  -0.898011;m_PhiROImax[1][27][21]=	-0.78835;
     m_EtaROImin[1][27][22]=   0.898201;m_EtaROImax[1][27][22]=   0.961344;m_PhiROImin[1][27][22]=   -1.00841;m_PhiROImax[1][27][22]=  -0.901517;
     m_EtaROImin[1][27][23]=   0.964674;m_EtaROImax[1][27][23]=    1.02514;m_PhiROImin[1][27][23]=   -1.00841;m_PhiROImax[1][27][23]=  -0.901517;
     m_EtaROImin[1][27][24]=    1.03003;m_EtaROImax[1][27][24]=    1.06547;m_PhiROImin[1][27][24]=  -0.898827;m_PhiROImax[1][27][24]=  -0.788467;
     m_EtaROImin[1][27][25]=	    0;m_EtaROImax[1][27][25]=	     0;m_PhiROImin[1][27][25]=	      0;m_PhiROImax[1][27][25]=	       0;
     m_EtaROImin[1][27][26]=    1.03003;m_EtaROImax[1][27][26]=    1.06547;m_PhiROImin[1][27][26]=  -0.982541;m_PhiROImax[1][27][26]=  -0.902356;
     m_EtaROImin[1][27][27]=	    0;m_EtaROImax[1][27][27]=	     0;m_PhiROImin[1][27][27]=	      0;m_PhiROImax[1][27][27]=	       0;
     m_EtaROImin[1][27][28]=	    0;m_EtaROImax[1][27][28]=	     0;m_PhiROImin[1][27][28]=	      0;m_PhiROImax[1][27][28]=	       0;
     m_EtaROImin[1][27][29]=	    0;m_EtaROImax[1][27][29]=	     0;m_PhiROImin[1][27][29]=	      0;m_PhiROImax[1][27][29]=	       0;
     m_EtaROImin[1][27][30]=	    0;m_EtaROImax[1][27][30]=	     0;m_PhiROImin[1][27][30]=	      0;m_PhiROImax[1][27][30]=	       0;
     m_EtaROImin[1][27][31]=	    0;m_EtaROImax[1][27][31]=	     0;m_PhiROImin[1][27][31]=	      0;m_PhiROImax[1][27][31]=	       0;
     m_EtaROImin[1][28][ 0]=   0.180742;m_EtaROImax[1][28][ 0]=   0.238294;m_PhiROImin[1][28][ 0]=   -0.78243;m_PhiROImax[1][28][ 0]=  -0.672186;
     m_EtaROImin[1][28][ 1]=  0.0874905;m_EtaROImax[1][28][ 1]=   0.176882;m_PhiROImin[1][28][ 1]=   -0.78243;m_PhiROImax[1][28][ 1]=  -0.672186;
     m_EtaROImin[1][28][ 2]=   0.180742;m_EtaROImax[1][28][ 2]=   0.238294;m_PhiROImin[1][28][ 2]=  -0.668661;m_PhiROImax[1][28][ 2]=  -0.561229;
     m_EtaROImin[1][28][ 3]=  0.0874905;m_EtaROImax[1][28][ 3]=   0.176882;m_PhiROImin[1][28][ 3]=  -0.668661;m_PhiROImax[1][28][ 3]=  -0.561229;
     m_EtaROImin[1][28][ 4]=   0.350831;m_EtaROImax[1][28][ 4]=   0.452099;m_PhiROImin[1][28][ 4]=  -0.782446;m_PhiROImax[1][28][ 4]=  -0.672786;
     m_EtaROImin[1][28][ 5]=   0.246207;m_EtaROImax[1][28][ 5]=    0.35049;m_PhiROImin[1][28][ 5]=  -0.782446;m_PhiROImax[1][28][ 5]=  -0.672786;
     m_EtaROImin[1][28][ 6]=   0.350831;m_EtaROImax[1][28][ 6]=   0.452099;m_PhiROImin[1][28][ 6]=  -0.669279;m_PhiROImax[1][28][ 6]=  -0.562387;
     m_EtaROImin[1][28][ 7]=   0.246207;m_EtaROImax[1][28][ 7]=    0.35049;m_PhiROImin[1][28][ 7]=  -0.669279;m_PhiROImax[1][28][ 7]=  -0.562387;
     m_EtaROImin[1][28][ 8]=   0.555789;m_EtaROImax[1][28][ 8]=   0.648101;m_PhiROImin[1][28][ 8]=  -0.782446;m_PhiROImax[1][28][ 8]=  -0.672786;
     m_EtaROImin[1][28][ 9]=   0.458091;m_EtaROImax[1][28][ 9]=   0.554444;m_PhiROImin[1][28][ 9]=  -0.782446;m_PhiROImax[1][28][ 9]=  -0.672786;
     m_EtaROImin[1][28][10]=   0.555789;m_EtaROImax[1][28][10]=   0.648101;m_PhiROImin[1][28][10]=  -0.669279;m_PhiROImax[1][28][10]=  -0.562387;
     m_EtaROImin[1][28][11]=   0.458091;m_EtaROImax[1][28][11]=   0.554444;m_PhiROImin[1][28][11]=  -0.669279;m_PhiROImax[1][28][11]=  -0.562387;
     m_EtaROImin[1][28][12]=   0.731124;m_EtaROImax[1][28][12]=   0.776449;m_PhiROImin[1][28][12]=   -0.78243;m_PhiROImax[1][28][12]=  -0.672186;
     m_EtaROImin[1][28][13]=   0.655857;m_EtaROImax[1][28][13]=   0.728056;m_PhiROImin[1][28][13]=   -0.78243;m_PhiROImax[1][28][13]=  -0.672186;
     m_EtaROImin[1][28][14]=   0.731124;m_EtaROImax[1][28][14]=   0.776449;m_PhiROImin[1][28][14]=  -0.668661;m_PhiROImax[1][28][14]=  -0.561229;
     m_EtaROImin[1][28][15]=   0.655857;m_EtaROImax[1][28][15]=   0.728056;m_PhiROImin[1][28][15]=  -0.668661;m_PhiROImax[1][28][15]=  -0.561229;
     m_EtaROImin[1][28][16]=   0.853359;m_EtaROImax[1][28][16]=   0.895163;m_PhiROImin[1][28][16]=   -0.78243;m_PhiROImax[1][28][16]=  -0.672186;
     m_EtaROImin[1][28][17]=   0.783867;m_EtaROImax[1][28][17]=   0.850528;m_PhiROImin[1][28][17]=   -0.78243;m_PhiROImax[1][28][17]=  -0.672186;
     m_EtaROImin[1][28][18]=   0.853359;m_EtaROImax[1][28][18]=   0.895163;m_PhiROImin[1][28][18]=  -0.668661;m_PhiROImax[1][28][18]=  -0.561229;
     m_EtaROImin[1][28][19]=   0.783867;m_EtaROImax[1][28][19]=   0.850528;m_PhiROImin[1][28][19]=  -0.668661;m_PhiROImax[1][28][19]=  -0.561229;
     m_EtaROImin[1][28][20]=   0.964674;m_EtaROImax[1][28][20]=    1.02514;m_PhiROImin[1][28][20]=  -0.782446;m_PhiROImax[1][28][20]=  -0.672786;
     m_EtaROImin[1][28][21]=   0.898201;m_EtaROImax[1][28][21]=   0.961344;m_PhiROImin[1][28][21]=  -0.782446;m_PhiROImax[1][28][21]=  -0.672786;
     m_EtaROImin[1][28][22]=   0.964674;m_EtaROImax[1][28][22]=    1.02514;m_PhiROImin[1][28][22]=  -0.669279;m_PhiROImax[1][28][22]=  -0.562387;
     m_EtaROImin[1][28][23]=   0.898201;m_EtaROImax[1][28][23]=   0.961344;m_PhiROImin[1][28][23]=  -0.669279;m_PhiROImax[1][28][23]=  -0.562387;
     m_EtaROImin[1][28][24]=	    0;m_EtaROImax[1][28][24]=	     0;m_PhiROImin[1][28][24]=	      0;m_PhiROImax[1][28][24]=	       0;
     m_EtaROImin[1][28][25]=    1.03003;m_EtaROImax[1][28][25]=    1.06547;m_PhiROImin[1][28][25]=  -0.782329;m_PhiROImax[1][28][25]=  -0.671969;
     m_EtaROImin[1][28][26]=	    0;m_EtaROImax[1][28][26]=	     0;m_PhiROImin[1][28][26]=	      0;m_PhiROImax[1][28][26]=	       0;
     m_EtaROImin[1][28][27]=    1.03003;m_EtaROImax[1][28][27]=    1.06547;m_PhiROImin[1][28][27]=   -0.66844;m_PhiROImax[1][28][27]=  -0.588255;
     m_EtaROImin[1][28][28]=	    0;m_EtaROImax[1][28][28]=	     0;m_PhiROImin[1][28][28]=	      0;m_PhiROImax[1][28][28]=	       0;
     m_EtaROImin[1][28][29]=	    0;m_EtaROImax[1][28][29]=	     0;m_PhiROImin[1][28][29]=	      0;m_PhiROImax[1][28][29]=	       0;
     m_EtaROImin[1][28][30]=	    0;m_EtaROImax[1][28][30]=	     0;m_PhiROImin[1][28][30]=	      0;m_PhiROImax[1][28][30]=	       0;
     m_EtaROImin[1][28][31]=	    0;m_EtaROImax[1][28][31]=	     0;m_PhiROImin[1][28][31]=	      0;m_PhiROImax[1][28][31]=	       0;
     m_EtaROImin[1][29][ 0]=  0.0208251;m_EtaROImax[1][29][ 0]=   0.118734;m_PhiROImin[1][29][ 0]=  -0.479445;m_PhiROImax[1][29][ 0]=  -0.395489;
     m_EtaROImin[1][29][ 1]=   0.116816;m_EtaROImax[1][29][ 1]=   0.213185;m_PhiROImin[1][29][ 1]=  -0.479445;m_PhiROImax[1][29][ 1]=  -0.395489;
     m_EtaROImin[1][29][ 2]=  0.0208251;m_EtaROImax[1][29][ 2]=   0.118734;m_PhiROImin[1][29][ 2]=  -0.565749;m_PhiROImax[1][29][ 2]=  -0.483076;
     m_EtaROImin[1][29][ 3]=   0.116816;m_EtaROImax[1][29][ 3]=   0.213185;m_PhiROImin[1][29][ 3]=  -0.565749;m_PhiROImax[1][29][ 3]=  -0.483076;
     m_EtaROImin[1][29][ 4]=   0.222449;m_EtaROImax[1][29][ 4]=   0.302928;m_PhiROImin[1][29][ 4]=  -0.479445;m_PhiROImax[1][29][ 4]=  -0.395489;
     m_EtaROImin[1][29][ 5]=    0.30075;m_EtaROImax[1][29][ 5]=   0.416721;m_PhiROImin[1][29][ 5]=  -0.479445;m_PhiROImax[1][29][ 5]=  -0.395489;
     m_EtaROImin[1][29][ 6]=   0.222449;m_EtaROImax[1][29][ 6]=   0.302928;m_PhiROImin[1][29][ 6]=  -0.565749;m_PhiROImax[1][29][ 6]=  -0.483076;
     m_EtaROImin[1][29][ 7]=    0.30075;m_EtaROImax[1][29][ 7]=   0.416721;m_PhiROImin[1][29][ 7]=  -0.565749;m_PhiROImax[1][29][ 7]=  -0.483076;
     m_EtaROImin[1][29][ 8]=    0.42967;m_EtaROImax[1][29][ 8]=   0.504617;m_PhiROImin[1][29][ 8]=  -0.479445;m_PhiROImax[1][29][ 8]=  -0.395489;
     m_EtaROImin[1][29][ 9]=   0.501681;m_EtaROImax[1][29][ 9]=   0.573871;m_PhiROImin[1][29][ 9]=  -0.479445;m_PhiROImax[1][29][ 9]=  -0.395489;
     m_EtaROImin[1][29][10]=    0.42967;m_EtaROImax[1][29][10]=   0.504617;m_PhiROImin[1][29][10]=  -0.565749;m_PhiROImax[1][29][10]=  -0.483076;
     m_EtaROImin[1][29][11]=   0.501681;m_EtaROImax[1][29][11]=   0.573871;m_PhiROImin[1][29][11]=  -0.565749;m_PhiROImax[1][29][11]=  -0.483076;
     m_EtaROImin[1][29][12]=   0.583785;m_EtaROImax[1][29][12]=   0.653329;m_PhiROImin[1][29][12]=  -0.479445;m_PhiROImax[1][29][12]=  -0.395489;
     m_EtaROImin[1][29][13]=   0.649934;m_EtaROImax[1][29][13]=   0.741516;m_PhiROImin[1][29][13]=  -0.479445;m_PhiROImax[1][29][13]=  -0.395489;
     m_EtaROImin[1][29][14]=   0.583785;m_EtaROImax[1][29][14]=   0.653329;m_PhiROImin[1][29][14]=  -0.565749;m_PhiROImax[1][29][14]=  -0.483076;
     m_EtaROImin[1][29][15]=   0.649934;m_EtaROImax[1][29][15]=   0.741516;m_PhiROImin[1][29][15]=  -0.565749;m_PhiROImax[1][29][15]=  -0.483076;
     m_EtaROImin[1][29][16]=   0.756521;m_EtaROImax[1][29][16]=   0.837822;m_PhiROImin[1][29][16]=  -0.479445;m_PhiROImax[1][29][16]=  -0.395489;
     m_EtaROImin[1][29][17]=	    0;m_EtaROImax[1][29][17]=	     0;m_PhiROImin[1][29][17]=	      0;m_PhiROImax[1][29][17]=	       0;
     m_EtaROImin[1][29][18]=   0.756521;m_EtaROImax[1][29][18]=   0.837822;m_PhiROImin[1][29][18]=  -0.565749;m_PhiROImax[1][29][18]=  -0.483076;
     m_EtaROImin[1][29][19]=	    0;m_EtaROImax[1][29][19]=	     0;m_PhiROImin[1][29][19]=	      0;m_PhiROImax[1][29][19]=	       0;
     m_EtaROImin[1][29][20]=   0.844116;m_EtaROImax[1][29][20]=   0.903324;m_PhiROImin[1][29][20]=  -0.479445;m_PhiROImax[1][29][20]=  -0.395489;
     m_EtaROImin[1][29][21]=   0.899344;m_EtaROImax[1][29][21]=   0.956037;m_PhiROImin[1][29][21]=  -0.479445;m_PhiROImax[1][29][21]=  -0.395489;
     m_EtaROImin[1][29][22]=   0.844116;m_EtaROImax[1][29][22]=   0.903324;m_PhiROImin[1][29][22]=  -0.565749;m_PhiROImax[1][29][22]=  -0.483076;
     m_EtaROImin[1][29][23]=   0.899344;m_EtaROImax[1][29][23]=   0.956037;m_PhiROImin[1][29][23]=  -0.565749;m_PhiROImax[1][29][23]=  -0.483076;
     m_EtaROImin[1][29][24]=	    0;m_EtaROImax[1][29][24]=	     0;m_PhiROImin[1][29][24]=	      0;m_PhiROImax[1][29][24]=	       0;
     m_EtaROImin[1][29][25]=	    0;m_EtaROImax[1][29][25]=	     0;m_PhiROImin[1][29][25]=	      0;m_PhiROImax[1][29][25]=	       0;
     m_EtaROImin[1][29][26]=	    0;m_EtaROImax[1][29][26]=	     0;m_PhiROImin[1][29][26]=	      0;m_PhiROImax[1][29][26]=	       0;
     m_EtaROImin[1][29][27]=	    0;m_EtaROImax[1][29][27]=	     0;m_PhiROImin[1][29][27]=	      0;m_PhiROImax[1][29][27]=	       0;
     m_EtaROImin[1][29][28]=	    0;m_EtaROImax[1][29][28]=	     0;m_PhiROImin[1][29][28]=	      0;m_PhiROImax[1][29][28]=	       0;
     m_EtaROImin[1][29][29]=	    0;m_EtaROImax[1][29][29]=	     0;m_PhiROImin[1][29][29]=	      0;m_PhiROImax[1][29][29]=	       0;
     m_EtaROImin[1][29][30]=	    0;m_EtaROImax[1][29][30]=	     0;m_PhiROImin[1][29][30]=	      0;m_PhiROImax[1][29][30]=	       0;
     m_EtaROImin[1][29][31]=	    0;m_EtaROImax[1][29][31]=	     0;m_PhiROImin[1][29][31]=	      0;m_PhiROImax[1][29][31]=	       0;
     m_EtaROImin[1][30][ 0]=   0.116816;m_EtaROImax[1][30][ 0]=   0.213185;m_PhiROImin[1][30][ 0]=  -0.389909;m_PhiROImax[1][30][ 0]=  -0.305953;
     m_EtaROImin[1][30][ 1]=  0.0208251;m_EtaROImax[1][30][ 1]=   0.118734;m_PhiROImin[1][30][ 1]=  -0.389909;m_PhiROImax[1][30][ 1]=  -0.305953;
     m_EtaROImin[1][30][ 2]=   0.116816;m_EtaROImax[1][30][ 2]=   0.213185;m_PhiROImin[1][30][ 2]=  -0.302322;m_PhiROImax[1][30][ 2]=  -0.219649;
     m_EtaROImin[1][30][ 3]=  0.0208251;m_EtaROImax[1][30][ 3]=   0.118734;m_PhiROImin[1][30][ 3]=  -0.302322;m_PhiROImax[1][30][ 3]=  -0.219649;
     m_EtaROImin[1][30][ 4]=    0.30075;m_EtaROImax[1][30][ 4]=   0.416721;m_PhiROImin[1][30][ 4]=  -0.389909;m_PhiROImax[1][30][ 4]=  -0.305953;
     m_EtaROImin[1][30][ 5]=   0.222449;m_EtaROImax[1][30][ 5]=   0.302928;m_PhiROImin[1][30][ 5]=  -0.389909;m_PhiROImax[1][30][ 5]=  -0.305953;
     m_EtaROImin[1][30][ 6]=    0.30075;m_EtaROImax[1][30][ 6]=   0.416721;m_PhiROImin[1][30][ 6]=  -0.302322;m_PhiROImax[1][30][ 6]=  -0.219649;
     m_EtaROImin[1][30][ 7]=   0.222449;m_EtaROImax[1][30][ 7]=   0.302928;m_PhiROImin[1][30][ 7]=  -0.302322;m_PhiROImax[1][30][ 7]=  -0.219649;
     m_EtaROImin[1][30][ 8]=   0.501681;m_EtaROImax[1][30][ 8]=   0.573871;m_PhiROImin[1][30][ 8]=  -0.389909;m_PhiROImax[1][30][ 8]=  -0.305953;
     m_EtaROImin[1][30][ 9]=    0.42967;m_EtaROImax[1][30][ 9]=   0.504617;m_PhiROImin[1][30][ 9]=  -0.389909;m_PhiROImax[1][30][ 9]=  -0.305953;
     m_EtaROImin[1][30][10]=   0.501681;m_EtaROImax[1][30][10]=   0.573871;m_PhiROImin[1][30][10]=  -0.302322;m_PhiROImax[1][30][10]=  -0.219649;
     m_EtaROImin[1][30][11]=    0.42967;m_EtaROImax[1][30][11]=   0.504617;m_PhiROImin[1][30][11]=  -0.302322;m_PhiROImax[1][30][11]=  -0.219649;
     m_EtaROImin[1][30][12]=   0.649934;m_EtaROImax[1][30][12]=   0.741516;m_PhiROImin[1][30][12]=  -0.389909;m_PhiROImax[1][30][12]=  -0.305953;
     m_EtaROImin[1][30][13]=   0.583785;m_EtaROImax[1][30][13]=   0.653329;m_PhiROImin[1][30][13]=  -0.389909;m_PhiROImax[1][30][13]=  -0.305953;
     m_EtaROImin[1][30][14]=   0.649934;m_EtaROImax[1][30][14]=   0.741516;m_PhiROImin[1][30][14]=  -0.302322;m_PhiROImax[1][30][14]=  -0.219649;
     m_EtaROImin[1][30][15]=   0.583785;m_EtaROImax[1][30][15]=   0.653329;m_PhiROImin[1][30][15]=  -0.302322;m_PhiROImax[1][30][15]=  -0.219649;
     m_EtaROImin[1][30][16]=	    0;m_EtaROImax[1][30][16]=	     0;m_PhiROImin[1][30][16]=	      0;m_PhiROImax[1][30][16]=	       0;
     m_EtaROImin[1][30][17]=   0.756521;m_EtaROImax[1][30][17]=   0.837822;m_PhiROImin[1][30][17]=  -0.389909;m_PhiROImax[1][30][17]=  -0.305953;
     m_EtaROImin[1][30][18]=	    0;m_EtaROImax[1][30][18]=	     0;m_PhiROImin[1][30][18]=	      0;m_PhiROImax[1][30][18]=	       0;
     m_EtaROImin[1][30][19]=   0.756521;m_EtaROImax[1][30][19]=   0.837822;m_PhiROImin[1][30][19]=  -0.302322;m_PhiROImax[1][30][19]=  -0.219649;
     m_EtaROImin[1][30][20]=   0.899344;m_EtaROImax[1][30][20]=   0.956037;m_PhiROImin[1][30][20]=  -0.389909;m_PhiROImax[1][30][20]=  -0.305953;
     m_EtaROImin[1][30][21]=   0.844116;m_EtaROImax[1][30][21]=   0.903324;m_PhiROImin[1][30][21]=  -0.389909;m_PhiROImax[1][30][21]=  -0.305953;
     m_EtaROImin[1][30][22]=   0.899344;m_EtaROImax[1][30][22]=   0.956037;m_PhiROImin[1][30][22]=  -0.302322;m_PhiROImax[1][30][22]=  -0.219649;
     m_EtaROImin[1][30][23]=   0.844116;m_EtaROImax[1][30][23]=   0.903324;m_PhiROImin[1][30][23]=  -0.302322;m_PhiROImax[1][30][23]=  -0.219649;
     m_EtaROImin[1][30][24]=	    0;m_EtaROImax[1][30][24]=	     0;m_PhiROImin[1][30][24]=	      0;m_PhiROImax[1][30][24]=	       0;
     m_EtaROImin[1][30][25]=	    0;m_EtaROImax[1][30][25]=	     0;m_PhiROImin[1][30][25]=	      0;m_PhiROImax[1][30][25]=	       0;
     m_EtaROImin[1][30][26]=	    0;m_EtaROImax[1][30][26]=	     0;m_PhiROImin[1][30][26]=	      0;m_PhiROImax[1][30][26]=	       0;
     m_EtaROImin[1][30][27]=	    0;m_EtaROImax[1][30][27]=	     0;m_PhiROImin[1][30][27]=	      0;m_PhiROImax[1][30][27]=	       0;
     m_EtaROImin[1][30][28]=	    0;m_EtaROImax[1][30][28]=	     0;m_PhiROImin[1][30][28]=	      0;m_PhiROImax[1][30][28]=	       0;
     m_EtaROImin[1][30][29]=	    0;m_EtaROImax[1][30][29]=	     0;m_PhiROImin[1][30][29]=	      0;m_PhiROImax[1][30][29]=	       0;
     m_EtaROImin[1][30][30]=	    0;m_EtaROImax[1][30][30]=	     0;m_PhiROImin[1][30][30]=	      0;m_PhiROImax[1][30][30]=	       0;
     m_EtaROImin[1][30][31]=	    0;m_EtaROImax[1][30][31]=	     0;m_PhiROImin[1][30][31]=	      0;m_PhiROImax[1][30][31]=	       0;
     m_EtaROImin[1][31][ 0]=  0.0231199;m_EtaROImax[1][31][ 0]=   0.131749;m_PhiROImin[1][31][ 0]=  -0.112612;m_PhiROImax[1][31][ 0]=-0.00295181;
     m_EtaROImin[1][31][ 1]=   0.130922;m_EtaROImax[1][31][ 1]=   0.238612;m_PhiROImin[1][31][ 1]=  -0.112612;m_PhiROImax[1][31][ 1]=-0.00295181;
     m_EtaROImin[1][31][ 2]=  0.0231199;m_EtaROImax[1][31][ 2]=   0.131749;m_PhiROImin[1][31][ 2]=  -0.223011;m_PhiROImax[1][31][ 2]=  -0.116119;
     m_EtaROImin[1][31][ 3]=   0.130922;m_EtaROImax[1][31][ 3]=   0.238612;m_PhiROImin[1][31][ 3]=  -0.223011;m_PhiROImax[1][31][ 3]=  -0.116119;
     m_EtaROImin[1][31][ 4]=   0.246207;m_EtaROImax[1][31][ 4]=    0.35049;m_PhiROImin[1][31][ 4]=  -0.112612;m_PhiROImax[1][31][ 4]=-0.00295181;
     m_EtaROImin[1][31][ 5]=   0.350831;m_EtaROImax[1][31][ 5]=   0.452099;m_PhiROImin[1][31][ 5]=  -0.112612;m_PhiROImax[1][31][ 5]=-0.00295181;
     m_EtaROImin[1][31][ 6]=   0.246207;m_EtaROImax[1][31][ 6]=    0.35049;m_PhiROImin[1][31][ 6]=  -0.223011;m_PhiROImax[1][31][ 6]=  -0.116119;
     m_EtaROImin[1][31][ 7]=   0.350831;m_EtaROImax[1][31][ 7]=   0.452099;m_PhiROImin[1][31][ 7]=  -0.223011;m_PhiROImax[1][31][ 7]=  -0.116119;
     m_EtaROImin[1][31][ 8]=   0.458091;m_EtaROImax[1][31][ 8]=   0.554444;m_PhiROImin[1][31][ 8]=  -0.112612;m_PhiROImax[1][31][ 8]=-0.00295181;
     m_EtaROImin[1][31][ 9]=   0.555789;m_EtaROImax[1][31][ 9]=   0.648101;m_PhiROImin[1][31][ 9]=  -0.112612;m_PhiROImax[1][31][ 9]=-0.00295181;
     m_EtaROImin[1][31][10]=   0.458091;m_EtaROImax[1][31][10]=   0.554444;m_PhiROImin[1][31][10]=  -0.223011;m_PhiROImax[1][31][10]=  -0.116119;
     m_EtaROImin[1][31][11]=   0.555789;m_EtaROImax[1][31][11]=   0.648101;m_PhiROImin[1][31][11]=  -0.223011;m_PhiROImax[1][31][11]=  -0.116119;
     m_EtaROImin[1][31][12]=   0.655857;m_EtaROImax[1][31][12]=   0.703298;m_PhiROImin[1][31][12]=  -0.113212;m_PhiROImax[1][31][12]=-0.00296767;
     m_EtaROImin[1][31][13]=   0.706413;m_EtaROImax[1][31][13]=   0.776449;m_PhiROImin[1][31][13]=  -0.113212;m_PhiROImax[1][31][13]=-0.00296767;
     m_EtaROImin[1][31][14]=   0.655857;m_EtaROImax[1][31][14]=   0.703298;m_PhiROImin[1][31][14]=  -0.224169;m_PhiROImax[1][31][14]=  -0.116737;
     m_EtaROImin[1][31][15]=   0.706413;m_EtaROImax[1][31][15]=   0.776449;m_PhiROImin[1][31][15]=  -0.224169;m_PhiROImax[1][31][15]=  -0.116737;
     m_EtaROImin[1][31][16]=   0.783867;m_EtaROImax[1][31][16]=    0.82768;m_PhiROImin[1][31][16]=  -0.113212;m_PhiROImax[1][31][16]=-0.00296767;
     m_EtaROImin[1][31][17]=   0.830556;m_EtaROImax[1][31][17]=   0.895163;m_PhiROImin[1][31][17]=  -0.113212;m_PhiROImax[1][31][17]=-0.00296767;
     m_EtaROImin[1][31][18]=   0.783867;m_EtaROImax[1][31][18]=    0.82768;m_PhiROImin[1][31][18]=  -0.224169;m_PhiROImax[1][31][18]=  -0.116737;
     m_EtaROImin[1][31][19]=   0.830556;m_EtaROImax[1][31][19]=   0.895163;m_PhiROImin[1][31][19]=  -0.224169;m_PhiROImax[1][31][19]=  -0.116737;
     m_EtaROImin[1][31][20]=   0.898201;m_EtaROImax[1][31][20]=   0.961344;m_PhiROImin[1][31][20]=  -0.112612;m_PhiROImax[1][31][20]=-0.00295181;
     m_EtaROImin[1][31][21]=   0.964674;m_EtaROImax[1][31][21]=    1.02514;m_PhiROImin[1][31][21]=  -0.112612;m_PhiROImax[1][31][21]=-0.00295181;
     m_EtaROImin[1][31][22]=   0.898201;m_EtaROImax[1][31][22]=   0.961344;m_PhiROImin[1][31][22]=  -0.223011;m_PhiROImax[1][31][22]=  -0.116119;
     m_EtaROImin[1][31][23]=   0.964674;m_EtaROImax[1][31][23]=    1.02514;m_PhiROImin[1][31][23]=  -0.223011;m_PhiROImax[1][31][23]=  -0.116119;
     m_EtaROImin[1][31][24]=    1.03003;m_EtaROImax[1][31][24]=    1.06547;m_PhiROImin[1][31][24]=  -0.113429;m_PhiROImax[1][31][24]=-0.00306876;
     m_EtaROImin[1][31][25]=	    0;m_EtaROImax[1][31][25]=	     0;m_PhiROImin[1][31][25]=	      0;m_PhiROImax[1][31][25]=	       0;
     m_EtaROImin[1][31][26]=    1.03003;m_EtaROImax[1][31][26]=    1.06547;m_PhiROImin[1][31][26]=  -0.197143;m_PhiROImax[1][31][26]=  -0.116958;
     m_EtaROImin[1][31][27]=	    0;m_EtaROImax[1][31][27]=	     0;m_PhiROImin[1][31][27]=	      0;m_PhiROImax[1][31][27]=	       0;
     m_EtaROImin[1][31][28]=	    0;m_EtaROImax[1][31][28]=	     0;m_PhiROImin[1][31][28]=	      0;m_PhiROImax[1][31][28]=	       0;
     m_EtaROImin[1][31][29]=	    0;m_EtaROImax[1][31][29]=	     0;m_PhiROImin[1][31][29]=	      0;m_PhiROImax[1][31][29]=	       0;
     m_EtaROImin[1][31][30]=	    0;m_EtaROImax[1][31][30]=	     0;m_PhiROImin[1][31][30]=	      0;m_PhiROImax[1][31][30]=	       0;
     m_EtaROImin[1][31][31]=	    0;m_EtaROImax[1][31][31]=	     0;m_PhiROImin[1][31][31]=	      0;m_PhiROImax[1][31][31]=	       0;

  
  return sc;
}
