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
 
  return StatusCode::SUCCESS;
}


StatusCode RpcLv1RawDataValAlg::StoreTriggerType() {
  const xAOD::EventInfo* eventInfo = nullptr;
  StatusCode sc = StatusCode::SUCCESS;
  sc = m_eventStore->retrieve(eventInfo);
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR ( "Could not find eventInfo " );
    return sc;
  }else {ATH_MSG_DEBUG ( "RpcLv1RawDataValAlg::retrieved eventInfo" );} 
  
  m_trigtype = eventInfo->level1TriggerType();

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

    int i_triggertimelowpt  =-1 ;    
    int i_triggertimehighpt =-1 ;  
    int n_triggerlowpt      = 0 ;  
    int n_triggerlowpt_eta  = 0 ;
    int n_triggerlowpt_phi  = 0 ;
    int n_triggerhighpt     = 0 ;
    int n_triggerhighpt_eta = 0 ;
    int n_triggerhighpt_phi = 0 ;
  
    int IJKtriggerLowPtPhi  = 0 ;
    int IJKtriggerLowPtEta  = 0 ;
    int IJKtriggerHighPtPhi = 0 ;
    int IJKtriggerHighPtEta = 0 ;
 
    int nFiredChannels = 0;  	
    
    int PhiOr[64][4][4][32] = {{{{0}}}};
  
    // Prepare the retrieval of the RpcPadContainer

    const RpcPadContainer* rpcRDO = nullptr;
    sc= (*m_activeStore)->retrieve(rpcRDO,"RPCPAD");
    if (sc.isFailure()) {
      ATH_MSG_ERROR ( "Could not find RPC Pads" );     
      return sc;
    }

    // begin loop to get trigger time
    for (RpcPadContainer::const_iterator rdoColli = rpcRDO->begin(); rdoColli!=rpcRDO->end(); ++rdoColli)
      {
	const RpcPad* rdoColl = *rdoColli;
	// Now loop on the RDO to find the ones with trigger hits ijk=6
	if ( (rdoColl)->size() != 0 ) {  	 
	  RpcPad::const_iterator itCM = (rdoColl)->begin();
	  for ( ; itCM != (rdoColl)->end() ; ++itCM ) {
	    RpcCoinMatrix::const_iterator itChan = (*itCM)->begin();	     
	     
	    for ( ; itChan != (*itCM)->end() ; ++itChan ) {	     
          
	      nFiredChannels++;
	  
	      int i_cmaId      = (*itCM)->onlineId()  ;
	      int i_bcId       = (*itChan)->bcid()    ;
	      int i_time       = (*itChan)->time()    ;
	      int i_ijk        = (*itChan)->ijk()     ;
			  
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
    std::string generic_path_rpclv1monitoring = "Muon/MuonRawDataMonitoring/RPCLV1";
  
    MonGroup rpclv1prd_shift( this, generic_path_rpclv1monitoring + "/Overview", run, ATTRIB_UNMANAGED );
    MonGroup rpclv1_shift_dqmf( this, generic_path_rpclv1monitoring + "/GLOBAL", run, ATTRIB_UNMANAGED ); 
    MonGroup rpcCoolDb( this, generic_path_rpclv1monitoring+"/CoolDB", run, ATTRIB_UNMANAGED )          ;
   
    TH1* rpclv1_hitperEvent = nullptr;
    sc  = rpclv1prd_shift.getHist(rpclv1_hitperEvent,"rpclv1_hitperEvent");
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register rpclv1_hitperEvent hist to MonGroup" );  
    rpclv1_hitperEvent->Fill(float(nFiredChannels)); 
  
    TH1* rpclv1triggerlowpt_etastat = nullptr;
    sc = rpclv1prd_shift.getHist(rpclv1triggerlowpt_etastat,"Trigger_Hits_LowPt_eta_Stat");
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register rpclv1triggerlowpt_etastat hist to MonGroup" );
    rpclv1triggerlowpt_etastat->Fill(float(n_triggerlowpt_eta)); 
  
    TH1* rpclv1triggerlowpt_phistat = nullptr;
    sc = rpclv1prd_shift.getHist(rpclv1triggerlowpt_phistat,"Trigger_Hits_LowPt_phi_Stat");
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register rpclv1triggerlowpt_phistat hist to MonGroup" );
    rpclv1triggerlowpt_phistat->Fill(float(n_triggerlowpt_phi)); 
  
    TH2* rpclv1triggerlowpt_etaphistat = nullptr;
    sc = rpclv1prd_shift.getHist(rpclv1triggerlowpt_etaphistat,"Trigger_Hits_Lowpt_etaphi_Stat");
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register Trigger_Hits_Lowpt_etaphi_Stat hist to MonGroup" );
    rpclv1triggerlowpt_etaphistat->Fill( float(n_triggerlowpt_phi), float(n_triggerlowpt_eta) );
   
    TH1* rpclv1triggerhighpt_etastat = nullptr;
    sc = rpclv1prd_shift.getHist(rpclv1triggerhighpt_etastat,"Trigger_Hits_HighPt_eta_Stat"); 
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register rpclv1triggerhighpt_eta_stat hist to MonGroup" );  
    rpclv1triggerhighpt_etastat->Fill(float(n_triggerhighpt_eta)); 
 
    TH1* rpclv1triggerhighpt_phistat = nullptr;
    sc = rpclv1prd_shift.getHist(rpclv1triggerhighpt_phistat,"Trigger_Hits_HighPt_phi_Stat"); 
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register rpclv1triggerhighpt_phi_stat hist to MonGroup" );  
    rpclv1triggerhighpt_phistat->Fill(float(n_triggerhighpt_phi)); 
  
    TH2* rpclv1triggerhighpt_etaphistat = nullptr;
    sc = rpclv1prd_shift.getHist(rpclv1triggerhighpt_etaphistat,"Trigger_Hits_HighPt_etaphi_Stat");
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register Trigger_Hits_HighPt_etaphi_Stat hist to MonGroup" ); 
    rpclv1triggerhighpt_etaphistat->Fill( float(n_triggerhighpt_phi), float(n_triggerhighpt_eta) );
    
    TH2* rpclv1ROI_LowPt = nullptr;
    sc = rpclv1prd_shift.getHist(rpclv1ROI_LowPt,"ROI_LowPt_distribution");
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register ROI_LowPt_distribution hist to MonGroup" );

    TH2* rpclv1ROI_HighPt = nullptr;
    sc = rpclv1prd_shift.getHist(rpclv1ROI_HighPt,"ROI_HighPt_distribution");
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register ROI_HighPt_distribution hist to MonGroup" );
  
    TH1* rpclv1sectorlogicstat = nullptr;
    sc = rpclv1prd_shift.getHist(rpclv1sectorlogicstat,"SLChannel_per_SectorLogic");	
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register rpclv1sectorlogicstat hist to MonGroup" );
	    
    TH1* rpclv1towervslogicstat = nullptr;
    sc = rpclv1prd_shift.getHist(rpclv1towervslogicstat,"PadChannel_per_SectorLogic");	
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register rpclv1towervslogicstat hist to MonGroup" );

    TH2* rpclv1cmalogicstat = nullptr;
    sc = rpclv1prd_shift.getHist(rpclv1cmalogicstat,"CMChannel_per_SectorLogic");
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register rpclv1cmalogicstat hist to MonGroup" );
 
    TH1* rpclv1_BCid_per_TriggerCorr = nullptr;
    sc = rpclv1prd_shift.getHist(rpclv1_BCid_per_TriggerCorr,"rpclv1_BCid_per_TriggerCorrelation");
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register rpclv1_BCid_per_TriggerCorr hist to MonGroup" );   
  
    TH2* rpclv1_BCid_vs_SL = nullptr;
    sc = rpclv1_shift_dqmf.getHist( rpclv1_BCid_vs_SL, "rpclv1_BCid_vs_SectorLogic") ;
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register rpclv1_BCid_vs_SectorLogic hist to MonGroup" ); 
    
    TH2* rpclv1_LPt_BCid_vs_SL = nullptr;
    sc = rpclv1_shift_dqmf.getHist( rpclv1_LPt_BCid_vs_SL, "rpclv1_LPt_BCid_vs_SectorLogic") ;
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register rpclv1_LPt_BCid_vs_SectorLogic hist to MonGroup" ); 
    
    TH2* rpclv1_HPt_BCid_vs_SL = nullptr;
    sc = rpclv1_shift_dqmf.getHist( rpclv1_HPt_BCid_vs_SL, "rpclv1_HPt_BCid_vs_SectorLogic") ;
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register rpclv1_HPt_BCid_vs_SectorLogic hist to MonGroup" ); 
    
    TH1* rpclv1_BCid_per_TriggerType = nullptr;
    sc = rpclv1_shift_dqmf.getHist(rpclv1_BCid_per_TriggerType,"rpclv1_BCid_per_TriggerType");
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register rpclv1_BCid_per_TriggerType hist to MonGroup" );   
       
    TH2* rpclv1_TriggerCond_vs_SL = nullptr;
    sc  = rpclv1_shift_dqmf.getHist(rpclv1_TriggerCond_vs_SL, "TriggerCondition_vs_SectorLogic" ) ;
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register TriggerCondition_vs_SectorLogic hist to MonGroup" );
    
    // logical or
    TH2* rpclv1_logicalOR_LowPt0 = nullptr;
    sc = rpclv1prd_shift.getHist(rpclv1_logicalOR_LowPt0,  "PhiLogicalOR_LowPt0"  );
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get PhiLogicalOR_LowPt0 hist "  );
    
    TH2* rpclv1_logicalOR_LowPt1 = nullptr;
    sc = rpclv1prd_shift.getHist(rpclv1_logicalOR_LowPt1,  "PhiLogicalOR_LowPt1"  );
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get PhiLogicalOR_LowPt1 hist "  );
    
    TH2* rpclv1_logicalOR_HighPt0 = nullptr;
    sc = rpclv1prd_shift.getHist(rpclv1_logicalOR_HighPt0, "PhiLogicalOR_HighPt0" );
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get PhiLogicalOR_HighPt0 hist " );
    
    TH2* rpclv1_logicalOR_HighPt1 = nullptr;
    sc = rpclv1prd_shift.getHist(rpclv1_logicalOR_HighPt1, "PhiLogicalOR_HighPt1" );
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get PhiLogicalOR_HighPt1 hist " );
    
    TH2* TriggerCondition_vs_CM = nullptr;
    sc = rpclv1prd_shift.getHist( TriggerCondition_vs_CM, "Trigger_Condition_vs_CM" );
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get TriggerCondition_vs_CM hist " );


    for (RpcPadContainer::const_iterator rdoColli = rpcRDO->begin(); rdoColli!=rpcRDO->end(); ++rdoColli)
      {
	const RpcPad* rdoColl = *rdoColli;
	// Now loop on the RDO to find the ones with trigger hits ijk=6
	if ( (rdoColl)->size() != 0 ) {		    
        	    
	  RpcPad::const_iterator itCM = (rdoColl)->begin();
	  for ( ; itCM != (rdoColl)->end() ; ++itCM ) {
              
	    RpcCoinMatrix::const_iterator itChan = (*itCM)->begin();	   
           
	    for ( ; itChan != (*itCM)->end() ; ++itChan ) {	   

	      int i_sector	 = (rdoColl)->sector()    ;
	      int i_padId	 = (rdoColl)->onlineId()  ;
	      int i_cmaId	 = (*itCM)->onlineId()    ;
	      int i_bcId	 = (*itChan)->bcid()	  ;
	      int i_time	 = (*itChan)->time()	  ;
	      int i_ijk	 = (*itChan)->ijk()	  ;
	      int i_channel	 = (*itChan)->channel()   ;
	      {
		if ( i_ijk==6 ) {
                  
		 
		  //look for thresholds ijk=7 in the next rdo's ijk=6,6...,7 with the same time  
		  int i_thr = 99 ;
		  RpcCoinMatrix::const_iterator itChanThr ;	   
		  int thrskip=0;
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
		  
		  rpclv1_BCid_per_TriggerCorr->Fill(m_trigtype, i_bcId +  8 * int(i_cmaId/4) );
        	
		  for (int k=0; k!= 8+1 ; k++ ) {
		    int trigBinType = m_trigtype & int(pow(2,float(k))) ;
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
		  int NtriggerLowPtPhi  = 0;
		  int NtriggerLowPtEta  = 0;
		  int NtriggerHighPtPhi = 0;
		  int NtriggerHighPtEta = 0;

                  IJKtriggerLowPtPhi  = 0;
                  IJKtriggerLowPtEta  = 0;
                  IJKtriggerHighPtPhi = 0;
                  IJKtriggerHighPtEta = 0;
                  
		  RpcCoinMatrix::const_iterator itChan3 = (*itCM)->begin();	   
		  for ( ; itChan3 != (*itCM)->end() ; ++itChan3 ) {
		    int ijk_trigger =  (*itChan3)->ijk() ;
		    if ( ijk_trigger>5  ) continue; 
               
		    int Layertrigger = ijk_trigger; 
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
		    
        	  
                    char sectorlogic_name_char[1000]      ;
		    sprintf(sectorlogic_name_char,"SectorLogic%d",i_sector);
                    std::string sectorlogic_name = sectorlogic_name_char ;
                    char tower_name_char[1000]		;
		    sprintf(tower_name_char      ,"Tower%d"      ,i_padId );
                    std::string tower_name	     = tower_name_char	     ;	
                    char cma_name_char[1000]		;
		    sprintf(cma_name_char        ,"Cma%d"        ,i_cmaId );
                    std::string cma_name	     = cma_name_char	     ;	
                    char thr_name_char[1000]		;
		    sprintf(thr_name_char        ,"Thr%d"        ,i_thr   );
                    std::string thr_name	     = thr_name_char	     ;
          
		    bool histo_flag=true;
		    for (std::vector<std::string>::const_iterator iter=m_sectorlogicTowerCma_name_list2.begin(); iter!=m_sectorlogicTowerCma_name_list2.end(); iter++){
		      if ( (sectorlogic_name+tower_name+cma_name+thr_name)==*iter){histo_flag=false;}
		    }
		    if (histo_flag){ 
		      m_sectorlogicTowerCma_name_list2.push_back(sectorlogic_name+tower_name+cma_name+thr_name); 
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
                  TH2* rpclv1_TriggerLy_vs_SL = nullptr;
		  sc  = rpclv1prd_shift.getHist(rpclv1_TriggerLy_vs_SL, "TriggerLayer_vs_SectorLogic" ) ;
		  if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't get TriggerCondition_vs_SectorLogic hist to MonGroup" );
              
		  if ( NtriggerLowPtPhi >0 ) {
                    TH1* rpclv1Trigger_cond_LowPt_phi = nullptr;
		    sc  = rpclv1prd_shift.getHist(rpclv1Trigger_cond_LowPt_phi,"Trigger_Condition_LowPt_Phi");
		    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register Trigger_Layer_LowPt_Phi hist to MonGroup" );
		    rpclv1Trigger_cond_LowPt_phi -> Fill ( float(IJKtriggerLowPtPhi) )  ;
		    rpclv1_TriggerLy_vs_SL       -> Fill (i_sector, IJKtriggerLowPtPhi ); // general 
		    TriggerCondition_vs_CM       -> Fill (i_sector*56 + i_padId*8 + i_cmaId, IJKtriggerLowPtPhi );
		  }
		  if ( NtriggerLowPtEta >0 ) {
                    TH1* rpclv1Trigger_cond_LowPt_eta = nullptr;
		    sc  = rpclv1prd_shift.getHist(rpclv1Trigger_cond_LowPt_eta,"Trigger_Condition_LowPt_Eta");
		    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't registerTrigger_Condition_LowPt_Eta hist to MonGroup" );
		    rpclv1Trigger_cond_LowPt_eta -> Fill ( float(IJKtriggerLowPtEta) )   ;
		    rpclv1_TriggerLy_vs_SL       -> Fill ( i_sector, IJKtriggerLowPtEta ); // general
		    TriggerCondition_vs_CM       -> Fill (i_sector*56 + i_padId*8 + i_cmaId, IJKtriggerLowPtEta );
		  }
		  if ( NtriggerHighPtPhi >0 ) {
                    TH1* rpclv1Trigger_cond_HighPt_phi = nullptr;
		    sc  = rpclv1prd_shift.getHist(rpclv1Trigger_cond_HighPt_phi,"Trigger_Condition_HighPt_Phi");
		    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't registerTrigger_Condition_LowPt_Eta hist to MonGroup" );
		    rpclv1Trigger_cond_HighPt_phi-> Fill ( float(IJKtriggerHighPtPhi))	    ;
		    rpclv1_TriggerLy_vs_SL       -> Fill ( i_sector, IJKtriggerHighPtPhi +16 ); // general
		    TriggerCondition_vs_CM       -> Fill (i_sector*56 + i_padId*8 + i_cmaId, IJKtriggerHighPtPhi );
		  }
		  if ( NtriggerHighPtEta >0 ) {
                    TH1* rpclv1Trigger_cond_HighPt_eta = nullptr;
		    sc  = rpclv1prd_shift.getHist(rpclv1Trigger_cond_HighPt_eta,"Trigger_Condition_HighPt_Eta");
		    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't registerTrigger_Condition_LowPt_Eta hist to MonGroup" );
		    rpclv1Trigger_cond_HighPt_eta-> Fill ( float(IJKtriggerHighPtEta))	    ;
		    rpclv1_TriggerLy_vs_SL       -> Fill ( i_sector, IJKtriggerHighPtEta +16 ); // general
		    TriggerCondition_vs_CM       -> Fill (i_sector*56 + i_padId*8 + i_cmaId, IJKtriggerHighPtEta );
		  }
        	
		  int n_trigLow_ly = 0;
                  int n_trigHigh_ly = 0;
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
        				      
                char sectorlogic_name_char[1000]      ;
		sprintf(sectorlogic_name_char,"SectorLogic%d",i_sector);
                std::string sectorlogic_name = sectorlogic_name_char ;
                char tower_name_char[1000]		;
		sprintf(tower_name_char	   ,"Tower%d"	   ,i_padId );
                std::string tower_name	   = tower_name_char	   ;	
                char cma_name_char[1000]		;
		sprintf(cma_name_char	   ,"Cma%d"	   ,i_cmaId );
                std::string cma_name  	   = cma_name_char	   ;
                char ijk_name_char[1000]              ;
		sprintf(ijk_name_char	   ,"ijk%d"	   ,i_ijk   );
                std::string ijk_name         = ijk_name_char         ;      
        	    
		std::string generic_path_RPCLV1cmatimevschcxx	     = generic_path_rpclv1monitoring+"/"+sectorlogic_name+"/";
		generic_path_RPCLV1cmatimevschcxx		    += tower_name				     ;
		generic_path_RPCLV1cmatimevschcxx		    += "/rpclv1cosmic_cmatimevsch"		     ;
		generic_path_RPCLV1cmatimevschcxx		    += cma_name 				     ;
        	     
		std::string generic_path_RPCLV1cmatimetriggervschcxx = generic_path_rpclv1monitoring+"/"+sectorlogic_name+"/";
		generic_path_RPCLV1cmatimetriggervschcxx	       += tower_name				     ;
		generic_path_RPCLV1cmatimetriggervschcxx	       += "/rpclv1cosmic_cmatimetriggervsch"	     ;
		generic_path_RPCLV1cmatimetriggervschcxx	       += cma_name				     ;
          
		// Fill time histograms begin
		if (m_rpclv1hist ) {	
		  bool histo_flag=true;
		  for (std::vector<std::string>::const_iterator iter=m_sectorlogicTowerCma_name_list.begin(); iter!=m_sectorlogicTowerCma_name_list.end(); iter++){
		    if ( (sectorlogic_name+tower_name+cma_name)==*iter){histo_flag=false;}
		  }
		  if (histo_flag){
		    m_sectorlogicTowerCma_name_list.push_back(sectorlogic_name+tower_name+cma_name);
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
		    int shift_ijk = 0;
		    int shift_pad = 1;

                    int shift_cm = 0;
                    std::string cma_name_p;
                    char cma_name_p_char[1000]		;
		    if ( i_cmaId==1 || i_cmaId==3 || i_cmaId==5 || i_cmaId==7 ) {
		      sprintf(cma_name_p_char , "Cma%d_%d" , i_cmaId-1, i_cmaId);  cma_name_p = cma_name_p_char	 ;
		      shift_cm = 32 ;
		    }
		    else {
		      sprintf(cma_name_p_char  ,"Cma%d_%d"  ,i_cmaId, i_cmaId+1);  cma_name_p = cma_name_p_char	 ;
		      shift_cm = 0; }

                    std::string ijk_name_p;
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

		    bool histo_flag=true;
		    for (std::vector<std::string>::const_iterator iter=m_profile_list.begin(); iter!=m_profile_list.end(); iter++){
		      if ( (sectorlogic_name+cma_name_p+ijk_name_p)==*iter){histo_flag=false;}
		    }
		    if (histo_flag) {
		      m_profile_list.push_back(sectorlogic_name + cma_name_p + ijk_name_p);
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
 
 	      std::vector<std::string>   rpclayersectorsidename = RpcGM::RpcLayerSectorSideName(m_rpcIdHelper,prdcoll_id, 0)  ;
 	      std::string		 sector_dphi_layer	= rpclayersectorsidename[12]		 ;
	
 	      std::vector<int>  	 RpcStrip = RpcGM::RpcStripShift(m_muonMgr,m_rpcIdHelper,prdcoll_id, 0);
 	      int strip_dbindex        = (RpcGM::RpcStripShift(m_muonMgr,m_rpcIdHelper,prdcoll_id, 0)).at(16);// cool strip profile
	        	  if ( m_doCoolDB ) {
                            TH1* rpcCool_StripProfile = nullptr;
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
    std::string generic_path_rpclv1monitoring = "Muon/MuonRawDataMonitoring/RPCLV1";
  
    MonGroup rpclv1prd_shift( this, generic_path_rpclv1monitoring +"/Overview", run, ATTRIB_UNMANAGED );
    MonGroup rpclv1_shift_dqmf( this, generic_path_rpclv1monitoring + "/GLOBAL", run, ATTRIB_UNMANAGED )  ;
    MonGroup rpcCoolDb( this, generic_path_rpclv1monitoring+"/CoolDB", run, ATTRIB_UNMANAGED )         ;
     
    if(newRunFlag())
      {         

	ATH_MSG_DEBUG ( "RPCLV1 RawData Monitoring : isNewRun" );
       
        
	// Trigger Layers vs SL
	std::string rpclv1_TriggerLy_vs_SL_title      = "TriggerLayer_vs_SectorLogic"       ;
	const char* rpclv1_TriggerLy_vs_SL_title_char = rpclv1_TriggerLy_vs_SL_title.c_str();
	
	TH2* rpclv1_TriggerLy_vs_SL = new TH2I(rpclv1_TriggerLy_vs_SL_title_char, rpclv1_TriggerLy_vs_SL_title_char, 64, 0, 64, 32, 0, 32);
	sc = rpclv1prd_shift.regHist( rpclv1_TriggerLy_vs_SL ) ;
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1_TriggerLy_vs_SL Failed to register histogram " );       
	    return sc;
	  }
	rpclv1_TriggerLy_vs_SL->GetXaxis()->SetTitle("Sector Logic")       ;
	rpclv1_TriggerLy_vs_SL->GetYaxis()->SetTitle("")                   ; 
	rpclv1_TriggerLy_vs_SL->SetOption("COLZ");
	
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1_TriggerLy_vs_SL << rpclv1_TriggerLy_vs_SL_title.c_str() );
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
	std::string rpclv1_TriggerCond_vs_CM_title      = "Trigger_Condition_vs_CM"                ;
	const char* rpclv1_TriggerCond_vs_CM_title_char = rpclv1_TriggerCond_vs_CM_title.c_str();
	TH2* rpclv1_TriggerCond_vs_CM = new TH2I(rpclv1_TriggerCond_vs_CM_title_char,	rpclv1_TriggerCond_vs_CM_title_char,
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
	  std::string rpclv1_logicalOR_title      = "PhiLogicalOR_" + *it           ;
	  const char* rpclv1_logicalOR_title_char = rpclv1_logicalOR_title.c_str();
          TH2* rpclv1_logicalOR = new TH2I( rpclv1_logicalOR_title_char, rpclv1_logicalOR_title_char, 
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
	std::string rpclv1_TriggerCond_vs_SL_title      = "TriggerCondition_vs_SectorLogic"       ;
	const char* rpclv1_TriggerCond_vs_SL_title_char = rpclv1_TriggerCond_vs_SL_title.c_str();
	
	TH2* rpclv1_TriggerCond_vs_SL = new TH2I(rpclv1_TriggerCond_vs_SL_title_char, rpclv1_TriggerCond_vs_SL_title_char, 64, 0, 64, 7, 1, 8);
	sc = rpclv1_shift_dqmf.regHist( rpclv1_TriggerCond_vs_SL ) ;
	if(sc.isFailure()) {
	  ATH_MSG_FATAL ( "rpclv1_TriggerCond_vs_SL Failed to register histogram " );       
	  return sc;
	}
	rpclv1_TriggerCond_vs_SL->GetXaxis()->SetTitle("Sector Logic");
	rpclv1_TriggerCond_vs_SL->GetYaxis()->SetTitle("")            ; 
	rpclv1_TriggerCond_vs_SL->GetYaxis()->SetTitleSize(0.03)      ;
	rpclv1_TriggerCond_vs_SL->SetOption("COLZ")                   ;
	
	
	
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1_TriggerCond_vs_SL << rpclv1_TriggerCond_vs_SL_title.c_str() );
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
	std::string rpclv1_BCid_vs_SL_title      = "rpclv1_BCid_vs_SectorLogic"     ;
	const char* rpclv1_BCid_vs_SL_title_char = rpclv1_BCid_vs_SL_title.c_str();
	
	TH2* rpclv1_BCid_vs_SL = new TH2I ( rpclv1_BCid_vs_SL_title_char, rpclv1_BCid_vs_SL_title_char, 64, 0, 64, 8*2, 0, 16) ;
	sc = rpclv1_shift_dqmf.regHist( rpclv1_BCid_vs_SL );
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1_BCid_vs_SectorLogic Failed to register histogram " );       
	    return sc;
	  }
	rpclv1_BCid_vs_SL->GetXaxis()->SetTitle("Triggered RPC Sector Logic") ;  
	rpclv1_BCid_vs_SL->GetYaxis()->SetTitle("BCId+HPtTrigger*8 for trigger hits") ;
	rpclv1_BCid_vs_SL->SetOption("COLZ");

	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1_BCid_vs_SL << rpclv1_BCid_vs_SL_title.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked rpclv1_BCid_vs_SectorLogic successfully" ); 
		
	// LPt BCid (ijk==6) vs SL
	std::string rpclv1_LPt_BCid_vs_SL_title      = "rpclv1_LPt_BCid_vs_SectorLogic"     ;
	const char* rpclv1_LPt_BCid_vs_SL_title_char = rpclv1_LPt_BCid_vs_SL_title.c_str();
	
	TH2* rpclv1_LPt_BCid_vs_SL = new TH2I ( rpclv1_LPt_BCid_vs_SL_title_char, rpclv1_LPt_BCid_vs_SL_title_char, 64, 0, 64, 8, 0, 8) ;
	sc = rpclv1_shift_dqmf.regHist( rpclv1_LPt_BCid_vs_SL );
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1_BCid_vs_SectorLogic Failed to register histogram " );       
	    return sc;
	  }
	rpclv1_LPt_BCid_vs_SL->GetXaxis()->SetTitle("Triggered LPt RPC Sector Logic") ;  
	rpclv1_LPt_BCid_vs_SL->GetYaxis()->SetTitle("Trigger hits BCid") ;
	rpclv1_LPt_BCid_vs_SL->SetOption("COLZ");

	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1_LPt_BCid_vs_SL << rpclv1_LPt_BCid_vs_SL_title.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked rpclv1_BCid_vs_SectorLogic successfully" ); 
		
	// BCid (ijk==6) vs SL
	std::string rpclv1_HPt_BCid_vs_SL_title      = "rpclv1_HPt_BCid_vs_SectorLogic"     ;
	const char* rpclv1_HPt_BCid_vs_SL_title_char = rpclv1_HPt_BCid_vs_SL_title.c_str();
	
	TH2* rpclv1_HPt_BCid_vs_SL = new TH2I ( rpclv1_HPt_BCid_vs_SL_title_char, rpclv1_HPt_BCid_vs_SL_title_char, 64, 0, 64, 8, 0, 8) ;
	sc = rpclv1_shift_dqmf.regHist( rpclv1_HPt_BCid_vs_SL );
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1_BCid_vs_SectorLogic Failed to register histogram " );       
	    return sc;
	  }
	rpclv1_HPt_BCid_vs_SL->GetXaxis()->SetTitle("Triggered HPt RPC Sector Logic") ;  
	rpclv1_HPt_BCid_vs_SL->GetYaxis()->SetTitle("Trigger hits BCid") ;
	rpclv1_HPt_BCid_vs_SL->SetOption("COLZ");

	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1_HPt_BCid_vs_SL << rpclv1_HPt_BCid_vs_SL_title.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked rpclv1_BCid_vs_SectorLogic successfully" ); 
	

	////////////////////////////////////////////////////
     
      
	//hits per event
	std::string rpclv1_hitperEvent_title = "rpclv1_hitperEvent";      
	const char* rpclv1_hitperEvent_title_char = rpclv1_hitperEvent_title.c_str();
       
	TH1 *rpclv1_hitperEvent=new TH1I(rpclv1_hitperEvent_title_char,rpclv1_hitperEvent_title_char, NMAXHIT,0.,NMAXHIT); 				
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
	std::string generic_path_rpclv1triggerlowpt_etastat = generic_path_rpclv1monitoring+"/Overview";
	std::string rpclv1triggerlowpt_etastat_title = "Trigger_Hits_LowPt_eta_Stat";      
	const char* rpclv1triggerlowpt_etastat_title_char = rpclv1triggerlowpt_etastat_title.c_str();
       
	TH1 *rpclv1triggerlowpt_etastat=new TH1I(rpclv1triggerlowpt_etastat_title_char,rpclv1triggerlowpt_etastat_title_char, 15, 0, 15); 				
	sc=rpclv1prd_shift.regHist(rpclv1triggerlowpt_etastat) ;  
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1triggerlowpt_etastat Failed to register histogram " );       
	    return sc;
	  }
	rpclv1triggerlowpt_etastat->SetFillColor(42);
	rpclv1triggerlowpt_etastat->GetXaxis()->SetTitle("LowPt eta trigger hits / event");
	rpclv1triggerlowpt_etastat->GetYaxis()->SetTitle("Counts");
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1triggerlowpt_etastat << generic_path_rpclv1triggerlowpt_etastat.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked rpclv1triggerlowpt_etastat successfully" ); 	

	////////////////////////////////////////////// 
 
	//lowpt PHI trigger hits stat
	std::string generic_path_rpclv1triggerlowpt_phistat = generic_path_rpclv1monitoring+"/Overview";
	std::string rpclv1triggerlowpt_phistat_title = "Trigger_Hits_LowPt_phi_Stat";      
	const char* rpclv1triggerlowpt_phistat_title_char = rpclv1triggerlowpt_phistat_title.c_str();
       
	TH1 *rpclv1triggerlowpt_phistat=new TH1I(rpclv1triggerlowpt_phistat_title_char,rpclv1triggerlowpt_phistat_title_char, 15, 0, 15); 				
	sc=rpclv1prd_shift.regHist(rpclv1triggerlowpt_phistat) ;  
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1triggerlowpt_phistat Failed to register histogram " );       
	    return sc;
	  }
	rpclv1triggerlowpt_phistat->SetFillColor(42);
	rpclv1triggerlowpt_phistat->GetXaxis()->SetTitle("LowPt phi trigger hits / event");
	rpclv1triggerlowpt_phistat->GetYaxis()->SetTitle("Counts");
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1triggerlowpt_phistat << generic_path_rpclv1triggerlowpt_phistat.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked rpclv1triggerlowpt_phistat successfully" ); 	
	
	//////////////////////////////////////////////
      
	//lowpt ETA-PHI trigger hits stat
	std::string generic_path_rpclv1triggerlowpt_etaphistat = generic_path_rpclv1monitoring+"/Overview";
	std::string rpclv1triggerlowpt_etaphistat_title = "Trigger_Hits_Lowpt_etaphi_Stat";      
	const char* rpclv1triggerlowpt_etaphistat_title_char = rpclv1triggerlowpt_etaphistat_title.c_str();
       
	TH2 *rpclv1triggerlowpt_etaphistat=new TH2I(rpclv1triggerlowpt_etaphistat_title_char,rpclv1triggerlowpt_etaphistat_title_char, 15, 0, 15,15,0,15); 				
	sc=rpclv1prd_shift.regHist(rpclv1triggerlowpt_etaphistat) ;  
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1triggerlowpt_etaphistat Failed to register histogram " );       
	    return sc;
	  }
	rpclv1triggerlowpt_etaphistat->SetOption("COLZ");
	rpclv1triggerlowpt_etaphistat->GetXaxis()->SetTitle("LowPt phi trigger hits / event");
	rpclv1triggerlowpt_etaphistat->GetYaxis()->SetTitle("LowPt eta trigger hits / event");
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1triggerlowpt_etaphistat << generic_path_rpclv1triggerlowpt_etaphistat.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked rpclv1triggerlowpt_etaphistat successfully" ); 	

      
	////////////////////////////////////////////// 
           
	//highpt ETA trigger hits stat
	std::string generic_path_rpclv1triggerhighpt_etastat = generic_path_rpclv1monitoring+"/Overview";
	std::string rpclv1triggerhighpt_etastat_title = "Trigger_Hits_HighPt_eta_Stat";
	const char* rpclv1triggerhighpt_etastat_title_char = rpclv1triggerhighpt_etastat_title.c_str();
	TH1 *rpclv1triggerhighpt_etastat=new TH1I(rpclv1triggerhighpt_etastat_title_char,rpclv1triggerhighpt_etastat_title_char, 15, 0, 15);
	sc=rpclv1prd_shift.regHist(rpclv1triggerhighpt_etastat) ;
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1triggerhighpt_etastat Failed to register histogram " );
	    return sc;
	  }
	rpclv1triggerhighpt_etastat->SetFillColor(42);
	rpclv1triggerhighpt_etastat->GetXaxis()->SetTitle("HighPt eta trigger hits / event");
	rpclv1triggerhighpt_etastat->GetYaxis()->SetTitle("Counts");
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1triggerhighpt_etastat << generic_path_rpclv1triggerhighpt_etastat.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run );
	ATH_MSG_DEBUG ( "Booked rpclv1triggertimehighpt_etastat successfully" );
 
	//////////////////////////////////////////////

	//highpt PHI trigger hits stat
	std::string generic_path_rpclv1triggerhighpt_phistat = generic_path_rpclv1monitoring+"/Overview";
	std::string rpclv1triggerhighpt_phistat_title = "Trigger_Hits_HighPt_phi_Stat";
	const char* rpclv1triggerhighpt_phistat_title_char = rpclv1triggerhighpt_phistat_title.c_str();
	TH1 *rpclv1triggerhighpt_phistat=new TH1I(rpclv1triggerhighpt_phistat_title_char,rpclv1triggerhighpt_phistat_title_char, 15, 0, 15);
	sc=rpclv1prd_shift.regHist(rpclv1triggerhighpt_phistat) ;
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1triggerhighpt_phistat Failed to register histogram " );
	    return sc;
	  }
	rpclv1triggerhighpt_phistat->SetFillColor(42);
	rpclv1triggerhighpt_phistat->GetXaxis()->SetTitle("HighPt phi trigger hits / event");
	rpclv1triggerhighpt_phistat->GetYaxis()->SetTitle("Counts");
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1triggerhighpt_phistat << generic_path_rpclv1triggerhighpt_phistat.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run );
	ATH_MSG_DEBUG ( "Booked rpclv1triggertimehighpt_phistat successfully" ); 
 
	//////////////////////////////////////////////
      
	//highpt ETA - PHI trigger hits stat
	std::string generic_path_rpclv1triggerhighpt_etaphistat = generic_path_rpclv1monitoring+"/Overview";
	std::string rpclv1triggerhighpt_etaphistat_title = "Trigger_Hits_HighPt_etaphi_Stat";    
	const char* rpclv1triggerhighpt_etaphistat_title_char = rpclv1triggerhighpt_etaphistat_title.c_str();     
	TH2 *rpclv1triggerhighpt_etaphistat=new TH2I(rpclv1triggerhighpt_etaphistat_title_char,rpclv1triggerhighpt_etaphistat_title_char, 15, 0, 15,15, 0, 15);
	sc=rpclv1prd_shift.regHist(rpclv1triggerhighpt_etaphistat) ;  
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1triggerhighpt_etaphistat Failed to register histogram " );       
	    return sc;
	  }      		
	rpclv1triggerhighpt_etaphistat->SetOption("COLZ");
	rpclv1triggerhighpt_etaphistat->GetXaxis()->SetTitle("HighPt phi trigger hits / event");
	rpclv1triggerhighpt_etaphistat->GetYaxis()->SetTitle("HighPt eta trigger hits / event");
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1triggerhighpt_etaphistat << generic_path_rpclv1triggerhighpt_etaphistat.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked rpclv1triggertimehighpt_etaphistat successfully" ); 	
  
	////////////////////////////////////////////// 
  
	//sector logic statistics
	std::string generic_path_rpclv1sectorlogicstat = generic_path_rpclv1monitoring+"/Overview";
	std::string rpclv1sectorlogicstat_title = "SLChannel_per_SectorLogic";    
	const char* rpclv1sectorlogicstat_title_char = rpclv1sectorlogicstat_title.c_str ();     
	TH1 *rpclv1sectorlogicstat=new TH1I(rpclv1sectorlogicstat_title_char,rpclv1sectorlogicstat_title_char, 64, 0, 64); 
	sc=rpclv1prd_shift.regHist(rpclv1sectorlogicstat) ;  
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1sectorlogicstat Failed to register histogram " );       
	    return sc;
	  }  		
	rpclv1sectorlogicstat->SetFillColor(42);
	rpclv1sectorlogicstat->GetXaxis()->SetTitle("SectorLogic");
	rpclv1sectorlogicstat->GetYaxis()->SetTitle("Counts/Sector Logic "); 
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1sectorlogicstat << generic_path_rpclv1sectorlogicstat.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked rpclv1sectorlogicstat successfully" ); 	
    
	////////////////////////////////////////////// 

	//tower per sector logic statistics
	std::string generic_path_rpclv1towervslogicstat = generic_path_rpclv1monitoring+"/Overview";
	std::string rpclv1towervslogicstat_title = "PadChannel_per_SectorLogic";    
	const char* rpclv1towervslogicstat_title_char = rpclv1towervslogicstat_title.c_str ();     
	TH2 *rpclv1towervslogicstat=new TH2I(rpclv1towervslogicstat_title_char,rpclv1towervslogicstat_title_char, 64, 0, 64, 8, 0, 8); 
	sc=rpclv1prd_shift.regHist(rpclv1towervslogicstat) ;  
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1towervslogicstat Failed to register histogram " );       
	    return sc;
	  }  		  
	rpclv1towervslogicstat->SetFillColor(42);
	rpclv1towervslogicstat->SetOption("COLZ");
	rpclv1towervslogicstat->GetXaxis()->SetTitle("SectorLogic");
	rpclv1towervslogicstat->GetYaxis()->SetTitle("Pad");
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1towervslogicstat << generic_path_rpclv1towervslogicstat.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked rpclv1towervslogicstat successfully" ); 	
 
	////////////////////////////////////////////// 
    
	// cma channel per sector logic stat
	std::string generic_path_rpclv1cmalogicstat = generic_path_rpclv1monitoring+"/Overview";
	std::string rpclv1cmalogicstat_title = "CMChannel_per_SectorLogic";    
	const char* rpclv1cmalogicstat_title_char = rpclv1cmalogicstat_title.c_str ();     
	TH2 *rpclv1cmalogicstat=new TH2I(rpclv1cmalogicstat_title_char,rpclv1cmalogicstat_title_char, 64, 0, 64, 8*8, 0, 8*8); 
	sc=rpclv1prd_shift.regHist(rpclv1cmalogicstat) ;  
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1cmalogicstat Failed to register histogram " );       
	    return sc;
	  }  		  
	rpclv1cmalogicstat->SetFillColor(42);
	rpclv1cmalogicstat->SetOption("COLZ");
	rpclv1cmalogicstat->GetXaxis()->SetTitle("SectorLogic");
	rpclv1cmalogicstat->GetYaxis()->SetTitle("Pad*8+Cma");
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1cmalogicstat << generic_path_rpclv1cmalogicstat.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked rpclv1cmalogicstat successfully" ); 	
 
	////////////////////////////////////////////// 
      
	//ROI distribution Low Pt
	std::string generic_path_rpclv1ROI_LowPt = generic_path_rpclv1monitoring+"/Overview";
	std::string rpclv1ROI_LowPt_title = "ROI_LowPt_distribution";
	const char* rpclv1ROI_LowPt_title_char = rpclv1ROI_LowPt_title.c_str();
	TH2 * rpclv1ROI_LowPt = new TH2I(rpclv1ROI_LowPt_title_char, rpclv1ROI_LowPt_title_char, 64, 0, 64, 28, 0, 28); 
	sc=rpclv1prd_shift.regHist(rpclv1ROI_LowPt);
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1ROI_LowPt Failed to register histogram " );       
	    return sc;
	  } 
	rpclv1ROI_LowPt->SetOption("COLZ");
	rpclv1ROI_LowPt->GetXaxis()->SetTitle("SectorLogic");
	rpclv1ROI_LowPt->GetYaxis()->SetTitle("Region of Interest");
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1ROI_LowPt << rpclv1ROI_LowPt_title.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked rpclv1ROI_LowPt successfully" ); 	
      
	/////////////////////////////////////////////
      
	// ROI distribution High Pt
	std::string generic_path_rpclv1ROI_HighPt = generic_path_rpclv1monitoring+"/Overview";
	std::string rpclv1ROI_HighPt_title = "ROI_HighPt_distribution";
	const char* rpclv1ROI_HighPt_title_char = rpclv1ROI_HighPt_title.c_str();
	TH2 * rpclv1ROI_HighPt = new TH2I(rpclv1ROI_HighPt_title_char, rpclv1ROI_HighPt_title_char, 64, 0, 64, 28, 0, 28); 
	sc=rpclv1prd_shift.regHist(rpclv1ROI_HighPt);
	if(sc.isFailure())
	  { ATH_MSG_FATAL ( "rpclv1ROI_HighPt Failed to register histogram " );       
	    return sc;
	  } 
	rpclv1ROI_HighPt->SetOption("COLZ");
	rpclv1ROI_HighPt->GetXaxis()->SetTitle("SectorLogic");
	rpclv1ROI_HighPt->GetYaxis()->SetTitle("Region of Interest");
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1ROI_HighPt << rpclv1ROI_HighPt_title.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked rpclv1ROI_HighPt successfully" ); 	
	
	////////////////////////////////////
      
	// Trigger_Cond LowPt PHI
	std::string generic_path_rpclv1Trigger_cond_LowPt_phi = generic_path_rpclv1monitoring+"/Overview";
	std::string rpclv1Trigger_cond_LowPt_phi_title = "Trigger_Condition_LowPt_Phi";
	const char* rpclv1Trigger_cond_LowPt_phi_title_char = rpclv1Trigger_cond_LowPt_phi_title.c_str();
	TH1* rpclv1Trigger_cond_LowPt_phi = new TH1I( rpclv1Trigger_cond_LowPt_phi_title_char, rpclv1Trigger_cond_LowPt_phi_title_char, 16, 0, 16 );
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
      
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1Trigger_cond_LowPt_phi << rpclv1Trigger_cond_LowPt_phi_title.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked Trigger_Condition_LowPt_Phi successfully" ); 	
	      
	//////////////////////////////////////////////
      
	// Trigger_Cond LowPt ETA
	std::string generic_path_rpclv1Trigger_cond_LowPt_eta = generic_path_rpclv1monitoring+"/Overview";
	std::string rpclv1Trigger_cond_LowPt_eta_title = "Trigger_Condition_LowPt_Eta";
	const char* rpclv1Trigger_cond_LowPt_eta_title_char = rpclv1Trigger_cond_LowPt_eta_title.c_str();
	TH1* rpclv1Trigger_cond_LowPt_eta = new TH1I( rpclv1Trigger_cond_LowPt_eta_title_char, rpclv1Trigger_cond_LowPt_eta_title_char, 16, 0, 16 );
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
      
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1Trigger_cond_LowPt_eta << rpclv1Trigger_cond_LowPt_eta_title.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked Trigger_Condition_LowPt_eta successfully" ); 
      
	/////////////////////////////////////////////
      
	// Trigger_Cond HighPt PHI
	std::string generic_path_rpclv1Trigger_cond_HighPt_phi = generic_path_rpclv1monitoring+"/Overview";
	std::string rpclv1Trigger_cond_HighPt_phi_title = "Trigger_Condition_HighPt_Phi";
	const char* rpclv1Trigger_cond_HighPt_phi_title_char = rpclv1Trigger_cond_HighPt_phi_title.c_str();
	TH1* rpclv1Trigger_cond_HighPt_phi = new TH1I( rpclv1Trigger_cond_HighPt_phi_title_char, rpclv1Trigger_cond_HighPt_phi_title_char, 16, 0, 16 );
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
      
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1Trigger_cond_HighPt_phi << rpclv1Trigger_cond_HighPt_phi_title.c_str() );
	//ATH_MSG_DEBUG ( "SHIFT : " << shift );
	ATH_MSG_DEBUG ( "RUN : " << run ); 	     	
	ATH_MSG_DEBUG ( "Booked Trigger_Condition_HighPt_Phi successfully" ); 
	
	///////////////////////////////////////////////
      
	// Trigger_Cond HighPt ETA
	std::string generic_path_rpclv1Trigger_cond_HighPt_eta = generic_path_rpclv1monitoring+"/Overview";
	std::string rpclv1Trigger_cond_HighPt_eta_title = "Trigger_Condition_HighPt_Eta";
	const char* rpclv1Trigger_cond_HighPt_eta_title_char = rpclv1Trigger_cond_HighPt_eta_title.c_str();
	TH1* rpclv1Trigger_cond_HighPt_eta = new TH1I( rpclv1Trigger_cond_HighPt_eta_title_char, rpclv1Trigger_cond_HighPt_eta_title_char, 16, 0, 16 );
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
      
	 
	ATH_MSG_DEBUG ( "INSIDE bookHistograms : " << rpclv1Trigger_cond_HighPt_eta << rpclv1Trigger_cond_HighPt_eta_title.c_str() );
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
	std::string m_generic_path_rpclv1triggertimemean = generic_path_rpclv1monitoring+"/Overview";
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
	std::string m_generic_path_rpclv1triggertimeRMS = generic_path_rpclv1monitoring+"/Overview";
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
	std::string rpclv1_BCid_per_TriggerType_title = "rpclv1_BCid_per_TriggerType";      
	const char* rpclv1_BCid_per_TriggerType_title_char = rpclv1_BCid_per_TriggerType_title.c_str();
       
	TH2 *rpclv1_BCid_per_TriggerType=new TH2I( rpclv1_BCid_per_TriggerType_title_char,rpclv1_BCid_per_TriggerType_title_char,8, 0, 8, 8*2, 0, 16); 				
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
	std::string rpclv1_BCid_per_TriggerCorr_title = "rpclv1_BCid_per_TriggerCorrelation";      
	const char* rpclv1_BCid_per_TriggerCorr_title_char = rpclv1_BCid_per_TriggerCorr_title.c_str();
       
	TH2 *rpclv1_BCid_per_TriggerCorr=new TH2I(rpclv1_BCid_per_TriggerCorr_title_char,rpclv1_BCid_per_TriggerCorr_title_char, 256, 0, 256, 8*2, 0, 16); 				
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
            std::vector<std::string> DB_list     ;
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
    
    std::vector<std::string>   rpclayersectorsidename = RpcGM::RpcLayerSectorSideName(m_rpcIdHelper,prdcoll_id, 0)  ; 
    std::string                sector_dphi_layer      = rpclayersectorsidename[12]             ;
	      
    std::vector<int>           RpcStrip = RpcGM::RpcStripShift(m_muonMgr,m_rpcIdHelper,prdcoll_id, 0);
    int strip_dbindex        = (RpcGM::RpcStripShift(m_muonMgr,m_rpcIdHelper,prdcoll_id, 0)).at(16);// cool strip profile
		if ( m_doCoolDB ) {
                  TH1* rpcCool_PanelIdHist = nullptr;
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



StatusCode RpcLv1RawDataValAlg::bookRPCLV1cmatimevschHistograms(std::string sectorlogic_name, std::string tower_name, std::string cma_name)
{
 
  StatusCode sc = StatusCode::SUCCESS;
 
  if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0Raw ) {  
 		    
    //declare a group of histograms
    std::string generic_path_rpclv1monitoring = "Muon/MuonRawDataMonitoring/RPCLV1";
    MonGroup rpclv1prd_expert( this, generic_path_rpclv1monitoring+"/TriggerRoad/"+sectorlogic_name, run, ATTRIB_UNMANAGED );   
    MuonDQAHistList& lst = m_stationHists.getList( sectorlogic_name );
    
    std::string generic_path_RPCLV1cmatimevsch = generic_path_rpclv1monitoring+"/"+sectorlogic_name+"/"+tower_name+"/rpclv1cosmic_cmatimevsch"+cma_name;
    std::string RPCLV1cmatimevsch_title = sectorlogic_name + "_" + tower_name + cma_name +"_time_vs_channel";     
    const char* RPCLV1cmatimevsch_title_char = RPCLV1cmatimevsch_title.c_str();
    TH2 *RPCLV1cmatimevsch=new TH2I(RPCLV1cmatimevsch_title_char,RPCLV1cmatimevsch_title_char, 32*7/m_rpclv1reducenbins, 0, 32*7, 64, 0,64);                       
    lst.addHist(RPCLV1cmatimevsch);
    RPCLV1cmatimevsch->SetFillColor(42);  
    RPCLV1cmatimevsch->SetMarkerColor(1);  
    RPCLV1cmatimevsch->SetMarkerStyle(21);   
    RPCLV1cmatimevsch->SetMarkerSize(0.2);
    RPCLV1cmatimevsch->GetXaxis()->SetTitle("Channel + 32 * ijk"     );
    RPCLV1cmatimevsch->GetYaxis()->SetTitle("Time=bcId*8+ticks"      );
     
    ATH_MSG_DEBUG ( "INSIDE bookRPCLV1cmatimevschHistograms : " << RPCLV1cmatimevsch << generic_path_RPCLV1cmatimevsch.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run ); 
  
    sc = rpclv1prd_expert.regHist( RPCLV1cmatimevsch );
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register RPCLV1cmatimevsch hist to MonGroup" );
 
    std::string generic_path_RPCLV1cmatimetriggervsch = generic_path_rpclv1monitoring+"/"+sectorlogic_name+"/"+tower_name+"/rpclv1cosmic_cmatimetriggervsch"+cma_name; 
    std::string RPCLV1cmatimetriggervsch_title = sectorlogic_name + "_" + tower_name + cma_name +"_(time-triggertime)_vs_channel";        
    const char* RPCLV1cmatimetriggervsch_title_char = RPCLV1cmatimetriggervsch_title.c_str ();
                 
    TH2 *RPCLV1cmatimetriggervsch=new TH2I(RPCLV1cmatimetriggervsch_title_char,RPCLV1cmatimetriggervsch_title_char, 32*7/m_rpclv1reducenbins, 0, 32*7, 2*64,-64, 64);             
    lst.addHist(RPCLV1cmatimetriggervsch);
    RPCLV1cmatimetriggervsch->SetFillColor(42);  
    RPCLV1cmatimetriggervsch->SetMarkerColor(1);  
    RPCLV1cmatimetriggervsch->SetMarkerStyle(21);   
    RPCLV1cmatimetriggervsch->SetMarkerSize(0.2);
    RPCLV1cmatimetriggervsch->GetXaxis()->SetTitle("Channel + 32 * ijk"     );
    RPCLV1cmatimetriggervsch->GetYaxis()->SetTitle("Time=bcId*8+ticks"      );
     
    ATH_MSG_DEBUG ( "INSIDE bookRPCLV1cmatimevschHistograms : " << RPCLV1cmatimetriggervsch << generic_path_RPCLV1cmatimetriggervsch.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run ); 
  
    sc = rpclv1prd_expert.regHist( RPCLV1cmatimetriggervsch );
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register RPCLV1cmatimetriggervsch hist to MonGroup" );

  }//AthenaMonManager::tier0 || AthenaMonManager::tier0Raw 
  return sc ;
    
}
 
StatusCode  RpcLv1RawDataValAlg::bookRPCLV1TriggerRoadHistograms(std::string sectorlogic_name, std::string tower_name, std::string cma_name, std::string thr_name)

{  
  // Trigger Road LowPt vs ijk channel   
  StatusCode sc = StatusCode::SUCCESS;

  if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0Raw  ) { 

    //declare a group of histograms
    std::string generic_path_rpclv1monitoring = "Muon/MuonRawDataMonitoring/RPCLV1";
    MonGroup rpclv1prd_expert( this, generic_path_rpclv1monitoring+"/TriggerRoad/"+sectorlogic_name, run, ATTRIB_UNMANAGED );   
    MuonDQAHistList& lst = m_stationHists.getList( sectorlogic_name );
  
    std::string generic_path_RPCLV1TriggerRoad =  generic_path_rpclv1monitoring+"/"+sectorlogic_name+"/"+tower_name+"/rpclv1cosmic_TriggerRoad"+cma_name+thr_name;
    std::string RPCLV1TriggerRoad_title = sectorlogic_name + "_" + tower_name + cma_name + thr_name + "_TriggerRoad";     
    const char* RPCLV1TriggerRoad_title_char = RPCLV1TriggerRoad_title.c_str();
    TH2 *RPCLV1TriggerRoad=new TH2I(RPCLV1TriggerRoad_title_char,RPCLV1TriggerRoad_title_char, 32/m_rpclv1reducenbins, 0, 32, 64/m_rpclv1reducenbins, 0, 64);                       
    lst.addHist(RPCLV1TriggerRoad);
    RPCLV1TriggerRoad->SetOption("COLZ");
    RPCLV1TriggerRoad->GetXaxis()->SetTitle("Channel  (ijk=6)"  );
    RPCLV1TriggerRoad->GetYaxis()->SetTitle("Confirm channel"   );
     
    ATH_MSG_DEBUG ( "INSIDE bookRPCLV1TriggerRoadHistograms : " << RPCLV1TriggerRoad << generic_path_RPCLV1TriggerRoad.c_str() );
    //ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run ); 
  
    sc = rpclv1prd_expert.regHist( RPCLV1TriggerRoad );
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register RPCLV1TriggerRoad hist to MonGroup" );

  }//AthenaMonManager::tier0 || AthenaMonManager::tier0Raw
  return sc ; 
        
}

