/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     DQTLumiMonTool.cxx
// PACKAGE:  DataQualityTools  
// 
// AUTHORS:  Zoltan Gecse (zgecse@cern.ch)
//           Anadi Canepa (acanepa@cern.ch)
//
// ********************************************************************

#include "DataQualityTools/DQTLumiMonTool.h"

#include "AthenaBaseComps/AthCheckMacros.h"

#include "GaudiKernel/ITHistSvc.h"
#include <vector>
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetIdentifier/PixelID.h"

#include "xAODEventInfo/EventInfo.h"
//##include "Tracking/TrkVertexFitter/TrkVxEdmCnv.h"
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"

using namespace std;
using namespace Trk;

DQTLumiMonTool::DQTLumiMonTool(const std::string & type, 
		   const std::string & name,
		   const IInterface* parent)
  : DataQualityFatherMonTool(type, name, parent),
    m_failedBooking(false),
    m_VertexContainerKey(""),
    m_aveMu_vs_LB(nullptr),
    m_actualMu_vs_LB(nullptr),
    m_actualMu(nullptr),
    m_aveMu(nullptr),
    m_nLooseVtx_vs_LB(nullptr),
    m_nTightVtx_vs_LB(nullptr),
    m_nLooseVtx_perMu_vs_LB(nullptr),
    m_nTightVtx_perMu_vs_LB(nullptr),
    m_nLooseVtx_vs_aveMu(nullptr),
    m_nTightVtx_vs_aveMu(nullptr),
    m_nClustersAll_vs_LB(nullptr),
    m_nClustersECA_vs_LB(nullptr),
    m_nClustersECC_vs_LB(nullptr),
    m_nClustersB0_vs_LB(nullptr),
    m_nClustersB1_vs_LB(nullptr),
    m_nClustersB2_vs_LB(nullptr),
    m_nClustersAll_perMu_vs_LB(nullptr),
    m_nClustersECA_perMu_vs_LB(nullptr),
    m_nClustersECC_perMu_vs_LB(nullptr),
    m_nClustersB0_perMu_vs_LB(nullptr),
    m_nClustersB1_perMu_vs_LB(nullptr),
    m_nClustersB2_perMu_vs_LB(nullptr),
    m_nClustersAll_vs_aveMu(nullptr),
    m_nClustersECA_vs_aveMu(nullptr),
    m_nClustersECC_vs_aveMu(nullptr),
    m_nClustersB0_vs_aveMu(nullptr),
    m_nClustersB1_vs_aveMu(nullptr),
    m_nClustersB2_vs_aveMu(nullptr),
    m_bunchtool("Trig::TrigConfBunchCrossingTool/BunchCrossingTool"),
    m_turnoffbunchtool(false)
{
  declareInterface<IMonitorToolBase>(this);
  declareProperty("histoPath", m_path = "");
  //  declareProperty("VertexContainerKey", m_VertexContainerKey = "VxPrimaryCandidate");
  declareProperty("VertexContainerKey", m_VertexContainerKey = "PrimaryVertices");
  declareProperty("MinNtracksForTight", m_MinNtracksForTight = 4);
  declareProperty("MinTrackWeightForTight", m_MinTrackWeightForTight = 0.01);
  declareProperty("PixelClustersKey", m_PixelClustersKey = "PixelClusters");
  declareProperty("PixelIDKey", m_PixelIDKey = "PixelID");
  declareProperty("bunchCrossingTool",m_bunchtool);
  declareProperty("TurnOffBunchTool",m_turnoffbunchtool);
}

DQTLumiMonTool::~DQTLumiMonTool(){
}

