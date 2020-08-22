/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "SimpleAmbiguityProcessorTool.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrackScoringTool.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkTrack/TrackInfo.h"
#include <map>
#include <memory>

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
  m_etabounds( {0.8,1.6,2.5,10.0} ),
  m_stat(m_etabounds)
{
  // statitics stuff

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
    ATH_MSG_WARNING( "Inconsistent parameter settings, forced refit is true, but fitting suppressed, resetingt force refit !" );
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
  // statistics
  if (m_etabounds.size() != Counter::nRegions) {
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
  Counter stat(m_etabounds);
  addNewTracks(tracks, trackScoreTrackMap, *prdToTrackMap, stat);
  {
    std::lock_guard<std::mutex> lock(m_statMutex);
    stat.newEvent();
  }
  // going to do simple algorithm for now:
  // - take track with highest score
  // - remove shared hits from all other tracks
  // - take next highest scoring tracks, and repeat
  ATH_MSG_DEBUG ("Solving Tracks");
  std::vector<std::unique_ptr<const Trk::Track> > cleanupTracks;
  TrackCollection* finalTracks = solveTracks(trackScoreTrackMap, *prdToTrackMap,cleanupTracks, stat);
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
                                                     Trk::PRDtoTrackMap &prdToTrackMap,
                                                     Counter &stat) const {
  ATH_MSG_DEBUG ("Number of tracks at Input: "<<tracks.size());
  /** signature map to drop double track. */
  PrdSignatureSet prdSigSet;
  for(const Track *pTrack : tracks) {
    ATH_MSG_DEBUG ("Processing track candidate "<<pTrack);
    // statistics
    stat.incrementCounterByRegion(ECounter::kNcandidates,pTrack);
    bool reject = false;
    // only fitted tracks get hole search, input is not fitted
    TrackScore score = m_scoringTool->score( *pTrack, true);
    // veto tracks with score 0
    if (score==0) { 
      ATH_MSG_DEBUG ("Candidate score is zero, reject it");
      // statistic
      stat.incrementCounterByRegion(ECounter::kNcandScoreZero,pTrack);
      reject = true;
    } else {
      ATH_MSG_DEBUG ("Track Score is "<< score);
      // double track rejection
      if (m_dropDouble) {
        const auto & prds = m_assoTool->getPrdsOnTrack(prdToTrackMap, *pTrack);
        // unfortunately PrepRawDataSet is not a set !
        PrdSignature prdSig;
        prdSig.insert( prds.begin(),prds.end() );
        // we try to insert it into the set, if we fail (pair.second), it then exists already
        if ( !(prdSigSet.insert(prdSig)).second ) {
          ATH_MSG_DEBUG ("Double track, reject it !");
          // statistic
          stat.incrementCounterByRegion(ECounter::kNcandDouble,pTrack);
          reject = true;
        } else {
          ATH_MSG_DEBUG ("Insert new track in PrdSignatureSet");
        }
      }
    }
    if (!reject) {
      // add track to map, map is sorted small to big ! set if fitted
      ATH_MSG_VERBOSE ("Track  ("<< pTrack <<") has score "<<score);
      TrackPtr ptr(pTrack);
      if (!m_forceRefit) ptr.forceFitted();
      trackScoreTrackMap.insert( std::make_pair(-score,std::move(ptr)) );
    }
  }
  ATH_MSG_DEBUG ("Number of tracks in map:"<<trackScoreTrackMap.size());
}

//==================================================================================================

void Trk::SimpleAmbiguityProcessorTool::addTrack(Trk::Track* in_track,
                                                 const bool fitted,
                                                 TrackScoreMap &trackScoreTrackMap,
                                                 Trk::PRDtoTrackMap &prdToTrackMap,
                                                 std::vector<std::unique_ptr<const Trk::Track> >& cleanupTracks,
                                                 Counter &stat) const {
  std::unique_ptr<Trk::Track> atrack(in_track);
  // compute score
  TrackScore score;
  bool suppressHoleSearch = fitted ? m_suppressHoleSearch : true;
  if (m_trackSummaryTool.isEnabled()) {
     m_trackSummaryTool->computeAndReplaceTrackSummary(*atrack,&prdToTrackMap,suppressHoleSearch);
  }
  score = m_scoringTool->score( *atrack, suppressHoleSearch );
  // do we accept the track ?
  if (score!=0){
    ATH_MSG_DEBUG ("Track  ("<< atrack.get() <<") has score "<<score);
    // statistic
    stat.incrementCounterByRegion(ECounter::kNscoreOk,atrack.get());
    // add track to map, map is sorted small to big !
    trackScoreTrackMap.insert( std::make_pair(-score, TrackPtr(atrack.release(), fitted)) );
    return;
  }
  // do we try to recover the track ?
  if (score==0 && fitted && m_tryBremFit &&
    !atrack->info().trackProperties(Trk::TrackInfo::BremFit) &&
    atrack->trackParameters()->front()->pT() > m_pTminBrem &&
    (!m_caloSeededBrem || atrack->info().patternRecoInfo(Trk::TrackInfo::TrackInCaloROI))){
    ATH_MSG_DEBUG ("Track score is zero, try to recover it via brem fit");
    // run track fit using electron hypothesis
    std::unique_ptr<Trk::Track> bremTrack( m_fitterTool->fit(*atrack,true,Trk::electron) );
    if (!bremTrack){
      ATH_MSG_DEBUG ("Brem refit failed, drop track");
      // statistic
      stat.incrementCounterByRegion(ECounter::kNscoreZeroBremRefitFailed,atrack.get());
      stat.incrementCounterByRegion(ECounter::kNfailedFits,atrack.get());
      // clean up
      cleanupTracks.push_back(std::move(atrack));
    } else {
      // statistic
      stat.incrementCounterByRegion(ECounter::kNgoodFits,bremTrack.get());
      // rerun score
      if (m_trackSummaryTool.isEnabled()) {
        m_trackSummaryTool->computeAndReplaceTrackSummary(*bremTrack, &prdToTrackMap,suppressHoleSearch);
      }
      score = m_scoringTool->score( *bremTrack, suppressHoleSearch );
      // do we accept the track ?
      if (score!=0){
        ATH_MSG_DEBUG ("Brem refit successful, recovered track  ("<< atrack.get() <<") has score "<<score);
        // statistics
        stat.incrementCounterByRegion(ECounter::kNscoreZeroBremRefit,bremTrack.get());
        // add track to map, map is sorted small to big !
        trackScoreTrackMap.insert( std::make_pair(-score, TrackPtr(bremTrack.release(), fitted)) );
        return;
      } else {
        ATH_MSG_DEBUG ("Brem refit gave still track score zero, reject it");
        // statistic
        stat.incrementCounterByRegion(ECounter::kNscoreZeroBremRefitScoreZero,bremTrack.get());
      }
      cleanupTracks.push_back(std::move(atrack));
    }
  } else {
    ATH_MSG_DEBUG ("Track score is zero, reject it");
    // statistic
    stat.incrementCounterByRegion(ECounter::kNscoreZero,atrack.get());
    cleanupTracks.push_back(std::move(atrack));
  }
}
//==================================================================================================

TrackCollection *Trk::SimpleAmbiguityProcessorTool::solveTracks(TrackScoreMap& trackScoreTrackMap,
                                                                Trk::PRDtoTrackMap &prdToTrackMap,
                                                                std::vector<std::unique_ptr<const Trk::Track> >& cleanupTracks,
                                                                Counter &stat) const{
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
    auto [cleanedTrack_tmp,keep_orig] = m_selectionTool->getCleanedOutTrack( atrack.track() , -(ascore), prdToTrackMap);
    cleanedTrack.reset( cleanedTrack_tmp);
    // cleaned track is input track and fitted
    if (keep_orig && atrack.fitted() ){
      // track can be kept as is and is already fitted
      ATH_MSG_DEBUG ("Accepted track "<<atrack.track()<<"\t has score "<<-(ascore));
      // statistic
      stat.incrementCounterByRegion(ECounter::kNaccepted,atrack.track());
      if (m_tryBremFit && atrack->info().trackProperties(Trk::TrackInfo::BremFit)) stat.incrementCounterByRegion(ECounter::kNacceptedBrem,atrack.track());
      // add track to PRD_AssociationTool
      if (m_assoTool->addPRDs(prdToTrackMap, *atrack.track()).isFailure()) ATH_MSG_ERROR("addPRDs() failed" );
      // add to output list 
      finalTracks->push_back( const_cast<Track*>(atrack.release()) );
    } else if ( keep_orig ) {
      // don't forget to drop track from map
      // track can be kept as is, but is not yet fitted
      ATH_MSG_DEBUG ("Good track, but need to fit this track first, score, add it into map again and retry !");
      refitTrack(atrack.track(),trackScoreTrackMap, prdToTrackMap, cleanupTracks, stat);
      if (atrack.newTrack()) {
        cleanupTracks.push_back( std::unique_ptr<Trk::Track>(atrack.release()) );
      }
    // delete original copy
    } else if ( cleanedTrack ) {
      // now delete original track
      if (atrack.newTrack()) {
        cleanupTracks.push_back( std::unique_ptr<Trk::Track>(atrack.release()));
      }
      // don't forget to drop track from map
      // stripped down version should be reconsidered
      ATH_MSG_DEBUG ("Candidate excluded, add subtrack to map. Track "<<cleanedTrack.get());
      // statistic
      stat.incrementCounterByRegion(ECounter::kNsubTrack,cleanedTrack.get());
      // track needs fitting !
      addTrack( cleanedTrack.release(), false, trackScoreTrackMap, prdToTrackMap, cleanupTracks, stat);
    } else {
      // track should be discarded
      ATH_MSG_DEBUG ("Track "<< atrack.track() << " is excluded, no subtrack, reject");
      // statistic
      stat.incrementCounterByRegion(ECounter::kNnoSubTrack,atrack.track());
      if (atrack.newTrack()) {
        cleanupTracks.push_back(  std::unique_ptr<Trk::Track>(atrack.release()) );
      }
    // don't forget to drop track from map
    }
  }
  ATH_MSG_DEBUG ("Finished, number of track on output: "<<finalTracks->size());
  return finalTracks.release();
}

