/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================
//
// $Id: T2VertexBeamSpot.cxx 760210 2016-07-07 09:33:44Z hartj $
//
// T2VertexBeamSpot.cxx, (c) ATLAS Detector software
// Trigger/TrigAlgorithms/TrigT2BeamSpot/T2VertexBeamSpot
//
// Online beam spot measurement and monitoring using
// L2 recontructed primary vertices.
//
// Authors : David W. Miller, Rainer Bartoldus,   
//           Su Dong
//
// Date : 11 May, 2008
//
//============================================================

// This algorithm
#include "TrigT2BeamSpot/T2VertexBeamSpot.h"
#include "T2VertexBeamSpotImpl.h"
#include "T2TrackClusterer.h"
#include "T2Timer.h"

// General ATHENA/Trigger stuff
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "TrigNavigation/TriggerElement.h"
#include "GaudiKernel/SystemOfUnits.h"
using Gaudi::Units::mm;
using Gaudi::Units::GeV;

// Specific to this algorithm
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrackHelper.h"
#include "TrigInDetEvent/TrigVertexCollection.h"

// Related to TOOLS
#include "TrigInDetToolInterfaces/ITrigPrimaryVertexFitter.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"

// Timer service
#include "TrigTimeAlgs/TrigTimerSvc.h"

// Standard headers
#include <cmath>
// #include <sstream>
// using std::ostringstream;

using std::string;

using namespace PESA;


//-------------
// Constructor
//-------------

