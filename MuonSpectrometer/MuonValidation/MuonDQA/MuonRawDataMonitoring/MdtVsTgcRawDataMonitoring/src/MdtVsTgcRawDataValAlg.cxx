/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Package : MdtVsTgcRawDataValAlg
// Authors:  A. Ishikawa(Kobe), M.King(Kobe)
// Jun. 2008`
//
// DESCRIPTION:
// Subject: correlation btw MDT hits vs TGC RoI -->Offline Muon Data Quality
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ToolHandle.h"
 
#include "MuonRDO/TgcRdo.h"
#include "MuonRDO/TgcRdoContainer.h"
#include "MuonRDO/TgcRdoIdHash.h"

// MuonDetDesc
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/TgcReadoutParams.h"

#include "MuonDQAUtils/MuonChamberNameConverter.h"
#include "MuonDQAUtils/MuonChambersRange.h"
#include "MuonDQAUtils/MuonCosmicSetup.h"
#include "MuonDQAUtils/MuonDQAHistMap.h" 

#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"

#include "TrkSegment/SegmentCollection.h"
 
#include "Identifier/Identifier.h"

//mdt stuff
#include "MuonCalibIdentifier/MuonFixedId.h"
 
#include "MdtVsTgcRawDataMonitoring/MdtVsTgcRawDataValAlg.h"
#include "AthenaMonitoring/AthenaMonManager.h"

//xAOD MDT segment container
#include "xAODMuon/MuonSegmentContainer.h"

#include <TH1F.h>
#include <TH2F.h>
#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TF1.h>
#include <inttypes.h> 

#include <sstream>
#include <algorithm>
#include <fstream>

using namespace std;

static const int maxColl =   1200;
static const int maxPRD  =   50000;
static const int maxClus =   1000;
 
/////////////////////////////////////////////////////////////////////////////

MdtVsTgcRawDataValAlg::MdtVsTgcRawDataValAlg( const std::string & type, const std::string & name, const IInterface* parent )
  :ManagedMonitorToolBase( type, name, parent ),
   m_log( msgSvc(), name ),
   m_debuglevel(false){
  // Declare the properties 
  declareProperty("CheckCabling",     m_checkCabling=false);
  declareProperty("TgcLv1File",       m_tgclv1file=true);    
  declareProperty("UseNewMDTSegment", m_new_MDTSG=true);    
  declareProperty("ChamberName",      m_chamberName="XXX");
  declareProperty("StationSize",      m_StationSize="XXX");
  declareProperty("Sector",           m_sector=0); 
  declareProperty("Side",             m_side=0); 
  declareProperty("LastEvent",        m_lastEvent=0);
  declareProperty("CosmicStation",    m_cosmicStation=0);
  declareProperty("MdtAdcCut",        m_MdtAdcCut=50);
  declareProperty("MdtTdcCut",        m_MdtTdcCut=1600);
  declareProperty("TgcPrepDataContainer", m_tgc_PrepDataContainerName = "TGC_Measurements");
  declareProperty("OutputCoinCollection", m_tgc_CoinContainerName     = "TrigT1CoinDataCollection" );
  declareProperty("MdtPrepDataContainer", m_mdt_PrepDataContainerName = "MDT_DriftCircles");
  declareProperty("MdtSegmentCollection", m_mdt_SegmentCollectionName = "MuonSegments");
  
  // initialize class members
  m_eventStore = 0;
  m_activeStore = 0;
  m_muonMgr = 0;
  m_mdtIdHelper = 0;
  m_tgcIdHelper = 0;
  theSL = 0;
  SLr = 0;
  SLz = 0;
  SLeta = 0;
  SLphi = 0;
   
  for(int ac=0; ac<2; ac++){
	mvt_cutspassed[ac] = 0;
	for(int jMDT=0; jMDT<4; jMDT++){
		mdt_segmmap[ac][jMDT] = 0;
		for(int sMDT=0; sMDT<4; sMDT++){
			mdt_segmposdirsag[ac][jMDT][sMDT] = 0;
			for(int iREPT=0; iREPT<4; iREPT++){
				mdt_segmmatchsag[ac][jMDT][sMDT][iREPT] = 0;
				mdt_trackdirdirsag[ac][jMDT][sMDT][iREPT] = 0;
				mdt_trackchecksag[ac][jMDT][sMDT][iREPT][0] = 0;
				mdt_trackchecksag[ac][jMDT][sMDT][iREPT][1] = 0;
			}
		}
	}
	for(int WS=0; WS<2; WS++){
		for(int EffNDE=0; EffNDE<4; EffNDE++){
			eff_stationmapbase[ac][WS][EffNDE] = 0;
			eff_stationmapmid[ac][WS][EffNDE] = 0;
			eff_stationmap[ac][WS][EffNDE] = 0;
		}
	}
  }

  // Initialize to zero
  for(int i=0;i<2;i++)// AC
    for(int jTGC=0;jTGC<4;jTGC++)// TGC Station
      for(int f=0;f<2;f++)// FE
        for(int k=0;k<2;k++)// WireStrip
          for(int x=0;x<4;x++){
            mvt_extrprdsag[i][jTGC][f][k][x]=0;
            mvt_extrprdsag2[i][jTGC][f][k][x]=0;
          }
  for(int k=0;k<2;k++)
    for(int i=0;i<2;i++)
      for(int x=0;x<4;x++){
        tgc_prdcompsag[i][k][x]=0;
      }

}

