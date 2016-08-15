/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Package: TgcLv1RawDataValAlg
// Authors:  A.Ishikawa(Kobe)  akimasa.ishikawa@cern.ch, M.King(Kobe) kingmgl@stu.kobe-u.ac.jp
// Original: Apr. 2008
// Modified: June 2011
//
// DESCRIPTION:
// Subject: TGCLV1-->Offline Muon Data Quality/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "StoreGate/DataHandle.h"

#include "MuonRDO/TgcRdo.h"
#include "MuonRDO/TgcRdoContainer.h"
#include "MuonRDO/TgcRdoIdHash.h"

// GeoModel
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/TgcReadoutParams.h"

#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonDQAUtils/MuonCosmicSetup.h"
#include "MuonDQAUtils/MuonDQAHistMap.h" 
 
#include "Identifier/Identifier.h"

#include "TgcRawDataMonitoring/TgcLv1RawDataValAlg.h"
#include "AthenaMonitoring/AthenaMonManager.h"

#include "AnalysisTriggerEvent/LVL1_ROI.h"

//offline 
#include "muonEvent/MuonContainer.h"

//for express menu
#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"

#include <TH1F.h>
#include <TH2F.h>
#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TF1.h>
#include <inttypes.h> 
#include <TProfile.h>

#include <sstream>

using namespace std;