//==================================================================================================

void 
Trk::SimpleAmbiguityProcessorTool::refitTrack( const Trk::Track* track,
                                                    TrackScoreMap& trackScoreTrackMap,
                                                    Trk::PRDtoTrackMap &prdToTrackMap,
                                                    std::vector<std::unique_ptr<const Trk::Track> >& cleanupTracks,
                                                    Counter &stat) const{
  std::unique_ptr<Trk::Track> newTrack;
  if (!m_suppressTrackFit) {
    if (m_refitPrds) {
      // simple case, fit PRD directly
      ATH_MSG_VERBOSE ("Refit track "<<track<<" from PRDs");
      newTrack.reset( refitPrds (track, prdToTrackMap,stat) );
    } else {
      // ok, we fit ROTs
      ATH_MSG_VERBOSE ("Refit track "<<track<<" from ROTs");
      newTrack.reset( refitRots (track,stat) );
    }
  }else{
    double reXi2 = 0.; 
    int nDF = 0;
    const DataVector<const TrackStateOnSurface>* tsos = track->trackStateOnSurfaces();
    DataVector<const TrackStateOnSurface>* vecTsos = new DataVector<const TrackStateOnSurface>();
    // loop over TSOS, copy TSOS and push into vector
    DataVector<const TrackStateOnSurface>::const_iterator iTsos    = tsos->begin();
    DataVector<const TrackStateOnSurface>::const_iterator iTsosEnd = tsos->end(); 
    for ( ; iTsos != iTsosEnd ; ++iTsos)  {
      const TrackStateOnSurface* newTsos = new TrackStateOnSurface(**iTsos);
      vecTsos->push_back(newTsos);
      if((*iTsos)->type(Trk::TrackStateOnSurface::Measurement)){  //Get the chi2 and number of hits
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
    newTrack = std::make_unique<Trk::Track>( info, vecTsos, fq );
  }
  if (newTrack){
    ATH_MSG_DEBUG ("New track successfully fitted"<<newTrack.get());
    addTrack( newTrack.release(), true, trackScoreTrackMap, prdToTrackMap, cleanupTracks, stat);
  } else {
   ATH_MSG_DEBUG ("Fit failed !");
  }  
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
  if (m_tryBremFit && track->info().trackProperties(Trk::TrackInfo::BremFit)){
    // statistics
    stat.incrementCounterByRegion(ECounter::kNbremFits,track);
    ATH_MSG_VERBOSE ("Brem track, refit with electron brem fit");
    newTrack = m_fitterTool->fit(prds, *par, true, Trk::electron);
  } else {
    // statistics
    stat.incrementCounterByRegion(ECounter::kNfits,track);
    ATH_MSG_VERBOSE ("Normal track, refit");
    newTrack = m_fitterTool->fit(prds, *par, true, m_particleHypothesis);
    if (!newTrack && m_tryBremFit && par->pT() > m_pTminBrem &&
        (!m_caloSeededBrem || track->info().patternRecoInfo(Trk::TrackInfo::TrackInCaloROI))){
      // statistics
      stat.incrementCounterByRegion(ECounter::kNrecoveryBremFits,track);
      ATH_MSG_VERBOSE ("Normal fit failed, try brem recovery");
      newTrack = m_fitterTool->fit(prds, *par, true, Trk::electron);
    }
  }
  if(newTrack){
    // statistic
    stat.incrementCounterByRegion(ECounter::kNgoodFits,newTrack);
    //keeping the track of previously accumulated TrackInfo
    const Trk::TrackInfo& originalInfo = track->info();
    newTrack->info().addPatternReco(originalInfo);
  } else {
    // statistic
    stat.incrementCounterByRegion(ECounter::kNfailedFits,track);
  }
  return newTrack;
}

//==================================================================================================

Trk::Track* Trk::SimpleAmbiguityProcessorTool::refitRots( const Trk::Track* track,
                                                          Counter &stat) const {
  ATH_MSG_VERBOSE ("Refit track "<<track);
  // refit using first parameter, do outliers
  Trk::Track* newTrack = nullptr;
  if (m_tryBremFit && track->info().trackProperties(Trk::TrackInfo::BremFit)) {
    // statistics
    stat.incrementCounterByRegion(ECounter::kNbremFits,track);
    ATH_MSG_VERBOSE ("Brem track, refit with electron brem fit");
    newTrack = m_fitterTool->fit(*track, true, Trk::electron);
  } else {
    // statistics
    stat.incrementCounterByRegion(ECounter::kNfits,track);
    ATH_MSG_VERBOSE ("Normal track, refit");
    newTrack = m_fitterTool->fit(*track, true, m_particleHypothesis);
    if (!newTrack && m_tryBremFit &&
      track->trackParameters()->front()->pT() > m_pTminBrem &&
      (!m_caloSeededBrem || track->info().patternRecoInfo(Trk::TrackInfo::TrackInCaloROI))){
      // statistics
      stat.incrementCounterByRegion(ECounter::kNrecoveryBremFits,track);

      ATH_MSG_VERBOSE ("Normal fit failed, try brem recovery");
      newTrack = m_fitterTool->fit(*track, true, Trk::electron);
    }
  }
  if(newTrack){
      // statistic
      stat.incrementCounterByRegion(ECounter::kNgoodFits,newTrack);
      //keeping the track of previously accumulated TrackInfo
      const Trk::TrackInfo& originalInfo = track->info();
      newTrack->info().addPatternReco(originalInfo);
    } else {
      // statistic
      stat.incrementCounterByRegion(ECounter::kNfailedFits,track);
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
  out << m_stat.dumpRegions(   "  Number of candidates at input   :", ECounter::kNcandidates);
  out << m_stat.dumpRegions(   "  - candidates rejected score 0   :", ECounter::kNcandScoreZero);
  out << m_stat.dumpRegions(   "  - candidates rejected as double :", ECounter::kNcandDouble);
  out <<             "---------------------------------------------------------------------------------" << "\n";
  out << m_stat.dumpRegions(   "  candidates with good score      :", ECounter::kNscoreOk);
  if (m_tryBremFit) {
     out << m_stat.dumpRegions("  + recovered after brem refit    :", ECounter::kNscoreZeroBremRefit);
  }
  out << m_stat.dumpRegions(   "  candidates rejected score 0     :", ECounter::kNscoreZero);
  if (m_tryBremFit) {
     out << m_stat.dumpRegions("  + rejected failed brem refit    :", ECounter::kNscoreZeroBremRefitFailed);
  }
  out <<             "---------------------------------------------------------------------------------" << "\n";
  out << m_stat.dumpRegions(   "  number of normal fits           :", ECounter::kNfits);
  if (m_tryBremFit) {
     out << m_stat.dumpRegions("  + 2nd brem fit for failed fit   :", ECounter::kNrecoveryBremFits);
     out << m_stat.dumpRegions("  normal brem fits for electrons  :", ECounter::kNbremFits);
  }
  out <<             "---------------------------------------------------------------------------------" << "\n";
  out << m_stat.dumpRegions(   "  sum of succesful fits           :", ECounter::kNgoodFits);
  out << m_stat.dumpRegions(   "  sum of failed fits              :", ECounter::kNfailedFits);
  out <<             "---------------------------------------------------------------------------------" << "\n";
  out << m_stat.dumpRegions(   "  Number of subtracks created     :", ECounter::kNsubTrack);
  out << m_stat.dumpRegions(   "  Number of candidates excluded   :", ECounter::kNnoSubTrack);
  out <<             "---------------------------------------------------------------------------------" << "\n";
  out << m_stat.dumpRegions(   "  Number of tracks accepted       :", ECounter::kNaccepted);
  if (m_tryBremFit) {
     out << m_stat.dumpRegions("  including number of brem fits   :", ECounter::kNacceptedBrem);
  }
  out <<             "---------------------------------------------------------------------------------" << "\n";
  out << std::setiosflags(std::ios::fixed | std::ios::showpoint) << std::setprecision(2)
      <<             "    definition: ( 0.0 < Barrel < " << m_etabounds[Counter::iBarrel] << " < Transition < " << m_etabounds[Counter::iTransi]
      <<             " < Endcap < " << m_etabounds[Counter::iEndcap] << " DBM )" << "\n";
  out <<             "-------------------------------------------------------------------------------" << "\n";
  out.precision (ss);
}





