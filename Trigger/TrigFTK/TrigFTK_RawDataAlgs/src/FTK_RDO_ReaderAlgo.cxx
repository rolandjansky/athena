/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTK_RawDataAlgs/FTK_RDO_ReaderAlgo.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/ITHistSvc.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "VxVertex/VxContainer.h"
#include "TrkEventPrimitives/VertexType.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackingPrimitives.h"

#include "TMath.h"
#define NINT(a) ((a) >= 0.0 ? (int)((a)+0.5) : (int)((a)-0.5))

using namespace std;

/////////////////////////////////////////////////////////////////////////////
FTK_RDO_ReaderAlgo::FTK_RDO_ReaderAlgo(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),

  m_getRawTracks(true),
  m_ftk_raw_trackcollection_Name("FTK_RDO_Tracks"),
  m_getTracks(true), //  m_TrackCollectionName("FTK_TrackCollection"),
  m_getRefitTracks(true), // m_refitTrackCollectionName("FTKrefit_TrackCollection"),
  m_getTrackParticles(true), // m_TrackParticleCollectionName("FTK_TrackParticleCollection"),
  m_getRefitTrackParticles(true), //m_refitTrackParticleCollectionName("FTKrefit_TrackParticleCollection"),
  m_getRawVxVertex(false), //m_VxContainerName("FTK_VxContainer"),
  m_getVxVertex(true), //m_VxContainerName("FTK_VxContainer"),
  m_getRefitVxVertex(true), //m_refitVxContainerName("FTKrefit_VxContainer"),
  m_getVertex(true), //m_VertexContainerName("FTK_VertexContainer"),
  m_getRefitVertex(true), //m_refitVertexContainerName("FTKrefit_VertexContainer"),
  m_DataProviderSvc("IFTK_DataProviderSvc/IFTK_DataProviderSvc", name),
  m_fillHists(false)
{
  declareProperty("RDO_CollectionName",m_ftk_raw_trackcollection_Name,
      "Collection name of RDO");
  declareProperty("GetRawTracks",m_getRawTracks);
  declareProperty("GetTracks",m_getTracks);
  declareProperty("GetRefitTracks",m_getRefitTracks);
  declareProperty("GetTrackParticles",m_getTrackParticles);
  declareProperty("GetRefitTrackParticles",m_getRefitTrackParticles=true);
  declareProperty("GetRawVxVertex",m_getRawVxVertex);
  declareProperty("GetVxVertex",m_getVxVertex);
  declareProperty("GetRefitVxVertex",m_getRefitVxVertex);
  declareProperty("GetVertex",m_getVertex);
  declareProperty("GetRefitVertex",m_getRefitVertex);


  declareProperty("FTK_DataProvider", m_DataProviderSvc);
  declareProperty("fillHists", m_fillHists);
}