///////////////////////////////////////////////////////////////////////////
// TgcLv1RawDataValAlg Constructor
///////////////////////////////////////////////////////////////////////////
TgcLv1RawDataValAlg::TgcLv1RawDataValAlg(const std::string &type, const std::string &name, const IInterface* parent)
  :ManagedMonitorToolBase(type, name, parent),
   m_log(msgSvc(), name),
   m_debuglevel(true){
  // Declare the properties
  declareProperty("TgcPrepDataContainer",         m_hitContainerLocation="TGC_Measurements");
  declareProperty("TgcPrepDataPreviousContainer", m_hitContainerLocationPrevious="TGC_MeasurementsPriorBC");
  declareProperty("TgcPrepDataNextContainer",     m_hitContainerLocationNext="TGC_MeasurementsNextBC");
  declareProperty("OutputCoinCollection",         m_coinCollectionLocation = "TrigT1CoinDataCollection" );
  declareProperty("OutputCoinCollectionPrevious", m_coinCollectionLocationPrevious = "TrigT1CoinDataCollectionPriorBC" );
  declareProperty("OutputCoinCollectionNext",     m_coinCollectionLocationNext = "TrigT1CoinDataCollectionNextBC" );
  declareProperty("L1muonRoIName",                m_L1muonRoIName = "LVL1MuonRoIs" );
  declareProperty("L1emtauRoIName",               m_L1emtauRoIName = "LVL1EmTauRoIs" );
  declareProperty("L1jetRoIName",                 m_L1jetRoIName = "LVL1JetRoIs" );

  // initialize the histos
  for(int ac=0;ac<2;ac++){
	  tgclv1lpttiming[ac] = 0;
	  tgclv1lpttimingptcut[ac] = 0;
	  tgclv1lpttimingmap[ac] = 0;
	  tgclv1lptcurrentfractionmap[ac] = 0;
	  tgclv1lpttimingmapptcut[ac] = 0;
	  tgclv1lptcurrentfractionmapptcut[ac] = 0;
	  tgclv1slvslpttiming[ac] = 0;
	  tgclv1slvslpttimingptcut[ac] = 0;
	  tgclv1_SL_trigger_timing_ES[ac] = 0;
	  tgclv1_SL_trigger_timing_num_ES[ac] = 0;
	  tgclv1_SL_trigger_timing_denom_ES[ac] = 0;
	  tgclv1_LPT_trigger_timing_ES[ac] = 0;
	  tgclv1_LPT_trigger_timing_num_ES[ac] = 0;
	  tgclv1_LPT_trigger_timing_denom_ES[ac] = 0;
	  tgclv1_SL_trigger_timing_ES_GM[ac] = 0;
	  tgclv1_LPT_trigger_timing_ES_GM[ac] = 0;
	  tgclv1_plateau_eff_counting_ES[ac] = 0;
	  tgclv1_plateau_eff_counting_num_ES[ac] = 0;
	  tgclv1_plateau_eff_counting_denom_ES[ac] = 0;
	  tgclv1_plateau_eff_counting_ES_GM[ac] = 0;
	  tgclv1summaryofsltiming[ac] = 0;
	  tgclv1summaryoflpttiming[ac] = 0;
	  
	  tgclv1slchamberlowstat[ac] = 0;
	  tgclv1sltimingptcutlowstat[ac] = 0;

	  tgclv1sltiming[ac] = 0;
	  tgclv1sltimingptcut[ac] = 0;
	  tgclv1sltimingmap[ac] = 0;
	  tgclv1slcurrentfractionmap[ac] = 0;
	  tgclv1sltimingmapptcut[ac] = 0;
	  tgclv1slcurrentfractionmapptcut[ac] = 0;

	  for(int isec=0; isec<12; isec++){
	    tgclv1sltimingsector[ac][isec] = 0;
	    tgclv1sltimingptcutsector[ac][isec] = 0;
	    tgclv1lpttimingsector[ac][isec] = 0;
	    tgclv1lpttimingptcutsector[ac][isec] = 0;
	    tgclv1slvslpttimingsector[ac][isec] = 0;
	    tgclv1slvslpttimingptcutsector[ac][isec] = 0;
	    for(int itrig=0; itrig<4; itrig++){
		tgclv1sltimingsectortrg[ac][isec][itrig] = 0;
		tgclv1sltimingptcutsectortrg[ac][isec][itrig] = 0;
		tgclv1lpttimingsectortrg[ac][isec][itrig] = 0;
		tgclv1lpttimingptcutsectortrg[ac][isec][itrig] = 0;
		tgclv1slvslpttimingsectortrg[ac][isec][itrig] = 0;
		tgclv1slvslpttimingptcutsectortrg[ac][isec][itrig] = 0;
	    }
	  }
	  for(int itrig=0; itrig<4; itrig++){
	    tgclv1sltimingtrg[ac][itrig] = 0;
	    tgclv1sltimingptcuttrg[ac][itrig] = 0;
	    tgclv1lpttimingtrg[ac][itrig] = 0;
	    tgclv1lpttimingptcuttrg[ac][itrig] = 0;
	  }
	  for(int im=0; im<2; im++){
	    tgclv1sltimingtrack[ac][im] = 0;
	    tgclv1sltimingptcuttrack[ac][im] = 0;
	    tgclv1lpttimingtrack[ac][im] = 0;
	    tgclv1lpttimingptcuttrack[ac][im] = 0;
	  }

	  for(int ipt=0; ipt<6; ipt++){
	    tgclv1turnon_ES[ac][ipt] = 0;
	    tgclv1turnontg_ES[ac][ipt] = 0;
	    tgclv1turnonnum_ES[ac][ipt] = 0;
	    tgclv1turnondenom_ES[ac][ipt] = 0;

	    tgclv1sltimingpt[ac][ipt] = 0;
	    tgclv1lpttimingpt[ac][ipt] = 0;
	    for(int itrig=0; itrig<4; itrig++){
		tgclv1sltimingpttrg[ac][ipt][itrig] = 0;
		tgclv1lpttimingpttrg[ac][ipt][itrig] = 0;
	    }
	    for(int im=0; im<2; im++)
		tgclv1deltar[ac][ipt][im] = 0;
	    for(int icharge=0; icharge<3; icharge++){
	       for(int imuid=0; imuid<2; imuid++){
		  tgclv1turnon[ac][ipt][icharge][imuid] = 0;
		  tgclv1turnontg[ac][ipt][icharge][imuid] = 0;
		  tgclv1turnonnum[ac][ipt][icharge][imuid] = 0;
		  tgclv1turnondenom[ac][ipt][icharge][imuid] = 0;
		  tgclv1effetavsphidenom[ac][ipt][icharge][imuid] = 0;
	          for(int ipcn=0; ipcn<3; ipcn++){
		     tgclv1effetavsphi[ac][ipt][icharge][imuid][ipcn] = 0;
		     tgclv1effetavsphinum[ac][ipt][icharge][imuid][ipcn] = 0;
		  }
	       }
	    }
	  }
          for(int mod=0;mod<9;mod++)
            for(int pt=0;pt<6;pt++){
              tgclv1cw[ac][mod][pt] = 0;
              for(int m=0;m<2;m++){
                tgclv1cwoffline[ac][mod][pt][m] = 0;
                tgclv1cwrejectedoffline[ac][mod][pt][m] = 0;
              }
            }

	
  }
  for(int ich=0; ich<6; ich++){
	  tgclv1summaryofsltimingperchambertype[ich] = 0;
	  tgclv1summaryoflpttimingperchambertype[ich] = 0;
  }

} 


