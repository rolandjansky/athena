/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "SimpleAmbiguityProcessorTool.h"
#include "TrackScoringTool.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/TrackInfo.h"
#include <map>
#include <memory>
#include "AmbiguityProcessorUtility.h"


//==================================================================================================
Trk::SimpleAmbiguityProcessorTool::SimpleAmbiguityProcessorTool(const std::string& t, 
                const std::string& n,
                const IInterface*  p )
  :
  AmbiguityProcessorBase(t,n,p),
  m_fitterTool ("Trk::KalmanFitter/InDetTrackFitter"), 
  m_selectionTool("InDet::InDetAmbiTrackSelectionTool/InDetAmbiTrackSelectionTool"){
  // statistics stuff

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
  declareProperty("etaBounds"            , m_etaBounds,"eta intervals for internal monitoring");

}
//==================================================================================================

Trk::SimpleAmbiguityProcessorTool::~SimpleAmbiguityProcessorTool()
{
}
//==================================================================================================

StatusCode Trk::SimpleAmbiguityProcessorTool::initialize(){
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "AlgTool::initialise failed" );
    return StatusCode::FAILURE;
  }
  // the association tool can be disabled if for this processor instance a PRD-to-track map is provided by the caller.
  ATH_CHECK( m_assoTool.retrieve() );
  ATH_CHECK( m_trackSummaryTool.retrieve( DisableTool{ m_trackSummaryTool.name().empty() } ) );
  ATH_CHECK( m_scoringTool.retrieve());
  ATH_CHECK( m_selectionTool.retrieve());
  ATH_CHECK( m_fitterTool.retrieve());
  // suppress refit overwrites force refit
  if (m_forceRefit && m_suppressTrackFit ) {
    ATH_MSG_WARNING( "Inconsistent parameter settings, forced refit is true, but fitting suppressed, resetting force refit !" );
    m_forceRefit = false;
  }
  // Print out memo that tracks have to be fitted
  if (!m_forceRefit) {
    ATH_MSG_INFO( "The forced refit of Tracks is switched off." );
    ATH_MSG_INFO( "Ensure that the tracks are fitted after the ambiguity processing!");
  }
  // Configuration of the material effects
  Trk::ParticleSwitcher particleSwitch;
  m_particleHypothesis = particleSwitch.particle[m_matEffects];
  // brem fitting enabled ?
  if (m_tryBremFit) {
     ATH_MSG_INFO( "Try brem fit and recovery for electron like tracks.");
  }

  ATH_CHECK(m_clusterSplitProbContainerIn.initialize(!m_clusterSplitProbContainerIn.key().empty()));
  ATH_CHECK(m_clusterSplitProbContainerOut.initialize(!m_clusterSplitProbContainerOut.key().empty()));
  // statistics
  if (m_etaBounds.size() != Counter::nRegions) {
     ATH_MSG_ERROR( "There must be exactly " << Counter::nRegions
                    << " etaBounds: barrel end, transition region end, end-cap end, DBM start, DBM end." );
     return StatusCode::FAILURE;
  }
  return sc;
}
//==================================================================================================


StatusCode Trk::SimpleAmbiguityProcessorTool::finalize(){
  return StatusCode::SUCCESS;
}

void Trk::SimpleAmbiguityProcessorTool::statistics(){
  if (msgLvl(MSG::INFO)) {
     MsgStream &out=msg(MSG::INFO);
     out << " -- statistics:" << "\n";
     std::lock_guard<std::mutex> lock( m_statMutex );
     dumpStat(out);
     out << endmsg;
  }
}


//==================================================================================================
/** Do actual processing of event. Takes a track container, 
    and then returns the tracks which have been selected*/

TrackCollection*  
Trk::SimpleAmbiguityProcessorTool::process(const TrackCollection* trackCol, Trk::PRDtoTrackMap *prdToTrackMap) const {
  return processVector(*trackCol, prdToTrackMap);
}


