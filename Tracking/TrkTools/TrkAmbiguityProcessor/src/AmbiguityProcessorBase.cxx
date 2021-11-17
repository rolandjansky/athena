/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AmbiguityProcessorBase.h"
#include "AmbiguityProcessorUtility.h"

#include "GaudiKernel/ToolVisitor.h"
#include "GaudiKernel/RenounceToolInputsVisitor.h"

namespace Trk {
  AmbiguityProcessorBase::AmbiguityProcessorBase(const std::string& t, const std::string& n, const IInterface*  p ):
    AthAlgTool(t,n,p), 
    m_etaBounds{0.8, 1.6, 2.5, 4.0}, 
    m_stat(m_etaBounds){
      declareProperty("ObserverTool", m_observerTool, "track observer tool");
}
  //
  bool
  AmbiguityProcessorBase::shouldTryBremRecovery(const Trk::Track & track) const{
    return m_tryBremFit and 
      not (track.info().trackProperties(Trk::TrackInfo::BremFit)) and
      (track.trackParameters()->front()->pT() > m_pTminBrem) and 
      ((not m_caloSeededBrem) or track.info().patternRecoInfo(Trk::TrackInfo::TrackInCaloROI));
  }

  StatusCode
  AmbiguityProcessorBase::initializeClusterSplitProbContainer() {
    // init ClusterSplitProb input and output container handles and renounce the output handle from its tools.
    // the latter works because the ClusterSplitProb output container is created and recorded before the tools
    // are called.
    ATH_CHECK(m_clusterSplitProbContainerIn.initialize(!m_clusterSplitProbContainerIn.key().empty()));
    ATH_CHECK(m_clusterSplitProbContainerOut.initialize(!m_clusterSplitProbContainerOut.key().empty()));
    if (!m_clusterSplitProbContainerOut.key().empty()) {
      auto visitor=[this](const IAlgTool *tool) {
         const AlgTool *alg_tool = dynamic_cast<const AlgTool *>(tool);
         for ( Gaudi::DataHandle* handle : alg_tool->inputHandles() ) {
            this->msg(MSG::DEBUG)  << " input Handle "
                                   << tool->name() << " . "
                                   << handle->objKey() << endmsg;
         }
         for (const auto& elm : alg_tool->inputDataObjs()) {
            this->msg(MSG::DEBUG) << " input object "
                                  << tool->name() << " . "
                                  << elm.key()  << endmsg;
         }
      };
      if (msgLvl(MSG::DEBUG)) {
         ToolVisitor::visit(tools(), visitor);
      }
      /// usage:
      ATH_MSG_DEBUG(  " Renounce " << m_clusterSplitProbContainerOut.objKey() );
      auto logger=RenounceToolInputsVisitor::createLogger( [this](const std::string_view& tool_name, const std::string_view& key) {
         this->msg(MSG::INFO) << " Renounce " << tool_name << " . " << key << endmsg;
      });
      RenounceToolInputsVisitor renounce(std::vector<DataObjID>{m_clusterSplitProbContainerOut.fullKey()}, logger);
      ToolVisitor::visit(tools(), renounce);
      ATH_MSG_DEBUG(" renounced " << m_clusterSplitProbContainerOut.objKey() );
      if (msgLvl(MSG::DEBUG)) {
        ToolVisitor::visit(tools(), visitor);
     }
    }
    return StatusCode::SUCCESS;
  }

  AmbiguityProcessorBase::UniqueClusterSplitProbabilityContainerPtr
  AmbiguityProcessorBase::createAndRecordClusterSplitProbContainer(const EventContext& ctx) const {
    SG::ReadHandle<Trk::ClusterSplitProbabilityContainer> splitProbContainerIn;
    if (!m_clusterSplitProbContainerIn.key().empty()) {
      splitProbContainerIn = SG::ReadHandle( m_clusterSplitProbContainerIn, ctx);
      if (!splitProbContainerIn.isValid()) {
        ATH_MSG_ERROR( "Failed to get input cluster split probability container "  << m_clusterSplitProbContainerIn.key());
      }
    }
    std::unique_ptr<Trk::ClusterSplitProbabilityContainer>
       newSplitProbContainer(!m_clusterSplitProbContainerIn.key().empty()
                             ? std::make_unique<Trk::ClusterSplitProbabilityContainer>(*splitProbContainerIn)
                             : std::make_unique<Trk::ClusterSplitProbabilityContainer>());
    SG::WriteHandle<Trk::ClusterSplitProbabilityContainer> splitProbContainerHandle;
    if (!m_clusterSplitProbContainerOut.key().empty()) {
      splitProbContainerHandle=SG::WriteHandle<Trk::ClusterSplitProbabilityContainer>( m_clusterSplitProbContainerOut, ctx);
      if (splitProbContainerHandle.record(std::move(newSplitProbContainer)).isFailure()) {
        ATH_MSG_FATAL( "Failed to record output cluster split probability container "  << m_clusterSplitProbContainerOut.key());
      }
      // newSplitProbContainer owned by storegate -> no cleanup
      return UniqueClusterSplitProbabilityContainerPtr(splitProbContainerHandle.ptr(), [](Trk::ClusterSplitProbabilityContainer *) {});
    }
    else {
      // when not recording the split prob container in storegate the container must be deleted once going out of scope
      return UniqueClusterSplitProbabilityContainerPtr(newSplitProbContainer.release(), [](Trk::ClusterSplitProbabilityContainer *ptr) { delete ptr;});
    }
  }

