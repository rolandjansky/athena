/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBPhysHypo/TrigMultiTrkComboHypo.cxx
 **
 **   Description: multi-track hypothesis algorithm
 **
 **   Author: Heather Russell
 **
 **************************************************************************/

#include <algorithm>
#include <numeric>

#include "Constants.h"
#include "TrigMultiTrkComboHypo.h"

#include "xAODMuon/Muon.h"
#include "xAODEgamma/Electron.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigMuon/L2CombinedMuonContainer.h"
#include "xAODTrigBphys/TrigBphys.h"
#include "xAODTrigBphys/TrigBphysContainer.h"
#include "xAODTrigBphys/TrigBphysAuxContainer.h"

#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigCompositeUtils/HLTIdentifier.h"

#include "AthViews/View.h"
#include "AthViews/ViewHelper.h"

#include "Math/GenVector/VectorUtil.h"
#include "Math/Vector2D.h"


using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::DecisionID;
using TrigCompositeUtils::DecisionIDContainer;
using ROOT::Math::XYVector;

typedef struct PDG20 PDG;


TrigMultiTrkComboHypo::TrigMultiTrkComboHypo(const std::string& name, ISvcLocator* pSvcLocator)
    : ::ComboHypo(name, pSvcLocator) {}


StatusCode TrigMultiTrkComboHypo::initialize() {
  ATH_MSG_DEBUG( "TrigMultiTrkHypo::initialize()" );

  ATH_CHECK( ::ComboHypo::initialize() );

  // check consistency of the properties
  ATH_CHECK( !m_nTrk.empty() );

  if (m_nTrkCharge.empty()) {
    ATH_MSG_INFO( "totalCharge value is not specified, no charge selection for track combinations will be used" );
    m_nTrkCharge = std::vector<int>(m_nTrk.size(), -1);
  }

  if (m_trkMass.empty()) {
    ATH_MSG_INFO( "trackMasses value is not specified, muon/electron mass will be used" );
    for (const auto& n : m_nTrk) {
      m_trkMass.value().emplace_back(std::vector<double>(n, (m_doElectrons ? PDG::mElectron : PDG::mMuon)));
    }
  }
  if (m_trkPt.empty()) {
    ATH_MSG_INFO( "trackPtThresholds value is not specified" );
    for (const auto& n : m_nTrk) {
      m_trkPt.value().emplace_back(std::vector<double>(n, -100.));
    }
  }
  m_trkPtMin = m_trkPt[0].back();
  for (size_t i = 0; i < m_trkPt.size(); ++i) {
    m_trkPtMin = std::min(m_trkPtMin, m_trkPt[i].back());
  }

  ATH_CHECK( m_trkMass.size() == m_nTrk.size() );
  ATH_CHECK( m_trkPt.size() == m_nTrk.size() );

  for (size_t i = 0; i < m_nTrk.size(); ++i) {
    ATH_CHECK( m_trkMass[i].size() == m_nTrk[i] );
    ATH_CHECK( m_trkPt[i].size() == m_nTrk[i] );
    ATH_CHECK( std::is_sorted(m_trkPt[i].begin(), m_trkPt[i].end(), std::greater<>()) );
  }

  for (const auto& range : m_massRange.value()) {
    ATH_CHECK( range.first < range.second );
  }

  // dump numerical values
  if (msgLvl(MSG::DEBUG)) {
    for (size_t i = 0; i < m_nTrk.size(); ++i) {
      ATH_MSG_DEBUG( "vertex topology: nTrk = " << m_nTrk[i] );
      for (size_t j = 0; j < m_nTrk[i]; ++j) {
        ATH_MSG_DEBUG( "  " << j + 1 << "  trk: mass = " <<  m_trkMass[i][j] << ", Pt > " << m_trkPt[i][j] );
      }
    }
    msg() << MSG::DEBUG << "  mass range: {";
    for (const auto& range : m_massRange.value()) {
      msg() << MSG::DEBUG << " { " << range.first << ", " << range.second << " }";
    }
    msg() << MSG::DEBUG << " }" << std::endl;
  }

  if (m_isStreamer) {
    ATH_MSG_DEBUG( "Configured to run in a streamer mode: no trigger objects will be created" );
  }
  if (!m_isStreamer && m_trigLevel != "EF") {
    ATH_MSG_ERROR( "Could not create trigger objects from tracks or L2 CB muons, use the streamer mode for L2 step" );
    return StatusCode::FAILURE;
  }

  ATH_CHECK( !((m_trigLevel == "L2IO" || m_trigLevel == "L2MT") && m_doElectrons) );

  if (m_trigLevel == "L2" || (m_trigLevel == "EF" && m_isMuTrkMode)) {
    ATH_CHECK( m_trackParticleContainerKey.initialize() );
    renounce(m_trackParticleContainerKey);
  }
  else if (m_trigLevel == "L2IO" || m_trigLevel == "L2MT" || m_trigLevel == "EF") {
    ATH_CHECK( m_trackParticleContainerKey.initialize(false) );
  }
  else {
    ATH_MSG_ERROR( "trigLevel should be L2, L2IO, L2MT or EF, but " << m_trigLevel << " provided" );
    return StatusCode::FAILURE;
  }

  ATH_CHECK( m_trigBphysContainerKey.initialize(!m_isStreamer.value()) );
  ATH_CHECK( m_beamSpotKey.initialize(!m_isStreamer.value()) );  // need beamSpot only to create xAOD::TrigBphys object

  ATH_CHECK( m_vertexFitter.retrieve() );
  ATH_CHECK( m_vertexPointEstimator.retrieve() );

  // allowed IDs to filter out incoming decisions at L2 level
  for (const auto& item : triggerMultiplicityMap()) {
    const HLT::Identifier id = HLT::Identifier::fromToolName(item.first);
    m_allowedIDs.insert(id.numeric());
    if (item.second.size() > 1) {
      for (size_t i = 0; i < item.second.size(); i++) {
        m_allowedIDs.insert(TrigCompositeUtils::createLegName(id, i).numeric());
      }
    }
  }
  if (msgLvl(MSG::DEBUG)) {
    ATH_MSG_DEBUG( "Allowed decisions:" );
    for (const DecisionID& id : m_allowedIDs) {
      ATH_MSG_DEBUG( " +++ " << HLT::Identifier(id) );
    }
  }
  if (m_doElectrons) {
    for (const DecisionID& id : m_allowedIDs) {
      std::string name = HLT::Identifier(id).name();
      bool isMergedElectronChain = false;
      for (size_t i = 0; i < m_mergedElectronChains.size(); ++i) {
        if (name.find(m_mergedElectronChains.value().at(i)) != std::string::npos) {
          isMergedElectronChain = true;
          break;
        }
      }
      (isMergedElectronChain ? m_mergedElectronIDs.insert(id) : m_resolvedElectronIDs.insert(id));
    }
    if (msgLvl(MSG::DEBUG)) {
      ATH_MSG_DEBUG( "Well-separated electron decisions:" );
      for (const DecisionID& id : m_resolvedElectronIDs) {
        ATH_MSG_DEBUG( " +++ " << HLT::Identifier(id) );
      }
      ATH_MSG_DEBUG( "Overlapping electron decisions:" );
      for (const DecisionID& id : m_mergedElectronIDs) {
        ATH_MSG_DEBUG( " +++ " << HLT::Identifier(id) );
      }
    }
  }

  if (!m_monTool.empty()) {
    ATH_CHECK( m_monTool.retrieve() );
    ATH_MSG_DEBUG( "GenericMonitoringTool name:" << m_monTool );
  }
  else {
    ATH_MSG_DEBUG( "No GenericMonitoringTool configured: no monitoring histograms will be available" );
  }

  return StatusCode::SUCCESS;
}