TrackCollection*  
Trk::SimpleAmbiguityProcessorTool::process(const TracksScores* tracksScores) const {
  TrackCollection tracks(SG::VIEW_ELEMENTS);
  tracks.reserve(tracksScores->size());
  for(const std::pair<const Trk::Track *, float>& e: *tracksScores){
    auto atrack ATLAS_THREAD_SAFE = const_cast<Trk::Track *>(e.first);
    tracks.push_back(atrack);
  }
  TrackCollection* re_tracks = processVector(tracks,nullptr /* no external PRD-to-track map*/);
  return re_tracks;
}

TrackCollection*  
Trk::SimpleAmbiguityProcessorTool::processVector(const TrackCollection &tracks, Trk::PRDtoTrackMap *prdToTrackMap) const{
  TrackScoreMap trackScoreTrackMap;
  std::unique_ptr<Trk::PRDtoTrackMap> prdToTrackMap_cleanup;
  if (!prdToTrackMap) {
     // create internal PRD-to-track map
     prdToTrackMap_cleanup = m_assoTool->createPRDtoTrackMap();
     prdToTrackMap = prdToTrackMap_cleanup.get();
  }
  //put tracks into maps etc
  ATH_MSG_DEBUG ("Adding input track candidates to list");
  Counter stat(m_etaBounds);
  addNewTracks(tracks, trackScoreTrackMap, *prdToTrackMap);
  // going to do simple algorithm for now:
  // - take track with highest score
  // - remove shared hits from all other tracks
  // - take next highest scoring tracks, and repeat
  ATH_MSG_DEBUG ("Solving Tracks");
  std::vector<std::unique_ptr<const Trk::Track> > trackDustbin;
  TrackCollection* finalTracks = solveTracks(trackScoreTrackMap, *prdToTrackMap,trackDustbin, stat);
  {
     std::lock_guard<std::mutex> lock(m_statMutex);
     m_stat += stat;
  }
  if (msgLvl(MSG::DEBUG)) dumpTracks(*finalTracks);
  return finalTracks;
}

//==================================================================================================
void Trk::SimpleAmbiguityProcessorTool::addNewTracks(const TrackCollection &tracks,
                                                     TrackScoreMap& trackScoreTrackMap,
                                                     Trk::PRDtoTrackMap &prdToTrackMap) const {
  Counter stat(m_etaBounds);
  stat.newEvent();
  //                              
  ATH_MSG_DEBUG ("Number of tracks at Input: "<<tracks.size());
  /** signature set to drop double track. */
  PrdSignatureSet prdSigSet;
  //map the two lowest categories (zero score, duplicate track) onto the counter categories
  const std::array<CounterIndex, 2> categoryMapping {CounterIndex::kNcandScoreZero, CounterIndex::kNcandDouble};
  for(const Track *pTrack : tracks) {
    stat.incrementCounterByRegion(CounterIndex::kNcandidates,pTrack);
    TrackScore score = m_scoringTool->score( *pTrack, true);
    const auto category = AmbiguityProcessor::categoriseTrack(*pTrack, score, m_dropDouble, m_assoTool, prdToTrackMap, prdSigSet);
    if (category < categoryMapping.size()) stat.incrementCounterByRegion(categoryMapping[category],pTrack);
    ATH_MSG_DEBUG(AmbiguityProcessor::debugMessage[category]);
    if (category == AmbiguityProcessor::TrackAccepted){
      ATH_MSG_VERBOSE ("Track  ("<< pTrack <<") has score "<<score);
      TrackPtr ptr(pTrack);
      if (!m_forceRefit) ptr.forceFitted();
      trackScoreTrackMap.insert( std::make_pair(-score,std::move(ptr)) );
    }
  }
  ATH_MSG_DEBUG ("Number of tracks in map:"<<trackScoreTrackMap.size());
  {
     std::lock_guard<std::mutex> lock(m_statMutex);
     m_stat += stat;
  }
}



//==================================================================================================

