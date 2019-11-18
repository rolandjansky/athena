/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SimpleAmbiguityProcessorTool.h"
#include "TrackScoringTool.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "TrkTrack/TrackCollection.h"
#include "GaudiKernel/MsgStream.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrkTrack/TrackInfo.h"
#include <map>
#include <ext/functional>
#include <iterator>

//==================================================================================================
Trk::SimpleAmbiguityProcessorTool::SimpleAmbiguityProcessorTool(const std::string& t, 
								const std::string& n,
								const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_particleHypothesis{undefined},
  m_scoringTool("Trk::TrackScoringTool/TrackScoringTool"), 
  m_fitterTool ("Trk::KalmanFitter/InDetTrackFitter"), 
  m_selectionTool("InDet::InDetAmbiTrackSelectionTool/InDetAmbiTrackSelectionTool"),
  m_Nevents(0),
  m_Ncandidates(5), m_NcandScoreZero(5), m_NcandDouble(5),
  m_NscoreOk(5),m_NscoreZeroBremRefit(5),m_NscoreZeroBremRefitFailed(5),
  m_NscoreZeroBremRefitScoreZero(5),m_NscoreZero(5),
  m_Naccepted(5),m_NsubTrack(5),m_NnoSubTrack(5),m_NacceptedBrem(5),
  m_NbremFits(5),m_Nfits(5),m_NrecoveryBremFits(5),m_NgoodFits(5),m_NfailedFits(5)
{
  // statitics stuff
  m_etabounds.push_back(0.8);
  m_etabounds.push_back(1.6);
  m_etabounds.push_back(2.5);
  m_etabounds.push_back(2.5);
  m_etabounds.push_back(10.0);

  declareInterface<ITrackAmbiguityProcessorTool>(this);
  declareProperty("DropDouble"           , m_dropDouble         = true);
  declareProperty("ForceRefit"           , m_forceRefit         = true);
  declareProperty("RefitPrds"            , m_refitPrds          = false);
  declareProperty("MatEffects"           , m_matEffects         = 3); // pion
  declareProperty("ScoringTool"          , m_scoringTool);
  declareProperty("SelectionTool"        , m_selectionTool);
  declareProperty("Fitter"               , m_fitterTool );
  declareProperty("SuppressHoleSearch"   , m_suppressHoleSearch = false);
  declareProperty("SuppressTrackFit"     , m_suppressTrackFit   = false);
  declareProperty("tryBremFit"           , m_tryBremFit         = false);
  declareProperty("caloSeededBrem"       , m_caloSeededBrem     = false);
  declareProperty("pTminBrem"            , m_pTminBrem          = 1000.);
  declareProperty("etaBounds"            , m_etabounds,"eta intervals for internal monitoring");

}
//==================================================================================================

Trk::SimpleAmbiguityProcessorTool::~SimpleAmbiguityProcessorTool()
{
}
//==================================================================================================

StatusCode Trk::SimpleAmbiguityProcessorTool::initialize()
{

  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) 
    {
      msg(MSG::FATAL) << "AlgTool::initialise failed" << endmsg;
      return StatusCode::FAILURE;
    }

  // the association tool can be disabled if for this processor instance a PRD-to-track map is provided by the caller.
  ATH_CHECK( m_assoTool.retrieve() );
  ATH_CHECK( m_trackSummaryTool.retrieve( DisableTool{ m_trackSummaryTool.name().empty() } ) );

  sc = m_scoringTool.retrieve();
  if (sc.isFailure()) 
    {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_scoringTool << endmsg;
      return StatusCode::FAILURE;
    } 
  else 
    msg(MSG::INFO) << "Retrieved tool " << m_scoringTool << endmsg;

  sc = m_selectionTool.retrieve();
  if (sc.isFailure()) 
    {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_selectionTool << endmsg;
      return StatusCode::FAILURE;
    } 
  else 
    msg(MSG::INFO) << "Retrieved tool " << m_selectionTool << endmsg;
  
  sc = m_fitterTool.retrieve();
  if (sc.isFailure()) 
    {
      msg(MSG::FATAL) << "Failed to retrieve tool " << m_fitterTool << endmsg;
      return sc;
    } 
  else 
    msg(MSG::INFO) << "Retrieved tool " << m_fitterTool << endmsg;
  
  // suppress refit overwrites force refit
  if (m_forceRefit && m_suppressTrackFit ) 
    {
      msg(MSG::WARNING) << "Inconsistent parameter settings, forced refit is true, but fitting suppressed, resetingt force refit !" << endmsg;
      m_forceRefit = false;
    }

  // Print out memo that tracks have to be fitted
  if (!m_forceRefit) 
    {
        msg(MSG::INFO) << "The forced refit of Tracks is switched off." << endmsg;
        msg(MSG::INFO) << "Ensure, that the tracks are fitted after the ambiguity processing!" << endmsg;
    }

  // Configuration of the material effects
  Trk::ParticleSwitcher particleSwitch;
  m_particleHypothesis = particleSwitch.particle[m_matEffects];

  // brem fitting enabled ?
  if (m_tryBremFit)
    msg(MSG::INFO) << "Try brem fit and recovery for electron like tracks." << endmsg;

  // statistics
  {
  std::lock_guard<std::mutex> lock( m_statMutex );
  for (int i=0; i<5; i++) {
    m_Ncandidates[i]      = 0;
    m_NcandScoreZero[i]   = 0;
    m_NcandDouble[i]      = 0;
    m_NscoreOk[i]         = 0;
    m_NscoreZeroBremRefit[i]          = 0;
    m_NscoreZeroBremRefitFailed[i]    = 0;
    m_NscoreZeroBremRefitScoreZero[i] = 0;
    m_NscoreZero[i]       = 0;
    m_Naccepted[i]        = 0;
    m_NacceptedBrem[i]    = 0;
    m_NsubTrack[i]        = 0;
    m_NnoSubTrack[i]      = 0;
    m_NbremFits[i]        = 0;
    m_Nfits[i]            = 0;
    m_NrecoveryBremFits[i]= 0;
    m_NgoodFits[i]        = 0;
    m_NfailedFits[i]      = 0;
  }
  }

  return sc;
}
//==================================================================================================