///////////////////////////////////////////////////////////////////////////
// TgcLv1RawDataValAlg Destructor
///////////////////////////////////////////////////////////////////////////
TgcLv1RawDataValAlg::~TgcLv1RawDataValAlg(){
  m_log << MSG::INFO << " deleting TgcLv1RawDataValAlg " << endreq;
}


///////////////////////////////////////////////////////////////////////////
// TgcLv1RawDataValAlg initialize
///////////////////////////////////////////////////////////////////////////
StatusCode
TgcLv1RawDataValAlg::initialize(){
  // init message streams
  m_log.setLevel(outputLevel());                // individual outputlevel not known before initialise
  //m_log.setLevel(MSG::DEBUG);                   // individual outputlevel not known before initialise
  m_debuglevel = (m_log.level() <= MSG::DEBUG); // save if threshold for debug printout reached
  
  m_log << MSG::INFO << "in TgcLv1RawDataValAlg initialize" << endreq;

  StatusCode sc;

  /*
  // Store Gate store
  sc = serviceLocator()->service("StoreGateSvc", m_eventStore);
  if (sc != StatusCode::SUCCESS ) {
  m_log << MSG::ERROR << " Cannot get StoreGateSvc " << endreq;
  return sc ;
  }
  */
  
  // Retrieve the Active Store
  sc = serviceLocator()->service("ActiveStoreSvc", m_activeStore);
  if(sc != StatusCode::SUCCESS ){
    m_log << MSG::ERROR << " Cannot get ActiveStoreSvc " << endreq;
    return sc ;
  }

  // Initialize the IdHelper
  StoreGateSvc* detStore = 0;
  sc = service("DetectorStore", detStore);
  if(sc.isFailure()){
    m_log << MSG::FATAL << "DetectorStore service not found !" << endreq;
    return StatusCode::FAILURE;
  }   
  
  // Retrieve the MuonDetectorManager  
  sc = detStore->retrieve(m_muonMgr);
  if(sc.isFailure()){
    m_log << MSG::FATAL << "Cannot get MuonDetectorManager from detector store" << endreq;
    return StatusCode::FAILURE;
  }else{
    if (m_debuglevel) m_log << MSG::DEBUG << " Found the MuonDetectorManager from detector store. " << endreq;
  }

  sc = detStore->retrieve(m_tgcIdHelper,"TGCIDHELPER");
  if(sc.isFailure()){
    m_log << MSG::ERROR << "Can't retrieve TgcIdHelper" << endreq;
    return sc;
  } 
  
  /*
    if ( m_checkCabling ) {
    // get Cabling Server Service
    const ITGCcablingServerSvc* TgcCabGet = 0;
    sc = service("TGCcablingServerSvc", TgcCabGet);
    if (sc.isFailure()){
    m_log << MSG::ERROR << " Can't get TGCcablingServerSvc " << endreq;
    return StatusCode::FAILURE;
    }
    // get Cabling Service
    sc = TgcCabGet->giveCabling(m_cabling);
    if (sc.isFailure()){
    m_log << MSG::ERROR << " Can't get TGCcablingSvc Server" << endreq;
    return StatusCode::FAILURE; 
    }
    
    // check whether TGCcabling is compatible with 1/12 sector or not
    int maxRodId,maxSswId, maxSbloc,minChannelId, maxChannelId;
    m_cabling->getReadoutIDRanges( maxRodId,maxSswId, maxSbloc,minChannelId, maxChannelId);
    if (maxRodId ==12) {
    m_log << MSG::INFO << "TGCcabling12Svc OK" << endreq ;
    } else {
    m_log << MSG::WARNING << "TGCcablingSvc(octant segmentation) OK" << endreq ;
    }

    }
  */
  
  // Set number of Muon Algorithms to use, 1:muid, 2:muid&staco
  m_nMuonAlgorithms=1;
  if(m_environment==AthenaMonManager::online) m_nMuonAlgorithms=1;
  
  ManagedMonitorToolBase::initialize().ignore();// Ignore the checking code
  
  return StatusCode::SUCCESS;
}


