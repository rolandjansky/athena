/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================
//
// $Id: T2VertexBeamSpot.cxx 577881 2014-01-10 20:43:51Z masik $
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
#include "version.h"

// General ATHENA/Trigger stuff
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "TrigNavigation/TriggerElement.h"
#include "CLHEP/Units/SystemOfUnits.h"
using CLHEP::mm;
using CLHEP::GeV;

// Specific to this algorithm
#include "TrigInDetEvent/TrigInDetTrack.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"
#include "TrigInDetEvent/TrigInDetTrackHelper.h"
#include "TrigInDetEvent/TrigVertexCollection.h"

// Related to TOOLS
#include "TrigVertexFitter/ITrigPrimaryVertexFitter.h"
#include "InDetBeamSpotService/IBeamCondSvc.h"

// Timer service
#include "TrigTimeAlgs/TrigTimerSvc.h"

// Standard headers
#include <cmath>
using std::abs;
// #include <sstream>
// using std::ostringstream;

using std::vector;
using std::string;

using namespace PESA;


//-------------
// Constructor
//-------------

T2VertexBeamSpot::T2VertexBeamSpot( const string& name, ISvcLocator* pSvcLocator )
  : HLT::AllTEAlgo(name, pSvcLocator)
  , d( new T2VertexBeamSpotImpl( this ) )
{
   
  // Beam Conditions Service usage
  declareProperty("BeamCondSvcName", d->m_beamCondSvcName = "BeamCondSvc" );
   
  // Read cuts 
  declareProperty("AcceptAll",       d->m_AcceptAll    = false );
  declareProperty("TotalNTrackMin",  d->m_totalNTrkMin = 2 );
  
  // Detail for plots
  declareProperty("HistoTrackAlgs",  d->m_saveTrackAlgs   = false );
  declareProperty("SaveAllVertices", d->m_saveAllVertices = false );
  
  // Track clustering parameters
  declareProperty("TrackSeedPt",       d->m_trackSeedPt       =  1.0*GeV );
  declareProperty("TrackClusterDZ",    d->m_trackClusDZ       = 10.0*mm  );
  declareProperty("WeightClusterZ",    d->m_weightSeed        = true     );
  declareProperty("ReclusterSplit",    d->m_reclusterSplit    = true     );
  declareProperty("SplitWholeCluster", d->m_splitWholeCluster = false    );
  
  // Track Selection criteria
  declareProperty("TrackAlgoID",     d->m_TrackAlgoId      = TrigInDetTrack::IDSCANID);
  declareProperty("TrackMinPt",      d->m_minTrackPt       =  0.7*GeV );
  declareProperty("TrackMaxPt",      d->m_maxTrackPt       = 200.*GeV );
  declareProperty("TrackMinEta",     d->m_minTrackEta      =   0.     );
  declareProperty("TrackMaxEta",     d->m_maxTrackEta      =   2.5    );
  declareProperty("TrackMaxZ0",      d->m_maxTrackZ0       = 200.*mm  );
  declareProperty("TrackMaxD0",      d->m_maxTrackD0       =  10.*mm  );
  declareProperty("TrackMaxZ0err",   d->m_maxTrackZ0err    =   2.*mm  );
  declareProperty("TrackMaxD0err",   d->m_maxTrackD0err    =   2.*mm  );
  declareProperty("TrackMinNDF",     d->m_minTrackNDF      =   2      );
  declareProperty("TrackMinQual",    d->m_minTrackQual     =   0.2    );
  declareProperty("TrackMaxQual",    d->m_maxTrackQual     =   5.0    );
  declareProperty("TrackMinChi2Prob",d->m_minTrackChi2Prob = -10.0    );
  declareProperty("TrackMinSiHits",  d->m_minSiHits        =   4      );
  declareProperty("TrackMinPIXHits", d->m_minPIXHits       =   1      );
  declareProperty("TrackMinSCTHits", d->m_minSCTHits       =   3      );
  declareProperty("TrackMinTRTHits", d->m_minTRTHits       =   1      );
  
  // Vertex Selection criteria
  declareProperty("VertexMinNTrk",     d->m_vtxNTrkMin     =   2      );
  declareProperty("VertexMaxNTrk",     d->m_vtxNTrkMax     = 100      );
  declareProperty("VertexMinQual",     d->m_vtxQualMin     =   0.0    );
  declareProperty("VertexMaxQual",     d->m_vtxQualMax     =   5.     );
  declareProperty("VertexMinChi2Prob", d->m_vtxChi2ProbMin = -10.0    );
  declareProperty("VertexMinMass",     d->m_vtxMassMin     =  -1.*GeV );
  declareProperty("VertexMinSumPt",    d->m_vtxSumPtMin    =   0.*GeV );
  declareProperty("VertexMaxX",        d->m_vtxXposMax     =  10.*mm  );
  declareProperty("VertexMaxXerr",     d->m_vtxXerrMax     =   1.*mm  );
  declareProperty("VertexMaxY",        d->m_vtxYposMax     =  10.*mm  );
  declareProperty("VertexMaxYerr",     d->m_vtxYerrMax     =   1.*mm  );
  declareProperty("VertexMaxZ",        d->m_vtxZposMax     = 200.*mm  );
  declareProperty("VertexMaxZerr",     d->m_vtxZerrMax     =   1.*mm  );

  // Vertex selection for use in BCID measurements
  declareProperty("VertexBCIDMinNTrk", d->m_vtxBCIDNTrkMin =   2      );

  // Vertexing
  declareProperty("doVertexFit",       d->m_doVertexFit = true);
  
  //-----------------------
  // activate TE  
  //-----------------------
  declareProperty("activateTE",          d->m_activateTE          = false               );
  declareProperty("activateAllTE",       d->m_activateAllTE       = false               );
  declareProperty("attachVertices",      d->m_attachVertices      = false               );
  declareProperty("attachSplitVertices", d->m_attachSplitVertices = false               );
  declareProperty("nSplitVertices",      d->m_nSplitVertices      = 1                   );           
  declareProperty("vertexCollName",      d->m_vertexCollName      = "TrigBeamSpotVertex"); 
 
  //---------------------------
  // Single interaction trigger
  //---------------------------
  declareProperty("minNpvTrigger",  d->m_minNpvTrigger  = 0);
  declareProperty("maxNpvTrigger",  d->m_maxNpvTrigger  = 2);
  declareProperty("activateSI",     d->m_activateSI     = false);
  
  // Number of Z blocks
  declareProperty( "NumberZClusters", d->m_nZfitBins       = 10 );
  declareProperty( "ZLowerLimit",     d->m_ZLowerLimit     = -200.*mm );
  declareProperty( "ZUpperLimit",     d->m_ZUpperLimit     =  200.*mm );
  
  //-----------------------
  // Monitoring
  //-----------------------

  // Variables for monitoring histograms
  const IMonitoredAlgo::ContainerReset reset = IMonitoredAlgo::AutoClear;

  // per-Job variables, which need to be reset
  declareMonitoredVariable("TotalEvents" ,        d->m_TotalEvents);

  // per-Event variables, which need to be reset
  declareMonitoredVariable("RunID" ,              d->m_RunID);
  declareMonitoredVariable("EventID" ,            d->m_EventID);
  declareMonitoredVariable("LumiBlock",           d->m_LumiBlock);
  declareMonitoredVariable("BCID",                d->m_bcid);
                                               
  declareMonitoredVariable("TotalTEs",            d->m_TotalTEs);
  declareMonitoredVariable("TotalROIs",           d->m_TotalROIs);
  declareMonitoredVariable("TotalTracks" ,        d->m_TotalTracks);
  declareMonitoredVariable("TotalTracksPass",     d->m_TotalTracksPass);
  declareMonitoredVariable("TotalHiPTTracks",     d->m_TotalTracksPass);
  
  declareMonitoredVariable("NClusters",           d->m_NClusters);
  declareMonitoredVariable("Nvtx",                d->m_Nvtx);
  declareMonitoredVariable("NvtxPass",            d->m_NvtxPass);
  declareMonitoredVariable("NvtxPassBCID",        d->m_NvtxPassBCID);

  // Containers of per per-Event, per-TE, per-ROI variables
  declareMonitoredStdContainer("EventStatistics", d->m_eventStage, reset );
  
  declareMonitoredStdContainer("ROIperTE",        d->m_ROIperTE        , reset );
  declareMonitoredStdContainer("TracksPerTE",     d->m_TracksPerTE     , reset );
  declareMonitoredStdContainer("TracksPerTEPass", d->m_TracksPerTEPass , reset );
  declareMonitoredStdContainer("HiPTTracksPerTE", d->m_HiPTTracksPerTE , reset );

  declareMonitoredStdContainer("TracksPerROI",    d->m_TracksPerROI    , reset );
  declareMonitoredStdContainer("TracksPerROIPass",d->m_TracksPerROIPass, reset );
  declareMonitoredStdContainer("HiPTTracksPerROI",d->m_HiPTTracksPerROI, reset );

  declareMonitoredStdContainer("RoIEta",          d->m_RoIEta, reset );
  declareMonitoredStdContainer("RoIPhi",          d->m_RoIPhi, reset );
  declareMonitoredStdContainer("RoIID",           d->m_RoIID , reset );
  
  declareMonitoredStdContainer("ClusterNTracks",       d->m_clusterNTracks      , reset );
  declareMonitoredStdContainer("ClusterNTracksUnused", d->m_clusterNTracksUnused, reset );
  declareMonitoredStdContainer("ClusterZ",             d->m_clusterZ            , reset );
  declareMonitoredStdContainer("ClusterDeltaZ0",       d->m_clusterDeltaZ0      , reset );
  declareMonitoredStdContainer("ClusterZ0Pull",        d->m_clusterZ0Pull       , reset );
  declareMonitoredStdContainer("ClusterDeltaVertexZ",  d->m_clusterDeltaVertexZ , reset );
  declareMonitoredStdContainer("ClusterVertexZPull",   d->m_clusterVertexZPull  , reset );

  d->m_L2StarB        .declare( this, "L2StarB"                  );
  d->m_L2StarA        .declare( this, "L2StarA"                  );
  d->m_L2StarF        .declare( this, "L2StarF"                  );
  d->m_SiTrack        .declare( this, "SiTrack"                  );
  d->m_IdScan         .declare( this, "IdScan"                   );

  d->m_trackPass      .declare( this, "Track"       , "Pass"     );
  d->m_vertex         .declare( this, "Vertex"                   );
  d->m_vertexPass     .declare( this, "Vertex"      , "Pass"     );   
  d->m_vertexPassBCID .declare( this, "Vertex"      , "PassBCID" );   
  d->m_splitVertexPass.declare( this, "SplitVertex" , "Pass"     );
}