  bool
  AmbiguityProcessorBase::shouldTryBremRecovery(const Trk::Track & track, const TrackParameters * pPar) const{
    return m_tryBremFit and
      (pPar->pT() > m_pTminBrem) and 
      ((not m_caloSeededBrem)  or  track.info().patternRecoInfo(Trk::TrackInfo::TrackInCaloROI));
  }
  //
  Track * 
  AmbiguityProcessorBase::refitTrack( const Trk::Track* track,Trk::PRDtoTrackMap &prdToTrackMap, Counter &stat, int trackId, int subtrackId) const{
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
      if (m_observerTool.isEnabled()){
        m_observerTool->rejectTrack(trackId, xAOD::RejectionStep::refitTrack, xAOD::RejectionReason::subtrackCreated);
        m_observerTool->addSubTrack(subtrackId, trackId, *newTrack);
      }
      ATH_MSG_DEBUG ("New track "<<newTrack.get()<<" successfully fitted from "<<track);
    } else {
      if (m_observerTool.isEnabled()){
        m_observerTool->rejectTrack(trackId, xAOD::RejectionStep::refitTrack, xAOD::RejectionReason::refitFailed);
      }  
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
                                                 Counter &stat,
                                                 int parentTrackId) const {
    std::unique_ptr<Trk::Track> atrack(in_track);
    // compute score
    TrackScore score;
    bool suppressHoleSearch = fitted ? m_suppressHoleSearch : true;
    if (m_trackSummaryTool.isEnabled()) {
       m_trackSummaryTool->computeAndReplaceTrackSummary(*atrack,&prdToTrackMap,suppressHoleSearch);
    }
    score = m_scoringTool->score( *atrack, suppressHoleSearch );
    if (m_observerTool.isEnabled()){
      m_observerTool->updateScore(parentTrackId, static_cast<double>(score));
    }
    // do we accept the track ?
    if (score!=0){
      ATH_MSG_DEBUG ("Track  ("<< atrack.get() <<") has score "<<score);
      // statistic
      stat.incrementCounterByRegion(CounterIndex::kNscoreOk,atrack.get());
      // add track to map, map is sorted small to big !
      if (m_observerTool.isEnabled()){
        trackScoreTrackMap.emplace(-score, TrackPtr(atrack.release(), fitted, parentTrackId));
      }
      else{
        trackScoreTrackMap.emplace(-score, TrackPtr(atrack.release(), fitted));
      }
      return;
    }
    // do we try to recover the track ?
    if (fitted and shouldTryBremRecovery(*atrack)){
      ATH_MSG_DEBUG ("Track score is zero, try to recover it via brem fit");
      // run track fit using electron hypothesis
      auto bremTrack(doBremRefit(*atrack));
      if (!bremTrack){
        ATH_MSG_DEBUG ("Brem refit failed, drop track");
        if (m_observerTool.isEnabled()){
          m_observerTool->rejectTrack(parentTrackId, xAOD::RejectionStep::addTrack, xAOD::RejectionReason::bremRefitFailed);
        }
        // statistic
        stat.incrementCounterByRegion(CounterIndex::kNscoreZeroBremRefitFailed,atrack.get());
        stat.incrementCounterByRegion(CounterIndex::kNfailedFits,atrack.get());
        // clean up
        trackDustbin.push_back(std::move(atrack));
      } else {
        int newTrackId = AmbiguityProcessor::getUid();
        if (m_observerTool.isEnabled()){
          m_observerTool->rejectTrack(parentTrackId, xAOD::RejectionStep::addTrack, xAOD::RejectionReason::bremRefitSubtrackCreated);
          m_observerTool->addSubTrack(newTrackId, parentTrackId, *bremTrack);
        }
        // statistic
        stat.incrementCounterByRegion(CounterIndex::kNgoodFits,bremTrack.get());
        // rerun score
        if (m_trackSummaryTool.isEnabled()) {
          m_trackSummaryTool->computeAndReplaceTrackSummary(*bremTrack, &prdToTrackMap,suppressHoleSearch);
        }
        score = m_scoringTool->score( *bremTrack, suppressHoleSearch );
        if (m_observerTool.isEnabled()){
          m_observerTool->updateScore(newTrackId, static_cast<double>(score));
        }
        //put original track in the bin, ready to preserve a new Brem track
        trackDustbin.push_back(std::move(atrack) );
        // do we accept the track ?
        if (score!=0){
          ATH_MSG_DEBUG ("Brem refit successful, recovered track  ("<< bremTrack.get() <<") has score "<<score);
          // statistics
          stat.incrementCounterByRegion(CounterIndex::kNscoreZeroBremRefit,bremTrack.get());
          // add track to map, map is sorted small to big !
          if (m_observerTool.isEnabled()){
            m_observerTool->addSubTrack(newTrackId, parentTrackId, *bremTrack);
            trackScoreTrackMap.emplace(-score, TrackPtr(bremTrack.release(), fitted, newTrackId) );
          }
          else{
            trackScoreTrackMap.emplace(-score, TrackPtr(bremTrack.release(), fitted) );
          }
          return;
        } else {
          ATH_MSG_DEBUG ("Brem refit gave still track score zero, reject it");
          if (m_observerTool.isEnabled()){
            m_observerTool->rejectTrack(newTrackId, xAOD::RejectionStep::addTrack, xAOD::RejectionReason::bremRefitTrackScoreZero);
          }
          // statistic
          stat.incrementCounterByRegion(CounterIndex::kNscoreZeroBremRefitScoreZero,bremTrack.get());
        }
      }
    } else {
      ATH_MSG_DEBUG ("Track score is zero, reject it");
      if (m_observerTool.isEnabled()){
        m_observerTool->rejectTrack(parentTrackId, xAOD::RejectionStep::addTrack, xAOD::RejectionReason::refitTrackScoreZero);
      }
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
