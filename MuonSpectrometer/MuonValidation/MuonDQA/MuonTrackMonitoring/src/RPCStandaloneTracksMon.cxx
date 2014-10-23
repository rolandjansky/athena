/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "MuonDigitContainer/RpcDigitContainer.h"
 
#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include "MuonTrigCoinData/RpcCoinDataContainer.h"

#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonDQAUtils/MuonCosmicSetup.h"
#include "MuonDQAUtils/MuonDQAHistMap.h" 
 
#include "xAODEventInfo/EventInfo.h" 
 
#include "MuonTrackMonitoring/RPCStandaloneTracksMon.h"
     
#include <fstream> 
#include <sstream>
#include <iostream>     

using namespace std;

static const   int maxColl	      =  1200;
static const   int maxPRD 	      = 50000;
static const   int maxClus	      =  1000;
static const   int timeminrange	      =	    0;
static const   int timemaxrange	      =	  200;
static const   int timeNbin	      =	   64;
static const   int nstripfiducial     =     0;
static const   int nstripfiduceff     =    80;
static const   int MergePointDistance =    50;
static const   int EtaStationSpan     =     2;
static const   int DoublePhiSpan      =     1;
static const   int maxCSres           =     8;
static const float Chi2dofCut         =     1;

/////////////////////////////////////////////////////////////////////////////

RPCStandaloneTracksMon::RPCStandaloneTracksMon( const std::string & type, const std::string & name, const IInterface* parent )
  :ManagedMonitorToolBase( type, name, parent ),
   m_first(true) 
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
  declareProperty("doRadiography",       m_doRadiography        = false ); 
  declareProperty("doCoolDB",	         m_doCoolDB		= false	);
  declareProperty("selectLB",	         m_selectLB		= false	);
  declareProperty("minSelLB",		 m_minSelLB             = 1	);
  declareProperty("maxSelLB",            m_maxSelLB             = 9999	);
  declareProperty("trackOnlySmall",	 m_trOnlySmall 		= false	);
  declareProperty("trackOnlyLarge",	 m_trOnlyLarge 		= false	);
  declareProperty("HPtPointForHPteff",	 m_HPtPointForHPteff 	= true	);
  declareProperty("HPtPointForLPteff",	 m_HPtPointForLPteff 	= true	);
  declareProperty("HPtPointForTracks",	 m_HPtPointForTracks 	= false	);
  
  m_padsId     = 0;
  m_chambersId = 0;
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
 
  rpc_eventstotal=0;
  
  // Clear Muon Monitoring Histograms 
  m_SummaryHist.clear();
  m_SummaryHistRPCBA.clear();
  m_SummaryHistRPCBC.clear();
  m_sumTmpHist.clear();
  rpcAverageSide_A.clear(); 
  rpcAverageSide_C.clear(); 
  
  
  return StatusCode::SUCCESS;
}