///////////////////////////////////////////////////////////////////////////
// TgcLv1RawDataValAlg bookHistograms
///////////////////////////////////////////////////////////////////////////
StatusCode
TgcLv1RawDataValAlg::bookHistogramsRecurrent(){
  if (m_debuglevel) m_log << MSG::DEBUG << "TGCLV1 RawData Monitoring Histograms being booked" << endreq;
  
  // Book histograms per 10LBs 
  if( newRun || newLowStatInterval ){
    // not monitor these profiles at GM
    if( m_environment != AthenaMonManager::online ){
      if( ( bookHistogramsLowStat() ).isFailure() ){
        m_log << MSG::FATAL << "bookLowStatHisto() Failed  " << endreq;       
        return StatusCode::FAILURE;
      }
    }
    else{
      for(int ac=0;ac<2;ac++){
        tgclv1slchamberlowstat[ac] = 0;
        tgclv1sltimingptcutlowstat[ac] = 0;
      }
    }
  }// newLowStatInterval
  
  // Book histograms per ... (not used yet, but commenting them out causes "unused" warnings)
  if(newLumiBlock){}
  
  
  // Book histograms per Run
  if(newRun){         
    // Book Number Of Trigger And Profile histograms
    if((bookHistogramsNumberOfTriggersAndProfile()).isFailure()){
      m_log << MSG::FATAL << "bookNumberOfTriggersAndProfileHisto() Failed  " << endreq;
      return StatusCode::FAILURE;
    }

    // Book Number Of Trigger Rate histograms
    if((bookHistogramsTriggerRate()).isFailure()){
      m_log << MSG::FATAL << "bookTriggerRateHisto() Failed  " << endreq;
      return StatusCode::FAILURE;
    }

    // Book Timing histograms
    if((bookHistogramsTiming()).isFailure()){
      m_log << MSG::FATAL << "bookTimingHisto() Failed  " << endreq;
      return StatusCode::FAILURE;
    }

    // Book Turn-on curves and efficiency maps
    if((bookHistogramsEfficiency()).isFailure()){
      m_log << MSG::FATAL << "bookEfficiencyHisto() Failed  " << endreq;
      return StatusCode::FAILURE;
    }

    // not monitor these profiles at GM
    if( m_environment != AthenaMonManager::online ){
      // Book Coincidence Windows
      if((bookHistogramsCoincidenceWindow()).isFailure()){
        m_log << MSG::FATAL << "bookCoincidenceWindowHisto() Failed  " << endreq;
        return StatusCode::FAILURE;
      }
    }
    else
      for(int ac=0;ac<2;ac++)
        for(int mod=0;mod<9;mod++)
          for(int pt=0;pt<6;pt++){
            tgclv1cw[ac][mod][pt] = 0;
            for(int m=0;m<m_nMuonAlgorithms;m++){
              tgclv1cwoffline[ac][mod][pt][m] = 0;
              tgclv1cwrejectedoffline[ac][mod][pt][m] = 0;
            }
          }

    // Book Summary histograms
    if( ( bookHistogramsSummary() ).isFailure() ){
      m_log << MSG::FATAL << "bookSummaryHisto() Failed  " << endreq;
      return StatusCode::FAILURE;
    }
  }// newRun

  m_log << MSG::INFO << "TGCLV1 RawData Monitoring : histo booked " << endreq;

  return StatusCode::SUCCESS;
}// EOF


