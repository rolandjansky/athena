/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
//============================================================
// T2VertexBeamSpot.cxx, (c) ATLAS Detector software
// Trigger/TrigAlgorithms/TrigT2BeamSpot/T2VertexBeamSpot
//
// Online beam spot measurement and monitoring using
// L2 recontructed primary vertices.
//
// Authors : David W. Miller, Rainer Bartoldus,   
//           Su Dong
//
//============================================================
// This algorithm
#include "TrigT2BeamSpot/T2VertexBeamSpotTool.h"
#include "T2TrackClusterer.h"
#include "T2Timer.h"
// Specific to this algorithm
#include "TrigInDetEvent/TrigVertex.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "TrigInDetToolInterfaces/ITrigPrimaryVertexFitter.h"
#include "TrkParameters/TrackParameters.h"
// Generic Trigger tools
#include "TrigNavigation/TriggerElement.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigTimeAlgs/TrigTimer.h"
//Conversion units
#include "GaudiKernel/SystemOfUnits.h"
using Gaudi::Units::GeV;
using Gaudi::Units::mm;

#include <string>
#include <sstream>
#include <cmath>

using std::string;
using std::ostringstream;
using std::vector;
using std::abs;

using namespace PESA;


namespace Beamspot
{ 
  class TrackPTSort{
  public:
    inline bool operator () (const Trk::Track* trk1, const Trk::Track* trk2) 
    { 
      const Trk::TrackParameters* params1 = trk1->perigeeParameters();
      float pT1 = std::abs(sin(params1->parameters()[Trk::theta])/params1->parameters()[Trk::qOverP]);
      const Trk::TrackParameters* params2 = trk2->perigeeParameters();
      float pT2 = std::abs(sin(params2->parameters()[Trk::theta])/params2->parameters()[Trk::qOverP]);
      return pT1 > pT2;
    }
  };

  class TrackPhiSort{
  public:
    inline bool operator () (const Trk::Track* trk1, const Trk::Track* trk2) 
    { 
      float phi1 = trk1->perigeeParameters()->parameters()[Trk::phi];
      float phi2 = trk2->perigeeParameters()->parameters()[Trk::phi];
      return std::abs(phi1) > std::abs(phi2);
    }
  };
}

