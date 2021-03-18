/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

#include <algorithm>
#include <string>
#include <cmath>

// This algorithm
#include "T2VertexBeamSpotTool.h"
#include "T2TrackManager.h"
// Specific to this algorithm
#include "AthContainers/ConstDataVector.h"
#include "TrigInDetEvent/TrigVertex.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "TrigInDetToolInterfaces/ITrigPrimaryVertexFitter.h"
#include "TrkParameters/TrackParameters.h"
//Conversion units
#include "GaudiKernel/SystemOfUnits.h"

using Gaudi::Units::GeV;
using Gaudi::Units::mm;

using std::string;
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
   declareProperty( "PrimaryVertexFitter",    m_primaryVertexFitterTool);

   //Declare variables:
   declareProperty("TotalNTrackMin",  m_totalNTrkMin = 2 ); 

   // Track clustering parameters 
   declareProperty("TrackSeedPt",       m_trackSeedPt       =  1.0*GeV ); 
   declareProperty("TrackClusterDZ",    m_trackClusDZ       = 10.0*mm  ); 
   declareProperty("WeightClusterZ",    m_weightSeed        = true     ); 
   declareProperty("ReclusterSplit",    m_reclusterSplit    = true     ); 
   declareProperty("SplitWholeCluster", m_splitWholeCluster = false    ); 
   declareProperty("ClusterPerigee",    m_clusterPerigee    = "beamspot" );

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

   declareProperty("filterBS",          m_filterBS         = true);
}

StatusCode T2VertexBeamSpotTool::initialize(){
   ATH_MSG_INFO( "Initialising BeamSpot tool" );

   //Initialize data Handles
   ATH_CHECK( m_beamSpotKey.initialize() ) ;
   ATH_CHECK( m_outputVertexCollectionKey.initialize() );

   // Retrieve tools
   ATH_CHECK(m_trackFilterTool.retrieve());
   ATH_CHECK(m_monTool.retrieve());
   ATH_CHECK( m_primaryVertexFitterTool.retrieve() );

   m_clusterTrackPerigee = T2TrackClusterer::trackPerigeeFromString(m_clusterPerigee);

   return StatusCode::SUCCESS;
}


void T2VertexBeamSpotTool::updateBS(const TrackCollection& tracks, const EventContext& ctx) const
{
   // Monitor how long does it take to loop over all collections
   auto tSelectingTracks = Monitored::Timer("TIME_SelectingTracks");

   // Select tracks
   auto selectedTracks = m_trackFilterTool->filter(tracks);
   if (m_filterBS) {
      auto& eventID = ctx.eventID();
      bool has_bs = m_trackFilterTool->updateBS(selectedTracks, eventID.lumi_block(),
                                                eventID.bunch_crossing_id());
      if (has_bs) {
         selectedTracks = m_trackFilterTool->filterBS(selectedTracks);
      }
   }

   //Monitor total all/passed/highPt track numbers
   tSelectingTracks.stop();
   auto nTotalTracks = Monitored::Scalar<unsigned>("nTotalTracks", tracks.size());
   auto nTotalPassedTracks = Monitored::Scalar<unsigned>("nTotalPassedTracks", selectedTracks.size());
   auto nTotalHighPTTracks = Monitored::Scalar<unsigned>("nTotalHighPTTracks", numHighPTTrack(selectedTracks));

   //Monitor total number of tracks
   auto monitor = Monitored::Group(m_monTool, tSelectingTracks, nTotalTracks,
                                   nTotalPassedTracks, nTotalHighPTTracks);
   ATH_MSG_DEBUG("Number of all Tracks: " << nTotalTracks << " Selected Tracks: " << nTotalPassedTracks <<
                 " highPt Tracks: " << nTotalHighPTTracks );

   // Check for seed tracks (= high-pT tracks)
   if (nTotalHighPTTracks == 0) {
      ATH_MSG_DEBUG("No seed tracks for vertexing");
      return;
   }

   // Check for the total number of available tracks
   if (nTotalPassedTracks < m_totalNTrkMin) {
      ATH_MSG_DEBUG( "Not enough total passed tracks to vertex");
      return;
   }

   SG::WriteHandle<TrigVertexCollection> myVertexCollection(m_outputVertexCollectionKey, ctx);
   myVertexCollection = std::make_unique<TrigVertexCollection>();
   DataVector<TrigVertexCollection> mySplitVertexCollections;

   ATH_MSG_DEBUG( "Reconstruct vertices" );
   reconstructVertices(selectedTracks, *myVertexCollection, mySplitVertexCollections, ctx);
}