///  profile cabling 
//void RpcLv1RawDataValAlg::bookRPCLV1ProfilesHistograms(int m_i_sector, std::string sectorlogic_name, int m_i_cmaId, std::string cma_name, int m_i_ijk, std::string m_ijk_name) 
StatusCode RpcLv1RawDataValAlg::bookRPCLV1ProfilesHistograms(int i_sector, std::string sectorlogic_name, std::string cma_name, int i_ijk, std::string ijk_name) 
{  
  // book profiles histograms
  
  StatusCode sc = StatusCode::SUCCESS;
  if( m_environment == AthenaMonManager::tier0 || m_environment == AthenaMonManager::tier0Raw  ) { 
  
    int N_pad = 6            ;  
    std::string cm_ch_label  =  "64" ;
    std::string pad_ch_label = "128" ;
  
    //declare a group of histograms
    std::string generic_path_rpclv1monitoring = "Muon/MuonRawDataMonitoring/RPCLV1";
    MonGroup rpclv1prd_expert( this, generic_path_rpclv1monitoring+"/Profiles/"+sectorlogic_name, run, ATTRIB_UNMANAGED );   
    MuonDQAHistList& lst = m_stationHists.getList( sectorlogic_name );
  
    std::string generic_path_RPCLV1Profiles = generic_path_rpclv1monitoring+"/"+sectorlogic_name+"_"+cma_name+"_"+ijk_name+"_profiles";
  
    if ( (i_sector % 4)==0 || (i_sector % 4)==3 )  { 
      N_pad = 7; 
    }
    int N_bin_profile = 0;
    if ( i_ijk<2 || i_ijk==6  ) { 
      N_bin_profile = 2 * 32 * N_pad ;
      cm_ch_label  = "32"  ;
      pad_ch_label = "64"  ; }
    else { N_bin_profile = 2 * 64 * N_pad; }
    /*
      if ( m_i_cmaId==0 || m_i_cmaId==1 ) { cma_name="Cma0_1" ; }
      if ( m_i_cmaId==2 || m_i_cmaId==3 ) { cma_name="Cma2_3" ; }
      if ( m_i_cmaId==4 || m_i_cmaId==5 ) { cma_name="Cma4_5" ; }
      if ( m_i_cmaId==6 || m_i_cmaId==7 ) { cma_name="Cma6_7" ; }
				       
      if ( i_ijk==2 || i_ijk==3 ) { ijk_name="ijk2_3" ; }
      if ( i_ijk==4 || i_ijk==5 ) { ijk_name="ijk4_5" ; }  */
    
    std::string RPCLV1Profiles_title = sectorlogic_name + "_" + cma_name+"_"+ ijk_name+"_Profiles";
    const char* RPCLV1Profiles_title_char = RPCLV1Profiles_title.c_str();
    TH1 *RPCLV1Profiles=new TH1I(RPCLV1Profiles_title_char,RPCLV1Profiles_title_char, N_bin_profile/m_rpclv1reducenbins, 0, N_bin_profile );
    lst.addHist(RPCLV1Profiles);
  
    std::string x_axis_title = "Channel+ (ijk-ijk_off)*32 +cm*"+cm_ch_label+" +pad*"+pad_ch_label ;
    const char* x_axis_title_char = x_axis_title.c_str();
    RPCLV1Profiles->GetXaxis()->SetTitle(x_axis_title_char );
    RPCLV1Profiles->GetYaxis()->SetTitle("Counts  "   );
    RPCLV1Profiles->SetFillColor(42) ;  // 38
     
    ATH_MSG_DEBUG ( "INSIDE bookRPCLV1ProfilesHistograms : " << RPCLV1Profiles << generic_path_RPCLV1Profiles.c_str() );
   // ATH_MSG_DEBUG ( "SHIFT : " << shift );
    ATH_MSG_DEBUG ( "RUN : " << run ); 
 
    sc = rpclv1prd_expert.regHist( RPCLV1Profiles );
    if(sc.isFailure() ) ATH_MSG_WARNING ( "couldn't register RPCLV1Profiles hist to MonGroup" );
  
  }//AthenaMonManager::tier0 || AthenaMonManager::tier0Raw 
  
  return sc ;     
}

 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