TrackCollection *
Trk::SimpleAmbiguityProcessorTool::solveTracks(TrackScoreMap& trackScoreTrackMap,
                                                                Trk::PRDtoTrackMap &prdToTrackMap,
                                                                std::vector<std::unique_ptr<const Trk::Track> >& trackDustbin,
                                                                Counter &stat) const{
  const EventContext& ctx = Gaudi::Hive::currentContext();
  SG::ReadHandle<Trk::ClusterSplitProbabilityContainer> splitProbContainerIn;
  if (!m_clusterSplitProbContainerIn.key().empty()) {
     splitProbContainerIn = SG::ReadHandle( m_clusterSplitProbContainerIn, ctx);
     if (!splitProbContainerIn.isValid()) {
        ATH_MSG_ERROR( "Failed to get input cluster split probability container "  << m_clusterSplitProbContainerIn.key());
     }
  }
  std::unique_ptr<Trk::ClusterSplitProbabilityContainer> splitProbContainerCleanup(!m_clusterSplitProbContainerIn.key().empty()
                                                                                      ? std::make_unique<ClusterSplitProbabilityContainer>(*splitProbContainerIn)
                                                                                      : std::make_unique<ClusterSplitProbabilityContainer>());
  SG::WriteHandle<Trk::ClusterSplitProbabilityContainer> splitProbContainerHandle;
  Trk::ClusterSplitProbabilityContainer *splitProbContainer;
  if (!m_clusterSplitProbContainerOut.key().empty()) {
     splitProbContainerHandle = SG::WriteHandle<Trk::ClusterSplitProbabilityContainer>( m_clusterSplitProbContainerOut, ctx);
     if (splitProbContainerHandle.record(std::move(splitProbContainerCleanup)).isFailure()) {
        ATH_MSG_FATAL( "Failed to record output cluster split probability container "  << m_clusterSplitProbContainerOut.key());
     }
     splitProbContainer=splitProbContainerHandle.ptr();
  }
  else {
     splitProbContainer=splitProbContainerCleanup.get();
  }

  std::unique_ptr<TrackCollection> finalTracks(std::make_unique<TrackCollection>());

  ATH_MSG_DEBUG ("Starting to solve tracks");
  // now loop as long as map is not empty
  while ( !trackScoreTrackMap.empty() ){
    // get current best candidate 
    TrackScoreMap::iterator itnext = trackScoreTrackMap.begin();
    TrackScore ascore(itnext->first);
    TrackPtr  atrack(std::move(itnext->second));
    trackScoreTrackMap.erase(itnext);
    // clean it out to make sure not to many shared hits
    ATH_MSG_VERBOSE ("--- Trying next track "<<atrack.track()<<"\t with score "<<-ascore);
    std::unique_ptr<Trk::Track> cleanedTrack;
    auto [cleanedTrack_tmp,keep_orig] = m_selectionTool->getCleanedOutTrack( atrack.track() , -(ascore), *splitProbContainer, prdToTrackMap);
    cleanedTrack.reset( cleanedTrack_tmp);
    // cleaned track is input track and fitted
    if (keep_orig && atrack.fitted() ){
      // track can be kept as is and is already fitted
      ATH_MSG_DEBUG ("Accepted track "<<atrack.track()<<"\t has score "<<-(ascore));
      // statistic
      stat.incrementCounterByRegion(CounterIndex::kNaccepted,atrack.track());
      if (m_tryBremFit && atrack->info().trackProperties(Trk::TrackInfo::BremFit)) stat.incrementCounterByRegion(CounterIndex::kNacceptedBrem,atrack.track());
      // add track to PRD_AssociationTool
      if (m_assoTool->addPRDs(prdToTrackMap, *atrack.track()).isFailure()) ATH_MSG_ERROR("addPRDs() failed" );
      // add to output list 
      finalTracks->push_back( const_cast<Track*>(atrack.release()) );
    } else if ( keep_orig ) {
      // don't forget to drop track from map
      // track can be kept as is, but is not yet fitted
      ATH_MSG_DEBUG ("Good track, but need to fit this track first, score, add it into map again and retry !");
      auto pRefittedTrack = refitTrack(atrack.track(), prdToTrackMap, stat);
      if(pRefittedTrack) {
         addTrack( pRefittedTrack, true , trackScoreTrackMap, prdToTrackMap, trackDustbin, stat);
      }
      if (atrack.newTrack()) {
        trackDustbin.emplace_back(atrack.release());
      }
    // delete original copy
    } else if ( cleanedTrack ) {
      // now delete original track
      if (atrack.newTrack()) {
        trackDustbin.emplace_back(atrack.release());
      }
      // don't forget to drop track from map
      // stripped down version should be reconsidered
      ATH_MSG_DEBUG ("Candidate excluded, add subtrack to map. Track "<<cleanedTrack.get());
      // statistic
      stat.incrementCounterByRegion(CounterIndex::kNsubTrack,cleanedTrack.get());
      // track needs fitting !
      addTrack( cleanedTrack.release(), false, trackScoreTrackMap, prdToTrackMap, trackDustbin, stat);
    } else {
      // track should be discarded
      ATH_MSG_DEBUG ("Track "<< atrack.track() << " is excluded, no subtrack, reject");
      // statistic
      stat.incrementCounterByRegion(CounterIndex::kNnoSubTrack,atrack.track());
      if (atrack.newTrack()) {
        trackDustbin.emplace_back(atrack.release());
      }
    // don't forget to drop track from map
    }
  }
  ATH_MSG_DEBUG ("Finished, number of track on output: "<<finalTracks->size());
  return finalTracks.release();
}


