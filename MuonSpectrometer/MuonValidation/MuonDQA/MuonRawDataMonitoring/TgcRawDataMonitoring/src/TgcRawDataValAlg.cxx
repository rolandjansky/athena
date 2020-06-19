/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

// GeoModel
#include "MuonReadoutGeometry/TgcReadoutParams.h"

// MuonRDO
#include "MuonRDO/TgcRdo.h"
#include "MuonRDO/TgcRdoIdHash.h"
#include "MuonRDO/TgcRdoContainer.h"

#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonDQAUtils/MuonCosmicSetup.h"
 
#include "TgcRawDataMonitoring/TgcRawDataValAlg.h"

#include <inttypes.h> 
#include <sstream>
#include <math.h>

/////////////////////////////////////////////////////////////////////////////

TgcRawDataValAlg::TgcRawDataValAlg( const std::string & type, const std::string & name, const IInterface* parent )
  :ManagedMonitorToolBase( type, name, parent )
{
  // Declare the properties 
  m_nEvent=0;
  m_numberingVersion=0; 
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode
TgcRawDataValAlg::initialize(){
  ATH_MSG_INFO( "in initializing TgcRawDataValAlg"  );
  ATH_CHECK(ManagedMonitorToolBase::initialize());
  ATH_CHECK(m_idHelperSvc.retrieve());
  // MuonDetectorManager from the conditions store
  ATH_CHECK(m_DetectorManagerKey.initialize());
  //histograms directory names
  m_generic_path_tgcmonitoring = "Muon/MuonRawDataMonitoring/TGC";

  //offset
  setOffset();
  //set minimum and maximum of channel difference
  setChannelDifferenceRange();

  //do not monitor profile histograms
  //m_mon_profile=false;
  m_mon_profile=true;

  ATH_CHECK(m_tgcPrepDataContainerName.initialize());
  ATH_CHECK(m_outputCoinCollectionLocation.initialize());
  ATH_CHECK(m_eventInfo.initialize());
  
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
  SG::ReadHandle<Muon::TgcPrepDataContainer> tgc_prd_container(m_tgcPrepDataContainerName);
  ATH_MSG_DEBUG( "****** tgc prd container size() : " << tgc_prd_container->size()  );

  // Increment event counter
  m_nEvent++;
  ATH_MSG_DEBUG("event : " << m_nEvent  );
  
  
  /////////////////////////////////////
  // Get Data from TGC Containers
  clearVectorsArrays();
  // fill vectors and arrays from TgcPrepData
  readTgcPrepDataContainer(tgc_prd_container.cptr());
  
  
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
  SG::ReadHandle<xAOD::EventInfo> evt(m_eventInfo);

  m_lumiblock = evt->lumiBlock() ;
  m_event     = evt->eventNumber() ;
  m_BCID      = evt->bcid() ;

  //tgceventsinlb->Fill( m_lumiblock );
  //tgceventsinbcid->Fill( m_BCID );

  return StatusCode::SUCCESS;

}
