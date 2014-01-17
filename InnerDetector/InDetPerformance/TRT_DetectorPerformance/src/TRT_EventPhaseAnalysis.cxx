/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_EventPhaseAnalysis.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TRT_DetectorPerformance/TRT_EventPhaseAnalysis.h"

#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "InDetPrepRawData/TRT_DriftCircle.h"
#include "InDetRIO_OnTrack/SCT_ClusterOnTrack.h"
#include "InDetRIO_OnTrack/PixelClusterOnTrack.h"
//#include "TrkParameters/MeasuredPerigee.h"
//#include "TrkParameters/MeasuredAtaStraightLine.h"
#include "TRT_ConditionsData/RtRelation.h"

#include "CommissionEvent/ComTime.h"

#include "TH2F.h"
#include "TF1.h"

//================ Constructor =================================================

InDet::TRT_EventPhaseAnalysis::TRT_EventPhaseAnalysis( const std::string& name
						     , ISvcLocator* pSvcLocator
						     ) : AthAlgorithm(name,pSvcLocator)
						       , m_histSvc("THistSvc", name)
                                                       , m_trtCondDbTool("TRT_CalDbSvc", name)
                                                       , m_tracksplitter("InDet::InDetTrackSplitterTool")
						       , m_eventPhaseTool()
						       , m_tracksCollection(0)
						       , m_trtHelper(0)
						       , m_tracksName("CombinedInDetTracks_CTB")
						       , eventCount(-1)
{
  declareProperty("histogramService", m_histSvc);
  declareProperty("tracksCollection", m_tracksName);
  
  declareProperty("EventPhaseTool", m_eventPhaseTool);


  m_x_trk = 0;
  m_x_top = 0;
  m_x_bot = 0;

  m_y_trk = 0;
  m_y_top = 0;
  m_y_bot = 0;

  m_z_trk = 0;
  m_z_top = 0;
  m_z_bot = 0;

  m_timeDiff_trk = 0;
  m_timeDiff_top = 0;
  m_timeDiff_bot = 0;

  m_dbT0_trk = 0;
  m_dbT0_top = 0;
  m_dbT0_bot = 0; 

}

//================ Destructor =================================================

InDet::TRT_EventPhaseAnalysis::~TRT_EventPhaseAnalysis()
{}


//================ Initialisation =================================================