unsigned T2VertexBeamSpotTool::numHighPTTrack( const TrackVector& tracks ) const
{
   unsigned count = 0;
   for (auto&& track: tracks) {
      const Trk::TrackParameters* trackPars = track->perigeeParameters();
      if (trackPars) {
         float qOverP = trackPars->parameters()[Trk::qOverP];
         float theta = trackPars->parameters()[Trk::theta];
         double pt = std::abs(std::sin(theta)/qOverP)/Gaudi::Units::GeV;
         if (pt > m_trackSeedPt) {
            ++count;
         }
      }
   }
   return count;
}

unsigned int T2VertexBeamSpotTool::reconstructVertices(
      TrackVector& tracks,
      TrigVertexCollection& myVertexCollection,
      DataVector< TrigVertexCollection >&  mySplitVertexCollections,
      const EventContext& ctx) const
{
    ATH_MSG_DEBUG( "Reconstructing vertices" );

   unsigned bcid = ctx.eventID().bunch_crossing_id();

   //Monitoring counters and timers
   auto timerVertexRec = Monitored::Timer("TIME_VertexReconstruction");
   auto nClusters      = Monitored::Scalar<unsigned>("NClusters", 0);
   auto nVtx           = Monitored::Scalar<unsigned>("Nvtx", 0);
   auto nPassVtx       = Monitored::Scalar<unsigned>("NvtxPass", 0);
   auto nPassBCIDVtx   = Monitored::Scalar<unsigned>("NvtxPassBCID", 0);
   auto BCID           = Monitored::Scalar<unsigned>("BCID", bcid);

   // Sort tracks by track pT
   {
      auto timeToSortTracks = Monitored::Timer("TIME_toSortTracks");
      std::sort(tracks.begin(), tracks.end(), Beamspot::TrackPTSort());
      auto mon = Monitored::Group(m_monTool,  timeToSortTracks );
   }

   // Extract beam spot parameters
   SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle { m_beamSpotKey, ctx };
   const InDet::BeamSpotData* indetBeamSpot(*beamSpotHandle);
   const T2BeamSpot beamSpot(indetBeamSpot);
   ATH_MSG_DEBUG( "Beamspot from BeamCondSvc: " << beamSpot);

   // Prepare a track clustering algorithm with the given parameters
   // Should we leave this as a standalone class or merge with the tool?
   T2TrackClusterer trackClusterer( m_trackClusDZ, m_trackSeedPt, m_weightSeed, m_vtxNTrkMax,
                                    m_clusterTrackPerigee );

   std::vector<double> clusterZ0; // Z0 for each cluster, for monitoring only

   // Create clusters from the track collection until all its tracks are used
   while ( ! tracks.empty() ) {

      const Trk::TrackParameters* params = (*tracks.begin())->perigeeParameters();
      float pT = std::abs(sin(params->parameters()[Trk::theta])/params->parameters()[Trk::qOverP]);
      ATH_MSG_DEBUG( "Number of tracks remaining = " << tracks.size() );
      ATH_MSG_DEBUG( "pT of first (seed) track (GeV) = " << pT/GeV );


      // Cluster tracks in z around the first (highest-pT track) in the collection, which is taken
      // as the seed.
      {
         auto timeToZCluster = Monitored::Timer("TIME_toZCluster");
         trackClusterer.cluster(tracks, indetBeamSpot);
         auto mon = Monitored::Group(m_monTool,  timeToZCluster);
      }

      // Sanity check:
      if ( trackClusterer.clusterTracks().size() + trackClusterer.unusedTracks().size()
            != tracks.size() ) {
         ATH_MSG_DEBUG( "Track clustering check sum failed: "
               << "cluster().size()=" << trackClusterer.clusterTracks().size()
               << " + unusedTracks().size()=" << trackClusterer.unusedTracks().size()
               << " != tracks.size()=" << tracks.size()
               );
      }

      // Continue with the remaining tracks - still pT-sorted
      tracks = trackClusterer.unusedTracks();

      // This always uses at least one track (the seed), so we are sure to reduce the track
      // selection and terminate the loop

      // Make sure we have enough tracks in the cluster
      if ( trackClusterer.clusterTracks().size() < m_totalNTrkMin ) {
         ATH_MSG_DEBUG( "Not enough tracks in cluster!" );
         continue;
      }

      // Event has a good cluster
      nClusters++;

      // Monitor properties of of the cluster
      monitor_cluster( trackClusterer );

      // Monitor properties of tracks inside the cluster
      for (auto&& track: trackClusterer.clusterTracks()) {
         monitor_cluster_tracks( trackClusterer,  *track);
      }

      ATH_MSG_DEBUG( "Number of tracks remaining after cluster #(" << nClusters << ") = " << tracks.size());
      ATH_MSG_DEBUG( "Total number of tracks to fit    = " << trackClusterer.clusterTracks().size() );
      ATH_MSG_DEBUG( "Average Z position (from trk Z0) = " << trackClusterer.seedZ0()         );
      ATH_MSG_DEBUG( "Fitting tracks");

      // Fit a primary vertex to this cluster around its seed track
      TrackCollection vertexTracks;
      ConstDataVector<TrackCollection> cluster(trackClusterer.clusterTracks().begin(), trackClusterer.clusterTracks().end());
      TrigVertex* primaryVertex = m_primaryVertexFitterTool->fit(cluster.asDataVector(), vertexTracks, trackClusterer.seedZ0());

      // Check to see if the fit succeeded / converged
      if ( ! primaryVertex ) { 
         ATH_MSG_DEBUG( "Vertex fit failed");
         continue; 
      }

      // Update vertex counter
      nVtx++;

      const T2Vertex myVertex(*primaryVertex, vertexTracks, beamSpot, trackClusterer.seedZ0());

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
      myVertexCollection.push_back(primaryVertex); // passes ownership to vertex collection

      //Monitor parameters of the  passed vertex 
      monitor_vertex( "Vertex", "Pass", myVertex ); 

      //Update good vertex counter
      nPassVtx++;    

      // Learn something more about the cluster that ended up in this vertex
      auto deltaVtxZ = Monitored::Scalar<double>("ClusterDeltaVertexZ",  trackClusterer.seedZ0() - myVertex.Z());
      auto mon = Monitored::Group(m_monTool, deltaVtxZ);

      // monitor cluster-cluster delta Z0
      for (double prevClusterZ0: clusterZ0) {
         auto clusterClusterDeltaZ = Monitored::Scalar<double>("ClusterClusterDeltaZ0",  trackClusterer.seedZ0() - prevClusterZ0);
         auto mon = Monitored::Group(m_monTool,  clusterClusterDeltaZ);
      }
      clusterZ0.push_back(trackClusterer.seedZ0());

      // If the vertex is good, splits are requested, and we have enough tracks to split, split them!
      if ( passVertex && m_nSplitVertices > 1 ) {

         TrackVector mySplitTrackCollection;

         if ( m_splitWholeCluster ) {
            ATH_MSG_DEBUG( "Splitting the entire cluster of tracks into two");
            // Alternative 1: Split the entire cluster of track into two
            mySplitTrackCollection = trackClusterer.clusterTracks();
         } else {
            ATH_MSG_DEBUG( "Splitting only tracks succesfully fitted to a vertex");
            // Alternative 2: Split only the tracks that were successfully fit to a vertex
            mySplitTrackCollection.assign(vertexTracks.begin(), vertexTracks.end());
         }

         if (mySplitTrackCollection.size() >= m_nSplitVertices * m_totalNTrkMin) {
            // Split, optinally re-cluster, and fit separate vertices
            ATH_MSG_DEBUG( "Reconstruct split vertices");
            reconstructSplitVertices( mySplitTrackCollection, mySplitVertexCollections, trackClusterer, indetBeamSpot, ctx );
         }
         // Alternative 3: Split all the tracks and iterate with the remaining tracks
         //          mySplitTrackCollection = tracks;
      }

      ATH_MSG_DEBUG( "Number of tracks remaining = " << tracks.size() );

      // Now check if this vertex passed the higher multiplicity cut to be used for per-BCID measurements
      const bool passVertexBCID = isGoodVertexBCID( myVertex );

      if ( passVertexBCID ) {
         // Fill accepted per-BCID vertex histograms 
         monitor_vertex( "Vertex", "PassBCID", myVertex, bcid );

         // Update good per-BCID vertex counter
         nPassBCIDVtx++;
      }

    }//End looping over tracks

  //monitor number of (passed) vertices, clusters, etc
  auto mon = Monitored::Group(m_monTool, nVtx, nPassVtx, nPassBCIDVtx, nClusters, timerVertexRec, BCID);
  return static_cast<unsigned int>(nPassVtx);
}