PESA::T2VertexBeamSpotTool::T2VertexBeamSpotTool( const std::string& type, const std::string& name, const IInterface* parent )
   : AthAlgTool( type, name, parent),
   m_primaryVertexFitterTool("TrigInDetToolInterfaces/ITrigPrimaryVertexFitter", this){

   //Declare properties in here
   declareProperty( "BeamSpotData",  m_beamSpotKey   );
   declareProperty( "PrimaryVertexFitter",  m_primaryVertexFitterTool);

   //Declare variables:
   declareProperty("TotalNTrackMin",  m_totalNTrkMin = 2 ); 

   // Track clustering parameters 
   declareProperty("TrackSeedPt",       m_trackSeedPt       =  1.0*GeV ); 
   declareProperty("TrackClusterDZ",    m_trackClusDZ       = 10.0*mm  ); 
   declareProperty("WeightClusterZ",    m_weightSeed        = true     ); 
   declareProperty("ReclusterSplit",    m_reclusterSplit    = true     ); 
   declareProperty("SplitWholeCluster", m_splitWholeCluster = false    ); 

   // Track Selection criteria 
   declareProperty("TrackMinPt",      m_minTrackPt       =  0.7*GeV ); 
   declareProperty("TrackMaxEta",     m_maxTrackEta      =   2.5    ); 
   declareProperty("TrackMaxZ0",      m_maxTrackZ0       = 200.*mm  ); 
   declareProperty("TrackMaxD0",      m_maxTrackD0       =  10.*mm  ); 
   declareProperty("TrackMaxZ0err",   m_maxTrackZ0err    =   2.*mm  ); 
   declareProperty("TrackMaxD0err",   m_maxTrackD0err    =   2.*mm  ); 
   declareProperty("TrackMinNDF",     m_minTrackNDF      =   2      ); 
   declareProperty("TrackMinQual",    m_minTrackQual     =   0.2    ); 
   declareProperty("TrackMaxQual",    m_maxTrackQual     =   5.0    ); 
   declareProperty("TrackMinChi2Prob",m_minTrackChi2Prob = -10.0    ); 
   declareProperty("TrackMinSiHits",  m_minSiHits        =   4      ); 
   declareProperty("TrackMinPIXHits", m_minPIXHits       =   1      ); 
   declareProperty("TrackMinSCTHits", m_minSCTHits       =   3      ); 
   declareProperty("TrackMinTRTHits", m_minTRTHits       =   0      ); 

   // Vertex Selection criteria 
   declareProperty("VertexMinNTrk",     m_vtxNTrkMin     =   2      ); 
   declareProperty("VertexMaxNTrk",     m_vtxNTrkMax     = 100      ); 
   declareProperty("VertexMinQual",     m_vtxQualMin     =   0.0    ); 
   declareProperty("VertexMaxQual",     m_vtxQualMax     =   5.     ); 
   declareProperty("VertexMinChi2Prob", m_vtxChi2ProbMin = -10.0    ); 
   declareProperty("VertexMinMass",     m_vtxMassMin     =  -1.*GeV ); 
   declareProperty("VertexMinSumPt",    m_vtxSumPtMin    =   0.*GeV ); 
   declareProperty("VertexMaxX",        m_vtxXposMax     =  10.*mm  ); 
   declareProperty("VertexMaxXerr",     m_vtxXerrMax     =   1.*mm  ); 
   declareProperty("VertexMaxY",        m_vtxYposMax     =  10.*mm  ); 
   declareProperty("VertexMaxYerr",     m_vtxYerrMax     =   1.*mm  ); 
   declareProperty("VertexMaxZ",        m_vtxZposMax     = 200.*mm  ); 
   declareProperty("VertexMaxZerr",     m_vtxZerrMax     =   1.*mm  ); 

   // Vertex selection for use in BCID measurements 
   declareProperty("VertexBCIDMinNTrk", m_vtxBCIDNTrkMin =   2      ); 

   declareProperty("nSplitVertices",    m_nSplitVertices      = 1);

   // Single interaction trigger
   declareProperty("minNpvTrigger",  m_minNpvTrigger  = 0);
   declareProperty("maxNpvTrigger",  m_maxNpvTrigger  = 2);


 
}




StatusCode T2VertexBeamSpotTool::initialize(){
   ATH_MSG_INFO( "Initialising BeamSpot tool" );

   //Initialize data Handles
   ATH_CHECK( m_beamSpotKey.initialize() ) ;

   //Retrieve monitoring tool
   ATH_CHECK( m_monTool.retrieve());

   //Retrieve primary vertex fitter tool
   ATH_CHECK( m_primaryVertexFitterTool.retrieve() );

   return StatusCode::SUCCESS;
}


/********************************************//**
* \fn Select tracks
* \brief Loop over provided tracks and selects them based on their parameters
*  Detailed description of the select track function
 ***********************************************/