T2VertexBeamSpot::T2VertexBeamSpot( const std::string& name, ISvcLocator* pSvcLocator )
  : HLT::AllTEAlgo(name, pSvcLocator)
  , impl( new T2VertexBeamSpotImpl( this ) )
{
   
  // Beam Conditions Service usage
  declareProperty("BeamCondSvcName", impl->m_beamCondSvcName = "BeamCondSvc" );
   
  // Read cuts 
  declareProperty("AcceptAll",       impl->m_AcceptAll    = false );
  declareProperty("TotalNTrackMin",  impl->m_totalNTrkMin = 2 );
  
  // Detail for plots
  declareProperty("SaveAllVertices", impl->m_saveAllVertices = false );
  
  // Track clustering parameters
  declareProperty("TrackSeedPt",       impl->m_trackSeedPt       =  1.0*GeV );
  declareProperty("TrackClusterDZ",    impl->m_trackClusDZ       = 10.0*mm  );
  declareProperty("WeightClusterZ",    impl->m_weightSeed        = true     );
  declareProperty("ReclusterSplit",    impl->m_reclusterSplit    = true     );
  declareProperty("SplitWholeCluster", impl->m_splitWholeCluster = false    );
  
  // Track Selection criteria
  declareProperty("TrackAlgoID",     impl->m_TrackAlgoId      = TrigInDetTrack::IDSCANID);
  declareProperty("TrackMinPt",      impl->m_minTrackPt       =  0.7*GeV );
  declareProperty("TrackMaxPt",      impl->m_maxTrackPt       = 200.*GeV );
  declareProperty("TrackMinEta",     impl->m_minTrackEta      =   0.     );
  declareProperty("TrackMaxEta",     impl->m_maxTrackEta      =   2.5    );
  declareProperty("TrackMaxZ0",      impl->m_maxTrackZ0       = 200.*mm  );
  declareProperty("TrackMaxD0",      impl->m_maxTrackD0       =  10.*mm  );
  declareProperty("TrackMaxZ0err",   impl->m_maxTrackZ0err    =   2.*mm  );
  declareProperty("TrackMaxD0err",   impl->m_maxTrackD0err    =   2.*mm  );
  declareProperty("TrackMinNDF",     impl->m_minTrackNDF      =   2      );
  declareProperty("TrackMinQual",    impl->m_minTrackQual     =   0.2    );
  declareProperty("TrackMaxQual",    impl->m_maxTrackQual     =   5.0    );
  declareProperty("TrackMinChi2Prob",impl->m_minTrackChi2Prob = -10.0    );
  declareProperty("TrackMinSiHits",  impl->m_minSiHits        =   4      );
  declareProperty("TrackMinPIXHits", impl->m_minPIXHits       =   1      );
  declareProperty("TrackMinSCTHits", impl->m_minSCTHits       =   3      );
  declareProperty("TrackMinTRTHits", impl->m_minTRTHits       =   1      );
  
  // Vertex Selection criteria
  declareProperty("VertexMinNTrk",     impl->m_vtxNTrkMin     =   2      );
  declareProperty("VertexMaxNTrk",     impl->m_vtxNTrkMax     = 100      );
  declareProperty("VertexMinQual",     impl->m_vtxQualMin     =   0.0    );
  declareProperty("VertexMaxQual",     impl->m_vtxQualMax     =   5.     );
  declareProperty("VertexMinChi2Prob", impl->m_vtxChi2ProbMin = -10.0    );
  declareProperty("VertexMinMass",     impl->m_vtxMassMin     =  -1.*GeV );
  declareProperty("VertexMinSumPt",    impl->m_vtxSumPtMin    =   0.*GeV );
  declareProperty("VertexMaxX",        impl->m_vtxXposMax     =  10.*mm  );
  declareProperty("VertexMaxXerr",     impl->m_vtxXerrMax     =   1.*mm  );
  declareProperty("VertexMaxY",        impl->m_vtxYposMax     =  10.*mm  );
  declareProperty("VertexMaxYerr",     impl->m_vtxYerrMax     =   1.*mm  );
  declareProperty("VertexMaxZ",        impl->m_vtxZposMax     = 200.*mm  );
  declareProperty("VertexMaxZerr",     impl->m_vtxZerrMax     =   1.*mm  );

  // Vertex selection for use in BCID measurements
  declareProperty("VertexBCIDMinNTrk", impl->m_vtxBCIDNTrkMin =   2      );

  // Vertexing
  declareProperty("doVertexFit",       impl->m_doVertexFit = true);

  // Use TrigInDetTracks instead of Trk::Tracks
  declareProperty("doTrigInDetTrack",       m_doTrigInDetTrack = false);
  
  //-----------------------
  // activate TE  
  //-----------------------
  declareProperty("activateTE",          impl->m_activateTE          = false               );
  declareProperty("activateAllTE",       impl->m_activateAllTE       = false               );
  declareProperty("attachVertices",      impl->m_attachVertices      = false               );
  declareProperty("attachSplitVertices", impl->m_attachSplitVertices = false               );
  declareProperty("nSplitVertices",      impl->m_nSplitVertices      = 1                   );           
  declareProperty("vertexCollName",      impl->m_vertexCollName      = "TrigBeamSpotVertex"); 
 
  //---------------------------
  // Single interaction trigger
  //---------------------------
  declareProperty("minNpvTrigger",  impl->m_minNpvTrigger  = 0);
  declareProperty("maxNpvTrigger",  impl->m_maxNpvTrigger  = 2);
  declareProperty("activateSI",     impl->m_activateSI     = false);
  
  // Number of Z blocks
  declareProperty( "NumberZClusters", impl->m_nZfitBins       = 10 );
  declareProperty( "ZLowerLimit",     impl->m_ZLowerLimit     = -200.*mm );
  declareProperty( "ZUpperLimit",     impl->m_ZUpperLimit     =  200.*mm );
  
  //-----------------------
  // Monitoring
  //-----------------------

  // Variables for monitoring histograms
  const IMonitoredAlgo::ContainerReset reset = IMonitoredAlgo::AutoClear;

  // per-Job variables, which need to be reset
  declareMonitoredVariable("TotalEvents" ,        impl->m_TotalEvents);

  // per-Event variables, which need to be reset
  declareMonitoredVariable("RunID" ,              impl->m_RunID);
  declareMonitoredVariable("EventID" ,            impl->m_EventID);
  declareMonitoredVariable("LumiBlock",           impl->m_LumiBlock);
  declareMonitoredVariable("BCID",                impl->m_bcid);
                                               
  declareMonitoredVariable("TotalTEs",            impl->m_TotalTEs);
  declareMonitoredVariable("TotalROIs",           impl->m_TotalROIs);
  declareMonitoredVariable("TotalTracks" ,        impl->m_TotalTracks);
  declareMonitoredVariable("TotalTracksPass",     impl->m_TotalTracksPass);
  declareMonitoredVariable("TotalHiPTTracks",     impl->m_TotalTracksPass);
  
  declareMonitoredVariable("NClusters",           impl->m_NClusters);
  declareMonitoredVariable("Nvtx",                impl->m_Nvtx);
  declareMonitoredVariable("NvtxPass",            impl->m_NvtxPass);
  declareMonitoredVariable("NvtxPassBCID",        impl->m_NvtxPassBCID);

  // Containers of per per-Event, per-TE, per-ROI variables
  declareMonitoredStdContainer("EventStatistics", impl->m_eventStage, reset );
  
  declareMonitoredStdContainer("ROIperTE",        impl->m_ROIperTE        , reset );
  declareMonitoredStdContainer("TracksPerTE",     impl->m_TracksPerTE     , reset );
  declareMonitoredStdContainer("TracksPerTEPass", impl->m_TracksPerTEPass , reset );
  declareMonitoredStdContainer("HiPTTracksPerTE", impl->m_HiPTTracksPerTE , reset );

  declareMonitoredStdContainer("TracksPerROI",    impl->m_TracksPerROI    , reset );
  declareMonitoredStdContainer("TracksPerROIPass",impl->m_TracksPerROIPass, reset );
  declareMonitoredStdContainer("HiPTTracksPerROI",impl->m_HiPTTracksPerROI, reset );

  declareMonitoredStdContainer("RoIEta",          impl->m_RoIEta, reset );
  declareMonitoredStdContainer("RoIPhi",          impl->m_RoIPhi, reset );
  declareMonitoredStdContainer("RoIID",           impl->m_RoIID , reset );
  
  declareMonitoredStdContainer("ClusterNTracks",       impl->m_clusterNTracks      , reset );
  declareMonitoredStdContainer("ClusterNTracksUnused", impl->m_clusterNTracksUnused, reset );
  declareMonitoredStdContainer("ClusterZ",             impl->m_clusterZ            , reset );
  declareMonitoredStdContainer("ClusterDeltaZ0",       impl->m_clusterDeltaZ0      , reset );
  declareMonitoredStdContainer("ClusterZ0Pull",        impl->m_clusterZ0Pull       , reset );
  declareMonitoredStdContainer("ClusterDeltaVertexZ",  impl->m_clusterDeltaVertexZ , reset );
  declareMonitoredStdContainer("ClusterVertexZPull",   impl->m_clusterVertexZPull  , reset );

  impl->m_trackPass      .declare( this, "Track"       , "Pass"     );
  impl->m_vertex         .declare( this, "Vertex"                   );
  impl->m_vertexPass     .declare( this, "Vertex"      , "Pass"     );   
  impl->m_vertexPassBCID .declare( this, "Vertex"      , "PassBCID" );   
  impl->m_splitVertexPass.declare( this, "SplitVertex" , "Pass"     );
}


