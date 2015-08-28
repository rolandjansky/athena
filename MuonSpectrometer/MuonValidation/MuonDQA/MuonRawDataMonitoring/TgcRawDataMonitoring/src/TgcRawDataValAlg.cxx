/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Package: TgcRawDataValAlg
// Authors:  N. Benekos(Illinois), M. Ishino(ICEPP), H.Kurashige (Kobe), A.Ishikawa (Kobe)  akimasa.ishikawa@cern.ch, M.King(Kobe) kingmgl@stu.kobe-u.ac.jp
// Original: Apr. 2008
// Modified: June 2011
//
// DESCRIPTION:
// Subject: TGCLV1-->Offline Muon Data Quality/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

// GeoModel
#include "MuonReadoutGeometry/TgcReadoutParams.h"

// Cabling Service
//#include "TGCcablingInterface/ITGCcablingServerSvc.h"

#include "Identifier/Identifier.h"

#include "xAODEventInfo/EventInfo.h"

// MuonRDO
#include "MuonRDO/TgcRdo.h"
#include "MuonRDO/TgcRdoIdHash.h"
#include "MuonRDO/TgcRdoContainer.h"

#include "MuonDigitContainer/TgcDigitContainer.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonDQAUtils/MuonCosmicSetup.h"
//#include "MuonDQAUtils/TGCDQAUtils.h"

 
#include "TgcRawDataMonitoring/TgcRawDataValAlg.h"
#include "AthenaMonitoring/AthenaMonManager.h"

#include <TError.h>
#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TF1.h>
#include <inttypes.h> 

#include <sstream>
#include <math.h>

using namespace std;

/////////////////////////////////////////////////////////////////////////////

