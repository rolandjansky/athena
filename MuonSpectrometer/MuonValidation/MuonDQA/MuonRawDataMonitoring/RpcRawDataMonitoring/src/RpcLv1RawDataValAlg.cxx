/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Package : RpcLv1RawDataValAlg
// Authors:  N. Benekos(Illinois) - M. Bianco(INFN-Lecce) - G. Chiodini(INFN-Lecce) - A. Guida (INFN-Lecce) 
// March 2008
//
// DESCRIPTION:
// Subject: RPCLV1-->Offline Muon Data Quality
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
      
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
 
#include "xAODEventInfo/EventInfo.h"
#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonDQAUtils/MuonCosmicSetup.h"
#include "MuonDQAUtils/MuonDQAHistMap.h" 
 
#include "RpcRawDataMonitoring/RpcLv1RawDataValAlg.h"
#include "AthenaMonitoring/AthenaMonManager.h"

#include <inttypes.h> 

#include <sstream>

static const int  NMAXHIT = 200 ;
   

//std::string lv1_layer_name         ;
int  i_triggertimelowpt  	     ;    
int  i_triggertimehighpt 	     ;  
int  n_triggerlowpt	 	     ;  
int  n_triggerlowpt_eta  	     ;
int  n_triggerlowpt_phi  	     ;
int  n_triggerhighpt	 	     ;
int  n_triggerhighpt_eta 	     ;
int  n_triggerhighpt_phi 	     ;

int  IJKtriggerLowPtPhi  	     ;
int  IJKtriggerLowPtEta  	     ;
int  IJKtriggerHighPtPhi 	     ;
int  IJKtriggerHighPtEta 	     ;

/////////////////////////////////////////////////////////////////////////////

RpcLv1RawDataValAlg::RpcLv1RawDataValAlg( const std::string & type, const std::string & name, const IInterface* parent )
  :ManagedMonitorToolBase( type, name, parent )
{  
  // Declare the properties 
  declareProperty("CheckCabling",     	  m_checkCabling=false);
  declareProperty("RpcLv1File",       	  m_rpclv1file=false);    
  declareProperty("RpcLv1Hist",       	  m_rpclv1hist=false); 
  declareProperty("RpcLv1Prof",       	  m_rpclv1prof=false); 
  declareProperty("RpcLv1ReduceNbins",    m_rpclv1reducenbins=2);	   
  declareProperty("ChamberName",      	  m_chamberName="XXX");
  declareProperty("StationSize",      	  m_StationSize="XXX");
  declareProperty("StationEta",       	  m_StationEta=-100);
  declareProperty("StationPhi",       	  m_StationPhi=-100);
  declareProperty("LastEvent",        	  m_lastEvent=0);
  declareProperty("Sector",           	  m_sector=0); 
  declareProperty("CosmicStation",    	  m_cosmicStation=0);
  declareProperty("Side",             	  m_side=0); 
  declareProperty("Clusters",             m_doClusters = false); 
  declareProperty("doCoolDB",		  m_doCoolDB   = false );	 
  m_padsId        = 0;
  m_rpcchambersId = 0;
}

RpcLv1RawDataValAlg::~RpcLv1RawDataValAlg()
{
      
  ATH_MSG_INFO ( " deleting RpcLv1RawDataValAlg " );
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode RpcLv1RawDataValAlg::initialize()
{

  ATH_MSG_INFO ( "in initializing RpcLv1RawDataValAlg" );
  
  ATH_MSG_INFO ( "package version = "   << PACKAGE_VERSION );
  
  ATH_MSG_INFO ( "properties are 	" 				);
  ATH_MSG_INFO ( "RpcLv1Prof 		" << m_rpclv1prof		);
  ATH_MSG_INFO ( "RpcLv1Hist		" << m_rpclv1hist		);
  ATH_MSG_INFO ( "RpcLv1ReduceNbins	" << m_rpclv1reducenbins	);
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
  } else {
    ATH_MSG_DEBUG ( " Found the RPCcablingSvc. " );    }

  
  std::vector<std::string> hardware_name_list                  ;
  std::vector<std::string> layer_name_list                     ;
  std::vector<std::string> layervslayer_name_list              ;
  std::vector<std::string> layerPhivsEta_name_list             ;
  std::vector<std::string> layerPhivsEtaSector_name_list       ;
  hardware_name_list.push_back("XXX");
  
  ManagedMonitorToolBase::initialize().ignore();  //  Ignore the checking code;
 
  rpc_eventstotal=0;
  rpc_event_inarea=0;
  return StatusCode::SUCCESS;
}


StatusCode RpcLv1RawDataValAlg::StoreTriggerType() {
  const xAOD::EventInfo* eventInfo;
  StatusCode sc = StatusCode::SUCCESS;
  sc = m_eventStore->retrieve(eventInfo);
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR ( "Could not find eventInfo " );
    return sc;
  }else {ATH_MSG_DEBUG ( "RpcLv1RawDataValAlg::retrieved eventInfo" );} 
  
  trigtype = eventInfo->level1TriggerType();

  return sc;
}