///////////////////////////////////////////////////////////////////////////
// TgcLv1RawDataValAlg fillHistograms
///////////////////////////////////////////////////////////////////////////
StatusCode
TgcLv1RawDataValAlg::fillHistograms(){
  StatusCode sc = StatusCode::SUCCESS;
  if (m_debuglevel) m_log << MSG::DEBUG << "TgcLv1RawDataValAlg::TGCLV1 RawData Monitoring Histograms being filled" << endreq;  
  
  ///////////////////////////////////////////////////////////////////////////
  // Get Event Data
  /////////////////////////////////////
  // Read Event Info
  sc=readEventInfo();
  if( sc.isFailure() ){
    m_log << MSG::WARNING << "no event info container" << endreq;
  }
  // Read L1 Trigger Data
  sc = readL1TriggerType();
  if( sc.isFailure() )return sc;
  
  /////////////////////////////////////
  // Get TGC Coin Containers
  const Muon::TgcCoinDataContainer* tgc_previous_coin_container(0);
  const Muon::TgcCoinDataContainer* tgc_current_coin_container(0);
  const Muon::TgcCoinDataContainer* tgc_next_coin_container(0);
  
  // Previous
  sc = (*m_activeStore)->retrieve(tgc_previous_coin_container, m_coinCollectionLocationPrevious);
  if (sc.isFailure() || 0 == tgc_previous_coin_container ) {
    m_log << MSG::ERROR << " Cannot retrieve TgcCoinDataContainer for previous BC" << endreq;
    return sc;
  }
  if (m_debuglevel) m_log << MSG::DEBUG << "****** tgc previous coin container size() : " << tgc_previous_coin_container->size() << endreq;
  
  // Current
  sc = (*m_activeStore)->retrieve(tgc_current_coin_container, m_coinCollectionLocation);
  if (sc.isFailure() || 0 == tgc_current_coin_container ) {
    m_log << MSG::ERROR << " Cannot retrieve TgcCoinDataContainer for current BC" << endreq;
    return sc;
  }
  if (m_debuglevel) m_log << MSG::DEBUG << "****** tgc current coin container size() : " << tgc_current_coin_container->size() << endreq;
  
  // Next
  sc = (*m_activeStore)->retrieve(tgc_next_coin_container, m_coinCollectionLocationNext);
  if (sc.isFailure() || 0 == tgc_next_coin_container ) {
    m_log << MSG::ERROR << " Cannot retrieve TgcCoinDataContainer for next BC" << endreq;
    return sc;
  }
  if (m_debuglevel) m_log << MSG::DEBUG << "****** tgc next coin container size() : " << tgc_next_coin_container->size() << endreq;
  
  
  ///////////////////////////////////////////////////////////////////////////
  // ReadContainer
  /////////////////////////////////////
  // Read Offline Muon Containers
  if(readOfflineMuonContainer( "Muons",  &m_muid_pt,  &m_muid_eta,  &m_muid_phi,  &m_muid_q).isFailure()){
    m_log << MSG::ERROR << " fillOfflineMuon failed" << endreq;
    return StatusCode::FAILURE;
  }

  /////////////////////////////////////
  // Get Data from TGC Containers
  clearVectorsArrays();
  // fill vectors and arrays from TgcCoinData
  readTgcCoinDataContainer(tgc_previous_coin_container, PREV);
  readTgcCoinDataContainer( tgc_current_coin_container, CURR);
  readTgcCoinDataContainer(    tgc_next_coin_container, NEXT);
  readTgcCoinDataContainer(    tgc_next_coin_container, TOTA);
  
  
  ///////////////////////////////////////////////////////////////////////////
  // Fill Histograms
  
  // NumberOfTrigger
  fillNumberOfTrigger();
  
  // TriggerTiming
  int ptcut=1;
  fillTriggerTiming(ptcut);
  
  int ms;
  ms=0; //Muid
  fillTriggerTimingAssociatedWithTrack( ms, &m_muid_pt,  &m_muid_eta,  &m_muid_phi,  &m_muid_q );
  
  // Efficiency
  ms=0; //Muid
  fillEfficiency( ms, &m_muid_pt,  &m_muid_eta,  &m_muid_phi,  &m_muid_q );
  
  
  // Coincidence Window
  // not monitor these profiles at GM
  if( m_environment != AthenaMonManager::online ){
    fillCoincidenceWindow( ms, &m_muid_pt,  &m_muid_eta,  &m_muid_phi,  &m_muid_q );
  }
  
  /*
    int numberOfSL=m_nSL[0]+m_nSL[1]+m_nSL[2];
    // check hit Timing if single RoI
    if (m_debuglevel) m_log << MSG::DEBUG << "number of SL " << numberOfSL << endreq;
    if(numberOfSL!=1) return StatusCode::SUCCESS;//not single RoI
  */
  
  return sc;  // statuscode check  
}// EOF


