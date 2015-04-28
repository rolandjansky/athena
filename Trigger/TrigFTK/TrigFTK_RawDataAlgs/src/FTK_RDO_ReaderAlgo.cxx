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
#include "xAODTracking/VertexContainer.h"

#include "TMath.h"
#define NINT(a) ((a) >= 0.0 ? (int)((a)+0.5) : (int)((a)-0.5))

#define DEBUGMSG MSG::DEBUG
#define VERBOSEMSG MSG::VERBOSE

using namespace std;

/////////////////////////////////////////////////////////////////////////////
FTK_RDO_ReaderAlgo::FTK_RDO_ReaderAlgo(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_ftk_raw_trackcollection_Name("FTK_RDO_Tracks"),
  m_ftk_raw_trackcollection(0x0),
  m_DataProviderSvc("FTK_DataProviderSvc", name),
  m_fillHists(false)
{  
  declareProperty("RDO_CollectionName",m_ftk_raw_trackcollection_Name,
		  "Collection name of RDO");
  declareProperty("FTK_DataProvider", m_DataProviderSvc);
  declareProperty("fillHists", m_fillHists);
 }

FTK_RDO_ReaderAlgo::~FTK_RDO_ReaderAlgo()
{ }

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode FTK_RDO_ReaderAlgo::initialize(){
  MsgStream athlog(msgSvc(), name());
  athlog << MSG::INFO << "FTK_RDO_ReaderAlgo::initialize()" << endreq;
  
  StatusCode scSG = service( "StoreGateSvc", m_StoreGate );
  if (scSG.isFailure()) {
    athlog << MSG::FATAL << "Unable to retrieve StoreGate service" << endreq;
    return scSG;
  }
  athlog << MSG::INFO << "Input RDO collection" << m_ftk_raw_trackcollection_Name << endreq;

  //Set up the FTK Data Provider SVC //
  scSG = m_DataProviderSvc.retrieve();
  //scSG = service("DataProviderSvc", m_DataProviderSvc);

  if (scSG.isFailure()) {
    athlog << MSG::FATAL << "Unable to retrieve Data Provier" << endreq;
    return scSG;
  }


  if (m_fillHists) {
    /// Get Histogram Service ///                                                                                                                                         
    if (service("THistSvc", rootHistSvc).isFailure()){
      ATH_MSG_ERROR("Failed to get ITHistSvc");
      return StatusCode::FAILURE;
    }
    //Book Histograms//
    double PI = TMath::Pi();
    std::vector<TH1D*> histograms;
    
    // FTK Raw Track Histograms //
    h_FTK_RawTrack_n    = new TH1D("h_FTK_RawTrack_n",    ";FTK Raw Track multiplicity;Tracks",      1000,  0.,    1000.);
    h_FTK_RawTrack_phi  = new TH1D("h_FTK_RawTrack_phi",  ";FTK Raw Track #phi [rad];Tracks",         100, -PI,    PI);
    h_FTK_RawTrack_d0   = new TH1D("h_FTK_RawTrack_d0",   ";FTK Raw Track d_{0} [mm];Tracks",         100, -10.,   10.);
    h_FTK_RawTrack_z0   = new TH1D("h_FTK_RawTrack_z0",   ";FTK Raw Track z_{0} [mm];Tracks",         500, -50.,   50.);
    h_FTK_RawTrack_curv = new TH1D("h_FTK_RawTrack_curv", ";FTK Raw Track 1/p_{T} [GeV^{-1}];Tracks", 100,   0.,   0.1);
    h_FTK_RawTrack_cot  = new TH1D("h_FTK_RawTrack_cot",  ";FTK Raw Track cot(#theta); Tracks",       100,  -PI,   PI);
    h_FTK_RawTrack_eta  = new TH1D("h_FTK_RawTrack_eta",  ";FTK Raw Track #eta; Tracks",              100, -3.0,   3.0);
    h_FTK_RawTrack_nPix = new TH1D("h_FTK_RawTrack_nPix", ";FTK Raw Track Pixel Cluster Multiplicity; Events", 10, 0.,   10.);
    h_FTK_RawTrack_nSCT = new TH1D("h_FTK_RawTrack_nSCT", ";FTK Raw Track SCT Cluster Multiplicity; Events",   10, 0.,   10.);
    
    histograms.push_back(h_FTK_RawTrack_n);
    histograms.push_back(h_FTK_RawTrack_phi);
    histograms.push_back(h_FTK_RawTrack_d0);
    histograms.push_back(h_FTK_RawTrack_z0);
    histograms.push_back(h_FTK_RawTrack_curv);
    histograms.push_back(h_FTK_RawTrack_cot);
    histograms.push_back(h_FTK_RawTrack_eta);
    histograms.push_back(h_FTK_RawTrack_nPix);
    histograms.push_back(h_FTK_RawTrack_nSCT);
    
    // Trk::Track (prefit) Histograms //
    h_Track_n    = new TH1D("h_Track_n",    ";Trk::Track multiplicity;Tracks",       1000,  0.,    1000.);
    h_Track_phi  = new TH1D("h_Track_phi",  ";Trk::Track #phi [rad];Tracks",         100, -PI,    PI);
    h_Track_d0   = new TH1D("h_Track_d0",   ";Trk::Track d_{0} [mm];Tracks",         100, -10.,   10.);
    h_Track_z0   = new TH1D("h_Track_z0",   ";Trk::Track z_{0} [mm];Tracks",         500, -50.,   50.);
    h_Track_curv = new TH1D("h_Track_curv", ";Trk::Track 1/p_{T} [GeV^{-1}];Tracks", 100,   0.,   0.1);
    h_Track_cot  = new TH1D("h_Track_cot",  ";Trk::Track cot(#theta); Tracks",       100,  -PI,   PI);
    h_Track_eta  = new TH1D("h_Track_eta",  ";Trk::Track #eta; Tracks",              100, -3.0,   3.0);
    h_Track_nPix = new TH1D("h_Track_nPix", ";Trk::Track Pixel Cluster Multiplicity; Events", 10, 0.,   10.);
    h_Track_nSCT = new TH1D("h_Track_nSCT", ";Trk::Track SCT Cluster Multiplicity; Events",   10, 0.,   10.);
    
    histograms.push_back(h_Track_n);
    histograms.push_back(h_Track_phi);
    histograms.push_back(h_Track_d0);
    histograms.push_back(h_Track_z0);
    histograms.push_back(h_Track_curv);
    histograms.push_back(h_Track_cot);
    histograms.push_back(h_Track_eta);
    histograms.push_back(h_Track_nPix);
    histograms.push_back(h_Track_nSCT);
    
    // Trk::Track (prefit) Histograms ///
    h_refitTrack_n    = new TH1D("h_refitTrack_n",    ";refit Trk::Track multiplicity;Tracks",       1000,  0.,    1000.);
    h_refitTrack_phi  = new TH1D("h_refitTrack_phi",  ";refit refit Trk::Track #phi [rad];Tracks",         100, -PI,    PI);
    h_refitTrack_d0   = new TH1D("h_refitTrack_d0",   ";Trk::Track d_{0} [mm];Tracks",         100, -10.,   10.);
    h_refitTrack_z0   = new TH1D("h_refitTrack_z0",   ";refit Trk::Track z_{0} [mm];Tracks",         500, -50.,   50.);
    h_refitTrack_curv = new TH1D("h_refitTrack_curv", ";refit Trk::Track 1/p_{T} [GeV^{-1}];Tracks", 100,   0.,   0.1);
    h_refitTrack_cot  = new TH1D("h_refitTrack_cot",  ";refit Trk::Track cot(#theta); Tracks",       100,  -PI,   PI);
    h_refitTrack_eta  = new TH1D("h_refitTrack_eta",  ";refit Trk::Track #eta; Tracks",              100, -3.0,   3.0);
    h_refitTrack_nPix = new TH1D("h_refitTrack_nPix", ";refit Trk::Track Pixel Cluster Multiplicity; Events", 10, 0.,   10.);
    h_refitTrack_nSCT = new TH1D("h_refitTrack_nSCT", ";refit Trk::Track SCT Cluster Multiplicity; Events",   10, 0.,   10.);
    
    histograms.push_back(h_refitTrack_n);
    histograms.push_back(h_refitTrack_phi);
    histograms.push_back(h_refitTrack_d0);
    histograms.push_back(h_refitTrack_z0);
    histograms.push_back(h_refitTrack_curv);
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
   athlog << DEBUGMSG << "FTK_RDO_ReaderAlgo::execute() start" << endreq;


   const EventInfo* eventInfo;
   StatusCode scEv = evtStore()->retrieve(eventInfo);
   if (scEv.isFailure()) {
     ATH_MSG_ERROR( "Could not retrieve event info" );
   }
   int eventNumber = eventInfo->event_ID()->event_number();
   athlog << DEBUGMSG << " Event " << eventNumber << endreq;

   //
   // Get RAW tracks
   //

   const FTK_RawTrackContainer *rawTracks ( 0 );
   if ( evtStore()->contains<FTK_RawTrackContainer> ( m_ftk_raw_trackcollection_Name) ) {
     if ( evtStore()->retrieve ( rawTracks, m_ftk_raw_trackcollection_Name  ).isFailure() )
       {	        
	 if (msgLvl(DEBUGMSG)) msg() << "Could not retrieve FTK_RawTrackContainer " <<  m_ftk_raw_trackcollection_Name << " in StoreGate." << endreq;
	 return StatusCode::SUCCESS;
       }
   } else {
     if (msgLvl(DEBUGMSG)) msg() << "Could not find FTK_RawTrackContainer " <<  m_ftk_raw_trackcollection_Name << " in StoreGate." << endreq;
     return StatusCode::SUCCESS;

   }

   athlog << DEBUGMSG << " Got FTK_RawTrackContainer with " << rawTracks->size() << " tracks" << endreq;

   // Fill RAW track histograms

   if (m_fillHists) h_FTK_RawTrack_n->Fill(rawTracks->size());

   
   FTK_RawTrackContainer::const_iterator pTrack = rawTracks->begin();
   FTK_RawTrackContainer::const_iterator pLastTrack = rawTracks->end();

   for ( int itr=0; pTrack!= pLastTrack; pTrack++, itr++) {

     float trkTheta = std::atan2(1.0,(*pTrack)->getCotTh());
     float trkEta = -std::log(std::tan(trkTheta/2.));
     float trkPt=1.e10;
     if ((*pTrack)->getCurv() !=0) trkPt=2./(*pTrack)->getCurv();
       

     athlog << VERBOSEMSG << itr << ": pT: " << trkPt  << " eta: " << trkEta << " phi: " <<  (*pTrack)->getPhi() << " d0: " << (*pTrack)->getD0() <<
       " z0: " << (*pTrack)->getZ0()<< " cot " << (*pTrack)->getCotTh() << " theta " << trkTheta << " curv " << (*pTrack)->getCurv() <<
       " nPix: " << (*pTrack)->getPixelClusters().size() << " nSCT: "<< (*pTrack)->getSCTClusters().size() << endreq;
     
     if (m_fillHists) {
       h_FTK_RawTrack_phi->Fill((*pTrack)->getPhi());
       h_FTK_RawTrack_d0->Fill((*pTrack)->getD0());
       h_FTK_RawTrack_z0->Fill((*pTrack)->getZ0());
       h_FTK_RawTrack_curv->Fill((*pTrack)->getCurv());
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
       std::cout << " - ipt     = " << (*pTrack)->getCurv()   << std::endl;
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


   //
   // Get Converted Tracks
   //

   athlog << DEBUGMSG << "Getting converted tracks from DataProviderSvc" << endreq;
   TrackCollection *track_collection = m_DataProviderSvc->getTracks(false);
   athlog << DEBUGMSG << "DataProviderSvc returned " <<  track_collection->size() << " converted tracks" << endreq;

   if (m_fillHists) h_Track_n->Fill(track_collection->size());
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
       athlog << VERBOSEMSG << " filling histograms " << endreq;
       h_Track_phi->Fill((*track_it)->perigeeParameters()->parameters()[Trk::phi0]);
       h_Track_d0->Fill((*track_it)->perigeeParameters()->parameters()[Trk::d0]);
       h_Track_z0->Fill((*track_it)->perigeeParameters()->parameters()[Trk::z0]);
       h_Track_curv->Fill((*track_it)->perigeeParameters()->parameters()[Trk::qOverP]);
       h_Track_cot->Fill(1./std::tan((*track_it)->perigeeParameters()->parameters()[Trk::theta]));
       h_Track_nPix->Fill(nPix);
       h_Track_nSCT->Fill(nSCT);
     }
     double trkPt = 1.e10;
     if ((*track_it)->perigeeParameters()->parameters()[Trk::qOverP]!=0) trkPt= sin((*track_it)->perigeeParameters()->parameters()[Trk::theta])/(*track_it)->perigeeParameters()->parameters()[Trk::qOverP];
     athlog << VERBOSEMSG << iTrack << ": pT: "<< trkPt <<
       " eta: " <<  -log(std::tan((*track_it)->perigeeParameters()->parameters()[Trk::theta]/2.)) <<
       " phi: " <<  (*track_it)->perigeeParameters()->parameters()[Trk::phi0] <<
       " d0: " << (*track_it)->perigeeParameters()->parameters()[Trk::d0] <<
       " z0: " << (*track_it)->perigeeParameters()->parameters()[Trk::z0] <<
       " nPix: " << nPix << " nSCT: " << nSCT << endreq;
     
   }

   //
   // Get TrackParticles from Converted Tracks
   // 
   

   athlog << DEBUGMSG << " Getting TrackParticles from converted tracks from DataProviderSvc" << endreq;
   xAOD::TrackParticleContainer *tpc  = m_DataProviderSvc->getTrackParticles(false);
   athlog << DEBUGMSG << "DataProviderSvc returned " <<  tpc->size() << " TrackParticles created from converted tracks" << endreq;

   int itpc = 0;
   for ( auto ptp =  tpc->begin(); ptp !=  tpc->end(); ptp++, itpc++) {
     //uint8_t NumOfPixHits = 0;
     //uint8_t NumOfSCTHits = 0;
     //if (!(*ptp)->summaryValue(NumOfSCTHits, xAOD::numberOfSCTHits) ) athlog << DEBUGMSG << "Could not retrieve number of SCT hits"<< endreq;
     //if (!(*ptp)->summaryValue(NumOfPixHits, xAOD::numberOfPixelHits) ) athlog << DEBUGMSG << "Could not retrieve number of Pixel hits"<< endreq;
     
     double p = 1e10;
     if ((*ptp)->qOverP() !=0) p = 1./(*ptp)->qOverP();
     athlog << VERBOSEMSG << itpc << ": q*pT " << (*ptp)->pt()*(*ptp)->charge() << " eta " << (*ptp)->eta() << " phi " << (*ptp)->phi0() <<
       " d0: " << (*ptp)->d0() << " z0: " << (*ptp)->z0() << " p " << p << endreq;
   }

   //
   // Get VxVertex from converted tracks
   //

   athlog << DEBUGMSG << " Getting VxContainer from Converted tracks from DataProviderSvc" << endreq;
   VxContainer* vxc = m_DataProviderSvc->getVxContainer(false);
   athlog << DEBUGMSG << "DataProviderSvc returned " <<  vxc->size() << " VxCandidates created from converted tracks" << endreq;

   int ivx=0;
   for (auto pvx = vxc->begin(); pvx != vxc->end(); pvx++, ivx++) {
     athlog << VERBOSEMSG << ivx << ": (x,y,z) = ( " << 
       (*pvx)->recVertex().position().x() << ", " <<  
       (*pvx)->recVertex().position().y() << ", " <<
       (*pvx)->recVertex().position().z() <<
       " ) Chi2: " << (*pvx)->recVertex().fitQuality().chiSquared() << 
       " nDoF " << (*pvx)->recVertex().fitQuality().numberDoF() << endreq;
   }


   //
   // Get xAoD::Vertex from converted tracks
   //

   athlog << DEBUGMSG << " Getting xAOD::VertexContainer from Converted tracks from DataProviderSvc" << endreq;
   xAOD::VertexContainer* vertexc = m_DataProviderSvc->getVertexContainer(false);
   athlog << DEBUGMSG << "DataProviderSvc returned " <<  vertexc->size() << " zAOD:Vertex created from converted tracks" << endreq;

   int ivertexc = 0;
   for (auto pv = vertexc->begin(); pv != vertexc->end(); pv++, ivertexc++) {
     athlog << VERBOSEMSG << ivertexc << ": (x,y,z)= ( " << (*pv)->position().x() << ", " <<  (*pv)->position().y() << ", " << (*pv)->position().z() <<
       " ) chi2: " << (*pv)->chiSquared() << " Ndof: "<< (*pv)->numberDoF()<< endreq;
   }

   // 
   // Get refitted Tracks
   //


   athlog << DEBUGMSG << " Getting refitted tracks from DataProviderSvc" << endreq;
   TrackCollection *refitTracks  = m_DataProviderSvc->getTracks(true);
   athlog << DEBUGMSG << "DataProviderSvc returned " <<  refitTracks->size() << " refitted tracks" << endreq;

   if (m_fillHists) h_Track_n->Fill(refitTracks->size());
   track_it   = refitTracks->begin();
   last_track = refitTracks->end();
   for (iTrack=0 ; track_it!= last_track; track_it++, iTrack++) {
     int nPix = 0;
     int nSCT = 0;
     const Trk::TrackSummary* summary = (*track_it)->trackSummary();
     if( summary != 0){
       nPix = summary->get(Trk::numberOfPixelHits);
       nSCT = summary->get(Trk::numberOfSCTHits);
     }
     if (m_fillHists) {
       athlog << VERBOSEMSG << " filling histograms " << endreq;
       h_Track_phi->Fill((*track_it)->perigeeParameters()->parameters()[Trk::phi0]);
       h_Track_d0->Fill((*track_it)->perigeeParameters()->parameters()[Trk::d0]);
       h_Track_z0->Fill((*track_it)->perigeeParameters()->parameters()[Trk::z0]);
       h_Track_curv->Fill((*track_it)->perigeeParameters()->parameters()[Trk::qOverP]);
       h_Track_cot->Fill(1./std::tan((*track_it)->perigeeParameters()->parameters()[Trk::theta]));
       h_Track_nPix->Fill(nPix);
       h_Track_nSCT->Fill(nSCT);
     }
     double trkPt = 1.e10;
     if ((*track_it)->perigeeParameters()->parameters()[Trk::qOverP]!=0) trkPt= sin((*track_it)->perigeeParameters()->parameters()[Trk::theta])/(*track_it)->perigeeParameters()->parameters()[Trk::qOverP];
       
     athlog << VERBOSEMSG << iTrack << ": pT: "<< trkPt <<
       " eta: " <<  -std::log(std::tan((*track_it)->perigeeParameters()->parameters()[Trk::theta]/2.)) <<
       " phi: " <<  (*track_it)->perigeeParameters()->parameters()[Trk::phi0] <<
       " d0: " << (*track_it)->perigeeParameters()->parameters()[Trk::d0] <<
       " z0: " << (*track_it)->perigeeParameters()->parameters()[Trk::z0] <<
       " nPix: " << nPix << " nSCT: " << nSCT << endreq;
   }

   
   //
   // Get TrackParticles from refitted tracks
   //

   athlog << DEBUGMSG << " Getting TrackParticles from refitted tracks from DataProviderSvc" << endreq;
   xAOD::TrackParticleContainer *tpr  = m_DataProviderSvc->getTrackParticles(true);
   athlog << DEBUGMSG << "DataProviderSvc returned " <<  tpr->size() << " TrackParticles created from refitted tracks" << endreq;

   int itpr=0;
   for ( auto ptpr =  tpr->begin(); ptpr !=  tpr->end(); ptpr++, itpr++) {
     double p = 1.e10;
     if ((*ptpr)->qOverP()!=0.) p = 1./(*ptpr)->qOverP();
     //uint8_t NumOfPixHits = 0;
     //uint8_t NumOfSCTHits = 0;
     //if (!(*ptpr)->summaryValue(NumOfSCTHits, xAOD::numberOfSCTHits) ) athlog << MSG::DEBUG << "Could not retrieve number of SCT hits"<< endreq;
     //if (!(*ptpr)->summaryValue(NumOfPixHits, xAOD::numberOfPixelHits) ) athlog << MSG::DEBUG << "Could not retrieve number of Pixel hits"<< endreq;
     athlog << VERBOSEMSG << itpr << ": q*pT " << (*ptpr)->pt()*(*ptpr)->charge() << " eta " << (*ptpr)->eta() << " phi " << (*ptpr)->phi0() << 
       " d0: " << (*ptpr)->d0() << " z0: " << " q*p: " << p << (*ptpr)->z0() << endreq;
	 
   }



   //
   // Get VxVertex from refitted tracks
   //

   athlog << DEBUGMSG << " Getting VxContainer from refitted tracks from DataProviderSvc" << endreq;
   VxContainer* vxr = m_DataProviderSvc->getVxContainer(true);
   athlog << DEBUGMSG << "DataProviderSvc returned " <<  vxr->size() << " VxCandidates created from refitted tracks" << endreq;
   

   int ivxr = 0;
   for (auto pvx = vxr->begin(); pvx != vxr->end(); pvx++, ivxr++) {
     athlog << VERBOSEMSG << ivxr << ": (x,y,z)= ( " << (*pvx)->recVertex().position().x() << ", " <<  (*pvx)->recVertex().position().y() << ", " << (*pvx)->recVertex().position().z()
	    << " ) Chi2: " << (*pvx)->recVertex().fitQuality().chiSquared() << " nDoF " << (*pvx)->recVertex().fitQuality().numberDoF() << endreq;
     
   }


   //
   // Get xAoD::Vertex from converted tracks
   //

   athlog << DEBUGMSG << " Getting xAOD::VertexContainer from Converted tracks from DataProviderSvc" << endreq;
   xAOD::VertexContainer* vertexr = m_DataProviderSvc->getVertexContainer(true);
   athlog << DEBUGMSG << "DataProviderSvc returned " <<  vertexr->size() << " zAOD:Vertex created from refitted tracks" << endreq;
   
   int ivertexr = 0;
   for (auto pv = vertexr->begin(); pv != vertexr->end(); pv++, ivertexr++) {
     athlog << VERBOSEMSG << ivertexr << ": (x,y,z)= ( " << (*pv)->position().x() << ", " <<  (*pv)->position().y() << ", " << (*pv)->position().z()<< 
       " ) chi2: " << (*pv)->chiSquared() << " Ndof: "<< (*pv)->numberDoF()<< endreq;
   }
   
   
   delete(track_collection);
   delete(refitTracks);   
   delete(tpc);
   delete(vxc);
   delete(vertexc);
   delete(tpr);
   delete(vxr);
   delete(vertexr);


   // Reset the ftk cache for this event //
   //scFTK = m_DataProviderSvc->endEvent();
   //if( scFTK.isFailure()){
   //  athlog << VERBOSEMSG << "Cache Clear " << endreq;
   //}
   athlog << DEBUGMSG << "FTK_RDO_ReaderAlgo::execute() end" << endreq;

   return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
StatusCode FTK_RDO_ReaderAlgo::finalize() {
    MsgStream athlog(msgSvc(), name());
    athlog << MSG::INFO << "finalize()" << endreq;

    return StatusCode::SUCCESS;
}
