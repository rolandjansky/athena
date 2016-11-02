/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTK_RawDataAlgs/FTK_RDO_ReaderAlgo.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/ITHistSvc.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/TriggerInfo.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "TrkEventPrimitives/VertexType.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "xAODTruth/TruthVertexContainer.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"

#include "TMath.h"
#define NINT(a) ((a) >= 0.0 ? (int)((a)+0.5) : (int)((a)-0.5))

using namespace std;



/////////////////////////////////////////////////////////////////////////////
FTK_RDO_ReaderAlgo::FTK_RDO_ReaderAlgo(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),

  m_getRawTracks(true),
  m_verticesKey("TruthVertices"),
  m_ftk_raw_trackcollection_Name("FTK_RDO_Tracks"),
  m_getTracks(true), 
  m_getRefitTracks(true), 
  m_getOfflineTracks(true), 
  m_getTrackParticles(true), 
  m_getRefitTrackParticles(true),
  m_getRawVertex_Fast(false), 
  m_getConvertedVertex(true), 
  m_getRefitVertex_Fast(false), 
  m_getRefitVertex(true),
  m_getOfflineVertex(true),
  m_getTruthVertex(true),

  m_DataProviderSvc("IFTK_DataProviderSvc/IFTK_DataProviderSvc", name),
  m_fillHists(true),
  m_fillTree(true)

{
  declareProperty("RDO_CollectionName",m_ftk_raw_trackcollection_Name, "Collection name of RDO");
  declareProperty("GetRawTracks",m_getRawTracks);
  declareProperty("GetTracks",m_getTracks);
  declareProperty("GetRefitTracks",m_getRefitTracks);
  declareProperty("GetOfflineTracks",m_getOfflineTracks);
  declareProperty("GetTrackParticles",m_getTrackParticles);
  declareProperty("GetRefitTrackParticles",m_getRefitTrackParticles=true);
  declareProperty("GetRawVertex_Fast",m_getRawVertex_Fast);
  declareProperty("GetVertex",m_getConvertedVertex);
  declareProperty("GetRefitVertex_Fast",m_getRefitVertex_Fast);
  declareProperty("GetRefitVertex",m_getRefitVertex);
  declareProperty("GetOfflineVertex_Offline",m_getOfflineVertex);
  declareProperty("GetTruthVertex",m_getTruthVertex);

  declareProperty("FTK_DataProvider", m_DataProviderSvc);
  declareProperty("fillHists", m_fillHists);
  declareProperty("fillTree", m_fillTree);

  declareProperty ("VerticesKey",  m_verticesKey = "TruthVertices", "TruthVertex container name");

}

FTK_RDO_ReaderAlgo::~FTK_RDO_ReaderAlgo()
{ }


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *


