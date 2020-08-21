/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "DenseEnvironmentsAmbiguityProcessorTool.h"
#include "TrackScoringTool.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "TrkTrack/TrackInfo.h"
#include "InDetRecToolInterfaces/IPixelClusterSplitProbTool.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkCaloClusterROI/CaloClusterROI_Collection.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetPrepRawData/SCT_Cluster.h"
#include "InDetIdentifier/PixelID.h"
#include "AmbiguityProcessorUtility.h"
#include <cmath>
#include <iterator>

//TODO: to be improved
bool 
Trk::DenseEnvironmentsAmbiguityProcessorTool::checkTrack( const Trk::Track *track) const {
	  if (!track )return true;
	  bool error=false;
	  if (track->trackParameters()){
	    int counter=0;
	    for (const  Trk::TrackParameters *param: *(track->trackParameters())) {
	      if (param && param->covariance() && param->covariance()->determinant() < 0) {
	        ATH_MSG_DEBUG( " negative determinant for track param " << counter << " "
	                       << *(param)  << " cov=" << *(param->covariance())
	                       << "\n"
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
  AmbiguityProcessorBase(t,n,p),
  m_extrapolatorTool("Trk::Extrapolator/AtlasExtrapolator"),
  m_selectionTool("InDet::InDetDenseEnvAmbiTrackSelectionTool/InDetAmbiTrackSelectionTool"){
  // statitics stuff

  m_fitterTool.push_back("Trk::KalmanFitter/InDetTrackFitter");

  declareInterface<ITrackAmbiguityProcessorTool>(this);
  declareProperty("RefitPrds"            , m_refitPrds          = true); //  True to allow for updated NN information to be taken into account
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
  declareProperty("etaBounds"            , m_etaBounds,"eta intervals for internal monitoring");

  //To determine the ROI for high pt Bs
  declareProperty("doHadCaloSeed"        ,m_useHClusSeed = false );
  declareProperty("minPtBjetROI"         ,m_minPtBjetROI = 15000.); //inMeV
  declareProperty("phiWidth"             ,m_phiWidth     = 0.1   );
  declareProperty("etaWidth"             ,m_etaWidth     = 0.1   );
  declareProperty("InputHadClusterContainerName",m_inputHadClusterContainerName="InDetHadCaloClusterROIs");

}
//==================================================================================================

Trk::DenseEnvironmentsAmbiguityProcessorTool::~DenseEnvironmentsAmbiguityProcessorTool(){
}
//==================================================================================================

StatusCode 
Trk::DenseEnvironmentsAmbiguityProcessorTool::initialize(){
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
  ATH_CHECK( detStore()->retrieve(m_pixelId, "PixelID"));
  ATH_CHECK( detStore()->retrieve(m_idHelper, "AtlasID"));
  
  // Configuration of the material effects
  Trk::ParticleSwitcher particleSwitch;
  m_particleHypothesis = particleSwitch.particle[m_matEffects];

  // brem fitting enabled ?
  if (m_tryBremFit)
    ATH_MSG_INFO( "Try brem fit and recovery for electron like tracks." );

  //Initialise the ROI tool
  ATH_CHECK(m_inputHadClusterContainerName.initialize(m_useHClusSeed));

  if (m_etaBounds.size() != Counter::nRegions) {
     ATH_MSG_FATAL("There must be exactly " << (Counter::nRegions) << " eta bounds but "
                   << m_etaBounds.size() << " are set." );
     return StatusCode::FAILURE;
  }
  ATH_MSG_INFO(m_fitterTool.size()<<" fitters was/were input");
  for(const auto & i:m_fitterTool){
    ATH_MSG_INFO(i.name());
  }
  return sc;
}
//==================================================================================================

StatusCode 
Trk::DenseEnvironmentsAmbiguityProcessorTool::finalize(){
  return StatusCode::SUCCESS;
}

void 
Trk::DenseEnvironmentsAmbiguityProcessorTool::statistics(){
  if (msgLvl(MSG::INFO)) {
     MsgStream &out=msg(MSG::INFO);
     out << " -- statistics \n";
     std::lock_guard<std::mutex> lock( m_statMutex );
     dumpStat(out);
     out << endmsg;
  }
}

//==================================================================================================
/** Do actual processing of event. Takes a track container, 
    and then returns the tracks which have been selected*/

TrackCollection* 
Trk::DenseEnvironmentsAmbiguityProcessorTool::process(const TracksScores *trackScoreTrackMap) const{
  if (!trackScoreTrackMap) return nullptr;
  // clear prdAssociationTool via selection tool
  // @TODO remove :
  std::unique_ptr<Trk::PRDtoTrackMap> prdToTrackMap( m_assoTool->createPRDtoTrackMap() );
  if (!m_assoMapName.key().empty()) {
     SG::ReadHandle<Trk::PRDtoTrackMap> input_prd_map(m_assoMapName);
     if (!input_prd_map.isValid()) {
        ATH_MSG_ERROR("Failed to retrieve prd to track map " << m_assoMapName.key() );
     }
  }
  std::vector<std::unique_ptr<const Trk::Track> > trackDustbin;
  reloadHadROIs();
  // going to do simple algorithm for now:
  // - take track with highest score
  // - remove shared hits from all other tracks
  // - take next highest scoring tracks, and repeat 
  ATH_MSG_DEBUG ("Solving Tracks");
  TrackCollection* finalTracks = new TrackCollection;
  {
     Counter stat(m_etaBounds);
     stat.newEvent();
     solveTracks(*trackScoreTrackMap, *prdToTrackMap, *finalTracks, trackDustbin,stat);
     {
        std::lock_guard<std::mutex> lock(m_statMutex);
        m_stat += stat;
     }
  }
  return finalTracks;
}



void 
Trk::DenseEnvironmentsAmbiguityProcessorTool::solveTracks(const TracksScores &trackScoreTrackMap,
                                                               Trk::PRDtoTrackMap &prdToTrackMap,
                                                               TrackCollection &finalTracks,
                                                               std::vector<std::unique_ptr<const Trk::Track> > &trackDustbin,
                                                               Counter &stat) const{
  TrackScoreMap scoreTrackFitflagMap;
  for(const std::pair< const Trk::Track *, float> &scoreTrack: trackScoreTrackMap){
     scoreTrackFitflagMap.emplace(scoreTrack.second, TrackPtr(scoreTrack.first) );
     stat.incrementCounterByRegion(CounterIndex::kNcandidates,scoreTrack.first);
  }
  ATH_MSG_DEBUG ("Starting to solve tracks");
  // now loop as long as map is not empty
  while ( !scoreTrackFitflagMap.empty() ){
    // get current best candidate 
    TrackScoreMap::iterator itnext = scoreTrackFitflagMap.begin();
    TrackPtr atrack( std::move(itnext->second) );
    float ascore =  itnext->first;
    scoreTrackFitflagMap.erase(itnext);
    // clean it out to make sure not to many shared hits
    ATH_MSG_DEBUG ("--- Trying next track "<<atrack.track()<<"\t with score "<<-ascore);
    std::unique_ptr<Trk::Track> cleanedTrack;
    const auto & [tmpCleanedTrack, keepOriginal] = m_selectionTool->getCleanedOutTrack( atrack.track() , -ascore, prdToTrackMap);
    cleanedTrack.reset(tmpCleanedTrack);
    ATH_MSG_DEBUG ("--- cleaned next track "<< cleanedTrack.get());
    // cleaned track is input track and fitted
    if (keepOriginal && atrack.fitted()){
      // track can be kept as is and is already fitted
      ATH_MSG_DEBUG ("Accepted track "<<atrack.track()<<"\t has score "<<-ascore);
      stat.incrementCounterByRegion(CounterIndex::kNaccepted, atrack.track() );
      if (m_tryBremFit && atrack.track()->info().trackProperties(Trk::TrackInfo::BremFit)) {
        stat.incrementCounterByRegion(CounterIndex::kNacceptedBrem,atrack.track());
      }
      //Compute the fitQuality
      const float fitQual = AmbiguityProcessor::calculateFitQuality(*atrack);
      if(fitQual > 1.3 && decideIfInHighPtBROI(atrack.track())){
        std::unique_ptr<Trk::Track> refittedTrack( refitTracksFromB(atrack.track(), fitQual)); //Q: Is there the case  atrack == refittedTrack ?
        if(refittedTrack){
          // add track to PRD_AssociationTool
          if ( m_assoTool->addPRDs(prdToTrackMap, *refittedTrack).isFailure()){
            ATH_MSG_ERROR( "addPRDs() failed" );
          } 
          // add to output list
          finalTracks.push_back( refittedTrack.release() );
          if (atrack.newTrack()) {
             trackDustbin.emplace_back(atrack.release());
          }
        } else {
          // add track to PRD_AssociationTool
          StatusCode sc = m_assoTool->addPRDs(prdToTrackMap, *atrack);
          if (m_assoTool->addPRDs(prdToTrackMap, *atrack).isFailure()){
            ATH_MSG_ERROR( "addPRDs() failed" );
          }
          // add to output list 
          finalTracks.push_back( atrack.release() );
        }
      } else {
        // add track to PRD_AssociationTool
        if (m_assoTool->addPRDs(prdToTrackMap, *atrack).isFailure()){ 
          ATH_MSG_ERROR( "addPRDs() failed" );
        }
        // add to output list 
        finalTracks.push_back( atrack.release() );
      }
    } else if ( keepOriginal){
      // track can be kept as is, but is not yet fitted
      ATH_MSG_DEBUG ("Good track("<< atrack.track() << ") but need to fit this track first, score, add it into map again and retry ! ");
      Trk::Track * pRefittedTrack = refitTrack(atrack.track(),prdToTrackMap, stat);
      if(pRefittedTrack) {
         addTrack( pRefittedTrack, true , scoreTrackFitflagMap, prdToTrackMap, trackDustbin, stat);
      }
      // remove original copy, but delay removal since some pointer to it or its constituents may still be in used
      if (atrack.newTrack()) {
         trackDustbin.emplace_back(atrack.release());
      }
    } else if ( cleanedTrack ) {//cleanedTrack != atrack
      ATH_MSG_DEBUG ("Candidate excluded, add subtrack to map. Track "<<cleanedTrack.get());
      stat.incrementCounterByRegion(CounterIndex::kNsubTrack,cleanedTrack.get());
      // for this case clenedTrack is a new created object.
      addTrack(cleanedTrack.release(), false, scoreTrackFitflagMap, prdToTrackMap, trackDustbin, stat);
      // remove original copy, but delay removal since some pointer to it or its constituents may still be in used
      if (atrack.newTrack()) {
         trackDustbin.emplace_back(atrack.release() );
      }
    } else {
      // track should be discarded
      ATH_MSG_DEBUG ("Track "<< atrack.track() << " is excluded, no subtrack, reject");
      stat.incrementCounterByRegion(CounterIndex::kNnoSubTrack,atrack.track());
      // remove original copy, but delay removal since some pointer to it or its constituents may still be in used
      if (atrack.newTrack()) {
        trackDustbin.emplace_back(atrack.release());
      }
    }
  }
  ATH_MSG_DEBUG ("Finished, number of track on output: "<<finalTracks.size());
}



//==================================================================================================

Trk::Track* 
Trk::DenseEnvironmentsAmbiguityProcessorTool::refitPrds( const Trk::Track* track,
                                                 Trk::PRDtoTrackMap &prdToTrackMap,
                                                 Counter &stat) const{
  // get vector of PRDs
  // @TODO ensured that prds on track are registered for this track ?
  const auto & prds = m_assoTool->getPrdsOnTrack(prdToTrackMap,*track);
  if ( prds.empty() ) {
    ATH_MSG_WARNING( "No PRDs on track");
    return nullptr;
  }
  ATH_MSG_VERBOSE ("Track "<<track<<"\t has "<<prds.size()<<"\t PRDs");
  const TrackParameters* par = getTrackParameters(track);
  if (not par) return nullptr;
  //
  // refit using first parameter, do outliers
  std::unique_ptr<Trk::Track> newTrack;
  if (m_tryBremFit and track->info().trackProperties(Trk::TrackInfo::BremFit)){
    stat.incrementCounterByRegion(CounterIndex::kNbremFits,track);
    ATH_MSG_VERBOSE ("Brem track, refit with electron brem fit");
    //revert once GlobalChi2Fitter properly handles brem fits when starting from prds
    // newTrack = fit(prds, *par, true, Trk::electron);
    newTrack = doBremRefit(*track);
  } else {
    stat.incrementCounterByRegion(CounterIndex::kNfits,track);
    ATH_MSG_VERBOSE ("Normal track, refit");
    newTrack = fit(prds, *par, true, m_particleHypothesis);
    if ((not newTrack) and shouldTryBremRecovery(*track, par)){
      stat.incrementCounterByRegion(CounterIndex::kNrecoveryBremFits,track);
      ATH_MSG_VERBOSE ("Normal fit failed, try brem recovery");
      //revert once GlobalChi2Fitter properly handles brem fits when starting from prds
      //newTrack = fit(prds, *par, true, Trk::electron);
      newTrack = doBremRefit(*track);
    }
  }
  if(newTrack) {
    stat.incrementCounterByRegion(CounterIndex::kNgoodFits,newTrack.get());
    //keeping the track of previously accumulated TrackInfo
    const Trk::TrackInfo& originalInfo = track->info();
    newTrack->info().addPatternReco(originalInfo);
  } else {
     stat.incrementCounterByRegion(CounterIndex::kNfailedFits,track);
  }
  return newTrack.release();
}


//============================================================================================================
bool 
Trk::DenseEnvironmentsAmbiguityProcessorTool::decideIfInHighPtBROI(const Trk::Track* ptrTrack) const{
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
    return false;
  } 
  return false;
}

//============================================================================================================
bool 
Trk::DenseEnvironmentsAmbiguityProcessorTool::isHadCaloCompatible(const Trk::TrackParameters& Tp) const{
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
    double df = std::abs(F-(*f));
    if(df > pi        ) df = std::abs(pi2-df);
    if(df < m_phiWidth) {
      //Correct eta of cluster to take into account the z postion of the track
      double newZ   = *z - Tp.position().z();
      double newEta =  std::atanh( newZ / std::sqrt( (*r) * (*r) + newZ*newZ ) );
      double de = std::abs(E-newEta);
      if(de < m_etaWidth) return true;
    }
    ++e;
    ++r;
    ++z;
  }
  return false;
}

//============================================================================================================
void 
Trk::DenseEnvironmentsAmbiguityProcessorTool::reloadHadROIs() const{
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
void 
Trk::DenseEnvironmentsAmbiguityProcessorTool::removeInnerHits(std::vector<const Trk::MeasurementBase*>& measurements) const{
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
        break;
      }
      break;
    }
  }
}

//============================================================================================================
Trk::Track* 
Trk::DenseEnvironmentsAmbiguityProcessorTool::refitTracksFromB(const Trk::Track* track, double fitQualityOriginal) const{
  const Trk::TrackParameters* par = track->perigeeParameters();
  if (par==nullptr) {
    par = track->trackParameters()->front();
    if (par==nullptr) {
      ATH_MSG_DEBUG ("Track ("<<track<<") has no Track Parameters ! No refit !");
      return nullptr;
    }
  }  
  std::vector<const Trk::MeasurementBase*> measurementSet{};
  //store all silicon measurements into the measurementset
  DataVector<const Trk::TrackStateOnSurface>::const_iterator TrackStateOnSurface = track->trackStateOnSurfaces()->begin();
  for ( ; TrackStateOnSurface != track->trackStateOnSurfaces()->end(); ++TrackStateOnSurface ) {
    if ( !(*TrackStateOnSurface) ){
      ATH_MSG_WARNING( "This track contains an empty MeasurementBase object that won't be included in the fit" );
      continue;
    }
    if ( (*TrackStateOnSurface)->measurementOnTrack() ){
      if ( (*TrackStateOnSurface)->type( Trk::TrackStateOnSurface::Measurement) ){
        const Trk::RIO_OnTrack* rio = dynamic_cast <const Trk::RIO_OnTrack*>( (*TrackStateOnSurface)->measurementOnTrack() );
        if (rio != nullptr) {
          const Identifier& surfaceID = (rio->identify()) ;                            
          if(m_idHelper->is_pixel(surfaceID)|| m_idHelper->is_sct(surfaceID)) {
            measurementSet.push_back( (*TrackStateOnSurface)->measurementOnTrack() );
          }
        }
      }
    }
  }

  size_t previousMeasSize = measurementSet.size();
  while (true){
    removeInnerHits(measurementSet); 
    if(measurementSet.size()>4){
      auto pRefittedTrack{fit(measurementSet,*par,true,Trk::pion)};
      double fitQualPostRefit = 10;
      if  (pRefittedTrack && pRefittedTrack->fitQuality() && pRefittedTrack->fitQuality()->numberDoF()!=0 ) 
        fitQualPostRefit = pRefittedTrack->fitQuality()->chiSquared()/pRefittedTrack->fitQuality()->numberDoF(); 
      if (fitQualityOriginal/fitQualPostRefit > 1){
        if ( fitQualityOriginal/fitQualPostRefit > 1.2){
          return pRefittedTrack.release();
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

void 
Trk::DenseEnvironmentsAmbiguityProcessorTool::dumpStat(MsgStream &out) const{
   auto parseFileName=[](const std::string & fullname){
    auto dotPosition = fullname.rfind('.');
    auto slashPosition = fullname.rfind('/');
    auto stringLength = dotPosition - slashPosition;
    return fullname.substr(slashPosition, stringLength);
   };
   // @TODO restore ios
   std::streamsize ss = out.precision();
   int iw=9;
   out << "Output from ";
   out << parseFileName(__FILE__);
   out << "::";
   out << __func__;
   out << "\n";
   out << "------------------------------------------------------------------------------------" << "\n";
   out << "  Number of events processed      :   "<< m_stat.globalCount(Counter::nEvents) << "\n";
   if (const auto nInvalid = m_stat.globalCount(Counter::nInvalidTracks); nInvalid>0) {
      out << "  Number of invalid tracks        :   "<< nInvalid<< "\n";
   }
   if (const auto nNoParams = m_stat.globalCount(Counter::nTracksWithoutParam); nNoParams>0) {
      out << "  Tracks without parameters       :   "<< nNoParams << "\n";
   }
   out << "  statistics by eta range          ------All---Barrel---Trans.-- Endcap-- Forwrd-- " << "\n";
   out << "------------------------------------------------------------------------------------" << "\n";
   out << m_stat.dumpRegions( "  Number of candidates at input   :",    CounterIndex::kNcandidates,iw);
   out << "------------------------------------------------------------------------------------" << "\n";
   out << m_stat.dumpRegions( "  candidates with good score      :",    CounterIndex::kNscoreOk,iw);
   if (m_tryBremFit) {
      out << m_stat.dumpRegions( "  + recovered after brem refit    :", CounterIndex::kNscoreZeroBremRefit,iw);
   }
   out << m_stat.dumpRegions( "  candidates rejected score 0     :",    CounterIndex::kNscoreZero,iw);
   if (m_tryBremFit) {
      out << m_stat.dumpRegions( "  + m refit                       :", CounterIndex::kNscoreZeroBremRefitFailed,iw);
      out << m_stat.dumpRegions( "  + rejected brem refit score 0   :", CounterIndex::kNscoreZeroBremRefitScoreZero,iw);
   }
   out << "------------------------------------------------------------------------------------" << "\n";
   out << m_stat.dumpRegions( "  number of normal fits           :" ,   CounterIndex::kNfits,iw);
   if (m_tryBremFit) {
      out << m_stat.dumpRegions( "  + 2nd brem fit for failed fit   :", CounterIndex::kNrecoveryBremFits,iw);
      out << m_stat.dumpRegions( "  normal brem fits for electrons  :", CounterIndex::kNbremFits,iw);
   }
   out << "------------------------------------------------------------------------------------" << "\n";
   out << m_stat.dumpRegions( "  sum of succesful fits           :",    CounterIndex::kNgoodFits,iw);
   out << m_stat.dumpRegions( "  sum of failed fits              :",    CounterIndex::kNfailedFits,iw);
   out << "------------------------------------------------------------------------------------" << "\n";
   out << m_stat.dumpRegions( "  Number of subtracks created     :",    CounterIndex::kNsubTrack,iw);
   out << m_stat.dumpRegions( "  Number of candidates excluded   :",    CounterIndex::kNnoSubTrack,iw);
   out << "------------------------------------------------------------------------------------" << "\n";
   out << m_stat.dumpRegions( "  Number of tracks accepted       :",    CounterIndex::kNaccepted,iw);
   if (m_tryBremFit) {
      out << m_stat.dumpRegions( "  including number of brem fits   :", CounterIndex::kNacceptedBrem,iw);
   }
   out << "------------------------------------------------------------------------------------" << "\n";
   out << std::setiosflags(std::ios::fixed | std::ios::showpoint) << std::setprecision(2)
       << "    definition: ( 0.0 < Barrel < " << m_etaBounds[Counter::iBarrel] << " < Transition < " << m_etaBounds[Counter::iTransi]
       << " < Endcap < " << m_etaBounds[Counter::iEndcap] << " < Forward < " << m_etaBounds[Counter::iForwrd] << " )" << "\n";
   out << "------------------------------------------------------------------------------------" << "\n";
   out << std::setprecision(ss);
}
  
std::unique_ptr<Trk::Track>
Trk::DenseEnvironmentsAmbiguityProcessorTool::doBremRefit(const Trk::Track & track) const{
  return std::unique_ptr<Trk::Track>(fit(track,true,Trk::electron));
}

