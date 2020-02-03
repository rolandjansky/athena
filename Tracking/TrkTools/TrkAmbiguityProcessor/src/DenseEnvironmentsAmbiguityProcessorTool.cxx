/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "DenseEnvironmentsAmbiguityProcessorTool.h"
#include "TrackScoringTool.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "TrkTrack/TrackCollection.h"
#include "GaudiKernel/MsgStream.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrkTrack/TrackInfo.h"
#include "InDetRecToolInterfaces/IPixelClusterSplitProbTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkCaloClusterROI/CaloClusterROI_Collection.h"

#include <map>
#include <ext/functional>
#include <iterator>
#include "TString.h"

#include "InDetPrepRawData/PixelCluster.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetIdentifier/PixelID.h"

//TODO: to be improved
bool Trk::DenseEnvironmentsAmbiguityProcessorTool::_checkTrack( const Trk::Track *track) const {
	  if (!track )return true;
	
	  bool error=false;
	  if (track->trackParameters()){
	    int counter=0;
	    for (const  Trk::TrackParameters *param: *(track->trackParameters())) {
	      if (param && param->covariance() && param->covariance()->determinant() < 0) {
	        ATH_MSG_DEBUG( " negative determinant for track param " << counter << " "
	                       << *(param)  << " cov=" << *(param->covariance())
	                       << std::endl
	                       << " det=" << param->covariance()->determinant() );
	        error=true;
	      }
	      ++counter;
	      if (counter>=2) break;
	    }
	  }
	  return !error;
}