/*----------------------------------------------------------------------------------*/
StatusCode RpcLv1RawDataValAlg::fillHistograms()
/*----------------------------------------------------------------------------------*/
{
  StatusCode sc = StatusCode::SUCCESS; 
  
  if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0Raw || m_environment ==  AthenaMonManager::online) {  
  
    ATH_MSG_DEBUG ( "RpcLv1RawDataValAlg::RPCLV1 RawData Monitoring Histograms being filled" );
  
    if(StoreTriggerType() != StatusCode::SUCCESS) {ATH_MSG_ERROR ( "Could not find StoreTriggerType" );  } // retreive and store trigger type for this event
 
    ATH_MSG_DEBUG ( "GetTriggerType() "<< GetTriggerType()  );

    i_triggertimelowpt  =-1 ;    
    i_triggertimehighpt =-1 ;  
    n_triggerlowpt      = 0 ;  
    n_triggerlowpt_eta  = 0 ;
    n_triggerlowpt_phi  = 0 ;
    n_triggerhighpt     = 0 ;
    n_triggerhighpt_eta = 0 ;
    n_triggerhighpt_phi = 0 ;
  
    IJKtriggerLowPtPhi  = 0 ;
    IJKtriggerLowPtEta  = 0 ;
    IJKtriggerHighPtPhi = 0 ;
    IJKtriggerHighPtEta = 0 ;
 
 
    m_nPads          = 0;
    m_nCMA           = 0;
    m_nFiredChannels = 0;  	
    
    for ( int i=0; i!=64; i++ ) {
      for ( int j=0; j!=4; j++) {
        for ( int k=0; k!=4; k++ ) {
	  for ( int l=0; l!=32; l++ ) {
	    PhiOr[i][j][k][l] = 0;
	  }
	}
      }
    }  
  
    // Prepare the retrieval of the RpcPadContainer
  
    sc= (*m_activeStore)->retrieve(rpcRDO,"RPCPAD");
    if (sc.isFailure()) {
      ATH_MSG_ERROR ( "Could not find RPC Pads" );     
      return sc;
    }

    // begin loop to get trigger time
    for (RpcPadContainer::const_iterator rdoColli = rpcRDO->begin(); rdoColli!=rpcRDO->end(); ++rdoColli)
      {
	rdoColl = *rdoColli;
	// Now loop on the RDO to find the ones with trigger hits ijk=6
	if ( (rdoColl)->size() != 0 ) {  	 
	  m_nPads++;   
	  RpcPad::const_iterator itCM = (rdoColl)->begin();
	  for ( ; itCM != (rdoColl)->end() ; ++itCM ) {
	    m_nCMA ++;       
	    RpcCoinMatrix::const_iterator itChan = (*itCM)->begin();	     
	     
	    for ( ; itChan != (*itCM)->end() ; ++itChan ) {	     
          
	      m_nFiredChannels++;
	  
	      i_sector     = (rdoColl)->sector()     ;
	      i_padId      = (rdoColl)->onlineId()   ;
	      i_status     = (rdoColl)->status()     ;
	      i_errorCode  = (rdoColl)->errorCode()  ;
	      i_cmaId      = (*itCM)->onlineId()  ;
	      i_fel1Id     = (*itCM)->fel1Id()    ;
	      i_febcId     = (*itCM)->febcId()    ;
	      i_crc        = (*itCM)->crc()	;
	      i_bcId       = (*itChan)->bcid()    ;
	      i_time       = (*itChan)->time()    ;
	      i_ijk        = (*itChan)->ijk()     ;
	      i_channel    = (*itChan)->channel() ;
	      i_overlap    = -1                   ;
	      i_threshold  = -1                   ;
	      if (i_ijk==7) {
		i_overlap  =(*itChan)->ovl();
		i_threshold=(*itChan)->thr();
	      }
			  
	      if(i_ijk==6){
		if(i_cmaId<4){  
		  n_triggerlowpt++                            ;   
		  i_triggertimelowpt += i_bcId * 8 +  i_time  ;
		  if (i_cmaId<2) {            //  i_cmaId==0 || i_cmaId==1
		    n_triggerlowpt_phi++ ;  
		  }
		  else {
		    n_triggerlowpt_eta++ ;
		  }
		}
		else{ 
		  n_triggerhighpt++                           ;   
		  i_triggertimehighpt += i_bcId * 8 +  i_time ;
		  if (i_cmaId<6) {               //     i_cmaId==4 || i_cmaId==5
		    n_triggerhighpt_phi++ ;
		  }
		  else {
		    n_triggerhighpt_eta++ ;
		  }
		}
			   
	      }			  		
		
	    }
	  }
	}
      }
    // end loop to get trigger time
		 
    if(n_triggerlowpt>0)i_triggertimelowpt=i_triggertimelowpt/n_triggerlowpt;
    if(n_triggerhighpt>0)i_triggertimehighpt=i_triggertimehighpt/n_triggerhighpt;
		 
  
    //counts rpc hits
    std::string m_generic_path_rpclv1monitoring = "Muon/MuonRawDataMonitoring/RPCLV1";
  
    MonGroup rpclv1prd_shift( this, m_generic_path_rpclv1monitoring + "/Overview", run, ATTRIB_UNMANAGED );
    MonGroup rpclv1_shift_dqmf( this, m_generic_path_rpclv1monitoring + "/GLOBAL", run, ATTRIB_UNMANAGED ); 
    MonGroup rpcCoolDb( this, m_generic_path_rpclv1monitoring+"/CoolDB", run, ATTRIB_UNMANAGED )          ;
   
    sc  = rpclv1prd_shift.getHist(rpclv1_hitperEvent,"rpclv1_hitperEvent");
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register rpclv1_hitperEvent hist to MonGroup" );  
    rpclv1_hitperEvent->Fill(float(m_nFiredChannels)); 
  
    sc = rpclv1prd_shift.getHist(rpclv1triggerlowpt_etastat,"Trigger_Hits_LowPt_eta_Stat");
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register rpclv1triggerlowpt_etastat hist to MonGroup" );
    rpclv1triggerlowpt_etastat->Fill(float(n_triggerlowpt_eta)); 
  
    sc = rpclv1prd_shift.getHist(rpclv1triggerlowpt_phistat,"Trigger_Hits_LowPt_phi_Stat");
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register rpclv1triggerlowpt_phistat hist to MonGroup" );
    rpclv1triggerlowpt_phistat->Fill(float(n_triggerlowpt_phi)); 
  
    sc = rpclv1prd_shift.getHist(rpclv1triggerlowpt_etaphistat,"Trigger_Hits_Lowpt_etaphi_Stat");
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register Trigger_Hits_Lowpt_etaphi_Stat hist to MonGroup" );
    rpclv1triggerlowpt_etaphistat->Fill( float(n_triggerlowpt_phi), float(n_triggerlowpt_eta) );
   
    sc = rpclv1prd_shift.getHist(rpclv1triggerhighpt_etastat,"Trigger_Hits_HighPt_eta_Stat"); 
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register rpclv1triggerhighpt_eta_stat hist to MonGroup" );  
    rpclv1triggerhighpt_etastat->Fill(float(n_triggerhighpt_eta)); 
 
    sc = rpclv1prd_shift.getHist(rpclv1triggerhighpt_phistat,"Trigger_Hits_HighPt_phi_Stat"); 
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register rpclv1triggerhighpt_phi_stat hist to MonGroup" );  
    rpclv1triggerhighpt_phistat->Fill(float(n_triggerhighpt_phi)); 
  
    sc = rpclv1prd_shift.getHist(rpclv1triggerhighpt_etaphistat,"Trigger_Hits_HighPt_etaphi_Stat");
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register Trigger_Hits_HighPt_etaphi_Stat hist to MonGroup" ); 
    rpclv1triggerhighpt_etaphistat->Fill( float(n_triggerhighpt_phi), float(n_triggerhighpt_eta) );
    
    sc = rpclv1prd_shift.getHist(rpclv1ROI_LowPt,"ROI_LowPt_distribution");
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register ROI_LowPt_distribution hist to MonGroup" );

    sc = rpclv1prd_shift.getHist(rpclv1ROI_HighPt,"ROI_HighPt_distribution");
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register ROI_HighPt_distribution hist to MonGroup" );
  
    sc = rpclv1prd_shift.getHist(rpclv1sectorlogicstat,"SLChannel_per_SectorLogic");	
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register rpclv1sectorlogicstat hist to MonGroup" );
	    
    sc = rpclv1prd_shift.getHist(rpclv1towervslogicstat,"PadChannel_per_SectorLogic");	
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register rpclv1towervslogicstat hist to MonGroup" );

    sc = rpclv1prd_shift.getHist(rpclv1cmalogicstat,"CMChannel_per_SectorLogic");
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register rpclv1cmalogicstat hist to MonGroup" );
 
    sc = rpclv1prd_shift.getHist(rpclv1cmalogicstat,"CMChannel_per_SectorLogic");
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register rpclv1cmalogicstat hist to MonGroup" );  
 
    sc = rpclv1prd_shift.getHist(rpclv1_BCid_per_TriggerCorr,"rpclv1_BCid_per_TriggerCorrelation");
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register rpclv1_BCid_per_TriggerCorr hist to MonGroup" );   
  
    sc = rpclv1_shift_dqmf.getHist( rpclv1_BCid_vs_SL, "rpclv1_BCid_vs_SectorLogic") ;
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register rpclv1_BCid_vs_SectorLogic hist to MonGroup" ); 
    
    sc = rpclv1_shift_dqmf.getHist( rpclv1_LPt_BCid_vs_SL, "rpclv1_LPt_BCid_vs_SectorLogic") ;
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register rpclv1_LPt_BCid_vs_SectorLogic hist to MonGroup" ); 
    
    sc = rpclv1_shift_dqmf.getHist( rpclv1_HPt_BCid_vs_SL, "rpclv1_HPt_BCid_vs_SectorLogic") ;
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register rpclv1_HPt_BCid_vs_SectorLogic hist to MonGroup" ); 
    
    sc = rpclv1_shift_dqmf.getHist(rpclv1_BCid_per_TriggerType,"rpclv1_BCid_per_TriggerType");
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register rpclv1_BCid_per_TriggerType hist to MonGroup" );   
       
    sc  = rpclv1_shift_dqmf.getHist(rpclv1_TriggerCond_vs_SL, "TriggerCondition_vs_SectorLogic" ) ;
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register TriggerCondition_vs_SectorLogic hist to MonGroup" );
    
    // logical or
    sc = rpclv1prd_shift.getHist(rpclv1_logicalOR_LowPt0,  "PhiLogicalOR_LowPt0"  );
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get PhiLogicalOR_LowPt0 hist "  );
    
    sc = rpclv1prd_shift.getHist(rpclv1_logicalOR_LowPt1,  "PhiLogicalOR_LowPt1"  );
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get PhiLogicalOR_LowPt1 hist "  );
    
    sc = rpclv1prd_shift.getHist(rpclv1_logicalOR_HighPt0, "PhiLogicalOR_HighPt0" );
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get PhiLogicalOR_HighPt0 hist " );
    
    sc = rpclv1prd_shift.getHist(rpclv1_logicalOR_HighPt1, "PhiLogicalOR_HighPt1" );
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get PhiLogicalOR_HighPt1 hist " );
    
    sc = rpclv1prd_shift.getHist( TriggerCondition_vs_CM, "Trigger_Condition_vs_CM" );
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get TriggerCondition_vs_CM hist " );


    for (RpcPadContainer::const_iterator rdoColli = rpcRDO->begin(); rdoColli!=rpcRDO->end(); ++rdoColli)
      {
	rdoColl = *rdoColli;
	// Now loop on the RDO to find the ones with trigger hits ijk=6
	if ( (rdoColl)->size() != 0 ) {		    
        	    
	  RpcPad::const_iterator itCM = (rdoColl)->begin();
	  for ( ; itCM != (rdoColl)->end() ; ++itCM ) {
              
	    RpcCoinMatrix::const_iterator itChan = (*itCM)->begin();	   
           
	    for ( ; itChan != (*itCM)->end() ; ++itChan ) {	   

	      i_sector	 = (rdoColl)->sector()    ;
	      i_padId	 = (rdoColl)->onlineId()  ;
	      i_status	 = (rdoColl)->status()    ;
	      i_errorCode  = (rdoColl)->errorCode() ;
	      i_cmaId	 = (*itCM)->onlineId()    ;
	      i_fel1Id	 = (*itCM)->fel1Id()	  ;
	      i_febcId	 = (*itCM)->febcId()	  ;
	      i_crc	 = (*itCM)->crc()	  ;
	      i_bcId	 = (*itChan)->bcid()	  ;
	      i_time	 = (*itChan)->time()	  ;
	      i_ijk	 = (*itChan)->ijk()	  ;
	      i_channel	 = (*itChan)->channel()   ;
	      i_overlap	 = -1			  ;
	      i_threshold  = -1			  ;
	      if (i_ijk==7) {
		i_overlap  =(*itChan)->ovl()	  ;
		i_threshold=(*itChan)->thr()	  ;
	      }			
	      else{
		if ( i_ijk==6 ) {
                  
		 
		  //look for thresholds ijk=7 in the next rdo's ijk=6,6...,7 with the same time  
		  i_thr = 99 ;
		  RpcCoinMatrix::const_iterator itChanThr ;	   
		  thrskip=0;
		  for ( itChanThr = itChan+1 ; itChanThr!=(*itCM)->end() ; ++itChanThr ) {	
		    if(thrskip==1)continue;
		    if((*itChanThr)->ijk()<6) {
		      thrskip=1;//search finish
		      continue;
		    }
		    if((*itChanThr)->bcid()!=i_bcId||(*itChanThr)->time()!=i_time) {
		      thrskip=1;//error if there are different timing
		      i_thr = 98 ; 
		      continue;
		    }
		    if((*itChanThr)->ijk()==7&&(*itChanThr)->bcid()==i_bcId&&(*itChanThr)->time()==i_time) {
		      i_thr=(*itChanThr)->thr();
		      thrskip=1;//search finish
		      continue;
		    }
		  }
		  
		  rpclv1_BCid_per_TriggerCorr->Fill(trigtype, i_bcId +  8 * int(i_cmaId/4) );
        	
		  for (int k=0; k!= 8+1 ; k++ ) {
		    int trigBinType = trigtype & int(pow(2,float(k))) ;
		    if (trigBinType!=0) rpclv1_BCid_per_TriggerType->Fill(k , i_bcId +  8 * int(i_cmaId/4) );  
		  }
        	
		  rpclv1_BCid_vs_SL -> Fill( i_sector, i_bcId +  8 * int(i_cmaId/4) ) ;

		  if(i_cmaId<4){ 
		    rpclv1_LPt_BCid_vs_SL -> Fill( i_sector, i_bcId  ) ;
		  }
		  else { 
		    rpclv1_HPt_BCid_vs_SL -> Fill( i_sector, i_bcId   ) ;
		  }

		  // Trigger conditions selection begin
		  NtriggerLowPtPhi  = 0 ; IJKtriggerLowPtPhi  = 0 ;
		  NtriggerLowPtEta  = 0 ; IJKtriggerLowPtEta  = 0 ;
		  NtriggerHighPtPhi = 0 ; IJKtriggerHighPtPhi = 0 ;
		  NtriggerHighPtEta = 0 ; IJKtriggerHighPtEta = 0 ;
          
		  RpcCoinMatrix::const_iterator itChan3 = (*itCM)->begin();	   
		  for ( ; itChan3 != (*itCM)->end() ; ++itChan3 ) {
		    ijk_trigger =  (*itChan3)->ijk() ;
		    if ( ijk_trigger>5  ) continue; 
               
		    Layertrigger = ijk_trigger; 
		    if(ijk_trigger==3) Layertrigger = Layertrigger-1;
		    if(ijk_trigger==4) Layertrigger = Layertrigger-1;
		    if(ijk_trigger==5) Layertrigger = Layertrigger-2;
        	  
		    if (i_cmaId < 2 )  {
		      IJKtriggerLowPtPhi=IJKtriggerLowPtPhi|int(pow(2,float(Layertrigger)));
		      NtriggerLowPtPhi++ ;  
		    }
		    if (i_cmaId ==2 || i_cmaId==3) {
		      IJKtriggerLowPtEta=IJKtriggerLowPtEta|int(pow(2,float(Layertrigger)));
		      NtriggerLowPtEta++ ; 
		    }
		    if (i_cmaId ==4 || i_cmaId==5) {
		      IJKtriggerHighPtPhi=IJKtriggerHighPtPhi|int(pow(2,float(Layertrigger)));
		      NtriggerHighPtPhi++ ;   
		    }
		    if (i_cmaId ==6 || i_cmaId==7) {
		      IJKtriggerHighPtEta=IJKtriggerHighPtEta|int(pow(2,float(Layertrigger)));
		      NtriggerHighPtEta++ ;  
		    }
        	  
		    // trigger road
		    if (!m_rpclv1hist ) continue     ;
		    if (ijk_trigger < 2 ) continue   ; 
		    int ConfirmCh =  (*itChan3)->channel() ;
		    if(ijk_trigger==3)ConfirmCh +=  32 ;
		    if(ijk_trigger==5)ConfirmCh +=  32 ;
		    
        	  
		    sprintf(sectorlogic_name_char,"SectorLogic%d",i_sector);sectorlogic_name = sectorlogic_name_char ;
		    sprintf(tower_name_char      ,"Tower%d"      ,i_padId );tower_name	     = tower_name_char	     ;	
		    sprintf(cma_name_char        ,"Cma%d"        ,i_cmaId );cma_name	     = cma_name_char	     ;	
		    sprintf(thr_name_char        ,"Thr%d"        ,i_thr   );thr_name	     = thr_name_char	     ;
          
		    histo_flag=true;
		    for (std::vector<std::string>::const_iterator iter=sectorlogicTowerCma_name_list2.begin(); iter!=sectorlogicTowerCma_name_list2.end(); iter++){
		      if ( (sectorlogic_name+tower_name+cma_name+thr_name)==*iter){histo_flag=false;}
		    }
		    if (histo_flag){ 
		      sectorlogicTowerCma_name_list2.push_back(sectorlogic_name+tower_name+cma_name+thr_name); 
		      bookRPCLV1TriggerRoadHistograms(sectorlogic_name, tower_name, cma_name, thr_name);
		    }
        	  
		    const MuonDQAHistList& hists = m_stationHists.getList( sectorlogic_name );
		    TH2* RPCLV1TriggerRoad = hists.getH2(sectorlogic_name + "_" + tower_name + cma_name + thr_name + "_TriggerRoad");
		    if (RPCLV1TriggerRoad) {
		      RPCLV1TriggerRoad->Fill( float(i_channel ), float( ConfirmCh ) );
		    }
		    else {ATH_MSG_DEBUG ( "RPCLV1TriggerRoad not in hist list!" );}
		  }
        	
		  // fill histo Trigger Conditions
		  sc  = rpclv1prd_shift.getHist(rpclv1_TriggerLy_vs_SL, "TriggerLayer_vs_SectorLogic" ) ;
		  if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get TriggerCondition_vs_SectorLogic hist to MonGroup" );
              
		  if ( NtriggerLowPtPhi >0 ) {
		    sc  = rpclv1prd_shift.getHist(rpclv1Trigger_cond_LowPt_phi,"Trigger_Condition_LowPt_Phi");
		    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register Trigger_Layer_LowPt_Phi hist to MonGroup" );
		    rpclv1Trigger_cond_LowPt_phi -> Fill ( float(IJKtriggerLowPtPhi) )  ;
		    rpclv1_TriggerLy_vs_SL       -> Fill (i_sector, IJKtriggerLowPtPhi ); // general 
		    TriggerCondition_vs_CM       -> Fill (i_sector*56 + i_padId*8 + i_cmaId, IJKtriggerLowPtPhi );
		  }
		  if ( NtriggerLowPtEta >0 ) {
		    sc  = rpclv1prd_shift.getHist(rpclv1Trigger_cond_LowPt_eta,"Trigger_Condition_LowPt_Eta");
		    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't registerTrigger_Condition_LowPt_Eta hist to MonGroup" );
		    rpclv1Trigger_cond_LowPt_eta -> Fill ( float(IJKtriggerLowPtEta) )   ;
		    rpclv1_TriggerLy_vs_SL       -> Fill ( i_sector, IJKtriggerLowPtEta ); // general
		    TriggerCondition_vs_CM       -> Fill (i_sector*56 + i_padId*8 + i_cmaId, IJKtriggerLowPtEta );
		  }
		  if ( NtriggerHighPtPhi >0 ) {
		    sc  = rpclv1prd_shift.getHist(rpclv1Trigger_cond_HighPt_phi,"Trigger_Condition_HighPt_Phi");
		    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't registerTrigger_Condition_LowPt_Eta hist to MonGroup" );
		    rpclv1Trigger_cond_HighPt_phi-> Fill ( float(IJKtriggerHighPtPhi))	    ;
		    rpclv1_TriggerLy_vs_SL       -> Fill ( i_sector, IJKtriggerHighPtPhi +16 ); // general
		    TriggerCondition_vs_CM       -> Fill (i_sector*56 + i_padId*8 + i_cmaId, IJKtriggerHighPtPhi );
		  }
		  if ( NtriggerHighPtEta >0 ) {
		    sc  = rpclv1prd_shift.getHist(rpclv1Trigger_cond_HighPt_eta,"Trigger_Condition_HighPt_Eta");
		    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't registerTrigger_Condition_LowPt_Eta hist to MonGroup" );
		    rpclv1Trigger_cond_HighPt_eta-> Fill ( float(IJKtriggerHighPtEta))	    ;
		    rpclv1_TriggerLy_vs_SL       -> Fill ( i_sector, IJKtriggerHighPtEta +16 ); // general
		    TriggerCondition_vs_CM       -> Fill (i_sector*56 + i_padId*8 + i_cmaId, IJKtriggerHighPtEta );
		  }
        	
		  n_trigLow_ly = 0; n_trigHigh_ly = 0;
		  for (int k=0; k!=4; k++ ) {
		    if ( ((IJKtriggerLowPtPhi | IJKtriggerLowPtEta ) & int(pow(2,float(k)))) !=0 ) n_trigLow_ly++  ;
		  }
		  if (NtriggerLowPtPhi >0 || NtriggerLowPtEta >0 ) {
		    rpclv1_TriggerCond_vs_SL -> Fill(i_sector, n_trigLow_ly ) ;
		  }
		  if ( ((IJKtriggerHighPtPhi | IJKtriggerHighPtEta) & 1 )!=0 ) {
		    for (int k=2; k!=4; k++ ) {  // k=1
		      if ( ((IJKtriggerHighPtPhi | IJKtriggerHighPtEta) & int(pow(2,float(k)))) !=0 ) n_trigHigh_ly++ ;		  
		    }
		    if (n_trigHigh_ly==0) { rpclv1_TriggerCond_vs_SL -> Fill(i_sector, 5);}//bin 5-> only trigger from LowPt
		    else {rpclv1_TriggerCond_vs_SL -> Fill(i_sector, n_trigHigh_ly +5 ) ;}
		  }
		  // Trigger conditions selection end
            
		  int i_cma_phi = i_cmaId ;
		  RpcPad::const_iterator itCM2 = (rdoColl)->begin();
		  for ( ; itCM2 != (rdoColl)->end() ; ++itCM2 ) {
		    int  i_cmaId2 = (*itCM2)->onlineId();
		    int  i_cma_eta = i_cmaId2;
        	
		    // RoI and trigger conditions selection begin
		    if ( i_cmaId < 2 ) {  		     // phi coincidence LowPt
		      if ( i_cma_eta==2 || i_cma_eta==3 ) { 
			RpcCoinMatrix::const_iterator itChan2 = (*itCM2)->begin();	   
			for ( ; itChan2 != (*itCM2)->end() ; ++itChan2 ) {
			  if ( (*itChan2)->ijk()==6 ) { 
			    int roi = 4*i_padId + i_cma_phi + 2*( i_cma_eta-2 );
			    rpclv1ROI_LowPt -> Fill( float(i_sector), float(roi) );  
			  }
			}
		      }
		    }	
		    else if ( i_cmaId==4 || i_cmaId==5 ) {     // phi coincidence HighPt
		      if ( i_cma_eta==6 || i_cma_eta==7  ){ 
			RpcCoinMatrix::const_iterator itChan2 = (*itCM2)->begin();	   
			for ( ; itChan2 != (*itCM2)->end() ; ++itChan2 ) {
			  if ( (*itChan2)->ijk()==6 ) { 
			    int roi = 4*i_padId + i_cma_phi-4 + 2*( i_cma_eta-6 );
			    rpclv1ROI_HighPt -> Fill( float(i_sector), float(roi) );  
			  }
			}
		      }
		    }
		    // end RoI selection
              
              
              
		  }//end loop on CM2
            
		} // end if ijk=6 of i_channel 
              
		// logical or
		if ( i_ijk>1 && i_ijk<6 ) {
		  if ( i_cmaId==0 || i_cmaId==1 ) {
		    PhiOr[i_sector][i_cmaId][i_ijk-2][i_channel] = PhiOr[i_sector][i_cmaId][i_ijk-2][i_channel] | int(pow(2,float(i_padId)));
		  }
		  if ( i_cmaId==4 || i_cmaId==5 ) {
		    PhiOr[i_sector][i_cmaId-2][i_ijk-2][i_channel] = PhiOr[i_sector][i_cmaId-2][i_ijk-2][i_channel] | int(pow(2,float(i_padId)));
		  }
		}
		// SL stat
		rpclv1sectorlogicstat->Fill( float(i_sector)  );
          
		// tower stat  
		rpclv1towervslogicstat->Fill( float(i_sector), float(i_padId)); 
        			      
		// cma per sector logic
		rpclv1cmalogicstat -> Fill( float(i_sector), float(i_padId*8 + i_cmaId) );
        				      
		sprintf(sectorlogic_name_char,"SectorLogic%d",i_sector);sectorlogic_name = sectorlogic_name_char ;
		sprintf(tower_name_char	   ,"Tower%d"	   ,i_padId );tower_name	   = tower_name_char	   ;	
		sprintf(cma_name_char	   ,"Cma%d"	   ,i_cmaId );cma_name  	   = cma_name_char	   ;
		sprintf(ijk_name_char	   ,"ijk%d"	   ,i_ijk   );ijk_name         = ijk_name_char         ;      
        	    
		std::string m_generic_path_RPCLV1cmatimevschcxx	     = m_generic_path_rpclv1monitoring+"/"+sectorlogic_name+"/";
		m_generic_path_RPCLV1cmatimevschcxx		    += tower_name				     ;
		m_generic_path_RPCLV1cmatimevschcxx		    += "/rpclv1cosmic_cmatimevsch"		     ;
		m_generic_path_RPCLV1cmatimevschcxx		    += cma_name 				     ;
        	     
		std::string m_generic_path_RPCLV1cmatimetriggervschcxx = m_generic_path_rpclv1monitoring+"/"+sectorlogic_name+"/";
		m_generic_path_RPCLV1cmatimetriggervschcxx	       += tower_name				     ;
		m_generic_path_RPCLV1cmatimetriggervschcxx	       += "/rpclv1cosmic_cmatimetriggervsch"	     ;
		m_generic_path_RPCLV1cmatimetriggervschcxx	       += cma_name				     ;
          
		// Fill time histograms begin
		if (m_rpclv1hist ) {	
		  histo_flag=true;
		  for (std::vector<std::string>::const_iterator iter=sectorlogicTowerCma_name_list.begin(); iter!=sectorlogicTowerCma_name_list.end(); iter++){
		    if ( (sectorlogic_name+tower_name+cma_name)==*iter){histo_flag=false;}
		  }
		  if (histo_flag){
		    sectorlogicTowerCma_name_list.push_back(sectorlogic_name+tower_name+cma_name);
		    bookRPCLV1cmatimevschHistograms(sectorlogic_name, tower_name, cma_name);
		  }

		  const MuonDQAHistList& hists = m_stationHists.getList( sectorlogic_name );
		  TH2* RPCLV1cmatimevsch = hists.getH2(sectorlogic_name + "_" + tower_name + cma_name +"_time_vs_channel");
		  if (RPCLV1cmatimevsch) {RPCLV1cmatimevsch->Fill( float(i_channel + i_ijk * 32 ),  float(i_bcId*8 + i_time)   );}
		  else {ATH_MSG_DEBUG ( "RPCLV1cmatimevsch not in hist list!" );}

		  if(n_triggerlowpt>0){
		    TH2* RPCLV1cmatimetriggervsch = hists.getH2(sectorlogic_name + "_" + tower_name + cma_name +"_(time-triggertime)_vs_channel");
		    if (RPCLV1cmatimetriggervsch) {RPCLV1cmatimetriggervsch->Fill( float(i_channel + i_ijk * 32 ),  float(i_bcId*8 + i_time -i_triggertimelowpt ) );}
		    else {ATH_MSG_DEBUG ( "RPCLV1cmatimetriggervsch not in hist list!" );}
		  }
		} // Fill time histograms end
          
		// profiles 
		if ( m_rpclv1prof )  
		  { 
		    shift_ijk = 0;
		    shift_pad = 1;
		    if ( (i_sector % 4)==0 || (i_sector % 4)==3 )  {   // Large sector
		      sec_large=2 ; }
		    else {
		      sec_large=1  ;}

		    if ( i_cmaId==1 || i_cmaId==3 || i_cmaId==5 || i_cmaId==7 ) {
		      sprintf(cma_name_p_char , "Cma%d_%d" , i_cmaId-1, i_cmaId);  cma_name_p = cma_name_p_char	 ;
		      shift_cm = 32 ;
		    }
		    else {
		      sprintf(cma_name_p_char  ,"Cma%d_%d"  ,i_cmaId, i_cmaId+1);  cma_name_p = cma_name_p_char	 ;
		      shift_cm = 0; }

		    if ( i_ijk==2 || i_ijk==3 ) {
		      ijk_name_p = "ijk2_3" ;
		      shift_pad = 2;
		      shift_cm = shift_cm *2 ; }
		    else {
		      if ( i_ijk==4 || i_ijk==5 ) {
			ijk_name_p = "ijk4_5" ;
			shift_pad = 2 ;
			shift_cm = shift_cm *2 ;}
		      else { ijk_name_p = ijk_name ; }
		    }

		    if ( i_ijk==3  || i_ijk==5 ) { shift_ijk = 32 ; }

		    histo_flag=true;
		    for (std::vector<std::string>::const_iterator iter=profile_list.begin(); iter!=profile_list.end(); iter++){
		      if ( (sectorlogic_name+cma_name_p+ijk_name_p)==*iter){histo_flag=false;}
		    }
		    if (histo_flag) {
		      profile_list.push_back(sectorlogic_name + cma_name_p + ijk_name_p);
		      //bookRPCLV1ProfilesHistograms( i_sector, sectorlogic_name, i_cmaId, cma_name_p, i_ijk, ijk_name_p ); // compilation warning to i_cmaId
		      bookRPCLV1ProfilesHistograms( i_sector, sectorlogic_name, cma_name_p, i_ijk, ijk_name_p );
		    }
		    const MuonDQAHistList& hists2 = m_stationHists.getList( sectorlogic_name );
		    TH1* RPCLV1Profiles = hists2.getH1(sectorlogic_name + "_" + cma_name_p + "_" + ijk_name_p + "_Profiles") ;
		    if (RPCLV1Profiles ) { RPCLV1Profiles->Fill( float( i_padId*shift_pad*64 + shift_cm + shift_ijk + i_channel ) ) ;}
		    else {ATH_MSG_DEBUG ( "RPCLV1Profiles not in hist list!" );}
		  }     
	      }	
	      
	      
	      
if ( m_doCoolDB ) {	      
	            
  uint16_t side	       =          0; 
  uint16_t slogic      =   i_sector;
  if(slogic>31) {
   slogic -=        32;
   side    =         1;
  } 
  uint16_t padId       =    i_padId;
  uint16_t cmaId       =    i_cmaId;
  uint16_t ijk	       =      i_ijk;
  uint16_t channel     =  i_channel;
      
  // Get the list of offline channels corresponding to the online identifier
  std::list<Identifier> idList = m_cabling->give_strip_id(side, slogic, padId, cmaId, ijk, channel);
  std::list<Identifier>::const_iterator it_list;
 
  for (it_list=idList.begin() ; it_list != idList.end() ; ++it_list) {
   
 	      // and add the digit to the collection
 	      Identifier prdcoll_id = *it_list;

 	      //    // RPC digits do not hold anymore time of flight : digit time (and RDO time) is TOF subtracted
 	      //    // recalculate the time of flight in case it was not in the RDOs
 	      //    if (time==0) {
 	      //      // get the digit position
 	      //      const MuonGM::RpcReadoutElement* descriptor =
 	      //  m_muonMgr->getRpcReadoutElement(stripOfflineId);
 	      //
 	      //      const HepGeom::Point3D<double> stripPos = descriptor->stripPos(stripOfflineId);
 	      //      // TEMP : set the time of flight from the digit position
 	      //      // temporary solution
 	      //      time = static_cast<int> ( stripPos.distance()/(299.7925*CLHEP::mm/CLHEP::ns) );
 	      //
 	      //
 
 	      std::vector<std::string>   rpclayersectorsidename = RpcLayerSectorSideName(prdcoll_id, 0)  ;
 	      std::string		 sector_dphi_layer	= rpclayersectorsidename[12]		 ;
	
 	      std::vector<int>  	 RpcStrip = RpcStripShift(prdcoll_id, 0);
 	      int strip_dbindex        = (RpcStripShift(prdcoll_id, 0)).at(16);// cool strip profile
	        	  if ( m_doCoolDB ) {
	        	    if(cmaId==0||cmaId==2||cmaId==4||cmaId==6)sc = rpcCoolDb.getHist( rpcCool_StripProfile, sector_dphi_layer+"_ProfileDataCMeven" ) ;
	        	    if(cmaId==1||cmaId==3||cmaId==5||cmaId==7)sc = rpcCoolDb.getHist( rpcCool_StripProfile, sector_dphi_layer+"_ProfileDataCModd"  ) ;
	        	    if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get " << sector_dphi_layer << "_ProfileDataCMeven or odd" );
	        	    if ( rpcCool_StripProfile ) {
	        		rpcCool_StripProfile->Fill( strip_dbindex );
	        	    }
	        	  }
    
     }
    }//m_doCoolDB
      
	      
	      
	      
	      
	      
	      
	      
	      
	    }//loop over chan
	  } //loop over CM			    
	} // if RDO_pad_size>0
      }  // pads loop
 
    // Fill logical OR histograms
    // PhiOr[sector logic][cm][ijk][channel]
    // LowPt0	cm 0 1	ijk 2 3	PhiOr: 	0 1	0 1 
    // LowPt1	cm 0 1	ijk 4 5		0 1	2 3
    // HighPt0	cm 4 5	ijk 2 3		2 3	0 1
    // HighPt1	cm 4 5 	ijk 4 5		2 3	2 3
    for ( int iSL=0; iSL!=64; iSL++ ) {
      for ( int k=0; k!=2; k++ ) {
        for ( int iCh=0; iCh!=32; iCh++ ) {
	  if ( PhiOr[iSL][0+k][0+k][iCh] !=0 ) rpclv1_logicalOR_LowPt0 ->Fill( iSL+1, PhiOr[iSL][0+k][0+k][iCh] );
	  if ( PhiOr[iSL][0+k][2+k][iCh] !=0 ) rpclv1_logicalOR_LowPt1 ->Fill( iSL+1, PhiOr[iSL][0+k][2+k][iCh] );
	  if ( PhiOr[iSL][2+k][0+k][iCh] !=0 ) rpclv1_logicalOR_HighPt0->Fill( iSL+1, PhiOr[iSL][2+k][0+k][iCh] );
	  if ( PhiOr[iSL][2+k][2+k][iCh] !=0 ) rpclv1_logicalOR_HighPt1->Fill( iSL+1, PhiOr[iSL][2+k][2+k][iCh] );
	}
      }
    } 
    	
  
  }//AthenaMonManager::tier0 || AthenaMonManager::tier0Raw   
  
 		
  return sc;  // statuscode check  
   
} 