StatusCode InDet::TRT_EventPhaseAnalysis::initialize()
{
  // Code entered here will be executed once at program start.
  
  if(msgLvl(MSG::INFO)) msg(MSG::INFO) << name() << " initialize()" << endreq;

  /////////////////////////////////////////////////
  // Create TTrees and register them to THistSvc
  /////////////////////////////////////////////////
  StatusCode sc = m_histSvc.retrieve();
  if ( sc.isFailure() )
    {
      msg(MSG::FATAL) << "histogram service not found" << endreq;
      return StatusCode::FAILURE;
    } else 
      if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "retrieved histogramService" << endreq;
  
  m_tree   = new TTree("splitTracks", "splitTracks");

  std::string fullTreeName =  "/AANT/splitTracks" ;
  sc = (m_histSvc->regTree(fullTreeName, m_tree));
  if( sc.isFailure() )
    {
      msg(MSG::ERROR) << "Unable to register TTree : " << fullTreeName << endreq;
      return StatusCode::FAILURE;
    }
  
  sc = m_trtCondDbTool.retrieve() ;
  if (sc.isFailure()) 
    {
      msg(MSG::ERROR) << "Could not find TRTCalDbTool " << endreq;
      return sc;
    }

  //////////////////////////////
  /// Initialize data vectors
  //////////////////////////////
  m_x_trk = new std::vector<float>;
  m_x_top = new std::vector<float>;
  m_x_bot = new std::vector<float>;

  m_y_trk = new std::vector<float>;
  m_y_top = new std::vector<float>;
  m_y_bot = new std::vector<float>;
  
  m_z_trk = new std::vector<float>;
  m_z_top = new std::vector<float>;
  m_z_bot = new std::vector<float>;
  
  m_timeDiff_trk = new std::vector<float>;
  m_timeDiff_top = new std::vector<float>;
  m_timeDiff_bot = new std::vector<float>;

  m_dbT0_trk = new std::vector<float>;
  m_dbT0_top = new std::vector<float>;
  m_dbT0_bot = new std::vector<float>;

  //////////////////////////////////////////////////////
  /// Book variables to be added to the output ntuple
  //////////////////////////////////////////////////////
  m_tree->Branch("nTrtBarrelHits_trk", &m_nTrtBarrelHits_trk, "nTrtBarrelHits_trk/I");
  m_tree->Branch("nTrtBarrelHits_top", &m_nTrtBarrelHits_top, "nTrtBarrelHits_top/I");
  m_tree->Branch("nTrtBarrelHits_bot", &m_nTrtBarrelHits_bot, "nTrtBarrelHits_bot/I");

  m_tree->Branch("nTrtEndcapHits_trk", &m_nTrtEndcapHits_trk, "nTrtEndcapHits_trk/I");
  m_tree->Branch("nTrtEndcapHits_top", &m_nTrtEndcapHits_top, "nTrtEndcapHits_top/I");
  m_tree->Branch("nTrtEndcapHits_bot", &m_nTrtEndcapHits_bot, "nTrtEndcapHits_bot/I");

  m_tree->Branch("nSctHits_trk", &m_nSctHits_trk, "nSctHits_trk/I");
  m_tree->Branch("nSctHits_top", &m_nSctHits_top, "nSctHits_top/I");
  m_tree->Branch("nSctHits_bot", &m_nSctHits_bot, "nSctHits_bot/I");

  m_tree->Branch("nPixelHits_trk", &m_nPixelHits_trk, "nPixelHits_trk/I");
  m_tree->Branch("nPixelHits_top", &m_nPixelHits_top, "nPixelHits_top/I");
  m_tree->Branch("nPixelHits_bot", &m_nPixelHits_bot, "nPixelHits_bot/I");

  m_tree->Branch("EP_trk", &m_EP_trk, "EP_trk/D");
  m_tree->Branch("EP_top", &m_EP_top, "EP_top/D");
  m_tree->Branch("EP_bot", &m_EP_bot, "EP_bot/D");

  m_tree->Branch("EPFromTE_trk", &m_EPFromTE_trk, "EPFromTE_trk/D");
  m_tree->Branch("EPFromTE_top", &m_EPFromTE_top, "EPFromTE_top/D");
  m_tree->Branch("EPFromTE_bot", &m_EPFromTE_bot, "EPFromTE_bot/D");

  m_tree->Branch("tofFitPar0_trk", &m_tofFitPar0_trk, "tofFitPar0_trk/F");
  m_tree->Branch("tofFitPar0_top", &m_tofFitPar0_top, "tofFitPar0_top/F");
  m_tree->Branch("tofFitPar0_bot", &m_tofFitPar0_bot, "tofFitPar0_bot/F");

  m_tree->Branch("tofFitPar1_trk", &m_tofFitPar1_trk, "tofFitPar1_trk/F");
  m_tree->Branch("tofFitPar1_top", &m_tofFitPar1_top, "tofFitPar1_top/F");
  m_tree->Branch("tofFitPar1_bot", &m_tofFitPar1_bot, "tofFitPar1_bot/F");

  m_tree->Branch("tofChi2OverNdf_trk", &m_tofChi2OverNdf_trk, "tofChi2OverNdf_trk/F");
  m_tree->Branch("tofChi2OverNdf_top", &m_tofChi2OverNdf_top, "tofChi2OverNdf_top/F");
  m_tree->Branch("tofChi2OverNdf_bot", &m_tofChi2OverNdf_bot, "tofChi2OverNdf_bot/F");

  m_tree->Branch("trtPhase", &m_trtPhase, "trtPhase/D");
  m_tree->Branch("d0",       &m_d0,    "d0/D");
  m_tree->Branch("z0",       &m_z0,    "z0/D");
  m_tree->Branch("phi",      &m_phi,   "phi/D");
  m_tree->Branch("theta",    &m_theta, "theta/D");

  m_tree->Branch("x_trk", &m_x_trk);
  m_tree->Branch("x_top", &m_x_top);
  m_tree->Branch("x_bot", &m_x_bot);

  m_tree->Branch("y_trk", &m_y_trk);
  m_tree->Branch("y_top", &m_y_top);
  m_tree->Branch("y_bot", &m_y_bot);

  m_tree->Branch("z_trk", &m_z_trk);
  m_tree->Branch("z_top", &m_z_top);
  m_tree->Branch("z_bot", &m_z_bot);

  m_tree->Branch("timeDiff_trk", &m_timeDiff_trk);
  m_tree->Branch("timeDiff_top", &m_timeDiff_top);
  m_tree->Branch("timeDiff_bot", &m_timeDiff_bot);

  m_tree->Branch("dbT0_trk", &m_dbT0_trk);
  m_tree->Branch("dbT0_top", &m_dbT0_top);
  m_tree->Branch("dbT0_bot", &m_dbT0_bot);

  if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "initialize() successful in " << name() << endreq;
  return StatusCode::SUCCESS;
}