void T2VertexBeamSpotTool::selectTracks( const TrackCollection* trackCollection,
      ConstDataVector<TrackCollection>& mySelectedTrackCollection, std::vector<unsigned> &trackCounter ) const {

   ATH_MSG_DEBUG( "Selecting tracks for the beamSpot algorithm" );

   //Monitoring counters and timers
   auto timerTrackSelection = Monitored::Timer("TIME_TrackSelection");
   //Switch probably naming from PerROI to PerCollection 
   auto nTracksPerROI           = Monitored::Scalar<unsigned>("TracksPerROI", 0);
   auto nTracksPassedPerROI     = Monitored::Scalar<unsigned>("SelectedTracksPerROI", 0);
   auto nHiPTTracksPassedPerROI = Monitored::Scalar<unsigned>("SelectedHiPTTracksPerROI", 0);

   //T2Track with easily accesable parameters
   std::vector<T2Track> myTracks; myTracks.reserve(trackCollection->size() );

   // Loop over all tracks in the given track collections
   for ( TrackCollection::const_iterator trackIter = trackCollection->begin();
         trackIter != trackCollection->end(); ++trackIter ) { 

      const Trk::Track& track = **trackIter;

      // Make sure that the event has tracks
      //Counter for all input tracks
      nTracksPerROI++;

      T2Track myTrack( track );
      // Check for passed track
      if ( isGoodTrack( myTrack ) ) {
         // Add this track to the set used to find a vertex
         mySelectedTrackCollection.push_back( *trackIter );

         //Stored only for monitoring
         myTracks.push_back( myTrack );

         //Counter for selected tracks
         nTracksPassedPerROI++;

         // Check for high-pT track
         if ( myTrack.Pt() > m_trackSeedPt ){
            //Counter for high pT tracks
            nHiPTTracksPassedPerROI++;
         }
         else ATH_MSG_DEBUG( "Track.pt: " << myTrack.Pt()*GeV );
      }
      else {
         ATH_MSG_DEBUG( "Track.failed selection: d0: " << myTrack.D0() <<
               " z0: " << myTrack.Z0() <<
               " phi0: " << myTrack.Phi() <<
               " eta: " << myTrack.Eta() <<
               " pT: " << myTrack.Pt()*GeV <<
               " chi2: " << myTrack.Qual() <<
               " NpixSPs: " << myTrack.PIXHits() <<
               " NsctSPs: " << myTrack.SCTHits() <<
               " NstrawHits: " << myTrack.TRTHits() );
      }
   } //end for loop over tracks in a collection

      //Save track counts
      trackCounter[0] = nTracksPerROI;
      trackCounter[1] = nTracksPassedPerROI;
      trackCounter[2] = nHiPTTracksPassedPerROI;
   
      //How many tracks per collection
      ATH_MSG_DEBUG( "Total Tracks: " << nTracksPerROI << " selectedTracks: " << nTracksPassedPerROI << " High PT tracks: " << nHiPTTracksPassedPerROI );

      //Monitor all passed tracks variables/parameters
      monitor_tracks( "Track", "Pass", myTracks);

      //Monitor counters per track collection and time to select tracks
      auto mon = Monitored::Group(m_monTool, nTracksPerROI, nTracksPassedPerROI, nHiPTTracksPassedPerROI,
                 timerTrackSelection ); //timers
}