/*----------------------------------------------------------------------------------*/
StatusCode RpcLv1RawDataValAlg::bookHistogramsRecurrent()
/*----------------------------------------------------------------------------------*/
{
  
  ATH_MSG_DEBUG ( "RPC RawData Monitoring Histograms being booked" );


  StatusCode sc = StatusCode::SUCCESS; 
  
  if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0Raw  || m_environment ==  AthenaMonManager::online) {     
    //declare a group of histograms
    std::string m_generic_path_rpclv1monitoring = "Muon/MuonRawDataMonitoring/RPCLV1";
  
    MonGroup rpclv1prd_shift( this, m_generic_path_rpclv1monitoring +"/Overview", run, ATTRIB_UNMANAGED );
    MonGroup rpclv1_shift_dqmf( this, m_generic_path_rpclv1monitoring + "/GLOBAL", run, ATTRIB_UNMANAGED )  ;
    MonGroup rpcCoolDb( this, m_generic_path_rpclv1monitoring+"/CoolDB", run, ATTRIB_UNMANAGED )         ;
     
    if(newEventsBlock){}
    if(newLumiBlock){}
    if(newRun)
      {         

	ATH_MSG_DEBUG ( "RPCLV1 RawData Monitoring : isNewRun" );
       
        
	// Trigger Layers vs SL
	std::string m_rpclv1_TriggerLy_vs_SL_title      = "TriggerLayer_vs_SectorLogic"       ;
	const char* m_rpclv1_TriggerLy_vs_SL_title_char = m_rpclv1_TriggerLy_vs_SL_title.c_str();
	
	TH2* rpclv1_TriggerLy_vs_SL = new TH2I(m_rpclv1_TriggerLy_vs_SL_title_char, m_rpclv1_TriggerLy_vs_SL_title_char, 64, 0, 64, 32, 0, 32);
	sc = rpclv1prd_shift.regHist( rpclv1_TriggerLy_vs_SL ) ;
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1_TriggerLy_vs_SL Failed to register histogram " );       
	    return sc;
	  }
	rpclv1_TriggerLy_vs_SL->GetXaxis()->SetTitle("Sector Logic")       ;
	rpclv1_TriggerLy_vs_SL->GetYaxis()->SetTitle("")                   ; 
	rpclv1_TriggerLy_vs_SL->SetOption("COLZ");
	
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1_TriggerLy_vs_SL << m_rpclv1_TriggerLy_vs_SL_title.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked rpclv1_TriggerLy_vs_SL successfully" ); 
	  
	// set label for Trigger Layer vs SL histograms
	char YLabel[100];
	for(int ie=1;ie!=16;ie++){
	  sprintf(YLabel,"L=");
	  int first = 0 ;
	  for(int layer=0;layer!=4;layer++){
	    int bit = (ie>>layer) & 1  ;
	    if(bit==1){
	      if(first==0){
		sprintf(YLabel,"%s%d",YLabel,layer);
	      }
	      else{
		sprintf(YLabel,"%s,%d",YLabel,layer);
	      }
	      first=1;
	      rpclv1_TriggerLy_vs_SL->GetYaxis()->SetBinLabel(ie+1,YLabel   );  //  low pt
	      rpclv1_TriggerLy_vs_SL->GetYaxis()->SetBinLabel(ie+1+16,YLabel);  //  high pt
	    }
	  }
	}
	
	// trigger conditions per CM
	// n bin x = 8 cm * 7 pad * 64 sector logic 
	int n_bin_trigCm = 8 * 7 * 64;
	std::string m_rpclv1_TriggerCond_vs_CM_title      = "Trigger_Condition_vs_CM"                ;
	const char* m_rpclv1_TriggerCond_vs_CM_title_char = m_rpclv1_TriggerCond_vs_CM_title.c_str();
	TH2* rpclv1_TriggerCond_vs_CM = new TH2I(m_rpclv1_TriggerCond_vs_CM_title_char,	m_rpclv1_TriggerCond_vs_CM_title_char,
						 n_bin_trigCm , 1, n_bin_trigCm+1, 16, 0, 16 );
        sc = rpclv1prd_shift.regHist( rpclv1_TriggerCond_vs_CM );
	
	rpclv1_TriggerCond_vs_CM->GetXaxis()->SetTitle("CM + Pad*8 + SL*56");
	rpclv1_TriggerCond_vs_CM->GetYaxis()->SetTitle("")                  ; 
	rpclv1_TriggerCond_vs_CM->SetOption("COLZ") ;
	
	// set label for Trigger conditions per CM
	rpclv1_TriggerCond_vs_CM->GetYaxis()->SetBinLabel(1 ,"No Hits" );
	for(int ie=1;ie!=16;ie++){
	  sprintf(YLabel,"L=");
	  int first = 0 ;
	  for(int layer=0;layer!=4;layer++){
	    int bit = (ie>>layer) & 1  ;
	    if(bit==1){
	      if(first==0){
		sprintf(YLabel,"%s%d",YLabel,layer);
	      }
	      else{
		sprintf(YLabel,"%s,%d",YLabel,layer);
	      }
	      first=1;
	      rpclv1_TriggerCond_vs_CM->GetYaxis()->SetBinLabel(ie+1,YLabel   ); 
	    }
	  }
	}
	
	// logical OR
	
	std::vector<std::string> LyNameVec;
	LyNameVec.push_back("LowPt0" );
	LyNameVec.push_back("LowPt1" );
	LyNameVec.push_back("HighPt0");
	LyNameVec.push_back("HighPt1");
	
	// n bin x = 64 sector logic
	// n bin y = 2 ^ ( nPad=7 ) = 128
	int n_bin_logOR = 64 ;
	
	for (std::vector<std::string>::const_iterator it=LyNameVec.begin(); it!=LyNameVec.end(); it++ ) {
	  std::string m_rpclv1_logicalOR_title      = "PhiLogicalOR_" + *it           ;
	  const char* m_rpclv1_logicalOR_title_char = m_rpclv1_logicalOR_title.c_str();
          TH2* rpclv1_logicalOR = new TH2I( m_rpclv1_logicalOR_title_char, m_rpclv1_logicalOR_title_char, 
					    n_bin_logOR, 0, n_bin_logOR, 128, 0, 128 );
	  sc = rpclv1prd_shift.regHist( rpclv1_logicalOR );
	  
	  rpclv1_logicalOR->GetXaxis()->SetTitle("Sector Logic");
	  // rpclv1_logicalOR->GetYaxis()->SetTitle("Pad")         ;
	  rpclv1_logicalOR->SetOption("COLZ");
	  rpclv1_logicalOR->GetYaxis()->SetLabelSize(0.03);
	  for (int ie=0+1; ie!=128; ie++ ) {
	    sprintf(YLabel,"pad");
	    for ( int k=0; k!=8; k++ ) {
	      if ( (int(pow(2,float(k))) & ie) != 0 ) {
	        sprintf(YLabel,"%s %d", YLabel, k);
              }
	    }
	    rpclv1_logicalOR->GetYaxis()->SetBinLabel(ie+1, YLabel );
	  }
	}


	// SHIFT histograms
	// Trigger conditions vs SL
	std::string m_rpclv1_TriggerCond_vs_SL_title      = "TriggerCondition_vs_SectorLogic"       ;
	const char* m_rpclv1_TriggerCond_vs_SL_title_char = m_rpclv1_TriggerCond_vs_SL_title.c_str();
	
	TH2* rpclv1_TriggerCond_vs_SL = new TH2I(m_rpclv1_TriggerCond_vs_SL_title_char, m_rpclv1_TriggerCond_vs_SL_title_char, 64, 0, 64, 7, 1, 8);
	sc = rpclv1_shift_dqmf.regHist( rpclv1_TriggerCond_vs_SL ) ;
	if(sc.isFailure()) {
	  ATH_MSG_FATAL ( "rpclv1_TriggerCond_vs_SL Failed to register histogram " );       
	  return sc;
	}
	rpclv1_TriggerCond_vs_SL->GetXaxis()->SetTitle("Sector Logic");
	rpclv1_TriggerCond_vs_SL->GetYaxis()->SetTitle("")            ; 
	rpclv1_TriggerCond_vs_SL->GetYaxis()->SetTitleSize(0.03)      ;
	rpclv1_TriggerCond_vs_SL->SetOption("COLZ")                   ;
	
	
	
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1_TriggerCond_vs_SL << m_rpclv1_TriggerCond_vs_SL_title.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked rpclv1_TriggerCond_vs_SL successfully" ); 
	  
	// set label for Trigger Condition vs SL histograms
	rpclv1_TriggerCond_vs_SL->GetYaxis()->SetBinLabel(1,"1/4 Lpt "  ); // low pt
	rpclv1_TriggerCond_vs_SL->GetYaxis()->SetBinLabel(2,"2/4 Lpt "  );
	rpclv1_TriggerCond_vs_SL->GetYaxis()->SetBinLabel(3,"3/4 Lpt "  );
        rpclv1_TriggerCond_vs_SL->GetYaxis()->SetBinLabel(4,"4/4 Lpt "  ); 
	rpclv1_TriggerCond_vs_SL->GetYaxis()->SetBinLabel(5,"Lpt+0/2Hpt"); // high pt
	rpclv1_TriggerCond_vs_SL->GetYaxis()->SetBinLabel(6,"Lpt+1/2Hpt");
	rpclv1_TriggerCond_vs_SL->GetYaxis()->SetBinLabel(7,"Lpt+2/2Hpt");
	
	// BCid (ijk==6) vs SL
	std::string m_rpclv1_BCid_vs_SL_title      = "rpclv1_BCid_vs_SectorLogic"     ;
	const char* m_rpclv1_BCid_vs_SL_title_char = m_rpclv1_BCid_vs_SL_title.c_str();
	
	TH2* rpclv1_BCid_vs_SL = new TH2I ( m_rpclv1_BCid_vs_SL_title_char, m_rpclv1_BCid_vs_SL_title_char, 64, 0, 64, 8*2, 0, 16) ;
	sc = rpclv1_shift_dqmf.regHist( rpclv1_BCid_vs_SL );
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1_BCid_vs_SectorLogic Failed to register histogram " );       
	    return sc;
	  }
	rpclv1_BCid_vs_SL->GetXaxis()->SetTitle("Triggered RPC Sector Logic") ;  
	rpclv1_BCid_vs_SL->GetYaxis()->SetTitle("BCId+HPtTrigger*8 for trigger hits") ;
	rpclv1_BCid_vs_SL->SetOption("COLZ");

	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1_BCid_vs_SL << m_rpclv1_BCid_vs_SL_title.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked rpclv1_BCid_vs_SectorLogic successfully" ); 
		
	// LPt BCid (ijk==6) vs SL
	std::string m_rpclv1_LPt_BCid_vs_SL_title      = "rpclv1_LPt_BCid_vs_SectorLogic"     ;
	const char* m_rpclv1_LPt_BCid_vs_SL_title_char = m_rpclv1_LPt_BCid_vs_SL_title.c_str();
	
	TH2* rpclv1_LPt_BCid_vs_SL = new TH2I ( m_rpclv1_LPt_BCid_vs_SL_title_char, m_rpclv1_LPt_BCid_vs_SL_title_char, 64, 0, 64, 8, 0, 8) ;
	sc = rpclv1_shift_dqmf.regHist( rpclv1_LPt_BCid_vs_SL );
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1_BCid_vs_SectorLogic Failed to register histogram " );       
	    return sc;
	  }
	rpclv1_LPt_BCid_vs_SL->GetXaxis()->SetTitle("Triggered LPt RPC Sector Logic") ;  
	rpclv1_LPt_BCid_vs_SL->GetYaxis()->SetTitle("Trigger hits BCid") ;
	rpclv1_LPt_BCid_vs_SL->SetOption("COLZ");

	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1_LPt_BCid_vs_SL << m_rpclv1_LPt_BCid_vs_SL_title.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked rpclv1_BCid_vs_SectorLogic successfully" ); 
		
	// BCid (ijk==6) vs SL
	std::string m_rpclv1_HPt_BCid_vs_SL_title      = "rpclv1_HPt_BCid_vs_SectorLogic"     ;
	const char* m_rpclv1_HPt_BCid_vs_SL_title_char = m_rpclv1_HPt_BCid_vs_SL_title.c_str();
	
	TH2* rpclv1_HPt_BCid_vs_SL = new TH2I ( m_rpclv1_HPt_BCid_vs_SL_title_char, m_rpclv1_HPt_BCid_vs_SL_title_char, 64, 0, 64, 8, 0, 8) ;
	sc = rpclv1_shift_dqmf.regHist( rpclv1_HPt_BCid_vs_SL );
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1_BCid_vs_SectorLogic Failed to register histogram " );       
	    return sc;
	  }
	rpclv1_HPt_BCid_vs_SL->GetXaxis()->SetTitle("Triggered HPt RPC Sector Logic") ;  
	rpclv1_HPt_BCid_vs_SL->GetYaxis()->SetTitle("Trigger hits BCid") ;
	rpclv1_HPt_BCid_vs_SL->SetOption("COLZ");

	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1_HPt_BCid_vs_SL << m_rpclv1_HPt_BCid_vs_SL_title.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked rpclv1_BCid_vs_SectorLogic successfully" ); 
	

	////////////////////////////////////////////////////
     
      
	//hits per event
	std::string m_rpclv1_hitperEvent_title = "rpclv1_hitperEvent";      
	const char* m_rpclv1_hitperEvent_title_char = m_rpclv1_hitperEvent_title.c_str();
       
	TH1 *rpclv1_hitperEvent=new TH1I(m_rpclv1_hitperEvent_title_char,m_rpclv1_hitperEvent_title_char, NMAXHIT,0.,NMAXHIT); 				
	sc=rpclv1prd_shift.regHist(rpclv1_hitperEvent) ;  
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1_hitperEvent Failed to register histogram " );       
	    return sc;
	  }
	rpclv1_hitperEvent->SetFillColor(42);
	rpclv1_hitperEvent->GetXaxis()->SetTitle("RPC hit / event");
	rpclv1_hitperEvent->GetYaxis()->SetTitle("Counts");
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1_hitperEvent );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked rpclv1_hitperEvent successfully" ); 
      
	//lowpt ETA trigger hits stat
	std::string m_generic_path_rpclv1triggerlowpt_etastat = m_generic_path_rpclv1monitoring+"/Overview";
	std::string m_rpclv1triggerlowpt_etastat_title = "Trigger_Hits_LowPt_eta_Stat";      
	const char* m_rpclv1triggerlowpt_etastat_title_char = m_rpclv1triggerlowpt_etastat_title.c_str();
       
	TH1 *rpclv1triggerlowpt_etastat=new TH1I(m_rpclv1triggerlowpt_etastat_title_char,m_rpclv1triggerlowpt_etastat_title_char, 15, 0, 15); 				
	sc=rpclv1prd_shift.regHist(rpclv1triggerlowpt_etastat) ;  
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1triggerlowpt_etastat Failed to register histogram " );       
	    return sc;
	  }
	rpclv1triggerlowpt_etastat->SetFillColor(42);
	rpclv1triggerlowpt_etastat->GetXaxis()->SetTitle("LowPt eta trigger hits / event");
	rpclv1triggerlowpt_etastat->GetYaxis()->SetTitle("Counts");
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1triggerlowpt_etastat << m_generic_path_rpclv1triggerlowpt_etastat.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked rpclv1triggerlowpt_etastat successfully" ); 	

	////////////////////////////////////////////// 
 
	//lowpt PHI trigger hits stat
	std::string m_generic_path_rpclv1triggerlowpt_phistat = m_generic_path_rpclv1monitoring+"/Overview";
	std::string m_rpclv1triggerlowpt_phistat_title = "Trigger_Hits_LowPt_phi_Stat";      
	const char* m_rpclv1triggerlowpt_phistat_title_char = m_rpclv1triggerlowpt_phistat_title.c_str();
       
	TH1 *rpclv1triggerlowpt_phistat=new TH1I(m_rpclv1triggerlowpt_phistat_title_char,m_rpclv1triggerlowpt_phistat_title_char, 15, 0, 15); 				
	sc=rpclv1prd_shift.regHist(rpclv1triggerlowpt_phistat) ;  
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1triggerlowpt_phistat Failed to register histogram " );       
	    return sc;
	  }
	rpclv1triggerlowpt_phistat->SetFillColor(42);
	rpclv1triggerlowpt_phistat->GetXaxis()->SetTitle("LowPt phi trigger hits / event");
	rpclv1triggerlowpt_phistat->GetYaxis()->SetTitle("Counts");
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1triggerlowpt_phistat << m_generic_path_rpclv1triggerlowpt_phistat.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked rpclv1triggerlowpt_phistat successfully" ); 	
	
	//////////////////////////////////////////////
      
	//lowpt ETA-PHI trigger hits stat
	std::string m_generic_path_rpclv1triggerlowpt_etaphistat = m_generic_path_rpclv1monitoring+"/Overview";
	std::string m_rpclv1triggerlowpt_etaphistat_title = "Trigger_Hits_Lowpt_etaphi_Stat";      
	const char* m_rpclv1triggerlowpt_etaphistat_title_char = m_rpclv1triggerlowpt_etaphistat_title.c_str();
       
	TH2 *rpclv1triggerlowpt_etaphistat=new TH2I(m_rpclv1triggerlowpt_etaphistat_title_char,m_rpclv1triggerlowpt_etaphistat_title_char, 15, 0, 15,15,0,15); 				
	sc=rpclv1prd_shift.regHist(rpclv1triggerlowpt_etaphistat) ;  
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1triggerlowpt_etaphistat Failed to register histogram " );       
	    return sc;
	  }
	rpclv1triggerlowpt_etaphistat->SetOption("COLZ");
	rpclv1triggerlowpt_etaphistat->GetXaxis()->SetTitle("LowPt phi trigger hits / event");
	rpclv1triggerlowpt_etaphistat->GetYaxis()->SetTitle("LowPt eta trigger hits / event");
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1triggerlowpt_etaphistat << m_generic_path_rpclv1triggerlowpt_etaphistat.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked rpclv1triggerlowpt_etaphistat successfully" ); 	

      
	////////////////////////////////////////////// 
           
	//highpt ETA trigger hits stat
	std::string m_generic_path_rpclv1triggerhighpt_etastat = m_generic_path_rpclv1monitoring+"/Overview";
	std::string m_rpclv1triggerhighpt_etastat_title = "Trigger_Hits_HighPt_eta_Stat";
	const char* m_rpclv1triggerhighpt_etastat_title_char = m_rpclv1triggerhighpt_etastat_title.c_str();
	TH1 *rpclv1triggerhighpt_etastat=new TH1I(m_rpclv1triggerhighpt_etastat_title_char,m_rpclv1triggerhighpt_etastat_title_char, 15, 0, 15);
	sc=rpclv1prd_shift.regHist(rpclv1triggerhighpt_etastat) ;
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1triggerhighpt_etastat Failed to register histogram " );
	    return sc;
	  }
	rpclv1triggerhighpt_etastat->SetFillColor(42);
	rpclv1triggerhighpt_etastat->GetXaxis()->SetTitle("HighPt eta trigger hits / event");
	rpclv1triggerhighpt_etastat->GetYaxis()->SetTitle("Counts");
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1triggerhighpt_etastat << m_generic_path_rpclv1triggerhighpt_etastat.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run );
	ATH_MSG_DEBUG ( "Booked rpclv1triggertimehighpt_etastat successfully" );
 
	//////////////////////////////////////////////

	//highpt PHI trigger hits stat
	std::string m_generic_path_rpclv1triggerhighpt_phistat = m_generic_path_rpclv1monitoring+"/Overview";
	std::string m_rpclv1triggerhighpt_phistat_title = "Trigger_Hits_HighPt_phi_Stat";
	const char* m_rpclv1triggerhighpt_phistat_title_char = m_rpclv1triggerhighpt_phistat_title.c_str();
	TH1 *rpclv1triggerhighpt_phistat=new TH1I(m_rpclv1triggerhighpt_phistat_title_char,m_rpclv1triggerhighpt_phistat_title_char, 15, 0, 15);
	sc=rpclv1prd_shift.regHist(rpclv1triggerhighpt_phistat) ;
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1triggerhighpt_phistat Failed to register histogram " );
	    return sc;
	  }
	rpclv1triggerhighpt_phistat->SetFillColor(42);
	rpclv1triggerhighpt_phistat->GetXaxis()->SetTitle("HighPt phi trigger hits / event");
	rpclv1triggerhighpt_phistat->GetYaxis()->SetTitle("Counts");
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1triggerhighpt_phistat << m_generic_path_rpclv1triggerhighpt_phistat.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run );
	ATH_MSG_DEBUG ( "Booked rpclv1triggertimehighpt_phistat successfully" ); 
 
	//////////////////////////////////////////////
      
	//highpt ETA - PHI trigger hits stat
	std::string m_generic_path_rpclv1triggerhighpt_etaphistat = m_generic_path_rpclv1monitoring+"/Overview";
	std::string m_rpclv1triggerhighpt_etaphistat_title = "Trigger_Hits_HighPt_etaphi_Stat";    
	const char* m_rpclv1triggerhighpt_etaphistat_title_char = m_rpclv1triggerhighpt_etaphistat_title.c_str();     
	TH2 *rpclv1triggerhighpt_etaphistat=new TH2I(m_rpclv1triggerhighpt_etaphistat_title_char,m_rpclv1triggerhighpt_etaphistat_title_char, 15, 0, 15,15, 0, 15);
	sc=rpclv1prd_shift.regHist(rpclv1triggerhighpt_etaphistat) ;  
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1triggerhighpt_etaphistat Failed to register histogram " );       
	    return sc;
	  }      		
	rpclv1triggerhighpt_etaphistat->SetOption("COLZ");
	rpclv1triggerhighpt_etaphistat->GetXaxis()->SetTitle("HighPt phi trigger hits / event");
	rpclv1triggerhighpt_etaphistat->GetYaxis()->SetTitle("HighPt eta trigger hits / event");
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1triggerhighpt_etaphistat << m_generic_path_rpclv1triggerhighpt_etaphistat.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked rpclv1triggertimehighpt_etaphistat successfully" ); 	
  
	////////////////////////////////////////////// 
  
	//sector logic statistics
	std::string m_generic_path_rpclv1sectorlogicstat = m_generic_path_rpclv1monitoring+"/Overview";
	std::string m_rpclv1sectorlogicstat_title = "SLChannel_per_SectorLogic";    
	const char* m_rpclv1sectorlogicstat_title_char = m_rpclv1sectorlogicstat_title.c_str ();     
	TH1 *rpclv1sectorlogicstat=new TH1I(m_rpclv1sectorlogicstat_title_char,m_rpclv1sectorlogicstat_title_char, 64, 0, 64); 
	sc=rpclv1prd_shift.regHist(rpclv1sectorlogicstat) ;  
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1sectorlogicstat Failed to register histogram " );       
	    return sc;
	  }  		
	rpclv1sectorlogicstat->SetFillColor(42);
	rpclv1sectorlogicstat->GetXaxis()->SetTitle("SectorLogic");
	rpclv1sectorlogicstat->GetYaxis()->SetTitle("Counts/Sector Logic "); 
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1sectorlogicstat << m_generic_path_rpclv1sectorlogicstat.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked rpclv1sectorlogicstat successfully" ); 	
    
	////////////////////////////////////////////// 

	//tower per sector logic statistics
	std::string m_generic_path_rpclv1towervslogicstat = m_generic_path_rpclv1monitoring+"/Overview";
	std::string m_rpclv1towervslogicstat_title = "PadChannel_per_SectorLogic";    
	const char* m_rpclv1towervslogicstat_title_char = m_rpclv1towervslogicstat_title.c_str ();     
	TH2 *rpclv1towervslogicstat=new TH2I(m_rpclv1towervslogicstat_title_char,m_rpclv1towervslogicstat_title_char, 64, 0, 64, 8, 0, 8); 
	sc=rpclv1prd_shift.regHist(rpclv1towervslogicstat) ;  
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1towervslogicstat Failed to register histogram " );       
	    return sc;
	  }  		  
	rpclv1towervslogicstat->SetFillColor(42);
	rpclv1towervslogicstat->SetOption("COLZ");
	rpclv1towervslogicstat->GetXaxis()->SetTitle("SectorLogic");
	rpclv1towervslogicstat->GetYaxis()->SetTitle("Pad");
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1towervslogicstat << m_generic_path_rpclv1towervslogicstat.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked rpclv1towervslogicstat successfully" ); 	
 
	////////////////////////////////////////////// 
    
	// cma channel per sector logic stat
	std::string m_generic_path_rpclv1cmalogicstat = m_generic_path_rpclv1monitoring+"/Overview";
	std::string m_rpclv1cmalogicstat_title = "CMChannel_per_SectorLogic";    
	const char* m_rpclv1cmalogicstat_title_char = m_rpclv1cmalogicstat_title.c_str ();     
	TH2 *rpclv1cmalogicstat=new TH2I(m_rpclv1cmalogicstat_title_char,m_rpclv1cmalogicstat_title_char, 64, 0, 64, 8*8, 0, 8*8); 
	sc=rpclv1prd_shift.regHist(rpclv1cmalogicstat) ;  
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1cmalogicstat Failed to register histogram " );       
	    return sc;
	  }  		  
	rpclv1cmalogicstat->SetFillColor(42);
	rpclv1cmalogicstat->SetOption("COLZ");
	rpclv1cmalogicstat->GetXaxis()->SetTitle("SectorLogic");
	rpclv1cmalogicstat->GetYaxis()->SetTitle("Pad*8+Cma");
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1cmalogicstat << m_generic_path_rpclv1cmalogicstat.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked rpclv1cmalogicstat successfully" ); 	
 
	////////////////////////////////////////////// 
      
	//ROI distribution Low Pt
	std::string m_generic_path_rpclv1ROI_LowPt = m_generic_path_rpclv1monitoring+"/Overview";
	std::string m_rpclv1ROI_LowPt_title = "ROI_LowPt_distribution";
	const char* m_rpclv1ROI_LowPt_title_char = m_rpclv1ROI_LowPt_title.c_str();
	TH2 * rpclv1ROI_LowPt = new TH2I(m_rpclv1ROI_LowPt_title_char, m_rpclv1ROI_LowPt_title_char, 64, 0, 64, 28, 0, 28); 
	sc=rpclv1prd_shift.regHist(rpclv1ROI_LowPt);
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1ROI_LowPt Failed to register histogram " );       
	    return sc;
	  } 
	rpclv1ROI_LowPt->SetOption("COLZ");
	rpclv1ROI_LowPt->GetXaxis()->SetTitle("SectorLogic");
	rpclv1ROI_LowPt->GetYaxis()->SetTitle("Region of Interest");
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1ROI_LowPt << m_rpclv1ROI_LowPt_title.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked rpclv1ROI_LowPt successfully" ); 	
      
	/////////////////////////////////////////////
      
	// ROI distribution High Pt
	std::string m_generic_path_rpclv1ROI_HighPt = m_generic_path_rpclv1monitoring+"/Overview";
	std::string m_rpclv1ROI_HighPt_title = "ROI_HighPt_distribution";
	const char* m_rpclv1ROI_HighPt_title_char = m_rpclv1ROI_HighPt_title.c_str();
	TH2 * rpclv1ROI_HighPt = new TH2I(m_rpclv1ROI_HighPt_title_char, m_rpclv1ROI_HighPt_title_char, 64, 0, 64, 28, 0, 28); 
	sc=rpclv1prd_shift.regHist(rpclv1ROI_HighPt);
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1ROI_HighPt Failed to register histogram " );       
	    return sc;
	  } 
	rpclv1ROI_HighPt->SetOption("COLZ");
	rpclv1ROI_HighPt->GetXaxis()->SetTitle("SectorLogic");
	rpclv1ROI_HighPt->GetYaxis()->SetTitle("Region of Interest");
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1ROI_HighPt << m_rpclv1ROI_HighPt_title.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked rpclv1ROI_HighPt successfully" ); 	
	
	////////////////////////////////////
      
	// Trigger_Cond LowPt PHI
	std::string m_generic_path_rpclv1Trigger_cond_LowPt_phi = m_generic_path_rpclv1monitoring+"/Overview";
	std::string m_rpclv1Trigger_cond_LowPt_phi_title = "Trigger_Condition_LowPt_Phi";
	const char* m_rpclv1Trigger_cond_LowPt_phi_title_char = m_rpclv1Trigger_cond_LowPt_phi_title.c_str();
	TH1* rpclv1Trigger_cond_LowPt_phi = new TH1I( m_rpclv1Trigger_cond_LowPt_phi_title_char, m_rpclv1Trigger_cond_LowPt_phi_title_char, 16, 0, 16 );
	sc=rpclv1prd_shift.regHist(rpclv1Trigger_cond_LowPt_phi);
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "Trigger_Condition_LowPt_Phi Failed to register histogram " );       
	    return sc;
	  } 
	rpclv1Trigger_cond_LowPt_phi->SetFillColor(42);
	rpclv1Trigger_cond_LowPt_phi->GetXaxis()->SetTitle("Trigger Conditions: 0=Pivot0, 1=Pivot1, 2=LowPt0, 3=LowPt1");
	rpclv1Trigger_cond_LowPt_phi->GetYaxis()->SetTitle("Counts");
	rpclv1Trigger_cond_LowPt_phi->GetXaxis()->SetBinLabel(1,"None");     
	rpclv1Trigger_cond_LowPt_phi->GetXaxis()->SetBinLabel(1,"None");
      
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1Trigger_cond_LowPt_phi << m_rpclv1Trigger_cond_LowPt_phi_title.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked Trigger_Condition_LowPt_Phi successfully" ); 	
	      
	//////////////////////////////////////////////
      
	// Trigger_Cond LowPt ETA
	std::string m_generic_path_rpclv1Trigger_cond_LowPt_eta = m_generic_path_rpclv1monitoring+"/Overview";
	std::string m_rpclv1Trigger_cond_LowPt_eta_title = "Trigger_Condition_LowPt_Eta";
	const char* m_rpclv1Trigger_cond_LowPt_eta_title_char = m_rpclv1Trigger_cond_LowPt_eta_title.c_str();
	TH1* rpclv1Trigger_cond_LowPt_eta = new TH1I( m_rpclv1Trigger_cond_LowPt_eta_title_char, m_rpclv1Trigger_cond_LowPt_eta_title_char, 16, 0, 16 );
	sc=rpclv1prd_shift.regHist(rpclv1Trigger_cond_LowPt_eta);
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "Trigger_Condition_LowPt_eta Failed to register histogram " );       
	    return sc;
	  } 
	rpclv1Trigger_cond_LowPt_eta->SetFillColor(42);
	rpclv1Trigger_cond_LowPt_eta->GetXaxis()->SetTitle("Trigger Conditions: 0=Pivot0, 1=Pivot1, 2=LowPt0, 3=LowPt1");
	rpclv1Trigger_cond_LowPt_eta->GetYaxis()->SetTitle("Counts");
	rpclv1Trigger_cond_LowPt_eta->GetXaxis()->SetBinLabel(1,"None");     
	rpclv1Trigger_cond_LowPt_eta->GetXaxis()->SetBinLabel(1,"None");
      
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1Trigger_cond_LowPt_eta << m_rpclv1Trigger_cond_LowPt_eta_title.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked Trigger_Condition_LowPt_eta successfully" ); 
      
	/////////////////////////////////////////////
      
	// Trigger_Cond HighPt PHI
	std::string m_generic_path_rpclv1Trigger_cond_HighPt_phi = m_generic_path_rpclv1monitoring+"/Overview";
	std::string m_rpclv1Trigger_cond_HighPt_phi_title = "Trigger_Condition_HighPt_Phi";
	const char* m_rpclv1Trigger_cond_HighPt_phi_title_char = m_rpclv1Trigger_cond_HighPt_phi_title.c_str();
	TH1* rpclv1Trigger_cond_HighPt_phi = new TH1I( m_rpclv1Trigger_cond_HighPt_phi_title_char, m_rpclv1Trigger_cond_HighPt_phi_title_char, 16, 0, 16 );
	sc=rpclv1prd_shift.regHist(rpclv1Trigger_cond_HighPt_phi);
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "Trigger_Condition_HighPt_Phi Failed to register histogram " );       
	    return sc;
	  } 
	rpclv1Trigger_cond_HighPt_phi->SetFillColor(42);
	rpclv1Trigger_cond_HighPt_phi->GetXaxis()->SetTitle("Trigger Conditions: 0=Trigger from LowPt, 2=HighPt0, 3=HighPt1");
	rpclv1Trigger_cond_HighPt_phi->GetYaxis()->SetTitle("Counts");
	rpclv1Trigger_cond_HighPt_phi->GetXaxis()->SetBinLabel(1,"None");     
	rpclv1Trigger_cond_HighPt_phi->GetXaxis()->SetBinLabel(1,"None");
      
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1Trigger_cond_HighPt_phi << m_rpclv1Trigger_cond_HighPt_phi_title.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked Trigger_Condition_HighPt_Phi successfully" ); 
	
	///////////////////////////////////////////////
      
	// Trigger_Cond HighPt ETA
	std::string m_generic_path_rpclv1Trigger_cond_HighPt_eta = m_generic_path_rpclv1monitoring+"/Overview";
	std::string m_rpclv1Trigger_cond_HighPt_eta_title = "Trigger_Condition_HighPt_Eta";
	const char* m_rpclv1Trigger_cond_HighPt_eta_title_char = m_rpclv1Trigger_cond_HighPt_eta_title.c_str();
	TH1* rpclv1Trigger_cond_HighPt_eta = new TH1I( m_rpclv1Trigger_cond_HighPt_eta_title_char, m_rpclv1Trigger_cond_HighPt_eta_title_char, 16, 0, 16 );
	sc=rpclv1prd_shift.regHist(rpclv1Trigger_cond_HighPt_eta);
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "Trigger_Condition_HighPt_eta Failed to register histogram " );       
	    return sc;
	  } 
	rpclv1Trigger_cond_HighPt_eta->SetFillColor(42);
	rpclv1Trigger_cond_HighPt_eta->GetXaxis()->SetTitle("Trigger Conditions: 0 =Trigger from LowPt, 2=HighPt0, 3=HighPt1");
	rpclv1Trigger_cond_HighPt_eta->GetYaxis()->SetTitle("Counts");
	rpclv1Trigger_cond_HighPt_eta->GetXaxis()->SetBinLabel(1,"None");     
	rpclv1Trigger_cond_HighPt_eta->GetXaxis()->SetBinLabel(1,"None");
      
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1Trigger_cond_HighPt_eta << m_rpclv1Trigger_cond_HighPt_eta_title.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked Trigger_Condition_HighPt_eta successfully" ); 
      
	/////////////////////////////////////////
      
	// set label for Trigger Condition histograms
	char XLabel[1000];
	for(int ie=1;ie!=16;ie++){
	  sprintf(XLabel,"L=");
	  int first = 0 ;
	  for(int layer=0;layer!=4;layer++){
	    int bit = (ie>>layer) & 1  ;
	    if(bit==1){
	      if(first==0){
		sprintf(XLabel,"%s%d",XLabel,layer);
	      }
	      else{
		sprintf(XLabel,"%s,%d",XLabel,layer);
	      }
	      first=1;
	      rpclv1Trigger_cond_LowPt_phi->GetXaxis()->SetBinLabel(ie+1,XLabel);
	      rpclv1Trigger_cond_LowPt_phi->GetXaxis()->SetBinLabel(ie+1,XLabel);
	    
	      rpclv1Trigger_cond_LowPt_eta->GetXaxis()->SetBinLabel(ie+1,XLabel);
	      rpclv1Trigger_cond_LowPt_eta->GetXaxis()->SetBinLabel(ie+1,XLabel);
	    
	      rpclv1Trigger_cond_HighPt_phi->GetXaxis()->SetBinLabel(ie+1,XLabel);
	      rpclv1Trigger_cond_HighPt_phi->GetXaxis()->SetBinLabel(ie+1,XLabel);
	    
	      rpclv1Trigger_cond_HighPt_eta->GetXaxis()->SetBinLabel(ie+1,XLabel);
	      rpclv1Trigger_cond_HighPt_eta->GetXaxis()->SetBinLabel(ie+1,XLabel);
	    }
	  }
	}
         
	////////////////////////////////////////////////////////
    
	/*  
	//mean time-trigger time
	std::string m_generic_path_rpclv1triggertimemean = m_generic_path_rpclv1monitoring+"/Overview";
	std::string m_rpclv1triggertimemean_title = "Mean time - Mean Trigger time";    
	const char* m_rpclv1triggertimemean_title_char = m_rpclv1triggertimemean_title.c_str ();     
	TH2 *rpclv1triggertimemean=new TH2I(m_rpclv1triggertimemean_title_char,m_rpclv1triggertimemean_title_char, 1000, 0, 1000, 64, 0, 64 );
	sc=rpclv1prd_shift.regHist(rpclv1triggertimemean) ;  
	if(sc.isFailure())
	{ ATH_MSG_FATAL ( "rpclv1triggertimemean Failed to register histogram " );       
	return sc;
	}  	   		
	rpclv1triggertimemean->SetFillColor(42);
	rpclv1triggertimemean->GetXaxis()->SetTitle("16channelgroup + ijk*2 + Cma*12 + Tower*100");
	rpclv1triggertimemean->GetYaxis()->SetTitle("SectorLogic");
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1triggertimemean << m_generic_path_rpclv1triggertimemean.c_str() );
	ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked rpclv1triggertimemean successfully" ); 	
    
	////////////////////////////////////////////// 
  
	//RMS time-trigger time
	std::string m_generic_path_rpclv1triggertimeRMS = m_generic_path_rpclv1monitoring+"/Overview";
	std::string m_rpclv1triggertimeRMS_title = "RMS of time - Trigger time";    
	const char* m_rpclv1triggertimeRMS_title_char = m_rpclv1triggertimeRMS_title.c_str ();     
	TH2 *rpclv1triggertimeRMS=new TH2I(m_rpclv1triggertimeRMS_title_char,m_rpclv1triggertimeRMS_title_char, 1000, 0, 1000, 64, 0, 64 );
	sc=rpclv1prd_shift.regHist(rpclv1triggertimeRMS) ;  
	if(sc.isFailure())
	{ ATH_MSG_FATAL ( "rpclv1triggertimeRMS Failed to register histogram " );       
	return sc;
	}  	   		 
	rpclv1triggertimeRMS->SetFillColor(42);
	rpclv1triggertimeRMS->GetXaxis()->SetTitle("16channelgroup + ijk*2 + Cma*12 + Tower*100);
	rpclv1triggertimeRMS->GetYaxis()->SetTitle("SectorLogic");
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1triggertimeRMS << m_generic_path_rpclv1triggertimeRMS.c_str() );
	ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked rpclv1triggertimeRMS successfully" );	
  
	*/

	////////////////////////////////////////////// 
        
	//BCid_per_TriggerType
	std::string m_rpclv1_BCid_per_TriggerType_title = "rpclv1_BCid_per_TriggerType";      
	const char* m_rpclv1_BCid_per_TriggerType_title_char = m_rpclv1_BCid_per_TriggerType_title.c_str();
       
	TH2 *rpclv1_BCid_per_TriggerType=new TH2I( m_rpclv1_BCid_per_TriggerType_title_char,m_rpclv1_BCid_per_TriggerType_title_char,8, 0, 8, 8*2, 0, 16); 				
	sc=rpclv1_shift_dqmf.regHist(rpclv1_BCid_per_TriggerType) ;  
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1_BCid_per_TriggerType Failed to register histogram " );       
	    return sc;
	  }
	rpclv1_BCid_per_TriggerType->SetFillColor(42);
	rpclv1_BCid_per_TriggerType->GetXaxis()->SetTitle("Trigger Type");
	rpclv1_BCid_per_TriggerType->GetYaxis()->SetTitle("BCId+HPtTrigger*8 for trigger hits");
	rpclv1_BCid_per_TriggerType->SetOption("COLZ");
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1_BCid_per_TriggerType );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked rpclv1_BCid_per_TriggerType successfully" ); 
	//
      
	//BCid_per_TriggerCorrelation
	std::string m_rpclv1_BCid_per_TriggerCorr_title = "rpclv1_BCid_per_TriggerCorrelation";      
	const char* m_rpclv1_BCid_per_TriggerCorr_title_char = m_rpclv1_BCid_per_TriggerCorr_title.c_str();
       
	TH2 *rpclv1_BCid_per_TriggerCorr=new TH2I(m_rpclv1_BCid_per_TriggerCorr_title_char,m_rpclv1_BCid_per_TriggerCorr_title_char, 256, 0, 256, 8*2, 0, 16); 				
	sc=rpclv1prd_shift.regHist(rpclv1_BCid_per_TriggerCorr) ;  
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1_BCid_per_TriggerCorr Failed to register histogram " );       
	    return sc;
	  }
	rpclv1_BCid_per_TriggerCorr->SetFillColor(42);
	rpclv1_BCid_per_TriggerCorr->GetXaxis()->SetTitle("Trigger Corr");
	rpclv1_BCid_per_TriggerCorr->GetYaxis()->SetTitle("BCId+HPtTrigger*8 for trigger hits");
	rpclv1_BCid_per_TriggerCorr->SetOption("COLZ");
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1_BCid_per_TriggerCorr );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked rpclv1_BCid_per_TriggerCorr successfully" ); 	
	//

      
        	
	  // cool histogram
	  // strip profile -> noise and dead strips
	  if ( m_doCoolDB ) {
	    //DB_list.push_back( "StripId" );
	    DB_list.push_back( "ProfileCabling" );
	    DB_list.push_back( "ProfileDataCModd"  );
	    DB_list.push_back( "ProfileDataCMeven" );
	
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
	    
	    
      
  for(uint16_t side	   =  0; side   != 2  ; side   ++) {
  for(uint16_t slogic      =  0; slogic !=32  ; slogic ++) {
  for(uint16_t padId       =  0; padId  != 9  ; padId  ++) {
  for(uint16_t cmaId       =  0; cmaId  != 8  ; cmaId  ++) {
  for(uint16_t ijk	   =  0; ijk    != 6  ; ijk    ++) {if(cmaId>3&&ijk<2)continue;
  for(uint16_t channel     =  0; channel!=32  ; channel++) {
      
  // Get the list of offline channels corresponding to the online identifier
  std::list<Identifier> idList = m_cabling->give_strip_id(side, slogic, padId, cmaId, ijk, channel);
  std::list<Identifier>::const_iterator it_list;
 
  for (it_list=idList.begin() ; it_list != idList.end() ; ++it_list) {
   
    // and add the digit to the collection
    Identifier prdcoll_id = *it_list;

    //    // RPC digits do not hold anymore time of flight : digit time (and RDO time) is TOF subtracted
    //    // recalculate the time of flight in case it was not in the RDOs
    //    if (time==0) {
    //      // get the digit position
    //      const MuonGM::RpcReadoutElement* descriptor =
    //  m_muonMgr->getRpcReadoutElement(stripOfflineId);
    //     
    //      const HepGeom::Point3D<double> stripPos = descriptor->stripPos(stripOfflineId);
    //      // TEMP : set the time of flight from the digit position
    //      // temporary solution
    //      time = static_cast<int> ( stripPos.distance()/(299.7925*CLHEP::mm/CLHEP::ns) );
    //   
    //
    
    std::vector<std::string>   rpclayersectorsidename = RpcLayerSectorSideName(prdcoll_id, 0)  ; 
    std::string                sector_dphi_layer      = rpclayersectorsidename[12]             ;
	      
    std::vector<int>           RpcStrip = RpcStripShift(prdcoll_id, 0);
    int strip_dbindex        = (RpcStripShift(prdcoll_id, 0)).at(16);// cool strip profile
		if ( m_doCoolDB ) {
		  sc = rpcCoolDb.getHist( rpcCool_PanelIdHist, sector_dphi_layer+"_ProfileCabling" ) ;
		  if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get " << sector_dphi_layer << "_ProfileCabling" );
  
		  if ( rpcCool_PanelIdHist ) {		      
			rpcCool_PanelIdHist->Fill( strip_dbindex );
		  }
		}
    
    }
    }}}}}}  
	    
	    
	  } // end if (m_doCoolDB)
            
      }//isNewRun     

  }//AthenaMonManager::tier0 || AthenaMonManager::tier0Raw 



  return sc;
}