//------------
// Destructor
//------------

T2VertexBeamSpot::~T2VertexBeamSpot()
{
  delete impl;
}


//----------------
// Initialization
//----------------

HLT::ErrorCode
T2VertexBeamSpot::hltInitialize()
{
  StatusCode sc;
  
  // Vertex fitting tool
  sc = toolSvc()->retrieveTool( "TrigPrimaryVertexFitter", "TrigPrimaryVertexFitter", impl->m_primaryVertexFitter );
  if ( sc.isFailure() || ! impl->m_primaryVertexFitter )
    { 
      msg() << MSG::ERROR << "Unable to locate TrigPrimaryVertexFitter tool." << endreq;
      return HLT::BAD_JOB_SETUP;
    }

  // Pick up the beam conditions service
  sc = service( impl->m_beamCondSvcName, impl->m_beamCondSvc );
  if ( sc.isFailure() || ! impl->m_beamCondSvc )
    {
      msg() << MSG::ERROR << "Failed to retrieve BeamCondSvc: " <<  impl->m_beamCondSvcName << endreq;
      return HLT::BAD_JOB_SETUP;
    }

  // Set up execution timers
  {
    //      impl->m_timer.resize(numTimers);
    impl->m_timer[total         ] = addTimer("TotalAlgoTime"     );
    impl->m_timer[allTE         ] = addTimer("LoopOverInputTEs"  );
    impl->m_timer[allROI        ] = addTimer("LoopOverROIs"      );
    impl->m_timer[allTrack      ] = addTimer("LoopOverTracks"    );
    impl->m_timer[allVertex     ] = addTimer("Vertexing"         );
    impl->m_timer[allVertexSplit] = addTimer("SplitVertexing"    );
    impl->m_timer[allOutput     ] = addTimer("OutputTEWriting"   );
    impl->m_timer[trackSort     ] = addTimer("TrackSorting"      );
    impl->m_timer[trackSortSplit] = addTimer("TrackSortingSplit" );
    impl->m_timer[vertexFit     ] = addTimer("VertexFit"         );
    impl->m_timer[vertexFitSplit] = addTimer("VertexFitSplit"    );
    impl->m_timer[zCluster      ] = addTimer("ZClustering"       );
    impl->m_timer[zClusterSplit ] = addTimer("ZClusteringSplit"  );
  }

  // Set up the track z clustering tool
  impl->m_trackClusterer.reset( new T2TrackClusterer( impl->m_trackClusDZ, impl->m_trackSeedPt, impl->m_weightSeed, impl->m_vtxNTrkMax ) );

  // Reset per-Job monitored variables
  impl->m_TotalEvents = 0;

  msg() << MSG::INFO << "Initialization of T2VertexBeamSpot successful" << endreq;
  return HLT::OK;
}


