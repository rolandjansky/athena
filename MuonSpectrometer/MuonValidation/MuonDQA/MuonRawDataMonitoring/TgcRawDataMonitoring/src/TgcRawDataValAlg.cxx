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
  :ManagedMonitorToolBase( type, name, parent )
{
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
  ATH_MSG_INFO( " deleting TgcRawDataValAlg "  );
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode
TgcRawDataValAlg::initialize(){

  ATH_MSG_INFO( "in initializing TgcRawDataValAlg"  );

  ATH_CHECK( ManagedMonitorToolBase::initialize() );
  
  /*
  // Store Gate store
  sc = serviceLocator()->service("StoreGateSvc", _eventStore);
  if (sc != StatusCode::SUCCESS ) {
  m_log << MSG::ERROR << " Cannot get StoreGateSvc " << endmsg;
  return sc ;
  }
  */

  // Retrieve the Active Store
  ATH_CHECK(  serviceLocator()->service("ActiveStoreSvc", m_activeStore) );

  // Retrieve the MuonDetectorManager  
  ATH_CHECK(  detStore()->retrieve(m_muonMgr) );
  ATH_MSG_DEBUG( " Found the MuonDetectorManager from detector store. "  );

  ATH_CHECK(  detStore()->retrieve(m_tgcIdHelper,"TGCIDHELPER") );
  
  //histograms directory names
  m_generic_path_tgcmonitoring = "Muon/MuonRawDataMonitoring/TGC";

  //tgcchamberId();
  
  
  //offset
  setOffset();
  //set minimum and maximum of channel difference
  setChannelDifferenceRange();

  //do not monitor profile histograms
  //m_mon_profile=false;
  m_mon_profile=true;
  
  return StatusCode::SUCCESS;
} 


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode
TgcRawDataValAlg::bookHistogramsRecurrent(){
  ATH_MSG_DEBUG( "TGC RawData Monitoring Histograms being booked"  );

  if( newRunFlag() || newLowStatIntervalFlag() ){
    // not monitor these profiles at GM
    if( m_environment != AthenaMonManager::online ) {
      ATH_CHECK(  bookHistogramsLowStat() );
    }
    else
      for(int ac=0;ac<2;ac++){
        m_tgcwirestripcoinlowstat[ac] = 0;
      }
  }
     
  if(newLumiBlockFlag()){}   

  if(newRunFlag()) {      

    ATH_CHECK( bookHistogramsNumberOfHits() );
    ATH_CHECK( bookHistogramsProfile() );
    ATH_CHECK( bookHistogramsXYView() );
    ATH_CHECK( bookHistogramsEfficiency() );
    ATH_CHECK(  bookHistogramsSummary() );
    
    ATH_MSG_DEBUG( "INSIDE bookHistograms : " << m_tgcevents << m_generic_path_tgcmonitoring  );


    ATH_MSG_DEBUG( "have registered histograms for Number of Wire/Strip Hits"  );


  }//new run
  
  return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode
TgcRawDataValAlg::fillHistograms(){
  
  ATH_MSG_DEBUG( "TgcRawDataValAlg::TGC RawData Monitoring Histograms being filled"  );
  
  clearVectorsArrays();

  //fillEventInfo information
  ATH_CHECK( fillEventInfo() );
  
  /////////////////////////////////////
  // Get TGC Hit PRD Containers
  const Muon::TgcPrepDataContainer* tgc_previous_prd_container(0);
  const Muon::TgcPrepDataContainer* tgc_current_prd_container(0);
  const Muon::TgcPrepDataContainer* tgc_next_prd_container(0);
  
  // Previous
  ATH_CHECK( (*m_activeStore)->retrieve(tgc_previous_prd_container, m_tgcPrepDataPreviousContainerName) );
  ATH_MSG_DEBUG( "****** tgc previous prd container size() : " << tgc_previous_prd_container->size()  );
  
  // Current
  ATH_CHECK( (*m_activeStore)->retrieve(tgc_current_prd_container, m_tgcPrepDataContainerName) );
  ATH_MSG_DEBUG( "****** tgc current prd container size() : " << tgc_current_prd_container->size()  );
  
  // Next
  ATH_CHECK(  (*m_activeStore)->retrieve(tgc_next_prd_container, m_tgcPrepDataNextContainerName) );
  ATH_MSG_DEBUG( "****** tgc next prd container size() : " << tgc_next_prd_container->size()  );
  
  
  // Increment event counter
  m_nEvent++;
  ATH_MSG_DEBUG("event : " << m_nEvent  );
  
  
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
  ATH_CHECK( fillNumberOfHits() );
  
  // fill profile histograms
  ATH_CHECK( fillProfile() );

  // fill efficiency
  ATH_CHECK( fillEfficiency() );
  
  // fill XY view
  ATH_CHECK( fillXYView() );
  
  //vector<double> SLEta[2];//[ac]
  //vector<double> SLPhi[2];
  
  ATH_MSG_DEBUG( "********TGC event number : " << m_nEvent  );

  return StatusCode::SUCCESS;
 
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode
TgcRawDataValAlg::procHistograms(){
  ATH_MSG_DEBUG( "********Reached Last Event in TgcRawDataValAlg !!!"  );
  ATH_MSG_DEBUG( "TgcRawDataValAlg finalize()"  );
  if(endOfLumiBlockFlag()){
    //histogram division every LB only for Global Montioring
    if(m_environment==AthenaMonManager::online){
      // calculate occupancy map
      for(int ac=0;ac<2;ac++){
        for(int ws=0;ws<2;ws++){
          
          if( m_tgcprofilemap[ac][ws] && m_tgcoccupancymap[ac][ws] ){
            int nbinx=m_tgcprofilemap[ac][ws]->GetNbinsX();
            int nbiny=m_tgcprofilemap[ac][ws]->GetNbinsY();
            
            for(int binx=1;binx<=nbinx;binx++)
              for(int biny=1;biny<=nbiny;biny++){
                m_tgcoccupancymap[ac][ws]->SetBinContent( binx, biny, nWireStripMap( ws, binx, biny ) * m_nEvent );
              }
            m_tgcoccupancymap[ac][ws]->Divide( m_tgcprofilemap[ac][ws], m_tgcoccupancymap[ac][ws], 1., 1., "B");
          }

        }// ws
      }// ac


      // calculate efficiency histograms and map
      for(int ac=0;ac<2;ac++){
        if(m_tgceff[ac] &&
           m_tgceffnum[ac] &&
           m_tgceffdenom[ac] )
          m_tgceff[ac]->Divide(m_tgceffnum[ac], m_tgceffdenom[ac], 1., 1., "B");
      
        for(int ws=0;ws<2;ws++){
          if(m_tgceffmap[ac][ws] &&
             m_tgceffmapnum[ac][ws] &&
             m_tgceffmapdenom[ac][ws]){
            m_tgceffmap[ac][ws]->Divide(m_tgceffmapnum[ac][ws], m_tgceffmapdenom[ac][ws], 1., 1., "B");
          }
          
          for(int bc=0;bc<2;bc++){
            if(m_tgceffmapbc[ac][ws][bc] &&
               m_tgceffmapnumbc[ac][ws][bc] &&
               m_tgceffmapdenom[ac][ws]){
              m_tgceffmapbc[ac][ws][bc]->Divide(m_tgceffmapnumbc[ac][ws][bc], m_tgceffmapdenom[ac][ws], 1., 1., "B" );
            }
          }
          
          if(m_tgceffsector[ac][ws] &&
             m_tgceffsectornum[ac][ws] &&
             m_tgceffsectordenom[ac][ws] )
            m_tgceffsector[ac][ws]->Divide(m_tgceffsectornum[ac][ws], m_tgceffsectordenom[ac][ws], 1., 1., "B");
            
        }// ws
      }// ac
        
    }// isOnlineMonitoring
  }//endOfLumiBlock

  if(endOfRunFlag()){
    // calculate occupancy map
    for(int ac=0;ac<2;ac++){
      for(int ws=0;ws<2;ws++){
        
        if(m_tgcprofilemap[ac][ws] && m_tgcoccupancymap[ac][ws]){
          int nbinx=m_tgcprofilemap[ac][ws]->GetNbinsX();
          int nbiny=m_tgcprofilemap[ac][ws]->GetNbinsY();
          
          for(int binx=1;binx<=nbinx;binx++)
            for(int biny=1;biny<=nbiny;biny++){
              //m_tgcoccupancymap[ac][ws]->SetBinContent( binx, biny, nWireStripMap(ac, binx, biny ) * m_nEvent );//run1 default
              m_tgcoccupancymap[ac][ws]->SetBinContent( binx, biny, nWireStripMap(ws, binx, biny ) * m_nEvent );
            }
          m_tgcoccupancymap[ac][ws]->Divide( m_tgcprofilemap[ac][ws], m_tgcoccupancymap[ac][ws], 1., 1., "B");
        }

      }// ws
    }// ac

    // calculate efficiency histogram and map
    for(int ac=0;ac<2;ac++){
      if(m_tgceff[ac] &&
         m_tgceffnum[ac] &&
         m_tgceffdenom[ac] )
        m_tgceff[ac]->Divide(m_tgceffnum[ac], m_tgceffdenom[ac], 1., 1., "B");

      for(int ws=0;ws<2;ws++){
        if(m_tgceffmap[ac][ws] &&
           m_tgceffmapnum[ac][ws] &&
           m_tgceffmapdenom[ac][ws] ){
          m_tgceffmap[ac][ws]->Divide(m_tgceffmapnum[ac][ws], m_tgceffmapdenom[ac][ws], 1., 1., "B");
        }
        
        if(m_tgceffsector[ac][ws] &&
           m_tgceffsectornum[ac][ws] &&
           m_tgceffsectordenom[ac][ws] )
          m_tgceffsector[ac][ws]->Divide(m_tgceffsectornum[ac][ws], m_tgceffsectordenom[ac][ws], 1., 1., "B");
          
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
  ATH_CHECK( (*m_activeStore)->retrieve(evt) );

  m_lumiblock = evt->lumiBlock() ;
  m_event     = evt->eventNumber() ;
  m_BCID      = evt->bcid() ;

  //tgceventsinlb->Fill( m_lumiblock );
  //tgceventsinbcid->Fill( m_BCID );

  return StatusCode::SUCCESS;

}