void RpcLv1RawDataValAlg::bookRPCCoolHistograms( std::vector<std::string>::const_iterator & iter, int isec, int idblPhi,
					      std::string layer ) 
{
  StatusCode sc = StatusCode::SUCCESS ;
  
  std::string generic_path_rpcmonitoring = "Muon/MuonRawDataMonitoring/RPCLV1";
  MonGroup rpcCoolDb( this, generic_path_rpcmonitoring+"/CoolDB", run, ATTRIB_UNMANAGED );
  
  char histName_char[100]	       ;
  sprintf(histName_char,"Sector%.2d_%s_dblPhi%d", isec+1, layer.c_str(), idblPhi+1) ;
  // example: Sector01_Pivot0_dblPhi1_StripId
  
  std::string histName  = histName_char  ;
  histName += "_"            ;
  histName += *iter        ;  //histName += m_coolQuantity ;
  int istatPhi  = int( isec/2) ;
  int iName     = 0              ;
  int kName = 0              ;
  int ig        = 0          ;
  int iNameMax  = 0          ;
  int ir = 0;

  if ( isec==12) {
    // if ( layer.find("Pivot",0) )
    if ( layer == "Pivot0" || layer == "Pivot1" )   {
      iName     =  1          ;
      iNameMax  =  2          ;
      ir    = 2 	      ;		
      ig    = atoi( (layer.substr(5,1)).c_str() ) ;
    }
    if ( layer == "LowPt0" || layer == "LowPt1" )   {
      iName     =  1          ;
      iNameMax  =  2          ;
      ir    = 1 	      ;
      ig    = atoi( (layer.substr(5,1)).c_str() ) ;
    }
    if ( layer == "HighPt0" || layer == "HighPt1" ) {
      iName = 4               ;
      iNameMax  =  iName      ;
      ir    = 1 	      ;
      ig    = atoi( (layer.substr(6,1)).c_str() ) ; 
    }
  }//sector 13
  else if ( isec==11 ||  isec==13){
    if ( layer == "Pivot0" || layer == "Pivot1" )   {
      iName = 8 ;
      iNameMax  =  10;
      ir    = 2 ;   
      ig    = atoi( (layer.substr(5,1)).c_str() ) ;   
    }
    if ( layer == "LowPt0" || layer == "LowPt1" )   {
      iName = 8 ;
      iNameMax  =  iName         ;
      ir    = 1 ;
      ig    = atoi( (layer.substr(5,1)).c_str() ) ;
    }
    if ( layer == "HighPt0" || layer == "HighPt1" ) {
      iName = 9 ; // or 10 ;
      iNameMax=10          ;
      ir    = 1 ; // doubletR=2 -> upgrade of Atlas
      ig    = atoi( (layer.substr(6,1)).c_str() ) ; 
    }
  } // end sectors 12 and 14  
  else {
    // if ( layer.find("Pivot",0) )
    if ( layer == "Pivot0" || layer == "Pivot1" )   {
      iName = 2 + (isec%2 ) ;
      ir    = 2 	      ;		
      ig    = atoi( (layer.substr(5,1)).c_str() ) ;
    }
    if ( layer == "LowPt0" || layer == "LowPt1" )   {
      iName = 2 + (isec%2 ) ;
      ir    = 1 	      ;
      ig    = atoi( (layer.substr(5,1)).c_str() ) ;
    }
    if ( layer == "HighPt0" || layer == "HighPt1" ) {
      iName = 4 + (isec%2 ) ;
      ir    = 1 	      ;
      ig    = atoi( (layer.substr(6,1)).c_str() ) ; 
    }
    iNameMax  =  iName         ;
  }
   
  kName = iName ;
  if(iName==1)kName=53;
  
  int NTotStripsSideA = 1;
  int NTotStripsSideC = 1;     
 
  const MuonGM::RpcReadoutElement* rpc = m_muonMgr->getRpcRElement_fromIdFields( kName, 1 , istatPhi+1, ir, 1, idblPhi+1 );    
  
  if(rpc != NULL ){  
    Identifier idr = rpc->identify();
    std::vector<int>   rpcstripshift = RpcGM::RpcStripShift(m_muonMgr,m_rpcIdHelper,idr, 0)  ;
    NTotStripsSideA = rpcstripshift[6]+rpcstripshift[17];
  } 
  rpc = m_muonMgr->getRpcRElement_fromIdFields( kName, -1 , istatPhi+1, ir, 1, idblPhi+1 );    
  
  if(rpc != NULL ){  
    Identifier idr = rpc->identify();
    std::vector<int>   rpcstripshift = RpcGM::RpcStripShift(m_muonMgr,m_rpcIdHelper,idr, 0)  ;
    NTotStripsSideC = rpcstripshift[7]+rpcstripshift[18]; 
  }
  
  
  TH1 *rpcCoolHisto = new TH1F(histName.c_str(), histName.c_str(), NTotStripsSideC+NTotStripsSideA, -NTotStripsSideC, NTotStripsSideA );

  sc=rpcCoolDb.regHist(rpcCoolHisto) ;
  if(sc.isFailure() ) ATH_MSG_WARNING (  "couldn't register " << histName << "hist to MonGroup" );
  rpcCoolHisto->GetXaxis()->SetTitle("strip");  
  
  
  
  // Fill strip Id histogram
  if ( (histName.find("ProfileCabling", 0)) != std::string::npos ) {
    
    TH1* rpcCool_PanelIdHist = nullptr;
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
	  if(isec==12&&abs(ieta-8)==6&&ir==1&&kName==2)continue;
	  if(abs(ieta-8)==7&&ir==2)irc=1; 
	  if(isec==12&&abs(ieta-8)==6&&ir==2)irc=1;	 
											   
    	  const MuonGM::RpcReadoutElement* rpc = m_muonMgr->getRpcRElement_fromIdFields(kName, ieta-8, istatPhi+1, irc, iz+1, idblPhi+1);  
    	  if( rpc == NULL ) continue;   
	  
    	  if  ( iz+1 != rpc->getDoubletZ() ) { 
    	    continue ;
    	  }
    	  Identifier idr = m_rpcIdHelper->parentID( rpc->identify() );
    	  rpcElemPhiStrip = int (rpc->NphiStrips() ) ;
    	  rpcElemEtaStrip = int (rpc->NetaStrips() ) ;
	  
	  
    	  for ( int istripEta=0; istripEta!=rpcElemEtaStrip; istripEta++ ) {
    	    Identifier strip_id  =  m_rpcIdHelper->channelID(idr, iz+1, idblPhi+1, ig+1, 0, istripEta+1) ;
    	    if( strip_id == 0 ) continue;
    	    coolStripIndex = (RpcGM::RpcStripShift(m_muonMgr,m_rpcIdHelper,strip_id, 0)).at(16);
    	    rpcCool_PanelIdHist->Fill(coolStripIndex, -1) ;
          }
    	  for ( int istripPhi=0; istripPhi!=rpcElemPhiStrip; istripPhi++ ) {
    	    Identifier strip_id  =  m_rpcIdHelper->channelID(idr, iz+1, idblPhi+1, ig+1, 1, istripPhi+1) ;
    	    if( strip_id == 0 ) continue;
    	    coolStripIndex = (RpcGM::RpcStripShift(m_muonMgr,m_rpcIdHelper,strip_id, 0)).at(16);
    	    rpcCool_PanelIdHist->Fill(coolStripIndex, -1 );
          }
	  
	  
	  
        } // end loop on doubletZ
      }
    }  // end loop on stationeta
 
  } // end fill cool histograms with panelId 
  
   
}  

 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 


StatusCode RpcLv1RawDataValAlg::procHistograms()
{
 
  ATH_MSG_DEBUG ( "RpcLv1RawDataValAlg finalize()" );
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