StatusCode RpcLv1RawDataValAlg::bookRPCLV1cmatimevschHistograms(std::string m_sectorlogic_name, std::string m_tower_name, std::string m_cma_name)
{
 
  StatusCode sc = StatusCode::SUCCESS;
 
  if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0Raw ) {  
 		    
    //declare a group of histograms
    std::string m_generic_path_rpclv1monitoring = "Muon/MuonRawDataMonitoring/RPCLV1";
    MonGroup rpclv1prd_expert( this, m_generic_path_rpclv1monitoring+"/TriggerRoad/"+m_sectorlogic_name, run, ATTRIB_UNMANAGED );   
    MuonDQAHistList& lst = m_stationHists.getList( m_sectorlogic_name );
    
    std::string m_generic_path_RPCLV1cmatimevsch = m_generic_path_rpclv1monitoring+"/"+m_sectorlogic_name+"/"+m_tower_name+"/rpclv1cosmic_cmatimevsch"+m_cma_name;
    std::string m_RPCLV1cmatimevsch_title = m_sectorlogic_name + "_" + m_tower_name + m_cma_name +"_time_vs_channel";     
    const char* m_RPCLV1cmatimevsch_title_char = m_RPCLV1cmatimevsch_title.c_str();
    TH2 *RPCLV1cmatimevsch=new TH2I(m_RPCLV1cmatimevsch_title_char,m_RPCLV1cmatimevsch_title_char, 32*7/m_rpclv1reducenbins, 0, 32*7, 64, 0,64);                       
    lst.addHist(RPCLV1cmatimevsch);
    RPCLV1cmatimevsch->SetFillColor(42);  
    RPCLV1cmatimevsch->SetMarkerColor(1);  
    RPCLV1cmatimevsch->SetMarkerStyle(21);   
    RPCLV1cmatimevsch->SetMarkerSize(0.2);
    RPCLV1cmatimevsch->GetXaxis()->SetTitle("Channel + 32 * ijk"     );
    RPCLV1cmatimevsch->GetYaxis()->SetTitle("Time=bcId*8+ticks"      );
     
    ATH_MSG_DEBUG ( "INSIDE bookRPCLV1cmatimevschHistograms : " << RPCLV1cmatimevsch << m_generic_path_RPCLV1cmatimevsch.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run ); 
  
    sc = rpclv1prd_expert.regHist( RPCLV1cmatimevsch );
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register RPCLV1cmatimevsch hist to MonGroup" );
 
    std::string m_generic_path_RPCLV1cmatimetriggervsch = m_generic_path_rpclv1monitoring+"/"+m_sectorlogic_name+"/"+m_tower_name+"/rpclv1cosmic_cmatimetriggervsch"+m_cma_name; 
    std::string m_RPCLV1cmatimetriggervsch_title = m_sectorlogic_name + "_" + m_tower_name + m_cma_name +"_(time-triggertime)_vs_channel";        
    const char* m_RPCLV1cmatimetriggervsch_title_char = m_RPCLV1cmatimetriggervsch_title.c_str ();
                 
    TH2 *RPCLV1cmatimetriggervsch=new TH2I(m_RPCLV1cmatimetriggervsch_title_char,m_RPCLV1cmatimetriggervsch_title_char, 32*7/m_rpclv1reducenbins, 0, 32*7, 2*64,-64, 64);             
    lst.addHist(RPCLV1cmatimetriggervsch);
    RPCLV1cmatimetriggervsch->SetFillColor(42);  
    RPCLV1cmatimetriggervsch->SetMarkerColor(1);  
    RPCLV1cmatimetriggervsch->SetMarkerStyle(21);   
    RPCLV1cmatimetriggervsch->SetMarkerSize(0.2);
    RPCLV1cmatimetriggervsch->GetXaxis()->SetTitle("Channel + 32 * ijk"     );
    RPCLV1cmatimetriggervsch->GetYaxis()->SetTitle("Time=bcId*8+ticks"      );
     
    ATH_MSG_DEBUG ( "INSIDE bookRPCLV1cmatimevschHistograms : " << RPCLV1cmatimetriggervsch << m_generic_path_RPCLV1cmatimetriggervsch.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run ); 
  
    sc = rpclv1prd_expert.regHist( RPCLV1cmatimetriggervsch );
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register RPCLV1cmatimetriggervsch hist to MonGroup" );

  }//AthenaMonManager::tier0 || AthenaMonManager::tier0Raw 
  return sc ;
    
}
 