//------------
// Destructor
//------------

T2VertexBeamSpot::~T2VertexBeamSpot()
{
  delete d;
}


//----------------
// Initialization
//----------------

HLT::ErrorCode
T2VertexBeamSpot::hltInitialize()
{
  StatusCode sc;
  msg() << MSG::INFO << "Initializing T2VertexBeamSpot. Version: " << TrigT2BeamSpot_version()() << endreq;
  
  // Vertex fitting tool
  sc = toolSvc()->retrieveTool( "TrigPrimaryVertexFitter", "TrigPrimaryVertexFitter", d->m_primaryVertexFitter );
  if ( sc.isFailure() || ! d->m_primaryVertexFitter )
    { 
      msg() << MSG::ERROR << "Unable to locate TrigPrimaryVertexFitter tool." << endreq;
      return HLT::BAD_JOB_SETUP;
    }

  // Pick up the beam conditions service
  sc = service( d->m_beamCondSvcName, d->m_beamCondSvc );
  if ( sc.isFailure() || ! d->m_beamCondSvc )
    {
      msg() << MSG::ERROR << "Failed to retrieve BeamCondSvc: " <<  d->m_beamCondSvcName << endreq;
      return HLT::BAD_JOB_SETUP;
    }

  // Set up execution timers
  {
    //      d->m_timer.resize(numTimers);
    d->m_timer[total         ] = addTimer("TotalAlgoTime"     );
    d->m_timer[allTE         ] = addTimer("LoopOverInputTEs"  );
    d->m_timer[allROI        ] = addTimer("LoopOverROIs"      );
    d->m_timer[allTrack      ] = addTimer("LoopOverTracks"    );
    d->m_timer[allVertex     ] = addTimer("Vertexing"         );
    d->m_timer[allVertexSplit] = addTimer("SplitVertexing"    );
    d->m_timer[allOutput     ] = addTimer("OutputTEWriting"   );
    d->m_timer[trackSort     ] = addTimer("TrackSorting"      );
    d->m_timer[trackSortSplit] = addTimer("TrackSortingSplit" );
    d->m_timer[vertexFit     ] = addTimer("VertexFit"         );
    d->m_timer[vertexFitSplit] = addTimer("VertexFitSplit"    );
    d->m_timer[zCluster      ] = addTimer("ZClustering"       );
    d->m_timer[zClusterSplit ] = addTimer("ZClusteringSplit"  );
  }

  // Set up the track z clustering tool
  d->m_trackClusterer.reset( new T2TrackClusterer( d->m_trackClusDZ, d->m_trackSeedPt, d->m_weightSeed, d->m_vtxNTrkMax ) );

  // Reset per-Job monitored variables
  d->m_TotalEvents = 0;

  msg() << MSG::INFO << "Initialization of T2VertexBeamSpot successful" << endreq;
  return HLT::OK;
}


