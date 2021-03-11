/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//============================================================
//
// T2VertexBeamSpot.cxx, (c) ATLAS Detector software
// Trigger/TrigAlgorithms/TrigT2BeamSpot/T2VertexBeamSpot
//
// Online beam spot measurement and monitoring using
// L2 recontructed primary vertices.
//
// Authors : David W. Miller, Rainer Bartoldus,   
//           Su Dong
//============================================================
#include "T2VertexBeamSpot.h"
// General ATHENA/Trigger stuff
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include <typeinfo>
#include <cmath>

using std::string;

using namespace PESA;
using Gaudi::Units::mm;
using Gaudi::Units::GeV;
using TrackData = T2BSTrackFilterTool::TrackData;

namespace {

//Loads all TEs into one vector

} // namespace

// Constructor
T2VertexBeamSpot::T2VertexBeamSpot( const std::string& name, ISvcLocator* pSvcLocator )
  : AthReentrantAlgorithm(name, pSvcLocator){
   
   declareProperty( "TrackCollection",  m_trackCollectionKey = "TrigFastTrackFinder_Tracks"  );
   declareProperty( "VertexCollection",  m_outputVertexCollectionKey =  "myVertices"  );

   declareProperty("filterBS",            m_filterBS         = true);
   declareProperty("doTrackBeamSpot",     m_doTrackBeamSpot  = true);
}

StatusCode T2VertexBeamSpot::initialize() {

   ATH_CHECK(m_trackFilterTool.retrieve());
   ATH_CHECK(m_beamSpotTool.retrieve());
   ATH_CHECK(m_trackBSTool.retrieve());
   ATH_CHECK( m_monTool.retrieve());

   ATH_CHECK( m_trackCollectionKey.initialize() );
   ATH_CHECK( m_outputVertexCollectionKey.initialize() );

   return StatusCode::SUCCESS;
}


StatusCode T2VertexBeamSpot::execute(const EventContext& ctx) const{
   // Start the overall timer
   auto tTotal = Monitored::Timer("TIME_TotalTime");

   auto& eventID = ctx.eventID();

   //// Initialize vertex collections
   SG::WriteHandle<TrigVertexCollection> myVertexCollection(m_outputVertexCollectionKey, ctx);
   myVertexCollection = std::make_unique<TrigVertexCollection>();

   //Need to convert to the write handles
   DataVector< TrigVertexCollection > mySplitVertexCollections;

   ConstDataVector<TrackCollection> mySelectedTrackCollection;
   mySelectedTrackCollection.clear( SG::VIEW_ELEMENTS );


   //Counters to keep track of number of all/selected/highPt tracks
   unsigned nAllTracks = 0;
   unsigned nSelectedTracks = 0;
   unsigned nHighPtTracks = 0;

   {
      //Monitor how long does it take to loop over all collections
      auto tSelectingTracks = Monitored::Timer("TIME_SelectingTracks");

      //Monitor total all/passed/highPt track numbers
      auto nTotalTracks = Monitored::Scalar<unsigned>("nTotalTracks");
      auto nTotalPassedTracks = Monitored::Scalar<unsigned>("nTotalPassedTracks");
      auto nTotalHighPTTracks = Monitored::Scalar<unsigned>("nTotalHighPTTracks");

      //Loop over track collections and select tracks
      SG::ReadHandle<TrackCollection> trackCollection (m_trackCollectionKey, ctx);
      ATH_CHECK(trackCollection.isValid());

      //Dereference tracks
      const TrackCollection* tracks = trackCollection.cptr();

      //Select tracks
      auto selectedTracks = m_trackFilterTool->filter(*tracks);

      std::vector<TrackData> bsTracks;
      bool has_bs = m_trackFilterTool->updateBS(selectedTracks, eventID.lumi_block(),
                                                eventID.bunch_crossing_id(),
                                                &bsTracks);

      std::vector<const Trk::Track*> selectedTracksBS;
      if ((m_doTrackBeamSpot or m_filterBS) and has_bs) {
         selectedTracksBS = m_trackFilterTool->filterBS(selectedTracks);
      }

      // collect tracks for vertex-based tool into a single colelction
      if (m_filterBS and has_bs) {
         mySelectedTrackCollection.insert(mySelectedTrackCollection.end(),
                                          selectedTracksBS.begin(), selectedTracksBS.end());
      } else {
         mySelectedTrackCollection.insert(mySelectedTrackCollection.end(),
                                          selectedTracks.begin(), selectedTracks.end());
      }

      // call track-based tool
      if (m_doTrackBeamSpot and has_bs) {
         if (not bsTracks.empty()) {
            m_trackBSTool->updateBS(bsTracks);
         } else {
            m_trackBSTool->updateBS(selectedTracksBS, eventID.bunch_crossing_id());
         }
      }

      //FIXME: make a counter class
      nTotalTracks       += tracks->size();
      nTotalPassedTracks += selectedTracks.size();
      nTotalHighPTTracks += 0;

      //Store counts for all/highPt/selected tracks
      nAllTracks = nTotalTracks;
      nSelectedTracks = nTotalPassedTracks;
      nTotalHighPTTracks = m_beamSpotTool->numHighPTTrack(mySelectedTrackCollection);
      nHighPtTracks = nTotalHighPTTracks;

      //Monitor total number of tracks
      auto monitor = Monitored::Group(m_monTool, tSelectingTracks, nTotalTracks, nTotalPassedTracks, nTotalHighPTTracks );
      ATH_MSG_DEBUG( "Number of all Tracks: "<< nAllTracks <<" Selected Tracks: " << nSelectedTracks << " highPt Tracks: " << nHighPtTracks );
   }


   // Check for seed tracks (= high-pT tracks)
   if ( nHighPtTracks == 0  ) {
      ATH_MSG_DEBUG( " No seed tracks for vertex");
      return StatusCode::SUCCESS;
   }

   // Check for the total number of available tracks
   if (  ( m_beamSpotTool->notEnoughTracks(nSelectedTracks)) ) {
      ATH_MSG_DEBUG( "Not enough total passed tracks to vertex");
      return StatusCode::SUCCESS;
   }

   ATH_MSG_DEBUG( "Reconstruct vertices" );
   //Reconstruct vertices if passed track selection
   {
      //Monitor how long does it take to loop over all collections
      auto tReconstructVertices = Monitored::Timer("TIME_ReconstructVertices");
      m_beamSpotTool->reconstructVertices( mySelectedTrackCollection, *myVertexCollection, mySplitVertexCollections, ctx );
      //Monitor total number of tracks
      auto monitor = Monitored::Group(m_monTool, tReconstructVertices);
   }

   auto monitor = Monitored::Group(m_monTool, tTotal);

   ATH_MSG_DEBUG( "Number of track collection containers: " << mySelectedTrackCollection.size() );

   //What should go as an output? SelectedTrackCollection and Vertices?
   //Atm just try add vertex
   //TODO: adding split vertices as well, will need an array

   return StatusCode::SUCCESS;
} 