// StatusCode DQTLumiMonTool::bookHistograms( bool /*isNewEventsBlock*/, bool /*isNewLumiBlock*/, bool isNewRun ){ // avoid compilation warnings
StatusCode DQTLumiMonTool::bookHistograms( ){ // avoid compilation warnings
  //if(newRun){
  m_failedBooking = false
    || registerHist(m_path, m_aveMu_vs_LB = new TProfile("aveMu_vs_LB", "Average number of interactions per event;LB;<#mu>_{LB}", 201, -0.5, 200.5), run, ATTRIB_X_VS_LB, "merge").isFailure()
    || registerHist(m_path, m_actualMu_vs_LB = new TProfile("actualMu_vs_LB", "Actual number of interactions per event;LB;#mu_{LB}", 201, -0.5, 200.5), run, ATTRIB_X_VS_LB, "merge").isFailure()

    //Luminosity Histograms  
    || registerHist(m_path, m_nAvgLumi_vs_LB = new TProfile("m_nAvgLumi_vs_LB", "Average Lumi vs LB", 201, -0.5, 200.5), run, ATTRIB_X_VS_LB, "merge").isFailure()
    || registerHist(m_path, m_nAvgIntPerXing_vs_LB = new TProfile("m_nAvgIntPerXing_vs_LB", "Average Mu  vs LB", 201, -0.5, 200.5), run, ATTRIB_X_VS_LB, "merge").isFailure()
    || registerHist(m_path, m_lumiperBCID_vs_LB = new TProfile("m_lumiperBCID_vs_LB", "Instant Luminosity vs LB", 201, -0.5, 200.5), run, ATTRIB_X_VS_LB, "merge").isFailure()
    || registerHist(m_path, m_lbintperxing_vs_LB = new TProfile("m_lbintperxing_vs_LB", "Instantaneous interactions per bunch Xing vs LB", 201, -0.5, 200.5), run, ATTRIB_X_VS_LB, "merge").isFailure()
    || registerHist(m_path, m_duration_vs_LB = new TProfile("m_duration_vs_LB", "Lumi Block time in sec vs LB", 201, -0.5, 200.5), run, ATTRIB_X_VS_LB, "merge").isFailure()
    || registerHist(m_path, m_avglivefrac_vs_LB = new TProfile("m_avglivefrac_vs_LB", "Average live fraction lumi over all BCIDs vs LB", 201, -0.5, 200.5), run, ATTRIB_X_VS_LB, "merge").isFailure()
    || registerHist(m_path, m_fracperBCID_vs_LB = new TProfile("m_fracperBCID_vs_LB", "Current BCID lumi vs LB", 201, -0.5, 200.5), run, ATTRIB_X_VS_LB, "merge").isFailure()
    || registerHist(m_path, m_lumiweight_VS_LB = new TProfile("m_lumiweight_VS_LB", "Current BCID lumi vs LB", 201, -0.5, 200.5), run, ATTRIB_X_VS_LB, "merge").isFailure()
    || registerHist(m_path, m_actualMu = TH1F_LW::create("actualMu", "Actual number of interactions per bunch Xing", 101, -0.5, 100.5),run).isFailure()
    || registerHist(m_path, m_aveMu = TH1F_LW::create("aveMu", "Average number of interactions per bunch Xing", 101, -0.5, 100.5),run).isFailure()


    || registerHist(m_path, m_nLooseVtx_vs_LB = new TProfile("nLooseVtx_vs_LB", "Number of loose vertices per event;LB;<NlooseVtx/event>_{LB}", 200, 0.5, 200.5), run, ATTRIB_X_VS_LB, "merge").isFailure()
    || registerHist(m_path, m_nTightVtx_vs_LB = new TProfile("nTightVtx_vs_LB", "Number of tight vertices per event;LB;<NtightVtx/event>_{LB}", 200, 0.5, 200.5), run, ATTRIB_X_VS_LB, "merge").isFailure()
    || registerHist(m_path, m_nLooseVtx_perMu_vs_LB = new TProfile("nLooseVtx_perMu_vs_LB", "Number of loose vertices per event per Mu;LB;<NlooseVtx/event/#mu>_{LB}", 200, 0.5, 200.5), run, ATTRIB_X_VS_LB, "merge").isFailure()
    || registerHist(m_path, m_nTightVtx_perMu_vs_LB = new TProfile("nTightVtx_perMu_vs_LB", "Number of tight vertices per event per Mu;LB;<NtightVtx/event/#mu>_{LB}", 200, 0.5, 200.5), run, ATTRIB_X_VS_LB, "merge").isFailure()
    || registerHist(m_path, m_nLooseVtx_vs_aveMu = new TProfile("nLooseVtx_vx_aveMu", "Number of loose vertices per event;#mu;NlooseVtx/event", 250, 0, 25)).isFailure()
    || registerHist(m_path, m_nTightVtx_vs_aveMu = new TProfile("nTightVtx_vs_aveMu", "Number of tight vertices per event;#mu;NtightVtx/event", 250, 0, 25)).isFailure();
  m_aveMu_vs_LB->SetCanExtend(TH1::kXaxis);
  if ( m_environment != AthenaMonManager::AOD ) {
    m_failedBooking |= 
      ( registerHist(m_path, m_nClustersAll_vs_LB = new TProfile("nClustersAll_vs_LB", "Number of pixel clusters per event, all;LB;<NclustersAll/event>_{LB}", 200, 0.5, 200.5), run, ATTRIB_X_VS_LB, "merge").isFailure()
	|| registerHist(m_path, m_nClustersECA_vs_LB = new TProfile("nClustersECA_vs_LB", "Number of pixel clusters per event, endcap A;LB;<NlustersECA/event>_{LB}", 200, 0.5, 200.5), run, ATTRIB_X_VS_LB, "merge").isFailure()
	|| registerHist(m_path, m_nClustersECC_vs_LB = new TProfile("nClustersECC_vs_LB", "Number of pixel clusters per event, endcap C;LB;<NclustersECC/event>_{LB}", 200, 0.5, 200.5), run, ATTRIB_X_VS_LB, "merge").isFailure()
	|| registerHist(m_path, m_nClustersB0_vs_LB  = new TProfile("nClustersB0_vs_LB",  "Number of pixel clusters per event, barrel layer 0;LB;<NclustersB0/event>_{LB}", 200, 0.5, 200.5), run, ATTRIB_X_VS_LB, "merge").isFailure()
	|| registerHist(m_path, m_nClustersB1_vs_LB  = new TProfile("nClustersB1_vs_LB",  "Number of pixel clusters per event, barrel layer 1;LB;<NclustersB1/event>_{LB}", 200, 0.5, 200.5), run, ATTRIB_X_VS_LB, "merge").isFailure()
	|| registerHist(m_path, m_nClustersB2_vs_LB  = new TProfile("nClustersB2_vs_LB",  "Number of pixel clusters per event, barrel layer 2;LB;<NclustersB2/event>_{LB}", 200, 0.5, 200.5), run, ATTRIB_X_VS_LB, "merge").isFailure()
	|| registerHist(m_path, m_nClustersAll_perMu_vs_LB = new TProfile("nClustersAll_perMu_vs_LB", "Number of pixel clusters per event per Mu, all;LB;<NclustersAll/event/#mu>_{LB}", 200, 0.5, 200.5), run, ATTRIB_X_VS_LB, "merge").isFailure()
	|| registerHist(m_path, m_nClustersECA_perMu_vs_LB = new TProfile("nClustersECA_perMu_vs_LB", "Number of pixel clusters per event per Mu, endcap A;LB;<NlustersECA/event/#mu>_{LB}", 200, 0.5, 200.5), run, ATTRIB_X_VS_LB, "merge").isFailure()
	|| registerHist(m_path, m_nClustersECC_perMu_vs_LB = new TProfile("nClustersECC_perMu_vs_LB", "Number of pixel clusters per event per Mu, endcap C;LB;<NclustersECC/event/#mu>_{LB}", 200, 0.5, 200.5), run, ATTRIB_X_VS_LB, "merge").isFailure()
	|| registerHist(m_path, m_nClustersB0_perMu_vs_LB  = new TProfile("nClustersB0_perMu_vs_LB",  "Number of pixel clusters per event per Mu, barrel layer 0;LB;<NclustersB0/event/#mu>_{LB}", 200, 0.5, 200.5), run, ATTRIB_X_VS_LB, "merge").isFailure()
	|| registerHist(m_path, m_nClustersB1_perMu_vs_LB  = new TProfile("nClustersB1_perMu_vs_LB",  "Number of pixel clusters per event per Mu, barrel layer 1;LB;<NclustersB1/event/#mu>_{LB}", 200, 0.5, 200.5), run, ATTRIB_X_VS_LB, "merge").isFailure()
	|| registerHist(m_path, m_nClustersB2_perMu_vs_LB  = new TProfile("nClustersB2_perMu_vs_LB",  "Number of pixel clusters per event per Mu, barrel layer 2;LB;<NclustersB2/event/#mu>_{LB}", 200, 0.5, 200.5), run, ATTRIB_X_VS_LB, "merge").isFailure()
	|| registerHist(m_path, m_nClustersAll_vs_aveMu = new TProfile("nClustersAll_vs_aveMu", "Number of pixel clusters per event, all;#mu;NclustersAll/event", 250, 0, 25)).isFailure()
	|| registerHist(m_path, m_nClustersECA_vs_aveMu = new TProfile("nClustersECA_vs_aveMu", "Number of pixel clusters per event, endcap A;#mu;NclustersECA/event", 250, 0, 25)).isFailure()
	|| registerHist(m_path, m_nClustersECC_vs_aveMu = new TProfile("nClustersECC_vs_aveMu", "Number of pixel clusters per event, endcap C;#mu;NclustersECC/event", 250, 0, 25)).isFailure()
	|| registerHist(m_path, m_nClustersB0_vs_aveMu = new TProfile("nClustersB0_vs_aveMu", "Number of pixel clusters per event, barrel layer 0;#mu;NclustersB0/event", 250, 0, 25)).isFailure()
	|| registerHist(m_path, m_nClustersB1_vs_aveMu = new TProfile("nClustersB1_vs_aveMu", "Number of pixel clusters per event, barrel layer 1;#mu;NclustersB1/event", 250, 0, 25)).isFailure()
	|| registerHist(m_path, m_nClustersB2_vs_aveMu = new TProfile("nClustersB2_vs_aveMu", "Number of pixel clusters per event, barrel layer 2;#mu;NclustersB2/event", 250, 0, 25)).isFailure()

	);
  }
  
  if(m_failedBooking){
    msg(MSG::WARNING) << "Could not book all histograms, will not fill any histogram!" << endmsg;
  }
  //}
  return StatusCode::SUCCESS;
}	