unsigned int T2VertexBeamSpotTool::reconstructVertices( ConstDataVector<TrackCollection>& mySelectedTrackCollection,
                                           TrigVertexCollection& myVertexCollection,
                                           DataVector< TrigVertexCollection >&  mySplitVertexCollections, const EventContext& ctx) const {
    ATH_MSG_DEBUG( "Reconstructing vertices" );

   //Monitoring counters and timers
   auto timerVertexRec = Monitored::Timer("TIME_VertexReconstruction");
   auto nClusters      = Monitored::Scalar<unsigned>("NClusters", 0);
   auto nVtx           = Monitored::Scalar<unsigned>("Nvtx", 0);
   auto nPassVtx       = Monitored::Scalar<unsigned>("NvtxPass", 0);
   auto nPassBCIDVtx   = Monitored::Scalar<unsigned>("NvtxPassBCID", 0);

  // Make collections for vertex splitting (unsorted)
  ConstDataVector<TrackCollection> mySplitTrackCollection( mySelectedTrackCollection );

  // Sort tracks by track pT
  {
     auto timeToSortTracks = Monitored::Timer("TIME_toSortTracks");
     mySelectedTrackCollection.sort( Beamspot::TrackPTSort() );
     auto mon = Monitored::Group(m_monTool,  timeToSortTracks ); 
  }

  // Prepare a track clustering algorithm with the given parameters
  // Should we leave this as a standalone class or merge with the tool?
   T2TrackClusterer trackClusterer( m_trackClusDZ, m_trackSeedPt, m_weightSeed, m_vtxNTrkMax );

  // Create clusters from the track collection until all its tracks are used
   while ( ! mySelectedTrackCollection.empty() ) {

      const Trk::TrackParameters* params = (*mySelectedTrackCollection.begin())->perigeeParameters();
      float pT = std::abs(sin(params->parameters()[Trk::theta])/params->parameters()[Trk::qOverP]);
      ATH_MSG_DEBUG( "Number of tracks remaining = " << mySelectedTrackCollection.size() );
      ATH_MSG_DEBUG( "pT of first (seed) track (GeV) = " << pT/GeV );


      // Cluster tracks in z around the first (highest-pT track) in the collection, which is taken
      // as the seed.
      {
         auto timeToZCluster = Monitored::Timer("TIME_toZCluster");
         trackClusterer.cluster( *mySelectedTrackCollection.asDataVector() );
         auto mon = Monitored::Group(m_monTool,  timeToZCluster ); 
      }

      // Sanity check:
      if ( trackClusterer.cluster().size() + trackClusterer.unusedTracks().size()
            != mySelectedTrackCollection.size() ) {
         ATH_MSG_DEBUG( "Track clustering check sum failed: "
               << "cluster().size()=" << trackClusterer.cluster().size()
               << " + unusedTracks().size()=" << trackClusterer.unusedTracks().size()
               << " != mySelectedTrackCollection.size()=" << mySelectedTrackCollection.size()
               );
      }

      // Continue with the remaining tracks - still pT-sorted
      mySelectedTrackCollection.clear( SG::VIEW_ELEMENTS );
      mySelectedTrackCollection.assign (trackClusterer.unusedTracks().begin(),
            trackClusterer.unusedTracks().end());

      // This always uses at least one track (the seed), so we are sure to reduce the track
      // selection and terminate the loop

      // Make sure we have enough tracks in the cluster
      if ( trackClusterer.cluster().size() < m_totalNTrkMin ) {
         ATH_MSG_DEBUG( "Not enough tracks in cluster!" );
         continue;
      }

      // Event has a good cluster
      nClusters++;

      // Monitor properties of of the cluster
      monitor_cluster( trackClusterer );

      // Monitor properties of tracks inside the cluster
      for ( TrackCollection::const_iterator track = trackClusterer.cluster().begin();
            track !=  trackClusterer.cluster().end(); ++track ) {
         monitor_cluster_tracks( trackClusterer,  **track);
      }

      ATH_MSG_DEBUG( "Number of tracks remaining after cluster #(" << nClusters << ") = " << mySelectedTrackCollection.size());
      ATH_MSG_DEBUG( "Total number of tracks to fit    = " << trackClusterer.cluster().size() );
      ATH_MSG_DEBUG( "Average Z position (from trk Z0) = " << trackClusterer.seedZ0()         );
      ATH_MSG_DEBUG( "Fitting tracks");
      ATH_MSG_DEBUG( "Number of tracks remaining = " << mySelectedTrackCollection.size() );

      // Fit a primary vertex to this cluster around its seed track
      TrackCollection vertexTracks;
      TrigVertex* primaryVertex = 0;
      primaryVertex = (m_primaryVertexFitterTool->fit( &trackClusterer.cluster(), vertexTracks, trackClusterer.seedZ0() ) );
      

      // Check to see if the fit succeeded / converged
      if ( ! primaryVertex ) { 
         ATH_MSG_DEBUG( "Vertex fit failed");
         continue; 
      }

      // Update vertex counter
      nVtx++;

      // Extract beam spot parameters
      SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, ctx };

      T2BeamSpot beamSpot(*beamSpotHandle);

      ATH_MSG_DEBUG( "Beamspot from BeamCondSvc: " << beamSpot);

      const T2Vertex myVertex( *primaryVertex, vertexTracks, beamSpot, trackClusterer.seedZ0() );

      // Monitor all vertices parameters
      monitor_vertex( "Vertex", "", myVertex ); 
      ATH_MSG_DEBUG( "Vertex fit: " << '\n' << myVertex);

      // Query for vertex selection
      const bool passVertex = isGoodVertex( myVertex );

      if ( ! passVertex ) { 
         ATH_MSG_DEBUG( "Vertex failed selection" );
         continue; 
      }

      // Add primary vertex to collection
      myVertexCollection.push_back( primaryVertex ); // passes ownership to vertex collection

      //Monitor parameters of the  passed vertex 
      monitor_vertex( "Vertex", "Pass", myVertex ); 

      //Update good vertex counter
      nPassVtx++;    

      // Learn something more about the cluster that ended up in this vertex
      auto deltaVtxZ  = Monitored::Scalar<double>   ( "ClusterDeltaVertexZ",  trackClusterer.seedZ0() - myVertex.Z() );
      auto mon = Monitored::Group(m_monTool,  deltaVtxZ ); 

      // If the vertex is good, splits are requested, and we have enough tracks to split, split them!
      if ( passVertex && m_nSplitVertices > 1 ) {

         if ( m_splitWholeCluster )
         {
            ATH_MSG_DEBUG( "Splitting the entire cluster of tracks into two");
            // Alternative 1: Split the entire cluster of track into two
            mySelectedTrackCollection.clear( SG::VIEW_ELEMENTS );
            mySplitTrackCollection.assign (trackClusterer.cluster().begin(),
                  trackClusterer.cluster().end());
         }
         else
         {
            ATH_MSG_DEBUG( "Splitting only tracks succesfully fitted to a vertex");
            // Alternative 2: Split only the tracks that were successfully fit to a vertex
            mySelectedTrackCollection.clear( SG::VIEW_ELEMENTS );
            mySplitTrackCollection.assign (vertexTracks.begin(), vertexTracks.end());
         }

         if ( mySplitTrackCollection.size() >= m_nSplitVertices * m_totalNTrkMin )
         {
            // Split, optinally re-cluster, and fit separate vertices
            ATH_MSG_DEBUG( "Reconstruct split vertices");
            reconstructSplitVertices( mySplitTrackCollection, mySplitVertexCollections, trackClusterer, ctx );
         }
         // Alternative 3: Split all the tracks and iterate with the remaining tracks
         //          mySplitTrackCollection = mySelectedTrackCollection;
      }

      ATH_MSG_DEBUG( "Number of tracks remaining = " << mySelectedTrackCollection.size() );

      // Now check if this vertex passed the higher multiplicity cut to be used for per-BCID measurements
      const bool passVertexBCID = isGoodVertexBCID( myVertex );

      if ( passVertexBCID ) {
         // Fill accepted per-BCID vertex histograms 
         monitor_vertex( "Vertex", "BCID", myVertex ); 

         // Update good per-BCID vertex counter
         nPassBCIDVtx++;
      }

    }//End looping over tracks

  //monitor number of (passed) vertices, clusters, etc
  auto mon = Monitored::Group(m_monTool,  nVtx, nPassVtx, nPassBCIDVtx, nClusters, timerVertexRec );
  return static_cast<unsigned int>(nPassVtx);
}