//------------
// Event loop
//------------

HLT::ErrorCode
T2VertexBeamSpot::hltExecute( std::vector<std::vector<HLT::TriggerElement*> >& tes_in,
                              unsigned int type_out )
{
  // Say hello
  if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG << "In T2VertexBeamSpot::hltExecute" << endreq;  

  // Start monitoring
  beforeExecMonitors().ignore();
  
  // Start the overall timer
  T2Timer t( impl->m_timer[total] );
  
  // Initialize booleans for event stats
  impl->m_eventStageFlag = std::vector<bool>( numStatistics, false );

  // Reset monitored vars that are not reset by the steering
  impl->resetMonitoredVariables();
  // FIXME: this should be done by IMonitoredAlgo

  impl->m_TotalEvents++;
  impl->eventStage( allInput );

  // Initialize vertex collections
  TrigVertexCollection myVertexCollection;
  DataVector< TrigVertexCollection > mySplitVertexCollections;

  // Be optimistic
  HLT::ErrorCode errorCode = HLT::OK;

  // Process event - break to do cleanup before returning
  do 
    {
      // Retrieve event info
      const EventInfo* pEventInfo;
      if ( store()->retrieve(pEventInfo).isFailure() )
        {
          msg() << MSG::ERROR << "Failed to get EventInfo" << endreq;
          errorCode = HLT::ERROR;
          break;
        }

      impl->m_RunID     = pEventInfo->event_ID()->run_number();
      impl->m_EventID   = pEventInfo->event_ID()->event_number();
      impl->m_LumiBlock = pEventInfo->event_ID()->lumi_block();
      impl->m_bcid      = pEventInfo->event_ID()->bunch_crossing_id();

      if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG << "Run #" << impl->m_RunID << "; Event #" << impl->m_EventID
                                      << "; Lumi block #" << impl->m_LumiBlock << "; BCID #" << impl->m_bcid << endreq;

      // Count input TEs
      if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG << "Number of input TEs = " << tes_in.size() << endreq;
      impl->m_TotalTEs = tes_in.size();

      // Check for input TEs
      if ( tes_in.empty() )
        {
          if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG << " No trigger elements" << endreq;
          break;
        }
  
      impl->eventStage( hasTE );
 
      //-----------------
      // Track selection
      //-----------------

      // Create collections of selected tracks
      // TrigInDetTrackCollection mySelectedTrackCollection( SG::VIEW_ELEMENTS );
      if (m_doTrigInDetTrack) {
        TrigInDetTrackCollection mySelectedTrackCollection;
        mySelectedTrackCollection.clear( SG::VIEW_ELEMENTS );
        // FIXME: TrigInDetTrackCollection neglects to forward the
        // DataVector<TrigInDetTrack>(SG::OwnershipPolicy) constructor

        // Process all TEs (and in turn all ROIs in the TE, all tracks in the ROI)
        impl->processTEs( tes_in, mySelectedTrackCollection );

        //-------------------------
        // Requirements for tracks
        //-------------------------

        // Check for seed tracks (= high-pT tracks)
        if ( impl->m_TotalHiPTTracks < 1 )
        {
          if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG << " No seed tracks for vertex" << endreq;
          break;
        }

        impl->eventStage( hasSeedTrack );

        // Check for the total number of available tracks
        if ( impl->m_TotalTracksPass < impl->m_totalNTrkMin )
        {
          if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG << " Not enough total passed tracks to vertex" << endreq;
          break;
        }

        impl->eventStage( enoughTracks );

        //-----------------------
        // Vertex reconstruction
        //-----------------------

        // Cluster tracks in z around seed track and reconstruct vertices
        impl->reconstructVertices( mySelectedTrackCollection, myVertexCollection, mySplitVertexCollections );
      }
    else {
        TrackCollection mySelectedTrackCollection;
        mySelectedTrackCollection.clear( SG::VIEW_ELEMENTS );
        // FIXME: TrigInDetTrackCollection neglects to forward the
        // DataVector<TrigInDetTrack>(SG::OwnershipPolicy) constructor

        // Process all TEs (and in turn all ROIs in the TE, all tracks in the ROI)
        impl->processTEs( tes_in, mySelectedTrackCollection );

        //-------------------------
        // Requirements for tracks
        //-------------------------

        // Check for seed tracks (= high-pT tracks)
        if ( impl->m_TotalHiPTTracks < 1 )
        {
          if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG << " No seed tracks for vertex" << endreq;
          break;
        }

        impl->eventStage( hasSeedTrack );

        // Check for the total number of available tracks
        if ( impl->m_TotalTracksPass < impl->m_totalNTrkMin )
        {
          if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG << " Not enough total passed tracks to vertex" << endreq;
          break;
        }

        impl->eventStage( enoughTracks );

        //-----------------------
        // Vertex reconstruction
        //-----------------------

        // Cluster tracks in z around seed track and reconstruct vertices
        impl->reconstructVertices( mySelectedTrackCollection, myVertexCollection, mySplitVertexCollections );
      }
    } while (false);

  //------------------------------------
  // Store output (if any) in the event
  //------------------------------------

  // Add output TEs and attach vertex collections as features if requested
  // JKirk 7/7/16 Need to attach to input TEs
  impl->createOutputTEs( myVertexCollection, mySplitVertexCollections, type_out , tes_in );

  // Stop monitoring
  afterExecMonitors().ignore();

  // Return cause you're done!
  return errorCode;
}


//----------
// Finalize
//----------

HLT::ErrorCode
T2VertexBeamSpot::hltFinalize()
{
  msg() << MSG::INFO << "T2VertexBeamSpot finalize:" << endreq;

  //typedef std::map<std::string, IMonitoredAlgo::IMonitoredVariable*> MapType;

  if ( msgLvl()<=MSG::DEBUG )
    {
      for ( IMonitoredAlgo::VariablesMap::const_iterator i = variables().begin(); i != variables().end(); ++i )
        {
          msg() << MSG::DEBUG << i->first << " =";
	  //          const IMonitoredVariable* v = i->second;
          for ( unsigned j = 0; j < i->second->size(); ++j )
            msg() << " " << i->second->get(j);
          msg() << endreq;
        }
    }

  msg() << MSG::INFO << "T2VertexBeamSpot done." << endreq;

  return HLT::OK;
}