MdtVsTgcRawDataValAlg::~MdtVsTgcRawDataValAlg(){
  m_log << MSG::INFO << " deleting MdtVsTgcRawDataValAlg " << endreq;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode 
MdtVsTgcRawDataValAlg::initialize(){
  // init message stream
  m_log.setLevel(outputLevel());                // individual outputlevel not known before initialise
  m_debuglevel = (m_log.level() <= MSG::DEBUG); // save if threshold for debug printout reached
  
  m_log << MSG::INFO << "in initializing MdtVsTgcRawDataValAlg" << endreq;
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

  // Initialize the Detector Store
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
  if (sc.isFailure()) {
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

  prepareTREarray();
   
  return StatusCode::SUCCESS;
}


/*----------------------------------------------------------------------------------*/
StatusCode MdtVsTgcRawDataValAlg::bookHistogramsRecurrent(){
/*----------------------------------------------------------------------------------*/
  if (m_debuglevel) m_log << MSG::DEBUG << "TGC RawData Monitoring Histograms being booked" << endreq;
  
  StatusCode sc = StatusCode::SUCCESS; 
  
  //declare a group of histograms
  std::string m_generic_path_mdtvstgclv1 = "Muon/MuonRawDataMonitoring/MDTvsTGC";
  //MonGroup mdtvstgclv1_expert( this, m_generic_path_mdtvstgclv1+"/Global", expert, run );
  MonGroup mdtvstgclv1_expert_a( this, m_generic_path_mdtvstgclv1+"/TGCEA", run, ATTRIB_UNMANAGED );
  MonGroup mdtvstgclv1_expert_c( this, m_generic_path_mdtvstgclv1+"/TGCEC", run, ATTRIB_UNMANAGED );
  
  if(newEventsBlock){}
  if(newLumiBlock){}
  if(newRun){
    m_log << MSG::INFO << "MdtVsTgc RawData Monitoring : begin of run" << endreq;
    
    sc = bookmaphists(mdtvstgclv1_expert_a,
                      mdtvstgclv1_expert_c);
    if(sc.isFailure())return sc;
    sc = bookeffhists(mdtvstgclv1_expert_a,
                      mdtvstgclv1_expert_c);
    if(sc.isFailure())return sc;
  }//isNewRun
  return sc;
}


/*----------------------------------------------------------------------------------*/
StatusCode MdtVsTgcRawDataValAlg::fillHistograms(){
/*----------------------------------------------------------------------------------*/
  StatusCode sc = StatusCode::SUCCESS; 
  if (m_debuglevel) m_log << MSG::DEBUG << "MdtVsTgcRawDataValAlg::TGC RawData Monitoring Histograms being filled" << endreq;

  //TGC PRD
  const Muon::TgcPrepDataContainer* tgc_prd_container;
  sc = (*m_activeStore)->retrieve(tgc_prd_container, m_tgc_PrepDataContainerName);
  if (sc.isFailure()) {
    m_log << MSG::ERROR << " Cannot retrieve TgcPrepDataContainer " << "m_tgc_PrepDataContainerName" << endreq;
    return sc;
  }
  
  //TGC Coincidence
  const Muon::TgcCoinDataContainer* tgc_coin_container;
  sc = (*m_activeStore)->retrieve(tgc_coin_container, m_tgc_CoinContainerName);
  if (sc.isFailure()) {
    m_log << MSG::ERROR << " Cannot retrieve TgcCoinDataContainer " << "m_tgc_CoinContainerName" << endreq;
    return sc;
  }

  if (m_debuglevel) m_log << MSG::DEBUG << "size of tgc container is " << tgc_coin_container -> size() << endreq;
  
  //MDT PRD
  const Muon::MdtPrepDataContainer* mdt_prd_container;
  sc = (*m_activeStore)->retrieve(mdt_prd_container, m_mdt_PrepDataContainerName);
  if (sc.isFailure()) {
    m_log << MSG::ERROR << " Cannot retrieve MdtPrepDataContainer " << m_mdt_PrepDataContainerName << endreq;
    return sc;
  }
  
  if(m_new_MDTSG){ //new MDT Segments container
    const xAOD::MuonSegmentContainer* mdt_new_segment ;
    sc = (*m_activeStore)->retrieve(mdt_new_segment, m_mdt_SegmentCollectionName);
    if (sc.isFailure()) {
      m_log << MSG::ERROR << " Cannot retrieve New Mdt SegmentCollection " << m_mdt_SegmentCollectionName << endreq;
      return sc;
    }
  tgceffcalc(mdt_new_segment, tgc_prd_container);
  maphists(mdt_new_segment, tgc_prd_container);

  }else{//MDT Segments
    const Trk::SegmentCollection* mdt_segment_collection;
    sc = (*m_activeStore)->retrieve(mdt_segment_collection, m_mdt_SegmentCollectionName);
    if (sc.isFailure()) {
      m_log << MSG::ERROR << " Cannot retrieve Mdt SegmentCollection " << m_mdt_SegmentCollectionName << endreq;
      return sc;
    }
    tgceffcalc(mdt_segment_collection, tgc_prd_container);
    maphists(mdt_segment_collection, tgc_prd_container);
  }

  //only analyze nSL==1
  int nSL = numberOfSL(tgc_coin_container);
  //mdtvstgclv1_eff[0]->Fill(0);
  //mdtvstgclv1_eff[1]->Fill(0);

  if(nSL==1){
    //declare a group of histograms
    //std::string m_generic_path_tgclv1 = "Muon/MuonRawDataMonitoring/TGC";
    //MonGroup tgclv1_expert( this, m_generic_path_tgclv1+"/Overview", expert, run );
    
    //TH1* testptr0 = tgclv1roietavsphi[0];
    //sc = tgclv1_expert.getHist(testptr0,"RoI_Eta_Vs_Phi_A");
    //tgclv1roietavsphi[0] = dynamic_cast<TH2*>(testptr0);
    //if(sc.isFailure() ) m_log << MSG::WARNING << "couldn't get tgclv1roietavsphi[0] hist to MonGroup" << endreq;
    //m_log<<MSG::INFO <<"RoI_Eta_Vs_Phi_A_Side has been got"<<endreq;
    
    //fill MDT hit vs TGC RoI
    correlation(mdt_prd_container, tgc_coin_container);
  }

 
  return sc;  // statuscode check
} 
/*----------------------------------------------------------------------------------*/
StatusCode MdtVsTgcRawDataValAlg::procHistograms(){
/*----------------------------------------------------------------------------------*/
 
  if (m_debuglevel) m_log << MSG::DEBUG << "MdtVsTgcRawDataValAlg finalize()" << endreq;
  if(endOfEventsBlock){}
  if(endOfLumiBlock){}
  if(endOfRun){
    tgceffcalcfinalize();
    maphistsfinalize();
  }
  return StatusCode::SUCCESS;
}