void T2VertexBeamSpotTool::reconstructSplitVertices( ConstDataVector<TrackCollection>& myFullTrackCollection,
                                                DataVector< TrigVertexCollection >& mySplitVertexCollections,
                                                T2TrackClusterer& trackClusterer, const EventContext& ctx ) const {
  auto timerVertexRec = Monitored::Timer("TIME_SplitVertexReconstruction");


  // Sort the tracks in phi for splitting in the most independent, uniform variable
  {
    auto timeToSortTracks = Monitored::Timer("TIME_toSortSplitTracks");
    myFullTrackCollection.sort( Beamspot::TrackPhiSort() );
     auto mon = Monitored::Group(m_monTool,  timeToSortTracks ); 
  }

  // This returns m_nSplitVertices (ideally) or fewer (if clustering fails) track collections
  vector< ConstDataVector<TrackCollection> > splitTrackCollections = m_trackManager.split( *myFullTrackCollection.asDataVector(), ctx );

  // Add a new track collection for the split vertices corresponding to this primary vertex
  // There can be anywhere between zero and m_nSplitVertices entries in the collection
  std::unique_ptr<TrigVertexCollection>  splitVertices =  std::make_unique<TrigVertexCollection>();
  mySplitVertexCollections.push_back( splitVertices.get() ); // passes ownership

  // Loop over the split track collections to perform clustering and vertex fitting

  for ( vector< ConstDataVector<TrackCollection> >::iterator tracks = splitTrackCollections.begin(); 
        tracks != splitTrackCollections.end(); ++tracks ){
     TrigVertex* splitVertex = 0;

     if ( m_reclusterSplit ) {
        // Sort the tracks in pT for clustering around the highest-pT seed
        {
           tracks->sort( Beamspot::TrackPTSort() );
        }

        // Cluster in z
        {
           auto timeToZClusterSplit = Monitored::Timer("TIME_toZClusterSplit");
           trackClusterer.cluster( *tracks->asDataVector() );
           auto mon = Monitored::Group(m_monTool,  timeToZClusterSplit ); 
        }

        // Check for a good cluster
        if ( trackClusterer.cluster().size() < m_totalNTrkMin ) continue;


        // Fit a vertex to this split track cluster
        {
           auto timeToVertexFitSplit = Monitored::Timer("TIME_toVertexFitSplit");
           TrackCollection vertexTracks;
           splitVertex = m_primaryVertexFitterTool->fit( &trackClusterer.cluster(), vertexTracks, trackClusterer.seedZ0() );
           auto mon = Monitored::Group(m_monTool,  timeToVertexFitSplit ); 
        }

     }
     else
     {
        // Alternatively: Fit a vertex to the unclustered split track
        // collection, using the seed Z0 from the primary vertex
        {
           auto timeToVertexFitSplit = Monitored::Timer("TIME_toVertexFitSplit");
           TrackCollection vertexTracks;
           splitVertex = m_primaryVertexFitterTool->fit( tracks->asDataVector(), vertexTracks, trackClusterer.seedZ0() );
           auto mon = Monitored::Group(m_monTool,  timeToVertexFitSplit ); 
        }
     }

     if ( splitVertex ) {
        ATH_MSG_DEBUG( "Reconstructed a split vertex");
        // Add split vertex to collection
        splitVertices->push_back( splitVertex );  // passes ownership to split vertex collection
     }
  }

  // Monitor split vertex distributions (if we found all of them)
  if ( m_nSplitVertices > 1 && splitVertices->size() == m_nSplitVertices ) {
     ATH_MSG_DEBUG( "Split vertexing is ON." << "Attempting to split N = " << m_nSplitVertices << " vertices. ");

     // Store information on the first two vertices
     // There shouldn't be more, unless it's for systematic studies anyway
     const T2SplitVertex mySplitVertex( *(*splitVertices)[0], *(*splitVertices)[1] );

     //TODO Fill split vertex histograms
  }

      //Monitor timing
      auto mon = Monitored::Group(m_monTool, timerVertexRec ); 
}