StatusCode  RpcLv1RawDataValAlg::bookRPCLV1TriggerRoadHistograms(std::string m_sectorlogic_name, std::string m_tower_name, std::string m_cma_name, std::string m_thr_name)

{  
  // Trigger Road LowPt vs ijk channel   
  StatusCode sc = StatusCode::SUCCESS;

  if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0Raw  ) { 

    //declare a group of histograms
    std::string m_generic_path_rpclv1monitoring = "Muon/MuonRawDataMonitoring/RPCLV1";
    MonGroup rpclv1prd_expert( this, m_generic_path_rpclv1monitoring+"/TriggerRoad/"+m_sectorlogic_name, run, ATTRIB_UNMANAGED );   
    MuonDQAHistList& lst = m_stationHists.getList( m_sectorlogic_name );
  
    std::string m_generic_path_RPCLV1TriggerRoad =  m_generic_path_rpclv1monitoring+"/"+m_sectorlogic_name+"/"+m_tower_name+"/rpclv1cosmic_TriggerRoad"+m_cma_name+m_thr_name;
    std::string m_RPCLV1TriggerRoad_title = m_sectorlogic_name + "_" + m_tower_name + m_cma_name + m_thr_name + "_TriggerRoad";     
    const char* m_RPCLV1TriggerRoad_title_char = m_RPCLV1TriggerRoad_title.c_str();
    TH2 *RPCLV1TriggerRoad=new TH2I(m_RPCLV1TriggerRoad_title_char,m_RPCLV1TriggerRoad_title_char, 32/m_rpclv1reducenbins, 0, 32, 64/m_rpclv1reducenbins, 0, 64);                       
    lst.addHist(RPCLV1TriggerRoad);
    RPCLV1TriggerRoad->SetOption("COLZ");
    RPCLV1TriggerRoad->GetXaxis()->SetTitle("Channel  (ijk=6)"  );
    RPCLV1TriggerRoad->GetYaxis()->SetTitle("Confirm channel"   );
     
    ATH_MSG_DEBUG ( "INSIDE bookRPCLV1TriggerRoadHistograms : " << RPCLV1TriggerRoad << m_generic_path_RPCLV1TriggerRoad.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run ); 
  
    sc = rpclv1prd_expert.regHist( RPCLV1TriggerRoad );
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register RPCLV1TriggerRoad hist to MonGroup" );

  }//AthenaMonManager::tier0 || AthenaMonManager::tier0Raw
  return sc ; 
        
}