StatusCode TrigMultiTrkComboHypo::execute(const EventContext& context) const {

  ATH_MSG_DEBUG( "TrigMultiTrkHypo::execute() starts" );

  ATH_MSG_DEBUG( "decision input key: " << decisionsInput().at(0).key() );
  auto previousDecisionsHandle = SG::makeHandle(decisionsInput().at(0), context);
  CHECK( previousDecisionsHandle.isValid() );
  ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() << " previous decisions" );

  SG::WriteHandle<DecisionContainer> outputDecisionsHandle = TrigCompositeUtils::createAndStore(decisionsOutput().at(0), context);

  const InDet::BeamSpotData* beamSpotData = nullptr;
  if (!m_isStreamer) {
    SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle {m_beamSpotKey, context};
    ATH_CHECK( beamSpotHandle.isValid() );
    beamSpotData = *beamSpotHandle;
  }

  std::unique_ptr<TrigMultiTrkState<xAOD::MuonContainer>> muonState;
  std::unique_ptr<TrigMultiTrkState<xAOD::ElectronContainer>> electronState;
  TrigMultiTrkStateBase* commonState = nullptr;
  if (m_doElectrons) {
    electronState = std::make_unique<TrigMultiTrkState<xAOD::ElectronContainer>>(context, *previousDecisionsHandle, *outputDecisionsHandle, nullptr, beamSpotData);
    commonState = electronState.get();
  }
  else {
    muonState = std::make_unique<TrigMultiTrkState<xAOD::MuonContainer>>(context, *previousDecisionsHandle, *outputDecisionsHandle, nullptr, beamSpotData);
    commonState = muonState.get();
  }

  if (m_isStreamer) {
    if (m_trigLevel == "L2") {
      ATH_CHECK( mergeTracksFromViews(*commonState) );
    }
    else if (m_trigLevel == "L2IO" || m_trigLevel == "L2MT") {
      ATH_CHECK( mergeTracksFromDecisions<xAOD::L2CombinedMuonContainer>(*commonState) );
    }
    else if (m_trigLevel == "EF") {
      ATH_CHECK( (m_doElectrons ? mergeTracksFromDecisions<xAOD::ElectronContainer>(*commonState) : mergeTracksFromDecisions<xAOD::MuonContainer>(*commonState)) );
    }
    ATH_CHECK( filterTrackCombinations(*commonState) );
    ATH_CHECK( copyDecisionObjects(*commonState) );
  }
  else {
    auto trigBphysHandle = SG::makeHandle(m_trigBphysContainerKey, context);
    ATH_CHECK( trigBphysHandle.record(std::make_unique<xAOD::TrigBphysContainer>(),
                                      std::make_unique<xAOD::TrigBphysAuxContainer>()) );
    commonState->setTrigBphysCollection(trigBphysHandle.ptr());

    if (m_doElectrons) {
      ATH_CHECK( mergeLeptonsFromDecisions(*electronState) );
      ATH_CHECK( findMultiLeptonCandidates(*electronState) );
      ATH_CHECK( processMergedElectrons(*electronState) );
    }
    else if (m_isMuTrkMode) {
      ATH_CHECK( findMuTrkCandidates(*muonState) );
    }
    else {
      ATH_CHECK( mergeLeptonsFromDecisions(*muonState) );
      ATH_CHECK( findMultiLeptonCandidates(*muonState) );
    }
    ATH_CHECK( createDecisionObjects(*commonState) );
  }

  ATH_MSG_DEBUG( "TrigMultiTrkHypo::execute() terminates with StatusCode::SUCCESS" );
  return StatusCode::SUCCESS;
}