//------------
// Event loop
//------------

HLT::ErrorCode
T2VertexBeamSpot::hltExecute( vector<vector<HLT::TriggerElement*> >& tes_in,
                              unsigned int type_out )
{
  // Say hello
  if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG << "In T2VertexBeamSpot::hltExecute" << endreq;  

  // Start monitoring
  beforeExecMonitors().ignore();
  
  // Start the overall timer
  T2Timer t( d->m_timer[total] );
  
  // Initialize booleans for event stats
  d->m_eventStageFlag = vector<bool>( numStatistics, false );

  // Reset monitored vars that are not reset by the steering
  d->resetMonitoredVariables();
  // FIXME: this should be done by IMonitoredAlgo

  d->m_TotalEvents++;
  d->eventStage( allInput );

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

      d->m_RunID     = pEventInfo->event_ID()->run_number();
      d->m_EventID   = pEventInfo->event_ID()->event_number();
      d->m_LumiBlock = pEventInfo->event_ID()->lumi_block();
      d->m_bcid      = pEventInfo->event_ID()->bunch_crossing_id();

      if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG << "Run #" << d->m_RunID << "; Event #" << d->m_EventID
                                      << "; Lumi block #" << d->m_LumiBlock << "; BCID #" << d->m_bcid << endreq;

      // Count input TEs
      if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG << "Number of input TEs = " << tes_in.size() << endreq;
      d->m_TotalTEs = tes_in.size();

      // Check for input TEs
      if ( tes_in.empty() )
        {
          if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG << " No trigger elements" << endreq;
          break;
        }
  
      d->eventStage( hasTE );
 
      //-----------------
      // Track selection
      //-----------------

      // Create collections of selected tracks
      // TrigInDetTrackCollection mySelectedTrackCollection( SG::VIEW_ELEMENTS );
      TrigInDetTrackCollection mySelectedTrackCollection;
      mySelectedTrackCollection.clear( SG::VIEW_ELEMENTS );
      // FIXME: TrigInDetTrackCollection neglects to forward the
      // DataVector<TrigInDetTrack>(SG::OwnershipPolicy) constructor

      // Process all TEs (and in turn all ROIs in the TE, all tracks in the ROI)
      d->processTEs( tes_in, mySelectedTrackCollection );
  
      //-------------------------
      // Requirements for tracks
      //-------------------------

      // Check for seed tracks (= high-pT tracks)
      if ( d->m_TotalHiPTTracks < 1 )
        {
          if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG << " No seed tracks for vertex" << endreq;
          break;
        }

      d->eventStage( hasSeedTrack );

      // Check for the total number of available tracks
      if ( d->m_TotalTracksPass < d->m_totalNTrkMin )
        {
          if (msgLvl()<=MSG::DEBUG) msg() << MSG::DEBUG << " Not enough total passed tracks to vertex" << endreq;
          break;
        }
  
      d->eventStage( enoughTracks );
  
      //-----------------------
      // Vertex reconstruction
      //-----------------------

      // Cluster tracks in z around seed track and reconstruct vertices
      d->reconstructVertices( mySelectedTrackCollection, myVertexCollection, mySplitVertexCollections );

    } while (false);

  //------------------------------------
  // Store output (if any) in the event
  //------------------------------------

  // Add output TEs and attach vertex collections as features if requested
  d->createOutputTEs( myVertexCollection, mySplitVertexCollections, type_out );

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

  // FIXME: This needs to become DEBUG before tagging
  if ( msgLvl()<=MSG::DEBUG )
    {
      for ( IMonitoredAlgo::VariablesMap::const_iterator i = variables().begin(); i != variables().end(); ++i )
        {
          // FIXME: This needs to become DEBUG before tagging
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


bool T2VertexBeamSpot::reset()
{
  return true;
}