TgcRawDataValAlg::TgcRawDataValAlg( const std::string & type, const std::string & name, const IInterface* parent )
  :ManagedMonitorToolBase( type, name, parent ),
   m_log( msgSvc(), name ),
   m_debuglevel(false){
  // Declare the properties 
  declareProperty("TgcPrepDataContainer",         m_tgcPrepDataContainerName = "TGC_Measurements");
  declareProperty("TgcPrepDataPreviousContainer", m_tgcPrepDataPreviousContainerName="TGC_MeasurementsPriorBC");
  declareProperty("TgcPrepDataNextContainer",     m_tgcPrepDataNextContainerName="TGC_MeasurementsNextBC");
  declareProperty("OutputCoinCollection", m_outputCoinCollectionLocation = "TrigT1CoinDataCollection" );
  m_nEvent=0;
  m_numberingVersion=0; 
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
TgcRawDataValAlg::~TgcRawDataValAlg(){
  m_log << MSG::INFO << " deleting TgcRawDataValAlg " << endreq;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode
TgcRawDataValAlg::initialize(){

  // init message stream
  m_log.setLevel(outputLevel());                // individual outputlevel not known before initialise
  //m_log.setLevel(MSG::DEBUG);
  m_debuglevel = (m_log.level() <= MSG::DEBUG); // save if threshold for debug printout reached
  
  m_log << MSG::INFO << "in initializing TgcRawDataValAlg" << endreq;

  StatusCode sc;
  sc = ManagedMonitorToolBase::initialize();
  if(!sc.isSuccess()) return sc;
  
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
  if (sc != StatusCode::SUCCESS ) {
    m_log << MSG::ERROR << " Cannot get ActiveStoreSvc " << endreq;
    return sc ;
  }

  // Initialize the IdHelper
  StoreGateSvc* detStore = 0;
  sc = service("DetectorStore", detStore);
  if (sc.isFailure()) {
    m_log << MSG::FATAL << "DetectorStore service not found !" << endreq;
    return StatusCode::FAILURE;
  }   
  
  // Retrieve the MuonDetectorManager  
  sc = detStore->retrieve(m_muonMgr);
  if (sc.isFailure()) {
    m_log << MSG::FATAL 
          << "Cannot get MuonDetectorManager from detector store" << endreq;
    return StatusCode::FAILURE;
  }   else { if (m_debuglevel) m_log << MSG::DEBUG  << " Found the MuonDetectorManager from detector store. " << endreq; }

  sc = detStore->retrieve(m_tgcIdHelper,"TGCIDHELPER");
  if (sc.isFailure()) {
    m_log << MSG::ERROR << "Can't retrieve TgcIdHelper" << endreq;
    return sc;
  }
  
  //histograms directory names
  generic_path_tgcmonitoring = "Muon/MuonRawDataMonitoring/TGC";

  //tgcchamberId();
  
  
  //offset
  setOffset();
  //set minimum and maximum of channel difference
  setChannelDifferenceRange();

  //  m_log << MSG::INFO<<"TgcRawDataValAlg initialize finished "<<endreq;

  //do not monitor profile histograms
  //m_mon_profile=false;
  m_mon_profile=true;
  
  return StatusCode::SUCCESS;
} 


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode
TgcRawDataValAlg::bookHistogramsRecurrent(){
  if (m_debuglevel) m_log << MSG::DEBUG << "TGC RawData Monitoring Histograms being booked" << endreq;
  StatusCode sc = StatusCode::SUCCESS; 

  if( newRun || newLowStatInterval ){
    // not monitor these profiles at GM
    if( m_environment != AthenaMonManager::online ) {
      sc = bookHistogramsLowStat();
      if(sc.isFailure()){
        m_log << MSG::FATAL << "bookLowStatHisto() Failed  " << endreq;       
        return StatusCode::FAILURE;
      }
    }
    else
      for(int ac=0;ac<2;ac++){
        tgcwirestripcoinlowstat[ac] = 0;
      }
  }
     
  if(newLumiBlock){}   

  if(newRun) {      
    //if (m_debuglevel) m_log << MSG::DEBUG << "TGC RawData Monitoring : newRun" << endreq;

    sc = bookHistogramsNumberOfHits();
    sc = bookHistogramsProfile();
    sc = bookHistogramsXYView();
    sc = bookHistogramsEfficiency();
    sc = bookHistogramsSummary();
    
    if (m_debuglevel) {
      m_log << MSG::DEBUG << "INSIDE bookHistograms : " << tgcevents << generic_path_tgcmonitoring << endreq;
      //m_log << MSG::DEBUG << "SHIFT : " << shift << "  RUN : " << run << "  Booked booktgcevents successfully" << endreq; //attention
    }


    if(m_debuglevel) m_log << MSG::DEBUG << "have registered histograms for Number of Wire/Strip Hits" << endreq ;


  }//new run
  //m_log << MSG::INFO << "TGC RawData Monitoring Histograms booked" << endreq;
  
  return sc;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode
TgcRawDataValAlg::fillHistograms(){
  StatusCode sc = StatusCode::SUCCESS; 
  
  if (m_debuglevel) m_log << MSG::DEBUG << "TgcRawDataValAlg::TGC RawData Monitoring Histograms being filled" << endreq;
  
  clearVectorsArrays();

  //fillEventInfo information
  if ( (fillEventInfo() ).isFailure() ){
    m_log << MSG::ERROR << " Cannot fillEventInfo " << endreq;
    return sc;
  }
  
  /////////////////////////////////////
  // Get TGC Hit PRD Containers
  const Muon::TgcPrepDataContainer* tgc_previous_prd_container(0);
  const Muon::TgcPrepDataContainer* tgc_current_prd_container(0);
  const Muon::TgcPrepDataContainer* tgc_next_prd_container(0);
  
  // Previous
  sc = (*m_activeStore)->retrieve(tgc_previous_prd_container, m_tgcPrepDataPreviousContainerName);
  if(sc.isFailure()|| 0 == tgc_previous_prd_container) {
    m_log << MSG::WARNING << " Cannot retrieve TgcPrepDataContainer for previous BC" << endreq;
    return sc;
  }
  if(m_debuglevel) m_log << MSG::DEBUG << "****** tgc previous prd container size() : " << tgc_previous_prd_container->size() << endreq;
  
  // Current
  sc = (*m_activeStore)->retrieve(tgc_current_prd_container, m_tgcPrepDataContainerName);
  if(sc.isFailure()|| 0 == tgc_current_prd_container ) {
    m_log << MSG::WARNING << " Cannot retrieve TgcPrepDataContainer for current BC" << endreq;
    return sc;
  }
  if(m_debuglevel) m_log << MSG::DEBUG << "****** tgc current prd container size() : " << tgc_current_prd_container->size() << endreq;
  
  // Next
  sc = (*m_activeStore)->retrieve(tgc_next_prd_container, m_tgcPrepDataNextContainerName);
  if(sc.isFailure()|| 0 == tgc_next_prd_container) {
    m_log << MSG::WARNING << " Cannot retrieve TgcPrepDataContainer for next BC" << endreq;
    return sc;
  }
  if(m_debuglevel) m_log << MSG::DEBUG << "****** tgc next prd container size() : " << tgc_next_prd_container->size() << endreq;
  
  
  // Increment event counter
  m_nEvent++;
  if(m_debuglevel) m_log << MSG::DEBUG <<"event : " << m_nEvent << endreq;
  
  
  /////////////////////////////////////
  // Get Data from TGC Containers
  clearVectorsArrays();
  // fill vectors and arrays from TgcPrepData
  readTgcPrepDataContainer(tgc_previous_prd_container, PREV);
  readTgcPrepDataContainer( tgc_current_prd_container, CURR);
  readTgcPrepDataContainer(    tgc_next_prd_container, NEXT);
  
  
  ///////////////////////////////////////////////////////////////////////////
  // Fill Histograms
  
  // fill number of hit histograms
  sc=fillNumberOfHits();
  
  // fill profile histograms
  sc=fillProfile();

  // fill efficiency
  sc=fillEfficiency();
  
  // fill XY view
  sc=fillXYView();
  
  //vector<double> SLEta[2];//[ac]
  //vector<double> SLPhi[2];
  
  if (m_debuglevel) {
    m_log << MSG::DEBUG << "********TGC event number : " << m_nEvent << endreq;   
  }

  return sc; // statuscode check 
 
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode
TgcRawDataValAlg::procHistograms(){
  if (m_debuglevel) {
    m_log << MSG::DEBUG << "********Reached Last Event in TgcRawDataValAlg !!!" << endreq;
    m_log << MSG::DEBUG << "TgcRawDataValAlg finalize()" << endreq;
  } 
  if(endOfLumiBlock){
    //histogram division every LB only for Global Montioring
    if(m_environment==AthenaMonManager::online){
      // calculate occupancy map
      for(int ac=0;ac<2;ac++){
        for(int ws=0;ws<2;ws++){
          
          if( tgcprofilemap[ac][ws] && tgcoccupancymap[ac][ws] ){
            int nbinx=tgcprofilemap[ac][ws]->GetNbinsX();
            int nbiny=tgcprofilemap[ac][ws]->GetNbinsY();
            
            for(int binx=1;binx<=nbinx;binx++)
              for(int biny=1;biny<=nbiny;biny++){
                tgcoccupancymap[ac][ws]->SetBinContent( binx, biny, nWireStripMap( ws, binx, biny ) * m_nEvent );
              }
            tgcoccupancymap[ac][ws]->Divide( tgcprofilemap[ac][ws], tgcoccupancymap[ac][ws], 1., 1., "B");
          }

        }// ws
      }// ac


      // calculate efficiency histograms and map
      for(int ac=0;ac<2;ac++){
        if(tgceff[ac] &&
           tgceffnum[ac] &&
           tgceffdenom[ac] )
          tgceff[ac]->Divide(tgceffnum[ac], tgceffdenom[ac], 1., 1., "B");
      
        for(int ws=0;ws<2;ws++){
          if(tgceffmap[ac][ws] &&
             tgceffmapnum[ac][ws] &&
             tgceffmapdenom[ac][ws]){
            tgceffmap[ac][ws]->Divide(tgceffmapnum[ac][ws], tgceffmapdenom[ac][ws], 1., 1., "B");
          }
          
          for(int bc=0;bc<2;bc++){
            if(tgceffmapbc[ac][ws][bc] &&
               tgceffmapnumbc[ac][ws][bc] &&
               tgceffmapdenom[ac][ws]){
              tgceffmapbc[ac][ws][bc]->Divide(tgceffmapnumbc[ac][ws][bc], tgceffmapdenom[ac][ws], 1., 1., "B" );
            }
          }
          
          if(tgceffsector[ac][ws] &&
             tgceffsectornum[ac][ws] &&
             tgceffsectordenom[ac][ws] )
            tgceffsector[ac][ws]->Divide(tgceffsectornum[ac][ws], tgceffsectordenom[ac][ws], 1., 1., "B");
            
        }// ws
      }// ac
        
    }// isOnlineMonitoring
  }//endOfLumiBlock

  if(endOfRun){
    // calculate occupancy map
    for(int ac=0;ac<2;ac++){
      for(int ws=0;ws<2;ws++){
        
        if(tgcprofilemap[ac][ws] && tgcoccupancymap[ac][ws]){
          int nbinx=tgcprofilemap[ac][ws]->GetNbinsX();
          int nbiny=tgcprofilemap[ac][ws]->GetNbinsY();
          
          for(int binx=1;binx<=nbinx;binx++)
            for(int biny=1;biny<=nbiny;biny++){
              //tgcoccupancymap[ac][ws]->SetBinContent( binx, biny, nWireStripMap(ac, binx, biny ) * m_nEvent );//run1 default
              tgcoccupancymap[ac][ws]->SetBinContent( binx, biny, nWireStripMap(ws, binx, biny ) * m_nEvent );
            }
          tgcoccupancymap[ac][ws]->Divide( tgcprofilemap[ac][ws], tgcoccupancymap[ac][ws], 1., 1., "B");
        }

      }// ws
    }// ac

    // calculate efficiency histogram and map
    for(int ac=0;ac<2;ac++){
      if(tgceff[ac] &&
         tgceffnum[ac] &&
         tgceffdenom[ac] )
        tgceff[ac]->Divide(tgceffnum[ac], tgceffdenom[ac], 1., 1., "B");

      for(int ws=0;ws<2;ws++){
        if(tgceffmap[ac][ws] &&
           tgceffmapnum[ac][ws] &&
           tgceffmapdenom[ac][ws] ){
          tgceffmap[ac][ws]->Divide(tgceffmapnum[ac][ws], tgceffmapdenom[ac][ws], 1., 1., "B");
        }
        
        if(tgceffsector[ac][ws] &&
           tgceffsectornum[ac][ws] &&
           tgceffsectordenom[ac][ws] )
          tgceffsector[ac][ws]->Divide(tgceffsectornum[ac][ws], tgceffsectordenom[ac][ws], 1., 1., "B");
          
      }// ws
    }// ac
  }//endOfRun
 
  return StatusCode::SUCCESS;
}


StatusCode
TgcRawDataValAlg::fillEventInfo(){

  m_lumiblock = -1;
  m_event     = -1;
  m_BCID      = -1;

  //get Event Info
  //const DataHandle<EventInfo> evt;
  const xAOD::EventInfo* evt(0);
  StatusCode sc = (*m_activeStore)->retrieve(evt);
  if ( sc.isFailure() || evt==0) {
    m_log << MSG::ERROR <<" Cannot retrieve EventInfo " <<endreq;
    //tgceventsinlb->Fill( m_lumiblock );
    //tgceventsinbcid->Fill( m_BCID );
    return StatusCode::FAILURE;
  }

  m_lumiblock = evt->lumiBlock() ;
  m_event     = evt->eventNumber() ;
  m_BCID      = evt->bcid() ;

  //tgceventsinlb->Fill( m_lumiblock );
  //tgceventsinbcid->Fill( m_BCID );

  //m_log << MSG::INFO << "event " << m_event <<" lumiblock " << m_lumiblock << endreq;

  return StatusCode::SUCCESS;

}