///  profile cabling 
//void RpcLv1RawDataValAlg::bookRPCLV1ProfilesHistograms(int m_i_sector, std::string m_sectorlogic_name, int m_i_cmaId, std::string m_cma_name, int m_i_ijk, std::string m_ijk_name) 
StatusCode RpcLv1RawDataValAlg::bookRPCLV1ProfilesHistograms(int m_i_sector, std::string m_sectorlogic_name, std::string m_cma_name, int m_i_ijk, std::string m_ijk_name) 
{  
  // book profiles histograms
  
  StatusCode sc = StatusCode::SUCCESS;
  if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0Raw  ) { 
  
    N_pad = 6            ;  
    cm_ch_label  =  "64" ;
    pad_ch_label = "128" ;
  
    //declare a group of histograms
    std::string m_generic_path_rpclv1monitoring = "Muon/MuonRawDataMonitoring/RPCLV1";
    MonGroup rpclv1prd_expert( this, m_generic_path_rpclv1monitoring+"/Profiles/"+m_sectorlogic_name, run, ATTRIB_UNMANAGED );   
    MuonDQAHistList& lst = m_stationHists.getList( m_sectorlogic_name );
  
    std::string m_generic_path_RPCLV1Profiles = m_generic_path_rpclv1monitoring+"/"+m_sectorlogic_name+"_"+m_cma_name+"_"+m_ijk_name+"_profiles";
  
    if ( (m_i_sector % 4)==0 || (m_i_sector % 4)==3 )  { 
      N_pad = 7; 
    }
    if ( m_i_ijk<2 || m_i_ijk==6  ) { 
      N_bin_profile = 2 * 32 * N_pad ;
      cm_ch_label  = "32"  ;
      pad_ch_label = "64"  ; }
    else { N_bin_profile = 2 * 64 * N_pad; }
    /*
      if ( m_i_cmaId==0 || m_i_cmaId==1 ) { m_cma_name="Cma0_1" ; }
      if ( m_i_cmaId==2 || m_i_cmaId==3 ) { m_cma_name="Cma2_3" ; }
      if ( m_i_cmaId==4 || m_i_cmaId==5 ) { m_cma_name="Cma4_5" ; }
      if ( m_i_cmaId==6 || m_i_cmaId==7 ) { m_cma_name="Cma6_7" ; }
				       
      if ( m_i_ijk==2 || m_i_ijk==3 ) { m_ijk_name="ijk2_3" ; }
      if ( m_i_ijk==4 || m_i_ijk==5 ) { m_ijk_name="ijk4_5" ; }  */
    
    std::string m_RPCLV1Profiles_title = m_sectorlogic_name + "_" + m_cma_name+"_"+ m_ijk_name+"_Profiles";
    const char* m_RPCLV1Profiles_title_char = m_RPCLV1Profiles_title.c_str();
    TH1 *RPCLV1Profiles=new TH1I(m_RPCLV1Profiles_title_char,m_RPCLV1Profiles_title_char, N_bin_profile/m_rpclv1reducenbins, 0, N_bin_profile );
    lst.addHist(RPCLV1Profiles);
  
    std::string x_axis_title = "Channel+ (ijk-ijk_off)*32 +cm*"+cm_ch_label+" +pad*"+pad_ch_label ;
    const char* x_axis_title_char = x_axis_title.c_str();
    RPCLV1Profiles->GetXaxis()->SetTitle(x_axis_title_char );
    RPCLV1Profiles->GetYaxis()->SetTitle("Counts  "   );
    RPCLV1Profiles->SetFillColor(42) ;  // 38
     
    ATH_MSG_DEBUG ( "INSIDE bookRPCLV1ProfilesHistograms : " << RPCLV1Profiles << m_generic_path_RPCLV1Profiles.c_str() );
   // ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run ); 
 
    sc = rpclv1prd_expert.regHist( RPCLV1Profiles );
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register RPCLV1Profiles hist to MonGroup" );
  
  }//AthenaMonManager::tier0 || AthenaMonManager::tier0Raw 
  
  return sc ;     
}

 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