StatusCode Trk::SimpleAmbiguityProcessorTool::finalize()
{
  return StatusCode::SUCCESS;
}

void Trk::SimpleAmbiguityProcessorTool::statistics()
{

  if (msgLvl(MSG::INFO)) {
    std::lock_guard<std::mutex> lock( m_statMutex );
    ATH_MSG_INFO (name() << " -- statistics:");
    std::streamsize ss = std::cout.precision();
    int iw=9;
    std::cout << "-------------------------------------------------------------------------------" << std::endl;
    std::cout << "  Number of events processed      :   "<< m_Nevents << std::endl;
    std::cout << "  statistics by eta range          ------All---Barrel---Trans.--- Endcap---DBM--- " << std::endl;
    std::cout << "---------------------------------------------------------------------------------" << std::endl;
    std::cout << "  Number of candidates at input   :" << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_Ncandidates[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_Ncandidates[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_Ncandidates[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_Ncandidates[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Ncandidates[Trk::SimpleAmbiguityProcessorTool::iDBM] << std::endl;
    std::cout << "  - candidates rejected score 0   :" << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NcandScoreZero[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NcandScoreZero[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NcandScoreZero[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NcandScoreZero[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NcandScoreZero[Trk::SimpleAmbiguityProcessorTool::iDBM] << std::endl;
    std::cout << "  - candidates rejected as double :" << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NcandDouble[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NcandDouble[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NcandDouble[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NcandDouble[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NcandDouble[Trk::SimpleAmbiguityProcessorTool::iDBM] << std::endl;
    std::cout << "---------------------------------------------------------------------------------" << std::endl;
    std::cout << "  candidates with good score      :" << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NscoreOk[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NscoreOk[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NscoreOk[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NscoreOk[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NscoreOk[Trk::SimpleAmbiguityProcessorTool::iDBM] << std::endl;
    if (m_tryBremFit) {
      std::cout << "  + recovered after brem refit    :" << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NscoreZeroBremRefit[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NscoreZeroBremRefit[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NscoreZeroBremRefit[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NscoreZeroBremRefit[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NscoreZeroBremRefit[Trk::SimpleAmbiguityProcessorTool::iDBM] << std::endl;
    }
    std::cout << "  candidates rejected score 0     :" << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NscoreZero[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NscoreZero[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NscoreZero[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NscoreZero[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NscoreZero[Trk::SimpleAmbiguityProcessorTool::iDBM] << std::endl;
    if (m_tryBremFit) {
      std::cout << "  + rejected failed brem refit    :" << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NscoreZeroBremRefitFailed[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NscoreZeroBremRefitFailed[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NscoreZeroBremRefitFailed[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NscoreZeroBremRefitFailed[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NscoreZeroBremRefitFailed[Trk::SimpleAmbiguityProcessorTool::iDBM] << std::endl;
      std::cout << "  + rejected brem refit score 0   :" << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NscoreZeroBremRefitScoreZero[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NscoreZeroBremRefitScoreZero[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NscoreZeroBremRefitScoreZero[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NscoreZeroBremRefitScoreZero[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NscoreZeroBremRefitScoreZero[Trk::SimpleAmbiguityProcessorTool::iDBM] << std::endl;
    }
    std::cout << "---------------------------------------------------------------------------------" << std::endl;
    std::cout << "  number of normal fits           :" << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_Nfits[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_Nfits[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_Nfits[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_Nfits[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Nfits[Trk::SimpleAmbiguityProcessorTool::iDBM] << std::endl;
    if (m_tryBremFit) {
      std::cout << "  + 2nd brem fit for failed fit   :" << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NrecoveryBremFits[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NrecoveryBremFits[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NrecoveryBremFits[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NrecoveryBremFits[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NrecoveryBremFits[Trk::SimpleAmbiguityProcessorTool::iDBM] << std::endl;
      std::cout << "  normal brem fits for electrons  :" << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NbremFits[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NbremFits[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NbremFits[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NbremFits[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)
                << m_NbremFits[Trk::SimpleAmbiguityProcessorTool::iDBM] << std::endl;
    }
    std::cout << "---------------------------------------------------------------------------------" << std::endl;
    std::cout << "  sum of succesful fits           :" << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NgoodFits[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NgoodFits[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NgoodFits[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NgoodFits[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NgoodFits[Trk::SimpleAmbiguityProcessorTool::iDBM] << std::endl;
    std::cout << "  sum of failed fits              :" << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NfailedFits[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NfailedFits[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NfailedFits[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NfailedFits[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_NfailedFits[Trk::SimpleAmbiguityProcessorTool::iDBM] << std::endl;
    std::cout << "---------------------------------------------------------------------------------" << std::endl;
    std::cout << "  Number of subtracks created     :" << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NsubTrack[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NsubTrack[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NsubTrack[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NsubTrack[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::endl;
    std::cout << "  Number of candidates excluded   :" << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NnoSubTrack[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NnoSubTrack[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NnoSubTrack[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_NnoSubTrack[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::endl;
    std::cout << "---------------------------------------------------------------------------------" << std::endl;
    std::cout << "  Number of tracks accepted       :" << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_Naccepted[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_Naccepted[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_Naccepted[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
	      << m_Naccepted[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::setiosflags(std::ios::dec) << std::setw(iw)
              << m_Naccepted[Trk::SimpleAmbiguityProcessorTool::iDBM] << std::endl;
    if (m_tryBremFit) {
      std::cout << "  including number of brem fits   :" << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NacceptedBrem[Trk::SimpleAmbiguityProcessorTool::iAll] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NacceptedBrem[Trk::SimpleAmbiguityProcessorTool::iBarrel] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NacceptedBrem[Trk::SimpleAmbiguityProcessorTool::iTransi] << std::setiosflags(std::ios::dec) << std::setw(iw)
		<< m_NacceptedBrem[Trk::SimpleAmbiguityProcessorTool::iEndcap] << std::endl;
    }
    std::cout << "---------------------------------------------------------------------------------" << std::endl;
    std::cout << std::setiosflags(std::ios::fixed | std::ios::showpoint) << std::setprecision(2)
	      << "    definition: ( 0.0 < Barrel < " << m_etabounds[iBarrel-1] << " < Transition < " << m_etabounds[iTransi-1]
	      << " < Endcap < " << m_etabounds[iEndcap-1] << " DBM )" << std::endl;
    std::cout << "-------------------------------------------------------------------------------" << std::endl;
    std::cout.precision (ss);
  }
  return;
}

//==================================================================================================

/** helper function for statistics */

void Trk::SimpleAmbiguityProcessorTool::increment_by_eta(std::vector<int>& Ntracks, const Track* track, bool updateAll) const {

  std::lock_guard<std::mutex> lock( m_statMutex );
  if (updateAll) Ntracks[Trk::SimpleAmbiguityProcessorTool::iAll] += 1;

  // test
  if (!track)
    {
       ATH_MSG_ERROR ("track pointer zero, should not happen!");
       return;
    }

  // use first parameter 
  if (!track->trackParameters())
    {
      ATH_MSG_WARNING ("No track parameters, needed for statistics code in Trk::SimpleAmbiguityProcessorTool!");
    }
  else
    {
      double eta = track->trackParameters()->front()->eta();
      if (fabs(eta) < m_etabounds[0]) ++Ntracks[Trk::SimpleAmbiguityProcessorTool::iBarrel];
      else if (fabs(eta) < m_etabounds[1]) ++Ntracks[Trk::SimpleAmbiguityProcessorTool::iTransi];
      else if (fabs(eta) < m_etabounds[2]) ++Ntracks[Trk::SimpleAmbiguityProcessorTool::iEndcap];
      else if ((fabs(eta) > m_etabounds[3]) && (fabs(eta) < m_etabounds[4])) ++Ntracks[Trk::SimpleAmbiguityProcessorTool::iDBM];
    }
}

//==================================================================================================

/** Do actual processing of event. Takes a track container, 
    and then returns the tracks which have been selected*/


TrackCollection*  Trk::SimpleAmbiguityProcessorTool::process(const TrackCollection* trackCol, Trk::PRDtoTrackMap *prd_to_track_map) const {
  std::vector<const Track*> tracks;
  tracks.reserve(trackCol->size());
  for(const Track* e: *trackCol){
    tracks.push_back(e);
  }
  return process_vector(tracks, prd_to_track_map);
}


TrackCollection*  Trk::SimpleAmbiguityProcessorTool::process(const TracksScores* tracksScores) const {
  std::vector<const Track*> tracks;
  tracks.reserve(tracksScores->size());
  for(const std::pair<const Trk::Track *, float>& e: *tracksScores){
    tracks.push_back(e.first);
  }

  TrackCollection* re_tracks = process_vector(tracks,nullptr /* no external PRD-to-track map*/);
  return re_tracks;
}

TrackCollection*  Trk::SimpleAmbiguityProcessorTool::process_vector(std::vector<const Track*> &tracks, Trk::PRDtoTrackMap *prd_to_track_map) const{
  using namespace std;

  DEBUG_CODE( ntupleReset(tracks) );
  DEBUG_CODE( fillEventData(tracks) );
  ++m_Nevents; // statistics

  TrackScoreMap trackScoreTrackMap;
  std::unique_ptr<Trk::PRDtoTrackMap> prd_to_track_map_cleanup;
  if (!prd_to_track_map) {
     // create internal PRD-to-track map
     prd_to_track_map_cleanup = std::move( m_assoTool->createPRDtoTrackMap() );
     prd_to_track_map = prd_to_track_map_cleanup.get();
  }
  //put tracks into maps etc
  ATH_MSG_DEBUG ("Adding input track candidates to list");
  addNewTracks(tracks, trackScoreTrackMap, *prd_to_track_map);

  // going to do simple algorithm for now:
  // - take track with highest score
  // - remove shared hits from all other tracks
  // - take next highest scoring tracks, and repeat

  ATH_MSG_DEBUG ("Solving Tracks");
  std::vector<std::unique_ptr<const Trk::Track> > cleanup_tracks;
  TrackCollection* final_tracks = solveTracks(trackScoreTrackMap, *prd_to_track_map,cleanup_tracks);

  if (msgLvl(MSG::DEBUG)) dumpTracks(*final_tracks);
  return final_tracks;
}

//==================================================================================================
void Trk::SimpleAmbiguityProcessorTool::addNewTracks(const std::vector<const Track*> &tracks,
                                                     TrackScoreMap& trackScoreTrackMap,
                                                     Trk::PRDtoTrackMap &prd_to_track_map) const
{
  using namespace std;

  DEBUG_CODE( findTrueTracks(&tracks) );
 
  ATH_MSG_DEBUG ("Number of tracks at Input: "<<tracks.size());

  /** signature map to drop double track. */
  PrdSignatureSet prdSigSet;

  for(const Track *a_track : tracks) {

      DEBUG_CODE( resetTrackOutliers() );

      ATH_MSG_DEBUG ("Processing track candidate "<<a_track);
      // statistics
      increment_by_eta(m_Ncandidates,a_track);
    
      bool reject = false;
    
      // only fitted tracks get hole search, input is not fitted
      TrackScore score = m_scoringTool->score( *a_track, true);

      DEBUG_CODE( setBarcodeStats(a_track,score) );
      // veto tracks with score 0
      if (score==0) { 
	ATH_MSG_DEBUG ("Candidate score is zero, reject it");
	// statistic
	increment_by_eta(m_NcandScoreZero,a_track);
	
	reject = true;

        DEBUG_CODE(fillBadTrack(a_track,prd_to_track_map) );

      } else {

	ATH_MSG_DEBUG ("Track Score is "<< score);
	
	// double track rejection
	if (m_dropDouble) {
          std::vector<const Trk::PrepRawData*> prds = m_assoTool->getPrdsOnTrack(prd_to_track_map, *a_track);

	  // unfortunately PrepRawDataSet is not a set !
	  PrdSignature prdSig;
	  prdSig.insert( prds.begin(),prds.end() );

	  // we try to insert it into the set, if we fail (pair.second), it then exits already
	  if ( !(prdSigSet.insert(prdSig)).second ) {

	    ATH_MSG_DEBUG ("Double track, reject it !");
	    // statistic
	    increment_by_eta(m_NcandDouble,a_track);

	    reject = true;

            DEBUG_CODE(fillDuplicateTrack(a_track) );
	  } else {
	    ATH_MSG_DEBUG ("Insert new track in PrdSignatureSet");
	  }
	}
      }
 
      if (!reject) {

         // DEBUG_CODE( associateToOrig ( new_track, a_track) );

	// add track to map, map is sorted small to big ! set if fitted
	ATH_MSG_VERBOSE ("Track  ("<< a_track <<") has score "<<score);
        TrackPtr ptr(a_track);
        if (!m_forceRefit) ptr.forceFitted();
	trackScoreTrackMap.insert( make_pair(-score,std::move(ptr)) );

	// DEBUG_CODE(  keepTrackOfTracks(a_track,new_track) );

      }
    }
  
  ATH_MSG_DEBUG ("Number of tracks in map:"<<trackScoreTrackMap.size());
  DEBUG_CODE( countTrueTracksInMap( trackScoreTrackMap ) );
  
  return;
}

//==================================================================================================

void Trk::SimpleAmbiguityProcessorTool::addTrack(Trk::Track* in_track,
                                                 const bool fitted,
                                                 TrackScoreMap &trackScoreTrackMap,
                                                 Trk::PRDtoTrackMap &prd_to_track_map,
                                                 std::vector<std::unique_ptr<const Trk::Track> >& cleanup_tracks) const
{
  using namespace std;
  std::unique_ptr<Trk::Track> atrack(in_track);
  // compute score
  TrackScore score;
  bool suppressHoleSearch = fitted ? m_suppressHoleSearch : true;
  if (m_trackSummaryTool.isEnabled()) {
     m_trackSummaryTool->computeAndReplaceTrackSummary(*atrack,
                                                       &prd_to_track_map,
                                                       suppressHoleSearch);
  }

  score = m_scoringTool->score( *atrack, suppressHoleSearch );

  // do we accept the track ?
  if (score!=0)
    {
      ATH_MSG_DEBUG ("Track  ("<< atrack.get() <<") has score "<<score);
      // statistic
      increment_by_eta(m_NscoreOk,atrack.get());

      // add track to map, map is sorted small to big !
      trackScoreTrackMap.insert( make_pair(-score, TrackPtr(atrack.release(), fitted)) );

      return;
    }

  // do we try to recover the track ?
  if (score==0 && fitted && m_tryBremFit &&
      !atrack->info().trackProperties(Trk::TrackInfo::BremFit) &&
      atrack->trackParameters()->front()->pT() > m_pTminBrem &&
      (!m_caloSeededBrem || atrack->info().patternRecoInfo(Trk::TrackInfo::TrackInCaloROI)))
    {

      ATH_MSG_DEBUG ("Track score is zero, try to recover it via brem fit");

      // run track fit using electron hypothesis
      std::unique_ptr<Trk::Track> bremTrack( m_fitterTool->fit(*atrack,true,Trk::electron) );

      if (!bremTrack)
	{
	  ATH_MSG_DEBUG ("Brem refit failed, drop track");
	  // statistic
	  increment_by_eta(m_NscoreZeroBremRefitFailed,atrack.get());
	  increment_by_eta(m_NfailedFits,atrack.get());

	  // clean up
          cleanup_tracks.push_back(std::move(atrack));

	}
      else
	{

	  // statistic
	  increment_by_eta(m_NgoodFits,bremTrack.get());

	  // rerun score
	  score = m_scoringTool->score( *bremTrack, suppressHoleSearch );

	  // do we accept the track ?
	  if (score!=0)
	    {
              ATH_MSG_DEBUG ("Brem refit successful, recovered track  ("<< atrack.get() <<") has score "<<score);
	      // statistics
	      increment_by_eta(m_NscoreZeroBremRefit,bremTrack.get());

	      // add track to map, map is sorted small to big !
	      trackScoreTrackMap.insert( make_pair(-score, TrackPtr(bremTrack.release(), fitted)) );
	      return;
	    }
	  else
	    {
	      ATH_MSG_DEBUG ("Brem refit gave still track score zero, reject it");
	      // statistic
	      increment_by_eta(m_NscoreZeroBremRefitScoreZero,bremTrack.get());

	    }
          cleanup_tracks.push_back(std::move(atrack));
	}
    }
  else
    {
      ATH_MSG_DEBUG ("Track score is zero, reject it");
      // statistic
      increment_by_eta(m_NscoreZero,atrack.get());

      DEBUG_CODE( rejectedTrack(atrack.get(), prd_to_track_map) );
      cleanup_tracks.push_back(std::move(atrack));
    }
  return;
}
//==================================================================================================

TrackCollection *Trk::SimpleAmbiguityProcessorTool::solveTracks(TrackScoreMap& trackScoreTrackMap,
                                                                Trk::PRDtoTrackMap &prd_to_track_map,
                                                                std::vector<std::unique_ptr<const Trk::Track> >& cleanup_tracks) const
{
  std::unique_ptr<TrackCollection> final_tracks(std::make_unique<TrackCollection>());
  using namespace std;
 
  DEBUG_CODE( fitStatReset() );

  ATH_MSG_DEBUG ("Starting to solve tracks");

  // now loop as long as map is not empty
  while ( !trackScoreTrackMap.empty() )
    {
      // get current best candidate 
      TrackScoreMap::iterator itnext = trackScoreTrackMap.begin();
      TrackScore ascore(itnext->first);
      TrackPtr  atrack(std::move(itnext->second));
      trackScoreTrackMap.erase(itnext);

      // clean it out to make sure not to many shared hits
      ATH_MSG_VERBOSE ("--- Trying next track "<<atrack.track()<<"\t with score "<<-ascore);
      std::unique_ptr<Trk::Track> cleanedTrack;
      auto [cleanedTrack_tmp,keep_orig] = m_selectionTool->getCleanedOutTrack( atrack.track() , -(ascore), prd_to_track_map);
      cleanedTrack.reset( cleanedTrack_tmp);

      // cleaned track is input track and fitted
      if (keep_orig && atrack.fitted() )
	{

          DEBUG_CODE( keepFittedInputTrack(atrack.track(), ascore) );
	  // track can be kept as is and is already fitted
	  ATH_MSG_DEBUG ("Accepted track "<<atrack.track()<<"\t has score "<<-(ascore));
	  // statistic
	  increment_by_eta(m_Naccepted,atrack.track());
	  if (m_tryBremFit && atrack->info().trackProperties(Trk::TrackInfo::BremFit))
	    increment_by_eta(m_NacceptedBrem,atrack.track());

	  // add track to PRD_AssociationTool
          StatusCode sc = m_assoTool->addPRDs(prd_to_track_map, *atrack.track());
	  if (sc.isFailure()) msg(MSG::ERROR) << "addPRDs() failed" << endmsg;
	  // add to output list 

          DEBUG_CODE( acceptedTrack(atrack.track()) );

	  final_tracks->push_back( const_cast<Track*>(atrack.release()) );

	}
      else if ( keep_orig )
	{

          DEBUG_CODE( memoriseOutliers(itnext->second.frst) );

	  // don't forget to drop track from map
	  // track can be kept as is, but is not yet fitted
	  ATH_MSG_DEBUG ("Good track, but need to fit this track first, score, add it into map again and retry !");
	  refitTrack(atrack.track(),trackScoreTrackMap, prd_to_track_map, cleanup_tracks);
          if (atrack.newTrack()) {
             cleanup_tracks.push_back( std::unique_ptr<Trk::Track>(atrack.release()) );
          }
	  // delete original copy
	 }
      else if ( cleanedTrack.get() )
	{

           DEBUG_CODE(newCleanedTrack(cleanedTrack.get(), atrack.track()) );

	  // now delete original track
          if (atrack.newTrack()) {
              cleanup_tracks.push_back( std::unique_ptr<Trk::Track>(atrack.release()));
          }
	  // don't forget to drop track from map

	  // stripped down version should be reconsidered
	  ATH_MSG_DEBUG ("Candidate excluded, add subtrack to map. Track "<<cleanedTrack.get());
	  // statistic
	  increment_by_eta(m_NsubTrack,cleanedTrack.get());

	  // track needs fitting !
	  addTrack( cleanedTrack.release(), false, trackScoreTrackMap, prd_to_track_map, cleanup_tracks);

	}
      else
	{

          DEBUG_CODE( acceptedTrack(atrack.track()));

	  // track should be discarded
	  ATH_MSG_DEBUG ("Track "<< atrack.track() << " is excluded, no subtrack, reject");
	  // statistic
	  increment_by_eta(m_NnoSubTrack,atrack.track());

          if (atrack.newTrack()) {
             cleanup_tracks.push_back(  std::unique_ptr<Trk::Track>(atrack.release()) );
          }
	  // don't forget to drop track from map
	}
    }
  
  ATH_MSG_DEBUG ("Finished, number of track on output: "<<final_tracks->size());
  

  DEBUG_CODE( eventSummary(final_tracks) );

  return final_tracks.release();
}

//==================================================================================================

void Trk::SimpleAmbiguityProcessorTool::refitTrack( const Trk::Track* track,
                                                    TrackScoreMap& trackScoreTrackMap,
                                                    Trk::PRDtoTrackMap &prd_to_track_map,
                                                    std::vector<std::unique_ptr<const Trk::Track> >& cleanup_tracks) const
{
  using namespace std;
  std::unique_ptr<Trk::Track> newTrack;
  if (!m_suppressTrackFit) {
    if (m_refitPrds) 
      {
	// simple case, fit PRD directly
	ATH_MSG_VERBOSE ("Refit track "<<track<<" from PRDs");
	newTrack.reset( refitPrds (track, prd_to_track_map) );
      }
    else 
      {
	// ok, we fit ROTs
	ATH_MSG_VERBOSE ("Refit track "<<track<<" from ROTs");
	newTrack.reset( refitRots (track) );
      }
  }
  else
    {
      double reXi2 = 0.; int nDF = 0;
      const DataVector<const TrackStateOnSurface>* tsos = track->trackStateOnSurfaces();
      DataVector<const TrackStateOnSurface>* vecTsos = new DataVector<const TrackStateOnSurface>();
      // loop over TSOS, copy TSOS and push into vector
      DataVector<const TrackStateOnSurface>::const_iterator iTsos    = tsos->begin();
      DataVector<const TrackStateOnSurface>::const_iterator iTsosEnd = tsos->end(); 
      for ( ; iTsos != iTsosEnd ; ++iTsos) 
	{
	  const TrackStateOnSurface* newTsos = new TrackStateOnSurface(**iTsos);
	  vecTsos->push_back(newTsos);
	  if((*iTsos)->type(Trk::TrackStateOnSurface::Measurement))
	    {  //Get the chi2 and number of hits
	      if ((*iTsos)->fitQualityOnSurface()) {
		reXi2 += (*iTsos)->fitQualityOnSurface()->chiSquared();
		nDF   += (*iTsos)->fitQualityOnSurface()->numberDoF();
	      }
	    }
	}
      Trk::FitQuality* fq = new Trk::FitQuality(reXi2,nDF-5);
      Trk::TrackInfo info;
      info.addPatternRecoAndProperties(track->info());
      Trk::TrackInfo newInfo;
      newInfo.setPatternRecognitionInfo(Trk::TrackInfo::SimpleAmbiguityProcessorTool);
      info.addPatternReco(newInfo); 

      newTrack.reset( new Trk::Track(info, vecTsos, fq) );
    }

  if (newTrack)
    {
      ATH_MSG_DEBUG ("New track successfully fitted"<<newTrack.get());
      DEBUG_CODE( newCleanedTrack( newTrack.get(), atrack.get()) );

      addTrack( newTrack.release(), true, trackScoreTrackMap, prd_to_track_map, cleanup_tracks);
    }
  else {
     DEBUG_CODE( fillFailedFit(track, prd_to_track_map) );
     ATH_MSG_DEBUG ("Fit failed !");
  }  
  
  return;
}

//==================================================================================================

Trk::Track* Trk::SimpleAmbiguityProcessorTool::refitPrds( const Trk::Track* track, Trk::PRDtoTrackMap &prd_to_track_map) const
{

  // get vector of PRDs
  std::vector<const Trk::PrepRawData*> prds = m_assoTool->getPrdsOnTrack(prd_to_track_map,*track);

  if ( 0==prds.size() ) {
    msg(MSG::WARNING) << "No PRDs on track"<<endmsg;
    return nullptr;
  }
     
  ATH_MSG_VERBOSE ("Track "<<track<<"\t has "<<prds.size()<<"\t PRDs");

  const TrackParameters* par = track->perigeeParameters();
  if (par==nullptr) {
    ATH_MSG_DEBUG ("Track ("<<track<<") has no perigee! Try any other ?");
    par = track->trackParameters()->front();
    if (par==nullptr) {
      ATH_MSG_DEBUG ("Track ("<<track<<") has no Track Parameters ! No refit !");
      return nullptr;
    }
  }

  // refit using first parameter, do outliers
  Trk::Track* newTrack = nullptr;

  if (m_tryBremFit && track->info().trackProperties(Trk::TrackInfo::BremFit))
    {
      // statistics
      increment_by_eta(m_NbremFits,track);

      ATH_MSG_VERBOSE ("Brem track, refit with electron brem fit");
      newTrack = m_fitterTool->fit(prds, *par, true, Trk::electron);

    }
  else
    {
      // statistics
      increment_by_eta(m_Nfits,track);

      ATH_MSG_VERBOSE ("Normal track, refit");
      newTrack = m_fitterTool->fit(prds, *par, true, m_particleHypothesis);

      if (!newTrack && m_tryBremFit && par->pT() > m_pTminBrem &&
	  (!m_caloSeededBrem || track->info().patternRecoInfo(Trk::TrackInfo::TrackInCaloROI)))
	{
	  // statistics
	  increment_by_eta(m_NrecoveryBremFits,track);

	  ATH_MSG_VERBOSE ("Normal fit failed, try brem recovery");
	  newTrack = m_fitterTool->fit(prds, *par, true, Trk::electron);
	}
    }
  
  if(newTrack)
    {
      // statistic
      increment_by_eta(m_NgoodFits,newTrack);

      //keeping the track of previously accumulated TrackInfo
      const Trk::TrackInfo old_info = track->info();
      newTrack->info().addPatternReco(old_info);
    }
  else
    {
      // statistic
      increment_by_eta(m_NfailedFits,track);
    }
  return newTrack;
}

//==================================================================================================

Trk::Track* Trk::SimpleAmbiguityProcessorTool::refitRots( const Trk::Track* track) const
{

  ATH_MSG_VERBOSE ("Refit track "<<track);

  // refit using first parameter, do outliers
  Trk::Track* newTrack = 0;

  if (m_tryBremFit &&
      track->info().trackProperties(Trk::TrackInfo::BremFit))
    {
      // statistics
      increment_by_eta(m_NbremFits,track);

      ATH_MSG_VERBOSE ("Brem track, refit with electron brem fit");
      newTrack = m_fitterTool->fit(*track, true, Trk::electron);
    }
  else
    {
      // statistics
      increment_by_eta(m_Nfits,track);

      ATH_MSG_VERBOSE ("Normal track, refit");
      newTrack = m_fitterTool->fit(*track, true, m_particleHypothesis);

      if (!newTrack && m_tryBremFit &&
	  track->trackParameters()->front()->pT() > m_pTminBrem &&
	  (!m_caloSeededBrem || track->info().patternRecoInfo(Trk::TrackInfo::TrackInCaloROI)))
	{
	  // statistics
	  increment_by_eta(m_NrecoveryBremFits,track);

	  ATH_MSG_VERBOSE ("Normal fit failed, try brem recovery");
	  newTrack = m_fitterTool->fit(*track, true, Trk::electron);
	}
    }

  if(newTrack)
    {
      // statistic
      increment_by_eta(m_NgoodFits,newTrack);

      //keeping the track of previously accumulated TrackInfo
      const Trk::TrackInfo old_info = track->info();
      newTrack->info().addPatternReco(old_info);
    }
  else
    {
      // statistic
      increment_by_eta(m_NfailedFits,track);
    }
  return newTrack;
}

//==================================================================================================

void Trk::SimpleAmbiguityProcessorTool::dumpTracks( const TrackCollection& tracks ) const
{

  ATH_MSG_VERBOSE ("Dumping tracks in collection");
  int num=0;
  TrackScore totalScore = 0;
  TrackCollection::const_iterator it    = tracks.begin();
  TrackCollection::const_iterator itEnd = tracks.end();
  for (; it != itEnd ; ++it)
    {
      // score track:
      const TrackScore score = m_scoringTool->score( **it, m_suppressHoleSearch );
      ATH_MSG_VERBOSE (num++<<"\tTrack :"<<*it<<"\tScore: "<<score);
      totalScore+=score;
    }
  ATH_MSG_DEBUG ("Total event score : "<<totalScore);
  return;
}