bool DQTLumiMonTool::bookDQTLumiMonTool(){
  return true;
}

StatusCode DQTLumiMonTool::fillHistograms(){
  if(m_failedBooking) return StatusCode::SUCCESS;
  
  CHECK(m_bunchtool.retrieve());
  const xAOD::EventInfo* thisEventInfo;
  StatusCode sc = evtStore()->retrieve(thisEventInfo);
  if(sc.isFailure()  || !thisEventInfo){
    msg(MSG::WARNING) << "Could not retreive EventInfo from evtStore" << endmsg;
    return StatusCode::SUCCESS;
  }

  unsigned LB = thisEventInfo->lumiBlock();
  double aveMu = thisEventInfo->averageInteractionsPerCrossing();
  double actualMu = thisEventInfo->actualInteractionsPerCrossing();
  unsigned int bcid = thisEventInfo->bcid();

  //Read from Lumi API's                                                                                                                                                                    
  double lbavglumi_h = lbAverageLuminosity();
  double avgintperxing = lbAverageInteractionsPerCrossing();
  double lumiperBCID = lbLuminosityPerBCID();
  double lbintperxing_h = lbInteractionsPerCrossing();

  double duration_h = lbDuration();
  double avglivefrac_h = lbAverageLivefraction();
  double fracperBCID_h = livefractionPerBCID();
  double lumiweight_h = lbLumiWeight();

  m_aveMu_vs_LB->Fill(LB, aveMu);
  m_aveMu->Fill(aveMu);

  if(m_turnoffbunchtool) {
    m_actualMu_vs_LB->Fill(LB, actualMu);
    m_actualMu->Fill(actualMu);
  }
  else if (m_bunchtool->isFilled(bcid))
    {
      m_actualMu_vs_LB->Fill(LB, actualMu);
      m_actualMu->Fill(actualMu);
    }


  m_nAvgLumi_vs_LB->Fill(LB,lbavglumi_h);
  m_nAvgIntPerXing_vs_LB->Fill(LB,avgintperxing);
  m_avglivefrac_vs_LB->Fill(LB,avglivefrac_h);
  m_duration_vs_LB->Fill(LB,duration_h);
  m_lbintperxing_vs_LB->Fill(LB,lbintperxing_h);
  m_lumiperBCID_vs_LB->Fill(LB,lumiperBCID);
  m_fracperBCID_vs_LB->Fill(LB,fracperBCID_h);
  m_lumiweight_VS_LB->Fill(LB,lumiweight_h);
    


  // Get vertex related info
  const xAOD::VertexContainer* vertices(0);
  if(evtStore()->retrieve(vertices, m_VertexContainerKey).isFailure() || !vertices){
    msg(MSG::WARNING) << "Could not retrieve " <<m_VertexContainerKey<< " from evtStore" << endmsg;
  }else{
    int nVtxLoose = 0;
    int nVtxTight = 0;
    //last vertex is dummy
    for(xAOD::VertexContainer::const_iterator vi=vertices->begin(); vi!=vertices->end()-1; vi++){
      if((*vi)==0) continue;
      if (! ((*vi)->vxTrackAtVertexAvailable())) continue;
      int nGoodTracks=0;
      for(vector<Trk::VxTrackAtVertex>::const_iterator ti=(*vi)->vxTrackAtVertex().begin(); ti!=(*vi)->vxTrackAtVertex().end(); ti++){
        if((*ti).weight() < m_MinTrackWeightForTight) continue;
        nGoodTracks++;
      }
      if(nGoodTracks >= m_MinNtracksForTight) nVtxTight++;
      nVtxLoose++;
    }
    m_nLooseVtx_vs_LB->Fill(LB, nVtxLoose);
    m_nTightVtx_vs_LB->Fill(LB, nVtxTight);
    if(aveMu>0) m_nLooseVtx_perMu_vs_LB->Fill(LB, nVtxLoose/aveMu);
    if(aveMu>0) m_nTightVtx_perMu_vs_LB->Fill(LB, nVtxTight/aveMu);
    m_nLooseVtx_vs_aveMu->Fill(aveMu, nVtxLoose);
    m_nTightVtx_vs_aveMu->Fill(aveMu, nVtxTight);
  }
  
  // Get pixels related info
  if ( m_environment != AthenaMonManager::AOD ) {
    const InDet::PixelClusterContainer* pixelClContainer = 0;
    const PixelID* pixelId = 0;
    if(evtStore()->retrieve(pixelClContainer, m_PixelClustersKey).isFailure() || !pixelClContainer){
      msg(MSG::WARNING) << "Could not retrieve " <<m_PixelClustersKey<< " from evtStore" << endmsg;
    }else if(m_detStore->retrieve(pixelId, m_PixelIDKey).isFailure() || !pixelId){
      msg(MSG::WARNING) << "Could not retrieve " <<m_PixelIDKey<< " from DetStore" << endmsg;
    }else{
      int nClustersAll = 0;
      int nClustersECA = 0;
      int nClustersECC = 0;
      int nClustersB0 = 0;
      int nClustersB1 = 0;
      int nClustersB2 = 0;
      for(InDet::PixelClusterContainer::const_iterator colNext=pixelClContainer->begin(); colNext!=pixelClContainer->end(); ++colNext){
	const InDet::PixelClusterCollection* ClusterCollection(*colNext);
	if (!ClusterCollection) continue;
	for(DataVector<InDet::PixelCluster>::const_iterator p_clus=ClusterCollection->begin(); p_clus!=ClusterCollection->end(); ++p_clus){
	  Identifier clusID=(*p_clus)->identify();
	  nClustersAll++;
	  if(pixelId->barrel_ec(clusID)==2 ) nClustersECA++; 
	  if(pixelId->barrel_ec(clusID)==-2) nClustersECC++; 
	  if(pixelId->barrel_ec(clusID)==0) {
	    if(pixelId->layer_disk(clusID)==0) nClustersB0++;
	    if(pixelId->layer_disk(clusID)==1) nClustersB1++;
	    if(pixelId->layer_disk(clusID)==2) nClustersB2++; 
	  }
	}
      }
      m_nClustersAll_vs_LB->Fill(LB, nClustersAll); 
      m_nClustersECA_vs_LB->Fill(LB, nClustersECA); 
      m_nClustersECC_vs_LB->Fill(LB, nClustersECC); 
      m_nClustersB0_vs_LB->Fill(LB, nClustersB0); 
      m_nClustersB1_vs_LB->Fill(LB, nClustersB1); 
      m_nClustersB2_vs_LB->Fill(LB, nClustersB2);  
      if(aveMu>0) m_nClustersAll_perMu_vs_LB->Fill(LB, nClustersAll/aveMu);
      if(aveMu>0) m_nClustersECA_perMu_vs_LB->Fill(LB, nClustersECA/aveMu);
      if(aveMu>0) m_nClustersECC_perMu_vs_LB->Fill(LB, nClustersECC/aveMu);
      if(aveMu>0) m_nClustersB0_perMu_vs_LB->Fill(LB, nClustersB0/aveMu);
      if(aveMu>0) m_nClustersB1_perMu_vs_LB->Fill(LB, nClustersB1/aveMu);
      if(aveMu>0) m_nClustersB2_perMu_vs_LB->Fill(LB, nClustersB2/aveMu);
      m_nClustersAll_vs_aveMu->Fill(aveMu, nClustersAll);
      m_nClustersECA_vs_aveMu->Fill(aveMu, nClustersECA);
      m_nClustersECC_vs_aveMu->Fill(aveMu, nClustersECC);
      m_nClustersB0_vs_aveMu->Fill(aveMu, nClustersB0);
      m_nClustersB1_vs_aveMu->Fill(aveMu, nClustersB1);
      m_nClustersB2_vs_aveMu->Fill(aveMu, nClustersB2);
    }
  }

  return StatusCode::SUCCESS;
}