//==================================================================================================
Trk::DenseEnvironmentsAmbiguityProcessorTool::DenseEnvironmentsAmbiguityProcessorTool(const std::string& t, 
                                const std::string& n,
                                const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_particleHypothesis{undefined},
  m_scoringTool("Trk::TrackScoringTool/TrackScoringTool"),
  m_extrapolatorTool("Trk::Extrapolator/AtlasExtrapolator"),
  m_selectionTool("InDet::InDetDenseEnvAmbiTrackSelectionTool/InDetAmbiTrackSelectionTool"),
  m_etabounds{0.8, 1.6, 2.5, 4.0},
  m_stat(m_etabounds)
{
  // statitics stuff

  m_fitterTool.push_back("Trk::KalmanFitter/InDetTrackFitter");

  declareInterface<ITrackAmbiguityProcessorTool>(this);
  declareProperty("RefitPrds"            , m_refitPrds          = true); //  True to allow for updated NN information to be taken into account
  declareProperty("applydRcorrection"    , m_applydRcorrection  = false);
  declareProperty("MatEffects"           , m_matEffects         = 3); // pion
  declareProperty("ScoringTool"          , m_scoringTool);
  declareProperty("SelectionTool"        , m_selectionTool);
  declareProperty("Fitter"               , m_fitterTool );
  declareProperty("TrackExtrapolator"    , m_extrapolatorTool);
  declareProperty("SuppressHoleSearch"   , m_suppressHoleSearch = false);
  declareProperty("SuppressTrackFit"     , m_suppressTrackFit   = false);
  declareProperty("ForceRefit"           , m_forceRefit         = true);
  declareProperty("tryBremFit"           , m_tryBremFit         = false);
  declareProperty("caloSeededBrem"       , m_caloSeededBrem     = false);
  declareProperty("pTminBrem"            , m_pTminBrem          = 1000.);
  declareProperty("etaBounds"            , m_etabounds,"eta intervals for internal monitoring");

  //To determine the ROI for high pt Bs
  declareProperty("doHadCaloSeed"        ,m_useHClusSeed = false );
  declareProperty("minPtBjetROI"         ,m_minPtBjetROI = 15000.); //inMeV
  declareProperty("phiWidth"             ,m_phiWidth     = 0.1   );
  declareProperty("etaWidth"             ,m_etaWidth     = 0.1   );
  declareProperty("InputHadClusterContainerName",m_inputHadClusterContainerName="InDetHadCaloClusterROIs");

}
//==================================================================================================

Trk::DenseEnvironmentsAmbiguityProcessorTool::~DenseEnvironmentsAmbiguityProcessorTool()
{
}
//==================================================================================================

StatusCode Trk::DenseEnvironmentsAmbiguityProcessorTool::initialize()
{
  StatusCode sc = StatusCode::SUCCESS;

  ATH_CHECK( m_scoringTool.retrieve());

  ATH_CHECK( m_assoMapName.initialize(!m_assoMapName.key().empty()));
  ATH_CHECK( m_assoTool.retrieve() );
  ATH_CHECK( m_trackSummaryTool.retrieve( DisableTool{ m_trackSummaryTool.name().empty() } ) );

  ATH_CHECK( m_selectionTool.retrieve());
  ATH_CHECK( m_fitterTool.retrieve());
  if (m_fitterTool.empty()){
    ATH_MSG_FATAL("Failed to retrieve tool " << m_fitterTool );
    sc = StatusCode::FAILURE;
    return sc;
   }

  ATH_CHECK( m_extrapolatorTool.retrieve());
  
  sc = detStore()->retrieve(m_pixelId, "PixelID");
  if (sc.isFailure())
  {
    ATH_MSG_FATAL( "Could not get PixelID helper !" );
    return StatusCode::FAILURE;
  }

  sc = detStore()->retrieve(m_idHelper, "AtlasID");
  if (sc.isFailure())
  {
    ATH_MSG_FATAL( "Could not get AtlasDetectorID helper" );
    return StatusCode::FAILURE;
  }
  
  // Configuration of the material effects
  Trk::ParticleSwitcher particleSwitch;
  m_particleHypothesis = particleSwitch.particle[m_matEffects];

  // brem fitting enabled ?
  if (m_tryBremFit)
    ATH_MSG_INFO( "Try brem fit and recovery for electron like tracks." );

  //Initialise the ROI tool
  ATH_CHECK(m_inputHadClusterContainerName.initialize(m_useHClusSeed));

  if(!m_dRMap.key().empty()){
     ATH_CHECK(m_dRMap.initialize() );
  }

  if (m_stat.etaBounds().size() != TrackStat::kNStatRegions-1) {
     ATH_MSG_FATAL("There must be exactly " << (TrackStat::kNStatRegions-1) << " eta bounds but "
                   << m_stat.etaBounds().size() << " are set." );
     return StatusCode::FAILURE;
  }
  return sc;
}
//==================================================================================================

StatusCode Trk::DenseEnvironmentsAmbiguityProcessorTool::finalize()
{
  return StatusCode::SUCCESS;
}

void Trk::DenseEnvironmentsAmbiguityProcessorTool::statistics()
{
  if (msgLvl(MSG::INFO)) {
     MsgStream &out=msg(MSG::INFO);
     out << " -- statistics " << std::endl;
     std::lock_guard<std::mutex> lock( m_statMutex );
     m_stat.dump(out, m_tryBremFit);
     out << endmsg;
  }
  }

void Trk::DenseEnvironmentsAmbiguityProcessorTool::TrackStat::dump(MsgStream &out, bool try_brem_fit) const
{
   // @TODO restore ios
   std::streamsize ss = std::cout.precision();
   int iw=9;
   out << "------------------------------------------------------------------------------------" << std::endl;
   out << "  Number of events processed      :   "<< m_globalCounter[kNevents].value() << std::endl;
   if (m_globalCounter[kNInvalidTracks]>0) {
      out << "  Number of invalid tracks        :   "<< m_globalCounter[kNInvalidTracks].value() << std::endl;
   }
   if (m_globalCounter[kNTracksWithoutParam]>0) {
      out << "  Tracks without parameters       :   "<< m_globalCounter[kNTracksWithoutParam].value() << std::endl;
   }
   out << "  statistics by eta range          ------All---Barrel---Trans.-- Endcap-- Forwrd-- " << std::endl;
   out << "------------------------------------------------------------------------------------" << std::endl;
   dumpStatType(out, "  Number of candidates at input   :",    kNcandidates,iw);
   //   dumpStatType(out, "  - candidates rejected score 0   :",    kNcandScoreZero,iw);
   //   dumpStatType(out, "  - candidates rejected as double :",    kNcandDouble,iw);
   out << "------------------------------------------------------------------------------------" << std::endl;
   dumpStatType(out, "  candidates with good score      :",    kNscoreOk,iw);
   if (try_brem_fit) {
      dumpStatType(out, "  + recovered after brem refit    :", kNscoreZeroBremRefit,iw);
   }
   dumpStatType(out, "  candidates rejected score 0     :",    kNscoreZero,iw);
   if (try_brem_fit) {
      dumpStatType(out, "  + m refit                       :", kNscoreZeroBremRefitFailed,iw);
      dumpStatType(out, "  + rejected brem refit score 0   :", kNscoreZeroBremRefitScoreZero,iw);
   }
   out << "------------------------------------------------------------------------------------" << std::endl;
   dumpStatType(out, "  number of normal fits           :" ,   kNfits,iw);
   if (try_brem_fit) {
      dumpStatType(out, "  + 2nd brem fit for failed fit   :", kNrecoveryBremFits,iw);
      dumpStatType(out, "  normal brem fits for electrons  :", kNbremFits,iw);
   }
   out << "------------------------------------------------------------------------------------" << std::endl;
   dumpStatType(out, "  sum of succesful fits           :",    kNgoodFits,iw);
   dumpStatType(out, "  sum of failed fits              :",    kNfailedFits,iw);
   out << "------------------------------------------------------------------------------------" << std::endl;
   dumpStatType(out, "  Number of subtracks created     :",    kNsubTrack,iw);
   dumpStatType(out, "  Number of candidates excluded   :",    kNnoSubTrack,iw);
   out << "------------------------------------------------------------------------------------" << std::endl;
   dumpStatType(out, "  Number of tracks accepted       :",    kNaccepted,iw);
   if (try_brem_fit) {
      dumpStatType(out, "  including number of brem fits   :", kNacceptedBrem,iw);
   }
   out << "------------------------------------------------------------------------------------" << std::endl;
   out << std::setiosflags(std::ios::fixed | std::ios::showpoint) << std::setprecision(2)
       << "    definition: ( 0.0 < Barrel < " << (*m_etabounds)[iBarrel-1] << " < Transition < " << (*m_etabounds)[iTransi-1]
       << " < Endcap < " << (*m_etabounds)[iEndcap-1] << " < Forward < " << (*m_etabounds)[iForwrd-1] << " )" << std::endl;
   out << "------------------------------------------------------------------------------------" << std::endl;
   out << std::setprecision(ss);
  }


//==================================================================================================


/** Do actual processing of event. Takes a track container, 
    and then returns the tracks which have been selected*/

TrackCollection* Trk::DenseEnvironmentsAmbiguityProcessorTool::process(const TracksScores *trackScoreTrackMap) const
{
  if (!trackScoreTrackMap) return nullptr;
  // clear prdAssociationTool via selection tool

  // @TODO remove :
  std::unique_ptr<Trk::PRDtoTrackMap> prd_to_track_map( m_assoTool->createPRDtoTrackMap() );
  if (!m_assoMapName.key().empty()) {
     SG::ReadHandle<Trk::PRDtoTrackMap> input_prd_map(m_assoMapName);
     if (!input_prd_map.isValid()) {
        ATH_MSG_ERROR("Failed to retrieve prd to track map " << m_assoMapName.key() );
     }
     else {
        //  *prd_to_track_map = *input_prd_map;
     }
  }
  std::vector<std::unique_ptr<const Trk::Track> > cleanup_tracks;

  reloadHadROIs();

  // going to do simple algorithm for now:
  // - take track with highest score
  // - remove shared hits from all other tracks
  // - take next highest scoring tracks, and repeat 

  ATH_MSG_DEBUG ("Solving Tracks");

  TrackCollection* finalTracks = new TrackCollection;
  {
     TrackStat stat(m_stat.etaBounds());
     stat.newEvent();
     solveTracks(*trackScoreTrackMap, *prd_to_track_map, *finalTracks, cleanup_tracks,stat);
     {
        std::lock_guard<std::mutex> lock(m_statMutex);
        m_stat += stat;
     }
  }
  if(m_applydRcorrection)
  {
    ATH_MSG_ERROR("applydRcorrection is going to be removed.");
  }

  return finalTracks;
}


//==================================================================================================
// taking ownership of input track
void Trk::DenseEnvironmentsAmbiguityProcessorTool::addTrack(Trk::Track* track, const bool fitted,
                                                            std::multimap<float, TrackPtr > &scoreTrackFitflagMap,
                                                            const Trk::PRDtoTrackMap &prd_to_track_map,
                                                            std::vector<std::unique_ptr<const Trk::Track> >& cleanup_tracks,
                                                            Trk::DenseEnvironmentsAmbiguityProcessorTool::TrackStat &stat) const
{
  // compute score
  TrackScore score;
  bool suppressHoleSearch = fitted ? m_suppressHoleSearch : true;

  ATH_MSG_DEBUG ("addTrack()::Fitted             "<< fitted);
  if (m_trackSummaryTool.isEnabled()) {
     m_trackSummaryTool->computeAndReplaceTrackSummary(*track,
                                                       &prd_to_track_map,
                                                       suppressHoleSearch);
  }

  // @TODO create track summary for track
  score = m_scoringTool->score( *track, suppressHoleSearch );

  // do we accept the track ?
  if (score!=0)
  {
    ATH_MSG_DEBUG ("Track  ("<< track <<") has score "<<score);
    // add track to map, map is sorted small to big !
    scoreTrackFitflagMap.emplace(-score, TrackPtr(track, fitted) );
    return;
  }

  // do we try to recover the track ?
  if (score==0 && fitted && m_tryBremFit &&
      !track->info().trackProperties(Trk::TrackInfo::BremFit) &&
      track->trackParameters()->front()->pT() > m_pTminBrem &&
      (!m_caloSeededBrem || track->info().patternRecoInfo(Trk::TrackInfo::TrackInCaloROI)))
  {

    ATH_MSG_DEBUG ("Track score is zero, try to recover it via brem fit");
    // run track fit using electron hypothesis
    Trk::Track* bremTrack = fit(*track,true,Trk::electron);

    if (!bremTrack)
    {
      ATH_MSG_DEBUG ("Brem refit failed, drop track");
      stat.increment_by_eta(TrackStat::kNscoreZeroBremRefitFailed,track);
      stat.increment_by_eta(TrackStat::kNfailedFits,track);

      // clean up
      cleanup_tracks.push_back(std::unique_ptr<const Trk::Track>(track) );
      track=nullptr;
      
    }
    else
    {
      if (m_trackSummaryTool.isEnabled()) {
         m_trackSummaryTool->computeAndReplaceTrackSummary(*bremTrack,
                                                           &prd_to_track_map,
                                                           m_suppressHoleSearch);
      }

      stat.increment_by_eta(TrackStat::kNgoodFits,bremTrack);

      // rerun score
      score = m_scoringTool->score( *bremTrack, suppressHoleSearch );

      cleanup_tracks.push_back(std::unique_ptr<const Trk::Track>(track) );
      track=nullptr;

      // do we accept the track ?
      if (score!=0)
      {
        ATH_MSG_DEBUG ("Brem refit successful, recovered track  ("<< track <<") has score "<<score);
        stat.increment_by_eta(TrackStat::kNscoreZeroBremRefit,bremTrack);

        // add track to map, map is sorted small to big !
        scoreTrackFitflagMap.emplace( -score, TrackPtr(bremTrack, true) );
        return;
      }
      else
      {
        ATH_MSG_DEBUG ("Brem refit gave still track score zero, reject it");
        stat.increment_by_eta(TrackStat::kNscoreZeroBremRefitScoreZero,bremTrack);

        // clean up
        cleanup_tracks.push_back(std::unique_ptr<const Trk::Track>(bremTrack) );
      }
    }
  }
  else  
  {
    ATH_MSG_DEBUG ("Track score is zero, reject it");

    stat.increment_by_eta(TrackStat::kNscoreZero,track);

    // @TODO can delete this track ?
    cleanup_tracks.push_back(std::unique_ptr<const Trk::Track>(track) );
  }
  }
//==================================================================================================


void Trk::DenseEnvironmentsAmbiguityProcessorTool::solveTracks(const TracksScores &trackScoreTrackMap,
                                                               Trk::PRDtoTrackMap &prd_to_track_map,
                                                               TrackCollection &finalTracks,
                                                               std::vector<std::unique_ptr<const Trk::Track> > &cleanup_tracks,
                                                               Trk::DenseEnvironmentsAmbiguityProcessorTool::TrackStat &stat) const
{

  std::multimap<float, TrackPtr  > scoreTrackFitflagMap;
  for(const std::pair< const Trk::Track *, float> &scoreTrack: trackScoreTrackMap){
     scoreTrackFitflagMap.emplace(scoreTrack.second, TrackPtr(scoreTrack.first) );
     stat.increment_by_eta(TrackStat::kNcandidates,scoreTrack.first);
  }

  ATH_MSG_DEBUG ("Starting to solve tracks");

  // now loop as long as map is not empty
  while ( !scoreTrackFitflagMap.empty() )
  {
    // get current best candidate 
    std::multimap<float, TrackPtr >::iterator itnext = scoreTrackFitflagMap.begin();
    TrackPtr atrack( std::move(itnext->second) );
    float ascore =  itnext->first;

    scoreTrackFitflagMap.erase(itnext);

    // clean it out to make sure not to many shared hits
    ATH_MSG_DEBUG ("--- Trying next track "<<atrack.track()<<"\t with score "<<-ascore);
    std::unique_ptr<Trk::Track> cleanedTrack;
    auto [cleanedTrack_tmp, keep_orig] = m_selectionTool->getCleanedOutTrack( atrack.track() , -ascore, prd_to_track_map);
    cleanedTrack.reset(cleanedTrack_tmp);
    ATH_MSG_DEBUG ("--- cleand next track "<< cleanedTrack.get());


    // cleaned track is input track and fitted
    if (keep_orig && atrack.fitted())
    {
      // track can be kept as is and is already fitted
       ATH_MSG_DEBUG ("Accepted track "<<atrack.track()<<"\t has score "<<-ascore);
       stat.increment_by_eta(TrackStat::kNaccepted, atrack.track() );
       if (m_tryBremFit && atrack.track()->info().trackProperties(Trk::TrackInfo::BremFit)) {
          stat.increment_by_eta(TrackStat::kNacceptedBrem,atrack.track());
       }

      //Compute the fitQuality
      double fitQual = 0;
      if  (atrack->fitQuality() && atrack->fitQuality()->numberDoF()>0 ){ 
         fitQual = atrack->fitQuality()->chiSquared()/atrack->fitQuality()->numberDoF();
      }

      if(fitQual > 1.3 && decideIfInHighPtBROI(atrack.track())){
        std::unique_ptr<Trk::Track> refittedTrack( refitTracksFromB(atrack.track(), fitQual)); //Q: Is there the case  atrack == refittedTrack ?
        if(refittedTrack){
          // add track to PRD_AssociationTool
          StatusCode sc = m_assoTool->addPRDs(prd_to_track_map, *refittedTrack);
          if (sc.isFailure()) ATH_MSG_ERROR( "addPRDs() failed" );
          // add to output list
          finalTracks.push_back( refittedTrack.release() );
          if (atrack.newTrack()) {
             cleanup_tracks.push_back(std::unique_ptr<const Trk::Track>(atrack.release()) );
          }
        }
        else{
          // add track to PRD_AssociationTool
          StatusCode sc = m_assoTool->addPRDs(prd_to_track_map, *atrack);
          if (sc.isFailure()) ATH_MSG_ERROR( "addPRDs() failed" );
          // add to output list 
          finalTracks.push_back( atrack.release() );
        }
      }
      else {
        // add track to PRD_AssociationTool
        StatusCode sc = m_assoTool->addPRDs(prd_to_track_map, *atrack);
        if (sc.isFailure()) ATH_MSG_ERROR( "addPRDs() failed" );
        // add to output list 
        finalTracks.push_back( atrack.release() );
      }
    }
    else if ( keep_orig)
    {
      // track can be kept as is, but is not yet fitted
      ATH_MSG_DEBUG ("Good track("<< atrack.track() << ") but need to fit this track first, score, add it into map again and retry ! ");
      Trk::Track *refittedTrack = refitTrack(atrack.track(),prd_to_track_map, stat);
      if(refittedTrack) {
         addTrack( refittedTrack, true , scoreTrackFitflagMap, prd_to_track_map, cleanup_tracks, stat);
      }

      // remove original copy, but delay removal since some pointer to it or its constituents may still be in used
      if (atrack.newTrack()) {
         cleanup_tracks.push_back(std::unique_ptr<const Trk::Track>(atrack.release()) );
      }
    }
    else if ( cleanedTrack ) //cleanedTrack != atrack
    {
      ATH_MSG_DEBUG ("Candidate excluded, add subtrack to map. Track "<<cleanedTrack.get());
      stat.increment_by_eta(TrackStat::kNsubTrack,cleanedTrack.get());

      // for this case clenedTrack is a new created object.
      addTrack(cleanedTrack.release(), false, scoreTrackFitflagMap, prd_to_track_map, cleanup_tracks, stat);

      // remove original copy, but delay removal since some pointer to it or its constituents may still be in used
      if (atrack.newTrack()) {
         cleanup_tracks.push_back(std::unique_ptr<const Trk::Track>(atrack.release()) );
      }
    }
    else
    {
      // track should be discarded
      ATH_MSG_DEBUG ("Track "<< atrack.track() << " is excluded, no subtrack, reject");
      stat.increment_by_eta(TrackStat::kNnoSubTrack,atrack.track());

      // remove original copy, but delay removal since some pointer to it or its constituents may still be in used
      if (atrack.newTrack()) {
         cleanup_tracks.push_back(std::unique_ptr<const Trk::Track>(atrack.release()) );
      }
    }
  }

  ATH_MSG_DEBUG ("Finished, number of track on output: "<<finalTracks.size());
}



//==================================================================================================
Trk::Track* Trk::DenseEnvironmentsAmbiguityProcessorTool::refitTrack( const Trk::Track* track,
                                                                      Trk::PRDtoTrackMap &prd_to_track_map,
                                                                      Trk::DenseEnvironmentsAmbiguityProcessorTool::TrackStat &stat) const
{
  Trk::Track* newTrack = nullptr;
  if (!m_suppressTrackFit){
    if (m_refitPrds) 
    {
      // simple case, fit PRD directly
      ATH_MSG_VERBOSE ("Refit track "<<track<<" from PRDs");
      newTrack = refitPrds (track,prd_to_track_map, stat);
    }
    else 
    {
      // ok, we fit ROTs
      ATH_MSG_VERBOSE ("Refit track "<<track<<" from ROTs");
      newTrack = refitRots (track,stat);
    }
  } else {
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

    newTrack = new Trk::Track(info, vecTsos, fq);  
  }
  
  if (newTrack!=nullptr) 
  {
    ATH_MSG_DEBUG ("New track "<<newTrack<<" successfully fitted from "<<track);
  }
  else { 
    ATH_MSG_DEBUG ("Fit failed !");
  }  
  
  return newTrack;
}

//==================================================================================================

Trk::Track* Trk::DenseEnvironmentsAmbiguityProcessorTool::refitPrds( const Trk::Track* track,
                                                                     Trk::PRDtoTrackMap &prd_to_track_map,
                                                                     Trk::DenseEnvironmentsAmbiguityProcessorTool::TrackStat &stat) const
{

  // get vector of PRDs
  // @TODO ensured that prds on track are registered for this track ?
   std::vector<const Trk::PrepRawData*> prds = m_assoTool->getPrdsOnTrack(prd_to_track_map,*track);

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

  if (m_tryBremFit && track->info().trackProperties(Trk::TrackInfo::BremFit))
  {

    stat.increment_by_eta(TrackStat::kNbremFits,track);

    ATH_MSG_VERBOSE ("Brem track, refit with electron brem fit");
    // TODO revert once GlobalChi2Fitter properly handles brem fits when 
    //      starting from prds
    // newTrack = fit(prds, *par, true, Trk::electron);
    newTrack = fit(*track, true, Trk::electron);

  }
  else
  {
    stat.increment_by_eta(TrackStat::kNfits,track);

    ATH_MSG_VERBOSE ("Normal track, refit");
    newTrack = fit(prds, *par, true, m_particleHypothesis);

    if (!newTrack && m_tryBremFit && par->pT() > m_pTminBrem &&
  (!m_caloSeededBrem || track->info().patternRecoInfo(Trk::TrackInfo::TrackInCaloROI)))
    {
      stat.increment_by_eta(TrackStat::kNrecoveryBremFits,track);
      ATH_MSG_VERBOSE ("Normal fit failed, try brem recovery");
      // TODO revert once GlobalChi2Fitter properly handles brem fits when 
      //      starting from prds
      // newTrack = fit(prds, *par, true, Trk::electron);
      newTrack = fit(*track, true, Trk::electron);

    }
  }
  
  if(newTrack) {
    stat.increment_by_eta(TrackStat::kNgoodFits,newTrack);
    //keeping the track of previously accumulated TrackInfo
    const Trk::TrackInfo old_info = track->info();
    newTrack->info().addPatternReco(old_info);
  }
  else {
     stat.increment_by_eta(TrackStat::kNfailedFits,track);
  }
  return newTrack;
}

//==================================================================================================

Trk::Track* Trk::DenseEnvironmentsAmbiguityProcessorTool::refitRots(const Trk::Track* track,
                                                                    Trk::DenseEnvironmentsAmbiguityProcessorTool::TrackStat &stat) const
{

  ATH_MSG_VERBOSE ("Refit track "<<track);

  // refit using first parameter, do outliers
  Trk::Track* newTrack = nullptr;

  if (m_tryBremFit &&
      track->info().trackProperties(Trk::TrackInfo::BremFit))
  {
    stat.increment_by_eta(TrackStat::kNbremFits,track);
    ATH_MSG_VERBOSE ("Brem track, refit with electron brem fit");
    newTrack = fit(*track, true, Trk::electron);
  }
  else
  {

    stat.increment_by_eta(TrackStat::kNfits,track);
    ATH_MSG_VERBOSE ("Normal track, refit");
    newTrack = fit(*track, true, m_particleHypothesis);

    if (!newTrack && m_tryBremFit &&
        track->trackParameters()->front()->pT() > m_pTminBrem &&
        (!m_caloSeededBrem || track->info().patternRecoInfo(Trk::TrackInfo::TrackInCaloROI)))
    {
      stat.increment_by_eta(TrackStat::kNrecoveryBremFits,track);
      ATH_MSG_VERBOSE ("Normal fit failed, try brem recovery");
      newTrack = fit(*track, true, Trk::electron);
    }
  }

  if(newTrack)
  {
    stat.increment_by_eta(TrackStat::kNgoodFits,newTrack);
    //keeping the track of previously accumulated TrackInfo
    const Trk::TrackInfo old_info = track->info();
    newTrack->info().addPatternReco(old_info);
  }
  else {
    stat.increment_by_eta(TrackStat::kNfailedFits,track);
  }
  return newTrack;
}


//==================================================================================================

void Trk::DenseEnvironmentsAmbiguityProcessorTool::storeTrkDistanceMapdR( TrackCollection& tracks,
                                                                          std::vector<Trk::Track*> &refit_tracks_out )
{
  ATH_MSG_VERBOSE ("Creating track Distance dR map");
  SG::WriteHandle<InDet::DRMap> dRMapHandle (m_dRMap);
  dRMapHandle = std::make_unique<InDet::DRMap>();
  if ( !dRMapHandle.isValid() ){
    ATH_MSG_WARNING("Could not record Distance dR map.");
  } else{
    ATH_MSG_VERBOSE("Distance dR map recorded as '" << m_dRMap.key() <<"'.");
  }

  for (auto track : tracks){
      bool refit = false;
      const DataVector<const TrackStateOnSurface>* tsosVec = track->trackStateOnSurfaces();  
      if(!tsosVec){
        ATH_MSG_WARNING("TSOS vector does not exist");
        continue;   
      }  
      ATH_MSG_VERBOSE("---> Looping over TSOS's to allow  for cluster updates: "<< tsosVec->size() );
      for(auto tsos : *tsosVec){
          const MeasurementBase* measurement = tsos->measurementOnTrack(); 
          if(!measurement || ! tsos->trackParameters()){
            ATH_MSG_VERBOSE("---- TSOS has either no measurement or parameters: "<< measurement << "  " << tsos->trackParameters() );
            continue;           
          }
          
          if(!tsos->type(Trk::TrackStateOnSurface::Measurement)) {continue;}
          
          auto globalPosition = measurement->globalPosition();
          double radius = sqrt(globalPosition[0]*globalPosition[0]+globalPosition[1]*globalPosition[1]);
          // get the associated prd
          const Trk::RIO_OnTrack* rio = dynamic_cast<const Trk::RIO_OnTrack*> ( measurement );
          if(!rio){
              continue;
          }
          const InDet::PixelCluster* pixel = dynamic_cast<const InDet::PixelCluster*> ( rio->prepRawData() );
          // not pixel or not split
          if (!pixel || !pixel->isSplit() ) {continue ;}

          CylinderSurface iblSurface(radius,3000.0);
          
          const TrackParameters * trackParams = m_extrapolatorTool->extrapolate(*track,iblSurface);
          
          double yOnPix = trackParams->position().y();
          double zOnPix = trackParams->position().z();
          
          double Pi = acos(0);
          double twoPi = 2.*Pi;
          
          // now, find closest track  
          double dr = 0.; 
          double mindR = 99999999.;
          double mindX = 99999999.;
          double mindZ = 99999999.;
          
          for (auto track2 : tracks){
              if(track==track2) continue;
              float dEta = track->perigeeParameters()->momentum().eta() - track2->perigeeParameters()->momentum().eta();
              float dPhi2 = track->perigeeParameters()->momentum().phi() - track2->perigeeParameters()->momentum().phi();
              dr =  sqrtf(dEta*dEta + dPhi2*dPhi2);
              if(dr>0.4) continue;
              
              //extrapolation to pixel hit radius
              const TrackParameters * track2Params = m_extrapolatorTool->extrapolate(*track2,iblSurface);
          
              double y2OnPix = track2Params->position().y();
              double z2OnPix = track2Params->position().z();
              
              float dPhi = asin(yOnPix/radius) -asin(y2OnPix/radius);
              if (dPhi >= Pi) dPhi -= twoPi;
              if (dPhi < -Pi) dPhi += twoPi;
              
              double dx = fabs(radius*dPhi);
              double dz = fabs(zOnPix - z2OnPix);
              if(dx>mindX && dz>mindZ) continue;
              dr = sqrt(dx*dx + dz*dz);
              
              if(dr<mindR && dr > 1.e-4){
                  mindR = dr;
                  mindX = dx;
                  mindZ = dz;
              }
         }
         refit = true;
         std::pair<InDet::DRMap::iterator,bool> ret;
         std::pair<float,float> min (mindX, mindZ);
         ret = dRMapHandle->insert ( std::pair<const InDet::PixelCluster*,std::pair<float,float> >(pixel,min));
         // if we already have a dR for this prd, we update it, if current value is smaller
         if (!ret.second) {
            InDet::DRMap::iterator it;
            it = dRMapHandle->find(pixel);
            if(sqrt(pow((*it).second.first,2)+pow((*it).second.second,2)) > (float)mindR) {
                (*it).second.first  = (float)mindX;
                (*it).second.second = (float)mindZ;
	        }
         }
      }
      if(refit) refit_tracks_out.push_back(track);
  }
  }

//============================================================================================================
bool Trk::DenseEnvironmentsAmbiguityProcessorTool::decideIfInHighPtBROI(const Trk::Track* ptrTrack) const
{
  // Are we in a ROI?
  bool inROIandPTok(true);
  if(  ptrTrack->trackParameters()->front() ){
    if(  ptrTrack->trackParameters()->front()->pT() < m_minPtBjetROI ){
      inROIandPTok = false;
      return false;
    }
    if(inROIandPTok){

      bool inROI  = m_useHClusSeed && isHadCaloCompatible(*ptrTrack->trackParameters()->front());
      return inROI;
    }
    else
      return false;
  } else
    return false;
}

//============================================================================================================
bool Trk::DenseEnvironmentsAmbiguityProcessorTool::isHadCaloCompatible(const Trk::TrackParameters& Tp) const
{
  const double pi = M_PI;
  const double pi2 = 2.*M_PI;
    if(m_hadF.empty()) return false;
  auto f = m_hadF.begin();
  auto fe = m_hadF.end();
  auto e = m_hadE.begin();
  auto r = m_hadR.begin();
  auto z = m_hadZ.begin();
  
  double F = Tp.momentum().phi();
  double E = Tp.eta();
  
  for(; f!=fe; ++f) {
    double df = fabs(F-(*f));
    if(df > pi        ) df = fabs(pi2-df);
    if(df < m_phiWidth) {
      //Correct eta of cluster to take into account the z postion of the track
      double newZ   = *z - Tp.position().z();
      double newEta =  atanh( newZ / sqrt( (*r) * (*r) + newZ*newZ ) );
  
      double de = fabs(E-newEta);
      if(de < m_etaWidth) return true;
    }
    ++e;
    ++r;
    ++z;
  }
  return false;
}

//============================================================================================================
void Trk::DenseEnvironmentsAmbiguityProcessorTool::reloadHadROIs() const
{
  // turn into conditions algorithm
  if(m_useHClusSeed) {
    m_hadF.clear();
    m_hadE.clear();
    m_hadR.clear();
    m_hadZ.clear();
  
    SG::ReadHandle<CaloClusterROI_Collection> calo(m_inputHadClusterContainerName);
    for( const auto& ccROI : *calo) {
      m_hadF.push_back( ccROI->globalPosition().phi() );
      m_hadE.push_back( ccROI->globalPosition().eta() );
      m_hadR.push_back( ccROI->globalPosition().perp() );
      m_hadZ.push_back( ccROI->globalPosition().z() );
    }
  }
}
   
//============================================================================================================
void Trk::DenseEnvironmentsAmbiguityProcessorTool::removeInnerHits(std::vector<const Trk::MeasurementBase*>& measurements) const
{
  int count = 0; 
  for (size_t i=0; i < measurements.size(); ++i){
    const Trk::RIO_OnTrack* rio = dynamic_cast <const Trk::RIO_OnTrack*>(measurements.at(i));
    if (rio != nullptr) {
      const Identifier& surfaceID = (rio->identify()) ;                            
      if(m_idHelper->is_pixel(surfaceID) && count ==0){  
        //Only do this if we want to remove the pixel hits 
        const Identifier& id = m_pixelId->wafer_id(surfaceID);
        int layerDisk = m_pixelId -> layer_disk(id);
        if (layerDisk < 3){
          measurements.erase(measurements.begin()+i);
          break;
        }
        else{
          break;
        }
      }
      else 
        break;
    }
  }
}

//============================================================================================================
Trk::Track* Trk::DenseEnvironmentsAmbiguityProcessorTool::refitTracksFromB(const Trk::Track* track, double fitQualityOriginal) const
{
  const Trk::TrackParameters* par = track->perigeeParameters();
  if (par==nullptr) {
    par = track->trackParameters()->front();
    if (par==nullptr) {
      ATH_MSG_DEBUG ("Track ("<<track<<") has no Track Parameters ! No refit !");
      return nullptr;
    }
  }
  
  //std::vector<const Trk::MeasurementBase*>* measurementSet = new std::vector<const Trk::MeasurementBase*>;
  std::vector<const Trk::MeasurementBase*> measurementSet;
  //store all silicon measurements into the measurementset
  DataVector<const Trk::TrackStateOnSurface>::const_iterator trackStateOnSurface = track->trackStateOnSurfaces()->begin();
  for ( ; trackStateOnSurface != track->trackStateOnSurfaces()->end(); ++trackStateOnSurface ) {
    if ( !(*trackStateOnSurface) ){
      ATH_MSG_WARNING( "This track contains an empty MeasurementBase object that won't be included in the fit" );
      continue;
    }
    if ( (*trackStateOnSurface)->measurementOnTrack() ){
      if ( (*trackStateOnSurface)->type( Trk::TrackStateOnSurface::Measurement) ){
        const Trk::RIO_OnTrack* rio = dynamic_cast <const Trk::RIO_OnTrack*>( (*trackStateOnSurface)->measurementOnTrack() );
        if (rio != nullptr) {
          const Identifier& surfaceID = (rio->identify()) ;                            
          if(m_idHelper->is_pixel(surfaceID)|| m_idHelper->is_sct(surfaceID)) {
            measurementSet.push_back( (*trackStateOnSurface)->measurementOnTrack() );
          }
        }
      }
    }
  }

  size_t previousMeasSize = measurementSet.size();
  while (true){
    removeInnerHits(measurementSet); 
    if(measurementSet.size()>4){
      Trk::Track* refittedTrack = fit(measurementSet,*par,true,Trk::pion);
      double fitQualPostRefit = 10;
      if  (refittedTrack && refittedTrack->fitQuality() && refittedTrack->fitQuality()->numberDoF()!=0 ) 
        fitQualPostRefit = refittedTrack->fitQuality()->chiSquared()/refittedTrack->fitQuality()->numberDoF(); 
      if (fitQualityOriginal/fitQualPostRefit > 1){
        if ( fitQualityOriginal/fitQualPostRefit > 1.2){
          return refittedTrack;
        }
      }
      if (previousMeasSize == measurementSet.size()){
        return nullptr;
      }
      previousMeasSize = measurementSet.size();
    } else {
      //cannot refit the track because we do not have enough measurements
      return nullptr;
    }
  }
}
