/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================
//
// $Id: T2VertexBeamSpot.cxx 793164 2017-01-20 03:59:26Z ssnyder $
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
using Gaudi::Units::mm;
using Gaudi::Units::GeV;


//-------------
// Constructor
//-------------

T2VertexBeamSpot::T2VertexBeamSpot( const std::string& name, ISvcLocator* pSvcLocator )
  : HLT::AllTEAlgo(name, pSvcLocator)
  , m_impl( new T2VertexBeamSpotImpl( this ) )
{
   
  // Beam Conditions Service usage
  declareProperty("BeamCondSvcName", m_impl->m_beamCondSvcName = "BeamCondSvc" );
   
  // Read cuts 
  declareProperty("AcceptAll",       m_impl->m_AcceptAll    = false );
  declareProperty("TotalNTrackMin",  m_impl->m_totalNTrkMin = 2 );
  
  // Detail for plots
  declareProperty("SaveAllVertices", m_impl->m_saveAllVertices = false );
  
  // Track clustering parameters
  declareProperty("TrackSeedPt",       m_impl->m_trackSeedPt       =  1.0*GeV );
  declareProperty("TrackClusterDZ",    m_impl->m_trackClusDZ       = 10.0*mm  );
  declareProperty("WeightClusterZ",    m_impl->m_weightSeed        = true     );
  declareProperty("ReclusterSplit",    m_impl->m_reclusterSplit    = true     );
  declareProperty("SplitWholeCluster", m_impl->m_splitWholeCluster = false    );
  
  // Track Selection criteria
  declareProperty("TrackAlgoID",     m_impl->m_TrackAlgoId      = TrigInDetTrack::IDSCANID);
  declareProperty("TrackMinPt",      m_impl->m_minTrackPt       =  0.7*GeV );
  declareProperty("TrackMaxPt",      m_impl->m_maxTrackPt       = 200.*GeV );
  declareProperty("TrackMinEta",     m_impl->m_minTrackEta      =   0.     );
  declareProperty("TrackMaxEta",     m_impl->m_maxTrackEta      =   2.5    );
  declareProperty("TrackMaxZ0",      m_impl->m_maxTrackZ0       = 200.*mm  );
  declareProperty("TrackMaxD0",      m_impl->m_maxTrackD0       =  10.*mm  );
  declareProperty("TrackMaxZ0err",   m_impl->m_maxTrackZ0err    =   2.*mm  );
  declareProperty("TrackMaxD0err",   m_impl->m_maxTrackD0err    =   2.*mm  );
  declareProperty("TrackMinNDF",     m_impl->m_minTrackNDF      =   2      );
  declareProperty("TrackMinQual",    m_impl->m_minTrackQual     =   0.2    );
  declareProperty("TrackMaxQual",    m_impl->m_maxTrackQual     =   5.0    );
  declareProperty("TrackMinChi2Prob",m_impl->m_minTrackChi2Prob = -10.0    );
  declareProperty("TrackMinSiHits",  m_impl->m_minSiHits        =   4      );
  declareProperty("TrackMinPIXHits", m_impl->m_minPIXHits       =   1      );
  declareProperty("TrackMinSCTHits", m_impl->m_minSCTHits       =   3      );
  declareProperty("TrackMinTRTHits", m_impl->m_minTRTHits       =   1      );
  
  // Vertex Selection criteria
  declareProperty("VertexMinNTrk",     m_impl->m_vtxNTrkMin     =   2      );
  declareProperty("VertexMaxNTrk",     m_impl->m_vtxNTrkMax     = 100      );
  declareProperty("VertexMinQual",     m_impl->m_vtxQualMin     =   0.0    );
  declareProperty("VertexMaxQual",     m_impl->m_vtxQualMax     =   5.     );
  declareProperty("VertexMinChi2Prob", m_impl->m_vtxChi2ProbMin = -10.0    );
  declareProperty("VertexMinMass",     m_impl->m_vtxMassMin     =  -1.*GeV );
  declareProperty("VertexMinSumPt",    m_impl->m_vtxSumPtMin    =   0.*GeV );
  declareProperty("VertexMaxX",        m_impl->m_vtxXposMax     =  10.*mm  );
  declareProperty("VertexMaxXerr",     m_impl->m_vtxXerrMax     =   1.*mm  );
  declareProperty("VertexMaxY",        m_impl->m_vtxYposMax     =  10.*mm  );
  declareProperty("VertexMaxYerr",     m_impl->m_vtxYerrMax     =   1.*mm  );
  declareProperty("VertexMaxZ",        m_impl->m_vtxZposMax     = 200.*mm  );
  declareProperty("VertexMaxZerr",     m_impl->m_vtxZerrMax     =   1.*mm  );

  // Vertex selection for use in BCID measurements
  declareProperty("VertexBCIDMinNTrk", m_impl->m_vtxBCIDNTrkMin =   2      );

  // Vertexing
  declareProperty("doVertexFit",       m_impl->m_doVertexFit = true);

  // Use TrigInDetTracks instead of Trk::Tracks
  declareProperty("doTrigInDetTrack",       m_doTrigInDetTrack = false);
  
  //-----------------------
  // activate TE  
  //-----------------------
  declareProperty("activateTE",          m_impl->m_activateTE          = false               );
  declareProperty("activateAllTE",       m_impl->m_activateAllTE       = false               );
  declareProperty("attachVertices",      m_impl->m_attachVertices      = false               );
  declareProperty("attachSplitVertices", m_impl->m_attachSplitVertices = false               );
  declareProperty("nSplitVertices",      m_impl->m_nSplitVertices      = 1                   );           
  declareProperty("vertexCollName",      m_impl->m_vertexCollName      = "TrigBeamSpotVertex"); 
 
  //---------------------------
  // Single interaction trigger
  //---------------------------
  declareProperty("minNpvTrigger",  m_impl->m_minNpvTrigger  = 0);
  declareProperty("maxNpvTrigger",  m_impl->m_maxNpvTrigger  = 2);
  declareProperty("activateSI",     m_impl->m_activateSI     = false);
  
  // Number of Z blocks
  declareProperty( "NumberZClusters", m_impl->m_nZfitBins       = 10 );
  declareProperty( "ZLowerLimit",     m_impl->m_ZLowerLimit     = -200.*mm );
  declareProperty( "ZUpperLimit",     m_impl->m_ZUpperLimit     =  200.*mm );
  
  //-----------------------
  // Monitoring
  //-----------------------

  // Variables for monitoring histograms
  const IMonitoredAlgo::ContainerReset reset = IMonitoredAlgo::AutoClear;

  // per-Job variables, which need to be reset
  declareMonitoredVariable("TotalEvents" ,        m_impl->m_TotalEvents);

  // per-Event variables, which need to be reset
  declareMonitoredVariable("RunID" ,              m_impl->m_RunID);
  declareMonitoredVariable("EventID" ,            m_impl->m_EventID);
  declareMonitoredVariable("LumiBlock",           m_impl->m_LumiBlock);
  declareMonitoredVariable("BCID",                m_impl->m_bcid);
                                               
  declareMonitoredVariable("TotalTEs",            m_impl->m_TotalTEs);
  declareMonitoredVariable("TotalROIs",           m_impl->m_TotalROIs);
  declareMonitoredVariable("TotalTracks" ,        m_impl->m_TotalTracks);
  declareMonitoredVariable("TotalTracksPass",     m_impl->m_TotalTracksPass);
  declareMonitoredVariable("TotalHiPTTracks",     m_impl->m_TotalTracksPass);
  
  declareMonitoredVariable("NClusters",           m_impl->m_NClusters);
  declareMonitoredVariable("Nvtx",                m_impl->m_Nvtx);
  declareMonitoredVariable("NvtxPass",            m_impl->m_NvtxPass);
  declareMonitoredVariable("NvtxPassBCID",        m_impl->m_NvtxPassBCID);

  // Containers of per per-Event, per-TE, per-ROI variables
  declareMonitoredStdContainer("EventStatistics", m_impl->m_eventStage, reset );
  
  declareMonitoredStdContainer("ROIperTE",        m_impl->m_ROIperTE        , reset );
  declareMonitoredStdContainer("TracksPerTE",     m_impl->m_TracksPerTE     , reset );
  declareMonitoredStdContainer("TracksPerTEPass", m_impl->m_TracksPerTEPass , reset );
  declareMonitoredStdContainer("HiPTTracksPerTE", m_impl->m_HiPTTracksPerTE , reset );

  declareMonitoredStdContainer("TracksPerROI",    m_impl->m_TracksPerROI    , reset );
  declareMonitoredStdContainer("TracksPerROIPass",m_impl->m_TracksPerROIPass, reset );
  declareMonitoredStdContainer("HiPTTracksPerROI",m_impl->m_HiPTTracksPerROI, reset );

  declareMonitoredStdContainer("RoIEta",          m_impl->m_RoIEta, reset );
  declareMonitoredStdContainer("RoIPhi",          m_impl->m_RoIPhi, reset );
  declareMonitoredStdContainer("RoIID",           m_impl->m_RoIID , reset );
  
  declareMonitoredStdContainer("ClusterNTracks",       m_impl->m_clusterNTracks      , reset );
  declareMonitoredStdContainer("ClusterNTracksUnused", m_impl->m_clusterNTracksUnused, reset );
  declareMonitoredStdContainer("ClusterZ",             m_impl->m_clusterZ            , reset );
  declareMonitoredStdContainer("ClusterDeltaZ0",       m_impl->m_clusterDeltaZ0      , reset );
  declareMonitoredStdContainer("ClusterZ0Pull",        m_impl->m_clusterZ0Pull       , reset );
  declareMonitoredStdContainer("ClusterDeltaVertexZ",  m_impl->m_clusterDeltaVertexZ , reset );
  declareMonitoredStdContainer("ClusterVertexZPull",   m_impl->m_clusterVertexZPull  , reset );

  m_impl->m_trackPass      .declare( this, "Track"       , "Pass"     );
  m_impl->m_vertex         .declare( this, "Vertex"                   );
  m_impl->m_vertexPass     .declare( this, "Vertex"      , "Pass"     );   
  m_impl->m_vertexPassBCID .declare( this, "Vertex"      , "PassBCID" );   
  m_impl->m_splitVertexPass.declare( this, "SplitVertex" , "Pass"     );
}