//============== BeginRun =======================================================
StatusCode InDet::TRT_EventPhaseAnalysis::beginRun()
{
  m_eventPhaseTool->beginRun();
  
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode InDet::TRT_EventPhaseAnalysis::finalize()
{
  // Code entered here will be executed once at the end of the program run.
  
  if(m_x_trk) delete m_x_trk;
  if(m_x_top) delete m_x_top;
  if(m_x_bot) delete m_x_bot;

  if(m_y_trk) delete m_y_trk;
  if(m_y_top) delete m_y_top;
  if(m_y_bot) delete m_y_bot;

  if(m_z_trk) delete m_z_trk;
  if(m_z_top) delete m_z_top;
  if(m_z_bot) delete m_z_bot;

  if(m_timeDiff_trk) delete m_timeDiff_trk;
  if(m_timeDiff_top) delete m_timeDiff_top;
  if(m_timeDiff_bot) delete m_timeDiff_bot;

  if(m_dbT0_trk) delete m_dbT0_trk;
  if(m_dbT0_top) delete m_dbT0_top;
  if(m_dbT0_bot) delete m_dbT0_bot;
  
  return StatusCode::SUCCESS;
}

//================ Execution ====================================================

StatusCode InDet::TRT_EventPhaseAnalysis::execute()
{
  ++eventCount;
  
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "InDet::TRT_DetectorPlots::execute() event " << eventCount << endreq;

  //////////////////////////
  /// Retrieve event info
  //////////////////////////
  StatusCode sc = load();
  
  if (sc.isFailure())
    {
      msg(MSG::ERROR) << "InDet::TRT_DetectorPlots::load() failed, exiting" << endreq;
      return sc;
    }	
  
  if (m_tracksCollection->size() > 100)
    {
      if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "N tracks greater than 100: " << m_tracksCollection->size() << " for event " << eventCount << ", exiting" << endreq; 
      return sc;
    }
  
  for ( DataVector<Trk::Track>::const_iterator trackIt = m_tracksCollection->begin(); trackIt != m_tracksCollection->end(); ++trackIt ) 
    {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Track loop start" << endreq;

      //////////////////
      /// split tracks
      //////////////////
      std::pair<Trk::Track*, Trk::Track*> splitTracks =  std::pair<Trk::Track*, Trk::Track*> (0,0);
      splitTracks = m_tracksplitter->splitInUpperLowerTrack( **trackIt );
      
      /// Is there a top track?
      if( splitTracks.first == 0 ) 
	{
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No top track. Skipping track" << endreq;
	  
	  // no top track so delete bottom track if it exists 
	  if(splitTracks.second) delete splitTracks.second;
	  continue;
	}
      // is there a bottom track?
      if( splitTracks.second == 0 )
	{
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "No bottom track, Skipping track" << endreq;
	  
	  // no bottom track so delete top track (we know it exists because we got here)
	  delete splitTracks.first;
	  continue;
	}

      const ComTime* comtime = 0;
      evtStore()->retrieve(comtime,"TRT_Phase");

      m_trtPhase = comtime->getTime();
      m_d0    = (**trackIt).perigeeParameters()->parameters()[Trk::d0];
      m_z0    = (**trackIt).perigeeParameters()->parameters()[Trk::z0];
      m_phi   = (**trackIt).perigeeParameters()->parameters()[Trk::phi];
      m_theta = (**trackIt).perigeeParameters()->parameters()[Trk::theta];
      
      
      bool analysisFailed = false;
      if( !analyzeTrack( (*trackIt)
		       , m_nTrtBarrelHits_trk
		       , m_nTrtEndcapHits_trk
		       , m_nSctHits_trk
		       , m_nPixelHits_trk
		       , m_EP_trk
		       , m_EPFromTE_trk
		       , m_tofFitPar0_trk
		       , m_tofFitPar1_trk
		       , m_tofChi2OverNdf_trk
		       , m_timeDiff_trk
		       , m_x_trk
		       , m_y_trk
		       , m_z_trk
		       , m_dbT0_trk
		       ) ||
	  !analyzeTrack( splitTracks.first
		       , m_nTrtBarrelHits_top
		       , m_nTrtEndcapHits_top
		       , m_nSctHits_top
		       , m_nPixelHits_top
		       , m_EP_top
		       , m_EPFromTE_top
		       , m_tofFitPar0_top
		       , m_tofFitPar1_top
		       , m_tofChi2OverNdf_top
		       , m_timeDiff_top
		       , m_x_top
		       , m_y_top
		       , m_z_top
		       , m_dbT0_top
		       ) ||
	  !analyzeTrack( splitTracks.second
		       , m_nTrtBarrelHits_bot
		       , m_nTrtEndcapHits_bot
		       , m_nSctHits_bot
		       , m_nPixelHits_bot
		       , m_EP_bot
		       , m_EPFromTE_bot
		       , m_tofFitPar0_bot
		       , m_tofFitPar1_bot
		       , m_tofChi2OverNdf_bot
		       , m_timeDiff_bot
		       , m_x_bot
		       , m_y_bot
		       , m_z_bot
		       , m_dbT0_bot
		       ) 
	  )
	{
	  if(msgLvl(MSG::INFO)) msg(MSG::INFO) << "analysis failed. skipping track." << endreq;
	  analysisFailed = true;
	}
      
      
      if(!analysisFailed)
	m_tree->Fill();
      
      delete splitTracks.first;
      delete splitTracks.second;
    }

  return StatusCode::SUCCESS;
}

//================ Load =========================================================

StatusCode InDet::TRT_EventPhaseAnalysis::load()
{
  StatusCode sc = evtStore()->retrieve( m_tracksCollection, m_tracksName );
  if ( sc.isFailure() )
    {
      msg(MSG::ERROR) << "Could not find Tracks Collectioni " << m_tracksName << endreq;
      return sc;  
    }
  
  sc = detStore()->retrieve(m_trtHelper, "TRT_ID");
  if ( sc.isFailure() )
    {
      msg(MSG::FATAL) << "Could not get TRT ID helper" << endreq;
      return StatusCode::FAILURE;
    }
  
  return StatusCode::SUCCESS;
}

//================ Load =========================================================

bool InDet::TRT_EventPhaseAnalysis::analyzeTrack( Trk::Track* track
						, int& nTrtBarrelHits
						, int& nTrtEndcapHits
						, int& nSctHits
						, int& nPixelHits
						, double& EP
						, double& EPFromTE
						, float& tofFitPar0
						, float& tofFitPar1
						, float& tofChi2OverNdf
						, std::vector<float>* timeDiff
						, std::vector<float>* x
						, std::vector<float>* y
						, std::vector<float>* z
						, std::vector<float>* dbT0
						)
{
  const DataVector<const Trk::TrackStateOnSurface>* trackStates= track->trackStateOnSurfaces();
  if ( trackStates == 0 ) 
    {
      msg(MSG::ERROR) << "Trk::TrackStateOnSurface empty" << endreq;
      return false;
    }
  
  nTrtBarrelHits = 0;
  nTrtEndcapHits = 0;
  nSctHits       = 0;
  nPixelHits     = 0;
  
  EP       = m_eventPhaseTool->findPhase(track);
  EPFromTE = m_eventPhaseTool->findPhaseFromTE(track);
  
  
  x->clear();
  y->clear();
  z->clear();

  timeDiff->clear();
  
  dbT0->clear();

  TH2F* timeDifferenceVsY = new TH2F("timeDifferenceVsY", "Time Difference vs Y", 8000, -1000, 1000, 400, -25, 75);
  
  /////////////////////////////
  /// loop over track states 
  /////////////////////////////
  for ( DataVector<const Trk::TrackStateOnSurface>::const_iterator trackStatesIt = trackStates->begin(); 
	trackStatesIt != trackStates->end(); ++trackStatesIt ) 
    {
      // is there a track state?
      if ( *trackStatesIt == 0 )
	{
	  msg(MSG::ERROR) << "*trackStatesIt == 0" << endreq; 
	  continue;
	}
	  
      // is there a measurement on that track state? 
      if ( !((*trackStatesIt)->type(Trk::TrackStateOnSurface::Measurement)) ) 
	continue;
      
      // is this track state an SCT hit?
      if (dynamic_cast<const InDet::SCT_ClusterOnTrack*>((*trackStatesIt)->measurementOnTrack())!=0) 
	{
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "SCT hit on track." << endreq;
	  ++nSctHits;
	  continue;
	}

      // is this track state a pixel hit?
      if ( dynamic_cast<const InDet::PixelClusterOnTrack*>((*trackStatesIt)->measurementOnTrack()) ) // pixel hit 
	{
	  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Pixel hit on track." << endreq;
	  ++nPixelHits;
	  continue;
	}

      const InDet::TRT_DriftCircleOnTrack *driftCircleOnTrack = dynamic_cast<const InDet::TRT_DriftCircleOnTrack *>( (*trackStatesIt)->measurementOnTrack() );

      // is there a drift circle on this track state? 
      if ( driftCircleOnTrack == 0 ) 
	{
	  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "driftCircleOnTrack == 0 " << ( ((*trackStatesIt)->type(Trk::TrackStateOnSurface::Outlier)) ? "Outlier" : "." )  <<  endreq;
	  continue; 
	}
	  
      // is there a measurement base?
      if ( (*trackStatesIt)->measurementOnTrack() == 0 ) 
	{
	  msg(MSG::ERROR) << "measurementBase == 0, and we should have had it!!!" << endreq;
	  continue;
	}
      
      // is it possible to prep raw data for this drift circle?
      const InDet::TRT_DriftCircle *driftCircle = driftCircleOnTrack->prepRawData();
      //delete driftCircleOnTrack;
      if ( driftCircle == 0 ) 
	{
	  msg(MSG::ERROR) << "driftCircle == 0" << endreq; 
	  continue;
	}
	  
      Identifier id = driftCircle->identify();

      // Count barrel hits or ec hits
      if (abs( m_trtHelper->barrel_ec(id ) ) == 1) 
	nTrtBarrelHits++;
      else if (abs( m_trtHelper->barrel_ec(id) ) == 2) 
	nTrtEndcapHits++;
      
      //const CLHEP::Hep3Vector &center = driftCircle->detectorElement()->surface( id ).center();
      const Amg::Vector3D  center = driftCircle->detectorElement()->surface( id ).center(); 
      
      x->push_back(center.x());
      y->push_back(center.y());
      z->push_back(center.z());

      //const Trk::MeasuredAtaStraightLine *trackMeasuredAtLine = dynamic_cast<const Trk::MeasuredAtaStraightLine*>( (*trackStatesIt)->trackParameters() );
      
      const TRTCond::RtRelation *rtr = m_trtCondDbTool->getRtRelation(id);
      
      double leadingEdge  = 3.125 * driftCircle->driftTimeBin();
      double theDbT0      = m_trtCondDbTool->getT0(id, TRTCond::ExpandedIdentifier::STRAW);
      //double estDriftTime = rtr->drifttime( fabs(trackMeasuredAtLine->parameters()[Trk::locR] ) );
      double estDriftTime = rtr->drifttime( fabs(((*trackStatesIt)->trackParameters())->parameters()[Trk::locR] ) );

      timeDifferenceVsY->Fill(center.y()/1000, leadingEdge - theDbT0 - estDriftTime); // y in meters

      timeDiff->push_back(leadingEdge - theDbT0 - estDriftTime);
      dbT0->push_back(theDbT0);
    }


  if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "entries: " << timeDifferenceVsY->GetEntries() << endreq;
  
  timeDifferenceVsY->Fit("pol1", "Q");
  if(timeDifferenceVsY->GetFunction("pol1"))
    {
      if(msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "constant: " << timeDifferenceVsY->GetFunction("pol1")->GetParameter(0) << " -- splope: " << timeDifferenceVsY->GetFunction("pol1")->GetParameter(1) << endreq;
      
      tofFitPar0     = timeDifferenceVsY->GetFunction("pol1")->GetParameter(0);
      tofFitPar1     = timeDifferenceVsY->GetFunction("pol1")->GetParameter(1);
      tofChi2OverNdf = timeDifferenceVsY->GetFunction("pol1")->GetChisquare() / timeDifferenceVsY->GetFunction("pol1")->GetNDF();
    }
  else
    {
      tofFitPar0 = 0;
      tofFitPar1 = 0; 
      tofChi2OverNdf = -999;
    }
  
  delete timeDifferenceVsY;

  return true;
}

//============================================================================================