//==================================================================================================

Trk::Track* 
Trk::SimpleAmbiguityProcessorTool::refitPrds( const Trk::Track* track,
                                              Trk::PRDtoTrackMap &prdToTrackMap,
                                              Counter &stat) const{
  // get vector of PRDs
  std::vector<const Trk::PrepRawData*> prds = m_assoTool->getPrdsOnTrack(prdToTrackMap,*track);
  if ( prds.empty() ) {
    ATH_MSG_WARNING( "No PRDs on track");
    return nullptr;
  }
  const TrackParameters* par = getTrackParameters(track);
  if (not par) return nullptr;
  // refit using first parameter, do outliers
  Trk::Track* newTrack = nullptr;
  if (m_tryBremFit && track->info().trackProperties(Trk::TrackInfo::BremFit)){
    stat.incrementCounterByRegion(CounterIndex::kNbremFits,track);
    ATH_MSG_VERBOSE ("Brem track, refit with electron brem fit");
    newTrack = m_fitterTool->fit(prds, *par, true, Trk::electron);
  } else {
    stat.incrementCounterByRegion(CounterIndex::kNfits,track);
    ATH_MSG_VERBOSE ("Normal track, refit");
    newTrack = m_fitterTool->fit(prds, *par, true, m_particleHypothesis);
    if ((not newTrack) and shouldTryBremRecovery(*track, par)){
      stat.incrementCounterByRegion(CounterIndex::kNrecoveryBremFits,track);
      ATH_MSG_VERBOSE ("Normal fit failed, try brem recovery");
      newTrack = m_fitterTool->fit(prds, *par, true, Trk::electron);
    }
  }
  if(newTrack){
    stat.incrementCounterByRegion(CounterIndex::kNgoodFits,newTrack);
    //keeping the track of previously accumulated TrackInfo
    const Trk::TrackInfo& originalInfo = track->info();
    newTrack->info().addPatternReco(originalInfo);
  } else {
    stat.incrementCounterByRegion(CounterIndex::kNfailedFits,track);
  }
  return newTrack;
}


//==================================================================================================

void Trk::SimpleAmbiguityProcessorTool::dumpTracks( const TrackCollection& tracks ) const{
  ATH_MSG_VERBOSE ("Dumping tracks in collection");
  int num=0;
  TrackScore totalScore = 0;
  TrackCollection::const_iterator it    = tracks.begin();
  TrackCollection::const_iterator itEnd = tracks.end();
  for (; it != itEnd ; ++it){
    // score track:
    const TrackScore score = m_scoringTool->score( **it, m_suppressHoleSearch );
    ATH_MSG_VERBOSE (num++<<"\tTrack :"<<*it<<"\tScore: "<<score);
    totalScore+=score;
  }
  ATH_MSG_DEBUG ("Total event score : "<<totalScore);
}