void T2VertexBeamSpotTool::reconstructSplitVertices(
      TrackVector& myFullTrackCollection,
      DataVector<TrigVertexCollection>& mySplitVertexCollections,
      T2TrackClusterer& trackClusterer,
      const InDet::BeamSpotData* indetBeamSpot,
      const EventContext& ctx ) const
{
  auto timerVertexRec = Monitored::Timer("TIME_SplitVertexReconstruction");

  // Sort the tracks in phi for splitting in the most independent, uniform variable
  {
    auto timeToSortTracks = Monitored::Timer("TIME_toSortSplitTracks");
    std::sort(myFullTrackCollection.begin(), myFullTrackCollection.end(), Beamspot::TrackPhiSort());
    auto mon = Monitored::Group(m_monTool,  timeToSortTracks );
  }

  // This returns m_nSplitVertices (ideally) or fewer (if clustering fails) track collections
  T2TrackManager trackManager(m_nSplitVertices);
  vector<TrackVector> splitTrackCollections = trackManager.split(myFullTrackCollection, ctx);

  // Add a new track collection for the split vertices corresponding to this primary vertex
  // There can be anywhere between zero and m_nSplitVertices entries in the collection
  TrigVertexCollection* splitVertices =  new TrigVertexCollection();
  mySplitVertexCollections.push_back(splitVertices); // passes ownership

  // Loop over the split track collections to perform clustering and vertex fitting
  for (auto&& tracks: splitTrackCollections) {
     TrigVertex* splitVertex = nullptr;
     ATH_MSG_DEBUG( "split vertex # of tracks " << tracks.size());

     if ( m_reclusterSplit ) {
        // Sort the tracks in pT for clustering around the highest-pT seed
        {
           std::sort(tracks.begin(), tracks.end(), Beamspot::TrackPTSort());
        }

        // Cluster in z
        {
           auto timeToZClusterSplit = Monitored::Timer("TIME_toZClusterSplit");
           trackClusterer.cluster(tracks, indetBeamSpot);
           auto mon = Monitored::Group(m_monTool,  timeToZClusterSplit ); 
        }

        // Check for a good cluster
        if ( trackClusterer.clusterTracks().size() < m_totalNTrkMin ) continue;

        // Fit a vertex to this split track cluster
        {
           auto timeToVertexFitSplit = Monitored::Timer("TIME_toVertexFitSplit");
           TrackCollection vertexTracks;
           ConstDataVector<TrackCollection> cluster(trackClusterer.clusterTracks().begin(), trackClusterer.clusterTracks().end());
           splitVertex = m_primaryVertexFitterTool->fit(cluster.asDataVector(), vertexTracks, trackClusterer.seedZ0());
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
           ConstDataVector<TrackCollection> cluster(tracks.begin(), tracks.end());
           splitVertex = m_primaryVertexFitterTool->fit(cluster.asDataVector(), vertexTracks, trackClusterer.seedZ0() );
           auto mon = Monitored::Group(m_monTool,  timeToVertexFitSplit ); 
        }
     }

     if ( splitVertex ) {
        ATH_MSG_DEBUG( "Reconstructed a split vertex");
        // Add split vertex to collection
        splitVertices->push_back( splitVertex );  // passes ownership to split vertex collection
     } else {
        ATH_MSG_DEBUG( "Could not reconstruct a split vertex");
     }
  }

  // Monitor split vertex distributions (if we found all of them)
  if ( m_nSplitVertices > 1 && splitVertices->size() == m_nSplitVertices ) {
     ATH_MSG_DEBUG( "Split vertexing is ON." << "Attempting to split N = " << m_nSplitVertices << " vertices. ");

     // Store information on the first two vertices
     // There shouldn't be more, unless it's for systematic studies anyway
     const T2SplitVertex splitVertex( *(*splitVertices)[0], *(*splitVertices)[1] );

     monitor_split_vertex("SplitVertex", "Pass", splitVertex);
  }

      //Monitor timing
      auto mon = Monitored::Group(m_monTool, timerVertexRec ); 
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


void T2VertexBeamSpotTool::monitor_cluster( const T2TrackClusterer& clusterer  ) const {
   auto  clusterZ                = Monitored::Scalar<double>("ClusterZ", clusterer.seedZ0()            );
   auto  clusterNtracks          = Monitored::Scalar<int>("ClusterNTracks", clusterer.clusterTracks().size()       );
   auto  clusterNUnusedTracks    = Monitored::Scalar<int>("ClusterNTracksUnused", clusterer.unusedTracks().size()  );

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



void T2VertexBeamSpotTool::monitor_vertex(const std::string& prefix, const std::string& suffix, const T2Vertex &vertex, int bcid ) const {

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

   if (bcid >= 0) {
      auto BCID = Monitored::Scalar<unsigned>("BCID", unsigned(bcid));
      auto mon = Monitored::Group(m_monTool, ntrk, sumpt, sumpt2, mass, qual, chi2, x, y, z, xzoom, yzoom, zzoom, xerr, yerr, zerr, xy, pull, ntrkInVtx, BCID );
   } else {
      auto mon = Monitored::Group(m_monTool, ntrk, sumpt, sumpt2, mass, qual, chi2, x, y, z, xzoom, yzoom, zzoom, xerr, yerr, zerr, xy, pull, ntrkInVtx );
   }
}


void T2VertexBeamSpotTool::monitor_split_vertex(const std::string& prefix, const std::string& suffix, const T2SplitVertex& vertex) const {

   auto ntrk1 = Monitored::Scalar<unsigned>( prefix + "1NTrks"   + suffix, vertex.vertex1().NTrks());
   auto x1 = Monitored::Scalar<double>( prefix + "1X"       + suffix, vertex.vertex1().X());
   auto y1 = Monitored::Scalar<double>( prefix + "1Y"       + suffix, vertex.vertex1().Y());
   auto z1 = Monitored::Scalar<double>( prefix + "1Z"       + suffix, vertex.vertex1().Z());
   auto x1err = Monitored::Scalar<double>( prefix + "1Xerr"    + suffix, vertex.vertex1().Xerr());
   auto y1err = Monitored::Scalar<double>( prefix + "1Yerr"    + suffix, vertex.vertex1().Yerr());
   auto z1err = Monitored::Scalar<double>( prefix + "1Zerr"    + suffix, vertex.vertex1().Zerr());
   auto ntrk2 = Monitored::Scalar<unsigned>( prefix + "2NTrks"   + suffix, vertex.vertex2().NTrks());
   auto x2 = Monitored::Scalar<double>( prefix + "2X"       + suffix, vertex.vertex2().X());
   auto y2 = Monitored::Scalar<double>( prefix + "2Y"       + suffix, vertex.vertex2().Y());
   auto z2 = Monitored::Scalar<double>( prefix + "2Z"       + suffix, vertex.vertex2().Z());
   auto x2err = Monitored::Scalar<double>( prefix + "2Xerr"    + suffix, vertex.vertex2().Xerr());
   auto y2err = Monitored::Scalar<double>( prefix + "2Yerr"    + suffix, vertex.vertex2().Yerr());
   auto z2err = Monitored::Scalar<double>( prefix + "2Zerr"    + suffix, vertex.vertex2().Zerr());
   auto dntrk = Monitored::Scalar<double>( prefix + "DNTrks"   + suffix, vertex.DNTrks());
   auto dx = Monitored::Scalar<double>( prefix + "DX"       + suffix, vertex.DX());
   auto dy = Monitored::Scalar<double>( prefix + "DY"       + suffix, vertex.DY());
   auto dz = Monitored::Scalar<double>( prefix + "DZ"       + suffix, vertex.DZ());
   auto dxerr = Monitored::Scalar<double>( prefix + "DXerr"    + suffix, vertex.DXerr());
   auto dyerr = Monitored::Scalar<double>( prefix + "DYerr"    + suffix, vertex.DYerr());
   auto dzerr = Monitored::Scalar<double>( prefix + "DZerr"    + suffix, vertex.DZerr());
   auto dxpull = Monitored::Scalar<double>( prefix + "DXpull"   + suffix, vertex.DXpull());
   auto dypull = Monitored::Scalar<double>( prefix + "DYpull"   + suffix, vertex.DYpull());
   auto dzpull = Monitored::Scalar<double>( prefix + "DZpull"   + suffix, vertex.DZpull());

   auto mon = Monitored::Group(m_monTool, ntrk1, x1, y1, z1, x1err, y1err, z1err, ntrk2, x2, y2, z2, x2err, y2err, z2err,
                               dntrk, dx, dy, dz, dxerr, dyerr, dzerr, dxpull, dypull, dzpull);
}