///////////////////////////////////////////////////////////////////////////
// TgcLv1RawDataValAlg procHistograms
///////////////////////////////////////////////////////////////////////////
StatusCode
TgcLv1RawDataValAlg::procHistograms(){
  if (m_debuglevel) m_log << MSG::DEBUG << "TgcLv1RawDataValAlg finalize()" << endreq;
  
  // Process histograms per LumiBlock
  if(endOfLumiBlock){

    // Efficiency for Global Monitoring
    if( m_environment == AthenaMonManager::online ) {
      // express stream only
      for(int ac=0;ac<2;ac++){
        for(int pt=0;pt<6;pt++) {
          tgclv1turnon_ES[ac][pt]  ->Divide( tgclv1turnonnum_ES[ac][pt], tgclv1turnondenom_ES[ac][pt], 1, 1, "B" );
         if(tgclv1turnondenom_ES[ac][pt]->Integral()>0.) tgclv1turnontg_ES[ac][pt]->BayesDivide( tgclv1turnonnum_ES[ac][pt], tgclv1turnondenom_ES[ac][pt] );
        }//pt

        tgclv1_plateau_eff_counting_ES[ac]   ->Divide( tgclv1_plateau_eff_counting_num_ES[ac], tgclv1_plateau_eff_counting_denom_ES[ac], 1,1,"B" );
        tgclv1_plateau_eff_counting_ES_GM[ac]->Divide( tgclv1_plateau_eff_counting_num_ES[ac], tgclv1_plateau_eff_counting_denom_ES[ac], 1,1,"B" );

        tgclv1_SL_trigger_timing_ES[ac] ->Divide( tgclv1_SL_trigger_timing_num_ES[ac],  tgclv1_SL_trigger_timing_denom_ES[ac]  );
        tgclv1_LPT_trigger_timing_ES[ac]->Divide( tgclv1_LPT_trigger_timing_num_ES[ac], tgclv1_LPT_trigger_timing_denom_ES[ac] );

        tgclv1_SL_trigger_timing_ES_GM[ac] ->Divide( tgclv1_SL_trigger_timing_num_ES[ac],  tgclv1_SL_trigger_timing_denom_ES[ac]  );
        tgclv1_LPT_trigger_timing_ES_GM[ac]->Divide( tgclv1_LPT_trigger_timing_num_ES[ac], tgclv1_LPT_trigger_timing_denom_ES[ac] );

      }//ac
    }
  }
  // Process histograms per Run
  if(endOfRun){
    ///////////////////////////////////////////////////////////////////////////
    // TriggerTiming
    // SL/LPT timing
    for(int ac=0;ac<2;ac++){// side
      for(int ieta=0;ieta<6;ieta++){// stationEta
        // Get eta bin indexes for different timing
        int etabinp = ieta;
        int etabinc = ieta + 6;
        int etabinn = ieta + 12;

        for(int phi48=1;phi48<49;phi48++){// stationPhi
          float   p,   c,   n, tot;
          float  fp,  fc,  fn;
          float efp, efc, efn;

          /////////////////////////////////////
          // SL Timing Map
          // Get Total number of triggers for this chamber
          p = tgclv1sltimingmap[ac]->GetBinContent(etabinp +1, phi48);
          c = tgclv1sltimingmap[ac]->GetBinContent(etabinc +1, phi48);
          n = tgclv1sltimingmap[ac]->GetBinContent(etabinn +1, phi48);
          tot = p+c+n;

          // Get Fractions and Errors
          fp=fc=fn=efp=efc=efn=0;
          if( tot != 0 ){
            fp= p/tot; efp = sqrt( fp*(1.-fp)/tot );
            fc= c/tot; efc = sqrt( fc*(1.-fc)/tot );
            fn= n/tot; efn = sqrt( fn*(1.-fn)/tot );
          }
          // Fill Fractions into map
          tgclv1slcurrentfractionmap[ac]->SetBinContent(etabinp +1,  phi48, fp);
          tgclv1slcurrentfractionmap[ac]->SetBinContent(etabinc +1,  phi48, fc);
          tgclv1slcurrentfractionmap[ac]->SetBinContent(etabinn +1,  phi48, fn);
          tgclv1slcurrentfractionmap[ac]->SetBinError(etabinp +1,  phi48, efp);
          tgclv1slcurrentfractionmap[ac]->SetBinError(etabinc +1,  phi48, efc);
          tgclv1slcurrentfractionmap[ac]->SetBinError(etabinn +1,  phi48, efn);
          
          /////////////////////////////////////
          // SL Timing Map
          // Get Total number of triggers for this chamber
          p = tgclv1lpttimingmap[ac]->GetBinContent(etabinp +1, phi48);
          c = tgclv1lpttimingmap[ac]->GetBinContent(etabinc +1, phi48);
          n = tgclv1lpttimingmap[ac]->GetBinContent(etabinn +1, phi48);
          tot = p+c+n;

          // Get Fractions and Errors
          fp=fc=fn=efp=efc=efn=0;
          if( tot != 0 ){
            fp= p/tot; efp = sqrt( fp*(1.-fp)/tot );
            fc= c/tot; efc = sqrt( fc*(1.-fc)/tot );
            fn= n/tot; efn = sqrt( fn*(1.-fn)/tot );
          }
          // Fill Fractions into map
          tgclv1lptcurrentfractionmap[ac]->SetBinContent(etabinp +1,  phi48, fp);
          tgclv1lptcurrentfractionmap[ac]->SetBinContent(etabinc+1,  phi48, fc);
          tgclv1lptcurrentfractionmap[ac]->SetBinContent(etabinn+1,  phi48, fn);
          tgclv1lptcurrentfractionmap[ac]->SetBinError(etabinp +1,  phi48, efp);
          tgclv1lptcurrentfractionmap[ac]->SetBinError(etabinc+1,  phi48, efc);
          tgclv1lptcurrentfractionmap[ac]->SetBinError(etabinn+1,  phi48, efn);

        }
      }
    }

    ///////////////////////////////////////////////////////////////////////////
    // NumberOfTrigger
    for(int ac=0;ac<2;ac++){
      tgclv1slpereventlb[ac]->Sumw2();
      tgclv1slpereventlb[ac]->Divide( tgclv1slinlb[ac], tgclv1eventsinlb );

      tgclv1slpereventbcid[ac]->Sumw2();
      tgclv1slpereventbcid[ac]->Divide( tgclv1slinbcid[ac], tgclv1eventsinbcid );

      for(int pt=0;pt<6;pt++){
        tgclv1slpereventlbpt[ac][pt]->Sumw2();
        tgclv1slpereventlbpt[ac][pt]->Divide( tgclv1slinlbpt[ac][pt], tgclv1eventsinlb );

        tgclv1slpereventbcidpt[ac][pt]->Sumw2();
        tgclv1slpereventbcidpt[ac][pt]->Divide( tgclv1slinbcidpt[ac][pt], tgclv1eventsinbcid );
      }
    }

    ///////////////////////////////////////////////////////////////////////////
    // Efficiency
    for(int ac=0;ac<2;ac++){// side
      for(int ipt=0;ipt<6;ipt++){// pt
        for(int mu=0;mu<m_nMuonAlgorithms;mu++){// muon algorithm
          for(int pna=0;pna<3;pna++){// charge
            // Fill histograms for Both charges
            if(pna==2){
              for(int pcn=0;pcn<3;pcn++){// pcn
                tgclv1effetavsphinum[ac][ipt][pna][mu][pcn]->Add(tgclv1effetavsphinum[ac][ipt][0][mu][pcn], tgclv1effetavsphinum[ac][ipt][1][mu][pcn]);
              }
              tgclv1effetavsphidenom[ac][ipt][pna][mu]->Add(tgclv1effetavsphidenom[ac][ipt][0][mu], tgclv1effetavsphidenom[ac][ipt][1][mu]);
              tgclv1turnonnum[ac][ipt][pna][mu]       ->Add(tgclv1turnonnum[ac][ipt][0][mu],        tgclv1turnonnum[ac][ipt][1][mu]);
              tgclv1turnondenom[ac][ipt][pna][mu]     ->Add(tgclv1turnondenom[ac][ipt][0][mu],      tgclv1turnondenom[ac][ipt][1][mu]);
            }

            // Calculate Efficiencies
            for(int pcn=0;pcn<3;pcn++){// pcn
              //tgclv1turnon[ac][ipt][mu]->Sumw2();
              tgclv1turnon[ac][ipt][pna][mu]->Divide(tgclv1turnonnum[ac][ipt][pna][mu], tgclv1turnondenom[ac][ipt][pna][mu], 1, 1, "B");
              if(tgclv1turnondenom[ac][ipt][pna][mu]->Integral()>0.)tgclv1turnontg[ac][ipt][pna][mu]->BayesDivide(tgclv1turnonnum[ac][ipt][pna][mu], tgclv1turnondenom[ac][ipt][pna][mu]);

              //tgclv1effetavsphi[ac][ipt][mu]->Sumw2();
              tgclv1effetavsphi[ac][ipt][pna][mu][pcn]->Divide(tgclv1effetavsphinum[ac][ipt][pna][mu][pcn], tgclv1effetavsphidenom[ac][ipt][pna][mu], 1, 1, "B");
            }// pcn
          }// charge
        }// mu
      }// pt

      /////////////////////////////////////
      // express stream only
      for(int ipt=0;ipt<6;ipt++){// pt
        //tgclv1turnon_ES[ac][pt][mu]->Sumw2();
        tgclv1turnon_ES[ac][ipt]->Divide( tgclv1turnonnum_ES[ac][ipt], tgclv1turnondenom_ES[ac][ipt], 1, 1, "B" );
        if((tgclv1turnondenom_ES[ac][ipt]->Integral())>0.)tgclv1turnontg_ES[ac][ipt]->BayesDivide( tgclv1turnonnum_ES[ac][ipt], tgclv1turnondenom_ES[ac][ipt] );
      }// pt

      tgclv1_plateau_eff_counting_ES[ac]->Divide( tgclv1_plateau_eff_counting_num_ES[ac], tgclv1_plateau_eff_counting_denom_ES[ac], 1,1,"B" );
      tgclv1_plateau_eff_counting_ES_GM[ac]->Divide( tgclv1_plateau_eff_counting_num_ES[ac], tgclv1_plateau_eff_counting_denom_ES[ac], 1,1,"B" );

      tgclv1_SL_trigger_timing_ES[ac] -> Divide( tgclv1_SL_trigger_timing_num_ES[ac], tgclv1_SL_trigger_timing_denom_ES[ac] );
      tgclv1_LPT_trigger_timing_ES[ac] -> Divide( tgclv1_LPT_trigger_timing_num_ES[ac], tgclv1_LPT_trigger_timing_denom_ES[ac] );
      tgclv1_SL_trigger_timing_ES_GM[ac] -> Divide( tgclv1_SL_trigger_timing_num_ES[ac], tgclv1_SL_trigger_timing_denom_ES[ac] );
      tgclv1_LPT_trigger_timing_ES_GM[ac] -> Divide( tgclv1_LPT_trigger_timing_num_ES[ac], tgclv1_LPT_trigger_timing_denom_ES[ac] );
    }// side

  } //endOfRun

  return StatusCode::SUCCESS;
}



