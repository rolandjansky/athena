/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Package : TGC StandaloneTracksMon
// Authors:  A. Ishikawa(Kobe)
// Jun. 2008
//
// DESCRIPTION:
// Subject: correlation btw MDT hits vs TGC track (moved from MuonRawDataMon Feb.2009)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
 
#include "MuonRDO/TgcRdo.h"
#include "MuonRDO/TgcRdoContainer.h"
#include "MuonRDO/TgcRdoIdHash.h"

// DetDescr
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/TgcReadoutParams.h"

#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonDQAUtils/MuonCosmicSetup.h"
#include "MuonDQAUtils/MuonDQAHistMap.h" 
 
#include "Identifier/Identifier.h"

//mdt stuff
#include "MuonCalibIdentifier/MuonFixedId.h" 
#include "MuonTrackMonitoring/TGCStandaloneTracksMon.h"
#include "AthenaMonitoring/AthenaMonManager.h"

#include <TH1F.h>
#include <TH2F.h>
#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TF1.h>
#include <inttypes.h> 

#include <sstream>
#include <algorithm>

using namespace std;

static const int maxColl =   1200;
static const int maxPRD  =   50000;
static const int maxClus =   1000;
 
/////////////////////////////////////////////////////////////////////////////

TGCStandaloneTracksMon::TGCStandaloneTracksMon( const std::string & type, const std::string & name, const IInterface* parent )
  :ManagedMonitorToolBase( type, name, parent ),
   m_trigDecTool(""),
   m_log( msgSvc(), name ),
   m_debuglevel(false)
{
  // Declare the properties 
  //declareProperty("CheckCabling",     m_checkCabling=false);
  declareProperty("MdtAdcCut",        m_MdtAdcCut=50);
  declareProperty("MdtTdcMinCut",        m_MdtTdcMinCut=400);
  declareProperty("MdtTdcMaxCut",        m_MdtTdcMaxCut=1500);
  declareProperty("TgcPrepDataContainer", m_tgcPrepDataContainerName = "TGC_Measurements");
  declareProperty("OutputCoinCollection", m_outputCoinCollectionLocation = "TrigT1CoinDataCollection" );
  declareProperty("MdtPrepDataContainer", m_key_mdt="MDT_DriftCircles");
  //for trigger aware analysis
  declareProperty("TriggerAware", m_triggerAware);
  declareProperty("TriggerDecisionTool", m_trigDecTool);
  declareProperty("Muon_Trigger_Items",  m_muon_triggers);
  declareProperty("UseTriggerVector",    m_useTrigger);     
}