bool PESA::T2VertexBeamSpotTool::isGoodTrack( const T2Track& track ) const {
  // Vertex quality cuts
  return
    (
     abs( track.Pt() )  >= m_minTrackPt       &&
     track.SiHits()     >= m_minSiHits        &&
     track.PIXHits()    >= m_minPIXHits       &&
     track.SCTHits()    >= m_minSCTHits       &&
     track.TRTHits()    >= m_minTRTHits       &&
     track.NDF()        >= m_minTrackNDF      &&
     abs( track.D0() )  <= m_maxTrackD0       &&
     abs( track.Z0() )  <= m_maxTrackZ0       &&
     track.D0err()      <= m_maxTrackD0err    &&
     track.Z0err()      <= m_maxTrackZ0err    &&
     abs( track.Eta() ) <= m_maxTrackEta      &&
     track.Qual()       >= m_minTrackQual     && 
     track.Qual()       <= m_maxTrackQual     &&
     track.Chi2Prob()   >= m_minTrackChi2Prob &&
     true
     );

}


bool T2VertexBeamSpotTool::isGoodVertex( const T2Vertex& vertex ) const {
  // Vertex quality cuts
  return
    (
     vertex.NTrks()       >= m_vtxNTrkMin     &&  // FIXME: harmonize spelling
     abs( vertex.X() )    <= m_vtxXposMax     &&
     abs( vertex.Y() )    <= m_vtxYposMax     &&
     abs( vertex.Z() )    <= m_vtxZposMax     &&
     abs( vertex.Xerr() ) <= m_vtxXerrMax     &&  // FIXME: do we have negative errors?
     abs( vertex.Yerr() ) <= m_vtxYerrMax     &&
     abs( vertex.Zerr() ) <= m_vtxZerrMax     &&
     vertex.Mass()        >= m_vtxMassMin     &&
     vertex.SumPt()       >= m_vtxSumPtMin    &&
     vertex.Qual()        >= m_vtxQualMin     &&
     vertex.Qual()        <= m_vtxQualMax     &&
     vertex.Chi2Prob()    >= m_vtxChi2ProbMin &&
     true
     );
}