//------------
// Destructor
//------------

T2VertexBeamSpot::~T2VertexBeamSpot()
{
  delete m_impl;
}


//----------------
// Initialization
//----------------

HLT::ErrorCode
T2VertexBeamSpot::hltInitialize()
{
  StatusCode sc;
  
  // Vertex fitting tool
  sc = toolSvc()->retrieveTool( "TrigPrimaryVertexFitter", "TrigPrimaryVertexFitter", m_impl->m_primaryVertexFitter );
  if ( sc.isFailure() || ! m_impl->m_primaryVertexFitter )
    { 
      msg() << MSG::ERROR << "Unable to locate TrigPrimaryVertexFitter tool." << endmsg;
      return HLT::BAD_JOB_SETUP;
    }

  // Pick up the beam conditions service
  sc = service( m_impl->m_beamCondSvcName, m_impl->m_beamCondSvc );
  if ( sc.isFailure() || ! m_impl->m_beamCondSvc )
    {
      msg() << MSG::ERROR << "Failed to retrieve BeamCondSvc: " <<  m_impl->m_beamCondSvcName << endmsg;
      return HLT::BAD_JOB_SETUP;
    }

  // Set up execution timers
  {
    //      m_impl->m_timer.resize(numTimers);
    m_impl->m_timer[total         ] = addTimer("TotalAlgoTime"     );
    m_impl->m_timer[allTE         ] = addTimer("LoopOverInputTEs"  );
    m_impl->m_timer[allROI        ] = addTimer("LoopOverROIs"      );
    m_impl->m_timer[allTrack      ] = addTimer("LoopOverTracks"    );
    m_impl->m_timer[allVertex     ] = addTimer("Vertexing"         );
    m_impl->m_timer[allVertexSplit] = addTimer("SplitVertexing"    );
    m_impl->m_timer[allOutput     ] = addTimer("OutputTEWriting"   );
    m_impl->m_timer[trackSort     ] = addTimer("TrackSorting"      );
    m_impl->m_timer[trackSortSplit] = addTimer("TrackSortingSplit" );
    m_impl->m_timer[vertexFit     ] = addTimer("VertexFit"         );
    m_impl->m_timer[vertexFitSplit] = addTimer("VertexFitSplit"    );
    m_impl->m_timer[zCluster      ] = addTimer("ZClustering"       );
    m_impl->m_timer[zClusterSplit ] = addTimer("ZClusteringSplit"  );
  }

  // Set up the track z clustering tool
  m_impl->m_trackClusterer.reset( new T2TrackClusterer( m_impl->m_trackClusDZ, m_impl->m_trackSeedPt, m_impl->m_weightSeed, m_impl->m_vtxNTrkMax ) );

  // Reset per-Job monitored variables
  m_impl->m_TotalEvents = 0;

  msg() << MSG::INFO << "Initialization of T2VertexBeamSpot successful" << endmsg;
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
  if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG << "In T2VertexBeamSpot::hltExecute" << endmsg;  

  // Start monitoring
  beforeExecMonitors().ignore();
  
  // Start the overall timer
  T2Timer t( m_impl->m_timer[total] );
  
  // Initialize booleans for event stats
  m_impl->m_eventStageFlag = std::vector<bool>( numStatistics, false );

  // Reset monitored vars that are not reset by the steering
  m_impl->resetMonitoredVariables();
  // FIXME: this should be done by IMonitoredAlgo

  m_impl->m_TotalEvents++;
  m_impl->eventStage( allInput );

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
          msg() << MSG::ERROR << "Failed to get EventInfo" << endmsg;
          errorCode = HLT::ERROR;
          break;
        }

      m_impl->m_RunID     = pEventInfo->event_ID()->run_number();
      m_impl->m_EventID   = pEventInfo->event_ID()->event_number();
      m_impl->m_LumiBlock = pEventInfo->event_ID()->lumi_block();
      m_impl->m_bcid      = pEventInfo->event_ID()->bunch_crossing_id();

      if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG << "Run #" << m_impl->m_RunID << "; Event #" << m_impl->m_EventID
                                      << "; Lumi block #" << m_impl->m_LumiBlock << "; BCID #" << m_impl->m_bcid << endmsg;

      // Count input TEs
      if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG << "Number of input TEs = " << tes_in.size() << endmsg;
      m_impl->m_TotalTEs = tes_in.size();

      // Check for input TEs
      if ( tes_in.empty() )
        {
          if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG << " No trigger elements" << endmsg;
          break;
        }
  
      m_impl->eventStage( hasTE );
 
      //-----------------
      // Track selection
      //-----------------

      // Create collections of selected tracks
      // TrigInDetTrackCollection mySelectedTrackCollection( SG::VIEW_ELEMENTS );
      if (m_doTrigInDetTrack) {
        ConstDataVector<TrigInDetTrackCollection> mySelectedTrackCollection;
        mySelectedTrackCollection.clear( SG::VIEW_ELEMENTS );
        // FIXME: TrigInDetTrackCollection neglects to forward the
        // DataVector<TrigInDetTrack>(SG::OwnershipPolicy) constructor

        // Process all TEs (and in turn all ROIs in the TE, all tracks in the ROI)
        m_impl->processTEs( tes_in, mySelectedTrackCollection );

        //-------------------------
        // Requirements for tracks
        //-------------------------

        // Check for seed tracks (= high-pT tracks)
        if ( m_impl->m_TotalHiPTTracks < 1 )
        {
          if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG << " No seed tracks for vertex" << endmsg;
          break;
        }

        m_impl->eventStage( hasSeedTrack );

        // Check for the total number of available tracks
        if ( m_impl->m_TotalTracksPass < m_impl->m_totalNTrkMin )
        {
          if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG << " Not enough total passed tracks to vertex" << endmsg;
          break;
        }

        m_impl->eventStage( enoughTracks );

        //-----------------------
        // Vertex reconstruction
        //-----------------------

        // Cluster tracks in z around seed track and reconstruct vertices
        m_impl->reconstructVertices( mySelectedTrackCollection, myVertexCollection, mySplitVertexCollections );
      }
    else {
        ConstDataVector<TrackCollection> mySelectedTrackCollection;
        mySelectedTrackCollection.clear( SG::VIEW_ELEMENTS );
        // FIXME: TrigInDetTrackCollection neglects to forward the
        // DataVector<TrigInDetTrack>(SG::OwnershipPolicy) constructor

        // Process all TEs (and in turn all ROIs in the TE, all tracks in the ROI)
        m_impl->processTEs( tes_in, mySelectedTrackCollection );

        //-------------------------
        // Requirements for tracks
        //-------------------------

        // Check for seed tracks (= high-pT tracks)
        if ( m_impl->m_TotalHiPTTracks < 1 )
        {
          if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG << " No seed tracks for vertex" << endmsg;
          break;
        }

        m_impl->eventStage( hasSeedTrack );

        // Check for the total number of available tracks
        if ( m_impl->m_TotalTracksPass < m_impl->m_totalNTrkMin )
        {
          if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG << " Not enough total passed tracks to vertex" << endmsg;
          break;
        }

        m_impl->eventStage( enoughTracks );

        //-----------------------
        // Vertex reconstruction
        //-----------------------

        // Cluster tracks in z around seed track and reconstruct vertices
        m_impl->reconstructVertices( mySelectedTrackCollection, myVertexCollection, mySplitVertexCollections );
      }
    } while (false);

  //------------------------------------
  // Store output (if any) in the event
  //------------------------------------

  // Add output TEs and attach vertex collections as features if requested
  // JKirk 7/7/16 Need to attach to input TEs
  m_impl->createOutputTEs( myVertexCollection, mySplitVertexCollections, type_out , tes_in );

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
  msg() << MSG::INFO << "T2VertexBeamSpot finalize:" << endmsg;

  //typedef std::map<std::string, IMonitoredAlgo::IMonitoredVariable*> MapType;

  if ( msgLvl()<=MSG::DEBUG )
    {
      for ( IMonitoredAlgo::VariablesMap::const_iterator i = variables().begin(); i != variables().end(); ++i )
        {
          msg() << MSG::DEBUG << i->first << " =";
	  //          const IMonitoredVariable* v = i->second;
          for ( unsigned j = 0; j < i->second->size(); ++j )
            msg() << " " << i->second->get(j);
          msg() << endmsg;
        }
    }

  msg() << MSG::INFO << "T2VertexBeamSpot done." << endmsg;

  return HLT::OK;
}