TGCStandaloneTracksMon::~TGCStandaloneTracksMon()
{
  m_log << MSG::INFO << " deleting TGCStandaloneTracksMon " << endreq;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TGCStandaloneTracksMon::initialize()
{
  // init message stream
  m_log.setLevel(outputLevel());                // individual outputlevel not known before initialise
  m_debuglevel = (m_log.level() <= MSG::DEBUG); // save if threshold for debug printout reached
  
  m_log << MSG::INFO << "in initializing TGCStandaloneTracksMon" << endreq;

  if(m_debuglevel) m_log << MSG::DEBUG <<"m_triggerAware "<<m_triggerAware<<endreq;

  StatusCode sc;

  // Store Gate store
  sc = serviceLocator()->service("StoreGateSvc", m_eventStore);
  if (sc != StatusCode::SUCCESS ) {
    m_log << MSG::ERROR << " Cannot get StoreGateSvc " << endreq;
    return sc ;
  }
  // retrieve the active store
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
    m_log << MSG::FATAL << "Cannot get MuonDetectorManager from detector store" << endreq;
    return StatusCode::FAILURE;
  }  
  else {
    if (m_debuglevel) m_log << MSG::DEBUG << " Found the MuonDetectorManager from detector store. " << endreq;
  }

  //get TGC ID Helper
  sc = detStore->retrieve(m_tgcIdHelper,"TGCIDHELPER");
  if (sc.isFailure())
    {
      m_log << MSG::ERROR << "Can't retrieve TgcIdHelper" << endreq;
      return sc;
    }	 
  
  //get MDT ID Helper
  sc = detStore->retrieve(m_mdtIdHelper,"MDTIDHELPER");
  if (sc.isFailure()) {
    m_log << MSG::FATAL << "Cannot get MdtIdHelper" << endreq;
    return StatusCode::FAILURE;
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

  //std::vector<std::string> hardware_name_list                  ;
  //std::vector<std::string> layer_name_list                     ;
  //std::vector<std::string> layervslayer_name_list              ;
  // std::vector<std::string> layerPhivsEta_name_list             ;
  //std::vector<std::string> layerPhivsEtaSector_name_list       ;
  //hardware_name_list.push_back("XXX");
  
  ManagedMonitorToolBase::initialize().ignore();  //  Ignore the checking code;
 
  //MDT z position
  //Name MultiLayer TubeLayer z
  //17 1 1 14142.5
  //17 1 2 14168.5
  //17 1 3 14194.5
  //17 2 1 14394.6
  //17 2 2 14420.6
  //17 2 3 14446.6
  //18 1 1 13726.5
  //18 1 2 13752.5
  //18 1 3 13778.5
  //18 2 1 13978.6
  //18 2 2 14004.6
  //18 2 3 14030.6
  
  MDTZpos[0][0][0]=14142.5;
  MDTZpos[0][0][1]=14168.5;
  MDTZpos[0][0][2]=14194.5;
  MDTZpos[0][1][0]=14394.6;
  MDTZpos[0][1][1]=14420.6;
  MDTZpos[0][1][2]=14446.6;

  MDTZpos[1][0][0]=13726.5;
  MDTZpos[1][0][1]=13752.5;
  MDTZpos[1][0][2]=13778.5;
  MDTZpos[1][1][0]=13978.6;
  MDTZpos[1][1][1]=14004.6;
  MDTZpos[1][1][2]=14030.6;



  return StatusCode::SUCCESS;
}


/*----------------------------------------------------------------------------------*/
StatusCode TGCStandaloneTracksMon::bookHistograms()
/*----------------------------------------------------------------------------------*/
{
  if (m_debuglevel) m_log << MSG::DEBUG << "TGC Standalone Trk Monitoring Histograms being booked" << endreq;
 
  StatusCode sc = StatusCode::SUCCESS; 
     
  //declare a group of histograms
  std::string m_generic_path_mdtvstgclv1;
  if(m_triggerAware){
    m_generic_path_mdtvstgclv1  = "Muon/MuonTrackMonitoring/Trigger/TGCStandAloneTrackMon";
  }else{
    m_generic_path_mdtvstgclv1  = "Muon/MuonTrackMonitoring/NoTrigger/TGCStandAloneTrackMon";
  }
  MonGroup mdtvstgclv1_expert( this, m_generic_path_mdtvstgclv1+"/Global", run, ATTRIB_MANAGED );
  MonGroup mdtvstgclv1_expert_a( this, m_generic_path_mdtvstgclv1+"/TGCEA",run, ATTRIB_MANAGED );
  MonGroup mdtvstgclv1_expert_c( this, m_generic_path_mdtvstgclv1+"/TGCEC", run, ATTRIB_MANAGED );

  MonGroup mdtvstgclv1_track_a( this, m_generic_path_mdtvstgclv1+"/TGCEA/Track", run, ATTRIB_MANAGED );
  MonGroup mdtvstgclv1_track_c( this, m_generic_path_mdtvstgclv1+"/TGCEC/Track", run, ATTRIB_MANAGED );
 
  if(newEventsBlock){}
  if(newLumiBlock){}
  if(newRun){         
    if (m_debuglevel) m_log << MSG::DEBUG << " Before TriggerInfo Histogram, UseTrigger? " << m_useTrigger << endreq;
    if (m_debuglevel) m_log << MSG::DEBUG << " Before TriggerInfo Histogram, MuonTriggers size: " << m_muon_triggers.size() << endreq; 
    if (m_debuglevel) m_log << MSG::INFO << "TGC Standalone Trk Monitoring : begin of run" << endreq;

    std::stringstream sst;
    std::string side[2]={"A","C"};
    std::string ptcut="With_Pt_Cut_";

    // these histograms are only for debug purpose
    // at Tier0 not processed 
    if(m_debuglevel){

      //MDT ADC
      std::string s_mdtvstgclv1_mdt_adc = "MDT_ADC";
      mdtvstgclv1_mdt_adc =new TH1F(s_mdtvstgclv1_mdt_adc.c_str(), s_mdtvstgclv1_mdt_adc.c_str(), 256, 0, 512);
      sc=mdtvstgclv1_expert.regHist(mdtvstgclv1_mdt_adc) ;  
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_mdt_adc Failed to register histogram " << endreq;       
	  return sc;
        }
      mdtvstgclv1_mdt_adc->SetFillColor(5);
      mdtvstgclv1_mdt_adc->GetXaxis()->SetTitle("MDT ADC count");
      mdtvstgclv1_mdt_adc->GetYaxis()->SetTitle("Counts");
    
      //MDT TDC
      std::string s_mdtvstgclv1_mdt_tdc = "MDT_TDC";
      mdtvstgclv1_mdt_tdc =new TH1F(s_mdtvstgclv1_mdt_tdc.c_str(), s_mdtvstgclv1_mdt_tdc.c_str(), 400, 0, 3200);
      sc=mdtvstgclv1_expert.regHist(mdtvstgclv1_mdt_tdc) ;  
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_mdt_tdc Failed to register histogram " << endreq;       
	  return sc;
        }
      mdtvstgclv1_mdt_tdc->SetFillColor(5);
      mdtvstgclv1_mdt_tdc->GetXaxis()->SetTitle("MDT TDC count");
      mdtvstgclv1_mdt_tdc->GetYaxis()->SetTitle("Counts");
    
      //MDT TDC with ADC cut
      std::string s_mdtvstgclv1_mdt_tdc_adc_cut = "MDT_TDC_With_ADC_Cut_";
      mdtvstgclv1_mdt_tdc_adc_cut =new TH1F(s_mdtvstgclv1_mdt_tdc_adc_cut.c_str(), s_mdtvstgclv1_mdt_tdc_adc_cut.c_str(), 400, 0, 3200);
      sc=mdtvstgclv1_expert.regHist(mdtvstgclv1_mdt_tdc_adc_cut) ;  
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_mdt_tdc_adc_cut Failed to register histogram " << endreq;       
	  return sc;
        }
      mdtvstgclv1_mdt_tdc_adc_cut->SetFillColor(5);
      mdtvstgclv1_mdt_tdc_adc_cut->GetXaxis()->SetTitle("MDT TDC count");
      mdtvstgclv1_mdt_tdc_adc_cut->GetYaxis()->SetTitle("Counts");
    
      //MDT TDC vs ADC
      std::string s_mdtvstgclv1_mdt_tdc_vs_adc = "MDT_TDC_Vs_ADC";
      mdtvstgclv1_mdt_tdc_vs_adc =new TH2F(s_mdtvstgclv1_mdt_tdc_vs_adc.c_str(), s_mdtvstgclv1_mdt_tdc_vs_adc.c_str(), 128, 0, 4096, 128, 0,512);
      sc=mdtvstgclv1_expert.regHist(mdtvstgclv1_mdt_tdc_vs_adc) ;  
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_mdt_tdc_vs_adc Failed to register histogram " << endreq;       
	  return sc;
        }
      mdtvstgclv1_mdt_tdc_vs_adc->SetFillColor(5);
      mdtvstgclv1_mdt_tdc_vs_adc->GetXaxis()->SetTitle("MDT TDC count");
      mdtvstgclv1_mdt_tdc_vs_adc->GetYaxis()->SetTitle("MDT ADC count");

      for(int i=0;i<2;i++){
        sst<<"MDT_ADC_Vs_MDT_Sector_";
        sst<<side[i];
        m_log<<MSG::DEBUG<<"MDT_ADC_Vs_MDT_Sector "<<i<<" "<<sst.str().c_str()<<std::endl;
        mdtvstgclv1_adcsector[i] =new TH2F(sst.str().c_str(),sst.str().c_str(), 16, 1., 17., 128, 0, 512); 
        if(i==0){
          sc=mdtvstgclv1_expert_a.regHist(mdtvstgclv1_adcsector[i]) ;  
        }else{
          sc=mdtvstgclv1_expert_c.regHist(mdtvstgclv1_adcsector[i]) ;  
        }
        if(sc.isFailure())
          { m_log << MSG::FATAL << "mdtvstgclv1_adcsector["<<i<<"] Failed to register histogram " << endreq;       
	    return sc;
          }  		  
        //m_log<<MSG::INFO <<"RoI_Eta_Vs_Phi_"<<side<<" has been registered"<<endreq;
        //tgclv1roietavsphi[i]->SetFillColor(5);
        mdtvstgclv1_adcsector[i]->GetXaxis()->SetTitle("MDT Sector");
        mdtvstgclv1_adcsector[i]->GetYaxis()->SetTitle("MDT ADC");
        sst.str("");
      }
    }
    //end of debug histograms

    for(int i=0;i<2;i++){
      //Number of wire around RoI to be used for track finding
      sst<<"TGC_Number_Of_Wire_Around_RoI_";
      sst<<side[i];
      mdtvstgclv1_nwire_around_roi[i] =new TH1F(sst.str().c_str(),sst.str().c_str(), 50, 0., 50.);
      if(i==0){
        sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_nwire_around_roi[i]) ;  
      }else{
        sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_nwire_around_roi[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_nwire_around_roi Failed to register histogram " << endreq;       
	  return sc;
	}
      mdtvstgclv1_nwire_around_roi[i]->SetFillColor(5);
      mdtvstgclv1_nwire_around_roi[i]->GetXaxis()->SetTitle("nwire");
      mdtvstgclv1_nwire_around_roi[i]->GetYaxis()->SetTitle("Counts");
      sst.str("");

      //Number of wire around RoI to be used for track finding with pt cut
      sst<<"TGC_Number_Of_Wire_Around_RoI"<<ptcut;
      sst<<side[i];
      mdtvstgclv1_nwire_around_roi_ptcut[i] =new TH1F(sst.str().c_str(),sst.str().c_str(), 50, 0., 50.);
      if(i==0){
        sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_nwire_around_roi_ptcut[i]) ;  
      }else{
        sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_nwire_around_roi_ptcut[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_nwire_around_roi Failed to register histogram " << endreq;       
	  return sc;
	}
      mdtvstgclv1_nwire_around_roi_ptcut[i]->SetFillColor(5);
      mdtvstgclv1_nwire_around_roi_ptcut[i]->GetXaxis()->SetTitle("nwire");
      mdtvstgclv1_nwire_around_roi_ptcut[i]->GetYaxis()->SetTitle("Counts");
      sst.str("");

      //TGC Track chi2/ndf
      sst<<"TGC_Track_Chi2_Over_NDF_";
      sst<<side[i];
      mdtvstgclv1_chi2overndf[i] =new TH1F(sst.str().c_str(),sst.str().c_str(), 100, 0., 10.);
      if(i==0){
        sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_chi2overndf[i]) ;  
      }else{
        sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_chi2overndf[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_chi2overndf Failed to register histogram " << endreq;       
	  return sc;
	}
      mdtvstgclv1_chi2overndf[i]->SetFillColor(5);
      mdtvstgclv1_chi2overndf[i]->GetXaxis()->SetTitle("chi2/ndf");
      mdtvstgclv1_chi2overndf[i]->GetYaxis()->SetTitle("Counts");
      sst.str("");

      //TGC Track chi2/ndf 3station track
      sst<<"TGC_Track_Chi2_Over_NDF_3Station_";
      sst<<side[i];
      mdtvstgclv1_chi2overndf_3st[i] =new TH1F(sst.str().c_str(),sst.str().c_str(), 100, 0., 10.);
      if(i==0){
        sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_chi2overndf_3st[i]) ;  
      }else{
        sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_chi2overndf_3st[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_chi2overndf Failed to register histogram " << endreq;       
	  return sc;
	}
      mdtvstgclv1_chi2overndf_3st[i]->SetFillColor(5);
      mdtvstgclv1_chi2overndf_3st[i]->GetXaxis()->SetTitle("chi2/ndf");
      mdtvstgclv1_chi2overndf_3st[i]->GetYaxis()->SetTitle("Counts");
      sst.str("");

      //TGC Track chi2/ndf with ptcut
      sst<<"TGC_Track_Chi2_Over_NDF_"<<ptcut;
      sst<<side[i];
      mdtvstgclv1_chi2overndf_ptcut[i] =new TH1F(sst.str().c_str(),sst.str().c_str(), 100, 0., 10.);
      if(i==0){
        sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_chi2overndf_ptcut[i]) ;  
      }else{
        sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_chi2overndf_ptcut[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_chi2overndf Failed to register histogram " << endreq;       
	  return sc;
	}
      mdtvstgclv1_chi2overndf_ptcut[i]->SetFillColor(5);
      mdtvstgclv1_chi2overndf_ptcut[i]->GetXaxis()->SetTitle("chi2/ndf");
      mdtvstgclv1_chi2overndf_ptcut[i]->GetYaxis()->SetTitle("Counts");
      sst.str("");

      //TGC Track chi2/ndf 3station track with ptcut
      sst<<"TGC_Track_Chi2_Over_NDF_3Station_"<<ptcut;
      sst<<side[i];
      mdtvstgclv1_chi2overndf_3st_ptcut[i] =new TH1F(sst.str().c_str(),sst.str().c_str(), 100, 0., 10.);
      if(i==0){
        sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_chi2overndf_3st_ptcut[i]) ;  
      }else{
        sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_chi2overndf_3st_ptcut[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_chi2overndf Failed to register histogram " << endreq;       
	  return sc;
	}
      mdtvstgclv1_chi2overndf_3st_ptcut[i]->SetFillColor(5);
      mdtvstgclv1_chi2overndf_3st_ptcut[i]->GetXaxis()->SetTitle("chi2/ndf");
      mdtvstgclv1_chi2overndf_3st_ptcut[i]->GetYaxis()->SetTitle("Counts");
      sst.str("");

      //TGC Track ndf
      sst<<"TGC_Track_Nhit_";
      sst<<side[i];
      mdtvstgclv1_nhit[i] =new TH1F(sst.str().c_str(),sst.str().c_str(), 15, 0., 15.);
      if(i==0){
        sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_nhit[i]) ;  
      }else{
        sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_nhit[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_nhit Failed to register histogram " << endreq;       
	  return sc;
	}
      mdtvstgclv1_nhit[i]->SetFillColor(5);
      mdtvstgclv1_nhit[i]->GetXaxis()->SetTitle("nhitf");
      mdtvstgclv1_nhit[i]->GetYaxis()->SetTitle("Counts");
      sst.str("");

      //TGC Track ndf 3station track
      sst<<"TGC_Track_Nhit_3Station_";
      sst<<side[i];
      mdtvstgclv1_nhit_3st[i] =new TH1F(sst.str().c_str(),sst.str().c_str(), 15, 0., 15.);
      if(i==0){
        sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_nhit_3st[i]) ;  
      }else{
        sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_nhit_3st[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_nhit Failed to register histogram " << endreq;       
	  return sc;
	}
      mdtvstgclv1_nhit_3st[i]->SetFillColor(5);
      mdtvstgclv1_nhit_3st[i]->GetXaxis()->SetTitle("nhit");
      mdtvstgclv1_nhit_3st[i]->GetYaxis()->SetTitle("Counts");
      sst.str("");

      //TGC Track ndf with ptcut
      sst<<"TGC_Track_Nhit_"<<ptcut;
      sst<<side[i];
      mdtvstgclv1_nhit_ptcut[i] =new TH1F(sst.str().c_str(),sst.str().c_str(), 15, 0., 15.);
      if(i==0){
        sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_nhit_ptcut[i]) ;  
      }else{
        sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_nhit_ptcut[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_nhit Failed to register histogram " << endreq;       
	  return sc;
	}
      mdtvstgclv1_nhit_ptcut[i]->SetFillColor(5);
      mdtvstgclv1_nhit_ptcut[i]->GetXaxis()->SetTitle("nhit");
      mdtvstgclv1_nhit_ptcut[i]->GetYaxis()->SetTitle("Counts");
      sst.str("");

      //TGC Track ndf 3station track with ptcut
      sst<<"TGC_Track_Nhit_3Station_"<<ptcut;
      sst<<side[i];
      mdtvstgclv1_nhit_3st_ptcut[i] =new TH1F(sst.str().c_str(),sst.str().c_str(), 15, 0., 15.);
      if(i==0){
        sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_nhit_3st_ptcut[i]) ;  
      }else{
        sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_nhit_3st_ptcut[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_nhit Failed to register histogram " << endreq;       
	  return sc;
	}
      mdtvstgclv1_nhit_3st_ptcut[i]->SetFillColor(5);
      mdtvstgclv1_nhit_3st_ptcut[i]->GetXaxis()->SetTitle("nhit");
      mdtvstgclv1_nhit_3st_ptcut[i]->GetYaxis()->SetTitle("Counts");
      sst.str("");

      //TGC Track pull distribution
      sst<<"TGC_Track_Pull_";
      sst<<side[i];
      mdtvstgclv1_pull[i] =new TH1F(sst.str().c_str(),sst.str().c_str(), 100, -5., 5.);
      if(i==0){
        sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_pull[i]) ;  
      }else{
        sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_pull[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_pull Failed to register histogram " << endreq;       
	  return sc;
	}
      mdtvstgclv1_pull[i]->SetFillColor(5);
      mdtvstgclv1_pull[i]->GetXaxis()->SetTitle("pull");
      mdtvstgclv1_pull[i]->GetYaxis()->SetTitle("Counts");
      sst.str("");

      //TGC Track pull distribution for 3 station track
      sst<<"TGC_Track_Pull_3Station_Track_";
      sst<<side[i];
      mdtvstgclv1_pull3st[i] =new TH1F(sst.str().c_str(),sst.str().c_str(), 100, -5., 5.);
      if(i==0){
        sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_pull3st[i]) ;  
      }else{
        sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_pull3st[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_pull3st Failed to register histogram " << endreq;       
	  return sc;
	}
      mdtvstgclv1_pull3st[i]->SetFillColor(5);
      mdtvstgclv1_pull3st[i]->GetXaxis()->SetTitle("pull");
      mdtvstgclv1_pull3st[i]->GetYaxis()->SetTitle("Counts");
      sst.str("");

      //TGC Track pull distribution with pt cut
      sst<<"TGC_Track_Pull_"<<ptcut;
      sst<<side[i];
      mdtvstgclv1_pull_ptcut[i] =new TH1F(sst.str().c_str(),sst.str().c_str(), 100, -5., 5.);
      if(i==0){
        sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_pull_ptcut[i]) ;  
      }else{
        sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_pull_ptcut[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_pull Failed to register histogram " << endreq;       
	  return sc;
	}
      mdtvstgclv1_pull_ptcut[i]->SetFillColor(5);
      mdtvstgclv1_pull_ptcut[i]->GetXaxis()->SetTitle("pull");
      mdtvstgclv1_pull_ptcut[i]->GetYaxis()->SetTitle("Counts");
      sst.str("");

      //TGC Track pull distribution for 3 station track with pt cut
      sst<<"TGC_Track_Pull_3Station_Track_"<<ptcut;
      sst<<side[i];
      mdtvstgclv1_pull3st_ptcut[i] =new TH1F(sst.str().c_str(),sst.str().c_str(), 100, -5., 5.);
      if(i==0){
        sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_pull3st_ptcut[i]) ;  
      }else{
        sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_pull3st_ptcut[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_pull3st_ptcut Failed to register histogram " << endreq;       
	  return sc;
	}
      mdtvstgclv1_pull3st_ptcut[i]->SetFillColor(5);
      mdtvstgclv1_pull3st_ptcut[i]->GetXaxis()->SetTitle("pull");
      mdtvstgclv1_pull3st_ptcut[i]->GetYaxis()->SetTitle("Counts");
      sst.str("");

      for(int layer=0;layer<3;layer++){
        //TGC Track delta eta btw hits and RoI distribution
        sst<<"TGC_Track_Delta_Eta_";
        sst<<side[i];
        sst<<"_TGC"<<layer+1;
        mdtvstgclv1_deltaeta[i][layer] =new TH1F(sst.str().c_str(),sst.str().c_str(), 100, -0.2, 0.2);
        if(i==0){
          sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_deltaeta[i][layer]) ;  
        }else{
          sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_deltaeta[i][layer]) ;  
        }
        if(sc.isFailure())
          { m_log << MSG::FATAL << "mdtvstgclv1_deltaeta Failed to register histogram " << endreq;       
	    return sc;
          }
        mdtvstgclv1_deltaeta[i][layer]->SetFillColor(5);
        mdtvstgclv1_deltaeta[i][layer]->GetXaxis()->SetTitle("deltaeta");
        mdtvstgclv1_deltaeta[i][layer]->GetYaxis()->SetTitle("Counts");
        sst.str("");

        //TGC Track delta eta btw hits and RoI distribution
        sst<<"TGC_Track_Delta_Eta_3Station_Track_";
        sst<<side[i];
        sst<<"_TGC"<<layer+1;
        mdtvstgclv1_deltaeta3st[i][layer] =new TH1F(sst.str().c_str(),sst.str().c_str(), 100, -0.2, 0.2);
        if(i==0){
          sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_deltaeta3st[i][layer]) ;  
        }else{
          sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_deltaeta3st[i][layer]) ;  
        }
        if(sc.isFailure())
          { m_log << MSG::FATAL << "mdtvstgclv1_deltaeta3st Failed to register histogram " << endreq;       
	    return sc;
          }
        mdtvstgclv1_deltaeta3st[i][layer]->SetFillColor(5);
        mdtvstgclv1_deltaeta3st[i][layer]->GetXaxis()->SetTitle("deltaeta");
        mdtvstgclv1_deltaeta3st[i][layer]->GetYaxis()->SetTitle("Counts");
        sst.str("");

        //TGC Track delta eta btw hits and RoI distribution with pt cut
        sst<<"TGC_Track_Delta_Eta_"<<ptcut;
        sst<<side[i];
        sst<<"_TGC"<<layer+1;
        mdtvstgclv1_deltaeta_ptcut[i][layer] =new TH1F(sst.str().c_str(),sst.str().c_str(), 100, -0.2, 0.2);
        if(i==0){
          sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_deltaeta_ptcut[i][layer]) ;  
        }else{
          sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_deltaeta_ptcut[i][layer]) ;  
        }
        if(sc.isFailure())
          { m_log << MSG::FATAL << "mdtvstgclv1_deltaeta_ptcut Failed to register histogram " << endreq;       
	    return sc;
          }
        mdtvstgclv1_deltaeta_ptcut[i][layer]->SetFillColor(5);
        mdtvstgclv1_deltaeta_ptcut[i][layer]->GetXaxis()->SetTitle("deltaeta");
        mdtvstgclv1_deltaeta_ptcut[i][layer]->GetYaxis()->SetTitle("Counts");
        sst.str("");

        //TGC Track delta eta btw hits and RoI distribution with pt cut
        sst<<"TGC_Track_Delta_Eta_3Station_Track_"<<ptcut;
        sst<<side[i];
        sst<<"_TGC"<<layer+1;
        mdtvstgclv1_deltaeta3st_ptcut[i][layer] =new TH1F(sst.str().c_str(),sst.str().c_str(), 100, -0.2, 0.2);
        if(i==0){
          sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_deltaeta3st_ptcut[i][layer]) ;  
        }else{
          sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_deltaeta3st_ptcut[i][layer]) ;  
        }
        if(sc.isFailure())
          { m_log << MSG::FATAL << "mdtvstgclv1_deltaeta3st_ptcut Failed to register histogram " << endreq;       
	    return sc;
          }
        mdtvstgclv1_deltaeta3st_ptcut[i][layer]->SetFillColor(5);
        mdtvstgclv1_deltaeta3st_ptcut[i][layer]->GetXaxis()->SetTitle("deltaeta");
        mdtvstgclv1_deltaeta3st_ptcut[i][layer]->GetYaxis()->SetTitle("Counts");
        sst.str("");
      }

      //TGC Track a (r = a * z + b)
      sst<<"TGC_Track_A_Parameter_";
      sst<<side[i];
      mdtvstgclv1_a[i] =new TH1F(sst.str().c_str(),sst.str().c_str(), 100, -3., 3.);
      if(i==0){
        sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_a[i]) ;  
      }else{
        sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_a[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_a Failed to register histogram " << endreq;       
	  return sc;
	}
      mdtvstgclv1_a[i]->SetFillColor(5);
      mdtvstgclv1_a[i]->GetXaxis()->SetTitle("a");
      mdtvstgclv1_a[i]->GetYaxis()->SetTitle("Counts");
      sst.str("");

      //TGC Track b
      sst<<"TGC_Track_B_Parameter_";
      sst<<side[i];
      mdtvstgclv1_b[i] =new TH1F(sst.str().c_str(),sst.str().c_str(), 100, -30000., 30000.);
      if(i==0){
        sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_b[i]) ;  
      }else{
        sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_b[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_b Failed to register histogram " << endreq;       
	  return sc;
	}
      mdtvstgclv1_b[i]->SetFillColor(5);
      mdtvstgclv1_b[i]->GetXaxis()->SetTitle("b");
      mdtvstgclv1_b[i]->GetYaxis()->SetTitle("Counts");
      sst.str("");


      //TGC Track a 3 stations used(r = a * z + b)
      sst<<"TGC_Track_A_Parameter_3Station_";
      sst<<side[i];
      mdtvstgclv1_a_3st[i] =new TH1F(sst.str().c_str(),sst.str().c_str(), 100, -3., 3.);
      if(i==0){
        sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_a_3st[i]) ;  
      }else{
        sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_a_3st[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_a_3st Failed to register histogram " << endreq;       
	  return sc;
	}
      mdtvstgclv1_a_3st[i]->SetFillColor(5);
      mdtvstgclv1_a_3st[i]->GetXaxis()->SetTitle("a");
      mdtvstgclv1_a_3st[i]->GetYaxis()->SetTitle("Counts");
      sst.str("");

      //TGC Track b 3 stations used
      sst<<"TGC_Track_B_Parameter_3Station_";
      sst<<side[i];
      mdtvstgclv1_b_3st[i] =new TH1F(sst.str().c_str(),sst.str().c_str(), 100, -30000., 30000.);
      if(i==0){
        sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_b_3st[i]) ;  
      }else{
        sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_b_3st[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_b_3st Failed to register histogram " << endreq;       
	  return sc;
	}
      mdtvstgclv1_b_3st[i]->SetFillColor(5);
      mdtvstgclv1_b_3st[i]->GetXaxis()->SetTitle("b");
      mdtvstgclv1_b_3st[i]->GetYaxis()->SetTitle("Counts");
      sst.str("");



      //TGC Track a (r = a * z + b)
      sst<<"TGC_Track_A_Parameter"<<ptcut;
      sst<<side[i];
      mdtvstgclv1_a_ptcut[i] =new TH1F(sst.str().c_str(),sst.str().c_str(), 100, -3., 3.);
      if(i==0){
        sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_a_ptcut[i]) ;  
      }else{
        sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_a_ptcut[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_a_ptcut Failed to register histogram " << endreq;       
	  return sc;
	}
      mdtvstgclv1_a_ptcut[i]->SetFillColor(5);
      mdtvstgclv1_a_ptcut[i]->GetXaxis()->SetTitle("a");
      mdtvstgclv1_a_ptcut[i]->GetYaxis()->SetTitle("Counts");
      sst.str("");

      //TGC Track b
      sst<<"TGC_Track_B_Parameter"<<ptcut;
      sst<<side[i];
      mdtvstgclv1_b_ptcut[i] =new TH1F(sst.str().c_str(),sst.str().c_str(), 100, -30000., 30000.);
      if(i==0){
        sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_b_ptcut[i]) ;  
      }else{
        sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_b_ptcut[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_b_ptcut Failed to register histogram " << endreq;       
	  return sc;
	}
      mdtvstgclv1_b_ptcut[i]->SetFillColor(5);
      mdtvstgclv1_b_ptcut[i]->GetXaxis()->SetTitle("b");
      mdtvstgclv1_b_ptcut[i]->GetYaxis()->SetTitle("Counts");
      sst.str("");


      //TGC Track a 3 stations used(r = a * z + b)
      sst<<"TGC_Track_A_Parameter_3Station"<<ptcut;
      sst<<side[i];
      mdtvstgclv1_a_3st_ptcut[i] =new TH1F(sst.str().c_str(),sst.str().c_str(), 100, -3., 3.);
      if(i==0){
        sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_a_3st_ptcut[i]) ;  
      }else{
        sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_a_3st_ptcut[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_a_3st_ptcut Failed to register histogram " << endreq;       
	  return sc;
	}
      mdtvstgclv1_a_3st_ptcut[i]->SetFillColor(5);
      mdtvstgclv1_a_3st_ptcut[i]->GetXaxis()->SetTitle("a");
      mdtvstgclv1_a_3st_ptcut[i]->GetYaxis()->SetTitle("Counts");
      sst.str("");

      //TGC Track b 3 stations used
      sst<<"TGC_Track_B_Parameter_3Station"<<ptcut;
      sst<<side[i];
      mdtvstgclv1_b_3st_ptcut[i] =new TH1F(sst.str().c_str(),sst.str().c_str(), 100, -30000., 30000.);
      if(i==0){
        sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_b_3st_ptcut[i]) ;  
      }else{
        sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_b_3st_ptcut[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_b_3st_ptcut Failed to register histogram " << endreq;       
	  return sc;
	}
      mdtvstgclv1_b_3st_ptcut[i]->SetFillColor(5);
      mdtvstgclv1_b_3st_ptcut[i]->GetXaxis()->SetTitle("b");
      mdtvstgclv1_b_3st_ptcut[i]->GetYaxis()->SetTitle("Counts");
      sst.str("");

      //TGC Track nlayer
      sst<<"TGC_Track_Number_Of_Layer_Used_";
      sst<<side[i];
      mdtvstgclv1_nlayer[i] =new TH1F(sst.str().c_str(),sst.str().c_str(), 5, 3., 8.);
      if(i==0){
        sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_nlayer[i]) ;  
      }else{
        sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_nlayer[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_nlayer Failed to register histogram " << endreq;       
	  return sc;
	}
      mdtvstgclv1_nlayer[i]->SetFillColor(5);
      mdtvstgclv1_nlayer[i]->GetXaxis()->SetTitle("nLayer");
      mdtvstgclv1_nlayer[i]->GetYaxis()->SetTitle("Counts");
      mdtvstgclv1_nlayer[i]->SetMinimum(0.);
      sst.str("");

      //TGC Track nlayer with pt cut
      sst<<"TGC_Track_Number_Of_Layer_Used"<<ptcut;
      sst<<side[i];
      mdtvstgclv1_nlayer_ptcut[i] =new TH1F(sst.str().c_str(),sst.str().c_str(), 5, 3., 8.);
      if(i==0){
        sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_nlayer_ptcut[i]) ;  
      }else{
        sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_nlayer_ptcut[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_nlayer_ptcut Failed to register histogram " << endreq;       
	  return sc;
	}
      mdtvstgclv1_nlayer_ptcut[i]->SetFillColor(5);
      mdtvstgclv1_nlayer_ptcut[i]->GetXaxis()->SetTitle("nlayer");
      mdtvstgclv1_nlayer_ptcut[i]->GetYaxis()->SetTitle("Counts");
      mdtvstgclv1_nlayer_ptcut[i]->SetMinimum(0.);
      sst.str("");

      //TGC nlayer vs ndf
      sst<<"TGC_Number_Of_Layer_Used_Vs_NHit_";
      sst<<side[i];
      m_log<<MSG::DEBUG<<"TGC_Number_Of_Layer_Used_Vs_NHit_"<<i<<" "<<sst.str().c_str()<<std::endl;
      mdtvstgclv1_nlayervsnhit[i] =new TH2F(sst.str().c_str(),sst.str().c_str(), 5, 3, 8, 11, 3, 14); 
      if(i==0){
        sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_nlayervsnhit[i]) ;  
      }else{
        sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_nlayervsnhit[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_nlayervsnhit["<<i<<"] Failed to register histogram " << endreq;       
	  return sc;
        }  		  
      //m_log<<MSG::INFO <<"RoI_Eta_Vs_Phi_"<<side<<" has been registered"<<endreq;
      //tgclv1roietavsphi[i]->SetFillColor(5);
      mdtvstgclv1_nlayervsnhit[i]->GetXaxis()->SetTitle("NLayer");
      mdtvstgclv1_nlayervsnhit[i]->GetYaxis()->SetTitle("NHIT");
      sst.str("");

      //TGC nlayer vs ndf with ptcut
      sst<<"TGC_Number_Of_Layer_Used_Vs_NHit_"<<ptcut;
      sst<<side[i];
      m_log<<MSG::DEBUG<<"TGC_Number_Of_Layer_Used_Vs_NHit_"<<i<<" "<<sst.str().c_str()<<std::endl;
      mdtvstgclv1_nlayervsnhit_ptcut[i] =new TH2F(sst.str().c_str(),sst.str().c_str(), 5, 3, 8, 11, 3, 14); 
      if(i==0){
        sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_nlayervsnhit_ptcut[i]) ;  
      }else{
        sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_nlayervsnhit_ptcut[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_nlayervsnhit_ptcut["<<i<<"] Failed to register histogram " << endreq;       
	  return sc;
        }  		  
      //m_log<<MSG::INFO <<"RoI_Eta_Vs_Phi_"<<side<<" has been registered"<<endreq;
      //tgclv1roietavsphi[i]->SetFillColor(5);
      mdtvstgclv1_nlayervsnhit_ptcut[i]->GetXaxis()->SetTitle("NLayer");
      mdtvstgclv1_nlayervsnhit_ptcut[i]->GetYaxis()->SetTitle("NHIT");
      sst.str("");

      //TGC Track nstation
      sst<<"TGC_Track_Number_Of_Station_Used_";
      sst<<side[i];
      mdtvstgclv1_nstation[i] =new TH1F(sst.str().c_str(),sst.str().c_str(), 3, 1., 4.);
      if(i==0){
        sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_nstation[i]) ;  
      }else{
        sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_nstation[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_nstation Failed to register histogram " << endreq;       
	  return sc;
	}
      mdtvstgclv1_nstation[i]->SetFillColor(5);
      mdtvstgclv1_nstation[i]->GetXaxis()->SetTitle("nStation");
      mdtvstgclv1_nstation[i]->GetYaxis()->SetTitle("Counts");
      mdtvstgclv1_nstation[i]->SetMinimum(0.);
      sst.str("");

      //TGC Track nstation with pt cut
      sst<<"TGC_Track_Number_Of_Station_Used"<<ptcut;
      sst<<side[i];
      mdtvstgclv1_nstation_ptcut[i] =new TH1F(sst.str().c_str(),sst.str().c_str(), 3, 1., 4.);
      if(i==0){
        sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_nstation_ptcut[i]) ;  
      }else{
        sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_nstation_ptcut[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_nstation_ptcut Failed to register histogram " << endreq;       
	  return sc;
	}
      mdtvstgclv1_nstation_ptcut[i]->SetFillColor(5);
      mdtvstgclv1_nstation_ptcut[i]->GetXaxis()->SetTitle("nstation");
      mdtvstgclv1_nstation_ptcut[i]->GetYaxis()->SetTitle("Counts");
      mdtvstgclv1_nstation_ptcut[i]->SetMinimum(0.);
      sst.str("");

      //TGC Track Finding Efficiency
      sst<<"TGC_Track_Finding_Efficiency_";
      sst<<side[i];
      mdtvstgclv1_eff[i] =new TH1F(sst.str().c_str(),sst.str().c_str(), 11, 0., 11.);
      if(i==0){
        sc=mdtvstgclv1_track_a.regHist(mdtvstgclv1_eff[i]) ;  
      }else{
        sc=mdtvstgclv1_track_c.regHist(mdtvstgclv1_eff[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_eff Failed to register histogram " << endreq;       
	  return sc;
	}
      mdtvstgclv1_eff[i]->SetFillColor(5);
      //mdtvstgclv1_eff[i]->GetXaxis()->SetTitle("nStation");
      mdtvstgclv1_eff[i]->GetYaxis()->SetTitle("Counts");
      mdtvstgclv1_eff[i]->SetMinimum(0.);

      mdtvstgclv1_eff[i]->GetXaxis()->SetBinLabel(1,"N_Evt_Ana");
      mdtvstgclv1_eff[i]->GetXaxis()->SetBinLabel(2,"N_SL==1");
      mdtvstgclv1_eff[i]->GetXaxis()->SetBinLabel(3,"Track Found");
      mdtvstgclv1_eff[i]->GetXaxis()->SetBinLabel(4,"Track Found chi2/ndf<2.5");
      mdtvstgclv1_eff[i]->GetXaxis()->SetBinLabel(5,"3St Track Found");
      mdtvstgclv1_eff[i]->GetXaxis()->SetBinLabel(6,"3St Track Found chi2/ndf<2.5");
      mdtvstgclv1_eff[i]->GetXaxis()->SetBinLabel(7,"N_SL==1 pT>2");
      mdtvstgclv1_eff[i]->GetXaxis()->SetBinLabel(8,"Track Found pT>2");
      mdtvstgclv1_eff[i]->GetXaxis()->SetBinLabel(9,"Track Found pT>2 chi2/ndf<2.5");
      mdtvstgclv1_eff[i]->GetXaxis()->SetBinLabel(10,"3St Track Found pT>2");
      mdtvstgclv1_eff[i]->GetXaxis()->SetBinLabel(11,"3St Track Found pT>2 chi2/ndf<2.5");

      sst.str("");




    }

    //MDT eta vs TGC track eta
    for(int i=0;i<2;i++){

      //Track eta 
      sst<<"MDT_Eta_Vs_TGC_Track_Eta_";
      sst<<side[i];
      m_log<<MSG::DEBUG<<"MDT_Eta_Vs_TGC_Track_Eta "<<i<<" "<<sst.str().c_str()<<std::endl;
      mdtvstgclv1_eta[i] =new TH2F(sst.str().c_str(),sst.str().c_str(), 136, 1.0, 2.7, 136, 1.0, 2.7); 
      if(i==0){
        sc=mdtvstgclv1_expert_a.regHist(mdtvstgclv1_eta[i]) ;  
      }else{
        sc=mdtvstgclv1_expert_c.regHist(mdtvstgclv1_eta[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_eta["<<i<<"] Failed to register histogram " << endreq;       
	  return sc;
        }  		  
      //m_log<<MSG::INFO <<"RoI_Eta_Vs_Phi_"<<side<<" has been registered"<<endreq;
      //tgclv1roietavsphi[i]->SetFillColor(5);
      mdtvstgclv1_eta[i]->GetXaxis()->SetTitle("MDT Eta");
      mdtvstgclv1_eta[i]->GetYaxis()->SetTitle("TGC Track Eta");
      sst.str("");

      //Track eta diff
      sst<<"MDT_Eta_Minus_TGC_Track_Eta_";
      sst<<side[i];
      m_log<<MSG::DEBUG<<"MDT_Eta_Minus_TGC_Track_Eta "<<i<<" "<<sst.str().c_str()<<std::endl;
      mdtvstgclv1_etadiff[i] =new TH1F(sst.str().c_str(),sst.str().c_str(), 100, -0.25, 0.25); 
      if(i==0){
        sc=mdtvstgclv1_expert_a.regHist(mdtvstgclv1_etadiff[i]) ;  
      }else{
        sc=mdtvstgclv1_expert_c.regHist(mdtvstgclv1_etadiff[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_etadiff["<<i<<"] Failed to register histogram " << endreq;       
	  return sc;
        }  		  
      //m_log<<MSG::INFO <<"RoI_Eta_Vs_Phi_"<<side<<" has been registered"<<endreq;
      //tgclv1roietavsphi[i]->SetFillColor(5);
      mdtvstgclv1_etadiff[i]->GetXaxis()->SetTitle("MDT Eta - TGC Track Eta");
      mdtvstgclv1_etadiff[i]->GetYaxis()->SetTitle("Count");
      sst.str("");

      //Track eta 3 station
      sst<<"MDT_Eta_Vs_TGC_Track_Eta_3Station_Track_";
      sst<<side[i];
      m_log<<MSG::DEBUG<<"MDT_Eta_Vs_TGC_Track_Eta_3Station "<<i<<" "<<sst.str().c_str()<<std::endl;
      mdtvstgclv1_eta3st[i] =new TH2F(sst.str().c_str(),sst.str().c_str(), 136, 1.0, 2.7, 136, 1.0, 2.7); 
      if(i==0){
        sc=mdtvstgclv1_expert_a.regHist(mdtvstgclv1_eta3st[i]) ;  
      }else{
        sc=mdtvstgclv1_expert_c.regHist(mdtvstgclv1_eta3st[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_eta3st["<<i<<"] Failed to register histogram " << endreq;       
	  return sc;
        }  		  
      //m_log<<MSG::INFO <<"RoI_Eta_Vs_Phi_"<<side<<" has been registered"<<endreq;
      //tgclv1roietavsphi[i]->SetFillColor(5);
      mdtvstgclv1_eta3st[i]->GetXaxis()->SetTitle("MDT Eta");
      mdtvstgclv1_eta3st[i]->GetYaxis()->SetTitle("TGC Track Eta");
      sst.str("");

      //Track eta 3 station diff
      sst<<"MDT_Eta_Minus_TGC_Track_Eta_3Station_Track_";
      sst<<side[i];
      m_log<<MSG::DEBUG<<"MDT_Eta_Minus_TGC_Track_Eta_3Station "<<i<<" "<<sst.str().c_str()<<std::endl;
      mdtvstgclv1_etadiff3st[i] =new TH1F(sst.str().c_str(),sst.str().c_str(), 100, -0.25, 0.25); 
      if(i==0){
        sc=mdtvstgclv1_expert_a.regHist(mdtvstgclv1_etadiff3st[i]) ;  
      }else{
        sc=mdtvstgclv1_expert_c.regHist(mdtvstgclv1_etadiff3st[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_etadiff3st["<<i<<"] Failed to register histogram " << endreq;       
	  return sc;
        }  		  
      //m_log<<MSG::INFO <<"RoI_Eta_Vs_Phi_"<<side<<" has been registered"<<endreq;
      //tgclv1roietavsphi[i]->SetFillColor(5);
      mdtvstgclv1_etadiff3st[i]->GetXaxis()->SetTitle("MDT Eta - TGC Track Eta");
      mdtvstgclv1_etadiff3st[i]->GetYaxis()->SetTitle("Count");
      sst.str("");

      for( int sect=0 ; sect<16 ; sect++ ){
        //Track eta 3 station
        sst<<"MDT_Eta_Vs_TGC_Track_Eta_3Station_Track_MDTSector"<<sect+1<<"_";
        sst<<side[i];
        m_log<<MSG::DEBUG<<"MDT_Eta_Vs_TGC_Track_Eta_3Station_Track_MDTSector"<<sect+1<<"_"<<i<<" "<<sst.str().c_str()<<std::endl;
        mdtvstgclv1_eta3st_sector[i][sect] =new TH2F(sst.str().c_str(),sst.str().c_str(), 136, 1.0, 2.7, 136, 1.0, 2.7); 
        if(i==0){
          sc=mdtvstgclv1_expert_a.regHist(mdtvstgclv1_eta3st_sector[i][sect]) ;  
        }else{
          sc=mdtvstgclv1_expert_c.regHist(mdtvstgclv1_eta3st_sector[i][sect]) ;  
        }
        if(sc.isFailure())
          { m_log << MSG::FATAL << "mdtvstgclv1_eta3st["<<i<<"]["<<sect<<"] Failed to register histogram " << endreq;       
	    return sc;
          }  		  
        //m_log<<MSG::INFO <<"RoI_Eta_Vs_Phi_"<<side<<" has been registered"<<endreq;
        //tgclv1roietavsphi[i]->SetFillColor(5);
        mdtvstgclv1_eta3st_sector[i][sect]->GetXaxis()->SetTitle("MDT Eta");
        mdtvstgclv1_eta3st_sector[i][sect]->GetYaxis()->SetTitle("TGC Track Eta");
        sst.str("");
        
        //Track eta 3 station diff
        sst<<"MDT_Eta_Minus_TGC_Track_Eta_3Station_Track_MDTSector"<<sect+1<<"_";
        sst<<side[i];
        m_log<<MSG::DEBUG<<"MDT_Eta_Minus_TGC_Track_Eta_3Station_Track_MDTSector"<<sect+1<<"_"<<i<<" "<<sst.str().c_str()<<std::endl;
        mdtvstgclv1_etadiff3st_sector[i][sect] =new TH1F(sst.str().c_str(),sst.str().c_str(), 100, -0.25, 0.25); 
        if(i==0){
          sc=mdtvstgclv1_expert_a.regHist(mdtvstgclv1_etadiff3st_sector[i][sect]) ;  
        }else{
          sc=mdtvstgclv1_expert_c.regHist(mdtvstgclv1_etadiff3st_sector[i][sect]) ;  
        }
        if(sc.isFailure())
          { m_log << MSG::FATAL << "mdtvstgclv1_etadiff3st["<<i<<"]["<<sect<<"] Failed to register histogram " << endreq;       
	    return sc;
          }  		  
        //m_log<<MSG::INFO <<"RoI_Eta_Vs_Phi_"<<side<<" has been registered"<<endreq;
        //tgclv1roietavsphi[i]->SetFillColor(5);
        mdtvstgclv1_etadiff3st_sector[i][sect]->GetXaxis()->SetTitle("MDT Eta - TGC Track Eta");
        mdtvstgclv1_etadiff3st_sector[i][sect]->GetYaxis()->SetTitle("Count");
        sst.str("");
      }






      //Track eta 
      sst<<"MDT_Eta_Vs_TGC_Track_Eta_"<<ptcut;
      sst<<side[i];
      m_log<<MSG::DEBUG<<"MDT_Eta_Vs_TGC_Track_Eta "<<i<<" "<<sst.str().c_str()<<std::endl;
      mdtvstgclv1_eta_ptcut[i] =new TH2F(sst.str().c_str(),sst.str().c_str(), 136, 1.0, 2.7, 136, 1.0, 2.7); 
      if(i==0){
        sc=mdtvstgclv1_expert_a.regHist(mdtvstgclv1_eta_ptcut[i]) ;  
      }else{
        sc=mdtvstgclv1_expert_c.regHist(mdtvstgclv1_eta_ptcut[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_eta_ptcut["<<i<<"] Failed to register histogram " << endreq;       
	  return sc;
        }  		  
      //m_log<<MSG::INFO <<"RoI_Eta_Vs_Phi_"<<side<<" has been registered"<<endreq;
      //tgclv1roietavsphi[i]->SetFillColor(5);
      mdtvstgclv1_eta_ptcut[i]->GetXaxis()->SetTitle("MDT Eta");
      mdtvstgclv1_eta_ptcut[i]->GetYaxis()->SetTitle("TGC Track Eta");
      sst.str("");

      //Track eta diff
      sst<<"MDT_Eta_Minus_TGC_Track_Eta_"<<ptcut;
      sst<<side[i];
      m_log<<MSG::DEBUG<<"MDT_Eta_Minus_TGC_Track_Eta "<<i<<" "<<sst.str().c_str()<<std::endl;
      mdtvstgclv1_etadiff_ptcut[i] =new TH1F(sst.str().c_str(),sst.str().c_str(), 100, -0.25, 0.25); 
      if(i==0){
        sc=mdtvstgclv1_expert_a.regHist(mdtvstgclv1_etadiff_ptcut[i]) ;  
      }else{
        sc=mdtvstgclv1_expert_c.regHist(mdtvstgclv1_etadiff_ptcut[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_etadiff_ptcut["<<i<<"] Failed to register histogram " << endreq;       
	  return sc;
        }  		  
      //m_log<<MSG::INFO <<"RoI_Eta_Vs_Phi_"<<side<<" has been registered"<<endreq;
      //tgclv1roietavsphi[i]->SetFillColor(5);
      mdtvstgclv1_etadiff_ptcut[i]->GetXaxis()->SetTitle("MDT Eta - TGC Track Eta");
      mdtvstgclv1_etadiff_ptcut[i]->GetYaxis()->SetTitle("Count");
      sst.str("");

      //Track eta 3 station
      sst<<"MDT_Eta_Vs_TGC_Track_Eta_3Station_Track_"<<ptcut;
      sst<<side[i];
      m_log<<MSG::DEBUG<<"MDT_Eta_Vs_TGC_Track_Eta_3Station "<<i<<" "<<sst.str().c_str()<<std::endl;
      mdtvstgclv1_eta3st_ptcut[i] =new TH2F(sst.str().c_str(),sst.str().c_str(), 136, 1.0, 2.7, 136, 1.0, 2.7); 
      if(i==0){
        sc=mdtvstgclv1_expert_a.regHist(mdtvstgclv1_eta3st_ptcut[i]) ;  
      }else{
        sc=mdtvstgclv1_expert_c.regHist(mdtvstgclv1_eta3st_ptcut[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_eta3st_ptcut["<<i<<"] Failed to register histogram " << endreq;       
	  return sc;
        }  		  
      //m_log<<MSG::INFO <<"RoI_Eta_Vs_Phi_"<<side<<" has been registered"<<endreq;
      //tgclv1roietavsphi[i]->SetFillColor(5);
      mdtvstgclv1_eta3st_ptcut[i]->GetXaxis()->SetTitle("MDT Eta");
      mdtvstgclv1_eta3st_ptcut[i]->GetYaxis()->SetTitle("TGC Track Eta");
      sst.str("");

      //Track eta 3 station diff
      sst<<"MDT_Eta_Minus_TGC_Track_Eta_3Station_Track_"<<ptcut;
      sst<<side[i];
      m_log<<MSG::DEBUG<<"MDT_Eta_Minus_TGC_Track_Eta_3Station "<<i<<" "<<sst.str().c_str()<<std::endl;
      mdtvstgclv1_etadiff3st_ptcut[i] =new TH1F(sst.str().c_str(),sst.str().c_str(), 100, -0.25, 0.25); 
      if(i==0){
        sc=mdtvstgclv1_expert_a.regHist(mdtvstgclv1_etadiff3st_ptcut[i]) ;  
      }else{
        sc=mdtvstgclv1_expert_c.regHist(mdtvstgclv1_etadiff3st_ptcut[i]) ;  
      }
      if(sc.isFailure())
        { m_log << MSG::FATAL << "mdtvstgclv1_etadiff3st_ptcut["<<i<<"] Failed to register histogram " << endreq;       
	  return sc;
        }  		  
      //m_log<<MSG::INFO <<"RoI_Eta_Vs_Phi_"<<side<<" has been registered"<<endreq;
      //tgclv1roietavsphi[i]->SetFillColor(5);
      mdtvstgclv1_etadiff3st_ptcut[i]->GetXaxis()->SetTitle("MDT Eta - TGC Track Eta");
      mdtvstgclv1_etadiff3st_ptcut[i]->GetYaxis()->SetTitle("Count");
      sst.str("");

      for( int sect=0 ; sect<16 ; sect++ ){
        //Track eta 3 station
        sst<<"MDT_Eta_Vs_TGC_Track_Eta_3Station_Track_MDTSector"<<sect+1<<"_"<<ptcut;
        sst<<side[i];
        m_log<<MSG::DEBUG<<"MDT_Eta_Vs_TGC_Track_Eta_3Station_Track_MDTSector"<<sect+1<<"_"<<i<<" "<<sst.str().c_str()<<std::endl;
        mdtvstgclv1_eta3st_sector_ptcut[i][sect] =new TH2F(sst.str().c_str(),sst.str().c_str(), 136, 1.0, 2.7, 136, 1.0, 2.7); 
        if(i==0){
          sc=mdtvstgclv1_expert_a.regHist(mdtvstgclv1_eta3st_sector_ptcut[i][sect]) ;  
        }else{
          sc=mdtvstgclv1_expert_c.regHist(mdtvstgclv1_eta3st_sector_ptcut[i][sect]) ;  
        }
        if(sc.isFailure())
          { m_log << MSG::FATAL << "mdtvstgclv1_eta3st["<<i<<"]["<<sect<<"] Failed to register histogram " << endreq;       
	    return sc;
          }  		  
        //m_log<<MSG::INFO <<"RoI_Eta_Vs_Phi_"<<side<<" has been registered"<<endreq;
        //tgclv1roietavsphi[i]->SetFillColor(5);
        mdtvstgclv1_eta3st_sector_ptcut[i][sect]->GetXaxis()->SetTitle("MDT Eta");
        mdtvstgclv1_eta3st_sector_ptcut[i][sect]->GetYaxis()->SetTitle("TGC Track Eta");
        sst.str("");
        
        //Track eta 3 station diff
        sst<<"MDT_Eta_Minus_TGC_Track_Eta_3Station_Track_MDTSector"<<sect+1<<"_"<<ptcut;
        sst<<side[i];
        m_log<<MSG::DEBUG<<"MDT_Eta_Minus_TGC_Track_Eta_3Station_Track_MDTSector"<<sect+1<<"_"<<i<<" "<<sst.str().c_str()<<std::endl;
        mdtvstgclv1_etadiff3st_sector_ptcut[i][sect] =new TH1F(sst.str().c_str(),sst.str().c_str(), 100, -0.25, 0.25); 
        if(i==0){
          sc=mdtvstgclv1_expert_a.regHist(mdtvstgclv1_etadiff3st_sector_ptcut[i][sect]) ;  
        }else{
          sc=mdtvstgclv1_expert_c.regHist(mdtvstgclv1_etadiff3st_sector_ptcut[i][sect]) ;  
        }
        if(sc.isFailure())
          { m_log << MSG::FATAL << "mdtvstgclv1_etadiff3st["<<i<<"]["<<sect<<"] Failed to register histogram " << endreq;       
	    return sc;
          }  		  
        //m_log<<MSG::INFO <<"RoI_Eta_Vs_Phi_"<<side<<" has been registered"<<endreq;
        //tgclv1roietavsphi[i]->SetFillColor(5);
        mdtvstgclv1_etadiff3st_sector_ptcut[i][sect]->GetXaxis()->SetTitle("MDT Eta - TGC Track Eta");
        mdtvstgclv1_etadiff3st_sector_ptcut[i][sect]->GetYaxis()->SetTitle("Count");
        sst.str("");
      }

    }



  }//isNewRun

  return sc;
}


/*----------------------------------------------------------------------------------*/
StatusCode TGCStandaloneTracksMon::fillHistograms()
/*----------------------------------------------------------------------------------*/
{
  StatusCode sc = StatusCode::SUCCESS; 
  if (m_debuglevel) m_log << MSG::DEBUG << "TGCStandaloneTracksMon::TGC Standalone Trk Monitoring Histograms being filled" << endreq;

  // Checking if event passes muon triggers
  bool useMuonTrigger = ( (m_muon_triggers.size() != 0) && ( ! m_trigDecTool.empty() ) );
  bool passesMuonTrigger = false;

  if (m_debuglevel) m_log << MSG::DEBUG << "Before the loop over triggers, Using trigger? " << m_useTrigger << endreq;

  //return if no trigger decision && trigger aware analysis
  if( !useMuonTrigger && m_triggerAware ){
    if (m_debuglevel) m_log << MSG::DEBUG << "TGCStandaloneTracksMon::!useMuonTrigger && m_triggerAware" << endreq;
    return StatusCode::SUCCESS; 
  }
  if( (useMuonTrigger) && (m_useTrigger)){

    // Loop over the triggers.
    for (unsigned int ii = 0; ii < m_muon_triggers.size(); ii++ ){
      
      if (m_debuglevel) m_log << MSG::DEBUG << "Checking trigger " << m_muon_triggers[ii] << endreq;
      
      if( m_trigDecTool->isPassed(m_muon_triggers[ii]) ){
        if (m_debuglevel) m_log << MSG::DEBUG << "Fired trigger "<< m_muon_triggers[ii]<<endreq;
        //passesMuonTrigger = true;
        break; 
      }
    }
    if (m_debuglevel) m_log << MSG::DEBUG << "MuonTriggers' size: " << m_muon_triggers.size() << endreq;
    if(!passesMuonTrigger){ if (m_debuglevel) m_log << MSG::DEBUG << "No Trigger Fired!" << endreq; }
    if(!m_useTrigger) { passesMuonTrigger = true; }
  }

  //return if no fired muon trigger
  if(!passesMuonTrigger && m_triggerAware){ 
    if (m_debuglevel) m_log << MSG::DEBUG << "No Trigger Fired!" << endreq;
    return  StatusCode::SUCCESS; 
  }
    

  //TGC trigger PRD
  const Muon::TgcCoinDataContainer* tgc_trg_container;
  sc = (*m_activeStore)->retrieve(tgc_trg_container, m_outputCoinCollectionLocation);
  if (sc.isFailure()) {
    m_log << MSG::ERROR << " Cannot retrieve TgcCoinDataContainer " << endreq;
    return sc;
  }

  if (m_debuglevel) m_log << MSG::DEBUG << "size of tgc container is " << tgc_trg_container -> size() << endreq;

  //TGC hits
  const Muon::TgcPrepDataContainer* tgc_container;
  sc = (*m_activeStore)->retrieve(tgc_container, m_tgcPrepDataContainerName);
  if (sc.isFailure()) {
    m_log << MSG::ERROR << " Cannot retrieve TgcPrepDataContainer " << endreq;
    return sc;
  }
  
  //MDT PRD
  const Muon::MdtPrepDataContainer* mdt_container;
  sc = (*m_activeStore)->retrieve(mdt_container, m_key_mdt);
  if (sc.isFailure()) 
    {
      m_log << MSG::ERROR << " Cannot retrieve MdtPrepDataContainer " << m_key_mdt << endreq;
      return sc;
    }
  
  if (m_debuglevel) m_log << MSG::DEBUG <<"****** mdtContainer->size() : " << mdt_container->size()<<endreq;  

  LinearTrack wireTrack;
  LinearTrack stripTrack;
  
  //checkMDT(mdt_container);

  //only analize nSL==1
  int nSL = numberOfSL(tgc_trg_container);
  mdtvstgclv1_eff[0]->Fill(0);
  mdtvstgclv1_eff[1]->Fill(0);
  if(nSL!=1)return sc;



  //declare a group of histograms
  //std::string m_generic_path_tgclv1 = "Muon/MuonRawDataMonitoring/TGC";
  //MonGroup tgclv1_expert( this, m_generic_path_tgclv1+"/Overview", expert, run );

  //TH1* testptr0 = tgclv1roietavsphi[0];
  //sc = tgclv1_expert.getHist(testptr0,"RoI_Eta_Vs_Phi_A");
  //tgclv1roietavsphi[0] = dynamic_cast<TH2*>(testptr0);
  //if(sc.isFailure() ) m_log << MSG::WARNING << "couldn't get tgclv1roietavsphi[0] hist to MonGroup" << endreq;
  //m_log<<MSG::INFO <<"RoI_Eta_Vs_Phi_A_Side has been got"<<endreq;

  //find TGC track
  bool findTrack=findTGCTrack(tgc_container, tgc_trg_container, wireTrack);

  //fill MDT hit vs TGC RoI
  correlation(mdt_container, tgc_trg_container, wireTrack, findTrack);




  return sc;  // statuscode check  
   
} 

 
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TGCStandaloneTracksMon::procHistograms()
{ 
  if (m_debuglevel) m_log << MSG::DEBUG << "TGCStandaloneTracksMon finalize()" << endreq;
  if(endOfEventsBlock){}
  if(endOfLumiBlock){}
  if(endOfRun){} 
  return StatusCode::SUCCESS;
}

void TGCStandaloneTracksMon::correlation(const Muon::MdtPrepDataContainer* mdt_hit_container, 
					 const Muon::TgcCoinDataContainer* tgccontainer,
					 LinearTrack& wireTrack,
					 bool findTrack)
{
  if (m_debuglevel) m_log<<MSG::DEBUG<<"inside correlation"<<endreq;
  //StatusCode sc=StatusCode::SUCCESS;

  if( !findTrack && !m_debuglevel )return;

  double r[2][2][3];
  double eta[2][2][3];

  int SLAC=(theSL->isAside()==false);//a:0, c:1

  if(findTrack){


    for(int station=17;station<=18;station++)
      for(int multi=1;multi<=2;multi++)
        for(int layer=1;layer<=3;layer++){

          double z=MDTz(SLAC,station,multi,layer);
          r[station-17][multi-1][layer-1] = wireTrack.a() * z + wireTrack.b();

          CLHEP::Hep3Vector rz;
          rz.setRhoPhiZ(r[station-17][multi-1][layer-1],0,z);
          eta[station-17][multi-1][layer-1]=abs(rz.eta());

          if(m_debuglevel)
            m_log<<MSG::DEBUG
              //m_log<<MSG::INFO
                 <<"z "<<z<<" a "<<wireTrack.a()<<" b "<<wireTrack.b()
                 <<" r "<<r[station-17][multi-1][layer-1]
                 <<" eta "<<eta[station-17][multi-1][layer-1]<<endreq;

        }
  }


  //loop over TGC RoI container
  Muon::TgcCoinDataContainer::const_iterator it_end=tgccontainer->end();
  for( Muon::TgcCoinDataContainer::const_iterator it=tgccontainer->begin();
       it!=it_end;
       ++it){
  

    if (m_debuglevel) m_log<<MSG::DEBUG<< "size of tgc collection is " << (*it) -> size() << endreq;

    //loop over TGC RoI collection
    Muon::TgcCoinDataCollection::const_iterator itc_end=(*it)->end();
    for( Muon::TgcCoinDataCollection::const_iterator itc=(*it)->begin();
         itc!= itc_end;
         ++itc){

      Muon::TgcCoinData* tcd=*itc;
      Identifier tgcid=(*itc)->identify();

      if( tcd->type() != Muon::TgcCoinData::TYPE_SL )continue;

      int ac=(tcd->isAside()==false);//isNotAside a:0, c:1
      int ef=(tcd->isForward()==false);//isNotForward f:0, e:1
      int phi48=tcd->phi();//48(24)
      int roi=tcd->roi();
      int pt=tcd->pt();
      int roieta;//1-53
      int roiphi;//1-192

      roi2etaphi(*tcd, roieta, roiphi);

      //int tgcMdtSector=roiphi2mdtSector(roiphi,ef);

      const MuonGM::TgcReadoutElement*  pReadoutElementTGC = m_muonMgr->getTgcReadoutElement(tgcid);
      const Amg::Vector3D pos = pReadoutElementTGC->channelPos(tgcid);

      float tgcEta = abs(pos.eta());
      float tgcPhi = pos.phi();
      if(tgcPhi<0)tgcPhi+=2*M_PI;


      if(m_debuglevel){
        m_log<<MSG::DEBUG
	  //m_log<<MSG::INFO
             <<"ac "<<ac
             <<" ef "<<ef
             <<" phi48 "<<phi48
             <<" roi "<<roi
             <<" roieta "<<roieta
             <<" roiphi "<<roiphi
             <<" tgcEta "<<tgcEta
             <<" tgcPhi "<<tgcPhi
             <<endreq;
      }


      //loop over MDT container
      Muon::MdtPrepDataContainer::const_iterator containerIt;
      Muon::MdtPrepDataContainer::const_iterator container_end=mdt_hit_container->end();
      for (containerIt = mdt_hit_container->begin() ; 
           containerIt != container_end ; 
           ++containerIt){ 
    
        Identifier mdt_id = (*containerIt)->identify();
        // Field           Range               Notes
        // ==============================================================================
        // StationName     unsigned integer    maps to "BIL", "EMS" ,etc.
        // StationEta      [-6,-1]             backward endcap (-1 at lowest R)
        //                 [-8,8]              barrel (increases with Z)
        //                 [-6,-1]+[1,6]       forward endcap (1 at lowest R)
        // StationPhi      [1,8]               increases with phi
        // Technology      [0]                 maps to "MDT"
        // Multilayer      [1,2]               barrel: increases with R
        //                                     endcap: increases with |Z|
        // TubeLayer       [1,4]               barrel: increases with R
        //                                     endcap: increases with |Z|
        // Tube            [1,n]               barrel: increases with |Z|
        //                                     endcap: increases with R
        // ==============================================================================

        int mdtStationName      =   int(m_mdtIdHelper->stationName(mdt_id)) ;

        //SN     Layer Tube Radial
        //13:EIL 2x4   x54  x4
        //49:EIS 2x4   x36  x2
        //17:EML 2x3   x64  x5
        //18:EMS 2x3   x64  x5
        //20:EOL 2x3   x48  x6
        //21:EOS 2x3   x48  x6

        //only Endcap MDT
        //if(mdtStationName!=13 && mdtStationName!=49 && mdtStationName!=17 && mdtStationName!=18 && mdtStationName!=20 && mdtStationName!=21 )continue;

        //if (m_debuglevel){
        //m_log<<MSG::DEBUG
        ///     <<"mmdtStationName "<<mdtStationName
        //    <<endreq;
        //}

        //only Endcap middle MDT
        if(mdtStationName!=17 && mdtStationName!=18 )continue;

        int mdtStationEta       =   int(m_mdtIdHelper->stationEta(mdt_id))  ;//backward:[-6,-1], forward:[1,6], (1 or -1 at lowest R)
        int mdtStationPhi       =   int(m_mdtIdHelper->stationPhi(mdt_id))  ;//[1:8]
        int mdtAC = (mdtStationEta<0);//a:0, c:1

        int mdtSector=mdtStationPhi*2-1;//1-16
        if(mdtStationName==18)mdtSector+=1;
        double mdtSectorPhi = ((float)mdtSector-1.)*M_PI/8.;

        //same Side
        if(ac!=mdtAC)continue;
        if (m_debuglevel) m_log<<MSG::DEBUG<< "size of mdt collection is " << (*containerIt) -> size() << endreq;

        if (m_debuglevel){
          m_log<<MSG::DEBUG
               <<"mdtStationName "<<mdtStationName
               <<" mdtStationEta "<<mdtStationEta
               <<" mdtStationPhi "<<mdtStationPhi
               <<" mdtSectorPhi "<<mdtSectorPhi
               <<endreq;
        }
        //if( itgcstationEta!=RoIEta || itgcstationPhi!=RoIPhi48 || end_or_for!=RoIEF || a_or_c!=RoISide )continue;

        //loop over MDT PRD Collection
        Muon::MdtPrepDataCollection::const_iterator collection_it_end=(*containerIt)->end();


        int tmp[2][4][64];
        for(int i=0;i<2;i++)
          for(int j=0;j<4;j++)
            for(int k=0;k<64;k++)
              tmp[i][j][k]=0;

        for(Muon::MdtPrepDataCollection::const_iterator mdtCollection=(*containerIt)->begin();
            mdtCollection!= collection_it_end;
            ++mdtCollection){

          Identifier mdt_id2 = (*mdtCollection)->identify();

          int mdtMultiLayer       =   int(m_mdtIdHelper->multilayer(mdt_id2));
          int mdtTubeLayer        =   int(m_mdtIdHelper->tubeLayer(mdt_id2));
          int mdtTube             =   int(m_mdtIdHelper->tube(mdt_id2));
          int mdtTubeIdForEM = (abs(mdtStationEta)-1)*64 + mdtTube -1;

          if(m_debuglevel){
            m_log<<MSG::DEBUG
                 <<"mdtMultiLayer "<<mdtMultiLayer
                 <<" mdtTubeLayer "<<mdtTubeLayer
                 <<" mdtTube "<<mdtTube
                 <<" mdtTubeIdForEM "<<mdtTubeIdForEM
                 <<endreq;
          }

          if(tmp[mdtMultiLayer-1][mdtTubeLayer-1][mdtTube-1]==1)continue;
          tmp[mdtMultiLayer-1][mdtTubeLayer-1][mdtTube-1]=1;

          int adc = (*mdtCollection)->adc();
          int tdc = (*mdtCollection)->tdc();

          //these histograms are only for debug purpose
          if( m_debuglevel ){
            m_log << MSG::DEBUG << "fill MDT ADC" << endreq;
            mdtvstgclv1_mdt_adc->Fill((float)adc);
            mdtvstgclv1_mdt_tdc->Fill((float)tdc);
            mdtvstgclv1_mdt_tdc_vs_adc->Fill((float)tdc,(float)adc);
            mdtvstgclv1_adcsector[mdtAC]->Fill((float)mdtSector,(float)adc);          
          }
          if(adc < m_MdtAdcCut )continue;
          if(tdc < m_MdtTdcMinCut || tdc > m_MdtTdcMaxCut )continue;

          if( m_debuglevel ){
            m_log << MSG::DEBUG << "MDT Readout Element" << endreq;
          }            
          const MuonGM::MdtReadoutElement*  pReadoutElementMDT = m_muonMgr->getMdtReadoutElement(mdt_id2);
          const Amg::Vector3D mdtgPos = pReadoutElementMDT->tubePos(mdt_id2); //global position of the wire
          float mdtEta = abs(mdtgPos.eta());
          float mdtPhi = mdtgPos.phi();
          float mdtr = mdtgPos.perp();
          float mdtz = mdtgPos.z();
          if(mdtPhi<0)mdtPhi+=2*M_PI;
          
          if (m_debuglevel){
            m_log<<MSG::DEBUG
              //m_log<<MSG::INFO
                 <<" Name "<<mdtStationName
                 <<" Eta "<<mdtStationEta
                 <<" Phi "<<mdtStationPhi
                 <<" MultiLayer "<<mdtMultiLayer
                 <<" TubeLayer "<<mdtTubeLayer
                 <<" Tube "<<mdtTube
                 <<" TubeIdForEM "<<mdtTubeIdForEM
                 <<" Eta "<<mdtEta
                 <<" Phi "<<mdtPhi
                 <<" r "<<mdtr
                 <<" z "<<mdtz
                 <<" Sec "<<mdtSector
                 <<" SecPhi "<<mdtSectorPhi
                 <<" ADC "<<adc
                 <<" TDC "<<tdc
                 <<endreq;

          }

          //if(adc < m_MdtAdcCut || tdc < m_MdtTdcCut)continue;


          //these histograms are only for debug purpose
          if(m_debuglevel)
            mdtvstgclv1_mdt_tdc_adc_cut->Fill((float)tdc);

          double cosPhi = cos(mdtPhi-tgcPhi) ;
          if(m_debuglevel)
            m_log<<MSG::DEBUG
                 <<" mdtPhi "<<mdtPhi
                 <<" tgcPhi "<<tgcPhi
                 <<" cos(mdtPhi-tgcPhi) "<<cosPhi
                 <<endreq;
          if( cosPhi < cos(M_PI/15.) )continue;//bit larger than 2pi/32

          CLHEP::Hep3Vector rz;
          rz.setRhoPhiZ(mdtr/cosPhi,0,mdtz);
          double mdtEtaCorr = abs(rz.eta());

          if(m_debuglevel){
            m_log<<MSG::DEBUG
              //if(mdtSector==9 && mdtAC==0){
              //m_log<<MSG::INFO
                 <<"z "<<mdtz
                 <<" mdtPhi "<<mdtPhi
                 <<" tgcPhi "<<tgcPhi
                 <<" cos(mdtPhi-tgcPhi) "<<cosPhi
                 <<" r     "<<mdtr
                 <<" rcorr "<<mdtr/cosPhi
                 <<" eta "<<mdtEta
                 <<" etacorr "<<mdtEtaCorr
                 <<endreq;
          
            std::vector<XYPosition>::iterator it_end=wires.end();
            for(std::vector<XYPosition>::iterator it=wires.begin();
                it!=it_end;
                it++){

              CLHEP::Hep3Vector rzhit;
              //CLHEP::Hep3Vector rztrk;
              rzhit.setRhoPhiZ(it->y(),0,it->x());
              //rztrk.setRhoPhiZ(it->x(),0,a*it->x()+b);
              double etahit=rzhit.eta();
              
              m_log<<MSG::INFO
                   <<" used "<<it->used()
                   <<" layer "<<it->layer()
                   <<" r "<<it->y()
                   <<" z "<<it->x()
                   <<" eta "<<etahit
                   <<endreq;
            }
          }
          if(findTrack){

            mdtvstgclv1_eta[mdtAC]->Fill(mdtEtaCorr, eta[mdtStationName-17][mdtMultiLayer-1][mdtTubeLayer-1]);
            mdtvstgclv1_etadiff[mdtAC]->Fill(mdtEtaCorr-eta[mdtStationName-17][mdtMultiLayer-1][mdtTubeLayer-1]);
            if(pt>2){
              mdtvstgclv1_eta_ptcut[mdtAC]->Fill(mdtEtaCorr, eta[mdtStationName-17][mdtMultiLayer-1][mdtTubeLayer-1]);
              mdtvstgclv1_etadiff_ptcut[mdtAC]->Fill(mdtEtaCorr-eta[mdtStationName-17][mdtMultiLayer-1][mdtTubeLayer-1]);
            }
            if( wireTrack.nStation()==3 ){
	      //if( wireTrack.nLayer() >=6 ){
              mdtvstgclv1_eta3st[mdtAC]->Fill(mdtEtaCorr, eta[mdtStationName-17][mdtMultiLayer-1][mdtTubeLayer-1]);
              mdtvstgclv1_etadiff3st[mdtAC]->Fill(mdtEtaCorr-eta[mdtStationName-17][mdtMultiLayer-1][mdtTubeLayer-1]);

              mdtvstgclv1_eta3st_sector[mdtAC][mdtSector-1]->Fill(mdtEtaCorr, eta[mdtStationName-17][mdtMultiLayer-1][mdtTubeLayer-1]);
              mdtvstgclv1_etadiff3st_sector[mdtAC][mdtSector-1]->Fill(mdtEtaCorr-eta[mdtStationName-17][mdtMultiLayer-1][mdtTubeLayer-1]);

              if(pt>2){
                mdtvstgclv1_eta3st_ptcut[mdtAC]->Fill(mdtEtaCorr, eta[mdtStationName-17][mdtMultiLayer-1][mdtTubeLayer-1]);
                mdtvstgclv1_etadiff3st_ptcut[mdtAC]->Fill(mdtEtaCorr-eta[mdtStationName-17][mdtMultiLayer-1][mdtTubeLayer-1]);

                mdtvstgclv1_eta3st_sector_ptcut[mdtAC][mdtSector-1]->Fill(mdtEtaCorr, eta[mdtStationName-17][mdtMultiLayer-1][mdtTubeLayer-1]);
                mdtvstgclv1_etadiff3st_sector_ptcut[mdtAC][mdtSector-1]->Fill(mdtEtaCorr-eta[mdtStationName-17][mdtMultiLayer-1][mdtTubeLayer-1]);
              }
            }
          }

          //fill histograms

        }
      }
    }
  }


}

void TGCStandaloneTracksMon::roi2etaphi(Muon::TgcCoinData& cd, int& eta, int& phi)
{
  
  int roiphi=cd.phi();//phi in 1/48(24) sector for endcap(forward)
  int roi=cd.roi();//0-147(0-63)

  //int localeta=roi/4;

  if(cd.isForward()){//forward

    int localphi=roi%4;

    //eta=roi/4;
    eta=roi/4+36;
    phi=(localphi+roiphi*4)*2;

  }else{//endcap

    int sector=phi2sector(roiphi,1);

    phi=roiphi*4 + ( sector%2==0 ? roi%4 : 3-(roi%4)   );
    eta=roi/4;
  }

  //eta=52-eta;
  return;

}

int TGCStandaloneTracksMon::phi2sector(int phi, int ef)
{
  int sector=-1;
  if(ef==0){//forward
    sector=phi/2+1;//(phi,sector)= (1,1), (2,2), (3,2), (4,3)
  }else{//endcap
    sector=(phi+1)/4+1;//(phi,sector)= (1,1), (2,1), (3,2), (4,2)
  }
  if(sector>12)sector=1;

  return sector;
}

int TGCStandaloneTracksMon::roiphi2mdtSector(int roiphi, int ef)
{
  int mdtSector=-1;
  if(ef==0){//forward phi1-24
    mdtSector=roiphi/2+1;//(phi,sector)= (1,1), (2,2), (3,2), (4,3)
  }else{//endcap
    mdtSector=(roiphi+1)/4+1;//(phi,sector)= (1,1), (2,1), (3,2), (4,2)
  }
  if(mdtSector>8)mdtSector=1;

  return mdtSector;
}


int TGCStandaloneTracksMon::numberOfSL(const Muon::TgcCoinDataContainer* tgctrgcontainer)
{

  int nSL=0;

  //loop over TGC RoI container
  Muon::TgcCoinDataContainer::const_iterator it_end=tgctrgcontainer->end();
  for( Muon::TgcCoinDataContainer::const_iterator it=tgctrgcontainer->begin();
       it!=it_end;
       ++it){
  

    if (m_debuglevel) m_log<<MSG::DEBUG<< "size of tgc collection is " << (*it) -> size() << endreq;

    //loop over TGC RoI collection
    Muon::TgcCoinDataCollection::const_iterator itc_end=(*it)->end();
    for( Muon::TgcCoinDataCollection::const_iterator itc=(*it)->begin();
         itc!= itc_end;
         ++itc){

      Muon::TgcCoinData* tcd=*itc;
      Identifier tgcid=(*itc)->identify();

      if( tcd->type() != Muon::TgcCoinData::TYPE_SL )continue;
      m_log<<MSG::DEBUG<<"pt"<<tcd->pt()<<endreq;
      nSL++;
      theSL=tcd;
    }
  }

  return nSL;
}

bool TGCStandaloneTracksMon::findTGCTrack(const Muon::TgcPrepDataContainer* tgc_hit_container,
					  const Muon::TgcCoinDataContainer* /*tgc_trigger_container */,
					  LinearTrack& wireTrack)

// VSK: tgc_trigger_container is never used!. Fixed compilation warning

{
  if (m_debuglevel) m_log << MSG::DEBUG <<"inside findTGCTrack" <<endreq;  

  //WireHits.clear();
  //StripHits.clear();

  wires.clear();
  strips.clear();

  Identifier tgcSLID = theSL->identify();
  int SLAC=(theSL->isAside()==false);//isNotAside a:0, c:1
  //int SLEF=(theSL->isForward()==false);//isNotForward f:0, e:1
  //int SLStationPhi=theSL->phi();//1-48

  Amg::Vector3D SLpos=theSL->globalposOut();//HepGeom::Point3D<double> SL position
  SLr=SLpos.perp();
  SLeta=SLpos.eta();
  SLphi=SLpos.phi();
  SLz=SLpos.z();
  int pt = theSL->pt();

  //double SLerrorR=(theSL->errMat()).covValue(Trk::loc1)/sqrt(12.);
  //double SLerrorPhi=(theSL->errMat()).covValue(Trk::loc2)/sqrt(12.);

  if (m_debuglevel) m_log << MSG::DEBUG <<"SL eta" <<SLeta<<"  phi"<<SLphi<<endreq;

  //int SLStationEta       =   m_tgcIdHelper->stationEta(tgcSLID)  ;//backward:[-5,-1], forward:[1,5], (1 or -1 at lowest R)


  //loop over hit container
  Muon::TgcPrepDataContainer::const_iterator container_end=tgc_hit_container->end();
  for (Muon::TgcPrepDataContainer::const_iterator containerIt = tgc_hit_container->begin() ; 
       containerIt != container_end ; 
       ++containerIt){ 
    
    Identifier tgcHitColID=(*containerIt)->identify();

    int HStationName      =   m_tgcIdHelper->stationName(tgcHitColID) ;//unsigned integer 41:T1F 42:T1E 43:T2F 44:T2E 45:T3F 46:T3E 47:T4F 48:T4E (T4 means inner small wheel TGCs, EI/FI)
    int HStationPhi       =   m_tgcIdHelper->stationPhi(tgcHitColID)  ;//forward:[1:24], endcap:[1:48], FI:[1:21], EI:[1:24]
    int HStationEta       =   m_tgcIdHelper->stationEta(tgcHitColID)  ;//backward:[-5,-1], forward:[1,5], (1 or -1 at lowest R)
    int HAC = (HStationEta<0);//a:0, c:1
    //int HEF = (HStationName==42||HStationName==44||HStationName==46||HStationName==48);// 0:forward, 1:endcap

    if(m_debuglevel) m_log <<MSG::DEBUG<<"StationName "<<HStationName<<" StationPhi "<<HStationPhi<<" StationEta "<<HStationEta<<endreq;


    if(HAC!=SLAC ||//side should be same
       HStationName>46 )continue;//not EIFI

    //int diffStationPhi=abs(SLStationPhi-HStationPhi);//0-47
    //int diffStationEta=abs(SLStationEta-HStationEta);

    //if( HEF==1 && diffStationPhi<47 && diffStationPhi>1 )continue;//only allow phi+-1
    //if( HEF==0 && diffStationPhi<23 && diffStationPhi>1 )continue;//
    //if( diffStationEta>1 )continue;//only allow eta+-1;

    //loop over previous hit PRD
    Muon::TgcPrepDataCollection::const_iterator collection_it_end=(*containerIt)->end();
    for(Muon::TgcPrepDataCollection::const_iterator tgcCollection=(*containerIt)->begin();
        tgcCollection!= collection_it_end;
        ++tgcCollection){
    
      Identifier id = (*tgcCollection)->identify();
      const MuonGM::TgcReadoutElement*  pReadoutElementTGC = m_muonMgr->getTgcReadoutElement(id);
      const Amg::Vector3D pos = pReadoutElementTGC->channelPos(id);

      int HIsStrip        =   m_tgcIdHelper->isStrip(id);
      int HStationGasGap  =   m_tgcIdHelper->gasGap(id)  ;//

      //int tgcid = m_tgcIdHelper->channel(id);
      //double shortWidth = pReadoutElementTGC->gangShortWidth(HStationGasGap, tgcid);
      //double longWidth = pReadoutElementTGC->gangLongWidth(HStationGasGap, tgcid);
      //double length = pReadoutElementTGC->gangLength(HStationGasGap, tgcid);
      
      int tgcch = m_tgcIdHelper->channel(id);
      //double length;
      double width;
      double sigma;
 
      int layer=stationGasGap2layer(HStationName, HStationGasGap);

      double r=pos.perp();
      double eta=pos.eta();
      double phi=pos.phi();
      double z=pos.z();

      double diffeta=SLeta-eta;
      double diffphi=SLphi-phi;

      if(diffphi>M_PI)diffphi-=2*M_PI;
      if(diffphi<-M_PI)diffphi+=2*M_PI;
 
      if(HIsStrip==1){//strip hit
        if( HStationName>=45 && //M3
            (abs(diffphi)>0.035 || abs(diffeta)>0.15) )continue;//empirical
        if( HStationName<45 &&//M1 M2
            (abs(diffphi)>0.1 ||abs(diffeta)>0.15) )continue;

        if (m_debuglevel) m_log << MSG::DEBUG <<"add strip hit layer"<<layer<<"  eta" <<eta<<"  phi" <<phi<<" r "<<r<<endreq;
        //StripHits.push_back(id);

        //phi-z coordinate
        //length = pReadoutElementTGC->stripLength(HStationGasGap, tgcch);
        width = pReadoutElementTGC->stripWidth(HStationGasGap, tgcch);
        //calculate error on phi
        sigma=width/(r*sqrt(12.));

        XYPosition s(z,phi,sigma,layer);
        strips.push_back(s);

      }else{//wire hit

        if( HStationName>=45 && //M3
            (abs(diffeta)>0.035 || abs(diffphi)>0.131) )continue;// 2*M_PI/48
        if( (HStationName==43 || HStationName==44) &&//M2
            //(abs(diffeta)>0.1 || abs(diffphi)>0.131) )continue;// 2*M_PI/48
            (abs(diffeta)>0.05 || abs(diffphi)>0.131) )continue;// 2*M_PI/48
        if( (HStationName==41 || HStationName==42) &&//M1
            (abs(diffeta)>0.1 || abs(diffphi)>0.131) )continue;// 2*M_PI/48=0.1309


        /*
          if( HStationName==45 &&
          (abs(diffeta)>0.035 || abs(diffphi)>0.131)  )continue;//forward 2*M_PI/48=0.1309
          if( HStationName==46 &&//M3
          (abs(diffeta)>0.035 || abs(diffphi)>0.0655) )continue;//endcap  2*M_PI/96=0.0654498
          if( HStationName==43 && 
          (abs(diffeta)>0.100 || abs(diffphi)>0.131)  )continue;
          if( HStationName==44 && 
          (abs(diffeta)>0.100 || abs(diffphi)>0.0655) )continue;
          if( HStationName==41 &&
          (abs(diffeta)>0.100 || abs(diffphi)>0.131)  )continue;
          if( HStationName==42 &&
          (abs(diffeta)>0.100 || abs(diffphi)>0.0655) )continue;
        */

        if (m_debuglevel) m_log << MSG::DEBUG <<"add wire hit layer"<<layer<<"  eta" <<eta<<"  phi" <<phi<<" r "<<r<<endreq;
        //WireHits.push_back(id);

        //r-z coordinate
        //length = pReadoutElementTGC->gangShortWidth(HStationGasGap, tgcch);
        //calculate error on r
        width = pReadoutElementTGC->gangLength(HStationGasGap, tgcch);      
        sigma=width/sqrt(12.);

        XYPosition s(z,r,sigma,layer);
        wires.push_back(s);

      }
    }//hit collection
  }//hit container

  //std::sort(wires.begin(),wires.end());
  //std::vector<XYPosition>::iterator it_end=positions.end();
  //for(std::vector<XYPosition>::iterator it=positions.begin();
  //    it!=it_end;
  //    it++){
  //}
  //std::sort(strips);
  double a;
  double b;
  double chi2w;
  int ndfw;
  double chi2ondfw = 999.9;
  if (m_debuglevel) m_log << MSG::DEBUG<<"wire fit"<<endreq;
  //LinearFit(wires,a,b,chi2w,ndfw);

  int nwire = wires.size();
  mdtvstgclv1_nwire_around_roi[SLAC]->Fill( (nwire > 49 ? 49 : nwire) );
  if(pt>2)mdtvstgclv1_nwire_around_roi_ptcut[SLAC]->Fill( (nwire > 49 ? 49 : nwire) );

  if( nwire >=15 )return false;

  bool fitOK=LinearFit(wires,wireTrack);
  if(fitOK){
    a=wireTrack.a();
    b=wireTrack.b();
    chi2w=wireTrack.chi2();
    ndfw=wireTrack.ndf();
    chi2ondfw=chi2w/ndfw;

    int nstation=wireTrack.nStation();

    mdtvstgclv1_a[SLAC]->Fill(a);
    mdtvstgclv1_b[SLAC]->Fill(b);
    mdtvstgclv1_chi2overndf[SLAC]->Fill(chi2w/ndfw);
    mdtvstgclv1_nhit[SLAC]->Fill(ndfw+2);
    if(nstation==3){
      mdtvstgclv1_a_3st[SLAC]->Fill(a);
      mdtvstgclv1_b_3st[SLAC]->Fill(b);
      mdtvstgclv1_chi2overndf_3st[SLAC]->Fill(chi2w/ndfw);
      mdtvstgclv1_nhit_3st[SLAC]->Fill(ndfw+2);
    }

    mdtvstgclv1_nstation[SLAC]->Fill(nstation);
    mdtvstgclv1_nlayer[SLAC]->Fill(wireTrack.nLayer());
    mdtvstgclv1_eff[SLAC]->Fill(2);
    if(chi2ondfw < 2.5) mdtvstgclv1_eff[SLAC]->Fill(3);
    if(nstation==3)mdtvstgclv1_eff[SLAC]->Fill(4);
    if(nstation==3 && chi2ondfw < 2.5)mdtvstgclv1_eff[SLAC]->Fill(5);

    if(pt>2){
      mdtvstgclv1_a_ptcut[SLAC]->Fill(a);
      mdtvstgclv1_b_ptcut[SLAC]->Fill(b);
      mdtvstgclv1_chi2overndf_ptcut[SLAC]->Fill(chi2w/ndfw);
      mdtvstgclv1_nhit_ptcut[SLAC]->Fill(ndfw+2);
      if(nstation==3){
        mdtvstgclv1_a_3st_ptcut[SLAC]->Fill(a);
        mdtvstgclv1_b_3st_ptcut[SLAC]->Fill(b);
        mdtvstgclv1_chi2overndf_3st_ptcut[SLAC]->Fill(chi2w/ndfw);
        mdtvstgclv1_nhit_3st_ptcut[SLAC]->Fill(ndfw+2);
        mdtvstgclv1_eff[SLAC]->Fill(9);
        if(chi2ondfw<2.5 )mdtvstgclv1_eff[SLAC]->Fill(10);
      }

      mdtvstgclv1_nstation_ptcut[SLAC]->Fill(nstation);
      mdtvstgclv1_nlayer_ptcut[SLAC]->Fill(wireTrack.nLayer());
      mdtvstgclv1_eff[SLAC]->Fill(7);
      if(chi2ondfw<2.5 )mdtvstgclv1_eff[SLAC]->Fill(8);
    }

    mdtvstgclv1_nlayervsnhit[SLAC]->Fill(wireTrack.nLayer(),wireTrack.ndf()+2);
    if(pt>2)mdtvstgclv1_nlayervsnhit_ptcut[SLAC]->Fill(wireTrack.nLayer(),wireTrack.ndf()+2);

    std::vector<XYPosition>::iterator it_end=wires.end();
    for(std::vector<XYPosition>::iterator it=wires.begin();
        it!=it_end;
        it++){
      if(it->used()){
        mdtvstgclv1_pull[SLAC]->Fill(sqrt(it->pchi2())*it->signchi());
        if(nstation==3) mdtvstgclv1_pull3st[SLAC]->Fill(sqrt(it->pchi2())*it->signchi());
        if(pt>2){
          mdtvstgclv1_pull_ptcut[SLAC]->Fill(sqrt(it->pchi2())*it->signchi());
          if(nstation==3) mdtvstgclv1_pull3st_ptcut[SLAC]->Fill(sqrt(it->pchi2())*it->signchi());
        }
        int station=0;
        if(it->layer()<3) station=0;
        else if(it->layer()<5) station=1;
        else station=2;

        CLHEP::Hep3Vector rzhit;
        //CLHEP::Hep3Vector rztrk;
        rzhit.setRhoPhiZ(it->y(),0,it->x());
        //rztrk.setRhoPhiZ(it->x(),0,a*it->x()+b);
        double etahit=rzhit.eta();
        //double etatrk=rztrk.eta();

        //m_log<<MSG::INFO<<"side "<<SLAC<<" etahit "<<etahit<<" SLeta "<<SLeta<<endreq;
        //m_log<<MSG::INFO<<"rho "<<it->y()<<" z "<<it->x()<<endreq;
        //std::cout<<"side "<<SLAC<<" etahit "<<etahit<<" SLeta "<<SLeta<<std::endl;
        double deta=(etahit>0 ?  etahit - SLeta : SLeta-etahit);
        mdtvstgclv1_deltaeta[SLAC][station]->Fill( deta );
        if(nstation==3) mdtvstgclv1_deltaeta3st[SLAC][station]->Fill( deta );
        if(pt>2){
          mdtvstgclv1_deltaeta_ptcut[SLAC][station]->Fill( deta );
          if(nstation==3) mdtvstgclv1_deltaeta3st_ptcut[SLAC][station]->Fill( deta );
        }
      }
    }
  }
  mdtvstgclv1_eff[SLAC]->Fill(1);
  if(pt>2)mdtvstgclv1_eff[SLAC]->Fill(6);
  //double c;
  //double d;
  //double chi2s;
  //int ndfs;

  //if (m_debuglevel) m_log << MSG::DEBUG<<"strip fit"<<endreq;
  //LinearFit(strips,c,d,chi2s,ndfs);

  return (fitOK && (chi2ondfw <2.5) );

}



int TGCStandaloneTracksMon::stationGasGap2layer(int station, int GasGap)
{
  int layer=0;
  if(station==41||station==42){
    layer += GasGap;
  }else if(station==43||station==44){
    layer = 3+GasGap;
  }else if(station==45||station==46){
    layer = 5+GasGap;
  }
  return layer;
}

bool TGCStandaloneTracksMon::LinearFit(std::vector<XYPosition>& positions,
				       LinearTrack& wireTrack)
//double& a,
//double& b,
//double& chi2,
//int& ndf)
{
  if (m_debuglevel) m_log << MSG::DEBUG <<"inside LinearFit" <<endreq;  

  double a=0.;
  double b=0.;
  double chi2=0.;
  int ndf=0;


  //int nloop=7;
  //double pchi2road[7]={100.,50.,36.,25.,16.,9.,9.};
  //double largestpchi2=1000.;
  XYPosition* largest;

  double chi2overndfold=0.;
  double dchi2overndf=0.;
  int nlayer;//number of used layer
  int nstation;//number of used station

  /*
  //find initial value with all hits
  for(int loop=0;loop<nloop;loop++){

  if(largestpchi2<pchi2road[loop] && loop <4 )continue;

  if( !LinearFitLoop(positions,pchi2road[loop], a,b,largestpchi2,chi2,ndf, nlayer) )return false;
  dchi2overndf=chi2/ndf-chi2overndfold;//difference of chi2/ndf
  chi2overndfold=chi2/ndf;
  if( m_debuglevel )  m_log << MSG::DEBUG<<"a b largestpchi2 chi2 ndf chi2/ndf dchi2/ndf: "<<a<<" "<<b<<" "<<largestpchi2<<" "<<chi2<<" "<<ndf<<" "<<chi2overndfold << endreq;
  
  }

  std::vector<XYPosition>::iterator it_end=positions.end();
  for(std::vector<XYPosition>::iterator it=positions.begin();
  it!=it_end;
  it++){
  it->usable(true);
  it->used(true);
  it->pchi2(0.);
  }
  */

  //noise reduction for station 1
  // remove wire hits not close to others.
  //NoiseReductionForStation1(wires);

  double pchi2road=9.;
  do{

    if( !LinearFitLoop2(positions,pchi2road, a,b,largest,chi2,ndf, nlayer, nstation) )return false;
    //largest->usable(false);
    dchi2overndf=chi2/ndf-chi2overndfold;//difference of chi2/ndf
    chi2overndfold=chi2/ndf;

    if( m_debuglevel )m_log << MSG::DEBUG << "a b largestpchi2 chi2 ndf chi2/ndf dchi2/ndf: " << a << " " << b << " " << largest->pchi2() << " " << chi2 << " " << ndf <<
      " " << chi2overndfold << endreq; 
    
  }while(largest->pchi2() > pchi2road );
  largest->usable(true);

  wireTrack.a(a);
  wireTrack.b(b);
  wireTrack.chi2(chi2);
  wireTrack.ndf(ndf);
  wireTrack.nLayer(nlayer);
  wireTrack.nStation(nstation);

  //if( m_debuglevel ){
  //   
  //  std::vector<XYPosition>::iterator it_end=positions.end();
  //  for(std::vector<XYPosition>::iterator it=positions.begin();
  //      it!=it_end;
  //      it++){
  //  }
  //}

  return true;
}

bool TGCStandaloneTracksMon::LinearFitLoop(std::vector<XYPosition>& positions,
					   double pchi2road,
					   double& a,
					   double& b,
					   double& largestpchi2,
					   double& chi2,
					   int& ndf,
					   int& nlayer)
{
  if (m_debuglevel) m_log << MSG::DEBUG <<"inside LinearFitLoop" <<endreq;  


  ndf=0;
  double SumX=0.;
  double SumXX=0.;
  double SumXY=0.;
  double SumY=0.;
  double SumYY=0.;
  double SumSS=0.;

  int layer[7]={0,0,0,0,0,0,0};
  nlayer=0;

  std::vector<XYPosition>::iterator it_end=positions.end();
  for(std::vector<XYPosition>::iterator it=positions.begin();
      it!=it_end;
      it++){

    if(it->pchi2() > pchi2road || !it->usable() ){
      it->usable(false);
      it->used(false);
      continue;// pchi2road sigma road
    }

    SumX  += it->X();
    SumXX += it->XX();
    SumXY += it->XY();
    SumY  += it->Y();
    SumYY += it->YY();
    SumSS += it->SS();

    it->used(true);
    layer[it->layer()-1]=1;
    ndf++;
  }

  //ndf should be greater than 0
  ndf-=2;
  if(m_debuglevel) m_log << MSG::DEBUG << "ndf pchi2road : " << ndf << " " << pchi2road << endreq;
  if(ndf<1)return false;
    
  //nlayer should be greater than 2
  for(int i=0;i<7;i++)
    nlayer+=layer[i];
  if(nlayer<3)return false;

  //calculate a and b, (y = a*x + b)
  a = (SumSS*SumXY - SumX*SumY)/(SumSS*SumXX - SumX*SumX);
  b = (SumY-a*SumX)/SumSS;

  //set pchi2
  chi2=0.;
  largestpchi2=0.;
  for(std::vector<XYPosition>::iterator it=positions.begin();
      it!=it_end;
      it++){

    double x=it->x();
    double y=it->y();
    double SS=it->SS();//inverse squared of error for measured y = 1./(sigma*sigma)

    double dy=a*x+b-y;
    double pchi2=dy*dy*SS;
    it->pchi2(pchi2);
    if(dy>=0)it->signchi(-1);
    else it->signchi(1);
               
    if(pchi2 >= pchi2road) it->usable(false);//
    else if(pchi2 < pchi2road) it->usable(true);//

    if(it->used()){
      chi2+=pchi2;//only used hits
      if(pchi2>largestpchi2) largestpchi2=pchi2;
    }

  }

  return true;

}


bool TGCStandaloneTracksMon::LinearFitLoop2(std::vector<XYPosition>& positions,
					    double pchi2road,
					    double& a,
					    double& b,
					    XYPosition*& largest,
					    double& chi2,
					    int& ndf,
					    int& nlayer,
					    int& nstation)
{
  if (m_debuglevel) m_log << MSG::DEBUG <<"inside LinearFitLoop" <<endreq;  


  ndf=0;
  double SumX=0.;
  double SumXX=0.;
  double SumXY=0.;
  double SumY=0.;
  double SumYY=0.;
  double SumSS=0.;

  int layer[7]={0,0,0,0,0,0,0};
  nlayer=0;

  std::vector<XYPosition>::iterator it_end=positions.end();
  for(std::vector<XYPosition>::iterator it=positions.begin();
      it!=it_end;
      it++){
    //if(m_debuglevel) m_log << MSG::DEBUG << "calc sum " << *it << endreq;
    if(it->pchi2() > 500 || !it->usable() ){
      it->usable(false);
      it->used(false);
      continue;// pchi2road sigma road
    }

    SumX  += it->X();
    SumXX += it->XX();
    SumXY += it->XY();
    SumY  += it->Y();
    SumYY += it->YY();
    SumSS += it->SS();

    it->used(true);
    layer[it->layer()-1]++;
    ndf++;
  }

  //ndf should be greater than 0
  //ndf-=2;
  //if(m_debuglevel) m_log << MSG::DEBUG << "ndf pchi2road : " << ndf << " " << pchi2road << endreq;
  //if(ndf<1)return false;
    
  //nlayer should be greater than 2
  for(int i=0;i<7;i++)
    if(layer[i]>0) nlayer++;

  if(nlayer<3)return false;

  //nstation should be greater than 1
  nstation=0;
  if(layer[0]||layer[1]||layer[2])    nstation++;
  if(layer[3]||layer[4])  nstation++;
  if(layer[5]||layer[6])  nstation++;
  if(nstation<2) return false;

  m_log<<MSG::DEBUG<<"nlayerhit "<<layer[0]<<" "<<layer[1]<<" "<<layer[2]<<" "<<layer[3]<<" "<<layer[4]<<" "<<layer[5]<<" "<<layer[6]<<endreq;

  //calculate a and b, (y = a*x + b)
  a = (SumSS*SumXY - SumX*SumY)/(SumSS*SumXX - SumX*SumX);
  b = (SumY-a*SumX)/SumSS;

  //set pchi2
  chi2=0.;
  double largestpchi2=0.;
  largest=0;
  for(std::vector<XYPosition>::iterator it=positions.begin();
      it!=it_end;
      it++){

    double x=it->x();
    double y=it->y();
    double SS=it->SS();//inverse squared of error for measured y = 1./(sigma*sigma)

    double dy=a*x+b-y;
    double pchi2=dy*dy*SS;
    it->pchi2(pchi2);
    if(dy>=0)it->signchi(-1);//measured y value is smaller
    else it->signchi(1);//measured y value is greater
               
    if(pchi2 < pchi2road) it->usable(true);//
    //else if(pchi2 >= pchi2road) it->usable(false);//

    if(it->used()){
      chi2+=pchi2;//only used hits
      if(pchi2>largestpchi2){
        largestpchi2=pchi2;
        largest=&(*it);
        if(m_debuglevel)m_log << MSG::DEBUG << "set largest chi2 hit" << endreq;
      }
    }

  }
  largest->usable(false);
  if(m_debuglevel) m_log << MSG::DEBUG << "set usable flag to false" << endreq;

  return true;

}

void TGCStandaloneTracksMon::checkMDT(const Muon::MdtPrepDataContainer* mdt_hit_container)
{

  //loop over MDT container
  Muon::MdtPrepDataContainer::const_iterator containerIt;
  Muon::MdtPrepDataContainer::const_iterator container_end=mdt_hit_container->end();
  for (containerIt = mdt_hit_container->begin() ; 
       containerIt != container_end ; 
       ++containerIt){ 
    
    // Field           Range               Notes
    // ==============================================================================
    // StationName     unsigned integer    maps to "BIL", "EMS" ,etc.
    // StationEta      [-6,-1]             backward endcap (-1 at lowest R)
    //                 [-8,8]              barrel (increases with Z)
    //                 [-6,-1]+[1,6]       forward endcap (1 at lowest R)
    // StationPhi      [1,8]               increases with phi
    // Technology      [0]                 maps to "MDT"
    // Multilayer      [1,2]               barrel: increases with R
    //                                     endcap: increases with |Z|
    // TubeLayer       [1,4]               barrel: increases with R
    //                                     endcap: increases with |Z|
    // Tube            [1,n]               barrel: increases with |Z|
    //                                     endcap: increases with R
    // ==============================================================================

    //SN     Layer Tube Radial
    //13:EIL 2x4   x54  x4
    //49:EIS 2x4   x36  x2
    //17:EML 2x3   x64  x5
    //18:EMS 2x3   x64  x5
    //20:EOL 2x3   x48  x6
    //21:EOS 2x3   x48  x6

    Identifier mdt_id = (*containerIt)->identify();
    int mdtStationName      =   int(m_mdtIdHelper->stationName(mdt_id)) ;

    if(mdtStationName!=17 && mdtStationName!=18 )continue;

    //int mdtStationEta       =   int(m_mdtIdHelper->stationEta(mdt_id))  ;//backward:[-6,-1], forward:[1,6], (1 or -1 at lowest R)
    int mdtStationPhi       =   int(m_mdtIdHelper->stationPhi(mdt_id))  ;//[1:8]
    //int mdtAC = (mdtStationEta<0);//a:0, c:1

    float mdtSector=mdtStationPhi;
    if(mdtStationName==18)mdtSector+=0.5;
    //double mdtSectorPhi = (mdtSector-1.)*M_PI/4.;

    //loop over MDT PRD Collection
    Muon::MdtPrepDataCollection::const_iterator collection_it_end=(*containerIt)->end();
    
    for(Muon::MdtPrepDataCollection::const_iterator mdtCollection=(*containerIt)->begin();
        mdtCollection!= collection_it_end;
        ++mdtCollection){
      
      Identifier mdt_id2 = (*mdtCollection)->identify();

      //int mdtMultiLayer       =   int(m_mdtIdHelper->multilayer(mdt_id2));
      //int mdtTubeLayer        =   int(m_mdtIdHelper->tubeLayer(mdt_id2));
      //int mdtTube             =   int(m_mdtIdHelper->tube(mdt_id2));
      //int mdtTubeIdForEM = (abs(mdtStationEta)-1)*64 + mdtTube -1;

      int adc = (*mdtCollection)->adc();
      //int tdc = (*mdtCollection)->tdc();

      if(adc < m_MdtAdcCut ) continue;
      
      const MuonGM::MdtReadoutElement*  pReadoutElementMDT = m_muonMgr->getMdtReadoutElement(mdt_id2);
      const Amg::Vector3D mdtgPos = pReadoutElementMDT->tubePos(mdt_id2); //global position of the wire
      //float mdtEta = abs(mdtgPos.eta());
      float mdtPhi = mdtgPos.phi();
      if(mdtPhi<0)mdtPhi+=2*M_PI;

      

    }
  }  
}

double TGCStandaloneTracksMon::MDTz(int ac,
				    int stationName,
				    int multiLayer,
				    int tubeLayer)
{
  double z=( ac==0 ? MDTZpos[stationName-17][multiLayer-1][tubeLayer-1] : -MDTZpos[stationName-17][multiLayer-1][tubeLayer-1]);
  return z;
}


void 
TGCStandaloneTracksMon::NoiseReductionForStation1(std::vector<XYPosition>& positions)
{

  std::vector<XYPosition>::iterator it_end=positions.end();
  for(std::vector<XYPosition>::iterator it=positions.begin();
      it!=it_end;
      it++){

    int layer1=it->layer();
    if( layer1 > 3 )continue;
    it->usable(false);
  }

  for(std::vector<XYPosition>::iterator it=positions.begin();
      it!=it_end;
      it++){

    int layer1=it->layer();
    if( layer1 > 3 )continue;

    double r1=it->y();
    double z1=it->x();
    double er1=it->sigma();

    std::vector<XYPosition>::iterator it2_end=positions.end();
    for(std::vector<XYPosition>::iterator it2=it+1;
        it2!=it2_end;
        it2++){
    
      int layer2=it2->layer();
      if( layer2 > 3 || 
          layer1 == layer2 )continue;

      double r2=it2->y();
      double z2=it2->x();
      double er2=it2->sigma();
      
      double window = (er1+er2)/2.*4.;

      m_log<<MSG::DEBUG<<"layer1 layer2 r1 er1 z1 r2 er2 z2 window "<<layer1<<" "<<layer2<<" "<<r1<<" "<<er1<<" "<<z1<<" "<<r2<<" "<<er2<<" "<<z2<<" "<<window<<endreq;

      m_log<<MSG::DEBUG<<"abs(z1-z2) "<<abs(z1-z2) <<endreq;
      m_log<<MSG::DEBUG<<"abs(r1-r2) < (er1+er2)*7 "<<abs(r1-r2) <<" "<< (er1+er2)*7.<<endreq;

      //position difference should be less than a window : (error sum)*sqrt(12)*2.
      if( abs(z1-z2) < 50. && abs(r1-r2) < (er1+er2)*7. ){
        it->usable(true);
        it2->usable(true);

        m_log<<MSG::DEBUG<<" abs(z1-z2) "<<abs(z1-z2) <<endreq;
        m_log<<MSG::DEBUG<<" abs(r1-r2) < (er1+er2)*7 "<<abs(r1-r2) <<" "<< (er1+er2)*7.<<endreq;

      }
    }
  }

}