bool T2VertexBeamSpotTool::isGoodVertexBCID( const T2Vertex& vertex ) const {
  // Track quality cuts
  return
    (
     vertex.NTrks()       >= m_vtxBCIDNTrkMin &&  // FIXME: harmonize spelling
     true
     );
}


//Monitoring track variables
void T2VertexBeamSpotTool::monitor_tracks(const std::string& prefix, const std::string& suffix, const std::vector<T2Track>& tracks ) const {
   auto  trackPt      = Monitored::Collection(  prefix + "Pt"       + suffix, tracks, [](const T2Track t){ return t.Pt()      ;});
   auto  trackEta     = Monitored::Collection(  prefix + "Eta"      + suffix, tracks, [](const T2Track t){ return t.Eta()     ;});
   auto  trackPhi     = Monitored::Collection(  prefix + "Phi"      + suffix, tracks, [](const T2Track t){ return t.Phi()     ;});
   auto  trackZ0      = Monitored::Collection(  prefix + "Z0"       + suffix, tracks, [](const T2Track t){ return t.Z0()      ;});
   auto  trackD0      = Monitored::Collection(  prefix + "D0"       + suffix, tracks, [](const T2Track t){ return t.D0()      ;});
   auto  trackZ0err   = Monitored::Collection(  prefix + "Z0err"    + suffix, tracks, [](const T2Track t){ return t.Z0err()   ;});
   auto  trackD0err   = Monitored::Collection(  prefix + "D0err"    + suffix, tracks, [](const T2Track t){ return t.D0err()   ;});
   auto  trackNDF     = Monitored::Collection(  prefix + "NDF"      + suffix, tracks, [](const T2Track t){ return t.NDF()     ;});
   auto  trackQual    = Monitored::Collection(  prefix + "Qual"     + suffix, tracks, [](const T2Track t){ return t.Qual()    ;});
   auto  trackChi2Prob= Monitored::Collection(  prefix + "Chi2Prob" + suffix, tracks, [](const T2Track t){ return t.Chi2Prob();});
   auto  trackSiHits  = Monitored::Collection(  prefix + "SiHits"   + suffix, tracks, [](const T2Track t){ return t.SiHits()  ;});
   auto  trackPiHits  = Monitored::Collection(  prefix + "PIXHits"  + suffix, tracks, [](const T2Track t){ return t.PIXHits() ;});
   auto  trackSCTHits = Monitored::Collection(  prefix + "SCTHits"  + suffix, tracks, [](const T2Track t){ return t.SCTHits() ;});
   auto  trackTRTHits = Monitored::Collection(  prefix + "TRTHits"  + suffix, tracks, [](const T2Track t){ return t.TRTHits() ;});
   auto mon = Monitored::Group(m_monTool, trackPt, trackEta, trackPhi, trackZ0, trackD0, trackZ0err, trackD0err,
                               trackNDF, trackQual, trackChi2Prob, trackSiHits, trackPiHits, trackSCTHits, trackTRTHits );
}