/*----------------------------------------------------------------------------------*/
StatusCode RPCStandaloneTracksMon::fillHistograms()
/*----------------------------------------------------------------------------------*/
{
  StatusCode sc = StatusCode::SUCCESS; 
  
  
  ATH_MSG_DEBUG ( "RPCStandaloneTracksMon::RPCStandaloneTracksMon Histograms being filled" );
  if( m_doRpcESD==true ) { if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online ) {  
      const Muon::RpcPrepDataContainer* rpc_container;
      sc = (*m_activeStore)->retrieve(rpc_container, m_key_rpc);
      if (sc.isFailure()) {
	ATH_MSG_ERROR ( " Cannot retrieve RpcPrepDataContainer " << m_key_rpc );
	return sc;
      }

      //select lumiblock for analysis
      if ( m_selectLB ) {
	const DataHandle<xAOD::EventInfo> eventInfo;
	StatusCode sc = m_eventStore->retrieve( eventInfo );
	if (sc.isFailure()) {
	  ATH_MSG_DEBUG ( "no event info" );
	  return StatusCode::SUCCESS;
	}
	else {
	  // skip events outside Lb range selected in jobOpt
	  int lumiBlock = eventInfo->lumiBlock()  ;
	  if ( lumiBlock<m_minSelLB || lumiBlock>m_maxSelLB ) 
	    { 
	      ATH_MSG_INFO ( "event LB " << lumiBlock << " outside range " <<  m_minSelLB <<" "<<m_maxSelLB 
			     << "   -> skip event " ); 
	      return sc ;
	    }  
	}
      }
    
      ATH_MSG_DEBUG ("****** rpc->size() : " << rpc_container->size());  
    
      Muon::RpcPrepDataContainer::const_iterator containerIt;
   
    
      type="RPC";


      // recall general histos  
      m_generic_path_rpcmonitoring = "Muon/MuonTrackMonitoring/NoTrigger/RPCStandAloneTrackMon"	 ;
      MonGroup rpcprd_shift( this, m_generic_path_rpcmonitoring+"/Overview", run, ATTRIB_MANAGED )    ;
      MonGroup rpcprd_expert( this, m_generic_path_rpcmonitoring+"/Overview", run, ATTRIB_MANAGED )   ;
      MonGroup rpcprd_expert_sum( this, m_generic_path_rpcmonitoring+"/Summary", run, ATTRIB_MANAGED)  	 ;
      MonGroup rpc_dqmf_global( this, m_generic_path_rpcmonitoring + "/GLOBAL", run, ATTRIB_MANAGED )  ;
      MonGroup rpcprd_dq_BA( this, m_generic_path_rpcmonitoring + "/RPCBA", run, ATTRIB_MANAGED  )     ;
      MonGroup rpcprd_dq_BC( this, m_generic_path_rpcmonitoring + "/RPCBC", run, ATTRIB_MANAGED  )     ;
      MonGroup rpc_radiography(this, m_generic_path_rpcmonitoring +"/RPCRadiography", run, ATTRIB_MANAGED) ;
      MonGroup rpcCoolDb( this, m_generic_path_rpcmonitoring+"/CoolDB", run, ATTRIB_MANAGED )         ;
       
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
	  Rpc_t_3D.clear()         ;
	  Rpc_avEta_3D.clear()     ;
	  Rpc_avPhi_3D.clear()	   ;
	  Rpc_id_eta_3D.clear()    ;
	  Rpc_id_phi_3D.clear()    ;
	  Rpc_Point.clear()        ;
    
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
		irpc_clus_posx = stripPosC.x() ;
		irpc_clus_posy = stripPosC.y() ;
		irpc_clus_posz = stripPosC.z() ;
            
	    		  
		//get information from geomodel to book and fill rpc histos with the right max strip number
		std::vector<int>   rpcstripshift = RpcStripShift(prd_id, 0)  ;
	    		  
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
		std::vector<std::string>   rpclayersectorsidename = RpcLayerSectorSideName(prd_id, 0)  ;
  	       
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
		  
		    ShiftEtaStripsTot = RpcStripShift(prd_idII, 0)[8]  ;  // angelo 07 oct 2009
		    EtaStripSign      = RpcStripShift(prd_idII, 0)[10] ;  // angelo 07 oct 2009

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
		    irpc_clus_posxII = stripPosCII.x() ;
		    irpc_clus_posyII = stripPosCII.y() ;
		    irpc_clus_poszII = stripPosCII.z() ;
	   
  
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
	      
		    //build 3D Rpc points without trigger hits //select clusters with size 1 or 2 	      
		
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
		      Rpc_z_3D.push_back      (irpc_clus_poszII    )  ;
		      Rpc_t_3D.push_back      (irpc_clus_time	 )  ;
		      Rpc_SL_3D.push_back     (SectorLogic	 )  ;
		      Rpc_Side_3D.push_back   (Side		 )  ;
		      Rpc_id_phi_3D.push_back (prd_id  	         )  ;
		      Rpc_id_eta_3D.push_back (prd_idII  	         )  ;
		      Rpc_avEta_3D.push_back  (int (avstripeta )   )  ;
		      Rpc_avPhi_3D.push_back  (int (avstripphi )   )  ;
		      Amg::Vector3D Point3D(irpc_clus_posx  , irpc_clus_posy  ,  irpc_clus_poszII);
		      Rpc_Point.push_back     (Point3D 	         )  ;
		  
		    
            
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
		if(LayerType.at(i_3D0)==ilayertype &&  ilayertype!=6  )continue;
		if(Rpc_track[ i_3D0 ]>0||PlaneType.at(i_3D0)!=0)continue;//Start always with no-track assigned LowPt plane
		Phi_Rpc_Track = Rpc_Phi_3D[ i_3D0 ]; 
		PointperTrack = 1  ;
		Rpc_track[ i_3D0 ] = N_Rpc_Tracks + 1 ;//preliminary assigned
		//std::cout << "First Clusters 3D " << i_3D0 <<" " <<LayerType.at(i_3D0)<<" "<< (Rpc_Point.at(i_3D0)).x() << " " << (Rpc_Point.at(i_3D0)).y() << " " <<(Rpc_Point.at(i_3D0)).z() <<std::endl ;  
		linkedtrack   = 0 ;
		for (int i_3DI=0;i_3DI!=N_Rpc_Clusters3D;i_3DI++) {
		  if(linkedtrack == 1 ) continue ;
		  if(  abs(Rpc_Eta_3D.at(i_3DI)-Rpc_Eta_3D.at(i_3D0)) > EtaStationSpan )continue;
		  if(  abs(Rpc_Phi_3D.at(i_3DI)-Rpc_Phi_3D.at(i_3D0)) > DoublePhiSpan  )continue;
		  if(LayerType.at(i_3DI)==ilayertype &&  ilayertype!=6  )continue;
		  if(Rpc_track[ i_3DI ]>0||PlaneType.at(i_3DI)!=1)continue;//Second always no-track assigned Pivot plane
		  Rpc_track[ i_3DI ] = N_Rpc_Tracks + 1 ;
		  PointperTrack = 2 ;
		     
		  //std::cout << "Second Clusters 3D " << i_3DI <<" "<<LayerType.at(i_3DI)<<" " <<(Rpc_Point.at(i_3DI)).x() << " " <<(Rpc_Point.at(i_3DI)).y() << " " <<(Rpc_Point.at(i_3DI)).z() <<std::endl ; 
	
		  SegVector = Rpc_Point.at(i_3DI)-Rpc_Point.at(i_3D0) ;	  
		  SegPoint  =			Rpc_Point.at(i_3D0) ;
		  
		  lookforthirdII   = 0 ;
		  thirdlayertypeII = 0 ; 
		  thirdlayerHPt    = 0 ;
		
		  for (int i_3DII=0;i_3DII!=N_Rpc_Clusters3D;i_3DII++) {
		    if(  abs(Rpc_Eta_3D.at(i_3DII)-Rpc_Eta_3D.at(i_3DI)) > EtaStationSpan )continue;
		    if(  abs(Rpc_Phi_3D.at(i_3DII)-Rpc_Phi_3D.at(i_3DI)) > DoublePhiSpan  )continue;
		    if(LayerType.at(i_3DII)==ilayertype &&  ilayertype!=6  )continue;
		    if(Rpc_track[ i_3DII ]>0)continue;//Third no-track assigned LowPt or Pivot or HighPt plane
		  
		    //std::cout << "Third Clusters 3D " << i_3DII <<" "<<LayerType.at(i_3DII)<<" " <<(Rpc_Point.at(i_3DII)).x() << " " <<(Rpc_Point.at(i_3DII)).y() << " "<<(Rpc_Point.at(i_3DII)).z() <<std::endl ; 
		  
		    ImpactVector = (SegPoint-Rpc_Point.at(i_3DII)).cross(SegVector);	    
		    if(SegVector.mag()!=0)ImpactVector = ImpactVector/ SegVector.mag();	   
		    
		    /*
		      std::cout << "ImpactVector "<<ImpactVector.x() << " " <<ImpactVector.y()<< " "<< ImpactVector.z()<<std::endl ;
		      std::cout << "SegVector "<<SegVector.x() << " " <<SegVector.y()<< " "<< SegVector.z()<<std::endl ;
		      std::cout << "SegPoint "<<SegPoint.x() << " " <<SegPoint.y()<< " "<< SegPoint.z()<<std::endl ;
		      std::cout << "Distance " << ImpactVector.mag() <<std::endl ;
		    */
			      
		    if(ImpactVector.mag()<MergePointDistance){
		      //std::cout << "Merge " <<std::endl ;
		      Rpc_track[ i_3DII ] = N_Rpc_Tracks + 1 ;
		      PointperTrack++ ;
		      lookforthirdII = 1 ;
		      if (LayerType.at(i_3DII)!=LayerType.at(i_3DI) && LayerType.at(i_3DII)!=LayerType.at(i_3D0) ){ 
			thirdlayertypeII = 1 ;
			//std::cout << "third layer found " <<std::endl ;
		      }
		      if (LayerType.at(i_3DII)> 3 ){ 
			thirdlayerHPt = 1 ;
			//std::cout << "third layer found with HPt" <<std::endl ;
		      }
		     	    
		    }
		          
		    
		    //merge in costheta not used
		    SegVectorII = Rpc_Point.at(i_3DII)-Rpc_Point.at(i_3D0) ;
		    cosmerge = SegVector.dot(SegVectorII);
		    if(SegVector.mag()*SegVectorII.mag()!=0)cosmerge=cosmerge/(SegVector.mag()*SegVectorII.mag());
						
			
			
		  }//Third
		  //First and Second do not link with different layer type with any Third let free Second
	
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
 
 
		  //std::cout <<  " Start track fitting. with PointperTrack " << PointperTrack  <<std::endl ;
 
		  for (int i_3D=0;i_3D!=N_Rpc_Clusters3D;i_3D++) {

		    if(Rpc_track[ i_3D ] != N_Rpc_Tracks)continue;
		    /*
		      std::cout << i_3D <<" " <<LayerType.at(i_3D)<<" "<< (Rpc_Point.at(i_3D)).x() << " " << (Rpc_Point.at(i_3D)).y() << " " <<(Rpc_Point.at(i_3D)).z() <<std::endl ;
		      std::cout << SmallLarge.at(i_3D)<<std::endl ;
		      std::cout << Rpc_Station_3D.at(i_3D)<<" "<<Rpc_Eta_3D.at(i_3D)<<" "<< Rpc_Phi_3D.at(i_3D)<< std::endl ;
		      std::cout << Rpc_DBLr_3D.at(i_3D)<<" "<<Rpc_DBLz_3D.at(i_3D)<<" "<< Rpc_DBLphi_3D.at(i_3D)<< " "<< Rpc_GasGap_3D.at(i_3D)<< std::endl ;
		    */
 
		    //gap to span to find intersections
		    if(minphi>Rpc_Phi_3D.at(i_3D))minphi=Rpc_Phi_3D.at(i_3D);
		    if(maxphi<Rpc_Phi_3D.at(i_3D))maxphi=Rpc_Phi_3D.at(i_3D);
		    if(mineta>Rpc_Eta_3D.at(i_3D))mineta=Rpc_Eta_3D.at(i_3D);
		    if(maxeta<Rpc_Eta_3D.at(i_3D))maxeta=Rpc_Eta_3D.at(i_3D);
 
		    if(SmallLarge.at( i_3D)==0){
		      if(PlaneType.at( i_3D)==0)NplaneSmall0=1; if(PlaneType.at( i_3D)==1)NplaneSmall1=1; if(PlaneType.at( i_3D)==2)NplaneSmall2=1;
		    }
		    else if(SmallLarge.at( i_3D)==1){
		      if(PlaneType.at( i_3D)==0)NplaneLarge0=1; if(PlaneType.at( i_3D)==1)NplaneLarge1=1; if(PlaneType.at( i_3D)==2)NplaneLarge2=1;
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

				  const MuonGM::RpcReadoutElement* rpc = m_muonMgr->getRpcRElement_fromIdFields(iname, ieta, iphi, ir, iz, idp);
	      
				  if(rpc == NULL )continue;
	
	
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
    
				    Amg::Vector3D Point3DGapG (0.,0.,0.); 
				    Amg::Vector3D norm3DGap   (0.,0.,0.);  
				    Amg::Vector3D Inters3DG   (0.,0.,0.);   
    
				    Point3DGapG = rpc->gasGapPos( iz, idp, ig);
				    norm3DGap   = rpc->normal   (/*     idp, ig*/);
				    Inters3DG   = Poin3RLin + Vect3DCos*(norm3DGap.dot(Point3DGapG-Poin3RLin))/(norm3DGap.dot(Vect3DCos));
  		        
				    
				  
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
                      
				    Poseta1 = rpc->stripPos(ideta1); 
				    Poseta1L = ((rpc->transform(ideta1)).inverse())*Poseta1;
				    PosetaN = rpc->stripPos(idetaN);
				    PosetaNL = ((rpc->transform(ideta1)).inverse())*PosetaN;
				    Inters3DLeta1 = ((rpc->transform(ideta1)).inverse())*Inters3DG;
				    Posphi1  = rpc->stripPos(idphi1);
				    Posphi1L = ((rpc->transform(idphi1)).inverse())*Posphi1;
				    PosphiN  = rpc->stripPos(idphiN);
				    PosphiNL = ((rpc->transform(idphi1)).inverse())*PosphiN;
				    Inters3DLphi1 = ((rpc->transform(idphi1)).inverse())*Inters3DG; 
 				     
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
				      std::vector<int>   rpcstripshift = RpcStripShift(prdcoll_id, 0)  ;
 
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
				      std::vector<std::string>   rpclayersectorsidename = RpcLayerSectorSideName(prdcoll_id, 0)  ;
 
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
					    //  if(Vect3DCosL== NULL ) return StatusCode::SUCCESS ;
					    stripPosCL   = ((rpc->transform(ideta1)).inverse())*stripPosC  ;
					    //  if(stripPosCL== NULL ) return StatusCode::SUCCESS ;
					    Inters3DLCL  = ((rpc->transform(ideta1)).inverse())*Inters3DG  ;
					    // if(Inters3DLCL== NULL ) return StatusCode::SUCCESS ;
					    
					    if(imeasphi==1){
					      Vect3DCosL   = ((rpc->transform(idphi1)).inverse())*Vect3DCosL  ;
					      //  if(Vect3DCosL== NULL ) return StatusCode::SUCCESS ;
					      stripPosCL   = ((rpc->transform(idphi1)).inverse())*stripPosC  ;
					      //  if(stripPosCL== NULL ) return StatusCode::SUCCESS ;
					      Inters3DLCL  = ((rpc->transform(idphi1)).inverse())*Inters3DG  ;
					      //  if(Inters3DLCL== NULL ) return StatusCode::SUCCESS ;
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
			   
						incAngle = norm3DGap.mag() *  Vect3DCos.mag() + norm3DGap.z() *  Vect3DCos.z() ;
						norm1 = sqrt(norm3DGap.mag()*norm3DGap.mag()+norm3DGap.z()*norm3DGap.z()) ;
						norm2 = sqrt(Vect3DCos.mag()*Vect3DCos.mag()+Vect3DCos.z()*Vect3DCos.z()) ;
			  			   
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

	
    
						foundonehiteta = 1 ;
						PanelIndexeta  = PanelIndex ;
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
			   
						//incidence angle of tracks on surface (z,mag())
			   
						incAngle = norm3DGap.mag() *  Vect3DCos.mag() + norm3DGap.z() *  Vect3DCos.z() ;
						norm1 = sqrt(norm3DGap.mag()*norm3DGap.mag()+norm3DGap.z()*norm3DGap.z()) ;
						norm2 = sqrt(Vect3DCos.mag()*Vect3DCos.mag()+Vect3DCos.z()*Vect3DCos.z()) ;
			  			   
						if(norm1*norm2>0)incAngle = incAngle / norm1 / norm2 ;
			  			  		  
						sc = rpcprd_shift.getHist(m_rpcCS_angleLong, "CSvsLongitudinalAngleTrack") ;
						if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't get CSvsLongitudinalAngleTrack  hist in EtaView" );
						if ( m_rpcCS_angleLong ) m_rpcCS_angleLong->Fill( incAngle, irpc_clus_size );
		    	  
						foundonehitphi =	       1 ;
						PanelIndexphi  = PanelIndex	 ;
						StripIndexphi  = strip_dbindex ;
					      }}

					  
					    // 					  if ( m_doRadiography ) {
					    // 					    if(foundonehitphi==1 && foundonehiteta==1 && imeasphi==1 && foundEtaPhi==0) {
					    // 					      sc = rpc_radiography.getHist(rpcSectorLayerResponse,  sector_name+"_"+layeronly_name+"_Response" ); 
					    // 					      if (rpcSectorLayerResponse) {
					    // 					       rpcSectorLayerResponse->Fill(stripetaatlas, stripphisector  );
					    // 					       float a1 =  rpcSectorLayerResponse->GetBinContent(stripetaatlas, stripphisector  );
					    // 					       float a2 =  rpcSectorLayerTrackProj->GetBinContent(stripetaatlas, stripphisector  );
					    // 					       if(a2<a1)std::cout << " WARNING DIAMOND AND" <<std::endl;
					    // 					      }
					    // 					      else {  ATH_MSG_DEBUG ( "rpcSectorLayerResponse not in hist list!" );}
					    // 					      foundEtaPhi=1;
					    // 					     }
					    // 					     if ( (foundonehitphi==1 || foundonehiteta==1 ) && imeasphi==1  && foundEtaOrPhi==0 ) {
					    // 					      sc = rpc_radiography.getHist(rpcSectorLayerResponseOR, sector_name+"_"+layeronly_name+"_ResponseEtaOrPhi" ); 
					    // 					      if ( rpcSectorLayerResponseOR ) { 
					    // 					       rpcSectorLayerResponseOR->Fill(stripetaatlas, stripphisector  );
					    // 					       float a1 =  rpcSectorLayerResponseOR->GetBinContent(stripetaatlas, stripphisector  );
					    // 					       float a2 =  rpcSectorLayerTrackProj->GetBinContent(stripetaatlas, stripphisector  );
					    // 					       if(a2<a1)std::cout << " WARNING DIAMOND OR" <<std::endl;
					    // 					      }
					    // 					      else {  ATH_MSG_DEBUG ( "rpcSectorLayerResponseEtaOrPhi not in hist list!" );}
					    // 					      foundEtaOrPhi=1;
					    // 					     }
					    // 					   }  
   
					    res_th = nstripfiduceff ;

					  }}}//end loop on cluster
    
  
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
				  
				  
				  
    
				  } } } } } } } }//end look for gap intersection

   
  
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
		const MuonGM::RpcReadoutElement* descriptor = m_muonMgr->getRpcReadoutElement(prd_id);
	      
		const Amg::Vector3D stripPosC = descriptor->stripPos(prd_id);
	      
		irpc_clus_posx = stripPosC.x() ;
		irpc_clus_posy = stripPosC.y() ;
		irpc_clus_posz = stripPosC.z() ;
	
  
		//get information from geomodel to book and fill rpc histos with the right max strip number
		std::vector<int>   rpcstripshift = RpcStripShift(prd_id, 0)  ;
	    		  
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
		std::vector<std::string>   rpclayersectorsidename = RpcLayerSectorSideName(prd_id, 0)  ;
  	       
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
	      
	      
		irpc_clus_measphi  =  m_rpcIdHelper->measuresPhi(prd_id)  ;	
		
		const MuonGM::RpcReadoutElement* rpc = 
		  m_muonMgr->getRpcRElement_fromIdFields(irpc_clus_station, irpc_clus_eta, irpc_clus_phi, 
							 irpc_clus_doublr, irpc_clus_doublz, irpc_clus_doublphi);

		if(rpc == NULL )continue;
	 
		//		Identifier idr = rpc->identify();
		  
		Amg::Vector3D  Point3DGapG = rpc->gasGapPos(irpc_clus_doublz, irpc_clus_doublphi, irpc_clus_gasgap  );
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
		    (norm3DGap.dot(Point3DGapG-RpcTrack_Poin3RLin .at(i_RpcTrack)))/
		    (norm3DGap.dot(RpcTrack_Vect3DCos .at(i_RpcTrack) ));
	       
		  Inters3DL	 = ((rpc->transform(prd_id)).inverse())* Inters3DG   ;  				    
		  //if(  	Inters3DL       == NULL ) return StatusCode::SUCCESS;	     
		  stripPosCL	 = ((rpc->transform(prd_id)).inverse())* stripPosC   ;				    
		  // if(  	 stripPosCL       == NULL ) return StatusCode::SUCCESS;
                  
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
StatusCode RPCStandaloneTracksMon::bookHistograms()
/*----------------------------------------------------------------------------------*/
{

  ATH_MSG_DEBUG ( "RPCStandaloneTracksMon Histograms being booked" );
 
  StatusCode sc = StatusCode::SUCCESS; 
  if( m_doRpcESD==true ) {if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online ) {       
      //declare a group of histograms
      std::string m_generic_path_rpcmonitoring = "Muon/MuonTrackMonitoring/NoTrigger/RPCStandAloneTrackMon";
      MonGroup rpcprd_shift( this, m_generic_path_rpcmonitoring+"/Overview", run, ATTRIB_MANAGED )      ;
      MonGroup rpcprd_expert( this, m_generic_path_rpcmonitoring+"/Overview", run, ATTRIB_MANAGED )     ;
      MonGroup rpcprd_expert_sum( this, m_generic_path_rpcmonitoring +"/Summary", run, ATTRIB_MANAGED ) ;
      MonGroup rpc_dqmf_global( this, m_generic_path_rpcmonitoring + "/GLOBAL", run, ATTRIB_MANAGED  )    ;
      MonGroup rpcprd_dq_BA( this, m_generic_path_rpcmonitoring + "/RPCBA", run, ATTRIB_MANAGED  )       ;
      MonGroup rpcprd_dq_BC( this, m_generic_path_rpcmonitoring + "/RPCBC", run, ATTRIB_MANAGED  )        ;
      MonGroup rpc_radiography(this, m_generic_path_rpcmonitoring +"/RPCRadiography", run, ATTRIB_MANAGED ) ;
    
      if(newEventsBlock){}
      if(newLumiBlock){}
      if(newRun)
	{      
	  ATH_MSG_INFO ( "RPCStandaloneTracksMon : begin of run" );
	
	
	
	
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
	    ATH_MSG_DEBUG ( "RUN : " << run );
	    ATH_MSG_DEBUG ( "Booked bookrpctrack_b successfully" );}
     
     
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
	    ATH_MSG_DEBUG ( "RUN : " << run );	       
	    ATH_MSG_DEBUG ( "Booked bookLayerHitOnTrack successfully" );} 
	
	  //////////////////////////////////////
	  // Booking summary plot
	  // declare group of summary histograms per readout strip panels
	 	  
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
	    n_eta_station = ( (i_sec+1) %2 ) + 6 + 1 ;  // small: 1-6, large: 1-7, sec 12+14: 0-8
	    if ( (i_sec+1)==12 || (i_sec+1)==14 ) n_eta_station = 12 ;
	 
	    int n_bin = 72 * n_eta_station   ;  
	    //  72 = 2*2*2*3*3  measPhi + gap + dblPhi + dblZ + stationName + stationEta
	  
	    std::string PanelIndexTitle = "# Panel = View + (gap-1)*2 + (dblPhi-1)*4 + (dblZ-1)*8 + (LowPt=0,Pivot=1,HighPt=2)*24 + (Eta)*72";
	  
	    // book panelID histograms
	    std::string m_SummaryPanelID_title      = "SummaryPanelID_" + sector_name ;
	    const char* m_SummaryPanelID_title_char = m_SummaryPanelID_title.c_str()  ;
	  
	    TH1* SummaryPanelID = new TH1I(m_SummaryPanelID_title_char, m_SummaryPanelID_title_char, 2*n_bin, -n_bin, n_bin);
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
	    for (int iname=2; iname!=10+1; iname++ ){
	      if ( (i_sec%2 == 1) && (iname==2 || iname==4) ) continue;  /* small sectors */
	      if ( (i_sec%2 == 0) && (iname!=2 && iname!=4) ) continue;  /* large sectors */
	      for(int ieta=0; ieta!= 17; ieta++){
		for(int ir=0; ir!= 2; ir++){
		  for(int idbz=0; idbz!= 3; idbz++){
		    for (int idbphi=0; idbphi!=2; idbphi++) {
		      for ( int imeasphi=0; imeasphi!=2; imeasphi++ ) {
			for (int igap=0; igap!=2; igap++) {
			  const MuonGM::RpcReadoutElement* rpc = m_muonMgr->getRpcReadoutElement(iname-2, ieta, int(i_sec/2), ir, idbz);
			  if ( rpc != NULL ) {
			    // Identifier gapID(int stationName, int stationEta, int stationPhi, int doubletR, int doubletZ, int doubletPhi,int gasGap, bool check=false, bool* isValid=0) const;
			    // Identifier panelID  (int stationName, int stationEta, int stationPhi, int doubletR, int doubletZ, int doubletPhi,int gasGap, int measuresPhi, bool check=false, bool* isValid=0) const;
			    panelId = m_rpcIdHelper->panelID(iname, ieta-8, int(i_sec/2)+1, ir+1, idbz+1, idbphi+1, igap+1, imeasphi) ; 
			    indexplane = ir ;
			    if ( iname!=2 && iname!=3 && iname!=8 )  indexplane+= 2 ;
			    panelBin = imeasphi + igap*2 + idbphi*4 + idbz*8 + indexplane*24 + abs(ieta-8)*72 ;
			    if ( iname == 10 ) {
			      // convention: BOG <-> doubletZ=3 <-> (3-1)*8=16
			      panelBin = imeasphi + igap*2 + idbphi*4 + 16 + indexplane*24 + abs(ieta-8)*72 ;
			    }
			    if ( (ir==1) && (iname==9 || iname==10) ) {
			      // convention: chambers of RPC upgrade -> eta = eta + 7
			      panelBin = panelBin + 7*72 ;
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
	      ATH_MSG_DEBUG ( "RUN : " << run );}
	
	    sc = rpcprd_expert_sum.regHist( SummaryTimeDistriPerSector ) ;
	    if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register  SummaryTimeDistriPerSector hist to MonGroup" );    
	 	  
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
  std::string m_generic_path_rpcmonitoring = "Muon/MuonTrackMonitoring/NoTrigger/RPCStandAloneTrackMon";
     
  MonGroup rpcprd_dq_BA ( this, m_generic_path_rpcmonitoring + "/RPCBA" , run, ATTRIB_MANAGED );
  MonGroup rpcprd_dq_BC ( this, m_generic_path_rpcmonitoring + "/RPCBC" , run, ATTRIB_MANAGED );
		
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
    ATH_MSG_DEBUG ( "RUN : " << run );	       
    ATH_MSG_DEBUG ( "Booked bookrpcAverageTime_A successfully" );}      
    
}
    
// Radiography histograms
void RPCStandaloneTracksMon::bookRPCLayerRadiographyHistograms( int m_isec, std::string m_layer_name )
{
  gErrorIgnoreLevel=kInfo;
  StatusCode sc = StatusCode::SUCCESS;
  {  ATH_MSG_DEBUG ( "bookRPCLayerRadiographyHistograms" ); }
   
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
      const MuonGM::RpcReadoutElement* rpc = m_muonMgr->getRpcRElement_fromIdFields(iName, 1, istatPhi, ir, 1, 1);
 
      if(rpc != NULL ){
	Identifier idr = rpc->identify();
	std::vector<int>   rpcstripshift = RpcStripShift(idr, 0)  ;
	NphiStrips         =  rpcstripshift[0] ;
	NetaStripsTotSideA =  rpcstripshift[6] ;
	NetaStripsTotSideC =  rpcstripshift[7] ;
    
      }
    
      std::string m_generic_path_rpcmonitoring = "Muon/MuonTrackMonitoring/NoTrigger/RPCStandAloneTrackMon";
      MonGroup rpc_radiography(this, m_generic_path_rpcmonitoring + "/RPCRadiography", run, ATTRIB_MANAGED ) ;
    
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
      if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register " << m_rpcSectorLayerProj_title << " hist to MonGroup" );
    
      rpcSectorLayerTrackProj->SetOption("COLZ");
      rpcSectorLayerTrackProj->GetXaxis()->SetTitle("<--- SIDE C      Rpc Eta strip      SIDE A --->");
      if ( (m_isec%2)== 0 ) { rpcSectorLayerTrackProj->GetYaxis()->SetTitle("<--- HV side     Rpc Phi strip     RO side --->"); }  // large
      else		  { rpcSectorLayerTrackProj->GetYaxis()->SetTitle("<--- RO side     Rpc Phi strip     HV side --->"); }
    
    
      std::string m_rpcSectorLayerResponse_title      = m_sector_name + "_" + m_layer_name + "_Response" ;
      const char* m_rpcSectorLayerResponse_title_char = m_rpcSectorLayerResponse_title.c_str();
 
      TH2 * rpcSectorLayerResponse = new TH2I( m_rpcSectorLayerResponse_title_char, m_rpcSectorLayerResponse_title_char ,binz, binminz, binmaxz, binx, binminx, binmaxx);
 
      sc = rpc_radiography.regHist( rpcSectorLayerResponse );
      if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register " << m_rpcSectorLayerResponse_title << " hist to MonGroup" );
    
      rpcSectorLayerResponse->SetOption("COLZ");
      rpcSectorLayerResponse->GetXaxis()->SetTitle("<--- SIDE C	   Rpc Eta strip      SIDE A --->");
      if ( (m_isec%2) == 0) { rpcSectorLayerResponse->GetYaxis()->SetTitle("<--- HV side     Rpc Phi strip     RO side --->"); }  // large
      else	          { rpcSectorLayerResponse->GetYaxis()->SetTitle("<--- RO side     Rpc Phi strip     HV side --->"); }
    
      // eta OR phi response
      std::string m_rpcSectorLayerResponseOR_title      = m_sector_name + "_" + m_layer_name + "_ResponseEtaOrPhi" ;
      const char* m_rpcSectorLayerResponseOR_title_char = m_rpcSectorLayerResponseOR_title.c_str();
 
      TH2 * rpcSectorLayerResponseOR = new TH2I( m_rpcSectorLayerResponseOR_title_char, m_rpcSectorLayerResponseOR_title_char ,binz, binminz, binmaxz, binx, binminx, binmaxx);
 
      sc = rpc_radiography.regHist( rpcSectorLayerResponseOR );
      if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register " << m_rpcSectorLayerResponseOR_title << " hist to MonGroup" );
    
      rpcSectorLayerResponseOR->SetOption("COLZ");
      rpcSectorLayerResponseOR->GetXaxis()->SetTitle("<--- SIDE C	   Rpc Eta strip      SIDE A --->");
      if ( (m_isec%2) == 0) { rpcSectorLayerResponseOR->GetYaxis()->SetTitle("<--- HV side     Rpc Phi strip     RO side --->"); }  // large
      else	          { rpcSectorLayerResponse->GetYaxis()->SetTitle("<--- RO side     Rpc Phi strip     HV side --->"); }
    
    
    
      //ATLAS noise 
      std::string m_rpcSectorLayerNoiseCorr_title      = m_sector_name + "_" + m_layer_name + "_NoiseCorr" ;
      const char* m_rpcSectorLayerNoiseCorr_title_char = m_rpcSectorLayerNoiseCorr_title.c_str();
   
      TH2 * rpcSectorLayerNoiseCorr = new TH2I( m_rpcSectorLayerNoiseCorr_title_char, m_rpcSectorLayerNoiseCorr_title_char ,binz, binminz, binmaxz, binx, binminx, binmaxx);
 
      sc = rpc_radiography.regHist( rpcSectorLayerNoiseCorr );
      if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register " << m_rpcSectorLayerNoiseCorr_title << " hist to MonGroup" );
    
      rpcSectorLayerNoiseCorr->SetOption("COLZ");
      rpcSectorLayerNoiseCorr->GetXaxis()->SetTitle("<--- SIDE C	   Rpc Eta strip      SIDE A --->");
      if ( (m_isec%2) == 0) { rpcSectorLayerNoiseCorr->GetYaxis()->SetTitle("<--- HV side     Rpc Phi strip     RO side --->"); }  // large
      else	          { rpcSectorLayerNoiseCorr->GetYaxis()->SetTitle("<--- RO side     Rpc Phi strip     HV side --->"); }
    
    
      std::string m_rpcSectorLayerNoiseNotCorr_title      = m_sector_name + "_" + m_layer_name + "_NoiseNotCorr" ;
      const char* m_rpcSectorLayerNoiseNotCorr_title_char = m_rpcSectorLayerNoiseNotCorr_title.c_str();
   
      TH2 * rpcSectorLayerNoiseNotCorr = new TH2I( m_rpcSectorLayerNoiseNotCorr_title_char, m_rpcSectorLayerNoiseNotCorr_title_char ,binz, binminz, binmaxz, binx, binminx, binmaxx);
 
      sc = rpc_radiography.regHist( rpcSectorLayerNoiseNotCorr );
      if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register " << m_rpcSectorLayerNoiseNotCorr_title << " hist to MonGroup" );
    
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
  std::string m_generic_path_rpcmonitoring = "Muon/MuonTrackMonitoring/NoTrigger/RPCStandAloneTrackMon";
  MonGroup rpcCoolDb( this, m_generic_path_rpcmonitoring+"/CoolDB", run, ATTRIB_MANAGED );
  
  //sprintf(histName_char,"Sector%.2d_dblPhi%d_Pivot0", m_isec+1, m_idblPhi+1) ;
  sprintf(histName_char,"Sector%.2d_%s_dblPhi%d", m_isec+1, m_layer.c_str(), m_idblPhi+1) ;
  
  histName  = histName_char  ;
  histName += "_"            ;
  histName += *m_iter        ;  //histName += m_coolQuantity ;
  istatPhi  = int( m_isec/2) ;
  iName     = 0              ;
 
  if ( m_isec!=11 &&  m_isec!=13) {
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
  else {
    if ( m_layer == "Pivot0" || m_layer == "Pivot1" )   {
      iName = 8 ;
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
  const MuonGM::RpcReadoutElement* rpc = m_muonMgr->getRpcRElement_fromIdFields( iName, 1 , istatPhi+1, ir, 1, m_idblPhi+1 );
  if(rpc != NULL ){
    Identifier idr = rpc->identify();
    std::vector<int>   rpcstripshift = RpcStripShift(idr, 0)  ;
    NTotStripsSideA = rpcstripshift[6]+rpcstripshift[17];
    NTotStripsSideC = rpcstripshift[7]+rpcstripshift[18];
  }
  TH1 *rpcCoolHisto = new TH1F(histName.c_str(), histName.c_str(), NTotStripsSideC+NTotStripsSideA, -NTotStripsSideC, NTotStripsSideA );
  sc=rpcCoolDb.regHist(rpcCoolHisto) ;
  if(sc.isFailure() )  ATH_MSG_WARNING ( "couldn't register " << histName << "hist to MonGroup" );
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
  std::string m_generic_path_rpcmonitoring = "Muon/MuonTrackMonitoring/NoTrigger/RPCStandAloneTrackMon";
  MonGroup rpcCoolDb( this, m_generic_path_rpcmonitoring+"/CoolDB", run, ATTRIB_MANAGED );
   
  //sprintf(histName_char,"Sector%.2d_dblPhi%d_Pivot0", m_isec+1, m_idblPhi+1) ;
  sprintf(histName_char,"Sector%.2d_%s_dblPhi%d", m_isec+1, m_layer.c_str(), m_idblPhi+1) ;
     
  histName  = histName_char  ;
  histName += "_"            ;
  histName += *m_iter        ;  //histName += m_coolQuantity ;
  istatPhi  = int( m_isec/2) ;
  iName     = 0              ;
  
  if ( m_isec!=11 &&  m_isec!=13) {
    // if ( m_layer.find("Pivot",0) )
    if ( m_layer == "Pivot0" || m_layer == "Pivot1" )	{
      iName = 2 + (m_isec%2 ) ;
      ir    = 2 	      ; 	
    }
    if ( m_layer == "LowPt0" || m_layer == "LowPt1" )	{
      iName = 2 + (m_isec%2 ) ;
      ir    = 1 	      ;
    }
    if ( m_layer == "HighPt0" || m_layer == "HighPt1" ) {
      iName = 4 + (m_isec%2 ) ;
      ir    = 1 	      ;
    }
  }
  else {
    if ( m_layer == "Pivot0" || m_layer == "Pivot1" )	{
      iName = 8 ;
      ir    = 2 ;   
    }
    if ( m_layer == "LowPt0" || m_layer == "LowPt1" )	{
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
  const MuonGM::RpcReadoutElement* rpc = m_muonMgr->getRpcRElement_fromIdFields( iName, 1 , istatPhi+1, ir, 1, m_idblPhi+1 );
  if(rpc != NULL ){
    Identifier idr = rpc->identify();
    std::vector<int>   rpcstripshift = RpcStripShift(idr, 0)  ;
    NTotStripsSideA = rpcstripshift[6]+rpcstripshift[17];
    NTotStripsSideC = rpcstripshift[7]+rpcstripshift[18];
  }

  
  bool doRecursiveReferenceDelete = gROOT->MustClean();
  gROOT->SetMustClean(false);
    
  TH1 *rpcCoolHisto = new TH1F(histName.c_str(), histName.c_str(), NTotStripsSideC+NTotStripsSideA, -NTotStripsSideC, NTotStripsSideA );
  sc = rpcCoolDb.regHist(rpcCoolHisto) ;
  if(sc.isFailure() )  ATH_MSG_INFO ( "couldn't register " << histName << " hist to MonGroup" );
  rpcCoolHisto->GetXaxis()->SetTitle("strip");
  
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
  if( m_doRpcESD==true ) {if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online ) {    		    
      //declare a group of histograms
      std::string m_generic_path_rpcmonitoring = "Muon/MuonTrackMonitoring/NoTrigger/RPCStandAloneTrackMon";
      MonGroup rpcprd_expert( this, m_generic_path_rpcmonitoring+"/Chambers/"+m_hardware_name+"/Tracks/", run, ATTRIB_MANAGED );   
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
  
  std::string m_generic_path_rpcmonitoring = "Muon/MuonTrackMonitoring/NoTrigger/RPCStandAloneTrackMon";
  MonGroup rpcprd_expert_sum( this, m_generic_path_rpcmonitoring +"/Summary", run, ATTRIB_MANAGED ) ;
   
  char sector_char[10]    ;
  std::string sector_name ;
  sprintf(sector_char, "Sector%.2d", m_i_sec+1) ;  // sector number with 2 digits
  sector_name = sector_char ;
  // large sectors: (m_i_sec+1)%2 = 1;  small (m_i_sec+1)%2 = 0
  n_eta_station = ( (m_i_sec+1) %2 ) + 6 + 1 ;  // small: 1-6, large: 1-7, sec 12+14: 0-8
  if ( (m_i_sec+1)==12 || (m_i_sec+1)==14 ) n_eta_station = 12 ;
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
  
  MonGroup rpcprd_dq_BA      ( this, m_generic_path_rpcmonitoring + "/RPCBA" , run, ATTRIB_MANAGED );
  MonGroup rpcprd_dq_BC      ( this, m_generic_path_rpcmonitoring + "/RPCBC" , run, ATTRIB_MANAGED ); 
     
 
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
  std::string m_generic_path_rpcmonitoring = "Muon/MuonTrackMonitoring/NoTrigger/RPCStandAloneTrackMon";
  MonGroup rpcprd_expert_sum( this, m_generic_path_rpcmonitoring +"/Summary", run, ATTRIB_MANAGED ) ;
  
  ATH_MSG_DEBUG ( "Start bookRPCSummaryFinalHistograms: " << m_i_sec << " " << m_quantity ); 
  
  char sector_char[10]    ;
  std::string sector_name ;
  sprintf(sector_char, "Sector%.2d", m_i_sec+1) ;  // sector number with 2 digits
  sector_name = sector_char ;
  // large sectors: (m_i_sec+1)%2 = 1;  small (m_i_sec+1)%2 = 0
  n_eta_station = ( (m_i_sec+1) %2 ) + 6 + 1 ;  // small: 1-6, large: 1-7, sec 12+14: 0-8
  if ( (m_i_sec+1)==12 || (m_i_sec+1)==14 ) n_eta_station = 12 ;
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

StatusCode RPCStandaloneTracksMon::procHistograms()
{
  StatusCode sc = StatusCode::SUCCESS;
  
  return sc; 
  
  {
    ATH_MSG_DEBUG ( "********Reached Last Event in RPCStandaloneTracksMon !!!" );	  
    ATH_MSG_DEBUG ( "RPCStandaloneTracksMon finalize()" ); 	  
  }
  if( m_doRpcESD==true ) {if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0ESD || m_environment == AthenaMonManager::online ) {    
      std::string m_generic_path_rpcmonitoring = "Muon/MuonTrackMonitoring/NoTrigger/RPCStandAloneTrackMon" ;	
      MonGroup rpcprd_expert     ( this, m_generic_path_rpcmonitoring+"/Overview", run, ATTRIB_MANAGED );
      MonGroup rpcprd_shift      ( this, m_generic_path_rpcmonitoring+"/Overview", run, ATTRIB_MANAGED );
      MonGroup rpc_dqmf_global   ( this, m_generic_path_rpcmonitoring+"/GLOBAL"  , run, ATTRIB_MANAGED );
      MonGroup rpcprd_expert_sum ( this, m_generic_path_rpcmonitoring + "/Summary" , run, ATTRIB_MANAGED );
      MonGroup rpcprd_dq_BA      ( this, m_generic_path_rpcmonitoring + "/RPCBA" , run, ATTRIB_MANAGED );
      MonGroup rpcprd_dq_BC      ( this, m_generic_path_rpcmonitoring + "/RPCBC" , run, ATTRIB_MANAGED );
    
      if(endOfEventsBlock){}
      if(endOfLumiBlock){}
      if(endOfRun){        
      
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
 
 
std::vector<int>  RPCStandaloneTracksMon::RpcStripShift(Identifier prdcoll_id, int  irpctriggerInfo) 
{

  ATH_MSG_DEBUG ( "in RPCStandaloneTracksMon::RpcStripShift" );  
     
  int irpcstationPhi	 =   int(m_rpcIdHelper->stationPhi(prdcoll_id))   ;		     
  int irpcstationName	 =   int(m_rpcIdHelper->stationName(prdcoll_id))  ;	     
  int irpcstationEta	 =   int(m_rpcIdHelper->stationEta(prdcoll_id))   ;			     
  int irpcdoubletR  	 =   int(m_rpcIdHelper->doubletR(prdcoll_id))	  ;		     
  int irpcdoubletZ  	 =   int(m_rpcIdHelper->doubletZ(prdcoll_id))	  ;
  int irpcdoubletPhi	 =   int(m_rpcIdHelper->doubletPhi(prdcoll_id))   ;
  int irpcgasGap	 =   int(m_rpcIdHelper->gasGap(prdcoll_id))	  ;
  int irpcmeasuresPhi	 =   int(m_rpcIdHelper->measuresPhi(prdcoll_id))  ;
  int irpcstrip		 =   int(m_rpcIdHelper->strip(prdcoll_id))	  ;
   
  //get information from geomodel to book and fill rpc histos with the right max strip number
  
  const MuonGM::RpcReadoutElement* descriptor = m_muonMgr->getRpcReadoutElement(prdcoll_id);
  
  // const MuonGM::RpcReadoutElement* rpc = m_muonMgr->getRpcReadoutElement(irpcstationName-2, irpcstationEta  + 8,  irpcstationPhi-1, irpcdoubletR -1,irpcdoubletZ   -1);
  //  const MuonGM::RpcReadoutElement* rpc = m_muonMgr->getRpcRElement_fromIdFields( irpcstationName, irpcstationEta, irpcstationPhi, irpcdoubletR, irpcdoubletZ, irpcdoubletPhi  );
  
  //Identifier idr = rpc->identify();
  
  std::vector<int>  rpcstriptot  ;
  
  int NphiStrips	   = descriptor -> NphiStrips()  *  2		     ;
  int ShiftPhiStrips       = descriptor -> NphiStrips()  * (irpcdoubletPhi  -1 ) ;
  int NetaStrips	   = 0						     ;
  int ShiftEtaStrips       = 0						     ;
  int ShiftStrips	   = 0						     ;
  int NetaStripsTot        = 0						     ;
  int NetaStripsTotSideA   = 0						     ;
  int NetaStripsTotSideC   = 0						     ;
  int ShiftEtaStripsTot    = 0						     ;
  int Nbin  	           = 0						     ;
  int EtaStripSign         = 0						     ;
  int SectorLogic	   = 0						     ;
  int Side  	           = 0						     ;
  int PanelIndex	   = 0						     ;
  int Settore	           = 0						     ;
  int PlaneTipo	           = 0						     ;
  int strip_dbindex        = 0						     ;
  int ShiftPhiTot_db       = 0						     ;
  int NphiStripsTotSideA   = 0					             ;
  int NphiStripsTotSideC   = 0					             ;
  
  //evaluate sector logic
  //2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG
  SectorLogic  = (irpcstationPhi - 1) * 4 ; 
  SectorLogic +=  irpcdoubletPhi          ;
  Settore      = (irpcstationPhi -1) * 2  ;
  if(irpcstationName==3||irpcstationName==5||irpcstationName>7){
    SectorLogic+=2;
    Settore++; 
  }
  SectorLogic   = SectorLogic - 2  ; 
  Settore = Settore +1 ;
  if(SectorLogic==-1)SectorLogic=31;
  if(irpcstationEta>0){Side = 1     ;SectorLogic+=32;}
  
  //2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG
  if( (irpcstationName>1) && (irpcstationName<4||irpcstationName==8) ){
    if(irpcdoubletR==1){
      PlaneTipo=0;
      if(irpcstationName==2&&abs(irpcstationEta)==7)layer_name="Pivot";
      if(irpcstationName==2&&irpcstationPhi==7&&abs(irpcstationEta)==6)layer_name="Pivot";
    }
    else {PlaneTipo=1;}
  }			
  else {PlaneTipo=2;}
  
  /////// NB !!!!!
  // the eta strip number increases going far away from IP
  // the phi strip number increases going from HV side to RO side
  
  if ( irpcstationName< 9 )  {
    for ( int ieta=-1; ieta!= -9; ieta-- ) {
      int irpcdoubletRn = irpcdoubletR ;
      if(irpcstationName==2&&(abs(irpcstationEta)!=7)&&ieta==-7){
	if(irpcdoubletR==1)continue;
	irpcdoubletRn=1;
      }
      else if(irpcstationName==2&&irpcstationPhi==7&&(abs(irpcstationEta)!=6)&&ieta==-6){
	if(irpcdoubletR==1)continue;
	irpcdoubletRn=1;
      }      
      else if(irpcstationName==2&&irpcstationEta ==- 7 &&abs(ieta)<7){
	irpcdoubletRn=2;
      }
      else if(irpcstationName==2&&irpcstationPhi==7&& irpcstationEta==-6&&abs(ieta)<5){
	irpcdoubletRn=2;
      }
 
      for(int idbz=1; idbz!= 4; idbz++){
	const MuonGM::RpcReadoutElement* rpc = m_muonMgr->getRpcRElement_fromIdFields(irpcstationName, ieta, irpcstationPhi, irpcdoubletRn, idbz, 1 );
        if(rpc != NULL ){
          if ( idbz != rpc->getDoubletZ() ) continue ;
          if(ieta==irpcstationEta){
            if( idbz==irpcdoubletZ ){ 
              ShiftEtaStrips    = NetaStrips;  
              ShiftEtaStripsTot = NetaStripsTotSideC ;
              ShiftPhiTot_db    = NphiStripsTotSideC ;
            }
            NetaStrips    +=  rpc->NetaStrips()  ;
          }
          NetaStripsTot      +=  rpc->NetaStrips()   ;
          NetaStripsTotSideC +=  rpc->NetaStrips()   ;
          NphiStripsTotSideC +=  rpc->NphiStrips()   ;
        } 
      }
    } // end loop on side C    
    for(int ieta=0; ieta!= 9; ieta++){
      int irpcdoubletRn = irpcdoubletR ;
      if(irpcstationName==2&&(abs(irpcstationEta)!=7)&&ieta==7){
	if(irpcdoubletR==1)continue;
	irpcdoubletRn=1;
      }
      else if(irpcstationName==2&&irpcstationPhi==7&&(abs(irpcstationEta)!=6)&&ieta==6){
	if(irpcdoubletR==1)continue;
	irpcdoubletRn=1;
      }      
      else if(irpcstationName==2&&(irpcstationEta==7)&&abs(ieta)<7){
	irpcdoubletRn=2;
      }
      else if(irpcstationName==2&&irpcstationPhi==7&&(irpcstationEta==6)&&abs(ieta)<5){
	irpcdoubletRn=2;
      }
      
      for(int idbz=1; idbz!= 4; idbz++){
    	const MuonGM::RpcReadoutElement* rpc = m_muonMgr->getRpcRElement_fromIdFields(irpcstationName, ieta, irpcstationPhi, irpcdoubletRn, idbz, 1 );
    	if(rpc != NULL ){
	  if ( idbz != rpc->getDoubletZ() ) continue ;
    	  if( ieta==irpcstationEta ){
    	    if( idbz==irpcdoubletZ ){ ShiftEtaStrips=NetaStrips; }
    	    NetaStrips    +=  rpc->NetaStrips()  ;
    	  }
    	  NetaStripsTot   +=  rpc->NetaStrips()  ;
    	  
    	  if( ieta==irpcstationEta ){
	    if( idbz==irpcdoubletZ ) { ShiftEtaStripsTot = NetaStripsTotSideA  ;  ShiftPhiTot_db = NphiStripsTotSideA ; }
	  }
	  NetaStripsTotSideA  +=  rpc->NetaStrips()   ;
	  NphiStripsTotSideA  +=  rpc->NphiStrips()   ;
    	  
        } //check if rpc!=NULL
      } //for loop in idbz
    } // for loop in etastation
  }
  else {
    for ( int ieta=-1; ieta!= -9; ieta-- ) {
      for ( int irpcstationNameF=9; irpcstationNameF!=11; irpcstationNameF++  ) {
	for (int idbz=1; idbz!= 4; idbz++){ 
	  const MuonGM::RpcReadoutElement* rpc = m_muonMgr->getRpcRElement_fromIdFields(irpcstationNameF, ieta, irpcstationPhi, irpcdoubletR, idbz, 1 );
	  if(rpc != NULL ){
	    if ( idbz != rpc->getDoubletZ() ) continue ;
	    if ( ieta==irpcstationEta  && irpcstationNameF==irpcstationName ) {
	      if( idbz==irpcdoubletZ ){ 
		ShiftEtaStrips    = NetaStrips;  
		ShiftEtaStripsTot = NetaStripsTotSideC ;
		ShiftPhiTot_db    = NphiStripsTotSideC ;
	      }
	      NetaStrips    +=  rpc->NetaStrips()  ;
	    }
	    NetaStripsTot   +=  rpc->NetaStrips()  ;
	    NetaStripsTotSideC +=  rpc->NetaStrips()   ;
	    NphiStripsTotSideC +=  rpc->NphiStrips()   ;
	  }
	}
      }
    } // end loop on side C    
    for(int ieta=0; ieta!= 9; ieta++){
      for ( int irpcstationNameF=9; irpcstationNameF!=11; irpcstationNameF++  ) {
	for(int idbz=1; idbz!= 4; idbz++){
	  const MuonGM::RpcReadoutElement* rpc = m_muonMgr->getRpcRElement_fromIdFields(irpcstationNameF, ieta, irpcstationPhi, irpcdoubletR, idbz, 1 );
	  if(rpc != NULL ){
	    if ( idbz != rpc->getDoubletZ() ) continue ;
	    if ( ieta==irpcstationEta  && irpcstationNameF==irpcstationName ) {
	      if( idbz==irpcdoubletZ ){ ShiftEtaStrips=NetaStrips; }
	      NetaStrips    +=  rpc->NetaStrips()  ;
	    }
	    NetaStripsTot   +=  rpc->NetaStrips()  ;
    	  
	    if ( ieta==irpcstationEta && irpcstationNameF==irpcstationName ){
	      if( idbz==irpcdoubletZ ) { ShiftEtaStripsTot = NetaStripsTotSideA  ;  ShiftPhiTot_db = NphiStripsTotSideA ; }
	    }
	    NetaStripsTotSideA  +=  rpc->NetaStrips()   ;
	    NphiStripsTotSideA  +=  rpc->NphiStrips()   ;
  
	  } //check if rpc!=NULL
	} //for loop in idbz
      } // for loop in etastation 
    }    
  }
     
  Nbin	      = NetaStrips     ;
  ShiftStrips = ShiftEtaStrips ;
  
  // commented out 05 oct 2009
  // check it !!
  // if(irpcstationEta<0) { 
  //   ShiftEtaStripsTot = NetaStripsTotSideC - ShiftEtaStripsTot ;
  //   ShiftPhiTot_db    = NphiStripsTotSideC - ShiftPhiTot_db    ;
  // }
  
  //re-define for phi view
  if(irpcmeasuresPhi==1) {
    Nbin = NphiStrips ;	   
    ShiftStrips =  ShiftPhiStrips;
  }
     
  EtaStripSign =  1	                   ; 
  if(irpcstationEta<0)  EtaStripSign = -1  ; 
  
  // cool db strip index
  if(irpcmeasuresPhi==0) {
    strip_dbindex = ( ShiftEtaStripsTot + irpcstrip ) * EtaStripSign ;
  }
  else {
    if ( irpcstationEta<0 ) { strip_dbindex= - NetaStripsTotSideC - ShiftPhiTot_db - irpcstrip ;}
    else { strip_dbindex = NetaStripsTotSideA + ShiftPhiTot_db + irpcstrip; }
  } 
  
  // if ( irpcstationEta>0 ) strip_dbindex = strip_dbindex -1;
  if ( irpcstationEta>=0 ) strip_dbindex = strip_dbindex -1;
  
  //bin panel number for summary plots 
  int irpcstationName_index = irpcdoubletR -1 ; // 0 LowPt, 1 Pivot, 2 HighPt
  if ( irpcstationName !=2 && irpcstationName !=3 && irpcstationName !=8 ) irpcstationName_index += 2 ;  
  //PanelIndex = irpcmeasuresPhi + (irpcgasGap-1)*2 + (irpcdoubletPhi-1)*4 + (irpcdoubletZ-1)*8 + (irpcstationName_index)*24 
  //  + ( ( abs(irpcstationEta) ) -1 )*72 ;
  PanelIndex = irpcmeasuresPhi + (irpcgasGap-1)*2 + (irpcdoubletPhi-1)*4 + (irpcdoubletZ-1)*8 + (irpcstationName_index)*24 
    + ( ( abs(irpcstationEta) ) )*72 ;
  
  // exception for special chambers sectors 12 and 14:
  if ( irpcstationName==10 ) { 
    // convention: BOG <-> doubletZ=3 <-> (3-1)*8=16
    PanelIndex = irpcmeasuresPhi + (irpcgasGap-1)*2 + (irpcdoubletPhi-1)*4 + 16 + (irpcstationName_index)*24 
      + ( ( abs(irpcstationEta) ) )*72 ;
  }
  if ( (irpcdoubletR==2) && (irpcstationName==9 || irpcstationName==10) ) {
    // convention: chambers of RPC upgrade -> eta = eta + 7
    PanelIndex = PanelIndex + 7*72 ;
  }
  
  if(irpcstationEta<0) PanelIndex = -PanelIndex ; 
  
  PanelIndex+= 1000*irpctriggerInfo	;
  	  
  rpcstriptot.push_back(NphiStrips	      );  // 0
  rpcstriptot.push_back(ShiftPhiStrips	      );  // 1
  rpcstriptot.push_back(NetaStrips	      );
  rpcstriptot.push_back(ShiftEtaStrips	      );
  rpcstriptot.push_back(ShiftStrips	      );
  rpcstriptot.push_back(NetaStripsTot	      );
  rpcstriptot.push_back(NetaStripsTotSideA    );
  rpcstriptot.push_back(NetaStripsTotSideC    );
  rpcstriptot.push_back(ShiftEtaStripsTot     );
  rpcstriptot.push_back(Nbin		      );
  rpcstriptot.push_back(EtaStripSign	      );
  rpcstriptot.push_back(SectorLogic	      );
  rpcstriptot.push_back(Side		      );
  rpcstriptot.push_back(PanelIndex	      );   //13
  rpcstriptot.push_back(Settore 	      );
  rpcstriptot.push_back(PlaneTipo	      );   //15
  rpcstriptot.push_back(strip_dbindex	      );   //16
  rpcstriptot.push_back(NphiStripsTotSideA    );
  rpcstriptot.push_back(NphiStripsTotSideC    );
  
  return  rpcstriptot ;

}    
std::vector<std::string>    RPCStandaloneTracksMon::RpcLayerSectorSideName(Identifier prdcoll_id, int  irpctriggerInfo) 
{
  
  ATH_MSG_DEBUG ( "in RPCStandaloneTracksMon::RpcLayerSectorSideName" );  
  
  std::vector<std::string>  layersectorside_name ;
    
  std::string layer_name		   ;
  std::string layer_name_ee_pp		   ;
  std::string layer_name_e_p		   ;
  std::string layertodraw1_name 	   ;
  std::string layertodraw2_name 	   ;
  std::string layervslayer_name 	   ;
  std::string layer0_name		   ;
  std::string layer1_name		   ;
  std::string layer2_name		   ;
  std::string layerPhivsEta_name	   ;
  std::string layerPhivsEtaSector_name	   ;
  std::string layeronly_name               ;
  std::string HVorROsideleft  = "HV side"  ;
  std::string HVorROsideright = "RO side"  ;
  std::string layer_name_panel             ;
  std::string sector_dphi_layer            ;
  	       
 
  int irpcstationPhi	=   int(m_rpcIdHelper->stationPhi(prdcoll_id))   ;	      
  int irpcstationName	=   int(m_rpcIdHelper->stationName(prdcoll_id))  ;	      
  int irpcstationEta	=   int(m_rpcIdHelper->stationEta(prdcoll_id))   ;		      
  int irpcdoubletR  	=   int(m_rpcIdHelper->doubletR(prdcoll_id))	 ;	      
  int irpcdoubletZ  	=   int(m_rpcIdHelper->doubletZ(prdcoll_id))	 ;
  int irpcdoubletPhi	=   int(m_rpcIdHelper->doubletPhi(prdcoll_id))   ;
  int irpcgasGap	=   int(m_rpcIdHelper->gasGap(prdcoll_id))	 ;
  int irpcmeasuresPhi	=   int(m_rpcIdHelper->measuresPhi(prdcoll_id))  ;
  
  sprintf(dblZ_char    ,"_dblZ%d"    ,irpcdoubletZ  );
  sprintf(dblPhi_char  ,"_dblPhi%d"  ,irpcdoubletPhi);
  	    
  //2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG
  if( (irpcstationName>1) && (irpcstationName<4||irpcstationName==8) ){
    if(irpcdoubletR==1){
      layer_name="LowPt";
      if(irpcstationName==2&&abs(irpcstationEta)==7)layer_name="Pivot";
      if(irpcstationName==2&&irpcstationPhi==7&&abs(irpcstationEta)==6)layer_name="Pivot";
    }
    else {layer_name="Pivot";}
  }			
  else {layer_name="HighPt";}
	    
  if(irpcstationName==3||irpcstationName==5||irpcstationName>7){
    HVorROsideleft  = "RO side" ;
    HVorROsideright = "HV side" ;
  }
  
  layer_name_ee_pp = layer_name ;    
  //define if rpc hits from gasgap=0,1 or trigger hits
  if(irpctriggerInfo==0){
    if(irpcgasGap==1){
      layer_name+="0";layertodraw1_name="GasGap1";layertodraw2_name="GasGap2";
    }
    else {layer_name+="1";}	  
  }
  else if(irpctriggerInfo==100){
    layer_name="HighPt_TriggerFromLowPt";
    layertodraw1_name="TriggerIn";
    layertodraw2_name="TriggerOut LowPt";
  }
  else if(irpctriggerInfo==106){
    layer_name="HighPt_TriggerOut"; 
    layertodraw1_name="TriggerOut";
    layertodraw2_name="GasGap1or2 Pivot";
  }
  else if(irpctriggerInfo==6||irpctriggerInfo==106){
    layer_name="LowPt_TriggerOut"; 
    layertodraw1_name="TriggerOut";
    layertodraw2_name="GasGap1or2 Pivot";
  }
  else {
    layer_name+="_NotKnown"; ;
  }
            
  layeronly_name    = layer_name ;
  layer_name_e_p    = layer_name ;
  layer_name_panel  = layeronly_name + dblPhi_char + dblZ_char;
  sector_dphi_layer = layer_name + dblPhi_char;
  // sector_dphi_layer.insert(8, "_") ;
  
  if(irpcmeasuresPhi==0){
    layer_name+=dblPhi_char;
    layer_name+="_Eta";
    layer_name_e_p+="_Eta";
    layer_name_panel+="_Eta";
    layer_name_ee_pp+=dblPhi_char;
    layer_name_ee_pp+="_Eta";
    
    ////////////////
    
    if(irpctriggerInfo==0){
      layervslayer_name = layer_name_ee_pp+"2vsEta1" ;	  
    }
    else if(irpctriggerInfo==100){
      layervslayer_name = layer_name_ee_pp+"HPtTrInvsLPtTrOut" ;
    }
    else if(irpctriggerInfo==  6){
      layervslayer_name = layer_name_ee_pp+"vsLPtTrOut" ;
    }
    else if(irpctriggerInfo==106){
      layervslayer_name = layer_name_ee_pp+"vsHPtTrOut" ;
    }
    else {
      layervslayer_name = layer_name_ee_pp+"_NotKnown"; ;
    }
    
    ////////////////
    
    
    layer0_name="<--- IP  Rpc Eta strip  EC --->"	     ;
    layer1_name="<--- IP  Rpc Eta strip  "+layertodraw1_name+"  EC --->";
    layer2_name="<--- IP  Rpc Eta strip  "+layertodraw2_name+"  EC --->";	       
  }
  else{
    layer0_name=HVorROsideleft+"  Rpc Phi strip  "+ HVorROsideright    ;
    layer_name+=dblZ_char ;
    layer_name+="_Phi"    ;
    layer_name_e_p+="_Phi";
    layer_name_panel+="_Phi";
    layer_name_ee_pp+=dblZ_char;
    layer_name_ee_pp+="_Phi"   ;
    layerPhivsEta_name	 = layer_name_e_p    + "vsEta"		       ;
    
    ////////////////
    
    if(irpctriggerInfo==0){
      layervslayer_name = layer_name_ee_pp+"2vsPhi1" ;	  
    }
    else if(irpctriggerInfo==100){
      layervslayer_name = layer_name_ee_pp+"HighPtTrInvsLowPtTrOut" ;
    }
    else if(irpctriggerInfo==  6){
      layervslayer_name = layer_name_ee_pp+"LowPtvsLowPtTrOut" ;
    }
    else if(irpctriggerInfo==106){
      layervslayer_name = layer_name_ee_pp+"HighPtvsHighPtTrOut" ;
    }
    else {
      layervslayer_name = layer_name_ee_pp+"_NotKnown"; ;
    }
    
    ////////////////
    
    layerPhivsEtaSector_name  = sector_name + layer_name_e_p + "vsEta" ;
    layer1_name="<--- DBL_PHI0 Rpc Phi strip  "+layertodraw1_name+"  DBL_PHI1 --->";
    layer2_name="<--- DBL_PHI0 Rpc Phi strip  "+layertodraw2_name+"  DBL_PHI1 --->";	    
  }
	   

  	       
  //sector
  if(irpcstationEta>0){
    SideSector = "A"	 ; 
  }
  else{
    SideSector = "C"	 ; 
  }
	    
  sector = 2 * irpcstationPhi 			             ;
  if(irpcstationName==2 ||  irpcstationName==4 ) sector--    ;
  sprintf(sector_char,"Sector%.2d",sector)		     ;
  SideSector =  sector_char  			             ;
  
  sector_dphi_layer = SideSector + "_" + sector_dphi_layer   ;	   
	   
  layersectorside_name.push_back(layer_name		 ); // 0	  
  layersectorside_name.push_back(layertodraw1_name	 );	  
  layersectorside_name.push_back(layertodraw2_name	 );	  
  layersectorside_name.push_back(layervslayer_name	 );	  
  layersectorside_name.push_back(layer0_name		 );	  
  layersectorside_name.push_back(layer1_name		 );	  
  layersectorside_name.push_back(layer2_name		 );	  
  layersectorside_name.push_back(layerPhivsEta_name	 );	  
  layersectorside_name.push_back(layerPhivsEtaSector_name);	  
  layersectorside_name.push_back(SideSector              );	  
  layersectorside_name.push_back(layeronly_name          );
  layersectorside_name.push_back(layer_name_panel        );	
  layersectorside_name.push_back(sector_dphi_layer       );  //12
  
  return  layersectorside_name;

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
