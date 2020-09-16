/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AmbiguityProcessorBase.h"
#include "TrackScoringTool.h"
#include "AmbiguityProcessorUtility.h"



namespace Trk {
  AmbiguityProcessorBase::AmbiguityProcessorBase(const std::string& t, const std::string& n, const IInterface*  p ):
    AthAlgTool(t,n,p), 
    m_etaBounds{0.8, 1.6, 2.5, 4.0}, 
    m_stat(m_etaBounds),
    m_scoringTool("Trk::TrackScoringTool/TrackScoringTool"){
  }
  //
  bool
  AmbiguityProcessorBase::shouldTryBremRecovery(const Trk::Track & track) const{
    return m_tryBremFit and 
      not (track.info().trackProperties(Trk::TrackInfo::BremFit)) and
      (track.trackParameters()->front()->pT() > m_pTminBrem) and 
      ((not m_caloSeededBrem) or track.info().patternRecoInfo(Trk::TrackInfo::TrackInCaloROI));
  }
  
  bool
  AmbiguityProcessorBase::shouldTryBremRecovery(const Trk::Track & track, const TrackParameters * pPar) const{
    return m_tryBremFit and
      (pPar->pT() > m_pTminBrem) and 
      ((not m_caloSeededBrem)  or  track.info().patternRecoInfo(Trk::TrackInfo::TrackInCaloROI));
  }
  //
  Track * 
  AmbiguityProcessorBase::refitTrack( const Trk::Track* track,Trk::PRDtoTrackMap &prdToTrackMap, Counter &stat) const{
    std::unique_ptr<Trk::Track> newTrack;
    if (!m_suppressTrackFit){
      if (m_refitPrds) {
        // simple case, fit PRD directly
        ATH_MSG_VERBOSE ("Refit track "<<track<<" from PRDs");
        newTrack.reset(refitPrds (track,prdToTrackMap, stat));
      }else {
        // ok, we fit ROTs
        ATH_MSG_VERBOSE ("Refit track "<<track<<" from ROTs");
        newTrack.reset(refitRots (track,stat));
      }
    } else {
      newTrack = AmbiguityProcessor::createNewFitQualityTrack(*track);
    }
    if (newTrack) {
      ATH_MSG_DEBUG ("New track "<<newTrack.get()<<" successfully fitted from "<<track);
    } else { 
      ATH_MSG_DEBUG ("Fit failed !");
    }  
    return newTrack.release();
  }
  //
  void
  AmbiguityProcessorBase::addTrack(Trk::Track* in_track,const bool fitted,
                                                 TrackScoreMap &trackScoreTrackMap,
                                                 Trk::PRDtoTrackMap &prdToTrackMap,
                                                 std::vector<std::unique_ptr<const Trk::Track> >& trackDustbin,
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
      stat.incrementCounterByRegion(CounterIndex::kNscoreOk,atrack.get());
      // add track to map, map is sorted small to big !
      trackScoreTrackMap.emplace(-score, TrackPtr(atrack.release(), fitted));
      return;
    }
    // do we try to recover the track ?
    if (fitted and shouldTryBremRecovery(*atrack)){
      ATH_MSG_DEBUG ("Track score is zero, try to recover it via brem fit");
      // run track fit using electron hypothesis
      auto bremTrack(doBremRefit(*atrack));
      if (!bremTrack){
        ATH_MSG_DEBUG ("Brem refit failed, drop track");
        // statistic
        stat.incrementCounterByRegion(CounterIndex::kNscoreZeroBremRefitFailed,atrack.get());
        stat.incrementCounterByRegion(CounterIndex::kNfailedFits,atrack.get());
        // clean up
        trackDustbin.push_back(std::move(atrack));
      } else {
        // statistic
        stat.incrementCounterByRegion(CounterIndex::kNgoodFits,bremTrack.get());
        // rerun score
        if (m_trackSummaryTool.isEnabled()) {
          m_trackSummaryTool->computeAndReplaceTrackSummary(*bremTrack, &prdToTrackMap,suppressHoleSearch);
        }
        score = m_scoringTool->score( *bremTrack, suppressHoleSearch );
        //put original track in the bin, ready to preserve a new Brem track
        trackDustbin.push_back(std::move(atrack) );
        // do we accept the track ?
        if (score!=0){
          ATH_MSG_DEBUG ("Brem refit successful, recovered track  ("<< bremTrack.get() <<") has score "<<score);
          // statistics
          stat.incrementCounterByRegion(CounterIndex::kNscoreZeroBremRefit,bremTrack.get());
          // add track to map, map is sorted small to big !
          trackScoreTrackMap.emplace(-score, TrackPtr(bremTrack.release(), fitted) );
          return;
        } else {
          ATH_MSG_DEBUG ("Brem refit gave still track score zero, reject it");
          // statistic
          stat.incrementCounterByRegion(CounterIndex::kNscoreZeroBremRefitScoreZero,bremTrack.get());
        }
      }
    } else {
      ATH_MSG_DEBUG ("Track score is zero, reject it");
      // statistic
      stat.incrementCounterByRegion(CounterIndex::kNscoreZero,atrack.get());
      trackDustbin.push_back(std::move(atrack));
    }
  }

  const TrackParameters *
  AmbiguityProcessorBase::getTrackParameters(const Trk::Track* track) const{
    const TrackParameters* par = track->perigeeParameters();
    if (not par) {
      ATH_MSG_DEBUG ("Track ("<<track<<") has no perigee! Try any other ?");
      par = track->trackParameters()->front();
    }
    if (not par) ATH_MSG_DEBUG ("Track ("<<track<<") has no Track Parameters ! No refit !");
    return par;
  }
  
  //==================================================================================================

  Trk::Track* 
  AmbiguityProcessorBase::refitRots(const Trk::Track* track,Counter &stat) const{
    ATH_MSG_VERBOSE ("Refit track "<<track);
    // refit using first parameter, do outliers
    std::unique_ptr<Trk::Track> newTrack{};
    if (m_tryBremFit and track->info().trackProperties(Trk::TrackInfo::BremFit)){
      stat.incrementCounterByRegion(CounterIndex::kNbremFits,track);
      ATH_MSG_VERBOSE ("Brem track, refit with electron brem fit");
      newTrack = doBremRefit(*track);
    } else {
      stat.incrementCounterByRegion(CounterIndex::kNfits,track);
      ATH_MSG_VERBOSE ("Normal track, refit");
      newTrack = fit(*track, true, m_particleHypothesis);
      if ( (not newTrack) and shouldTryBremRecovery(*track)){
        stat.incrementCounterByRegion(CounterIndex::kNrecoveryBremFits,track);
        ATH_MSG_VERBOSE ("Normal fit failed, try brem recovery");
        newTrack = doBremRefit(*track);
      }
    }

    if(newTrack){
      stat.incrementCounterByRegion(CounterIndex::kNgoodFits,newTrack.get());
      //keeping the track of previously accumulated TrackInfo
      const Trk::TrackInfo& originalInfo = track->info();
      newTrack->info().addPatternReco(originalInfo);
    } else {
      stat.incrementCounterByRegion(CounterIndex::kNfailedFits,track);
    }
    return newTrack.release();
  }
  

}