void T2VertexBeamSpotTool::monitor_cluster( const T2TrackClusterer& clusterer  ) const {
   auto  clusterZ                = Monitored::Scalar<double>("ClusterZ", clusterer.seedZ0()            );
   auto  clusterNtracks          = Monitored::Scalar<int>("ClusterZ", clusterer.cluster().size()       );
   auto  clusterNUnusedTracks    = Monitored::Scalar<int>("ClusterZ", clusterer.unusedTracks().size()  );

   auto mon = Monitored::Group(m_monTool, clusterZ, clusterNtracks, clusterNUnusedTracks ); 
}


void T2VertexBeamSpotTool::monitor_cluster_tracks(T2TrackClusterer &clusterer, const Trk::Track &track  ) const {
   const double deltaZ0 = track.perigeeParameters()->parameters()[Trk::z0] - clusterer.seedZ0();
   const AmgSymMatrix(5)& perigeeCov = *track.perigeeParameters()->covariance();
   const double z0Error = std::sqrt(perigeeCov(Trk::z0,Trk::z0));
   const double z0Pull  = ( z0Error > 0. ) ? deltaZ0 / z0Error : 0.;

   auto  mon_deltaZ0          = Monitored::Scalar<double>("ClusterDeltaZ0",  deltaZ0    );
   auto  mon_z0Pull           = Monitored::Scalar<double>("ClusterZ0Pull",   z0Pull    );
   auto  mon = Monitored::Group(m_monTool, mon_deltaZ0, mon_z0Pull ); 
}



void T2VertexBeamSpotTool::monitor_vertex(const std::string& prefix, const std::string& suffix, const T2Vertex &vertex ) const {

   auto ntrk      = Monitored::Scalar<int>   ( prefix + "NTrks"      + suffix, vertex.NTrks()      ); 
   auto sumpt     = Monitored::Scalar<double>( prefix + "SumPt"      + suffix, vertex.SumPt()      ); 
   auto sumpt2    = Monitored::Scalar<double>( prefix + "SumPt2"     + suffix, vertex.SumPt2()     ); 
   auto mass      = Monitored::Scalar<double>( prefix + "Mass"       + suffix, vertex.Mass()       );      
   auto qual      = Monitored::Scalar<double>( prefix + "Qual"       + suffix, vertex.Qual()       ); 
   auto chi2      = Monitored::Scalar<double>( prefix + "Chi2Prob"   + suffix, vertex.Chi2Prob()   ); 
   auto x         = Monitored::Scalar<double>( prefix + "X"          + suffix, vertex.X()          ); 
   auto y         = Monitored::Scalar<double>( prefix + "Y"          + suffix, vertex.Y()          ); 
   auto z         = Monitored::Scalar<double>( prefix + "Z"          + suffix, vertex.Z()          ); 
   auto xzoom     = Monitored::Scalar<double>( prefix + "XZoom"      + suffix, vertex.XZoom()      ); 
   auto yzoom     = Monitored::Scalar<double>( prefix + "YZoom"      + suffix, vertex.YZoom()      ); 
   auto zzoom     = Monitored::Scalar<double>( prefix + "ZZoom"      + suffix, vertex.ZZoom()      ); 
   auto xerr      = Monitored::Scalar<double>( prefix + "Xerr"       + suffix, vertex.Xerr()       ); 
   auto yerr      = Monitored::Scalar<double>( prefix + "Yerr"       + suffix, vertex.Yerr()       ); 
   auto zerr      = Monitored::Scalar<double>( prefix + "Zerr"       + suffix, vertex.Zerr()       ); 
   auto xy        = Monitored::Scalar<double>( prefix + "XY"         + suffix, vertex.XY()         ); 
   auto pull      = Monitored::Scalar<double>( prefix + "Pull"       + suffix, vertex.Pull()       ); 
   auto ntrkInVtx = Monitored::Scalar<double>( prefix + "NTrksInVtx" + suffix, vertex.NTrksInVtx() );

   auto mon = Monitored::Group(m_monTool, ntrk, sumpt, sumpt2, mass, qual, chi2, x, y, z, xzoom, yzoom, zzoom, xerr, yerr, zerr, xy, pull, ntrkInVtx ); 
}

  