StatusCode FTK_RDO_ReaderAlgo::initialize(){


  MsgStream athlog(msgSvc(), name());
  ATH_MSG_INFO("FTK_RDO_ReaderAlgo::initialize()" );

  //FIX THESE
  ATH_CHECK( service("StoreGateSvc", m_StoreGate ));
  ATH_MSG_INFO("Input RDO collection" << m_ftk_raw_trackcollection_Name);
  if (m_getRawTracks) ATH_MSG_INFO("Getting FTK_RawTracks from Storegate");
  if (m_getTracks) ATH_MSG_INFO("Getting FTK Trk::Tracks from "<< m_DataProviderSvc);
  if (m_getRefitTracks) ATH_MSG_INFO("Getting FTK Refit Trk::Tracks from "<< m_DataProviderSvc);
  if (m_getTrackParticles) ATH_MSG_INFO("Getting FTK xAOD::TrackParticles from "<< m_DataProviderSvc);
  if (m_getRefitTrackParticles) ATH_MSG_INFO("Getting FTK Refit xAOD::TrackParticles from "<< m_DataProviderSvc);
  if (m_getRawVertex_Fast) ATH_MSG_INFO("Getting FTK_RawTrack Vertex from "<< m_DataProviderSvc);
  if (m_getConvertedVertex) ATH_MSG_INFO("Getting FTK Vertex from "<< m_DataProviderSvc);
  if (m_getRefitVertex_Fast) ATH_MSG_INFO("Getting FTK Refit Vertex (fast vertex tool) from "<< m_DataProviderSvc);
  if (m_getRefitVertex) ATH_MSG_INFO("Getting FTK Refit Vertex (offline vertex tool) from "<< m_DataProviderSvc);
  if (m_getOfflineVertex) ATH_MSG_INFO("Getting Offline Vertex");


  //Set up the FTK Data Provider SVC //
  ATH_CHECK(m_DataProviderSvc.retrieve());

  
  std::vector<TH1D*> *histograms = new std::vector<TH1D*> ;

    /// Get Histogram Service ///
    ATH_CHECK(service("THistSvc", rootHistSvc));

  if (m_fillHists) {
    //Book Histograms//
    Hist_Init(histograms);
  }


  ///Tree
  std::string trackRootPath;
  if(m_fillTree){ 

    Tree_ftk = new TTree("ftk_data", "ftk_data");
    Tree_Init();
    
    //Register Tree                                                                                   
    trackRootPath = "/TREE/";
    if (rootHistSvc->regTree("/TREE/",Tree_ftk).isFailure())
      {
	std::string temp = "Failed to book Tree";
	ATH_MSG_WARNING(temp);
      }
    else
      {
	std::string temp = "Booked Tree: ";
	ATH_MSG_INFO(temp);
      }
  }
  

  if(m_fillHists){ 
    trackRootPath = "/TRACKS/";
    for( uint i = 0; i < histograms->size(); ++i){
      if ( rootHistSvc->regHist( trackRootPath + (*histograms)[i]->GetName(), (*histograms)[i] ).isFailure()) {
	std::string temp = "Failed to book Histogram: " + std::string((*histograms)[i]->GetName());
	ATH_MSG_WARNING(temp);
      } else {
	std::string temp = "Booked Histogram: " + std::string((*histograms)[i]->GetName());
	ATH_MSG_INFO(temp);
      }
    }
  }
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
/*
StatusCode FTK_RDO_ReaderAlgo::execute() {
  MsgStream athlog(msgSvc(), name());
  ATH_MSG_DEBUG("FTK_RDO_ReaderAlgo::execute() start");
*/

StatusCode FTK_RDO_ReaderAlgo::execute() {
  MsgStream athlog(msgSvc(), name());
  ATH_MSG_DEBUG("FTK_RDO_ReaderAlgo::execute() start");


  const EventInfo* eventInfo;
  StatusCode scEv = evtStore()->retrieve(eventInfo);
  if (scEv.isFailure()) {
        ATH_MSG_ERROR( "Could not retrieve event info" );
  }
  int eventNumber = eventInfo->event_ID()->event_number();
  ATH_MSG_DEBUG(" Event " << eventNumber);

  const EventID* eventID( eventInfo->event_ID());
  ATH_MSG_DEBUG( "entered execution for run" << eventID->run_number()
		 << "event" << eventID->event_number());
  
  const TriggerInfo *triggerInfo(eventInfo->trigger_info());
  
  RunNumber = eventID->run_number();
  EventNumber = eventID->event_number();
  LumiBlock = eventID->lumi_block();
  BCID = eventID->bunch_crossing_id();
  averageInteractionsPerCrossing = eventInfo->averageInteractionsPerCrossing();
  actualInteractionsPerCrossing = eventInfo->actualInteractionsPerCrossing();
  extendedLevel1ID = triggerInfo->extendedLevel1ID();
  level1TriggerType = triggerInfo->level1TriggerType();
  std::vector<unsigned int> info = triggerInfo->level1TriggerInfo();

  for (unsigned int i = 0; i < info.size(); i++) level1TriggerInfo.push_back(info[i]);

  unsigned int track_requirement = 3;

  // Fills Track Variables
  if (m_getRawTracks)  Fill_Raw_Tracks();
  if (m_getTracks)  Fill_Converted_Tracks();
  if (m_getRefitTracks) Fill_Refit_Tracks();
  if (m_getOfflineTracks) Fill_Offline_Tracks();

  // Fills Vertex Variables, Fast Algorithm
  if (m_getRawVertex_Fast) Fill_Raw_Vertices_fast(track_requirement);
  if (m_getRefitVertex_Fast) Fill_Refit_Vertices_fast(track_requirement);

  // Fills Vertex Variables, Offline Algorithm
  if (m_getConvertedVertex) Fill_Converted_Vertices(track_requirement);
  if (m_getRefitVertex) Fill_Refit_Vertices(track_requirement);
  if (m_getOfflineVertex) Fill_Offline_Vertices(track_requirement);
 
  // Get Truth Vertices
  if (m_getTruthVertex) Fill_Truth_Vtx();

  // Get TrackParticles from Converted Tracks
  //
  if (m_getTrackParticles) {
    ATH_MSG_DEBUG( " Getting TrackParticles from converted tracks from DataProviderSvc" );
    xAOD::TrackParticleContainer *tpc  = m_DataProviderSvc->getTrackParticles(false);
    ATH_MSG_DEBUG( "DataProviderSvc returned " <<  tpc->size() << " TrackParticles created from converted tracks" );

    //       if ( evtStore()->get ( tpc, m_TrackParticleCollectionName).isFailure() ) {
    //	 ATH_MSG_DEBUG( "Failed to get FTK TrackParticleCollection with name " <<  m_TrackParticleCollectionName );
    //       }

    ATH_MSG_VERBOSE( " " );
    ATH_MSG_VERBOSE( " Printing information for " <<  tpc->size()<< " TrackParticles" );

    int itpc = 0;
    for ( auto ptp =  tpc->begin(); ptp !=  tpc->end(); ptp++, itpc++) {
      //uint8_t NumOfPixHits = 0;
      //uint8_t NumOfSCTHits = 0;
      //if (!(*ptp)->summaryValue(NumOfSCTHits, xAOD::numberOfSCTHits) ) ATH_MSG_DEBUG( "Could not retrieve number of SCT hits");
      //if (!(*ptp)->summaryValue(NumOfPixHits, xAOD::numberOfPixelHits) ) ATH_MSG_DEBUG( "Could not retrieve number of Pixel hits");

      double p = 1e10;
      if (fabs((*ptp)->qOverP()) >=1e-9) p = 1./(*ptp)->qOverP();
      ATH_MSG_VERBOSE( "itp:" << itpc << ": q*pT " << (*ptp)->pt()*(*ptp)->charge() << " eta " << (*ptp)->eta() << " phi " << (*ptp)->phi0() <<
          " d0: " << (*ptp)->d0() << " z0: " << (*ptp)->z0() << " p " << p );
    }
    delete (tpc);
  }


  //
  // Get TrackParticles from refitted tracks
  //
  if (m_getRefitTrackParticles) {
    ATH_MSG_DEBUG( " Getting TrackParticles from refitted tracks from DataProviderSvc" );
    xAOD::TrackParticleContainer *tpr  = m_DataProviderSvc->getTrackParticles(true);
    ATH_MSG_DEBUG( "DataProviderSvc returned " <<  tpr->size() << " TrackParticles created from refitted tracks" );
    //       if ( evtStore()->get ( tpr, m_refitTrackParticleCollectionName).isFailure() ) {
    //	 ATH_MSG_DEBUG( "Failed to get refit FTK TrackParticleCollection with name " <<  m_refitTrackParticleCollectionName );
    //       }

    ATH_MSG_VERBOSE( " " );
    ATH_MSG_VERBOSE( " Printing information for " <<  tpr->size()<< " refitted TrackParticles" );

    int itpr=0;
    for ( auto ptpr =  tpr->begin(); ptpr !=  tpr->end(); ptpr++, itpr++) {
      double p = 1.e10;
      if ((*ptpr)->qOverP()!=0.) p = 1./(*ptpr)->qOverP();
      //uint8_t NumOfPixHits = 0;
      //uint8_t NumOfSCTHits = 0;
      //if (!(*ptpr)->summaryValue(NumOfSCTHits, xAOD::numberOfSCTHits) ) athlog << MSG::DEBUG << "Could not retrieve number of SCT hits");
      //if (!(*ptpr)->summaryValue(NumOfPixHits, xAOD::numberOfPixelHits) ) athlog << MSG::DEBUG << "Could not retrieve number of Pixel hits");
      ATH_MSG_VERBOSE( itpr << ": q*pT " << (*ptpr)->pt()*(*ptpr)->charge() << " eta " << (*ptpr)->eta() << " phi " << (*ptpr)->phi0() <<
          " d0: " << (*ptpr)->d0() << " z0: " << " q*p: " << p << (*ptpr)->z0() );
    }
    delete (tpr);
  }


   if(m_fillTree)  Tree_ftk->Fill();
  // Reset the ftk cache for this event //
  //scFTK = m_DataProviderSvc->endEvent();
  //if( scFTK.isFailure()){
  //  ATH_MSG_VERBOSE( "Cache Clear " );
  //}
  ATH_MSG_DEBUG( "FTK_RDO_ReaderAlgo::execute() end" );

  return StatusCode::SUCCESS;

}
 


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode FTK_RDO_ReaderAlgo::finalize() {
  MsgStream athlog(msgSvc(), name());
  ATH_MSG_INFO("finalize()" );

  return StatusCode::SUCCESS;
}


void FTK_RDO_ReaderAlgo::Hist_Init(std::vector<TH1D*> *histograms){
  double PI = TMath::Pi();

    // FTK Raw Track Histograms //
    h_FTK_RawTrack_n    = new TH1D("h_FTK_RawTrack_n",    ";FTK Raw Track multiplicity;Tracks",      1000,  0.,    1000.);
    h_FTK_RawTrack_phi  = new TH1D("h_FTK_RawTrack_phi",  ";FTK Raw Track #phi [rad];Tracks",         100, -PI,    PI);
    h_FTK_RawTrack_d0   = new TH1D("h_FTK_RawTrack_d0",   ";FTK Raw Track d_{0} [mm];Tracks",         100, -10.,   10.);
    h_FTK_RawTrack_z0   = new TH1D("h_FTK_RawTrack_z0",   ";FTK Raw Track z_{0} [mm];Tracks",         500, -50.,   50.);
    h_FTK_RawTrack_invPt = new TH1D("h_FTK_RawTrack_invPt", ";FTK Raw Track 1/p_{T} [GeV^{-1}];Tracks", 100,   0.,   0.1);
    h_FTK_RawTrack_cot  = new TH1D("h_FTK_RawTrack_cot",  ";FTK Raw Track cot(#theta); Tracks",       100,  -PI,   PI);
    h_FTK_RawTrack_eta  = new TH1D("h_FTK_RawTrack_eta",  ";FTK Raw Track #eta; Tracks",              100, -3.0,   3.0);
    h_FTK_RawTrack_nPix = new TH1D("h_FTK_RawTrack_nPix", ";FTK Raw Track Pixel Cluster Multiplicity; Events", 10, 0.,   10.);
    h_FTK_RawTrack_nSCT = new TH1D("h_FTK_RawTrack_nSCT", ";FTK Raw Track SCT Cluster Multiplicity; Events",   10, 0.,   10.);

    histograms->push_back(h_FTK_RawTrack_n);
    histograms->push_back(h_FTK_RawTrack_phi);
    histograms->push_back(h_FTK_RawTrack_d0);
    histograms->push_back(h_FTK_RawTrack_z0);
    histograms->push_back(h_FTK_RawTrack_invPt);
    histograms->push_back(h_FTK_RawTrack_cot);
    histograms->push_back(h_FTK_RawTrack_eta);
    histograms->push_back(h_FTK_RawTrack_nPix);
    histograms->push_back(h_FTK_RawTrack_nSCT);

    // Trk::Track (prefit) Histograms //
    h_Track_n    = new TH1D("h_Track_n",    ";Trk::Track multiplicity;Tracks",       1000,  0.,    1000.);
    h_Track_phi  = new TH1D("h_Track_phi",  ";Trk::Track #phi [rad];Tracks",         100, -PI,    PI);
    h_Track_d0   = new TH1D("h_Track_d0",   ";Trk::Track d_{0} [mm];Tracks",         100, -10.,   10.);
    h_Track_z0   = new TH1D("h_Track_z0",   ";Trk::Track z_{0} [mm];Tracks",         500, -50.,   50.);
    h_Track_invPt = new TH1D("h_Track_invPt", ";Trk::Track 1/p_{T} [GeV^{-1}];Tracks", 100,   0.,   0.1);
    h_Track_cot  = new TH1D("h_Track_cot",  ";Trk::Track cot(#theta); Tracks",       100,  -PI,   PI);
    h_Track_eta  = new TH1D("h_Track_eta",  ";Trk::Track #eta; Tracks",              100, -3.0,   3.0);
    h_Track_nPix = new TH1D("h_Track_nPix", ";Trk::Track Pixel Cluster Multiplicity; Events", 10, 0.,   10.);
    h_Track_nSCT = new TH1D("h_Track_nSCT", ";Trk::Track SCT Cluster Multiplicity; Events",   10, 0.,   10.);

    histograms->push_back(h_Track_n);
    histograms->push_back(h_Track_phi);
    histograms->push_back(h_Track_d0);
    histograms->push_back(h_Track_z0);
    histograms->push_back(h_Track_invPt);
    histograms->push_back(h_Track_cot);
    histograms->push_back(h_Track_eta);
    histograms->push_back(h_Track_nPix);
    histograms->push_back(h_Track_nSCT);

    // Trk::Track (prefit) Histograms ///
    h_refitTrack_n    = new TH1D("h_refitTrack_n",    ";refit Trk::Track multiplicity;Tracks",       1000,  0.,    1000.);
    h_refitTrack_phi  = new TH1D("h_refitTrack_phi",  ";refit refit Trk::Track #phi [rad];Tracks",         100, -PI,    PI);
    h_refitTrack_d0   = new TH1D("h_refitTrack_d0",   ";Trk::Track d_{0} [mm];Tracks",         100, -10.,   10.);
    h_refitTrack_z0   = new TH1D("h_refitTrack_z0",   ";refit Trk::Track z_{0} [mm];Tracks",         500, -50.,   50.);
    h_refitTrack_invPt = new TH1D("h_refitTrack_invPt", ";refit Trk::Track 1/p_{T} [GeV^{-1}];Tracks", 100,   0.,   0.1);
    h_refitTrack_cot  = new TH1D("h_refitTrack_cot",  ";refit Trk::Track cot(#theta); Tracks",       100,  -PI,   PI);
    h_refitTrack_eta  = new TH1D("h_refitTrack_eta",  ";refit Trk::Track #eta; Tracks",              100, -3.0,   3.0);
    h_refitTrack_nPix = new TH1D("h_refitTrack_nPix", ";refit Trk::Track Pixel Cluster Multiplicity; Events", 10, 0.,   10.);
    h_refitTrack_nSCT = new TH1D("h_refitTrack_nSCT", ";refit Trk::Track SCT Cluster Multiplicity; Events",   10, 0.,   10.);

    histograms->push_back(h_refitTrack_n);
    histograms->push_back(h_refitTrack_phi);
    histograms->push_back(h_refitTrack_d0);
    histograms->push_back(h_refitTrack_z0);
    histograms->push_back(h_refitTrack_invPt);
    histograms->push_back(h_refitTrack_cot);
    histograms->push_back(h_refitTrack_eta);
    histograms->push_back(h_refitTrack_nPix);
    histograms->push_back(h_refitTrack_nSCT);

}


void FTK_RDO_ReaderAlgo::Tree_Init(){

  ////// Track Variables
  Tree_ftk->Branch("FTKraw_track_theta",&raw_track_theta);
  Tree_ftk->Branch("FTKraw_track_eta",&raw_track_eta);
  Tree_ftk->Branch("FTKraw_track_phi0",&raw_track_phi0);
  Tree_ftk->Branch("FTKraw_track_d0",&raw_track_d0);
  Tree_ftk->Branch("FTKraw_track_z0",&raw_track_z0);
  Tree_ftk->Branch("FTKraw_track_invPt",&raw_track_invPt);
  Tree_ftk->Branch("FTKraw_track_Pt",&raw_track_Pt);
  
  Tree_ftk->Branch("FTKconverted_track_theta",&converted_track_theta);
  Tree_ftk->Branch("FTKconverted_track_eta",&converted_track_eta);
  Tree_ftk->Branch("FTKconverted_track_phi0",&converted_track_phi0);
  Tree_ftk->Branch("FTKconverted_track_d0",&converted_track_d0);
  Tree_ftk->Branch("FTKconverted_track_z0",&converted_track_z0);
  Tree_ftk->Branch("FTKconverted_track_invPt",&converted_track_invPt);
  Tree_ftk->Branch("FTKconverted_track_Pt",&converted_track_Pt);
  
  Tree_ftk->Branch("FTKrefit_track_theta",&refit_track_theta);     
  Tree_ftk->Branch("FTKrefit_track_eta",&refit_track_eta);         
  Tree_ftk->Branch("FTKrefit_track_phi0",&refit_track_phi0);       
  Tree_ftk->Branch("FTKrefit_track_d0",&refit_track_d0);           
  Tree_ftk->Branch("FTKrefit_track_z0",&refit_track_z0);           
  Tree_ftk->Branch("FTKrefit_track_invPt",&refit_track_invPt);     
  Tree_ftk->Branch("FTKrefit_track_Pt",&refit_track_Pt);           

  Tree_ftk->Branch("offline_track_theta",&offline_track_theta);     
  Tree_ftk->Branch("offline_track_eta",&offline_track_eta);         
  Tree_ftk->Branch("offline_track_phi0",&offline_track_phi0);       
  Tree_ftk->Branch("offline_track_d0",&offline_track_d0);           
  Tree_ftk->Branch("offline_track_z0",&offline_track_z0);           
  Tree_ftk->Branch("offline_track_invPt",&offline_track_invPt);     
  Tree_ftk->Branch("offline_track_Pt",&offline_track_Pt);           

  //////////// Raw Tracks, Fast Algorithm
  Tree_ftk->Branch("fastAlg_FTKraw_vertex_x_position",&fastAlg_FTKraw_vertex_x_position);
  Tree_ftk->Branch("fastAlg_FTKraw_vertex_y_position",&fastAlg_FTKraw_vertex_y_position);
  Tree_ftk->Branch("fastAlg_FTKraw_vertex_z_position",&fastAlg_FTKraw_vertex_z_position);
  Tree_ftk->Branch("fastAlg_FTKraw_vertex_x_error",&fastAlg_FTKraw_vertex_x_error);
  Tree_ftk->Branch("fastAlg_FTKraw_vertex_y_error",&fastAlg_FTKraw_vertex_y_error);
  Tree_ftk->Branch("fastAlg_FTKraw_vertex_z_error",&fastAlg_FTKraw_vertex_z_error);
  Tree_ftk->Branch("fastAlg_FTKraw_vertex_nTrack",&fastAlg_FTKraw_vertex_nTrack);
  Tree_ftk->Branch("fastAlg_FTKraw_vertex_nDoF",&fastAlg_FTKraw_vertex_ndf);
  Tree_ftk->Branch("fastAlg_FTKraw_vertex_chi2",&fastAlg_FTKraw_vertex_chi2);
  Tree_ftk->Branch("fastAlg_FTKraw_vertex_chi2_over_ndf",&fastAlg_FTKraw_vertex_chi2_over_ndf);
  Tree_ftk->Branch("fastAlg_FTKraw_vertex_associated_track_theta",&fastAlg_FTKraw_vertex_associated_track_theta);
  Tree_ftk->Branch("fastAlg_FTKraw_vertex_associated_track_phi",&fastAlg_FTKraw_vertex_associated_track_phi0);
  Tree_ftk->Branch("fastAlg_FTKraw_vertex_associated_track_d0",&fastAlg_FTKraw_vertex_associated_track_d0);
  Tree_ftk->Branch("fastAlg_FTKraw_vertex_associated_track_z0",&fastAlg_FTKraw_vertex_associated_track_z0);
  Tree_ftk->Branch("fastAlg_FTKraw_vertex_associated_track_invPt",&fastAlg_FTKraw_vertex_associated_track_invPt);
  Tree_ftk->Branch("fastAlg_FTKraw_vertex_associated_track_Pt",&fastAlg_FTKraw_vertex_associated_track_Pt);
  Tree_ftk->Branch("fastAlg_FTKraw_vertex_associated_track_eta",&fastAlg_FTKraw_vertex_associated_track_eta);
  Tree_ftk->Branch("fastAlg_FTKraw_vertex_number",&fastAlg_FTKraw_vertex_number);
  Tree_ftk->Branch("fastAlg_FTKraw_vertex_associated_track_nVerts",&fastAlg_FTKraw_vertex_associated_track_nVerts);
  Tree_ftk->Branch("fastAlg_FTKraw_vertex_associated_track_VtxNumber",&fastAlg_FTKraw_vertex_associated_track_VtxNumber);
  Tree_ftk->Branch("fastAlg_FTKraw_sumPt",&fastAlg_FTKraw_vertex_sumPt);
  Tree_ftk->Branch("fastAlg_FTKraw_sumPt2",&fastAlg_FTKraw_vertex_sumPt2);
  Tree_ftk->Branch("fastAlg_FTKraw_sumPt2_vertex_number",&fastAlg_FTKraw_vertex_sumPt2_vtxNumber);
  
  
  /////////// Refit Tracks, Fast Algorithm
  Tree_ftk->Branch("fastAlg_FTKrefit_vertex_x_position",&fastAlg_FTKrefit_vertex_x_position);
  Tree_ftk->Branch("fastAlg_FTKrefit_vertex_y_position",&fastAlg_FTKrefit_vertex_y_position);
  Tree_ftk->Branch("fastAlg_FTKrefit_vertex_z_position",&fastAlg_FTKrefit_vertex_z_position);
  Tree_ftk->Branch("fastAlg_FTKrefit_vertex_x_error",&fastAlg_FTKrefit_vertex_x_error);
  Tree_ftk->Branch("fastAlg_FTKrefit_vertex_y_error",&fastAlg_FTKrefit_vertex_y_error);
  Tree_ftk->Branch("fastAlg_FTKrefit_vertex_z_error",&fastAlg_FTKrefit_vertex_z_error);
  Tree_ftk->Branch("fastAlg_FTKrefit_vertex_nTrack",&fastAlg_FTKrefit_vertex_nTrack);
  Tree_ftk->Branch("fastAlg_FTKrefit_vertex_nDoF",&fastAlg_FTKrefit_vertex_ndf);
  Tree_ftk->Branch("fastAlg_FTKrefit_vertex_chi2",&fastAlg_FTKrefit_vertex_chi2);
  Tree_ftk->Branch("fastAlg_FTKrefit_vertex_chi2_over_ndf",&fastAlg_FTKrefit_vertex_chi2_over_ndf);
  Tree_ftk->Branch("fastAlg_FTKrefit_vertex_associated_track_theta",&fastAlg_FTKrefit_vertex_associated_track_theta);
  Tree_ftk->Branch("fastAlg_FTKrefit_vertex_associated_track_phi",&fastAlg_FTKrefit_vertex_associated_track_phi0);
  Tree_ftk->Branch("fastAlg_FTKrefit_vertex_associated_track_d0",&fastAlg_FTKrefit_vertex_associated_track_d0);
  Tree_ftk->Branch("fastAlg_FTKrefit_vertex_associated_track_z0",&fastAlg_FTKrefit_vertex_associated_track_z0);
  Tree_ftk->Branch("fastAlg_FTKrefit_vertex_associated_track_invPt",&fastAlg_FTKrefit_vertex_associated_track_invPt);
  Tree_ftk->Branch("fastAlg_FTKrefit_vertex_associated_track_Pt",&fastAlg_FTKrefit_vertex_associated_track_Pt);
  Tree_ftk->Branch("fastAlg_FTKrefit_vertex_associated_track_eta",&fastAlg_FTKrefit_vertex_associated_track_eta);
  Tree_ftk->Branch("fastAlg_FTKrefit_vertex_number",&fastAlg_FTKrefit_vertex_number);
  Tree_ftk->Branch("fastAlg_FTKrefit_vertex_associated_track_nVerts",&fastAlg_FTKrefit_vertex_associated_track_nVerts);
  Tree_ftk->Branch("fastAlg_FTKrefit_vertex_associated_track_VtxNumber",&fastAlg_FTKrefit_vertex_associated_track_VtxNumber);
  Tree_ftk->Branch("fastAlg_FTKrefit_sumPt",&fastAlg_FTKrefit_vertex_sumPt);
  Tree_ftk->Branch("fastAlg_FTKrefit_sumPt2",&fastAlg_FTKrefit_vertex_sumPt2);
  Tree_ftk->Branch("fastAlg_FTKrefit_sumPt2_vertex_number",&fastAlg_FTKrefit_vertex_sumPt2_vtxNumber);
  
  /////////// Converted Tracks, Offline Algorithm
  Tree_ftk->Branch("offlineAlg_FTKconverted_vertex_x_position",&offlineAlg_FTKconverted_vertex_x_position);
  Tree_ftk->Branch("offlineAlg_FTKconverted_vertex_y_position",&offlineAlg_FTKconverted_vertex_y_position);
  Tree_ftk->Branch("offlineAlg_FTKconverted_vertex_z_position",&offlineAlg_FTKconverted_vertex_z_position);
  Tree_ftk->Branch("offlineAlg_FTKconverted_vertex_x_error",&offlineAlg_FTKconverted_vertex_x_error);
  Tree_ftk->Branch("offlineAlg_FTKconverted_vertex_y_error",&offlineAlg_FTKconverted_vertex_y_error);
  Tree_ftk->Branch("offlineAlg_FTKconverted_vertex_z_error",&offlineAlg_FTKconverted_vertex_z_error);
  Tree_ftk->Branch("offlineAlg_FTKconverted_vertex_nTrack",&offlineAlg_FTKconverted_vertex_nTrack);
  Tree_ftk->Branch("offlineAlg_FTKconverted_vertex_nDoF",&offlineAlg_FTKconverted_vertex_ndf);
  Tree_ftk->Branch("offlineAlg_FTKconverted_vertex_chi2",&offlineAlg_FTKconverted_vertex_chi2);
  Tree_ftk->Branch("offlineAlg_FTKconverted_vertex_chi2_over_ndf",&offlineAlg_FTKconverted_vertex_chi2_over_ndf);
  Tree_ftk->Branch("offlineAlg_FTKconverted_vertex_associated_track_theta",&offlineAlg_FTKconverted_vertex_associated_track_theta);
  Tree_ftk->Branch("offlineAlg_FTKconverted_vertex_associated_track_phi",&offlineAlg_FTKconverted_vertex_associated_track_phi0);
  Tree_ftk->Branch("offlineAlg_FTKconverted_vertex_associated_track_d0",&offlineAlg_FTKconverted_vertex_associated_track_d0);
  Tree_ftk->Branch("offlineAlg_FTKconverted_vertex_associated_track_z0",&offlineAlg_FTKconverted_vertex_associated_track_z0);
  Tree_ftk->Branch("offlineAlg_FTKconverted_vertex_associated_track_invPt",&offlineAlg_FTKconverted_vertex_associated_track_invPt);
  Tree_ftk->Branch("offlineAlg_FTKconverted_vertex_associated_track_Pt",&offlineAlg_FTKconverted_vertex_associated_track_Pt);
  Tree_ftk->Branch("offlineAlg_FTKconverted_vertex_associated_track_eta",&offlineAlg_FTKconverted_vertex_associated_track_eta);
  Tree_ftk->Branch("offlineAlg_FTKconverted_vertex_number",&offlineAlg_FTKconverted_vertex_number);
  Tree_ftk->Branch("offlineAlg_FTKconverted_vertex_associated_track_nVerts",&offlineAlg_FTKconverted_vertex_associated_track_nVerts);
  Tree_ftk->Branch("offlineAlg_FTKconverted_vertex_associated_track_VtxNumber",&offlineAlg_FTKconverted_vertex_associated_track_VtxNumber);
  Tree_ftk->Branch("offlineAlg_FTKconverted_sumPt",&offlineAlg_FTKconverted_vertex_sumPt);
  Tree_ftk->Branch("offlineAlg_FTKconverted_sumPt2",&offlineAlg_FTKconverted_vertex_sumPt2);
  Tree_ftk->Branch("offlineAlg_FTKconverted_sumPt2_vertex_number",&offlineAlg_FTKconverted_vertex_sumPt2_vtxNumber);
  
  /////////// Refit Tracks, Offline Algorithm
  Tree_ftk->Branch("offlineAlg_FTKrefit_vertex_x_position",&offlineAlg_FTKrefit_vertex_x_position);
  Tree_ftk->Branch("offlineAlg_FTKrefit_vertex_y_position",&offlineAlg_FTKrefit_vertex_y_position);
  Tree_ftk->Branch("offlineAlg_FTKrefit_vertex_z_position",&offlineAlg_FTKrefit_vertex_z_position);
  Tree_ftk->Branch("offlineAlg_FTKrefit_vertex_x_error",&offlineAlg_FTKrefit_vertex_x_error);
  Tree_ftk->Branch("offlineAlg_FTKrefit_vertex_y_error",&offlineAlg_FTKrefit_vertex_y_error);
  Tree_ftk->Branch("offlineAlg_FTKrefit_vertex_z_error",&offlineAlg_FTKrefit_vertex_z_error);
  Tree_ftk->Branch("offlineAlg_FTKrefit_vertex_chi2",&offlineAlg_FTKrefit_vertex_chi2);
  Tree_ftk->Branch("offlineAlg_FTKrefit_vertex_nTrack",&offlineAlg_FTKrefit_vertex_nTrack);
  Tree_ftk->Branch("offlineAlg_FTKrefit_vertex_nDoF",&offlineAlg_FTKrefit_vertex_ndf);
  Tree_ftk->Branch("offlineAlg_FTKrefit_vertex_chi2_over_ndf",&offlineAlg_FTKrefit_vertex_chi2_over_ndf);
  Tree_ftk->Branch("offlineAlg_FTKrefit_vertex_number",&offlineAlg_FTKrefit_vertex_number);
  Tree_ftk->Branch("offlineAlg_FTKrefit_vertex_associated_track_theta",&offlineAlg_FTKrefit_vertex_associated_track_theta);
  Tree_ftk->Branch("offlineAlg_FTKrefit_vertex_associated_track_eta",&offlineAlg_FTKrefit_vertex_associated_track_eta);
  Tree_ftk->Branch("offlineAlg_FTKrefit_vertex_associated_track_phi0",&offlineAlg_FTKrefit_vertex_associated_track_phi0);
  Tree_ftk->Branch("offlineAlg_FTKrefit_vertex_associated_track_z0",&offlineAlg_FTKrefit_vertex_associated_track_z0);
  Tree_ftk->Branch("offlineAlg_FTKrefit_vertex_associated_track_d0",&offlineAlg_FTKrefit_vertex_associated_track_d0);
  Tree_ftk->Branch("offlineAlg_FTKrefit_vertex_associated_track_invPt",&offlineAlg_FTKrefit_vertex_associated_track_invPt);
  Tree_ftk->Branch("offlineAlg_FTKrefit_vertex_associated_track_Pt",&offlineAlg_FTKrefit_vertex_associated_track_Pt);
  Tree_ftk->Branch("offlineAlg_FTKrefit_vertex_associated_track_nVerts",&offlineAlg_FTKrefit_vertex_associated_track_nVerts);
  Tree_ftk->Branch("offlineAlg_FTKrefit_vertex_associated_track_VtxNumber",&offlineAlg_FTKrefit_vertex_associated_track_VtxNumber);
  Tree_ftk->Branch("offlineAlg_FTKrefit_sumPt",&offlineAlg_FTKrefit_vertex_sumPt);
  Tree_ftk->Branch("offlineAlg_FTKrefit_sumPt2",&offlineAlg_FTKrefit_vertex_sumPt2);
  Tree_ftk->Branch("offlineAlg_FTKrefit_sumPt2_vertex_number",&offlineAlg_FTKrefit_vertex_sumPt2_vtxNumber);
  
  //////////// Offline Tracks, Offline Algorithm
  Tree_ftk->Branch("offlineAlg_offlineTracks_vertex_x_position",&offlineAlg_offlineTracks_vertex_x_position);
  Tree_ftk->Branch("offlineAlg_offlineTracks_vertex_y_position",&offlineAlg_offlineTracks_vertex_y_position);
  Tree_ftk->Branch("offlineAlg_offlineTracks_vertex_z_position",&offlineAlg_offlineTracks_vertex_z_position);
  Tree_ftk->Branch("offlineAlg_offlineTracks_vertex_x_error",&offlineAlg_offlineTracks_vertex_x_error);
  Tree_ftk->Branch("offlineAlg_offlineTracks_vertex_y_error",&offlineAlg_offlineTracks_vertex_y_error);
  Tree_ftk->Branch("offlineAlg_offlineTracks_vertex_z_error",&offlineAlg_offlineTracks_vertex_z_error);
  Tree_ftk->Branch("offlineAlg_offlineTracks_vertex_chi2",&offlineAlg_offlineTracks_vertex_chi2);
  Tree_ftk->Branch("offlineAlg_offlineTracks_vertex_nTrack",&offlineAlg_offlineTracks_vertex_nTrack);
  Tree_ftk->Branch("offlineAlg_offlineTracks_vertex_nDoF",&offlineAlg_offlineTracks_vertex_ndf);
  Tree_ftk->Branch("offlineAlg_offlineTracks_vertex_chi2_over_ndf",&offlineAlg_offlineTracks_vertex_chi2_over_ndf);
  Tree_ftk->Branch("offlineAlg_offlineTracks_vertex_number",&offlineAlg_offlineTracks_vertex_number);
  Tree_ftk->Branch("offlineAlg_offlineTracks_vertex_associated_track_theta",&offlineAlg_offlineTracks_vertex_associated_track_theta);
  Tree_ftk->Branch("offlineAlg_offlineTracks_vertex_associated_track_eta",&offlineAlg_offlineTracks_vertex_associated_track_eta);
  Tree_ftk->Branch("offlineAlg_offlineTracks_vertex_associated_track_phi0",&offlineAlg_offlineTracks_vertex_associated_track_phi0);
  Tree_ftk->Branch("offlineAlg_offlineTracks_vertex_associated_track_z0",&offlineAlg_offlineTracks_vertex_associated_track_z0);
  Tree_ftk->Branch("offlineAlg_offlineTracks_vertex_associated_track_d0",&offlineAlg_offlineTracks_vertex_associated_track_d0);
  Tree_ftk->Branch("offlineAlg_offlineTracks_vertex_associated_track_invPt",&offlineAlg_offlineTracks_vertex_associated_track_invPt);
  Tree_ftk->Branch("offlineAlg_offlineTracks_vertex_associated_track_Pt",&offlineAlg_offlineTracks_vertex_associated_track_Pt);
  Tree_ftk->Branch("offlineAlg_offlineTracks_vertex_associated_track_nVerts",&offlineAlg_offlineTracks_vertex_associated_track_nVerts);
  Tree_ftk->Branch("offlineAlg_offlineTracks_vertex_associated_track_VtxNumber",&offlineAlg_offlineTracks_vertex_associated_track_VtxNumber);
  Tree_ftk->Branch("offlineAlg_offlineTracks_sumPt",&offlineAlg_offlineTracks_vertex_sumPt);
  Tree_ftk->Branch("offlineAlg_offlineTracks_sumPt2",&offlineAlg_offlineTracks_vertex_sumPt2);
  Tree_ftk->Branch("offlineAlg_offlineTracks_sumPt2_vertex_number",&offlineAlg_offlineTracks_vertex_sumPt2_vtxNumber);
  
  // Truth Vertices
  Tree_ftk->Branch("truth_x0",&truth_x0);
  Tree_ftk->Branch("truth_y0",&truth_y0);
  Tree_ftk->Branch("truth_z0",&truth_z0);
  
  //Hard Scatter Variables
  Tree_ftk->Branch("isRawFastHS",&isRawFastHS);
  Tree_ftk->Branch("isRefitFastHS",&isRefitFastHS);
  Tree_ftk->Branch("isConvertedOfflineHS",&isConvertedOfflineHS);
  Tree_ftk->Branch("isRefitOfflineHS",&isRefitOfflineHS);
  Tree_ftk->Branch("isOfflineOfflineHS",&isOfflineOfflineHS);
  
  //Additional Variables                                                                            
  Tree_ftk->Branch("RunNumber",&RunNumber);
  Tree_ftk->Branch("EventNumber",&EventNumber);
  Tree_ftk->Branch("LumiBlock",&LumiBlock);
  Tree_ftk->Branch("BCID",&BCID);
  Tree_ftk->Branch("averageInteractionsPerCrossing",&averageInteractionsPerCrossing);
  Tree_ftk->Branch("actualInteractionsPerCrossing",&actualInteractionsPerCrossing);
  Tree_ftk->Branch("extendedLevel1ID",&extendedLevel1ID);
  Tree_ftk->Branch("level1TriggerType",&level1TriggerType);
  Tree_ftk->Branch("level1TriggerInfo",&level1TriggerInfo);
  
  ///////////////////////////// Clear Vectors ////////////////////////////////////////////////////           
  
  raw_track_theta.clear();
  raw_track_eta.clear();
  raw_track_phi0.clear();
  raw_track_d0.clear();
  raw_track_z0.clear();
  raw_track_invPt.clear();
  raw_track_Pt.clear();
  
  converted_track_theta.clear();                                          
  converted_track_eta.clear();                                                                    
  converted_track_phi0.clear();                                                                   
  converted_track_d0.clear();                                                                     
  converted_track_z0.clear();                                                                     
  converted_track_invPt.clear();                                                                  
  converted_track_Pt.clear();                                                                     
  
  refit_track_theta.clear();                                                                      
  refit_track_eta.clear();                                                                        
  refit_track_phi0.clear();                                                                       
  refit_track_d0.clear();                                                                         
  refit_track_z0.clear();                                                                         
  refit_track_invPt.clear();                                                                      
  refit_track_Pt.clear();                                                                         
  
  offline_track_theta.clear();                                                                      
  offline_track_eta.clear();                                                                        
  offline_track_phi0.clear();                                                                       
  offline_track_d0.clear();                                                                         
  offline_track_z0.clear();                                                                         
  offline_track_invPt.clear();                                                                      
  offline_track_Pt.clear();                                                                         
  
  //////
  fastAlg_FTKraw_vertex_x_position.clear();
  fastAlg_FTKraw_vertex_y_position.clear();
  fastAlg_FTKraw_vertex_z_position.clear();
  fastAlg_FTKraw_vertex_x_error.clear();
  fastAlg_FTKraw_vertex_y_error.clear();
  fastAlg_FTKraw_vertex_z_error.clear();
  fastAlg_FTKraw_vertex_nTrack.clear();
  fastAlg_FTKraw_vertex_ndf.clear();
  fastAlg_FTKraw_vertex_chi2.clear();
  fastAlg_FTKraw_vertex_chi2_over_ndf.clear();
  
  fastAlg_FTKraw_vertex_associated_track_theta.clear();
  fastAlg_FTKraw_vertex_associated_track_phi0.clear();
  fastAlg_FTKraw_vertex_associated_track_d0.clear();
  fastAlg_FTKraw_vertex_associated_track_z0.clear();
  fastAlg_FTKraw_vertex_associated_track_invPt.clear();
  fastAlg_FTKraw_vertex_associated_track_Pt.clear();
  fastAlg_FTKraw_vertex_associated_track_eta.clear();
  fastAlg_FTKraw_vertex_associated_track_nVerts.clear();
  fastAlg_FTKraw_vertex_associated_track_VtxNumber.clear();
  fastAlg_FTKraw_vertex_number.clear();
  ///
  
  fastAlg_FTKrefit_vertex_x_position.clear();
  fastAlg_FTKrefit_vertex_y_position.clear();
  fastAlg_FTKrefit_vertex_z_position.clear();
  fastAlg_FTKrefit_vertex_x_error.clear();
  fastAlg_FTKrefit_vertex_y_error.clear();
  fastAlg_FTKrefit_vertex_z_error.clear();
  fastAlg_FTKrefit_vertex_nTrack.clear();
  fastAlg_FTKrefit_vertex_ndf.clear();
  fastAlg_FTKrefit_vertex_chi2.clear();
  fastAlg_FTKrefit_vertex_chi2_over_ndf.clear();
  
  fastAlg_FTKrefit_vertex_associated_track_theta.clear();
  fastAlg_FTKrefit_vertex_associated_track_phi0.clear();
  fastAlg_FTKrefit_vertex_associated_track_d0.clear();
  fastAlg_FTKrefit_vertex_associated_track_z0.clear();
  fastAlg_FTKrefit_vertex_associated_track_invPt.clear();
  fastAlg_FTKrefit_vertex_associated_track_Pt.clear();
  fastAlg_FTKrefit_vertex_associated_track_eta.clear();
  fastAlg_FTKrefit_vertex_associated_track_nVerts.clear();
  fastAlg_FTKrefit_vertex_associated_track_VtxNumber.clear();
  fastAlg_FTKrefit_vertex_number.clear();
  
  ////////
  offlineAlg_FTKconverted_vertex_x_position.clear();
  offlineAlg_FTKconverted_vertex_y_position.clear();
  offlineAlg_FTKconverted_vertex_z_position.clear();
  offlineAlg_FTKconverted_vertex_x_error.clear();
  offlineAlg_FTKconverted_vertex_y_error.clear();
  offlineAlg_FTKconverted_vertex_z_error.clear();
  offlineAlg_FTKconverted_vertex_nTrack.clear();
  offlineAlg_FTKconverted_vertex_ndf.clear();
  offlineAlg_FTKconverted_vertex_chi2.clear();
  offlineAlg_FTKconverted_vertex_chi2_over_ndf.clear();
  
  offlineAlg_FTKconverted_vertex_associated_track_theta.clear();
  offlineAlg_FTKconverted_vertex_associated_track_phi0.clear();
  offlineAlg_FTKconverted_vertex_associated_track_d0.clear();
  offlineAlg_FTKconverted_vertex_associated_track_z0.clear();
  offlineAlg_FTKconverted_vertex_associated_track_invPt.clear();
  offlineAlg_FTKconverted_vertex_associated_track_Pt.clear();
  offlineAlg_FTKconverted_vertex_associated_track_eta.clear(); 
  offlineAlg_FTKconverted_vertex_associated_track_nVerts.clear();
  offlineAlg_FTKconverted_vertex_associated_track_VtxNumber.clear();
  offlineAlg_FTKconverted_vertex_number.clear();
  
  //////
  offlineAlg_FTKrefit_vertex_x_position.clear();
  offlineAlg_FTKrefit_vertex_y_position.clear();
  offlineAlg_FTKrefit_vertex_z_position.clear();
  offlineAlg_FTKrefit_vertex_x_error.clear();
  offlineAlg_FTKrefit_vertex_y_error.clear();
  offlineAlg_FTKrefit_vertex_z_error.clear();
  offlineAlg_FTKrefit_vertex_chi2.clear();
  offlineAlg_FTKrefit_vertex_nTrack.clear();
  offlineAlg_FTKrefit_vertex_ndf.clear();
  offlineAlg_FTKrefit_vertex_chi2_over_ndf.clear();
  
  offlineAlg_FTKrefit_vertex_associated_track_theta.clear();
  offlineAlg_FTKrefit_vertex_associated_track_eta.clear();
  offlineAlg_FTKrefit_vertex_associated_track_phi0.clear();
  offlineAlg_FTKrefit_vertex_associated_track_z0.clear();
  offlineAlg_FTKrefit_vertex_associated_track_d0.clear();
  offlineAlg_FTKrefit_vertex_associated_track_invPt.clear();
  offlineAlg_FTKrefit_vertex_associated_track_Pt.clear();
  offlineAlg_FTKrefit_vertex_associated_track_nVerts.clear();
  offlineAlg_FTKrefit_vertex_associated_track_VtxNumber.clear();
  offlineAlg_FTKrefit_vertex_number.clear();
  
  ///////
  offlineAlg_offlineTracks_vertex_x_position.clear();
  offlineAlg_offlineTracks_vertex_y_position.clear();
  offlineAlg_offlineTracks_vertex_z_position.clear();
  offlineAlg_offlineTracks_vertex_x_error.clear();
  offlineAlg_offlineTracks_vertex_y_error.clear();
  offlineAlg_offlineTracks_vertex_z_error.clear();
  offlineAlg_offlineTracks_vertex_chi2.clear();
  offlineAlg_offlineTracks_vertex_nTrack.clear();
  offlineAlg_offlineTracks_vertex_ndf.clear();
  offlineAlg_offlineTracks_vertex_chi2_over_ndf.clear();
  
  offlineAlg_offlineTracks_vertex_associated_track_theta.clear();
  offlineAlg_offlineTracks_vertex_associated_track_eta.clear();
  offlineAlg_offlineTracks_vertex_associated_track_phi0.clear();
  offlineAlg_offlineTracks_vertex_associated_track_z0.clear();
  offlineAlg_offlineTracks_vertex_associated_track_d0.clear();
  offlineAlg_offlineTracks_vertex_associated_track_invPt.clear();
  offlineAlg_offlineTracks_vertex_associated_track_Pt.clear();
  offlineAlg_offlineTracks_vertex_associated_track_nVerts.clear();
  offlineAlg_offlineTracks_vertex_associated_track_VtxNumber.clear();
  offlineAlg_offlineTracks_vertex_number.clear();
  
  ////
  isRawFastHS.clear();
  isRefitFastHS.clear();
  isConvertedOfflineHS.clear();
  isRefitOfflineHS.clear();
  isOfflineOfflineHS.clear();
  
  //additional variables                                                                            
  level1TriggerInfo.clear();

}


void FTK_RDO_ReaderAlgo::Fill_Raw_Tracks(){
   
  const FTK_RawTrackContainer *rawTracks ( nullptr );

  if ( evtStore()->contains<FTK_RawTrackContainer> ( m_ftk_raw_trackcollection_Name) ) {
    if ( evtStore()->retrieve ( rawTracks, m_ftk_raw_trackcollection_Name  ).isFailure() ) {
      ATH_MSG_DEBUG("Could not retrieve FTK_RawTrackContainer " <<  m_ftk_raw_trackcollection_Name << " in StoreGate.");
    }
  } else {
    ATH_MSG_DEBUG("Could not find FTK_RawTrackContainer " <<  m_ftk_raw_trackcollection_Name << " in StoreGate.");
    
  }

  if (rawTracks != nullptr) {
    ATH_MSG_DEBUG(" Got FTK_RawTrackContainer with " << rawTracks->size() << " tracks");
    
    FTK_RawTrackContainer::const_iterator pTrack = rawTracks->begin();
    FTK_RawTrackContainer::const_iterator pLastTrack = rawTracks->end();
    
    if (m_fillHists) 	h_FTK_RawTrack_n->Fill(rawTracks->size());

    if (m_fillTree) {
      raw_track_theta.clear();
      raw_track_eta.clear();
      raw_track_phi0.clear();
      raw_track_d0.clear();
      raw_track_z0.clear();
      raw_track_invPt.clear();
      raw_track_Pt.clear();
    }
    
    for ( int itr=0; pTrack!= pLastTrack; pTrack++, itr++) {
      float trkTheta = std::atan2(1.0,(*pTrack)->getCotTh());
      float trkEta = -std::log(std::tan(trkTheta/2.));
      float trkPt=1.e10;
      float d0=((*pTrack)->getD0());
      float z0=((*pTrack)->getZ0());
      float phi0=((*pTrack)->getPhi());
      
      if (fabs((*pTrack)->getInvPt()) >= 1e-9) trkPt=1./(*pTrack)->getInvPt();
      
      ATH_MSG_VERBOSE( itr << ": pT: " << trkPt  << " eta: " << trkEta << " phi: " <<  (*pTrack)->getPhi() << " d0: " << (*pTrack)->getD0() <<
		       " z0: " << (*pTrack)->getZ0()<< " cot " << (*pTrack)->getCotTh() << " theta " << trkTheta << " invPt " << (*pTrack)->getInvPt() <<
		       " nPix: " << (*pTrack)->getPixelClusters().size() << " nSCT: "<< (*pTrack)->getSCTClusters().size()<< " barCode: "<<(*pTrack)->getBarcode()  );
      ATH_MSG_VERBOSE( "     SectorID " << (*pTrack)->getSectorID()   <<   "  RoadID "  << (*pTrack)->getRoadID() << " LayerMap " << (*pTrack)->getLayerMap());
      
      if (m_fillHists) {
	h_FTK_RawTrack_phi->Fill((*pTrack)->getPhi());
	h_FTK_RawTrack_d0->Fill((*pTrack)->getD0());
	h_FTK_RawTrack_z0->Fill((*pTrack)->getZ0());
	h_FTK_RawTrack_invPt->Fill((*pTrack)->getInvPt());
	h_FTK_RawTrack_cot->Fill((*pTrack)->getCotTh());
	h_FTK_RawTrack_nPix->Fill((*pTrack)->getPixelClusters().size());
	h_FTK_RawTrack_nSCT->Fill((*pTrack)->getSCTClusters().size());
      }
      
      if (m_fillTree){
	raw_track_eta.push_back(trkEta);
	raw_track_theta.push_back(trkTheta);
	if(fabs(trkPt) >= 1e-9) raw_track_invPt.push_back(1000/trkPt);
	raw_track_Pt.push_back(trkPt);
	raw_track_d0.push_back(d0);
	raw_track_z0.push_back(z0);
	raw_track_phi0.push_back(phi0);
      }		
    }
  }    
}


void FTK_RDO_ReaderAlgo::Fill_Converted_Tracks(){
  ATH_MSG_DEBUG("Getting converted tracks from DataProviderSvc");
  TrackCollection *track_collection = m_DataProviderSvc->getTracks(false);
  ATH_MSG_DEBUG("DataProviderSvc returned " <<  track_collection->size() << " converted tracks");
  //  if ( evtStore()->record ( track_collection, m_TrackCollectionName).isFailure() ) {                                                                                      
  //   ATH_MSG_DEBUG("Failed to record FTK TrackCollection with name " <<  m_TrackCollectionName);                                                                            
  //  }                                                                                                                                                                       

 
  ATH_MSG_VERBOSE( " Printing information for " << track_collection->size()<< " FTK tracks" );
  if (m_fillHists)h_Track_n->Fill(track_collection->size());
  TrackCollection::const_iterator track_it   = track_collection->begin();
  TrackCollection::const_iterator last_track = track_collection->end();
  int iTrack=0;

  if(m_fillTree){
    converted_track_eta.clear();
    converted_track_theta.clear();
    converted_track_invPt.clear();
    converted_track_Pt.clear();
    converted_track_d0.clear();
    converted_track_z0.clear();
    converted_track_phi0.clear();
  }


  for ( ; track_it!= last_track; track_it++, iTrack++) {
    int nPix = 0;
    int nSCT = 0;
    const Trk::TrackSummary* summary = (*track_it)->trackSummary();
    if( summary != 0){
      nPix = summary->get(Trk::numberOfPixelHits);
      nSCT = summary->get(Trk::numberOfSCTHits);
    }
    
    float trkEta = -99999;
    float trkPt = -99999;
    float trkTheta = -99999;
    float d0=-99999;
    float z0=-99999;
    float phi0= -99999;
    
    if(m_fillTree){
      trkEta=-log(std::tan((*track_it)->perigeeParameters()->parameters()[Trk::theta]/2.));
      trkTheta = (*track_it)->perigeeParameters()->parameters()[Trk::theta];
      if (fabs((*track_it)->perigeeParameters()->parameters()[Trk::qOverP]) >= 1e-9) trkPt = sin((*track_it)->perigeeParameters()->parameters()[Trk::theta])/(*track_it)->perigeeParameters()->parameters()[Trk::qOverP];
      d0 = (*track_it)->perigeeParameters()->parameters()[Trk::d0];
      z0 = (*track_it)->perigeeParameters()->parameters()[Trk::z0];
      phi0 = (*track_it)->perigeeParameters()->parameters()[Trk::phi0];
      
      converted_track_eta.push_back(trkEta);
      converted_track_theta.push_back(trkTheta);
      if(fabs(trkPt) >= 1e-9) converted_track_invPt.push_back(1000/trkPt);
      converted_track_Pt.push_back(trkPt);
      converted_track_d0.push_back(d0);
      converted_track_z0.push_back(z0);
      converted_track_phi0.push_back(phi0);
    }
    
    if (m_fillHists) {
      h_Track_phi->Fill((*track_it)->perigeeParameters()->parameters()[Trk::phi0]);
      h_Track_d0->Fill((*track_it)->perigeeParameters()->parameters()[Trk::d0]);
      h_Track_z0->Fill((*track_it)->perigeeParameters()->parameters()[Trk::z0]);
      h_Track_invPt->Fill((*track_it)->perigeeParameters()->parameters()[Trk::qOverP]);
      double cotTheta=1.e10;
      double tanTheta = std::tan((*track_it)->perigeeParameters()->parameters()[Trk::theta]);
      if (fabs(tanTheta)>=1e-9) cotTheta= 1./tanTheta;
      h_Track_cot->Fill(cotTheta);
      
      h_Track_nPix->Fill(nPix);
      h_Track_nSCT->Fill(nSCT);
    }
    
    trkPt = 1.e10;
    if (fabs((*track_it)->perigeeParameters()->parameters()[Trk::qOverP])>=1e-9) trkPt= sin((*track_it)->perigeeParameters()->parameters()[Trk::theta])/(*track_it)->perigeeParameters()->parameters()[Trk::qOverP];
    ATH_MSG_VERBOSE( " Track "<< iTrack << ": pT: "<< trkPt <<
		     " eta: " <<  -log(std::tan((*track_it)->perigeeParameters()->parameters()[Trk::theta]/2.)) <<
		     " phi: " <<  (*track_it)->perigeeParameters()->parameters()[Trk::phi0] <<
		     " d0: " << (*track_it)->perigeeParameters()->parameters()[Trk::d0] <<
		     " z0: " << (*track_it)->perigeeParameters()->parameters()[Trk::z0] <<
		     " nPix: " << nPix << " nSCT: " << nSCT );

  }
  delete(track_collection);
}


void FTK_RDO_ReaderAlgo::Fill_Refit_Tracks(){
  ATH_MSG_DEBUG( " Getting refitted tracks from DataProviderSvc" );
  TrackCollection *refitTracks  = m_DataProviderSvc->getTracks(true);
  ATH_MSG_DEBUG( "DataProviderSvc returned " <<  refitTracks->size() << " refitted tracks" );
  //       if ( evtStore()->get ( refitTracks, m_refitTrackCollectionName).isFailure() ) {
  //	 ATH_MSG_DEBUG( "Failed to get refit FTK TrackCollection with name " <<  m_refitTrackCollectionName );
  //       }
  
  if(m_fillTree){
    refit_track_eta.clear();
    refit_track_theta.clear();
    refit_track_invPt.clear();
    refit_track_Pt.clear();
    refit_track_d0.clear();
    refit_track_z0.clear();
    refit_track_phi0.clear();
  }
  
  ATH_MSG_VERBOSE( " Printing information for " << refitTracks->size()<< " refitted FTK tracks" );
  if (m_fillHists) h_Track_n->Fill(refitTracks->size());
  auto track_it   = refitTracks->begin();
  auto last_track = refitTracks->end();
  for (int iTrack=0 ; track_it!= last_track; track_it++, iTrack++) {
    int nPix = 0;
    int nSCT = 0;
    const Trk::TrackSummary* summary = (*track_it)->trackSummary();
    if( summary != 0){
      nPix = summary->get(Trk::numberOfPixelHits);
      nSCT = summary->get(Trk::numberOfSCTHits);
    }
    if (m_fillHists) {
      h_Track_phi->Fill((*track_it)->perigeeParameters()->parameters()[Trk::phi0]);
      h_Track_d0->Fill((*track_it)->perigeeParameters()->parameters()[Trk::d0]);
      h_Track_z0->Fill((*track_it)->perigeeParameters()->parameters()[Trk::z0]);
      h_Track_invPt->Fill((*track_it)->perigeeParameters()->parameters()[Trk::qOverP]);
      double cotTheta=1.e10;
      double tanTheta = std::tan((*track_it)->perigeeParameters()->parameters()[Trk::theta]);
      if (fabs(tanTheta)>=1e-9) cotTheta=1./tanTheta;
      h_Track_cot->Fill(cotTheta);
      h_Track_nPix->Fill(nPix);
      h_Track_nSCT->Fill(nSCT);
    }
    
    float trkEta = -99999;
    float trkPt = -999999;
    float trkTheta = -99999;
    float d0=-99999;
    float z0=-9999999;
    float phi0= -999999;
    
    trkEta=-log(std::tan((*track_it)->perigeeParameters()->parameters()[Trk::theta]/2.));
    trkTheta = (*track_it)->perigeeParameters()->parameters()[Trk::theta];
    if( fabs((*track_it)->perigeeParameters()->parameters()[Trk::qOverP]) >= 1e-9) trkPt = sin((*track_it)->perigeeParameters()->parameters()[Trk::theta])/(*track_it)->perigeeParameters()->parameters()[Trk::qOverP];
    d0 = (*track_it)->perigeeParameters()->parameters()[Trk::d0];
    z0 = (*track_it)->perigeeParameters()->parameters()[Trk::z0];
    phi0 = (*track_it)->perigeeParameters()->parameters()[Trk::phi0];
    
    if (m_fillTree){
      refit_track_eta.push_back(trkEta);
      refit_track_theta.push_back(trkTheta);
      if(fabs(trkPt) >= 1e-9) refit_track_invPt.push_back(1000/trkPt);
      refit_track_Pt.push_back(trkPt);
      refit_track_d0.push_back(d0);
      refit_track_z0.push_back(z0);
      refit_track_phi0.push_back(phi0);
    }
    

    trkPt = 1.e10;
    if (fabs((*track_it)->perigeeParameters()->parameters()[Trk::qOverP])>=1e-9) trkPt= sin((*track_it)->perigeeParameters()->parameters()[Trk::theta])/(*track_it)->perigeeParameters()->parameters()[Trk::qOverP];

    ATH_MSG_VERBOSE( " Track" << iTrack << ": pT: "<< trkPt <<
		     " eta: " <<  -std::log(std::tan((*track_it)->perigeeParameters()->parameters()[Trk::theta]/2.)) <<
		     " phi: " <<  (*track_it)->perigeeParameters()->parameters()[Trk::phi0] <<
		     " d0: " << (*track_it)->perigeeParameters()->parameters()[Trk::d0] <<
		     " z0: " << (*track_it)->perigeeParameters()->parameters()[Trk::z0] <<
		     " nPix: " << nPix << " nSCT: " << nSCT );
  }
  delete (refitTracks);
}


void FTK_RDO_ReaderAlgo::Fill_Offline_Tracks(){
  if(m_fillTree){
    offline_track_theta.clear();                                                                      
    offline_track_eta.clear();                                                                        
    offline_track_phi0.clear();                                                                       
    offline_track_d0.clear();                                                                         
    offline_track_z0.clear();                                                                         
    offline_track_invPt.clear();                                                                      
    offline_track_Pt.clear();                                                                         
  }     
  const xAOD::TrackParticleContainer *offlineTracks = nullptr;
  if(evtStore()->retrieve(offlineTracks,"InDetTrackParticles").isFailure()){
    ATH_MSG_DEBUG("Failed to retrieve Offline Tracks");
  }
  ATH_MSG_VERBOSE( " Printing information for " << offlineTracks->size()<< " offline tracks" );
   
  if(offlineTracks->size()!=0){
    auto track_it   = offlineTracks->begin();
    auto last_track = offlineTracks->end();
    for (int iTrk=0 ; track_it!= last_track; track_it++, iTrk++){

      if(m_fillTree){
	offline_track_Pt.push_back((*track_it)->pt()*(*track_it)->charge()/1000);
	if(fabs(1000*(*track_it)->pt()*(*track_it)->charge()) >= 1e-9) offline_track_invPt.push_back(1000/(*track_it)->pt()*(*track_it)->charge());
	offline_track_theta.push_back((*track_it)->theta());
	offline_track_eta.push_back((*track_it)->eta());
	offline_track_phi0.push_back((*track_it)->phi0());
	offline_track_d0.push_back((*track_it)->d0());
	offline_track_z0.push_back((*track_it)->z0());
      }
    }
  }
}

void FTK_RDO_ReaderAlgo::Fill_Raw_Vertices_fast(unsigned int track_requirement){

  bool flag = false;
  
  ATH_MSG_DEBUG( " Getting xAOD::VertexContainer from raw tracks from DataProviderSvc" );
  const xAOD::VertexContainer *vxr = m_DataProviderSvc->getFastVertices(ftk::RawTrack); // RAW  FTK tracks
  
  if(vxr != 0 && vxr != nullptr ){
    ATH_MSG_DEBUG( "DataProviderSvc returned " <<  vxr->size() << " xAOD:Vertex created from raw tracks using fast FTK_VertexFinderTool" );
    float this_vertex_sumpt = 0;
    
    ATH_MSG_VERBOSE( " " );
    ATH_MSG_VERBOSE( " Printing information for " <<  vxr->size()<< " Vertex " );
    int ivx=0;
    
    if(m_fillTree){
      fastAlg_FTKraw_vertex_ndf.clear();
      fastAlg_FTKraw_vertex_chi2.clear();
      fastAlg_FTKraw_vertex_chi2_over_ndf.clear();
      fastAlg_FTKraw_vertex_x_position.clear();
      fastAlg_FTKraw_vertex_y_position.clear();
      fastAlg_FTKraw_vertex_z_position.clear();
      fastAlg_FTKraw_vertex_x_error.clear();
      fastAlg_FTKraw_vertex_y_error.clear();
      fastAlg_FTKraw_vertex_z_error.clear();
      fastAlg_FTKraw_vertex_nTrack.clear();
      fastAlg_FTKraw_vertex_number.clear();
      fastAlg_FTKraw_vertex_associated_track_phi0.clear();
      fastAlg_FTKraw_vertex_associated_track_d0.clear();
      fastAlg_FTKraw_vertex_associated_track_z0.clear();
      fastAlg_FTKraw_vertex_associated_track_invPt.clear();
      fastAlg_FTKraw_vertex_associated_track_cot.clear();
      fastAlg_FTKraw_vertex_associated_track_eta.clear();
      fastAlg_FTKraw_vertex_associated_track_nVerts.clear();
      fastAlg_FTKraw_vertex_associated_track_VtxNumber.clear();    
      fastAlg_FTKraw_vertex_number.clear();
      fastAlg_FTKraw_vertex_associated_track_Pt.clear();
      fastAlg_FTKraw_vertex_associated_track_eta.clear();
      fastAlg_FTKraw_vertex_associated_track_theta.clear();
      isRawFastHS.clear();  // currently HS variable does not work for Fast Algo (use sumpt2 for HSV)
    }
    
    for (auto pvx = vxr->begin(); pvx != vxr->end(); pvx++, ivx++) {
      
      this_vertex_sumpt=0;
      
      ATH_MSG_VERBOSE( ivx << ": (x,y,z) = ( " <<
		       (*pvx)->position().x() << ", " <<
		       (*pvx)->position().y() << ", " <<
		       (*pvx)->position().z() <<
		       " ) Chi2: " << (*pvx)->chiSquared() <<
		       " nDoF " << (*pvx)->numberDoF() );
      
      if(m_fillTree){
	fastAlg_FTKraw_vertex_nTrack.push_back((*pvx)->vxTrackAtVertex().size());
	fastAlg_FTKraw_vertex_ndf.push_back((*pvx)->numberDoF());
	fastAlg_FTKraw_vertex_chi2.push_back((*pvx)->chiSquared());
	fastAlg_FTKraw_vertex_x_position.push_back((*pvx)->position().x());
	fastAlg_FTKraw_vertex_y_position.push_back((*pvx)->position().y());
	fastAlg_FTKraw_vertex_z_position.push_back((*pvx)->position().z());
	fastAlg_FTKraw_vertex_x_error.push_back(sqrt((*pvx)->covariancePosition()(0,0)));
	fastAlg_FTKraw_vertex_y_error.push_back(sqrt((*pvx)->covariancePosition()(1,1)));
	fastAlg_FTKraw_vertex_z_error.push_back(sqrt((*pvx)->covariancePosition()(2,2)));
	if (fabs((*pvx)->numberDoF()) >= 1e-9) fastAlg_FTKraw_vertex_chi2_over_ndf.push_back((*pvx)->chiSquared()/(*pvx)->numberDoF());       
	
	int isThisRawFastHS = 0;
	if ((*pvx)->vertexType() == xAOD::VxType::PriVtx) isThisRawFastHS = 1;
	isRawFastHS.push_back(isThisRawFastHS);
	
	
	for(vector<Trk::VxTrackAtVertex>::const_iterator ti=(*pvx)->vxTrackAtVertex().begin(); ti!=(*pvx)->vxTrackAtVertex().end(); ti++){
	  const Trk::Perigee* vertexPerigee = dynamic_cast <const Trk::Perigee*>(ti->perigeeAtVertex());
	  
	  float trk_theta = 999;
	  float trk_eta = 999;
	  float trk_phi0 = 999;
	  float trk_d0 = 999;
	  float trk_z0 = 999;
	  float trk_invPt = 999;
	  
	  if (vertexPerigee){
	    trk_theta = vertexPerigee->parameters()[Trk::theta];
	    trk_eta = -std::log(std::tan(trk_theta/2));
	    trk_phi0 =vertexPerigee->parameters()[Trk::phi0];
	    trk_d0 = vertexPerigee->parameters()[Trk::d0];
	    trk_z0 = vertexPerigee->parameters()[Trk::z0];
	    trk_invPt = vertexPerigee->parameters()[Trk::qOverP];
	  }
	  if(m_fillTree){
	    fastAlg_FTKraw_vertex_associated_track_theta.push_back(trk_theta);
	    fastAlg_FTKraw_vertex_associated_track_phi0.push_back(trk_phi0);
	    fastAlg_FTKraw_vertex_associated_track_d0.push_back(trk_d0);
	    fastAlg_FTKraw_vertex_associated_track_z0.push_back(trk_z0);
	    if( fabs(sin(trk_theta)) >= 1e-9) fastAlg_FTKraw_vertex_associated_track_invPt.push_back(1000*trk_invPt/sin(trk_theta));
	    if( fabs(1000*trk_invPt) >= 1e-9) fastAlg_FTKraw_vertex_associated_track_Pt.push_back(sin(trk_theta)/(1000*trk_invPt));
	    fastAlg_FTKraw_vertex_associated_track_eta.push_back(trk_eta);
	    fastAlg_FTKraw_vertex_number.push_back(ivx);
	  }
	  if( fabs(1000*trk_invPt) >= 1e-9) this_vertex_sumpt = fabs(this_vertex_sumpt+sin(trk_theta)/(1000*trk_invPt));
	  if((*pvx)->vxTrackAtVertex().size() > track_requirement) flag = true;
	  else flag = false;
	}
	
	if(this_vertex_sumpt*this_vertex_sumpt >= fastAlg_FTKraw_vertex_sumPt*fastAlg_FTKraw_vertex_sumPt && flag == true ){
	  fastAlg_FTKraw_vertex_sumPt = this_vertex_sumpt;
	  fastAlg_FTKraw_vertex_sumPt2_vtxNumber = ivx;
	}
	fastAlg_FTKraw_vertex_associated_track_nVerts.push_back(vxr->size());
      }
      if(fabs(fastAlg_FTKraw_vertex_sumPt) >= 1e-9) fastAlg_FTKraw_vertex_sumPt2 = fastAlg_FTKraw_vertex_sumPt*fastAlg_FTKraw_vertex_sumPt;
    }
    delete(vxr);
  }
  else ATH_MSG_ERROR( "DataProviderSvc returned no valid vertices");  
}

void FTK_RDO_ReaderAlgo::Fill_Refit_Vertices_fast(unsigned int track_requirement){

  bool flag = false;

  ATH_MSG_DEBUG( " Getting xAOD::VertexContainer from refit tracks from DataProviderSvc" );
  const xAOD::VertexContainer *vxr = m_DataProviderSvc->getFastVertices(ftk::RefittedTrack); // Refitted FTK tracks
  if(vxr != 0 && vxr != nullptr){
    ATH_MSG_DEBUG( "DataProviderSvc returned " <<  vxr->size() << " xAOD:Vertex created from refit tracks using fast FTK_VertexFinderTool" );
    
    float this_vertex_sumpt = 0;
  
    ATH_MSG_VERBOSE( " " );
    ATH_MSG_VERBOSE( " Printing information for " <<  vxr->size()<< " Vertex " );
    int ivx=0;
    
    if(m_fillTree){
      fastAlg_FTKrefit_vertex_ndf.clear();
      fastAlg_FTKrefit_vertex_chi2.clear();
      fastAlg_FTKrefit_vertex_chi2_over_ndf.clear();
      fastAlg_FTKrefit_vertex_x_position.clear();
      fastAlg_FTKrefit_vertex_y_position.clear();
      fastAlg_FTKrefit_vertex_z_position.clear();
      fastAlg_FTKrefit_vertex_x_error.clear();
      fastAlg_FTKrefit_vertex_y_error.clear();
      fastAlg_FTKrefit_vertex_z_error.clear();
      fastAlg_FTKrefit_vertex_nTrack.clear();
      fastAlg_FTKrefit_vertex_number.clear();
      fastAlg_FTKrefit_vertex_associated_track_phi0.clear();
      fastAlg_FTKrefit_vertex_associated_track_d0.clear();
      fastAlg_FTKrefit_vertex_associated_track_z0.clear();
      fastAlg_FTKrefit_vertex_associated_track_invPt.clear();
      fastAlg_FTKrefit_vertex_associated_track_cot.clear();
      fastAlg_FTKrefit_vertex_associated_track_eta.clear();
      fastAlg_FTKrefit_vertex_associated_track_nVerts.clear();
      fastAlg_FTKrefit_vertex_associated_track_VtxNumber.clear();    
      fastAlg_FTKrefit_vertex_number.clear();
      fastAlg_FTKrefit_vertex_associated_track_Pt.clear();
      fastAlg_FTKrefit_vertex_associated_track_eta.clear();
      fastAlg_FTKrefit_vertex_associated_track_theta.clear();
      isRefitFastHS.clear();  // currently does not work for fast algo, use sumpt2 for HSV
    }
    
    for (auto pvx = vxr->begin(); pvx != vxr->end(); pvx++, ivx++) {
      
      this_vertex_sumpt=0;
      
      ATH_MSG_VERBOSE( ivx << ": (x,y,z) = ( " <<
		       (*pvx)->position().x() << ", " <<
		       (*pvx)->position().y() << ", " <<
		       (*pvx)->position().z() <<
		       " ) Chi2: " << (*pvx)->chiSquared() <<
		       " nDoF " << (*pvx)->numberDoF() );
      
      if(m_fillTree){
	fastAlg_FTKrefit_vertex_nTrack.push_back((*pvx)->vxTrackAtVertex().size());
	fastAlg_FTKrefit_vertex_ndf.push_back((*pvx)->numberDoF());
	fastAlg_FTKrefit_vertex_chi2.push_back((*pvx)->chiSquared());
	fastAlg_FTKrefit_vertex_x_position.push_back((*pvx)->position().x());
	fastAlg_FTKrefit_vertex_y_position.push_back((*pvx)->position().y());
	fastAlg_FTKrefit_vertex_z_position.push_back((*pvx)->position().z());
	fastAlg_FTKrefit_vertex_x_error.push_back(sqrt((*pvx)->covariancePosition()(0,0)));
	fastAlg_FTKrefit_vertex_y_error.push_back(sqrt((*pvx)->covariancePosition()(1,1)));
	fastAlg_FTKrefit_vertex_z_error.push_back(sqrt((*pvx)->covariancePosition()(2,2)));
	if (fabs((*pvx)->numberDoF()) >= 1e-9) fastAlg_FTKrefit_vertex_chi2_over_ndf.push_back((*pvx)->chiSquared()/(*pvx)->numberDoF());       
	
	int isThisRefitFastHS = 0;
	if ((*pvx)->vertexType() == xAOD::VxType::PriVtx) isThisRefitFastHS = 1;
	isRefitFastHS.push_back(isThisRefitFastHS);
	
	
	for(vector<Trk::VxTrackAtVertex>::const_iterator ti=(*pvx)->vxTrackAtVertex().begin(); ti!=(*pvx)->vxTrackAtVertex().end(); ti++){
	  const Trk::Perigee* vertexPerigee = dynamic_cast <const Trk::Perigee*>(ti->perigeeAtVertex());
	  
	  float trk_theta = 999;
	  float trk_eta = 999;
	  float trk_phi0 = 999;
	  float trk_d0 = 999;
	  float trk_z0 = 999;
	  float trk_invPt = 999;
	  
	  if (vertexPerigee){
	    trk_theta = vertexPerigee->parameters()[Trk::theta];
	    trk_eta = -std::log(std::tan(trk_theta/2));
	    trk_phi0 =vertexPerigee->parameters()[Trk::phi0];
	    trk_d0 = vertexPerigee->parameters()[Trk::d0];
	    trk_z0 = vertexPerigee->parameters()[Trk::z0];
	    trk_invPt = vertexPerigee->parameters()[Trk::qOverP];
	  }
	  
	  if(m_fillTree){
	    fastAlg_FTKrefit_vertex_associated_track_theta.push_back(trk_theta);
	    fastAlg_FTKrefit_vertex_associated_track_phi0.push_back(trk_phi0);
	    fastAlg_FTKrefit_vertex_associated_track_d0.push_back(trk_d0);
	    fastAlg_FTKrefit_vertex_associated_track_z0.push_back(trk_z0);
	    if( fabs(sin(trk_theta)) >= 1e-9) fastAlg_FTKrefit_vertex_associated_track_invPt.push_back(1000*trk_invPt/sin(trk_theta));
	    if( fabs(1000*trk_invPt) >= 1e-9) fastAlg_FTKrefit_vertex_associated_track_Pt.push_back(sin(trk_theta)/(1000*trk_invPt));
	    fastAlg_FTKrefit_vertex_associated_track_eta.push_back(trk_eta);
	    fastAlg_FTKrefit_vertex_number.push_back(ivx);
	  }
	  
	  if( fabs(1000*trk_invPt) >= 1e-9) this_vertex_sumpt = fabs(this_vertex_sumpt+sin(trk_theta)/(1000*trk_invPt));
	  if((*pvx)->vxTrackAtVertex().size() > track_requirement) flag = true;
	  else flag = false;
	  
	}
	
	if(this_vertex_sumpt*this_vertex_sumpt >= fastAlg_FTKrefit_vertex_sumPt*fastAlg_FTKrefit_vertex_sumPt && flag == true ){
	  fastAlg_FTKrefit_vertex_sumPt = this_vertex_sumpt;
	  fastAlg_FTKrefit_vertex_sumPt2_vtxNumber = ivx;
	  
	}
	fastAlg_FTKrefit_vertex_associated_track_nVerts.push_back(vxr->size());
	
      }
      if(fabs(fastAlg_FTKrefit_vertex_sumPt) >= 1e-9) fastAlg_FTKrefit_vertex_sumPt2 = fastAlg_FTKrefit_vertex_sumPt*fastAlg_FTKrefit_vertex_sumPt;
    }
    delete(vxr);
  }
  else ATH_MSG_ERROR( "DataProviderSvc returned no valid vertices");
}

void FTK_RDO_ReaderAlgo::Fill_Converted_Vertices(unsigned int track_requirement){
  
  bool flag = false;
  
  ATH_MSG_DEBUG( " Getting xAOD::VertexContainer from Converted tracks from DataProviderSvc" );
  xAOD::VertexContainer* vxc = m_DataProviderSvc->getVertexContainer(false);
  if(vxc != 0 && vxc != nullptr){
    ATH_MSG_DEBUG( "DataProviderSvc returned " <<  vxc->size() << " xAOD:Vertex created from converted tracks" );
    
    float this_vertex_sumpt = 0;
    
    ATH_MSG_VERBOSE( " " );
    ATH_MSG_VERBOSE( " Printing information for " <<  vxc->size()<< " Vertex " );
    int ivx=0;
    
    if(m_fillTree){
      offlineAlg_FTKconverted_vertex_ndf.clear();
      offlineAlg_FTKconverted_vertex_chi2.clear();
      offlineAlg_FTKconverted_vertex_chi2_over_ndf.clear();
      offlineAlg_FTKconverted_vertex_x_position.clear();
      offlineAlg_FTKconverted_vertex_y_position.clear();
      offlineAlg_FTKconverted_vertex_z_position.clear();
      offlineAlg_FTKconverted_vertex_x_error.clear();
      offlineAlg_FTKconverted_vertex_y_error.clear();
      offlineAlg_FTKconverted_vertex_z_error.clear();
      offlineAlg_FTKconverted_vertex_nTrack.clear();
      offlineAlg_FTKconverted_vertex_number.clear();
      offlineAlg_FTKconverted_vertex_associated_track_phi0.clear();
      offlineAlg_FTKconverted_vertex_associated_track_d0.clear();
      offlineAlg_FTKconverted_vertex_associated_track_z0.clear();
      offlineAlg_FTKconverted_vertex_associated_track_invPt.clear();
      offlineAlg_FTKconverted_vertex_associated_track_cot.clear();
      offlineAlg_FTKconverted_vertex_associated_track_eta.clear();
      offlineAlg_FTKconverted_vertex_associated_track_nVerts.clear();
      offlineAlg_FTKconverted_vertex_associated_track_VtxNumber.clear();    
      offlineAlg_FTKconverted_vertex_number.clear();
      offlineAlg_FTKconverted_vertex_associated_track_Pt.clear();
      offlineAlg_FTKconverted_vertex_associated_track_eta.clear();
      offlineAlg_FTKconverted_vertex_associated_track_theta.clear();
      isConvertedOfflineHS.clear();
    }
    
    for (auto pvx = vxc->begin(); pvx != vxc->end(); pvx++, ivx++) {
      this_vertex_sumpt=0;
      
      ATH_MSG_VERBOSE( ivx << ": (x,y,z) = ( " <<
		       (*pvx)->position().x() << ", " <<
		       (*pvx)->position().y() << ", " <<
		       (*pvx)->position().z() <<
		       " ) Chi2: " << (*pvx)->chiSquared() <<
		       " nDoF " << (*pvx)->numberDoF() );
      
      if(m_fillTree){
	offlineAlg_FTKconverted_vertex_nTrack.push_back((*pvx)->vxTrackAtVertex().size());
	offlineAlg_FTKconverted_vertex_ndf.push_back((*pvx)->numberDoF());
	offlineAlg_FTKconverted_vertex_chi2.push_back((*pvx)->chiSquared());
	offlineAlg_FTKconverted_vertex_x_position.push_back((*pvx)->position().x());
	offlineAlg_FTKconverted_vertex_y_position.push_back((*pvx)->position().y());
	offlineAlg_FTKconverted_vertex_z_position.push_back((*pvx)->position().z());
	offlineAlg_FTKconverted_vertex_x_error.push_back(sqrt((*pvx)->covariancePosition()(0,0)));
	offlineAlg_FTKconverted_vertex_y_error.push_back(sqrt((*pvx)->covariancePosition()(1,1)));
	offlineAlg_FTKconverted_vertex_z_error.push_back(sqrt((*pvx)->covariancePosition()(2,2)));
	if (fabs((*pvx)->numberDoF()) >= 1e-9) offlineAlg_FTKconverted_vertex_chi2_over_ndf.push_back((*pvx)->chiSquared()/(*pvx)->numberDoF());       
	
	int isThisConvertedOfflineHS = 0;
	if ((*pvx)->vertexType() == xAOD::VxType::PriVtx) isThisConvertedOfflineHS = 1;
	isConvertedOfflineHS.push_back(isThisConvertedOfflineHS);
	
	for(vector<Trk::VxTrackAtVertex>::const_iterator ti=(*pvx)->vxTrackAtVertex().begin(); ti!=(*pvx)->vxTrackAtVertex().end(); ti++){
	  const Trk::Perigee* vertexPerigee = dynamic_cast <const Trk::Perigee*>(ti->perigeeAtVertex());
	  
	  float trk_theta = 999;
	  float trk_eta = 999;
	  float trk_phi0 = 999;
	  float trk_d0 = 999;
	  float trk_z0 = 999;
	  float trk_invPt = 999;
	  
	  if (vertexPerigee){
	    trk_theta = vertexPerigee->parameters()[Trk::theta];
	    trk_eta = -std::log(std::tan(trk_theta/2));
	    trk_phi0 =vertexPerigee->parameters()[Trk::phi0];
	    trk_d0 = vertexPerigee->parameters()[Trk::d0];
	    trk_z0 = vertexPerigee->parameters()[Trk::z0];
	    trk_invPt = vertexPerigee->parameters()[Trk::qOverP];
	  }
	  
	  if(m_fillTree){
	    offlineAlg_FTKconverted_vertex_associated_track_theta.push_back(trk_theta);
	    offlineAlg_FTKconverted_vertex_associated_track_phi0.push_back(trk_phi0);
	    offlineAlg_FTKconverted_vertex_associated_track_d0.push_back(trk_d0);
	    offlineAlg_FTKconverted_vertex_associated_track_z0.push_back(trk_z0);
	    if( fabs(sin(trk_theta)) >= 1e-9) offlineAlg_FTKconverted_vertex_associated_track_invPt.push_back(1000*trk_invPt/sin(trk_theta));
	    if( fabs(1000*trk_invPt) >= 1e-9) offlineAlg_FTKconverted_vertex_associated_track_Pt.push_back(sin(trk_theta)/(1000*trk_invPt));
	    offlineAlg_FTKconverted_vertex_associated_track_eta.push_back(trk_eta);
	    offlineAlg_FTKconverted_vertex_number.push_back(ivx);
	  }
	  
	  if( fabs(1000*trk_invPt) >= 1e-9) this_vertex_sumpt = fabs(this_vertex_sumpt+sin(trk_theta)/(1000*trk_invPt));
	  if((*pvx)->vxTrackAtVertex().size() > track_requirement) flag = true;
	  else flag = false;
	}
	
	if(this_vertex_sumpt*this_vertex_sumpt >= offlineAlg_FTKconverted_vertex_sumPt*offlineAlg_FTKconverted_vertex_sumPt && flag == true ){
	  offlineAlg_FTKconverted_vertex_sumPt = this_vertex_sumpt;
	  offlineAlg_FTKconverted_vertex_sumPt2_vtxNumber = ivx;
	}
	offlineAlg_FTKconverted_vertex_associated_track_nVerts.push_back(vxc->size());
      }
      if(fabs(offlineAlg_FTKconverted_vertex_sumPt) >= 1e-9) offlineAlg_FTKconverted_vertex_sumPt2 = offlineAlg_FTKconverted_vertex_sumPt*offlineAlg_FTKconverted_vertex_sumPt;
    }
    delete(vxc);
  }
  else ATH_MSG_ERROR( "DataProviderSvc returned no valid vertices");
}

void FTK_RDO_ReaderAlgo::Fill_Refit_Vertices(unsigned int track_requirement){
  
  bool flag = false;
  ATH_MSG_DEBUG( " Getting xAOD::VertexContainer from refit tracks from DataProviderSvc" );
  xAOD::VertexContainer* vxr = m_DataProviderSvc->getVertexContainer(true);
  if(vxr != 0 && vxr != nullptr){
    ATH_MSG_DEBUG( "DataProviderSvc returned " <<  vxr->size() << " xAOD:Vertex created from refit tracks" );
    
    float this_vertex_sumpt = 0;
  
    ATH_MSG_VERBOSE( " " );
    ATH_MSG_VERBOSE( " Printing information for " <<  vxr->size()<< " Vertex " );
    int ivx=0;
    
    
    if(m_fillTree){
      offlineAlg_FTKrefit_vertex_ndf.clear();
      offlineAlg_FTKrefit_vertex_chi2.clear();
      offlineAlg_FTKrefit_vertex_chi2_over_ndf.clear();
      offlineAlg_FTKrefit_vertex_x_position.clear();
      offlineAlg_FTKrefit_vertex_y_position.clear();
      offlineAlg_FTKrefit_vertex_z_position.clear();
      offlineAlg_FTKrefit_vertex_x_error.clear();
      offlineAlg_FTKrefit_vertex_y_error.clear();
      offlineAlg_FTKrefit_vertex_z_error.clear();
      offlineAlg_FTKrefit_vertex_nTrack.clear();
      offlineAlg_FTKrefit_vertex_number.clear();
      offlineAlg_FTKrefit_vertex_associated_track_phi0.clear();
      offlineAlg_FTKrefit_vertex_associated_track_d0.clear();
      offlineAlg_FTKrefit_vertex_associated_track_z0.clear();
      offlineAlg_FTKrefit_vertex_associated_track_invPt.clear();
      offlineAlg_FTKrefit_vertex_associated_track_cot.clear();
      offlineAlg_FTKrefit_vertex_associated_track_eta.clear();
      offlineAlg_FTKrefit_vertex_associated_track_nVerts.clear();
      offlineAlg_FTKrefit_vertex_associated_track_VtxNumber.clear();    
      offlineAlg_FTKrefit_vertex_number.clear();
      offlineAlg_FTKrefit_vertex_associated_track_Pt.clear();
      offlineAlg_FTKrefit_vertex_associated_track_eta.clear();
      offlineAlg_FTKrefit_vertex_associated_track_theta.clear();
      isRefitOfflineHS.clear();
    }
    
    for (auto pvx = vxr->begin(); pvx != vxr->end(); pvx++, ivx++) {
    
      this_vertex_sumpt=0;
      
      ATH_MSG_VERBOSE( ivx << ": (x,y,z) = ( " <<
		       (*pvx)->position().x() << ", " <<
		       (*pvx)->position().y() << ", " <<
		       (*pvx)->position().z() <<
		       " ) Chi2: " << (*pvx)->chiSquared() <<
		       " nDoF " << (*pvx)->numberDoF() );
      
      if(m_fillTree){
	offlineAlg_FTKrefit_vertex_nTrack.push_back((*pvx)->vxTrackAtVertex().size());
	offlineAlg_FTKrefit_vertex_ndf.push_back((*pvx)->numberDoF());
	offlineAlg_FTKrefit_vertex_chi2.push_back((*pvx)->chiSquared());
	offlineAlg_FTKrefit_vertex_x_position.push_back((*pvx)->position().x());
	offlineAlg_FTKrefit_vertex_y_position.push_back((*pvx)->position().y());
	offlineAlg_FTKrefit_vertex_z_position.push_back((*pvx)->position().z());
	offlineAlg_FTKrefit_vertex_x_error.push_back(sqrt((*pvx)->covariancePosition()(0,0)));
	offlineAlg_FTKrefit_vertex_y_error.push_back(sqrt((*pvx)->covariancePosition()(1,1)));
	offlineAlg_FTKrefit_vertex_z_error.push_back(sqrt((*pvx)->covariancePosition()(2,2)));
	if (fabs((*pvx)->numberDoF()) >= 1e-9) offlineAlg_FTKrefit_vertex_chi2_over_ndf.push_back((*pvx)->chiSquared()/(*pvx)->numberDoF());       
	
	int isThisRefitOfflineHS = 0;
	if ((*pvx)->vertexType() == xAOD::VxType::PriVtx) isThisRefitOfflineHS = 1;
	isRefitOfflineHS.push_back(isThisRefitOfflineHS);
	
	
	for(vector<Trk::VxTrackAtVertex>::const_iterator ti=(*pvx)->vxTrackAtVertex().begin(); ti!=(*pvx)->vxTrackAtVertex().end(); ti++){
	  const Trk::Perigee* vertexPerigee = dynamic_cast <const Trk::Perigee*>(ti->perigeeAtVertex());
	  
	  float trk_theta = 999;
	  float trk_eta = 999;
	  float trk_phi0 = 999;
	  float trk_d0 = 999;
	  float trk_z0 = 999;
	  float trk_invPt = 999;
	  
	  if (vertexPerigee){
	    trk_theta = vertexPerigee->parameters()[Trk::theta];
	    trk_eta = -std::log(std::tan(trk_theta/2));
	    trk_phi0 =vertexPerigee->parameters()[Trk::phi0];
	    trk_d0 = vertexPerigee->parameters()[Trk::d0];
	    trk_z0 = vertexPerigee->parameters()[Trk::z0];
	    trk_invPt = vertexPerigee->parameters()[Trk::qOverP];
	  }
	  
	  if(m_fillTree){
	    offlineAlg_FTKrefit_vertex_associated_track_theta.push_back(trk_theta);
	    offlineAlg_FTKrefit_vertex_associated_track_phi0.push_back(trk_phi0);
	    offlineAlg_FTKrefit_vertex_associated_track_d0.push_back(trk_d0);
	    offlineAlg_FTKrefit_vertex_associated_track_z0.push_back(trk_z0);
	    if( fabs(sin(trk_theta)) >= 1e-9) offlineAlg_FTKrefit_vertex_associated_track_invPt.push_back(1000*trk_invPt/sin(trk_theta));
	    if( fabs(1000*trk_invPt) >= 1e-9) offlineAlg_FTKrefit_vertex_associated_track_Pt.push_back(sin(trk_theta)/(1000*trk_invPt));
	    offlineAlg_FTKrefit_vertex_associated_track_eta.push_back(trk_eta);
	    offlineAlg_FTKrefit_vertex_number.push_back(ivx);
	  }
	  
	  if( fabs(1000*trk_invPt) >= 1e-9) this_vertex_sumpt = fabs(this_vertex_sumpt+sin(trk_theta)/(1000*trk_invPt));
	  if((*pvx)->vxTrackAtVertex().size() > track_requirement) flag = true;
	  else flag = false;
	}
	
	if(this_vertex_sumpt*this_vertex_sumpt >= offlineAlg_FTKrefit_vertex_sumPt*offlineAlg_FTKrefit_vertex_sumPt && flag == true ){
	  offlineAlg_FTKrefit_vertex_sumPt = this_vertex_sumpt;
	  offlineAlg_FTKrefit_vertex_sumPt2_vtxNumber = ivx;
	}
	offlineAlg_FTKrefit_vertex_associated_track_nVerts.push_back(vxr->size());
      }
      if(fabs(offlineAlg_FTKrefit_vertex_sumPt) >= 1e-9) offlineAlg_FTKrefit_vertex_sumPt2 = offlineAlg_FTKrefit_vertex_sumPt*offlineAlg_FTKrefit_vertex_sumPt;
    }
    delete(vxr);
  }
  else ATH_MSG_ERROR( "DataProviderSvc returned no valid vertices");
}


void FTK_RDO_ReaderAlgo::Fill_Offline_Vertices(unsigned int track_requirement){
  bool flag = false;
  const xAOD::VertexContainer *offlineVertices = 0;
  float this_vertex_sumpt = 0;
  float this_vertex_sumpt2 = 0;
  
  if ( evtStore()->retrieve(offlineVertices,"PrimaryVertices").isFailure() || !offlineVertices ) {
    ATH_MSG_ERROR("Could not retrieve offline algo primary vertices");
  }
  else {
    
    int ivertexc = 0;
    if(m_fillTree){
      offlineAlg_offlineTracks_vertex_x_position.clear();
      offlineAlg_offlineTracks_vertex_y_position.clear();
      offlineAlg_offlineTracks_vertex_z_position.clear();
      offlineAlg_offlineTracks_vertex_x_error.clear();
      offlineAlg_offlineTracks_vertex_y_error.clear();
      offlineAlg_offlineTracks_vertex_z_error.clear();
      offlineAlg_offlineTracks_vertex_chi2.clear();
      offlineAlg_offlineTracks_vertex_nTrack.clear();
      offlineAlg_offlineTracks_vertex_ndf.clear();
      offlineAlg_offlineTracks_vertex_chi2_over_ndf.clear();
      offlineAlg_offlineTracks_vertex_number.clear();
      offlineAlg_offlineTracks_vertex_associated_track_theta.clear();
      offlineAlg_offlineTracks_vertex_associated_track_eta.clear();
      offlineAlg_offlineTracks_vertex_associated_track_phi0.clear();
      offlineAlg_offlineTracks_vertex_associated_track_z0.clear();
      offlineAlg_offlineTracks_vertex_associated_track_d0.clear();
      offlineAlg_offlineTracks_vertex_associated_track_invPt.clear();
      offlineAlg_offlineTracks_vertex_associated_track_Pt.clear();
      offlineAlg_offlineTracks_vertex_associated_track_nVerts.clear();
      offlineAlg_offlineTracks_vertex_associated_track_VtxNumber.clear();
      isOfflineOfflineHS.clear();
    }
    
    for (auto pv = offlineVertices->begin(); pv != offlineVertices->end(); pv++, ivertexc++) {
      
      this_vertex_sumpt = 0;
      this_vertex_sumpt2 = 0;
      
      ATH_MSG_VERBOSE( ivertexc << " Offline Alg, Offline Tracs: (x,y,z)= ( " << (*pv)->position().x() << ", " <<  (*pv)->position().y() << ", " << (*pv)->position().z() <<
		       " ) chi2: " << (*pv)->chiSquared() << " Ndof: "<< (*pv)->numberDoF() <<
		       " VertexType: " << this->strVertexType((*pv)->vertexType()));
      //  if(m_fillTree){
	offlineAlg_offlineTracks_vertex_nTrack.push_back((*pv)->vxTrackAtVertex().size());
	offlineAlg_offlineTracks_vertex_ndf.push_back((*pv)->numberDoF());
	offlineAlg_offlineTracks_vertex_chi2.push_back((*pv)->chiSquared());
	offlineAlg_offlineTracks_vertex_x_position.push_back((*pv)->position().x());
	offlineAlg_offlineTracks_vertex_y_position.push_back((*pv)->position().y());
	offlineAlg_offlineTracks_vertex_z_position.push_back((*pv)->position().z());
	offlineAlg_offlineTracks_vertex_x_error.push_back(sqrt((*pv)->covariancePosition()(0,0)));
	offlineAlg_offlineTracks_vertex_y_error.push_back(sqrt((*pv)->covariancePosition()(1,1)));
	offlineAlg_offlineTracks_vertex_z_error.push_back(sqrt((*pv)->covariancePosition()(2,2)));
	if(fabs((*pv)->numberDoF()) >=1e-9) offlineAlg_offlineTracks_vertex_chi2_over_ndf.push_back((*pv)->chiSquared()/(*pv)->numberDoF());
	
	int isThisOfflineOfflineHS = 0;
	if ((*pv)->vertexType() == xAOD::VxType::PriVtx) isThisOfflineOfflineHS = 1;
	isOfflineOfflineHS.push_back(isThisOfflineOfflineHS);
	
	
	int offline_nverts_counter = 0;
	
	for(vector<Trk::VxTrackAtVertex>::const_iterator ti=(*pv)->vxTrackAtVertex().begin(); ti!=(*pv)->vxTrackAtVertex().end(); ti++){
	  const Trk::Perigee* vertexPerigee = dynamic_cast <const Trk::Perigee*>(ti->perigeeAtVertex());
	  
	  
	  //initalizaiton                                                                              
	  float ol_trk_theta = 999;
	  float ol_trk_eta = 999;
	  float ol_trk_phi0 = 999;
	  float ol_trk_d0 = 999;
	  float ol_trk_z0 = 999;
	  float ol_trk_invPt = 999;
	  
	  if (vertexPerigee){
	    ol_trk_theta = vertexPerigee->parameters()[Trk::theta];
	    ol_trk_eta = -std::log(std::tan(ol_trk_theta/2));
	    ol_trk_phi0 = vertexPerigee->parameters()[Trk::phi0];
	    ol_trk_d0 = vertexPerigee->parameters()[Trk::d0];
	    ol_trk_z0 = vertexPerigee->parameters()[Trk::z0];
	    ol_trk_invPt = vertexPerigee->parameters()[Trk::qOverP];
	  }
	  
	  
	  offlineAlg_offlineTracks_vertex_associated_track_theta.push_back(ol_trk_theta);
	  offlineAlg_offlineTracks_vertex_associated_track_phi0.push_back(ol_trk_phi0);
	  offlineAlg_offlineTracks_vertex_associated_track_d0.push_back(ol_trk_d0);
	  offlineAlg_offlineTracks_vertex_associated_track_z0.push_back(ol_trk_z0);
	  if(fabs(sin(ol_trk_theta)) >= 1e-9) offlineAlg_offlineTracks_vertex_associated_track_invPt.push_back(1000*ol_trk_invPt/sin(ol_trk_theta));
	  if(fabs(1000*ol_trk_invPt) >= 1e-9) offlineAlg_offlineTracks_vertex_associated_track_Pt.push_back(sin(ol_trk_theta)/(1000*ol_trk_invPt));
	  offlineAlg_offlineTracks_vertex_associated_track_eta.push_back(ol_trk_eta);
	  offlineAlg_offlineTracks_vertex_associated_track_VtxNumber.push_back(ivertexc);
	  offline_nverts_counter++;
	  
	  offlineAlg_offlineTracks_vertex_number.push_back(ivertexc);
	  
	  float pT = sin(ol_trk_theta)/(1000*ol_trk_invPt);
	  this_vertex_sumpt = fabs(this_vertex_sumpt+pT);
	  this_vertex_sumpt2 = this_vertex_sumpt2+pT*pT;
	  
	  if((*pv)->vxTrackAtVertex().size() > track_requirement) flag = true;
	  else flag = false;
	  
	}
	offlineAlg_offlineTracks_vertex_associated_track_nVerts.push_back(offline_nverts_counter);
	
	if(this_vertex_sumpt2 >= offlineAlg_offlineTracks_vertex_sumPt2 && flag == true){
	  offlineAlg_offlineTracks_vertex_sumPt2 = this_vertex_sumpt2;
	  offlineAlg_offlineTracks_vertex_sumPt2_vtxNumber = ivertexc;
	}
    }
    //}
    offlineAlg_offlineTracks_vertex_sumPt2 = offlineAlg_offlineTracks_vertex_sumPt*offlineAlg_offlineTracks_vertex_sumPt;   
  }
}

void FTK_RDO_ReaderAlgo::Fill_Truth_Vtx(){
  if(m_fillTree){
     const xAOD::TruthVertexContainer* importedTruthVertices;

     if (evtStore()->retrieve(importedTruthVertices,m_verticesKey).isFailure()) {
       ATH_MSG_ERROR("No TruthVertexContainer with name " << m_verticesKey << " found in StoreGate!");
     }
    
     if (importedTruthVertices->size() !=0 ){

       const xAOD::TruthVertex *vertex =  (*importedTruthVertices)[0];
       
       truth_x0 = vertex->x();
       truth_y0 = vertex->y();
       truth_z0 = vertex->z();

     }
  }
}




std::string FTK_RDO_ReaderAlgo::strVertexType( const Trk::VertexType vxtype) {
  std::string vxstring = "unknown";
  switch (vxtype) {
    case Trk::NoVtx:
      vxstring="NoVtx";
      break;
    case Trk::PriVtx:
      vxstring="PriVtx";
      break;
    case Trk::SecVtx:
      vxstring="SecVtx";
      break;
    case Trk::PileUp:
      vxstring="PileUp";
      break;
    case Trk::ConvVtx:
      vxstring="ConvVtx";
      break;
    case Trk::V0Vtx:
      vxstring="V0Vtx";
      break;
    case Trk::KinkVtx:
      vxstring="KinkVtx";
      break;
    case Trk::V0Lambda:
      vxstring="V0LambdaVtx";
      break;
    case Trk::V0LambdaBar:
      vxstring="V0LambdaBar";
      break;
    case Trk::V0KShort:
      vxstring="V0KShort";
      break;
    case Trk::NotSpecified:
      vxstring="NotSpecified";
      break;
    default:
      vxstring = "unknown";
  }
  return vxstring;
}

std::string FTK_RDO_ReaderAlgo::strVertexType( const xAOD::VxType::VertexType vxtype) {
  std::string vxstring = "unknown";
  switch (vxtype) {
    case xAOD::VxType::NoVtx:
      vxstring="NoVtx";
      break;
    case xAOD::VxType::PriVtx:
      vxstring="PriVtx";
      break;
    case xAOD::VxType::SecVtx:
      vxstring="SecVtx";
      break;
    case xAOD::VxType::PileUp:
      vxstring="PileUp";
      break;
    case xAOD::VxType::ConvVtx:
      vxstring="ConvVtx";
      break;
    case xAOD::VxType::V0Vtx:
      vxstring="V0Vtx";
      break;
    case xAOD::VxType::KinkVtx:
      vxstring="KinkVtx";
      break;
    case xAOD::VxType::NotSpecified:
      vxstring="NotSpecified";
      break;
    default:
      vxstring = "unknown";
  }
  return vxstring;
}


