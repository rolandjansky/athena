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

TrigMultiTrkComboHypo::TrigMultiTrkComboHypo(const std::string& name, ISvcLocator* pSvcLocator)
    : ::ComboHypo(name, pSvcLocator) {}


StatusCode TrigMultiTrkComboHypo::initialize() {
  ATH_MSG_DEBUG( "TrigMultiTrkHypo::initialize()" );

  ATH_CHECK( ::ComboHypo::initialize() );

  // check consistency of the properties
  if (m_trkMass.size() != m_nTrk) {
    ATH_MSG_ERROR( "Requested " << m_nTrk.value() << " tracks per vertex, but only provided " << m_trkMass.size() << " track masses");
    return StatusCode::FAILURE;
  }
  if (m_trkPt.size() != m_nTrk) {
    ATH_MSG_ERROR( "Requested " << m_nTrk.value() << " tracks per vertex, but only provided " << m_trkPt.size() << " track pT thresholds");
    return StatusCode::FAILURE;
  }

  for (const auto& range : m_massRange.value()) {
    if (range.first > range.second) {
      ATH_MSG_ERROR( "Mass range incorrect values: {Mi_min, Mi_max}" );
      return StatusCode::FAILURE;
    }
  }

  // dump numerical values
  if (msgLvl(MSG::DEBUG)) {
    msg() << MSG::DEBUG << "vertex topology:" << std::endl;
    for (size_t i = 0; i < m_nTrk; ++i) {
      msg() << MSG::DEBUG << "  " << i << "  trk: mass = " <<  m_trkMass[i] << ", Pt > " << m_trkPt[i] << std::endl;
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

  if (m_trigLevel == "L2") {
    ATH_CHECK( m_trackParticleContainerKey.initialize() );
    renounce(m_trackParticleContainerKey);
  }
  else if (m_trigLevel == "L2IO" || m_trigLevel == "EF") {
    ATH_CHECK( m_trackParticleContainerKey.initialize(false) );
  }
  else {
    ATH_MSG_ERROR( "trigLevel should be L2, L2IO or EF, but " << m_trigLevel << " provided" );
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

  // add IDs to ComboHypoTools to check that each trigger leg fulfill TrigCompositeUtils::passed() requirement
  for (auto& tool : hypoTools()) {
    const HLT::Identifier id = tool->decisionId();
    const auto itr = triggerMultiplicityMap().find(id.name());
    if (itr == triggerMultiplicityMap().end()) {
      ATH_MSG_ERROR( "No entry found for " << tool->name() << " in triggerMultiplicityMap" );
    }
    const std::vector<int>& multiplicity = itr->second;
    std::vector<HLT::Identifier> legDecisionIds;
    if (multiplicity.size() == 1) {
      legDecisionIds.assign(multiplicity[0], id);
    }
    else {
      size_t n = static_cast<size_t>(std::accumulate(multiplicity.begin(), multiplicity.end(), 0));
      legDecisionIds.reserve(n);
      for (size_t i = 0; i < n; i++) {
        legDecisionIds.push_back(TrigCompositeUtils::createLegName(id, i));
      }
    }
    if (msgLvl(MSG::DEBUG)) {
      ATH_MSG_DEBUG( "Leg decisions for tool " << tool->name() );
      for (const auto& legDecisionId : legDecisionIds) {
        ATH_MSG_DEBUG( " +++ " << legDecisionId );
      }
    }
    tool->setLegDecisionIds(std::move(legDecisionIds));
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

  // create the mutable output DecisionContainer and register it to StoreGate
  SG::WriteHandle<DecisionContainer> outputDecisionsHandle = TrigCompositeUtils::createAndStore(decisionsOutput().at(0), context);

  std::unique_ptr<TrigMultiTrkStateCand<xAOD::MuonContainer>> muonstate;
  std::unique_ptr<TrigMultiTrkStateCand<xAOD::ElectronContainer>> electronstate;
  TrigMultiTrkState* commonstate=nullptr;
  if(m_doElectrons == true){
      electronstate = std::make_unique<TrigMultiTrkStateCand<xAOD::ElectronContainer>>();
      commonstate = electronstate.get();
  }else{
      muonstate = std::make_unique<TrigMultiTrkStateCand<xAOD::MuonContainer>>();
      commonstate = muonstate.get();
  }
  FillState(commonstate, &context, previousDecisionsHandle.cptr(), outputDecisionsHandle.ptr(), nullptr);

  if (m_isStreamer) {
    if (m_trigLevel == "L2") {
      ATH_CHECK( mergeTracksFromViews(*commonstate) );
    }
    else {
      ATH_CHECK( mergeTracksFromDecisions(*commonstate) );
    }
    ATH_CHECK( filterTrackCombinations(*commonstate) );
    ATH_CHECK( copyDecisionObjects(*commonstate) );
  }
  else {

    auto trigBphysHandle = SG::makeHandle(m_trigBphysContainerKey, context);
    ATH_CHECK( trigBphysHandle.record(std::make_unique<xAOD::TrigBphysContainer>(),
                                      std::make_unique<xAOD::TrigBphysAuxContainer>()) );
    commonstate->trigBphysCollection = trigBphysHandle.ptr();

    if(m_doElectrons == true){
        ATH_CHECK( mergeFromDecisions(*electronstate) );
        ATH_CHECK( findDiTrackCandidates(*electronstate) );
    } else{
        ATH_CHECK( mergeFromDecisions(*muonstate) );
        ATH_CHECK( findDiTrackCandidates(*muonstate) );
    }
    ATH_CHECK( createDecisionObjects(*commonstate) );
  }

  ATH_MSG_DEBUG( "TrigMultiTrkHypo::execute() terminates with StatusCode::SUCCESS" );
  return StatusCode::SUCCESS;
}


void TrigMultiTrkComboHypo::FillState(TrigMultiTrkState* state,
                                                                    const EventContext* context,
                                                                    const DecisionContainer* previousDecisions,
                                                                    DecisionContainer* decisions,
                                                                    xAOD::TrigBphysContainer* trigBphysCollection) const {
  state->context = context;
  state->previousDecisions = previousDecisions;
  state->decisions = decisions;
  state->trigBphysCollection = trigBphysCollection;
}

template<typename T>
StatusCode TrigMultiTrkComboHypo::mergeFromDecisions(TrigMultiTrkStateCand<T>& state) const {

  auto& leptons = state.LegList;
  leptons.clear();

  // all muons from views are already connected with previous decisions by TrigMuonEFHypoAlg
  for (const Decision* decision : *state.previousDecisions) {
    if (!TrigCompositeUtils::isAnyIDPassing(decision, m_allowedIDs)) continue;

    ATH_CHECK( decision->hasObjectLink(TrigCompositeUtils::featureString(), ClassID_traits<T>::ID()) );
    auto leptonEL = decision->objectLink<T>(TrigCompositeUtils::featureString());
    const auto lepton = *leptonEL;
    if constexpr (std::is_same<T, xAOD::MuonContainer>::value){
       if (!lepton->trackParticle(xAOD::Muon::TrackParticleType::CombinedTrackParticle)) continue;
       if (!lepton->trackParticle(xAOD::Muon::TrackParticleType::InnerDetectorTrackParticle)) continue;
    } else {
       if (!lepton->trackParticle()) continue;
    }
    auto decisionEL = TrigCompositeUtils::decisionToElementLink(decision, *state.context);
    auto itr = std::find_if(leptons.begin(), leptons.end(), [this, lepton = lepton](const auto& x){ return this->isIdenticalTracks(lepton, *x.link); });
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
      if constexpr (std::is_same<T, xAOD::MuonContainer>::value){
         track = *lepton->inDetTrackParticleLink();
      }else{
         track = *lepton->trackParticleLink();
      }
      ATH_MSG_DEBUG( " -- lepton InDetTrackParticle pt/eta/phi/q: " << track->pt() << " / " << track->eta() << " / " << track->phi() << " / " << track->charge() );
      ATH_MSG_DEBUG( "    lepton pt (muon: CombinedTrackParticle): " << lepton->pt() );
      ATH_MSG_DEBUG( "    allowed decisions:" );
      for (const DecisionID& id : item.decisionIDs) {
        ATH_MSG_DEBUG( "    +++ " << HLT::Identifier(id) );
      }
    }
  }
  return StatusCode::SUCCESS;
}




StatusCode TrigMultiTrkComboHypo::mergeTracksFromViews(TrigMultiTrkState& state) const {

  auto& tracks = state.tracks;
  tracks.clear();

  size_t viewCounter = 0;
  for (const Decision* decision : *state.previousDecisions) {
    if (!TrigCompositeUtils::isAnyIDPassing(decision, m_allowedIDs)) continue;

    auto viewLinkInfo = TrigCompositeUtils::findLink<ViewContainer>(decision, TrigCompositeUtils::viewString(), true);
    ATH_CHECK( viewLinkInfo.isValid() );
    auto viewEL = viewLinkInfo.link;

    auto tracksHandle = ViewHelper::makeHandle(*viewEL, m_trackParticleContainerKey, *state.context);
    ATH_CHECK( tracksHandle.isValid() );
    ATH_MSG_DEBUG( "tracks handle " << m_trackParticleContainerKey << " size: " << tracksHandle->size() );

    std::vector<ElementLink<xAOD::TrackParticleContainer>> tracksFromView;
    tracksFromView.reserve(tracksHandle->size());
    for (size_t idx = 0; idx < tracksHandle->size(); ++idx) {
      tracksFromView.emplace_back(ViewHelper::makeLink<xAOD::TrackParticleContainer>(*viewEL, tracksHandle, idx));
    }

    for (const auto& trackEL : tracksFromView) {
      const xAOD::TrackParticle* track = *trackEL;
      if (track->definingParametersCovMatrixVec().empty() || track->pt() < m_trkPt.value().back()) continue;

      if (viewCounter == 0 ||
          std::find_if(tracks.begin(), tracks.end(),
                       [this,track = track](const auto& x){ return isIdenticalTracks(track, *x); }) == tracks.end()) {
        tracks.emplace_back(trackEL);
      }
    }
    viewCounter++;
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


StatusCode TrigMultiTrkComboHypo::mergeTracksFromDecisions(TrigMultiTrkState& state) const {

  auto& tracks = state.tracks;
  tracks.clear();

  // all muons from views are already connected with previous decisions by TrigMuonEFHypoAlg
  for (const Decision* decision : *state.previousDecisions) {
    if (!TrigCompositeUtils::isAnyIDPassing(decision, m_allowedIDs)) continue;

    ElementLink<xAOD::TrackParticleContainer> trackEL;
    if (m_trigLevel == "EF") {
      ATH_CHECK( decision->hasObjectLink(TrigCompositeUtils::featureString(), ClassID_traits<xAOD::MuonContainer>::ID()) );
      auto muonEL = decision->objectLink<xAOD::MuonContainer>(TrigCompositeUtils::featureString());
      const xAOD::Muon* muon = *muonEL;
      if (!muon->trackParticle(xAOD::Muon::TrackParticleType::CombinedTrackParticle)) continue;
      if (!muon->trackParticle(xAOD::Muon::TrackParticleType::InnerDetectorTrackParticle)) continue;
      trackEL = muon->inDetTrackParticleLink();
    }
    else if (m_trigLevel == "L2IO") {
      ATH_CHECK( decision->hasObjectLink(TrigCompositeUtils::featureString(), ClassID_traits<xAOD::L2CombinedMuonContainer>::ID()) );
      auto muonEL = decision->objectLink<xAOD::L2CombinedMuonContainer>(TrigCompositeUtils::featureString());
      const xAOD::L2CombinedMuon* muon = *muonEL;
      trackEL = muon->idTrackLink();
    }
    else {
      ATH_MSG_ERROR( "mergeTracksFromDecisions() expects trigLevel to be L2IO or EF, but " << m_trigLevel << " provided" );
      return StatusCode::FAILURE;
    }
    if (!trackEL.isValid()) continue;

    if (std::find_if(tracks.begin(), tracks.end(),
                     [this, track = *trackEL](const auto& x){ return isIdenticalTracks(track, *x); }) == tracks.end()) {
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


StatusCode TrigMultiTrkComboHypo::filterTrackCombinations(TrigMultiTrkState& state) const {

  state.isEventAccepted = 0;
  auto& tracks = state.tracks;

  // monitored variables
  auto mon_nAcceptedTrk = Monitored::Scalar<int>("nAcceptedTrk", tracks.size());
  auto mon_nVertexFitterCalls = Monitored::Scalar<int>("nVertexFitterCalls", 0);
  auto mon_isEventAccepted = Monitored::Scalar<int>("acceptance", 0);
  auto mon_timer = Monitored::Timer( "TIME_all" );

  auto group = Monitored::Group(m_monTool,
    mon_nAcceptedTrk, mon_nVertexFitterCalls, mon_isEventAccepted,
    mon_timer);

  // combination is a selection of items from a collection, such that the order of selection does not matter
  // tracks have already been sorted over pT, the algorithm will keep this order in created subsets of tracks, i.e.
  // for m_nTrk = 2 combinations are [0, 1], [0, 2], ..., [1, 2], [1, 3], ...
  // for m_nTrk = 3 combinations are [0, 1, 2], [0, 1, 3], ..., [0, 2, 3], [0, 2, 4], ..., [1, 2, 3], [1, 2, 4], ...

  if (tracks.size() < m_nTrk) {
    ATH_MSG_DEBUG( "Could not build a subset of " << m_nTrk.value() << " tracks from collection which contains only " << tracks.size() << " objects" );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG( "Consider combinations of " << m_nTrk.value() << " tracks from collection which contains " << tracks.size() << " objects until find a good one" );

  std::vector<ElementLink<xAOD::TrackParticleContainer>> tracklist(m_nTrk);
  std::vector<xAOD::TrackParticle::GenVecFourMom_t> p(m_nTrk);

  // tracks from the current combination will have non-zero value against their position in the 'idx' vector
  // consider first m_nTrk tracks as an initial combination, then get the next one with std::prev_permutation()
  std::vector<char> idx(tracks.size(), 0);
  std::fill(idx.begin(), idx.begin() + m_nTrk, 1);
  do {
    // fill tracklist and momenta of tracks, also check that the track pT passes the threshold value
    bool isValidCombination = true;
    size_t j = 0;
    for (size_t i = 0; i < idx.size(); ++i) {
      if (!idx[i]) continue;
      const auto& trackEL = tracks[i];
      tracklist[j] = trackEL;
      p[j] = (*trackEL)->genvecP4();
      p[j].SetM(m_trkMass[j]);
      if (p[j].Pt() < m_trkPt[j]) {
        isValidCombination = false;
        break;
      }
      ++j;
    }
    if (!isValidCombination) continue;

    if (msgLvl(MSG::DEBUG)) {
      ATH_MSG_DEBUG( "Dump found tracks before vertex fit: pT / eta / phi / charge" );
      for (size_t i = 0; i < tracklist.size(); ++i) {
        const auto track = *tracklist[i];
        ATH_MSG_DEBUG( "track " << i + 1 << ": " << p[i].Pt() << " / " << p[i].Eta() << " / " << p[i].Phi() << " / " << track->charge() );
      }
    }

    auto mass = (std::accumulate(p.begin(), p.end(), xAOD::TrackParticle::GenVecFourMom_t())).M();
    ATH_MSG_DEBUG( "invariant mass: " << mass );

    if (!isInMassRange(mass)) continue;

    auto fitterState = m_vertexFitter->makeState(*state.context);
    std::unique_ptr<xAOD::Vertex> vertex(fit(tracklist, fitterState.get()));
    ++mon_nVertexFitterCalls;
    if (!vertex) continue;

    ATH_MSG_DEBUG( "Filter found a subset of tracks which passed the rough selection: stop looking for other combinations" );
    state.isEventAccepted = 1;
    break;

  } while (std::prev_permutation(idx.begin(), idx.end()));

  if (!state.isEventAccepted) {
    ATH_MSG_DEBUG( "Filter could not find a good subset of tracks" );
  }

  mon_isEventAccepted = state.isEventAccepted;

  return StatusCode::SUCCESS;
}

template<typename T>
StatusCode TrigMultiTrkComboHypo::findDiTrackCandidates(TrigMultiTrkStateCand<T>& state) const {

  state.trigBphysIndices.clear();
  auto& legs = state.LegList;

  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle {m_beamSpotKey, *state.context};
  ATH_CHECK( beamSpotHandle.isValid() );

  // monitored variables
  auto mon_nAcceptedTrk = Monitored::Scalar<int>("nAcceptedTrk", legs.size());
  auto mon_nCombination = Monitored::Scalar<int>("nCombination", 0);
  auto mon_nCombinationBeforeFit = Monitored::Scalar<int>("nCombinationBeforeFit", 0);
  auto mon_nBPhysObject = Monitored::Scalar<int>("nBPhysObject", 0);

  std::vector<float> trkMassBeforeFit;
  std::vector<float> bphysMass;
  std::vector<float> d0track1, d0track2;
  std::vector<int> bphysCharge;
  auto mon_trkMassBeforeFit = Monitored::Collection("trkMassBeforeFit", trkMassBeforeFit);
  auto mon_bphysChi2 = Monitored::Collection("bphysChi2", *state.trigBphysCollection, &xAOD::TrigBphys::fitchi2);
  auto mon_bphysLxy = Monitored::Collection("bphysLxy", *state.trigBphysCollection, &xAOD::TrigBphys::lxy);
  auto mon_bphysFitMass = Monitored::Collection("bphysFitMass", *state.trigBphysCollection, [](const xAOD::TrigBphys* x){ return x->fitmass()*0.001; });
  auto mon_bphysMass = Monitored::Collection("bphysMass", bphysMass);
  auto mon_d0track1 = Monitored::Collection("d0track1", d0track1);
  auto mon_d0track2 = Monitored::Collection("d0track2", d0track2);
  auto mon_bphysCharge = Monitored::Collection("bphysCharge", bphysCharge);

  auto mon_timer = Monitored::Timer( "TIME_all" );

  auto group = Monitored::Group(m_monTool,
    mon_nAcceptedTrk, mon_nCombination, mon_nCombinationBeforeFit, mon_nBPhysObject,
    mon_trkMassBeforeFit, mon_bphysChi2, mon_bphysLxy, mon_bphysFitMass, mon_bphysMass, mon_bphysCharge, mon_d0track1, mon_d0track2,
    mon_timer);

  if (legs.size() < m_nTrk) {
    ATH_MSG_DEBUG( "Could not build a subset of " << m_nTrk.value() << " legs from collection which contains only " << legs.size() << " objects" );
    return StatusCode::SUCCESS;
  }
  ATH_MSG_DEBUG( "Consider all combinations of " << m_nTrk.value() << "legs from collection which contains " << legs.size() << " objects" );

  std::vector<size_t> leptonIndices(m_nTrk);
  std::vector<ElementLink<xAOD::TrackParticleContainer>> tracklist(m_nTrk);
  std::vector<xAOD::TrackParticle::GenVecFourMom_t> p(m_nTrk);

  std::vector<char> leptonTags(legs.size(), 0);
  std::fill(leptonTags.begin(), leptonTags.begin() + m_nTrk, 1);
  do {
    // fill tracklist and momenta of muons in the current subset
    bool isValidCombination = true;
    int charge = 0;
    size_t j = 0;
    for (size_t i = 0; i < leptonTags.size(); ++i) {
      if (!leptonTags[i]) continue;
      leptonIndices[j] = i;
      auto leg = *legs[i].link;
      charge += static_cast<int>(lround(leg->charge()));
      ElementLink<xAOD::TrackParticleContainer> trackEL;
      if constexpr(std::is_same<T, xAOD::MuonContainer>::value){
         trackEL = leg->inDetTrackParticleLink();
      } else{
         trackEL = leg->trackParticleLink();
      }
      tracklist[j] = trackEL;
      p[j] = (*trackEL)->genvecP4();
      p[j].SetM(m_trkMass[j]);
      if (p[j].Pt() < m_trkPt[j]) {
        isValidCombination = false;
        break;
      }
      ++j;
    }
    if (!isValidCombination) continue;

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
    if (!isInMassRange(mass)) continue;

    mon_nCombinationBeforeFit++;
    auto fitterState = m_vertexFitter->makeState(*state.context);
    std::unique_ptr<xAOD::Vertex> vertex(fit(tracklist, fitterState.get()));
    if (!vertex) continue;
    xAOD::TrigBphys* trigBphys = makeTrigBPhys(vertex.get(), fitterState.get(), beamSpotHandle->beamPos());
    state.trigBphysCollection->push_back(trigBphys);
    state.trigBphysIndices.push_back(leptonIndices);

    mon_nBPhysObject++;
    bphysMass.push_back(mass * 0.001);
    bphysCharge.push_back(charge);
    d0track1.push_back((*tracklist[0])->d0());
    d0track2.push_back((*tracklist[1])->d0());

  } while (std::prev_permutation(leptonTags.begin(), leptonTags.end()));

  return StatusCode::SUCCESS;
}



StatusCode TrigMultiTrkComboHypo::copyDecisionObjects(TrigMultiTrkState& state) const {

  if (state.isEventAccepted) {
    ATH_MSG_DEBUG( "Copying decisions from " << decisionsInput().at(0).key() << " to " << decisionsOutput().at(0).key() );
    for (const Decision* previousDecision : *state.previousDecisions) {
      if (!TrigCompositeUtils::isAnyIDPassing(previousDecision, m_allowedIDs)) continue;

      DecisionIDContainer previousDecisionIDs;
      TrigCompositeUtils::decisionIDs(previousDecision, previousDecisionIDs);
      DecisionIDContainer decisionIDs;
      std::set_intersection(previousDecisionIDs.begin(), previousDecisionIDs.end(), m_allowedIDs.begin(), m_allowedIDs.end(),
                            std::inserter(decisionIDs, decisionIDs.begin()));

      Decision* decision = TrigCompositeUtils::newDecisionIn(state.decisions, TrigCompositeUtils::comboHypoAlgNodeName());
      TrigCompositeUtils::linkToPrevious(decision, previousDecision, *state.context);
      TrigCompositeUtils::insertDecisionIDs(decisionIDs, decision);
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode TrigMultiTrkComboHypo::createDecisionObjects(TrigMultiTrkState& state) const {

  size_t idx = 0;
  for (const xAOD::TrigBphys* triggerObject : *state.trigBphysCollection) {
    ATH_MSG_DEBUG( "Found xAOD::TrigBphys: mass / chi2 = " << triggerObject->mass() << " / " << triggerObject->fitchi2() );

    auto triggerObjectEL = ElementLink<xAOD::TrigBphysContainer>(*state.trigBphysCollection, triggerObject->index());
    ATH_CHECK( triggerObjectEL.isValid() );

    // create a new output Decision object, backed by the 'decisions' container.
    Decision* decision = TrigCompositeUtils::newDecisionIn(state.decisions, TrigCompositeUtils::comboHypoAlgNodeName());

    std::vector<const DecisionIDContainer*> previousDecisionIDs;
    for (const size_t& i : state.trigBphysIndices[idx]) {

          // attach all previous decisions: if the same previous decision is called twice, that's fine - internally takes care of that
          // we already have an array of links to the previous decisions, so there is no need to use TrigCompositeUtils::linkToPrevious()
          decision->addObjectCollectionLinks(TrigCompositeUtils::seedString(), state.getDecisionLinks(i));
          previousDecisionIDs.push_back(&state.getDecisionID(i));
    }

    // set mandatory feature ElementLink to xAOD::TrigBphys object
    decision->setObjectLink<xAOD::TrigBphysContainer>(TrigCompositeUtils::featureString(), triggerObjectEL);

    for (const auto& tool : hypoTools()) {
      ATH_MSG_DEBUG( "Go to " << tool );
      ATH_CHECK( tool->decideOnSingleObject(decision, previousDecisionIDs) );
    }
    ++idx;
  }

  return StatusCode::SUCCESS;
}


xAOD::Vertex* TrigMultiTrkComboHypo::fit(const std::vector<ElementLink<xAOD::TrackParticleContainer>>& trackParticleLinks,
         Trk::IVKalState* fitterState) const {

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

  m_vertexFitter->setMassInputParticles(m_trkMass, *fitterState);
  xAOD::Vertex* vertex = m_vertexFitter->fit(tracklist, startingPoint, *fitterState);
  if (!vertex) {
    ATH_MSG_DEBUG( "Vertex fit fails" );
    return nullptr;
  }
  if (vertex->chiSquared() > m_chi2) {
    ATH_MSG_DEBUG( "Fit is successful, but vertex chi2 is too high, we are not going to save it (chi2 = " << vertex->chiSquared() << " > " << m_chi2.value() << ")" );
    delete vertex;
    return nullptr;
  }
  ATH_MSG_DEBUG( "Fit is successful" );
  vertex->clearTracks();
  vertex->setTrackParticleLinks(trackParticleLinks);
  return vertex;
}


xAOD::TrigBphys* TrigMultiTrkComboHypo::makeTrigBPhys(xAOD::Vertex* vertex, Trk::IVKalState* fitterState, const Amg::Vector3D& beamspot) const {
  double invariantMass = 0.;
  double invariantMassError = 0.;
  if (!m_vertexFitter->VKalGetMassError(invariantMass, invariantMassError, *fitterState).isSuccess()) {
    ATH_MSG_DEBUG( "Warning from TrkVKalVrtFitter: can not calculate uncertainties" );
    invariantMass = -9999.;
  }

  xAOD::TrackParticle::GenVecFourMom_t momentum(0, 0, 0, 0);
  for (size_t i = 0; i < vertex->nTrackParticles(); ++i) {
    auto p = vertex->trackParticle(i)->genvecP4();
    p.SetM(m_trkMass[i]);
    momentum += p;
  }

  xAOD::TrigBphys* result = new xAOD::TrigBphys();
  result->makePrivateStore();
  result->initialise(0, momentum.Eta(), momentum.Phi(), momentum.Pt(), xAOD::TrigBphys::MULTIMU, momentum.M(), xAOD::TrigBphys::EF);

  result->setFitmass(invariantMass);
  result->setFitchi2(vertex->chiSquared());
  result->setFitndof(vertex->numberDoF());
  result->setFitx(vertex->x());
  result->setFity(vertex->y());
  result->setFitz(vertex->z());
  result->setTrackParticleLinks(vertex->trackParticleLinks());
  result->setLxy(Lxy(result, beamspot));

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
    "vertex:        (" << result->fitx() << ", " << result->fity() << ", " << result->fitz() << "\n\t  " <<
    "Lxy            " << result->lxy() << ")" );

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


float TrigMultiTrkComboHypo::Lxy(const xAOD::TrigBphys* vertex, const Amg::Vector3D& beamSpot) const {

  XYVector R(vertex->fitx() - beamSpot.x(), vertex->fity() - beamSpot.y());
  const auto& trackParticleLinks = vertex->trackParticleLinks();
  auto pT = std::accumulate(trackParticleLinks.begin(), trackParticleLinks.end(), XYVector(),
                            [](const auto& pT, const auto& trackEL){ const auto& p = (*trackEL)->genvecP4(); return pT + XYVector(p.x(), p.y()); });
  return R.Dot(pT.unit());
}


bool TrigMultiTrkComboHypo::isInMassRange(double mass) const {

  bool result = false;
  for (const auto& range : m_massRange) {
    if (mass > range.first && mass < range.second) {
      result = true;
      break;
    }
  }
  return result;
}

TrigMultiTrkState::~TrigMultiTrkState(){ }