FTK_RDO_ReaderAlgo::~FTK_RDO_ReaderAlgo()
{ }

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode FTK_RDO_ReaderAlgo::initialize(){
  MsgStream athlog(msgSvc(), name());
  ATH_MSG_INFO("FTK_RDO_ReaderAlgo::initialize()" );

  ATH_CHECK( service("StoreGateSvc", m_StoreGate ));
  ATH_MSG_INFO("Input RDO collection" << m_ftk_raw_trackcollection_Name);
  if (m_getRawTracks) ATH_MSG_INFO("Getting FTK_RawTracks from Storegate");
  if (m_getTracks) ATH_MSG_INFO("Getting FTK Trk::Tracks from "<< m_DataProviderSvc);
  if (m_getRefitTracks) ATH_MSG_INFO("Getting FTK Refit Trk::Tracks from "<< m_DataProviderSvc);
  if (m_getTrackParticles) ATH_MSG_INFO("Getting FTK xAOD::TrackParticles from "<< m_DataProviderSvc);
  if (m_getRefitTrackParticles) ATH_MSG_INFO("Getting FTK Refit xAOD::TrackParticles from "<< m_DataProviderSvc);
  if (m_getRawVxVertex) ATH_MSG_INFO("Getting FTK_RawTrack VxVertex from "<< m_DataProviderSvc);
  if (m_getVxVertex) ATH_MSG_INFO("Getting FTK VxVertex from "<< m_DataProviderSvc);
  if (m_getRefitVxVertex) ATH_MSG_INFO("Getting FTK Refit VxVertex from "<< m_DataProviderSvc);
  if (m_getVertex) ATH_MSG_INFO("Getting FTK xAOD::Vertex from "<< m_DataProviderSvc);
  if (m_getRefitVertex) ATH_MSG_INFO("Getting FTK Refit xAOD::Vertex  from "<< m_DataProviderSvc);

  //Set up the FTK Data Provider SVC //
  ATH_CHECK(m_DataProviderSvc.retrieve());

  if (m_fillHists) {
    /// Get Histogram Service ///
    ATH_CHECK(service("THistSvc", rootHistSvc));

    //Book Histograms//
    double PI = TMath::Pi();
    std::vector<TH1D*> histograms;

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

    histograms.push_back(h_FTK_RawTrack_n);
    histograms.push_back(h_FTK_RawTrack_phi);
    histograms.push_back(h_FTK_RawTrack_d0);
    histograms.push_back(h_FTK_RawTrack_z0);
    histograms.push_back(h_FTK_RawTrack_invPt);
    histograms.push_back(h_FTK_RawTrack_cot);
    histograms.push_back(h_FTK_RawTrack_eta);
    histograms.push_back(h_FTK_RawTrack_nPix);
    histograms.push_back(h_FTK_RawTrack_nSCT);

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

    histograms.push_back(h_Track_n);
    histograms.push_back(h_Track_phi);
    histograms.push_back(h_Track_d0);
    histograms.push_back(h_Track_z0);
    histograms.push_back(h_Track_invPt);
    histograms.push_back(h_Track_cot);
    histograms.push_back(h_Track_eta);
    histograms.push_back(h_Track_nPix);
    histograms.push_back(h_Track_nSCT);

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

    histograms.push_back(h_refitTrack_n);
    histograms.push_back(h_refitTrack_phi);
    histograms.push_back(h_refitTrack_d0);
    histograms.push_back(h_refitTrack_z0);
    histograms.push_back(h_refitTrack_invPt);
    histograms.push_back(h_refitTrack_cot);
    histograms.push_back(h_refitTrack_eta);
    histograms.push_back(h_refitTrack_nPix);
    histograms.push_back(h_refitTrack_nSCT);

    std::string trackRootPath = "/TRACKS/";

    for( uint i = 0; i < histograms.size(); ++i){
      if ( rootHistSvc->regHist( trackRootPath + histograms[i]->GetName(), histograms[i] ).isFailure()) {
        std::string temp = "Failed to book Histogram: " + std::string(histograms[i]->GetName());
        ATH_MSG_WARNING(temp);
      } else {
        std::string temp = "Booked Histogram: " + std::string(histograms[i]->GetName());
        ATH_MSG_INFO(temp);
      }
    }
  }
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
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

  //
  // Get RAW tracks
  //
  if (m_getRawTracks) {

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
      
      // Fill RAW track histograms
      
      if (m_fillHists) {
	h_FTK_RawTrack_n->Fill(rawTracks->size());
	
	
	FTK_RawTrackContainer::const_iterator pTrack = rawTracks->begin();
	FTK_RawTrackContainer::const_iterator pLastTrack = rawTracks->end();
	
	for ( int itr=0; pTrack!= pLastTrack; pTrack++, itr++) {
	  
	  float trkTheta = std::atan2(1.0,(*pTrack)->getCotTh());
	  float trkEta = -std::log(std::tan(trkTheta/2.));
	  float trkPt=1.e10;
	  if ((*pTrack)->getInvPt() !=0) trkPt=1./(*pTrack)->getInvPt();
	  
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
	  //if (msgLvl(MSG::VERBOSE)) {
	  //ATH_MSG_INFO
	  /*std::cout << "FTK_RawTrack Parameter Summary: " << std::endl;
	    std::cout << " - Road ID = " << (*pTrack)->getRoadID() << std::endl;
	    std::cout << " - Road ID = " << (*pTrack)->getTH1() << std::endl;
	    std::cout << " - d0      = " << (*pTrack)->getD0()  << std::endl;
	    std::cout << " - z0      = " << (*pTrack)->getZ0() << std::endl;
	    std::cout << " - phi     = " << (*pTrack)->getPhi()   << std::endl;
	    std::cout << " - cot     = " << (*pTrack)->getCotTh()  << std::endl;
	    std::cout << " - ipt     = " << (*pTrack)->getInvPt()   << std::endl;
	    std::cout << " - barcode = " << (*pTrack)->getBarcode() << std::endl;*/
	  /*for( unsigned int i = 0; i < (*pTrack)->getPixelClusters().size(); ++i){
	    std::cout << " PIXEL Cluster " << i << std::endl;
	    std::cout << "       Layer      = " << (*pTrack)->getPixelClusters()[i].getLayer() << std::endl;
	    std::cout << "       Col Coord  = " << (*pTrack)->getPixelClusters()[i].getColCoord() << std::endl;
	    std::cout << "       Col Width  = " << (*pTrack)->getPixelClusters()[i].getColWidth() << std::endl;
	    std::cout << "       Row Coord  = " << (*pTrack)->getPixelClusters()[i].getRowCoord() << std::endl;
	    std::cout << "       Row Width  = " << (*pTrack)->getPixelClusters()[i].getRowWidth() << std::endl;
	    std::cout << "       Hash ID    = " << (*pTrack)->getPixelClusters()[i].getModuleID() << std::endl;
	    }
	    for( unsigned int i = 0; i < (*pTrack)->getSCTClusters().size(); ++i){
	    std::cout << " SCT Cluster " << i << std::endl;
	    std::cout << "       Layer        = " << (*pTrack)->getSCTClusters()[i].getLayer() << std::endl;
	    std::cout << "       Hit Coord    = " << (*pTrack)->getSCTClusters()[i].getHitCoord() << std::endl;
	    std::cout << "       Hit Width    = " << (*pTrack)->getSCTClusters()[i].getHitWidth() << std::endl;
	    std::cout << "       Module ID    = " << (*pTrack)->getSCTClusters()[i].getModuleID() << std::endl;
	    } // end loop over i
	    std::cout << std::endl;*/
	  //} // end if (msgLvl(MSG::VERBOSE))
	} // end loop over tracks
      }
    }
  }

  //
  // Get Converted Tracks
  //
  if (m_getTracks) {
    ATH_MSG_DEBUG("Getting converted tracks from DataProviderSvc");
    TrackCollection *track_collection = m_DataProviderSvc->getTracks(false);
    ATH_MSG_DEBUG("DataProviderSvc returned " <<  track_collection->size() << " converted tracks");
    //  if ( evtStore()->record ( track_collection, m_TrackCollectionName).isFailure() ) {
    //	 ATH_MSG_DEBUG("Failed to record FTK TrackCollection with name " <<  m_TrackCollectionName);
    //  }

    ATH_MSG_VERBOSE( " Printing information for " << track_collection->size()<< " FTK tracks" );
    if (m_fillHists)h_Track_n->Fill(track_collection->size());
    TrackCollection::const_iterator track_it   = track_collection->begin();
    TrackCollection::const_iterator last_track = track_collection->end();
    int iTrack=0;
    for ( ; track_it!= last_track; track_it++, iTrack++) {
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
        if (tanTheta!=0) cotTheta= 1./tanTheta;
        h_Track_cot->Fill(cotTheta);

        h_Track_nPix->Fill(nPix);
        h_Track_nSCT->Fill(nSCT);
      }
      double trkPt = 1.e10;
      if ((*track_it)->perigeeParameters()->parameters()[Trk::qOverP]!=0) trkPt= sin((*track_it)->perigeeParameters()->parameters()[Trk::theta])/(*track_it)->perigeeParameters()->parameters()[Trk::qOverP];
      ATH_MSG_VERBOSE( " Track "<< iTrack << ": pT: "<< trkPt <<
          " eta: " <<  -log(std::tan((*track_it)->perigeeParameters()->parameters()[Trk::theta]/2.)) <<
          " phi: " <<  (*track_it)->perigeeParameters()->parameters()[Trk::phi0] <<
          " d0: " << (*track_it)->perigeeParameters()->parameters()[Trk::d0] <<
          " z0: " << (*track_it)->perigeeParameters()->parameters()[Trk::z0] <<
          " nPix: " << nPix << " nSCT: " << nSCT );

    }
    delete(track_collection);
  }


  //
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
      if ((*ptp)->qOverP() !=0) p = 1./(*ptp)->qOverP();
      ATH_MSG_VERBOSE( "itp:" << itpc << ": q*pT " << (*ptp)->pt()*(*ptp)->charge() << " eta " << (*ptp)->eta() << " phi " << (*ptp)->phi0() <<
          " d0: " << (*ptp)->d0() << " z0: " << (*ptp)->z0() << " p " << p );
    }
    delete (tpc);
  }


  //
  // Get VxVertex from raw tracks
  //
  if (m_getRawVxVertex) {
    ATH_MSG_DEBUG( " Getting VxContainer from Raw tracks from DataProviderSvc" );
    VxContainer* vxr = m_DataProviderSvc->getVxContainer(ftk::RawTrackType);
    ATH_MSG_DEBUG( "DataProviderSvc returned " <<  vxr->size() << " VxCandidates created from raw tracks" );
    //       if ( evtStore()->get ( vxc, m_VxContainerName).isFailure() ) {
    //	 ATH_MSG_DEBUG( "Failed to get FTK VxContainer with name " <<  m_VxContainerName );
    //       }

    ATH_MSG_VERBOSE( " " );
    ATH_MSG_VERBOSE( " Printing information for " <<  vxr->size()<< " VxVertex " );
    int ivx=0;
    for (auto pvx = vxr->begin(); pvx != vxr->end(); pvx++, ivx++) {
      ATH_MSG_VERBOSE( ivx << ": (x,y,z) = ( " <<
          (*pvx)->recVertex().position().x() << ", " <<
          (*pvx)->recVertex().position().y() << ", " <<
          (*pvx)->recVertex().position().z() <<
          " ) Chi2: " << (*pvx)->recVertex().fitQuality().chiSquared() <<
          " nDoF " << (*pvx)->recVertex().fitQuality().numberDoF() );
    }
    delete(vxr);
  }


  //
  // Get VxVertex from converted tracks
  //
  if (m_getVxVertex) {
    ATH_MSG_DEBUG( " Getting VxContainer from Converted tracks from DataProviderSvc" );
    VxContainer* vxc = m_DataProviderSvc->getVxContainer(ftk::ConvertedTrackType);
    ATH_MSG_DEBUG( "DataProviderSvc returned " <<  vxc->size() << " VxCandidates created from converted tracks" );
    //       if ( evtStore()->get ( vxc, m_VxContainerName).isFailure() ) {
    //	 ATH_MSG_DEBUG( "Failed to get FTK VxContainer with name " <<  m_VxContainerName );
    //       }

    ATH_MSG_VERBOSE( " " );
    ATH_MSG_VERBOSE( " Printing information for " <<  vxc->size()<< " VxVertex " );
    int ivx=0;
    for (auto pvx = vxc->begin(); pvx != vxc->end(); pvx++, ivx++) {

      ATH_MSG_VERBOSE( ivx << ": (x,y,z) = ( " <<
          (*pvx)->recVertex().position().x() << ", " <<
          (*pvx)->recVertex().position().y() << ", " <<
          (*pvx)->recVertex().position().z() <<
          " ) Chi2: " << (*pvx)->recVertex().fitQuality().chiSquared() <<
          " nDoF " << (*pvx)->recVertex().fitQuality().numberDoF() <<
          " VertexType: " << this->strVertexType((*pvx)->vertexType()));
    }
    delete (vxc);
  }


  //
  // Get xAoD::Vertex from converted tracks
  //
  if (m_getVertex) {
    ATH_MSG_DEBUG( " Getting xAOD::VertexContainer from Converted tracks from DataProviderSvc" );
    xAOD::VertexContainer* vertexc = m_DataProviderSvc->getVertexContainer(false);
    ATH_MSG_DEBUG( "DataProviderSvc returned " <<  vertexc->size() << " zAOD:Vertex created from converted tracks" );
    //       if ( evtStore()->get ( vertexc, m_VertexContainerName).isFailure() ) {
    //	 ATH_MSG_DEBUG( "Failed to get FTK VertexContainer with name " <<  m_VertexContainerName );
    //       }


    ATH_MSG_VERBOSE( " " );
    ATH_MSG_VERBOSE( " Printing information for " <<  vertexc->size()<< " xAOD::Vertex" );

    int ivertexc = 0;
    for (auto pv = vertexc->begin(); pv != vertexc->end(); pv++, ivertexc++) {
      ATH_MSG_VERBOSE( ivertexc << ": (x,y,z)= ( " << (*pv)->position().x() << ", " <<  (*pv)->position().y() << ", " << (*pv)->position().z() <<
          " ) chi2: " << (*pv)->chiSquared() << " Ndof: "<< (*pv)->numberDoF() <<
          " VertexType: " << this->strVertexType((*pv)->vertexType()));


    }
    delete (vertexc);
  }

  //
  // Get refitted Tracks
  //
  if (m_getRefitTracks) {
    ATH_MSG_DEBUG( " Getting refitted tracks from DataProviderSvc" );
    TrackCollection *refitTracks  = m_DataProviderSvc->getTracks(true);
    ATH_MSG_DEBUG( "DataProviderSvc returned " <<  refitTracks->size() << " refitted tracks" );
    //       if ( evtStore()->get ( refitTracks, m_refitTrackCollectionName).isFailure() ) {
    //	 ATH_MSG_DEBUG( "Failed to get refit FTK TrackCollection with name " <<  m_refitTrackCollectionName );
    //       }

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
        if (tanTheta !=0) cotTheta=1./tanTheta;
        h_Track_cot->Fill(cotTheta);
        h_Track_nPix->Fill(nPix);
        h_Track_nSCT->Fill(nSCT);
      }
      double trkPt = 1.e10;
      if ((*track_it)->perigeeParameters()->parameters()[Trk::qOverP]!=0) trkPt= sin((*track_it)->perigeeParameters()->parameters()[Trk::theta])/(*track_it)->perigeeParameters()->parameters()[Trk::qOverP];

      ATH_MSG_VERBOSE( " Track" << iTrack << ": pT: "<< trkPt <<
          " eta: " <<  -std::log(std::tan((*track_it)->perigeeParameters()->parameters()[Trk::theta]/2.)) <<
          " phi: " <<  (*track_it)->perigeeParameters()->parameters()[Trk::phi0] <<
          " d0: " << (*track_it)->perigeeParameters()->parameters()[Trk::d0] <<
          " z0: " << (*track_it)->perigeeParameters()->parameters()[Trk::z0] <<
          " nPix: " << nPix << " nSCT: " << nSCT );
    }
    delete (refitTracks);
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

  //
  // Get VxVertex from refitted tracks
  //
  if (m_getRefitVxVertex) {
    ATH_MSG_DEBUG( " Getting VxContainer from refitted tracks from DataProviderSvc" );
    VxContainer* vxr = m_DataProviderSvc->getVxContainer(ftk::RefittedTrackType);
    ATH_MSG_DEBUG( "DataProviderSvc returned " <<  vxr->size() << " VxCandidates created from refitted tracks" );
    //       if ( evtStore()->get ( vxr, m_refitVxContainerName).isFailure() ) {
    //	 ATH_MSG_DEBUG( "Failed to get refit FTK VxContainer with name " <<  m_refitVxContainerName );
    //       }

    ATH_MSG_VERBOSE( " " );
    ATH_MSG_VERBOSE( " Printing information for " <<  vxr->size()<< " VxVertex from refitted tracks" );
    int ivxr = 0;
    for (auto pvx = vxr->begin(); pvx != vxr->end(); pvx++, ivxr++) {
      ATH_MSG_VERBOSE( ivxr << ": (x,y,z)= ( " << (*pvx)->recVertex().position().x() << ", " <<  (*pvx)->recVertex().position().y() << ", " << (*pvx)->recVertex().position().z()
          << " ) Chi2: " << (*pvx)->recVertex().fitQuality().chiSquared() << " nDoF " << (*pvx)->recVertex().fitQuality().numberDoF() <<
          " VertexType: " << this->strVertexType((*pvx)->vertexType()));
    }
    delete (vxr);
  }

  //
  // Get xAoD::Vertex from converted tracks
  //
  if (m_getRefitVertex) {
    ATH_MSG_DEBUG( " Getting xAOD::VertexContainer from Converted tracks from DataProviderSvc" );
    xAOD::VertexContainer* vertexr = m_DataProviderSvc->getVertexContainer(true);
    ATH_MSG_DEBUG( "DataProviderSvc returned " <<  vertexr->size() << " xAOD:Vertex created from refitted tracks" );

    //       if ( evtStore()->get ( vertexr, m_refitVertexContainerName).isFailure() ) {
    //	 ATH_MSG_DEBUG( "Failed to get refit FTK VertexContainer with name " <<  m_refitVertexContainerName );
    //       }

    ATH_MSG_VERBOSE( " " );
    ATH_MSG_VERBOSE( " Printing information for " <<  vertexr->size()<< " xAOD::Vertex from refitted tracks" );
    int ivertexr = 0;
    for (auto pv = vertexr->begin(); pv != vertexr->end(); pv++, ivertexr++) {
      ATH_MSG_VERBOSE( ivertexr << ": (x,y,z)= ( " << (*pv)->position().x() << ", " <<  (*pv)->position().y() << ", " << (*pv)->position().z()<<
          " ) chi2: " << (*pv)->chiSquared() << " Ndof: "<< (*pv)->numberDoF() <<
          " VertexType: " << this->strVertexType((*pv)->vertexType()));
    }
    delete(vertexr);
  }


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