void 
Trk::SimpleAmbiguityProcessorTool::dumpStat(MsgStream &out) const {
  auto parseFileName=[](const std::string & fullname){
    auto dotPosition = fullname.rfind('.');
    auto slashPosition = fullname.rfind('/');
    auto stringLength = dotPosition - slashPosition;
    return fullname.substr(slashPosition, stringLength);
  };
  // @TODO restore ios
  std::streamsize ss = std::cout.precision();
  out << "Output from ";
  out << parseFileName(__FILE__);
  out << "::";
  out << __func__;
  out << "\n";
  out <<             "---------------------------------------------------------------------------------" << "\n";
  out <<             "  Number of events processed      :   "<< m_stat.numberOfEvents() << "\n";
  out <<             "  statistics by eta range          ------All---Barrel---Trans.--- Endcap---DBM---" << "\n";
  out <<             "---------------------------------------------------------------------------------" << "\n";
  out << m_stat.dumpRegions(   "  Number of candidates at input   :", CounterIndex::kNcandidates);
  out << m_stat.dumpRegions(   "  - candidates rejected score 0   :", CounterIndex::kNcandScoreZero);
  out << m_stat.dumpRegions(   "  - candidates rejected as double :", CounterIndex::kNcandDouble);
  out <<             "---------------------------------------------------------------------------------" << "\n";
  out << m_stat.dumpRegions(   "  candidates with good score      :", CounterIndex::kNscoreOk);
  if (m_tryBremFit) {
     out << m_stat.dumpRegions("  + recovered after brem refit    :", CounterIndex::kNscoreZeroBremRefit);
  }
  out << m_stat.dumpRegions(   "  candidates rejected score 0     :", CounterIndex::kNscoreZero);
  if (m_tryBremFit) {
     out << m_stat.dumpRegions("  + rejected failed brem refit    :", CounterIndex::kNscoreZeroBremRefitFailed);
  }
  out <<             "---------------------------------------------------------------------------------" << "\n";
  out << m_stat.dumpRegions(   "  number of normal fits           :", CounterIndex::kNfits);
  if (m_tryBremFit) {
     out << m_stat.dumpRegions("  + 2nd brem fit for failed fit   :", CounterIndex::kNrecoveryBremFits);
     out << m_stat.dumpRegions("  normal brem fits for electrons  :", CounterIndex::kNbremFits);
  }
  out <<             "---------------------------------------------------------------------------------" << "\n";
  out << m_stat.dumpRegions(   "  sum of succesful fits           :", CounterIndex::kNgoodFits);
  out << m_stat.dumpRegions(   "  sum of failed fits              :", CounterIndex::kNfailedFits);
  out <<             "---------------------------------------------------------------------------------" << "\n";
  out << m_stat.dumpRegions(   "  Number of subtracks created     :", CounterIndex::kNsubTrack);
  out << m_stat.dumpRegions(   "  Number of candidates excluded   :", CounterIndex::kNnoSubTrack);
  out <<             "---------------------------------------------------------------------------------" << "\n";
  out << m_stat.dumpRegions(   "  Number of tracks accepted       :", CounterIndex::kNaccepted);
  if (m_tryBremFit) {
     out << m_stat.dumpRegions("  including number of brem fits   :", CounterIndex::kNacceptedBrem);
  }
  out <<             "---------------------------------------------------------------------------------" << "\n";
  out << std::setiosflags(std::ios::fixed | std::ios::showpoint) << std::setprecision(2)
      <<             "    definition: ( 0.0 < Barrel < " << m_etaBounds[Counter::iBarrel] << " < Transition < " << m_etaBounds[Counter::iTransi]
      <<             " < Endcap < " << m_etaBounds[Counter::iEndcap] << " DBM )" << "\n";
  out <<             "-------------------------------------------------------------------------------" << "\n";
  out.precision (ss);
}


std::unique_ptr<Trk::Track>
Trk::SimpleAmbiguityProcessorTool::doBremRefit(const Trk::Track & track) const{
  return std::unique_ptr<Trk::Track>(m_fitterTool->fit(track,true,Trk::electron));
}

std::unique_ptr<Trk::Track>
Trk::SimpleAmbiguityProcessorTool::fit(const Track &track, bool flag, Trk::ParticleHypothesis hypo) const{
  return std::unique_ptr<Trk::Track>(m_fitterTool->fit(track,flag,hypo));
}