template<typename T>
StatusCode TrigMultiTrkComboHypo::mergeLeptonsFromDecisions(TrigMultiTrkState<T>& state) const {

  auto& leptons = state.leptons();
  leptons.clear();

  std::vector<const Decision*> previousDecisions(state.previousDecisions().begin(), state.previousDecisions().end());
  if (m_combineInputDecisionCollections && decisionsInput().size() > 1) {
    for (size_t k = 1; k < decisionsInput().size(); ++k) {
      auto previousDecisionsHandle = SG::makeHandle(decisionsInput().at(k), state.context());
      CHECK( previousDecisionsHandle.isValid() );
      ATH_MSG_DEBUG( "Adding " << previousDecisionsHandle->size() << " decisions from " << decisionsInput().at(k).key() );
      for (const Decision* decision : *previousDecisionsHandle) {
        previousDecisions.push_back(decision);
      }
    }
  }

  // all muons/electrons from views are already connected with previous decisions by TrigMuonEFHypoAlg
  for (const Decision* decision : previousDecisions) {
    if (!TrigCompositeUtils::isAnyIDPassing(decision, (m_doElectrons ? m_resolvedElectronIDs : m_allowedIDs))) continue;

    ElementLink<T> leptonEL;
    if (decision->hasObjectLink(TrigCompositeUtils::featureString(), ClassID_traits<T>::ID())) {
      leptonEL = decision->objectLink<T>(TrigCompositeUtils::featureString());
    }
    else {
      auto leptonLinkInfo = TrigCompositeUtils::findLink<T>(decision, TrigCompositeUtils::featureString(), true);
      ATH_CHECK( leptonLinkInfo.isValid() );
      leptonEL = leptonLinkInfo.link;
    }

    const auto lepton = *leptonEL;
    if constexpr(std::is_same<T, xAOD::MuonContainer>::value) {
      if (!lepton->trackParticle(xAOD::Muon::TrackParticleType::CombinedTrackParticle)) continue;
      if (!lepton->trackParticle(xAOD::Muon::TrackParticleType::InnerDetectorTrackParticle)) continue;
    }
    else if constexpr(std::is_same<T, xAOD::ElectronContainer>::value) {
      if (!lepton->trackParticle()) continue;
    }
    else {
      ATH_MSG_ERROR( "mergeLeptonsFromDecisions(): no scenario for the provided CONTAINER is specified" );
      return StatusCode::FAILURE;
    }
    auto decisionEL = TrigCompositeUtils::decisionToElementLink(decision, state.context());

    auto itr = leptons.end();
    if (m_applyOverlapRemoval) {
      itr = std::find_if(leptons.begin(), leptons.end(),
                         [this, lepton = lepton](const auto& x){ return this->isIdenticalTracks(lepton, *x.link); });
    }
    if (itr == leptons.end()) {
      leptons.push_back({leptonEL, ElementLinkVector<DecisionContainer>(1, decisionEL), DecisionIDContainer()});
    }
    else {
      (*itr).decisionLinks.push_back(decisionEL);
    }
  }

  // muon->pt() is equal to muon->trackParticle(xAOD::Muon::TrackParticleType::CombinedTrackParticle)->pt()
  // and the later is used by TrigMuonEFHypoTool for classification of muEFCB candidates
  std::sort(leptons.begin(), leptons.end(), [](const auto& lhs, const auto& rhs){ return ((*lhs.link)->pt() > (*rhs.link)->pt()); });

  // for each muon we extract DecisionIDs stored in the associated Decision objects and copy them at muon.decisionIDs
  for (auto& item : leptons) {
    for (const ElementLink<xAOD::TrigCompositeContainer> decisionEL : item.decisionLinks) {
      TrigCompositeUtils::decisionIDs(*decisionEL, item.decisionIDs);
    }
  }

  if (msgLvl(MSG::DEBUG)) {
    ATH_MSG_DEBUG( "Dump found leptons before vertex fit: " << leptons.size() << " candidates" );
    for (const auto& item : leptons) {
      const auto lepton = *item.link;
      const xAOD::TrackParticle* track;
      if constexpr(std::is_same<T, xAOD::MuonContainer>::value) {
        track = lepton->trackParticle(xAOD::Muon::TrackParticleType::InnerDetectorTrackParticle);
      }
      else {
        track = lepton->trackParticle();
      }
      ATH_MSG_DEBUG( " -- lepton InDetTrackParticle pt/eta/phi/q: " << track->pt() << " / " << track->eta() << " / " << track->phi() << " / " << track->charge() );
      ATH_MSG_DEBUG( "    lepton pt (muon: CombinedTrackParticle): " << lepton->pt() << " / " << lepton->eta() << " / " << lepton->phi() << " / " << lepton->charge() );
      ATH_MSG_DEBUG( "    allowed decisions:" );
      for (const DecisionID& id : item.decisionIDs) {
        ATH_MSG_DEBUG( "    +++ " << HLT::Identifier(id) );
      }
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode TrigMultiTrkComboHypo::mergeTracksFromViews(TrigMultiTrkStateBase& state) const {

  auto& tracks = state.tracks();
  tracks.clear();

  std::set<const SG::View*> views;
  for (const Decision* decision : state.previousDecisions()) {
    if (!TrigCompositeUtils::isAnyIDPassing(decision, m_allowedIDs)) continue;

    auto viewLinkInfo = TrigCompositeUtils::findLink<ViewContainer>(decision, TrigCompositeUtils::viewString(), true);
    ATH_CHECK( viewLinkInfo.isValid() );
    const SG::View* view = *viewLinkInfo.link;
    if (views.find(view) != views.end()) continue;  // tracks from this view have already been fetched

    auto tracksHandle = ViewHelper::makeHandle(view, m_trackParticleContainerKey, state.context());
    ATH_CHECK( tracksHandle.isValid() );
    ATH_MSG_DEBUG( "tracks handle " << m_trackParticleContainerKey << " size: " << tracksHandle->size() );

    std::vector<ElementLink<xAOD::TrackParticleContainer>> tracksFromView;
    tracksFromView.reserve(tracksHandle->size());
    for (size_t idx = 0; idx < tracksHandle->size(); ++idx) {
      tracksFromView.emplace_back(ViewHelper::makeLink<xAOD::TrackParticleContainer>(view, tracksHandle, idx));
    }

    for (const auto& trackEL : tracksFromView) {
      const xAOD::TrackParticle* track = *trackEL;
      if (track->definingParametersCovMatrixVec().empty() || track->pt() < m_trkPtMin) continue;

      if (views.empty() || !m_applyOverlapRemoval ||
          std::find_if(tracks.begin(), tracks.end(),
                       [this,track = track](const auto& x){ return isIdenticalTracks(track, *x); }) == tracks.end()) {
        tracks.emplace_back(trackEL);
      }
    }
    views.insert(view);
  }
  std::sort(tracks.begin(), tracks.end(), [](const auto& lhs, const auto& rhs){ return ((*lhs)->pt() > (*rhs)->pt()); });

  if (msgLvl(MSG::DEBUG)) {
    ATH_MSG_DEBUG( "Dump found tracks before vertex fit: " << tracks.size() << " candidates" );
    for (const auto& trackEL : tracks) {
      const xAOD::TrackParticle* track = *trackEL;
      ATH_MSG_DEBUG( " -- track pt/eta/phi/q: " << track->pt() << " / " << track->eta() << " / " << track->phi() << " / " << track->charge() );
    }
  }

  return StatusCode::SUCCESS;
}


template<typename CONTAINER>
StatusCode TrigMultiTrkComboHypo::mergeTracksFromDecisions(TrigMultiTrkStateBase& state) const {

  auto& tracks = state.tracks();
  tracks.clear();

  // all muons/electrons from views are already connected with previous decisions by TrigMuonEFHypoAlg or by TrigEgammaPrecisionElectronHypoAlg
  for (const Decision* decision : state.previousDecisions()) {
    if (!TrigCompositeUtils::isAnyIDPassing(decision, m_allowedIDs)) continue;

    ATH_CHECK( decision->hasObjectLink(TrigCompositeUtils::featureString(), ClassID_traits<CONTAINER>::ID()) );
    auto leptonEL = decision->objectLink<CONTAINER>(TrigCompositeUtils::featureString());
    const auto lepton = *leptonEL;

    ElementLink<xAOD::TrackParticleContainer> trackEL;
    if constexpr(std::is_same<CONTAINER, xAOD::MuonContainer>::value) {
      if (!lepton->trackParticle(xAOD::Muon::TrackParticleType::CombinedTrackParticle)) continue;
      if (!lepton->trackParticle(xAOD::Muon::TrackParticleType::InnerDetectorTrackParticle)) continue;
      trackEL = lepton->inDetTrackParticleLink();
    }
    else if constexpr(std::is_same<CONTAINER, xAOD::L2CombinedMuonContainer>::value) {
      if (!lepton->idTrack()) continue;
      trackEL = lepton->idTrackLink();
    }
    else if constexpr(std::is_same<CONTAINER, xAOD::ElectronContainer>::value) {
      if (!lepton->trackParticle()) continue;
      trackEL = lepton->trackParticleLink();
    }
    else {
      ATH_MSG_ERROR( "mergeTracksFromDecisions(): no scenario for the provided CONTAINER is specified" );
      return StatusCode::FAILURE;
    }

    if (!trackEL.isValid()) continue;
    if (!m_applyOverlapRemoval ||
        std::find_if(tracks.begin(), tracks.end(),
                     [this, track = *trackEL](const auto& x){ return this->isIdenticalTracks(track, *x); }) == tracks.end()) {
      tracks.emplace_back(trackEL);
    }
  }
  std::sort(tracks.begin(), tracks.end(), [](const auto& lhs, const auto& rhs){ return ((*lhs)->pt() > (*rhs)->pt()); });

  if (msgLvl(MSG::DEBUG)) {
    ATH_MSG_DEBUG( "Dump found tracks before vertex fit: " << tracks.size() << " candidates" );
    for (const auto& trackEL : tracks) {
      const xAOD::TrackParticle* track = *trackEL;
      ATH_MSG_DEBUG( " -- track pt/eta/phi/q: " << track->pt() << " / " << track->eta() << " / " << track->phi() << " / " << track->charge() );
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode TrigMultiTrkComboHypo::filterTrackCombinations(TrigMultiTrkStateBase& state) const {

  const auto& tracks = state.tracks();
  state.setEventAccepted(false);

  // monitored variables
  auto mon_nAcceptedTrk = Monitored::Scalar<int>("nAcceptedTrk", tracks.size());
  auto mon_nVertexFitterCalls = Monitored::Scalar<int>("nVertexFitterCalls", 0);
  auto mon_isEventAccepted = Monitored::Scalar<int>("acceptance", 0);
  auto mon_timer = Monitored::Timer( "TIME_all" );

  auto group = Monitored::Group(m_monTool,
    mon_nAcceptedTrk, mon_nVertexFitterCalls, mon_isEventAccepted,
    mon_timer);

  for (size_t iTrk = 0; iTrk < m_nTrk.size(); ++iTrk) {
    if (state.isEventAccepted()) break;
    size_t nTrk = m_nTrk[iTrk];

    if (tracks.size() < nTrk) {
      ATH_MSG_DEBUG( "Could not build a subset of " << nTrk << " tracks from collection which contains only " << tracks.size() << " objects" );
      continue;
    }
    ATH_MSG_DEBUG( "Consider combinations of " << nTrk << " tracks from collection which contains " << tracks.size() << " objects until find a good one" );

    std::vector<ElementLink<xAOD::TrackParticleContainer>> tracklist(nTrk);
    std::vector<xAOD::TrackParticle::GenVecFourMom_t> p(nTrk);

    // tracks from the current combination will have non-zero value against their position in the 'idx' vector
    // consider first nTrk tracks as an initial combination, then get the next one with std::prev_permutation()
    std::vector<char> idx(tracks.size(), 0);
    std::fill(idx.begin(), idx.begin() + nTrk, 1);
    do {
      // fill tracklist and momenta of tracks, also check that the track pT passes the threshold value
      bool isValidCombination = true;
      int totalCharge = 0;
      size_t j = 0;
      for (size_t i = 0; i < idx.size(); ++i) {
        if (!idx[i]) continue;
        const auto& trackEL = tracks[i];
        tracklist[j] = trackEL;
        const auto track = *trackEL;
        p[j] = track->genvecP4();
        p[j].SetM(m_trkMass[iTrk][j]);
        totalCharge += static_cast<int>(track->charge());
        if (p[j].Pt() < m_trkPt[iTrk][j]) {
          isValidCombination = false;
          break;
        }
        ++j;
      }
      if (!isValidCombination || (m_nTrkCharge[iTrk] >= 0 && totalCharge != m_nTrkCharge[iTrk]) || !passedDeltaRcut(p)) continue;

      if (msgLvl(MSG::DEBUG)) {
        ATH_MSG_DEBUG( "Dump found tracks before vertex fit: pT / eta / phi / charge" );
        for (size_t i = 0; i < tracklist.size(); ++i) {
          const auto track = *tracklist[i];
          ATH_MSG_DEBUG( "track " << i + 1 << ": " << p[i].Pt() << " / " << p[i].Eta() << " / " << p[i].Phi() << " / " << track->charge() );
        }
      }

      auto mass = (std::accumulate(p.begin(), p.end(), xAOD::TrackParticle::GenVecFourMom_t())).M();
      ATH_MSG_DEBUG( "invariant mass: " << mass );

      if (!isInMassRange(mass, iTrk)) continue;

      auto fitterState = m_vertexFitter->makeState(state.context());
      auto vertex = fit(tracklist, m_trkMass[iTrk], *fitterState);
      ++mon_nVertexFitterCalls;
      if (!vertex) continue;

      ATH_MSG_DEBUG( "Filter found a subset of tracks which passed the rough selection: stop looking for other combinations" );
      state.setEventAccepted(true);
      break;

    } while (std::prev_permutation(idx.begin(), idx.end()));
  }

  if (!state.isEventAccepted()) {
    ATH_MSG_DEBUG( "Filter could not find a good subset of tracks" );
  }

  mon_isEventAccepted = state.isEventAccepted();

  return StatusCode::SUCCESS;
}


template<typename T>
StatusCode TrigMultiTrkComboHypo::findMultiLeptonCandidates(TrigMultiTrkState<T>& state) const {

  state.trigBphysLegIndices().clear();
  const auto& leptons = state.leptons();

  // monitored variables
  auto mon_nAcceptedTrk = Monitored::Scalar<int>("nAcceptedTrk", leptons.size());
  auto mon_nCombination = Monitored::Scalar<int>("nCombination", 0);
  auto mon_nCombinationBeforeFit = Monitored::Scalar<int>("nCombinationBeforeFit", 0);
  auto mon_nBPhysObject = Monitored::Scalar<int>("nBPhysObject", 0);

  std::vector<float> trkMassBeforeFit;
  std::vector<float> bphysMass;
  std::vector<float> d0track1, d0track2;
  std::vector<float> pttrack1, pttrack2;
  std::vector<float> etatrack1, etatrack2;
  std::vector<int> bphysCharge;
  auto mon_trkMassBeforeFit = Monitored::Collection("trkMassBeforeFit", trkMassBeforeFit);
  auto mon_bphysChi2 = Monitored::Collection("bphysChi2", state.trigBphysCollection(), &xAOD::TrigBphys::fitchi2);
  auto mon_bphysLxy = Monitored::Collection("bphysLxy", state.trigBphysCollection(), &xAOD::TrigBphys::lxy);
  auto mon_bphysFitMass = Monitored::Collection("bphysFitMass", state.trigBphysCollection(), [](const xAOD::TrigBphys* x){ return x->fitmass()*0.001; });
  auto mon_bphysMass = Monitored::Collection("bphysMass", bphysMass);
  auto mon_d0track1 = Monitored::Collection("bphysd0_trk1", d0track1);
  auto mon_d0track2 = Monitored::Collection("bphysd0_trk2", d0track2);
  auto mon_pttrack1 = Monitored::Collection("bphysPt_trk1", pttrack1);
  auto mon_pttrack2 = Monitored::Collection("bphysPt_trk2", pttrack2);
  auto mon_etatrack1 = Monitored::Collection("bphysEtatrack1", etatrack1);
  auto mon_etatrack2 = Monitored::Collection("bphysEtatrack2", etatrack2);
  auto mon_bphysCharge = Monitored::Collection("bphysCharge", bphysCharge);

  auto mon_timer = Monitored::Timer( "TIME_all" );

  auto group = Monitored::Group(m_monTool,
    mon_nAcceptedTrk, mon_nCombination, mon_nCombinationBeforeFit, mon_nBPhysObject,
    mon_trkMassBeforeFit, mon_bphysChi2, mon_bphysLxy, mon_bphysFitMass, mon_bphysMass, mon_bphysCharge, mon_d0track1, mon_d0track2,
    mon_timer, mon_pttrack1, mon_pttrack2, mon_etatrack1, mon_etatrack2);

  for (size_t iTrk = 0; iTrk < m_nTrk.size(); ++iTrk) {
    size_t nTrk = m_nTrk[iTrk];

    if (leptons.size() < nTrk) {
      ATH_MSG_DEBUG( "Could not build a subset of " << nTrk << " legs from collection which contains only " << leptons.size() << " objects" );
      continue;
    }
    ATH_MSG_DEBUG( "Consider all combinations of " << nTrk << " legs from collection which contains " << leptons.size() << " objects" );

    std::vector<size_t> leptonIndices(nTrk);
    std::vector<ElementLink<xAOD::TrackParticleContainer>> tracklist(nTrk);
    std::vector<xAOD::TrackParticle::GenVecFourMom_t> p(nTrk);

    std::vector<char> combination(leptons.size(), 0);
    std::fill(combination.begin(), combination.begin() + nTrk, 1);
    do {
      // fill tracklist and momenta of muons in the current subset
      bool isValidCombination = true;
      int charge = 0;
      size_t j = 0;
      for (size_t i = 0; i < combination.size(); ++i) {
        if (!combination[i]) continue;
        leptonIndices[j] = i;
        auto leg = *leptons[i].link;
        charge += static_cast<int>(lround(leg->charge()));
        ElementLink<xAOD::TrackParticleContainer> trackEL;
        if constexpr(std::is_same<T, xAOD::MuonContainer>::value) {
          trackEL = leg->inDetTrackParticleLink();
        }
        else {
          trackEL = leg->trackParticleLink();
        }
        tracklist[j] = trackEL;
        if (m_useLeptonMomentum) {
          p[j] = leg->genvecP4();
          p[j].SetM(0.);  // to keep consistency with TrigComboHypoTool::compute()
        }
        else {
          p[j] = (*trackEL)->genvecP4();
          p[j].SetM(m_trkMass[iTrk][j]);
        }
        if (p[j].Pt() < m_trkPt[iTrk][j]) {
          isValidCombination = false;
          break;
        }
        ++j;
      }
      if (!isValidCombination || (m_nTrkCharge[iTrk] >= 0 && charge != m_nTrkCharge[iTrk]) || !passedDeltaRcut(p)) continue;

      if (msgLvl(MSG::DEBUG)) {
        ATH_MSG_DEBUG( "Dump found leptons before vertex fit: pT / eta / phi / charge" );
        for (size_t i = 0; i < tracklist.size(); ++i) {
          const auto track = *tracklist[i];
          ATH_MSG_DEBUG( "legs " << i + 1 << ": " << p[i].Pt() << " / " << p[i].Eta() << " / " << p[i].Phi() << " / " << track->charge() );
        }
      }

      auto mass = (std::accumulate(p.begin(), p.end(), xAOD::TrackParticle::GenVecFourMom_t())).M();
      ATH_MSG_DEBUG( "invariant mass: " << mass );

      mon_nCombination++;
      trkMassBeforeFit.push_back(mass * 0.001);
      if (!isInMassRange(mass, iTrk)) continue;

      mon_nCombinationBeforeFit++;
      auto fitterState = m_vertexFitter->makeState(state.context());
      auto vertex = fit(tracklist, m_trkMass[iTrk], *fitterState);
      if (!vertex) continue;
      xAOD::TrigBphys* trigBphys = makeTrigBPhys(*vertex, m_trkMass[iTrk], state.beamSpotPosition(), *fitterState);
      if (m_useLeptonMomentum) trigBphys->setMass(mass);
      state.addTrigBphysObject(trigBphys, leptonIndices);

      mon_nBPhysObject++;
      bphysMass.push_back(mass * 0.001);
      bphysCharge.push_back(charge);
      d0track1.push_back((*tracklist[0])->d0());
      d0track2.push_back((*tracklist[1])->d0());
      pttrack1.push_back((*tracklist[0])->pt() * 0.001);
      pttrack2.push_back((*tracklist[1])->pt() * 0.001);
      etatrack1.push_back((*tracklist[0])->eta());
      etatrack2.push_back((*tracklist[1])->eta());

    } while (std::prev_permutation(combination.begin(), combination.end()));
  }
  return StatusCode::SUCCESS;
}


StatusCode TrigMultiTrkComboHypo::processMergedElectrons(TrigMultiTrkState<xAOD::ElectronContainer>& state) const {

  ATH_MSG_DEBUG( "Try to find electrons originating from the same EM cluster" );

  // some electrons can be already attached to the list, add electrons from BPH-0DR3-EM7J15 clusters to the end
  auto& leptons = state.leptons();

  if (m_mergedElectronIDs.empty()) {
    ATH_MSG_DEBUG( "no chains similar to BPH-0DR3-EM7J15 have been requested, should not look for close-by electrons" );
    return StatusCode::SUCCESS;
  }

  const std::vector<double> particleMasses(2, PDG::mElectron);

  for (const Decision* decision : state.previousDecisions()) {
    if (!TrigCompositeUtils::isAnyIDPassing(decision, m_mergedElectronIDs)) continue;

    auto decisionEL = TrigCompositeUtils::decisionToElementLink(decision, state.context());
    ATH_CHECK( decision->hasObjectLink(TrigCompositeUtils::featureString(), ClassID_traits<xAOD::ElectronContainer>::ID()) );
    auto electronEL = decision->objectLink<xAOD::ElectronContainer>(TrigCompositeUtils::featureString());
    const auto electron = *electronEL;
    if (!electron->trackParticle()) continue;

    // get all electrons from the same SG::View, i.e. from the same initialRoI
    const auto electronContainer = electronEL.getStorableObjectPointer();
    if (electronContainer->size() <= 1) continue;

    // add electron from decision to state.leptons
    DecisionIDContainer decisionIDs;
    TrigCompositeUtils::decisionIDs(decision, decisionIDs);
    leptons.push_back({electronEL, ElementLinkVector<DecisionContainer>(1, decisionEL), decisionIDs});

    // get initialRoI this electron originating from
    auto roiInfo = TrigCompositeUtils::findLink<TrigRoiDescriptorCollection>(decision, TrigCompositeUtils::initialRoIString(), true);
    ATH_CHECK( roiInfo.isValid() );
    auto initialRoI = *roiInfo.link;

    // try to build di-electron pairs: first electron is always from decision, second from the same SG::View
    std::vector<ElementLink<xAOD::TrackParticleContainer>> tracklist(2);
    tracklist[0] = electron->trackParticleLink();
    for (size_t i = 0; i < electronContainer->size(); ++i) {
      const auto electronFromView = electronContainer->at(i);
      if (electronFromView == electron) continue;
      if (!electronFromView->trackParticle()) continue;
      tracklist[1] = electronFromView->trackParticleLink();

      auto fitterState = m_vertexFitter->makeState(state.context());
      auto vertex = fit(tracklist, particleMasses, *fitterState);
      if (!vertex) continue;
      xAOD::TrigBphys* trigBphys = makeTrigBPhys(*vertex, particleMasses, state.beamSpotPosition(), *fitterState);
      trigBphys->setRoiId(initialRoI->roiWord());
      state.addTrigBphysObject(trigBphys, std::vector<size_t>(1, leptons.size() - 1));
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode TrigMultiTrkComboHypo::findMuTrkCandidates(TrigMultiTrkState<xAOD::MuonContainer>& state) const {

  ATH_MSG_DEBUG( "Try to find muon + track combinations from the same SG::View" );

  auto& muons = state.leptons();
  muons.clear();

  const std::vector<double> particleMasses(2, PDG::mMuon);

  for (const Decision* decision : state.previousDecisions()) {
    if (!TrigCompositeUtils::isAnyIDPassing(decision, m_allowedIDs)) continue;

    auto decisionEL = TrigCompositeUtils::decisionToElementLink(decision, state.context());
    ATH_CHECK( decision->hasObjectLink(TrigCompositeUtils::featureString(), ClassID_traits<xAOD::MuonContainer>::ID()) );
    auto muonEL = decision->objectLink<xAOD::MuonContainer>(TrigCompositeUtils::featureString());
    const auto muon = *muonEL;
    if (!muon->trackParticle(xAOD::Muon::TrackParticleType::CombinedTrackParticle)) continue;
    if (!muon->trackParticle(xAOD::Muon::TrackParticleType::InnerDetectorTrackParticle)) continue;
    const auto muonInDetTrack = muon->trackParticle(xAOD::Muon::TrackParticleType::InnerDetectorTrackParticle);
    auto muonMomentum = muonInDetTrack->genvecP4();
    muonMomentum.SetM(PDG::mMuon);

    // add muon from decision to state.leptons
    DecisionIDContainer decisionIDs;
    TrigCompositeUtils::decisionIDs(decision, decisionIDs);
    muons.push_back({muonEL, ElementLinkVector<DecisionContainer>(1, decisionEL), decisionIDs});

    ATH_MSG_DEBUG( "Found muon (CombinedTrackParticle): " << muon->pt() << " / " << muon->eta() << " / " << muon->phi() << " / " << muon->charge() );

    auto viewLinkInfo = TrigCompositeUtils::findLink<ViewContainer>(decision, TrigCompositeUtils::viewString(), true);
    ATH_CHECK( viewLinkInfo.isValid() );
    auto view = *viewLinkInfo.link;

    auto tracksHandle = ViewHelper::makeHandle(view, m_trackParticleContainerKey, state.context());
    ATH_CHECK( tracksHandle.isValid() );
    ATH_MSG_DEBUG( "Tracks container " << m_trackParticleContainerKey << " size: " << tracksHandle->size() );

    // try to fit muon and track into common vertex: first track is always muon, second tracks comes from the same SG::View
    std::vector<ElementLink<xAOD::TrackParticleContainer>> tracklist(2);
    tracklist[0] = muon->inDetTrackParticleLink();
    for (size_t idx = 0; idx < tracksHandle->size(); ++idx) {
      const xAOD::TrackParticle* track = tracksHandle->at(idx);

      if (track->pt() < m_trkPt[0][1] || isIdenticalTracks(track, muonInDetTrack)) continue;
      auto trackMomentum = track->genvecP4();
      trackMomentum.SetM(PDG::mMuon);
      if (!isInMassRange((muonMomentum + trackMomentum).M(), 0)) continue;
      if (m_nTrkCharge[0] >= 0 && muonInDetTrack->charge() * track->charge() > 0.) continue;

      tracklist[1] = ViewHelper::makeLink<xAOD::TrackParticleContainer>(view, tracksHandle, idx);

      ATH_MSG_DEBUG( "Dump found muon+track pair before vertex fit: pT / eta / phi / charge" << endmsg <<
                     "   muon:  " << muonMomentum.Pt() << " / " << muonMomentum.Eta() << " / " << muonMomentum.Phi() << " / " << muon->charge() << endmsg <<
                     "   track: " << trackMomentum.Pt() << " / " << trackMomentum.Eta() << " / " << trackMomentum.Phi() << " / " << track->charge() );

      auto fitterState = m_vertexFitter->makeState(state.context());
      auto vertex = fit(tracklist, particleMasses, *fitterState);
      if (!vertex) continue;
      xAOD::TrigBphys* trigBphys = makeTrigBPhys(*vertex, particleMasses, state.beamSpotPosition(), *fitterState);
      // trigBphys->setRoiId(initialRoI->roiWord());
      state.addTrigBphysObject(trigBphys, std::vector<size_t>(1, muons.size() - 1));
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode TrigMultiTrkComboHypo::copyDecisionObjects(TrigMultiTrkStateBase& state) const {

  if (state.isEventAccepted()) {
    ATH_MSG_DEBUG( "Copying decisions from " << decisionsInput().at(0).key() << " to " << decisionsOutput().at(0).key() );
    for (const Decision* previousDecision : state.previousDecisions()) {
      if (!TrigCompositeUtils::isAnyIDPassing(previousDecision, m_allowedIDs)) continue;

      DecisionIDContainer previousDecisionIDs;
      TrigCompositeUtils::decisionIDs(previousDecision, previousDecisionIDs);
      DecisionIDContainer decisionIDs;
      std::set_intersection(previousDecisionIDs.begin(), previousDecisionIDs.end(), m_allowedIDs.begin(), m_allowedIDs.end(),
                            std::inserter(decisionIDs, decisionIDs.begin()));

      Decision* decision = TrigCompositeUtils::newDecisionIn(&state.decisions(), TrigCompositeUtils::comboHypoAlgNodeName());
      TrigCompositeUtils::linkToPrevious(decision, previousDecision, state.context());
      TrigCompositeUtils::insertDecisionIDs(decisionIDs, decision);
    }

    // copy additional decisions for combined chains, as 'HLT_e9_lhvloose_e5_lhvloose_bBeeM6000_mu4_L1BPH-0M9-EM7-EM5_MU6'
    if (!m_combineInputDecisionCollections) ATH_CHECK( copyAdditionalDecisionObjects(state) );
  }
  return StatusCode::SUCCESS;
}


StatusCode TrigMultiTrkComboHypo::createDecisionObjects(TrigMultiTrkStateBase& state) const {

  size_t idx = 0;
  for (const xAOD::TrigBphys* triggerObject : state.trigBphysCollection()) {
    ATH_MSG_DEBUG( "Found xAOD::TrigBphys: mass / chi2 = " << triggerObject->mass() << " / " << triggerObject->fitchi2() );

    auto triggerObjectEL = ElementLink<xAOD::TrigBphysContainer>(state.trigBphysCollection(), triggerObject->index());
    ATH_CHECK( triggerObjectEL.isValid() );

    // create a new output Decision object, backed by the 'decisions' container.
    Decision* decision = TrigCompositeUtils::newDecisionIn(&state.decisions(), TrigCompositeUtils::comboHypoAlgNodeName());

    std::vector<const DecisionIDContainer*> previousDecisionIDs;
    for (const size_t& i : state.getTrigBphysLegIndices(idx)) {

      // attach all previous decisions: if the same previous decision is called twice, that's fine - internally takes care of that
      // we already have an array of links to the previous decisions, so there is no need to use TrigCompositeUtils::linkToPrevious()
      decision->addObjectCollectionLinks(TrigCompositeUtils::seedString(), state.getDecisionLinks(i));
      previousDecisionIDs.push_back(&state.getDecisionIDs(i));
    }

    // set mandatory feature ElementLink to xAOD::TrigBphys object
    decision->setObjectLink<xAOD::TrigBphysContainer>(TrigCompositeUtils::featureString(), triggerObjectEL);
    decision->setDetail<int32_t>("noCombo", 1);

    for (const auto& tool : hypoTools()) {
      ATH_MSG_DEBUG( "Go to " << tool );
      ATH_CHECK( tool->decideOnSingleObject(decision, previousDecisionIDs) );
    }
    ++idx;
  }

  // copy additional decisions from Empty muon step for Combined chains, as 'HLT_e9_lhvloose_e5_lhvloose_bBeeM6000_mu4_L1BPH-0M9-EM7-EM5_MU6'
  ATH_CHECK( copyAdditionalDecisionObjects(state) );

  return StatusCode::SUCCESS;
}


StatusCode TrigMultiTrkComboHypo::copyAdditionalDecisionObjects(TrigMultiTrkStateBase& state) const {

  if (decisionsInput().size() > 1) {
    ATH_MSG_DEBUG( "Found more than one decision input key, decisionsInput().size = " << decisionsInput().size() );
    for (size_t i = 1; i < decisionsInput().size(); ++i) {
      ATH_MSG_DEBUG( "Copying decisions from " << decisionsInput().at(i).key() << " to " << decisionsOutput().at(i).key() );
      auto previousDecisionsHandle = SG::makeHandle(decisionsInput().at(i), state.context());
      CHECK( previousDecisionsHandle.isValid() );
      ATH_MSG_DEBUG( "Running with "<< previousDecisionsHandle->size() << " previous decisions" );
      SG::WriteHandle<DecisionContainer> outputDecisionsHandle = TrigCompositeUtils::createAndStore(decisionsOutput().at(i), state.context());
      for (const Decision* previousDecision : *previousDecisionsHandle) {
        if (!TrigCompositeUtils::isAnyIDPassing(previousDecision, m_allowedIDs)) continue;

        DecisionIDContainer previousDecisionIDs;
        TrigCompositeUtils::decisionIDs(previousDecision, previousDecisionIDs);
        DecisionIDContainer decisionIDs;
        std::set_intersection(previousDecisionIDs.begin(), previousDecisionIDs.end(), m_allowedIDs.begin(), m_allowedIDs.end(),
                              std::inserter(decisionIDs, decisionIDs.begin()));

        Decision* decision = TrigCompositeUtils::newDecisionIn(outputDecisionsHandle.ptr(), TrigCompositeUtils::comboHypoAlgNodeName());
        TrigCompositeUtils::linkToPrevious(decision, previousDecision, state.context());
        TrigCompositeUtils::insertDecisionIDs(decisionIDs, decision);
      }
    }
  }
  return StatusCode::SUCCESS;
}


std::unique_ptr<xAOD::Vertex> TrigMultiTrkComboHypo::fit(
    const std::vector<ElementLink<xAOD::TrackParticleContainer>>& trackParticleLinks,
    const std::vector<double>& particleMasses,
    Trk::IVKalState& fitterState) const {

  ATH_MSG_DEBUG( "Perform vertex fit" );
  std::vector<const xAOD::TrackParticle*> tracklist(trackParticleLinks.size(), nullptr);
  std::transform(trackParticleLinks.begin(), trackParticleLinks.end(), tracklist.begin(),
                 [](const ElementLink<xAOD::TrackParticleContainer>& link){ return *link; });

  const Trk::Perigee& perigee1 = tracklist[0]->perigeeParameters();
  const Trk::Perigee& perigee2 = tracklist[1]->perigeeParameters();
  int flag = 0;
  int errorcode = 0;
  Amg::Vector3D startingPoint = m_vertexPointEstimator->getCirclesIntersectionPoint(&perigee1, &perigee2, flag, errorcode);
  if (errorcode != 0) startingPoint = Amg::Vector3D::Zero(3);
  ATH_MSG_DEBUG( "Starting point: (" << startingPoint(0) << ", " << startingPoint(1) << ", " << startingPoint(2) << ")" );

  m_vertexFitter->setMassInputParticles(particleMasses, fitterState);
  std::unique_ptr<xAOD::Vertex> vertex(m_vertexFitter->fit(tracklist, startingPoint, fitterState));
  if (!vertex) {
    ATH_MSG_DEBUG( "Vertex fit fails" );
    return vertex;
  }
  if (vertex->chiSquared() > m_chi2) {
    ATH_MSG_DEBUG( "Fit is successful, but vertex chi2 is too high, we are not going to save it (chi2 = " << vertex->chiSquared() << " > " << m_chi2.value() << ")" );
    vertex.reset();
    return vertex;
  }
  ATH_MSG_DEBUG( "Fit is successful" );
  vertex->clearTracks();
  vertex->setTrackParticleLinks(trackParticleLinks);
  return vertex;
}


xAOD::TrigBphys* TrigMultiTrkComboHypo::makeTrigBPhys(
    const xAOD::Vertex& vertex,
    const std::vector<double>& particleMasses,
    const Amg::Vector3D& beamSpot,
    const Trk::IVKalState& fitterState) const {

  double invariantMass = 0.;
  double invariantMassError = 0.;
  if (!m_vertexFitter->VKalGetMassError(invariantMass, invariantMassError, fitterState).isSuccess()) {
    ATH_MSG_DEBUG( "Warning from TrkVKalVrtFitter: can not calculate uncertainties" );
    invariantMass = -9999.;
  }

  xAOD::TrackParticle::GenVecFourMom_t momentum;
  for (size_t i = 0; i < vertex.nTrackParticles(); ++i) {
    auto p = vertex.trackParticle(i)->genvecP4();
    p.SetM(particleMasses[i]);
    momentum += p;
  }

  xAOD::TrigBphys* result = new xAOD::TrigBphys();
  result->makePrivateStore();
  result->initialise(0, momentum.Eta(), momentum.Phi(), momentum.Pt(), xAOD::TrigBphys::MULTIMU, momentum.M(), xAOD::TrigBphys::EF);

  if (m_doElectrons) result->setParticleType(xAOD::TrigBphys::JPSIEE);
  result->setFitmass(invariantMass);
  result->setFitchi2(vertex.chiSquared());
  result->setFitndof(vertex.numberDoF());
  result->setFitx(vertex.x());
  result->setFity(vertex.y());
  result->setFitz(vertex.z());
  result->setTrackParticleLinks(vertex.trackParticleLinks());
  result->setLxy(Lxy(*result, beamSpot));

  ATH_MSG_DEBUG(
    "TrigBphys objects:\n\t  " <<
    "roiId:         " << result->roiId()  << "\n\t  " <<
    "particleType:  " << result->particleType() << "\n\t  " <<
    "level:         " << result->level() << "\n\t  " <<
    "eta:           " << result->eta() << "\n\t  " <<
    "phi:           " << result->phi() << "\n\t  " <<
    "mass:          " << result->mass() << "\n\t  " <<
    "fitmass:       " << result->fitmass() << "\n\t  " <<
    "chi2/NDF:      " << result->fitchi2() << " / " << result->fitndof() << "\n\t  " <<
    "vertex:        (" << result->fitx() << ", " << result->fity() << ", " << result->fitz() << ")\n\t  " <<
    "Lxy:           " << result->lxy() );

  return result;
}


bool TrigMultiTrkComboHypo::isIdenticalTracks(const xAOD::TrackParticle* lhs, const xAOD::TrackParticle* rhs) const {

  if (lhs->charge() * rhs->charge() < 0.) return false;
  return (ROOT::Math::VectorUtil::DeltaR(lhs->genvecP4(), rhs->genvecP4()) < m_deltaR);
}


bool TrigMultiTrkComboHypo::isIdenticalTracks(const xAOD::Muon* lhs, const xAOD::Muon* rhs) const {

  return isIdenticalTracks(*lhs->inDetTrackParticleLink(), *rhs->inDetTrackParticleLink());
}

bool TrigMultiTrkComboHypo::isIdenticalTracks(const xAOD::Electron* lhs, const xAOD::Electron* rhs) const {

  return isIdenticalTracks(*lhs->trackParticleLink(), *rhs->trackParticleLink());
}


float TrigMultiTrkComboHypo::Lxy(const xAOD::TrigBphys& vertex, const Amg::Vector3D& beamSpot) const {

  XYVector R(vertex.fitx() - beamSpot.x(), vertex.fity() - beamSpot.y());
  const auto& trackParticleLinks = vertex.trackParticleLinks();
  auto pT = std::accumulate(trackParticleLinks.begin(), trackParticleLinks.end(), XYVector(),
                            [](const auto& pT, const auto& trackEL){ const auto& p = (*trackEL)->genvecP4(); return pT + XYVector(p.x(), p.y()); });
  return R.Dot(pT.unit());
}


bool TrigMultiTrkComboHypo::isInMassRange(double mass, size_t idx) const {

  const auto& range = m_massRange[idx];
  return (mass > range.first && mass < range.second);
}


bool TrigMultiTrkComboHypo::passedDeltaRcut(const std::vector<xAOD::TrackParticle::GenVecFourMom_t>& p) const {

  if (m_deltaRMax == std::numeric_limits<float>::max() && m_deltaRMin == std::numeric_limits<float>::lowest()) {
    return true;
  }
  for (size_t i = 0; i < p.size(); ++i) {
    for (size_t j = i + 1; j < p.size(); ++j) {
      double deltaR = ROOT::Math::VectorUtil::DeltaR(p[i], p[j]);
      if (deltaR > m_deltaRMax || deltaR < m_deltaRMin) return false;
    }
  }
  return true;
}