void RpcLv1RawDataValAlg::bookRPCCoolHistograms( std::vector<std::string>::const_iterator & m_iter, int m_isec, int m_idblPhi,
					      std::string m_layer ) 
{
  StatusCode sc = StatusCode::SUCCESS ;
  
  std::string m_generic_path_rpcmonitoring = "Muon/MuonRawDataMonitoring/RPCLV1";
  MonGroup rpcCoolDb( this, m_generic_path_rpcmonitoring+"/CoolDB", run, ATTRIB_UNMANAGED );
  
  sprintf(histName_char,"Sector%.2d_%s_dblPhi%d", m_isec+1, m_layer.c_str(), m_idblPhi+1) ;
  // example: Sector01_Pivot0_dblPhi1_StripId
  
  histName  = histName_char  ;
  histName += "_"            ;
  histName += *m_iter        ;  //histName += m_coolQuantity ;
  istatPhi  = int( m_isec/2) ;
  iName     = 0              ;
  int kName = 0              ;
  int ig        = 0          ;
  int iNameMax  = 0          ;

  if ( m_isec==12) {
    // if ( m_layer.find("Pivot",0) )
    if ( m_layer == "Pivot0" || m_layer == "Pivot1" )   {
      iName     =  1          ;
      iNameMax  =  2          ;
      ir    = 2 	      ;		
      ig    = atoi( (m_layer.substr(5,1)).c_str() ) ;
    }
    if ( m_layer == "LowPt0" || m_layer == "LowPt1" )   {
      iName     =  1          ;
      iNameMax  =  2          ;
      ir    = 1 	      ;
      ig    = atoi( (m_layer.substr(5,1)).c_str() ) ;
    }
    if ( m_layer == "HighPt0" || m_layer == "HighPt1" ) {
      iName = 4               ;
      iNameMax  =  iName      ;
      ir    = 1 	      ;
      ig    = atoi( (m_layer.substr(6,1)).c_str() ) ; 
    }
  }//sector 13
  else if ( m_isec==11 ||  m_isec==13){
    if ( m_layer == "Pivot0" || m_layer == "Pivot1" )   {
      iName = 8 ;
      iNameMax  =  10;
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
  else {
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
   
  kName = iName ;
  if(iName==1)kName=53;
  
  int NTotStripsSideA = 1;
  int NTotStripsSideC = 1;     
 
  const MuonGM::RpcReadoutElement* rpc = m_muonMgr->getRpcRElement_fromIdFields( kName, 1 , istatPhi+1, ir, 1, m_idblPhi+1 );    
  
  if(rpc != NULL ){  
    Identifier idr = rpc->identify();
    std::vector<int>   rpcstripshift = RpcStripShift(idr, 0)  ;
    NTotStripsSideA = rpcstripshift[6]+rpcstripshift[17];
  } 
  rpc = m_muonMgr->getRpcRElement_fromIdFields( kName, -1 , istatPhi+1, ir, 1, m_idblPhi+1 );    
  
  if(rpc != NULL ){  
    Identifier idr = rpc->identify();
    std::vector<int>   rpcstripshift = RpcStripShift(idr, 0)  ;
    NTotStripsSideC = rpcstripshift[7]+rpcstripshift[18]; 
  }
  
  
  TH1 *rpcCoolHisto = new TH1F(histName.c_str(), histName.c_str(), NTotStripsSideC+NTotStripsSideA, -NTotStripsSideC, NTotStripsSideA );

  sc=rpcCoolDb.regHist(rpcCoolHisto) ;
  if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register " << histName << "hist to MonGroup" );
  rpcCoolHisto->GetXaxis()->SetTitle("strip");  
  
  
  
  // Fill strip Id histogram
  if ( (histName.find("ProfileCabling", 0)) != string::npos ) {
  
    sc = rpcCoolDb.getHist( rpcCool_PanelIdHist, histName.c_str() );
    if( sc.isFailure() ) ATH_MSG_WARNING (  "couldn't get "<< histName << " hist" );
    rpcCool_PanelIdHist->GetYaxis()->SetTitle("strip Id");
    rpcCool_PanelIdHist->SetBit(TH1::kIsAverage)         ;
    int rpcElemPhiStrip   ;
    int rpcElemEtaStrip   ;
    int coolStripIndex =0 ;
    
      
    for (int ieta=0; ieta!=17; ieta++) {
      for ( int iNameF=iName; iNameF!= iNameMax+1 ; iNameF++ ) {
         kName = iNameF ;
         if(iNameF==1)kName=53;
	 if (kName>10 && kName!=53) continue; 
	for (int iz=0; iz!=3; iz++ ) {
	  int irc = ir ;	
	  if(abs(ieta-8)==7&&ir==1&&kName==2)continue;	
	  if(m_isec==12&&abs(ieta-8)==6&&ir==1&&kName==2)continue;
	  if(abs(ieta-8)==7&&ir==2)irc=1; 
	  if(m_isec==12&&abs(ieta-8)==6&&ir==2)irc=1;	 
											   
    	  const MuonGM::RpcReadoutElement* rpc = m_muonMgr->getRpcRElement_fromIdFields(kName, ieta-8, istatPhi+1, irc, iz+1, m_idblPhi+1);  
    	  if( rpc == NULL ) continue;   
	  
    	  if  ( iz+1 != rpc->getDoubletZ() ) { 
    	    continue ;
    	  }
    	  Identifier idr = m_rpcIdHelper->parentID( rpc->identify() );
    	  rpcElemPhiStrip = int (rpc->NphiStrips() ) ;
    	  rpcElemEtaStrip = int (rpc->NetaStrips() ) ;
	  
	  
    	  for ( int istripEta=0; istripEta!=rpcElemEtaStrip; istripEta++ ) {
    	    Identifier strip_id  =  m_rpcIdHelper->channelID(idr, iz+1, m_idblPhi+1, ig+1, 0, istripEta+1) ;
    	    if( strip_id == 0 ) continue;
    	    coolStripIndex = (RpcStripShift(strip_id, 0)).at(16);
    	    rpcCool_PanelIdHist->Fill(coolStripIndex, -1) ;
          }
    	  for ( int istripPhi=0; istripPhi!=rpcElemPhiStrip; istripPhi++ ) {
    	    Identifier strip_id  =  m_rpcIdHelper->channelID(idr, iz+1, m_idblPhi+1, ig+1, 1, istripPhi+1) ;
    	    if( strip_id == 0 ) continue;
    	    coolStripIndex = (RpcStripShift(strip_id, 0)).at(16);
    	    rpcCool_PanelIdHist->Fill(coolStripIndex, -1 );
          }
	  
	  
	  
        } // end loop on doubletZ
      }
    }  // end loop on stationeta
 
  } // end fill cool histograms with panelId 
  
   
}  

 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

std::vector<int>  RpcLv1RawDataValAlg::RpcStripShift(Identifier prdcoll_id, int  irpctriggerInfo) 
{


  ATH_MSG_DEBUG ( "in RpcLv1RawDataValAlg::RpcStripShift" );  
     
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
  // const MuonGM::RpcReadoutElement* rpc = m_muonMgr->getRpcRElement_fromIdFields( irpcstationName, irpcstationEta, irpcstationPhi, irpcdoubletR, irpcdoubletZ, irpcdoubletPhi  );
  
  std::vector<int>  rpcstriptot  ;
  
  int NphiStrips	    = descriptor -> NphiStrips()* 2		     ;
  int ShiftPhiStrips        = descriptor -> NphiStrips()*(irpcdoubletPhi-1)  ;
  int NetaStrips	    = 0						     ;
  int ShiftEtaStrips        = 0						     ;
  int ShiftStrips	    = 0						     ;
  int NetaStripsTot         = 0						     ;
  int NetaStripsTotSideA    = 0						     ;
  int NetaStripsTotSideC    = 0						     ;
  int ShiftEtaStripsTot     = 0						     ;
  int Nbin  	            = 0						     ;
  int EtaStripSign          = 0						     ;
  int SectorLogic	    = 0						     ;
  int Side  	            = 0						     ;
  int PanelIndex	    = 0						     ;
  int Settore	            = 0						     ;
  int PlaneTipo	            = 0						     ;
  int strip_dbindex         = 0						     ;
  int ShiftPhiTot_db        = 0						     ;
  int NphiStripsTotSideA    = 0					             ;
  int NphiStripsTotSideC    = 0					             ;
  int NetaPanelsTot         = 0					             ;
  int ShiftEtaPanelsTot     = 0					             ;
  int NetaPanelsTotSideA    = 0					             ;
  int NetaPanelsTotSideC    = 0					             ;
  int ShiftStripPhiAtlas    = 0					             ;
  
  //1***
  //evaluate sector logic
  //2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG,53=BME
  SectorLogic  = (irpcstationPhi - 1) * 4 ; 
  SectorLogic +=  irpcdoubletPhi          ;
  Settore      = (irpcstationPhi -1) * 2  ;
  if( (irpcstationName==3||irpcstationName==5||irpcstationName>7) && (irpcstationName<11) ){
    SectorLogic+=2;
    Settore++; 
  }
  SectorLogic   = SectorLogic - 2  ; 
  Settore = Settore +1 ;
  if(SectorLogic==-1)SectorLogic=31;
  if(irpcstationEta>0){Side = 1     ;SectorLogic+=32;}
  
  //evaluate plane type
  //2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG,53=BME
  if( irpcstationName==2||irpcstationName==3||irpcstationName==8||irpcstationName==53 ){
    if(irpcdoubletR==1){
      PlaneTipo=0;
    }
    else {PlaneTipo=1;}
  }			
  else {PlaneTipo=2;}
  //Extension feet Pivot
  if(irpcdoubletR==2)PlaneTipo=1;
  //BML7 assigned to pivot 
  if( irpcstationName==2 && ( (abs(irpcstationEta)==7)||(irpcstationPhi==7&&abs(irpcstationEta)==6) ) )PlaneTipo=1;;
  
  //evaluate strip shift
  //2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG,53=BME
  /////// NB !!!!!
  // the eta strip number increases going far away from IP
  // the phi strip number increases going from HV side to RO side
     
    for(int keta=0; keta!= 9; keta++){
      int ieta = keta ;
      if(irpcstationEta<0)ieta = - keta ;
      int krpcstationName    = irpcstationName ;
      int krpcstationNameMin = irpcstationName ;
      int krpcstationNameMax = irpcstationName ;
      if((irpcstationName==2||irpcstationName==53)&&irpcstationPhi==7){ // sector 13 with BME
         krpcstationNameMin = 1; 
         krpcstationNameMax = 2;	 
      }  
      if(irpcstationName>=9&&irpcstationName<=10&&irpcdoubletR==1){ // feet extension
         krpcstationNameMin =  9; 
         krpcstationNameMax = 10; 
      }  
      if(irpcstationName>=8&&irpcstationName<=10&&irpcdoubletR==2){ // feet extension
         krpcstationNameMin =  8; 
         krpcstationNameMax = 10; 
      } 
      
      for(int jrpcstationName=krpcstationNameMin; jrpcstationName!=krpcstationNameMax+1; jrpcstationName++){
       if (jrpcstationName>10 && jrpcstationName!=53) continue; 
       krpcstationName = jrpcstationName ;
       if(krpcstationName==1)krpcstationName = 53 ; //BME
      for(int idbz=1; idbz!= 4; idbz++){
    	const MuonGM::RpcReadoutElement* rpc = m_muonMgr->getRpcRElement_fromIdFields(krpcstationName, ieta, irpcstationPhi, irpcdoubletR, idbz, 1 );
    	if(rpc != NULL ){
		
	  if ( idbz != rpc->getDoubletZ() ) continue ;
	  
    	  if( ieta==irpcstationEta ){if(krpcstationName == irpcstationName ){
    	    if( idbz==irpcdoubletZ ){ 
	     ShiftEtaStrips    = NetaStrips;
	     if(irpcstationEta<0){ 
	      ShiftEtaStripsTot = NetaStripsTotSideC  ;  
	      ShiftPhiTot_db    = NphiStripsTotSideC  ;
	      ShiftEtaPanelsTot = NetaPanelsTotSideC  ;
	     }
	     else{ 
	      ShiftEtaStripsTot = NetaStripsTotSideA  ;  
	      ShiftPhiTot_db    = NphiStripsTotSideA  ;
	      ShiftEtaPanelsTot = NetaPanelsTotSideA  ;
	     }
	    }
    	    NetaStrips    +=  rpc->NetaStrips()  ;
    	  }}
	  
    	  NetaStripsTot       +=  rpc->NetaStrips()  ;
    	  NetaPanelsTot       ++                     ;
	  if(irpcstationEta<0){
	   NetaStripsTotSideC  +=  rpc->NetaStrips()  ;
	   NphiStripsTotSideC  +=  rpc->NphiStrips()  ;
    	   NetaPanelsTotSideC  ++                     ;
	  }
	  else{ 
	   NetaStripsTotSideA  +=  rpc->NetaStrips()  ;
	   NphiStripsTotSideA  +=  rpc->NphiStrips()  ;
    	   NetaPanelsTotSideA  ++                     ;
	  }
    	
	  
        } //check if rpc!=NULL
      } //for loop in idbz
     } //for loop in krpcstationName
    } // for loop in etastation     
  
  Nbin	      = NetaStrips     ;
  ShiftStrips = ShiftEtaStrips ;
  //re-define for phi view
  if(irpcmeasuresPhi==1) {
    Nbin = NphiStrips ;	   
    ShiftStrips =  ShiftPhiStrips;
  }
     
  EtaStripSign =  1	                   ; 
  if(irpcstationEta<0)  EtaStripSign = -1  ; 
  		
  if(irpcdoubletR==1){
   if     (irpcstationName==2||irpcstationName==53 ){ShiftStripPhiAtlas = (2*48+64*2) * ( irpcstationPhi -1 )        ;}
   else if(irpcstationName==3                      ){ShiftStripPhiAtlas = (2*48+64*2) * ( irpcstationPhi -1 ) + 2*64 ;}
   else if(irpcstationName==8                      ){ShiftStripPhiAtlas = (2*48+64*2) * ( irpcstationPhi -1 ) + 2*64 ;}
   else if(irpcstationName==4		           ){ShiftStripPhiAtlas = (2*64+80*2) * ( irpcstationPhi -1 )	     ;}
   else{ShiftStripPhiAtlas = (2*64+80*2) * ( irpcstationPhi -1 ) + 2*80 ;} 
   //BML7 assigned to pivot 
   if( irpcstationName==2 && abs(irpcstationEta)==7){
    ShiftStripPhiAtlas = (2*48+64*2) * ( irpcstationPhi -1 );
    if(irpcstationPhi==8)ShiftStripPhiAtlas +=4*16;
   }
   if( irpcstationName==2 && irpcstationPhi==7&&abs(irpcstationEta)==6 ){
    ShiftStripPhiAtlas = (2*48+64*2) * ( irpcstationPhi -1 ) +2*16      ;
   }
  } 		
  if(irpcdoubletR==2){
   if(irpcstationPhi<=6){   
    if     (irpcstationName==2                      ){ShiftStripPhiAtlas = (2*48+64*2) * ( irpcstationPhi -1 )        ;}
    else if(irpcstationName==3                      ){ShiftStripPhiAtlas = (2*48+64*2) * ( irpcstationPhi -1 ) + 2*64 ;} 
    else if(irpcstationName>=8&&irpcstationName<=10 ){ShiftStripPhiAtlas = (2*48+64*2) * ( irpcstationPhi -1 ) + 2*64 ;}                    
   }
   else if(irpcstationPhi==7){   
    if     (irpcstationName==2||irpcstationName==53 ){ShiftStripPhiAtlas = (2*48+64*2) * ( irpcstationPhi -1 )+2*16     ;}
    else if(irpcstationName>=8&&irpcstationName<=10 ){ShiftStripPhiAtlas = (2*48+64*2) * ( irpcstationPhi -1 )+2*16+2*64;}
   }
   else if(irpcstationPhi==8){   
    if     (irpcstationName==2                      ){ShiftStripPhiAtlas = (2*48+64*2) * ( irpcstationPhi -1 ) +4*16      ;}
    else if(irpcstationName==3                      ){ShiftStripPhiAtlas = (2*48+64*2) * ( irpcstationPhi -1 ) +4*16+2*64 ;}
   }
  }
  //2***
  // cool db strip index
  if(irpcmeasuresPhi==0) {
    strip_dbindex = ( ShiftEtaStripsTot + irpcstrip ) * EtaStripSign ;
  }
  else {
    if ( irpcstationEta<0 ) { strip_dbindex= - NetaStripsTotSideC - ShiftPhiTot_db - irpcstrip ;}
    else { strip_dbindex = NetaStripsTotSideA + ShiftPhiTot_db + irpcstrip; }
  } 
  if ( irpcstationEta>=0 ) strip_dbindex = strip_dbindex -1;
  
  //3***
  //bin panel number for summary plots 
  int irpcstationName_index = irpcdoubletR -1 ; // 0 LowPt, 1 Pivot, 2 HighPt
  if ( irpcstationName !=2 && irpcstationName !=3 && irpcstationName !=8 && irpcstationName !=53) irpcstationName_index += 2 ;  
  

  PanelIndex = irpcmeasuresPhi + (irpcgasGap-1)*2 + (irpcdoubletPhi-1)*4 + (irpcdoubletZ-1)*8 + (irpcstationName_index)*24 
    + ( ( abs(irpcstationEta) ) )*72 ;
  // exception station name=53, assume irpcstationEta = 0 ;
  if(irpcstationName ==53)
  PanelIndex = irpcmeasuresPhi + (irpcgasGap-1)*2 + (irpcdoubletPhi-1)*4 + (irpcdoubletZ-1)*8 + (irpcstationName_index)*24;
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



  //4***
  //panel and tower consecutive index

  int tower_dbindex  = 0 ;  
  int panel_dbindex  = 0 ; 
  int lastname       = 0 ;
  int lastdoubletZ   = 0 ;
  int laststationEta = 0 ; 
  int krpcdoubletR   =  irpcdoubletR;
 
  for(int iphi = 1; iphi != 8*0+irpcstationPhi+1; iphi++ ){ 
    lastname    = 10;
    if(iphi==irpcstationPhi&&irpcstationName<10){
      lastname    = irpcstationName ;
    }
    for(int kname=      1; kname!=       10*0+lastname+1 ; kname++){
      int iname = kname ;
      if (kname == 1) iname= 53;
      if (iname>10 && iname!=53) continue; 
      laststationEta = 7 ;
   
      if(iname==irpcstationName&&iphi==irpcstationPhi){
	laststationEta    = irpcstationEta ;
	lastdoubletZ      = irpcdoubletZ   ;
      }
   
      for(int ieta = -7; ieta != 8*0+laststationEta+1; ieta++ ){
   
	krpcdoubletR   =  irpcdoubletR;   
	if( iname==2||iname==8 || iname==53){    
    
	  if(PlaneTipo!=1&&abs(ieta)>=6&&iphi==7)continue;
	  if(PlaneTipo!=1&&abs(ieta)>=7&&iphi!=7)continue;
	  if(PlaneTipo==1&&abs(ieta)==6&&iphi==7)krpcdoubletR   =  1;
	  if(PlaneTipo==1&&abs(ieta)==7&&iphi!=7)krpcdoubletR   =  1;
	  if(PlaneTipo==1&&abs(ieta) <6&&iphi==7)krpcdoubletR   =  2;
	  if(PlaneTipo==1&&abs(ieta) <7&&iphi!=7)krpcdoubletR   =  2;
    
	}    
	else if( iname==3 ){      
	  if(PlaneTipo>1)continue;
	}      
	else if( iname==6 || iname==7 ){      
	  continue;
	}       
	else if( (iname==4 || iname ==5 || iname == 9 ||  iname == 10) && (krpcdoubletR > 1) ) continue;       
           
	if(PlaneTipo==2 && krpcdoubletR > 1 )continue;
	if(PlaneTipo==2 &&  (iname==2 || iname ==3 || iname==8 || iname==53) )continue; 
	if(PlaneTipo <2 && !(iname==2 || iname ==3 || iname==8 || iname==53) )continue; 
    
	if(irpcstationEta>=0&&ieta<0){
	  continue;
	}
	else if(irpcstationEta<0&&ieta>=0){
	  continue;
	}
  
	lastdoubletZ   = 3 ;
   
	if(iname==irpcstationName&&iphi==irpcstationPhi&&ieta==irpcstationEta){
	  lastdoubletZ      = irpcdoubletZ   ;
	}    
 
	for(int iz   =      1; iz   !=      3*0+lastdoubletZ+1; iz++	){ 
	  
	  const MuonGM::RpcReadoutElement* rpc = m_muonMgr->getRpcRElement_fromIdFields(iname, ieta, iphi, krpcdoubletR, iz, 1);
	  //if(irpcstationPhi<=1)std::cout <<iname << " "<< ieta <<" "<< iphi<<" "<< iz<<" z "<< panel_dbindex<< std::endl; 
    
	  if(rpc == NULL )continue;
	  
	  panel_dbindex++;
	  
	  if(iz==1)tower_dbindex++;
	   
	   
	  //if(irpcstationPhi<=1)std::cout <<iname << " "<< ieta <<" "<< iphi<<" "<< iz<<" x "<< panel_dbindex<< std::endl; 
	  
	}}}}		  
  //if(irpcstationPhi<=1)std::cout << "PlaneTipo "<< PlaneTipo << " "<< irpcstationName<<" "<< irpcstationEta<<" "<< irpcstationPhi<<" "<< irpcdoubletR << " "<< irpcdoubletZ <<" "<< irpcdoubletPhi <<" panel_dbindex "<<  panel_dbindex<<" tower_dbindex "<< tower_dbindex<< std::endl; 
 
  for(int iphi = 1; iphi != 8*0+irpcstationPhi+1; iphi++ ){ 
    lastname    = 10;
    if(iphi==irpcstationPhi&&irpcstationName<10){
      lastname    = irpcstationName ;
    }
    for(int kname=      1; kname!=       10*0+lastname+1 ; kname++){
      int iname = kname ;
      if (kname == 1) iname= 53;
      if (iname>10 && iname!=53) continue; 
      laststationEta = 7 ;
   
      if(iname==irpcstationName&&iphi==irpcstationPhi){
	laststationEta    = irpcstationEta ;
	lastdoubletZ      = irpcdoubletZ   ;
      }
   
      for(int ieta = -7; ieta != 8*0+laststationEta+1; ieta++ ){
   
	krpcdoubletR   =  irpcdoubletR;   
	if( iname==2||iname==8 || iname==53){    
    
	  if(PlaneTipo!=1&&abs(ieta)>=6&&iphi==7)continue;
	  if(PlaneTipo!=1&&abs(ieta)>=7&&iphi!=7)continue;
	  if(PlaneTipo==1&&abs(ieta)==6&&iphi==7)krpcdoubletR   =  1;
	  if(PlaneTipo==1&&abs(ieta)==7&&iphi!=7)krpcdoubletR   =  1;
	  if(PlaneTipo==1&&abs(ieta) <6&&iphi==7)krpcdoubletR   =  2;
	  if(PlaneTipo==1&&abs(ieta) <7&&iphi!=7)krpcdoubletR   =  2;
    
	}    
	else if( iname==3 ){      
	  if(PlaneTipo>1)continue;
	}      
	else if( iname==6 || iname==7 ){      
	  continue;
	}       
	else if( (iname==4 || iname ==5 || iname == 9 ||  iname == 10) && (krpcdoubletR > 1) ) continue;       
           
	if(PlaneTipo==2 && krpcdoubletR > 1 )continue;
	if(PlaneTipo==2 &&  (iname==2 || iname ==3 || iname==8 || iname==53) )continue; 
	if(PlaneTipo <2 && !(iname==2 || iname ==3 || iname==8 || iname==53) )continue; 
    
	if(irpcstationEta>=0&&ieta<0){
	  continue;
	}
	else if(irpcstationEta<0&&ieta>=0){
	  continue;
	}
  
	lastdoubletZ   = 3 ;
   
	if(iname==irpcstationName&&iphi==irpcstationPhi&&ieta==irpcstationEta){
	  lastdoubletZ      = irpcdoubletZ   ;
	}    
 
	for(int iz   =      1; iz   !=      3*0+lastdoubletZ+1; iz++	){ 
	  
	  const MuonGM::RpcReadoutElement* rpc = m_muonMgr->getRpcRElement_fromIdFields(iname, ieta, iphi, krpcdoubletR, iz, 1);
	  //if(irpcstationPhi<=1)std::cout <<iname << " "<< ieta <<" "<< iphi<<" "<< iz<<" z "<< panel_dbindex<< std::endl; 
    
	  if(rpc == NULL )continue;
	  
	  panel_dbindex++;
	  
	  if(iz==1)tower_dbindex++;
	   
	   
	  //if(irpcstationPhi<=1)std::cout <<iname << " "<< ieta <<" "<< iphi<<" "<< iz<<" x "<< panel_dbindex<< std::endl; 
	  
	}}}}		  
  //std::cout << "PlaneTipo "<< PlaneTipo << " Name "<< irpcstationName<<" Eta "<< irpcstationEta<<" Phi "<< irpcstationPhi<<" dR "<< irpcdoubletR << " dZ "<< irpcdoubletZ<<" dPhi "<< irpcdoubletPhi <<" panel_dbindex "<<  panel_dbindex<<" tower_dbindex "<< tower_dbindex<< std::endl;    
  //  //calculate max panels and towers  
  //  int panel_dbindex1 = 0 ; 
  //  int tower_dbindex1 = 0 ;
  //  krpcdoubletR   =  irpcdoubletR;
  //  for(int iphi = 1; iphi != 8+1; iphi++ ){ 
  //   for(int iname=      2; iname!=       10+1 ; iname++){
  //    //     
  //    for(int ieta = -7; ieta != 8+1; ieta++ ){
  //    
  //     krpcdoubletR   =  irpcdoubletR;
  //     
  //     if( iname==2||iname==8 ){
  //  
  //     	if(PlaneTipo!=1&&abs(ieta)>=6&&iphi==7)continue;
  //     	if(PlaneTipo!=1&&abs(ieta)>=7&&iphi!=7)continue;
  //     	if(PlaneTipo==1&&abs(ieta)==6&&iphi==7)krpcdoubletR   =  1;
  //     	if(PlaneTipo==1&&abs(ieta)==7&&iphi!=7)krpcdoubletR   =  1;
  //     	if(PlaneTipo==1&&abs(ieta) <6&&iphi==7)krpcdoubletR   =  2;
  //     	if(PlaneTipo==1&&abs(ieta) <7&&iphi!=7)krpcdoubletR   =  2;
  //  
  //     }   
  //     else if( iname==3 ){      
  //      if(PlaneTipo>1)continue;
  //     }      
  //     else if( iname==6 || iname==7 ){      
  //      continue;
  //     } 
  //     else if( (iname==4 || iname ==5 || iname>8 ) && (krpcdoubletR > 1) ) continue;   
  //     if(PlaneTipo==2 && krpcdoubletR > 1 )continue;
  //     if(PlaneTipo==2 && (iname==2 || iname ==3 || iname==8 ) )continue;
  //     if(PlaneTipo <2 && !(iname==2 || iname ==3 || iname==8 ) )continue;  
  //      
  //     
  //     
  //       
  // 
  //     if(irpcstationEta>=0&&ieta<0){
  //      continue;
  //     }
  //     else if(irpcstationEta<0&&ieta>=0){
  //      continue;
  //     }
  //    
  //       for(int iz   =      1; iz   !=      3+1; iz++	){ 
  // 	  
  // 	  const MuonGM::RpcReadoutElement* rpc = m_muonMgr->getRpcRElement_fromIdFields(iname, ieta, iphi, krpcdoubletR , iz, 1 );
  // 	      
  // 	  if(rpc == NULL )continue;
  // 	  
  // 	  panel_dbindex1++;   
  // 	   
  // 	   
  // 	  if(iz==1)tower_dbindex1++;
  // 	  if(irpcstationPhi<=1&&iz==1)std::cout <<iname << " "<< ieta <<" "<< iphi<<" "<<irpcdoubletR <<" " << krpcdoubletR<< " " << iz<<" tower_dbindex "<<tower_dbindex1 << std::endl; 
  // 	  
  // 	  
  //   }}}}		 
  //   std::cout <<"PlaneTipo "<< PlaneTipo << " panel_dbindex1 "<< panel_dbindex1<< " tower_dbindex1 "<< tower_dbindex1<<std::endl; 

				    	  
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
  rpcstriptot.push_back(NetaPanelsTot         ); 
  rpcstriptot.push_back(ShiftEtaPanelsTot     ); 
  rpcstriptot.push_back(NetaPanelsTotSideC    );
  rpcstriptot.push_back(NetaPanelsTotSideA    );   //22 
  rpcstriptot.push_back(panel_dbindex         );
  rpcstriptot.push_back(tower_dbindex         );   //24
  rpcstriptot.push_back(ShiftStripPhiAtlas    );   //25
  
  //   std::cout << "----------------"<< std::endl;
  // 
  //   std::cout << "NphiStripsTotSideA "<<NphiStripsTotSideA<< " NphiStripsTotSideC "<<NphiStripsTotSideC<< " " <<irpcmeasuresPhi <<std::endl;
  //   std::cout << "NetaStripsTotSideA "<<NetaStripsTotSideA<< " NetaStripsTotSideC "<<NetaStripsTotSideC<<" ShiftEtaStripsTot "<< ShiftEtaStripsTot<< std::endl;
  //   std::cout << "NetaPanelsTotSideA "<<NetaPanelsTotSideA<< " NetaPanelsTotSideC "<<NetaPanelsTotSideC<<" ShiftEtaPanelsTot "<< ShiftEtaPanelsTot<< std::endl;
  //   
  
  return  rpcstriptot ;

}
    
std::vector<std::string>    RpcLv1RawDataValAlg::RpcLayerSectorSideName(Identifier prdcoll_id, int  irpctriggerInfo) 
{
  
  ATH_MSG_DEBUG (  "in RpcLvlRawDataValAlg::RpcLayerSectorSideName" );  
  
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
  	    
  //2=BML,3=BMS,4=BOL,5=BOS,8=BMF,9=BOF,10=BOG,53=BME
  if( irpcstationName==2 || irpcstationName==3 || irpcstationName==8 || irpcstationName==53 ){
    if(irpcdoubletR==1){
      layer_name="LowPt";
    }
    else {layer_name="Pivot";}
  }			
  else {layer_name="HighPt";}
  if(irpcdoubletR==2)layer_name="Pivot";
  //BML7 assigned to pivot 
  if( irpcstationName==2 && ( (abs(irpcstationEta)==7)||(irpcstationPhi==7&&abs(irpcstationEta)==6) ) )layer_name="Pivot";
	    
  if(irpcstationName==3||irpcstationName==5||irpcstationName==8||irpcstationName==9||irpcstationName==10){
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
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode RpcLv1RawDataValAlg::procHistograms()
{
 
  ATH_MSG_DEBUG ( "RpcLv1RawDataValAlg finalize()" );
  if(endOfEventsBlock){}
  if(endOfLumiBlock){}
  if(endOfRun){} 
  return StatusCode::SUCCESS;
}

 
//======================================================================================//
/**  finalize */
//======================================================================================//
StatusCode RpcLv1RawDataValAlg::finalize() 
{ 

  StatusCode sc = ManagedMonitorToolBase::finalize();
  if(!sc.isSuccess()) return sc;


  ATH_MSG_DEBUG ( "RpcLv1RawDataValAlg::finalize() " );

  
  return sc;
}
