/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include <numeric>
#include <iterator>

#include "TrigBmumuxComboHypo.h"

#include "xAODMuon/Muon.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigBphys/TrigBphys.h"
#include "xAODTrigBphys/TrigBphysContainer.h"
#include "xAODTrigBphys/TrigBphysAuxContainer.h"

#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigCompositeUtils/HLTIdentifier.h"

#include "AthViews/View.h"
#include "AthViews/ViewHelper.h"
#include "AthContainers/AuxElement.h"

#include "Math/Vector4D.h"
#include "Math/GenVector/VectorUtil.h"

using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::DecisionID;
using TrigCompositeUtils::DecisionIDContainer;
using TrigCompositeUtils::comboHypoAlgNodeName;

using GenVecFourMom_t = ROOT::Math::LorentzVector<ROOT::Math::PxPyPzM4D<double>>;


namespace {
static const SG::AuxElement::Accessor<std::vector<size_t>> muonIndices("MuonIndices");
static const SG::AuxElement::ConstAccessor<std::vector<size_t>> getMuonIndices("MuonIndices");
}  // namespace


TrigBmumuxComboHypo::TrigBmumuxComboHypo(const std::string& name, ISvcLocator* pSvcLocator)
    : ::ComboHypo(name, pSvcLocator) {}


StatusCode TrigBmumuxComboHypo::initialize() {
  ATH_MSG_DEBUG( "TrigBmumuxComboHypo::initialize()" );

  ATH_CHECK( ::ComboHypo::initialize() );

  ATH_CHECK( m_muonContainerKey.initialize() );
  renounce(m_muonContainerKey);
  ATH_CHECK( m_trackParticleContainerKey.initialize() );
  renounce(m_trackParticleContainerKey);
  ATH_CHECK( m_trigBphysContainerKey.initialize() );

  ATH_CHECK( m_vertexFitter.retrieve() );
  ATH_CHECK( m_vertexPointEstimator.retrieve() );
  ATH_CHECK( m_trackToVertexTool.retrieve() );

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
      std::fill_n(std::back_inserter(legDecisionIds), multiplicity[0], id);
    }
    else {
      size_t n = static_cast<size_t>(std::accumulate(multiplicity.begin(), multiplicity.end(), 0));
      for (size_t i = 0; i < n; i++) {
        legDecisionIds.push_back(TrigCompositeUtils::createLegName(id, i));
      }
    }
    tool->setLegDecisionIds(legDecisionIds);
    if (msgLvl(MSG::DEBUG)) {
      ATH_MSG_DEBUG( "Leg decisions for tool " << tool->name() );
      for (const auto& legId : legDecisionIds) {
        ATH_MSG_DEBUG( " +++ " << legId );
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


StatusCode TrigBmumuxComboHypo::execute(const EventContext& context) const {

  ATH_MSG_DEBUG( "TrigBmumuxComboHypo::execute() starts" );

  ATH_MSG_DEBUG( "decision input key: " << decisionsInput().at(0).key() );
  auto previousDecisionsHandle = SG::makeHandle(decisionsInput().at(0), context);
  ATH_CHECK( previousDecisionsHandle.isValid() );
  ATH_MSG_DEBUG( "Running with " << previousDecisionsHandle->size() << " previous decisions" );

  SG::WriteHandle<DecisionContainer> outputDecisionsHandle = TrigCompositeUtils::createAndStore(decisionsOutput().at(0), context);

  auto trigBphysHandle = SG::makeHandle(m_trigBphysContainerKey, context);
  ATH_CHECK( trigBphysHandle.record(std::make_unique<xAOD::TrigBphysContainer>(),
                                    std::make_unique<xAOD::TrigBphysAuxContainer>()) );

  auto state = makeState(&context, previousDecisionsHandle.cptr(), outputDecisionsHandle.ptr(), trigBphysHandle.ptr());
  ATH_CHECK( mergeMuonsFromViews(*state) );
  ATH_CHECK( findDimuonCandidates(*state) );

  if ( !state->dimuons.empty() ) {
    ATH_CHECK( mergeTracksFromViews(*state) );
    ATH_CHECK( findBmumuxCandidates(*state) );
    ATH_CHECK( createDecisionObjects(*state) );
  }

  ATH_MSG_DEBUG( "TrigBmumuxComboHypo::execute() terminates with StatusCode::SUCCESS" );
  return StatusCode::SUCCESS;
}


std::unique_ptr<TrigBmumuxState> TrigBmumuxComboHypo::makeState(const EventContext* context,
                                                                const DecisionContainer* previousDecisions,
                                                                DecisionContainer* decisions,
                                                                xAOD::TrigBphysContainer* trigBphysCollection) const {
  auto state = std::make_unique<TrigBmumuxState>();
  state->context = context;
  state->previousDecisions = previousDecisions;
  state->decisions = decisions;
  state->trigBphysCollection = trigBphysCollection;
  state->dimuons.setStore(&state->dimuonsStore);
  return state;
}


StatusCode TrigBmumuxComboHypo::mergeMuonsFromViews(TrigBmumuxState& state) const {

  auto& muons = state.muons;
  muons.clear();

  // all muons from views are already connected with previous decisions by TrigMuonEFHypoAlg
  for (const Decision* decision : *state.previousDecisions) {
    ATH_CHECK( decision->hasObjectLink(TrigCompositeUtils::featureString(), ClassID_traits<xAOD::MuonContainer>::ID()) );
    auto muonEL = decision->objectLink<xAOD::MuonContainer>(TrigCompositeUtils::featureString());
    const xAOD::Muon* muon = *muonEL;
    if (!muon->trackParticle(xAOD::Muon::TrackParticleType::CombinedTrackParticle)) continue;

    auto decisionEL = TrigCompositeUtils::decisionToElementLink(decision, *state.context);
    auto itr = std::find_if(muons.begin(), muons.end(), [this, muon = muon](const auto& x){ return isIdenticalTracks(muon, *x.link); });
    if (itr == muons.end()) {
      muons.push_back({muonEL, ElementLinkVector<DecisionContainer>(1, decisionEL), DecisionIDContainer()});
    }
    else {
      (*itr).decisionLinks.push_back(decisionEL);
    }
  }

  // muon->pt() is equal to muon->trackParticle(xAOD::Muon::TrackParticleType::CombinedTrackParticle)->pt()
  // and the later is used by TrigMuonEFHypoTool for classification of muEFCB candidates
  std::sort(muons.begin(), muons.end(), [](const auto& lhs, const auto& rhs){ return ((*lhs.link)->pt() > (*rhs.link)->pt()); });

  // for each muon we extract DecisionIDs stored in the associated Decision objects and copy them at muon.decisionIDs
  for (auto& item : muons) {
    for (const ElementLink<xAOD::TrigCompositeContainer> decisionEL : item.decisionLinks) {
      TrigCompositeUtils::decisionIDs(*decisionEL, item.decisionIDs);
    }
  }

  if (msgLvl(MSG::DEBUG)) {
    ATH_MSG_DEBUG( "Dump found muons before vertex fit: " << muons.size() << " candidates" );
    for (const auto& item : muons) {
      const xAOD::Muon* muon = *item.link;
      const xAOD::TrackParticle* track = *muon->inDetTrackParticleLink();
      ATH_MSG_DEBUG( " -- muon InDetTrackParticle pt/eta/phi/q: " << track->pt() << " / " << track->eta() << " / " << track->phi() << " / " << track->charge() );
      ATH_MSG_DEBUG( "    muon CombinedTrackParticle pt: " << muon->pt() );
      ATH_MSG_DEBUG( "    allowed decisions:" );
      for (const DecisionID& id : item.decisionIDs) {
        ATH_MSG_DEBUG( "    +++ " << HLT::Identifier(id) );
      }
    }
  }
  return StatusCode::SUCCESS;
}


StatusCode TrigBmumuxComboHypo::mergeTracksFromViews(TrigBmumuxState& state) const {

  auto& tracks = state.tracks;
  tracks.clear();

  size_t viewCounter = 0;
  for (const Decision* decision : *state.previousDecisions) {
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
      if (track->definingParametersCovMatrixVec().empty()) continue;

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


StatusCode TrigBmumuxComboHypo::findDimuonCandidates(TrigBmumuxState& state) const {

  auto mon_nDimuon = Monitored::Scalar<int>("nDimuon", 0);
  auto group = Monitored::Group(m_monTool, mon_nDimuon);

  const auto& muons = state.muons;
  std::vector<ElementLink<xAOD::TrackParticleContainer>> trackParticleLinks(2);
  std::vector<const DecisionIDContainer*> previousDecisionIDs(2, nullptr);
  for (size_t itrk1 = 0; itrk1 < muons.size(); ++itrk1) {
    const xAOD::Muon* mu1 = *muons[itrk1].link;
    trackParticleLinks[0] = mu1->inDetTrackParticleLink();
    previousDecisionIDs[0] = &muons[itrk1].decisionIDs;
    const xAOD::TrackParticle* trk1 = *trackParticleLinks[0];
    auto p1 = trk1->genvecP4();
    p1.SetM(PDG::mMuon);
    auto charge1 = trk1->charge();

    for (size_t itrk2 = itrk1 + 1; itrk2 < muons.size(); ++itrk2) {
      const xAOD::Muon* mu2 = *muons[itrk2].link;
      trackParticleLinks[1] = mu2->inDetTrackParticleLink();
      previousDecisionIDs[1] = &muons[itrk2].decisionIDs;
      const xAOD::TrackParticle* trk2 = *trackParticleLinks[1];
      auto p2 = trk2->genvecP4();
      p2.SetM(PDG::mMuon);
      auto charge2 = trk2->charge();

      double mass = (p1 + p2).M();

      ATH_MSG_DEBUG( "muon 1: " << p1.Pt()<< " / " << p1.Eta() << " / " << p1.Phi() << " / " << trk1->charge() );
      ATH_MSG_DEBUG( "muon 2: " << p2.Pt()<< " / " << p2.Eta() << " / " << p2.Phi() << " / " << trk2->charge() );
      ATH_MSG_DEBUG( "track pair mass: " << mass );

      if (m_dimuon_rejectSameChargeTracks && charge1 * charge2 > 0.) {
        ATH_MSG_DEBUG( "muon pair is rejected by opposite charge check" );
        continue;
      }

      if (!passDimuonTrigger(previousDecisionIDs)) {
        ATH_MSG_DEBUG( "muon pair did not pass passDimuonTrigger() check" );
        continue;
      }

      if (!isInMassRange(mass, m_dimuon_massRange)) {
        ATH_MSG_DEBUG( "muon pair is out of the requested mass range" );
        continue;
      }

      // fit muons to the common vertex and pass this vertex to the dimuons collection which also takes the ownership of the created object
      xAOD::Vertex* vertex = fit(state.context, trackParticleLinks);
      if (!vertex) continue;
      muonIndices(*vertex) = std::vector<size_t>{itrk1, itrk2};
      state.dimuons.push_back(vertex);

      // convert vertex to trigger object and add it to the output xAOD::TrigBphysContainer
      xAOD::TrigBphys* trigBphys = makeTriggerObject(vertex);
      if (!trigBphys) {
        ATH_MSG_ERROR( "xAOD::Vertex could not be converted to xAOD::TrigBphys object: please enable MakeExtendedVertex option in vertex fitter " << m_vertexFitter->name() );
        return StatusCode::FAILURE;
      }
      state.trigBphysCollection->push_back(trigBphys);
    }
  }
  mon_nDimuon = state.dimuons.size();
  ATH_MSG_DEBUG( "Found " << state.dimuons.size() << " dimuon candidates" );

  return StatusCode::SUCCESS;
}


StatusCode TrigBmumuxComboHypo::findBmumuxCandidates(TrigBmumuxState& state) const {

  std::vector<int> nSelectedTrk;
  auto mon_nTrk = Monitored::Scalar<int>("nTrk", 0);
  auto mon_nSelectedTrk = Monitored::Collection("nSelectedTrk", nSelectedTrk);
  auto mon_nBPhysObject = Monitored::Scalar<int>("nBPhysObject", 0);
  auto group = Monitored::Group(m_monTool, mon_nTrk, mon_nSelectedTrk, mon_nBPhysObject);

  const auto& tracks = state.tracks;
  mon_nTrk = tracks.size();
  mon_nBPhysObject = state.trigBphysCollection->size();

  for (size_t idx = 0; idx < state.dimuons.size(); ++idx) {
    const xAOD::Vertex* dimuon = state.dimuons.get(idx);

    auto dimuonTriggerObjectEL = ElementLink<xAOD::TrigBphysContainer>(*state.trigBphysCollection, idx);
    ATH_CHECK( dimuonTriggerObjectEL.isValid() );

    // vtx1 = {mu1, mu2, trk1}
    std::vector<ElementLink<xAOD::TrackParticleContainer>> trackParticleLinks_vtx1(dimuon->trackParticleLinks());
    trackParticleLinks_vtx1.emplace_back();

    // vtx2 = {mu1, mu2, trk1, trk2}
    std::vector<ElementLink<xAOD::TrackParticleContainer>> trackParticleLinks_vtx2(trackParticleLinks_vtx1);
    trackParticleLinks_vtx2.emplace_back();

    const xAOD::TrackParticle* mu1 = *trackParticleLinks_vtx1[0];
    const xAOD::TrackParticle* mu2 = *trackParticleLinks_vtx1[1];
    auto p_dimuon = mu1->genvecP4().SetM(PDG::mMuon) + mu2->genvecP4().SetM(PDG::mMuon);

    // check impact parameter of the track with respect to the fitted dimuon vertex
    // we can safely omit tracks with large z0
    std::vector<ElementLink<xAOD::TrackParticleContainer>> selectedTracks;
    selectedTracks.reserve(tracks.size());
    if (m_trkZ0 > 0.) {
      for (const auto& trackEL : tracks) {
        const Trk::Perigee* perigee = m_trackToVertexTool->perigeeAtVertex(**trackEL, dimuon->position());
        if (fabs(perigee->parameters()[Trk::z0]) < m_trkZ0) {
          selectedTracks.push_back(trackEL);
        }
        delete perigee;
      }
      ATH_MSG_DEBUG( "Found " << selectedTracks.size() << " tracks consistent with dimuon vertex " << idx );
    }
    else {
      std::copy(tracks.begin(), tracks.end(), std::back_inserter(selectedTracks));
    }
    nSelectedTrk.push_back(selectedTracks.size());

    // dimuon + 1 track
    for (size_t itrk1 = 0; itrk1 < selectedTracks.size(); ++itrk1) {
      const xAOD::TrackParticle* trk1 = *selectedTracks[itrk1];
      if (isIdenticalTracks(mu1, trk1) || isIdenticalTracks(mu2, trk1)) continue;

      trackParticleLinks_vtx1[2] = selectedTracks[itrk1];
      auto p_trk1 = trk1->genvecP4();
      auto charge1 = trk1->charge();

      xAOD::Vertex* vtx1 = nullptr;
      bool makeFit_vtx1 = true;

      // B+ -> mu+ mu- K+
      if (m_BplusToMuMuKaon &&
          p_trk1.Pt() > m_BplusToMuMuKaon_minKaonPt &&
          isInMassRange((p_dimuon + p_trk1.SetM(PDG::mKaon)).M(), m_BplusToMuMuKaon_massRange)) {
        vtx1 = fit(state.context, trackParticleLinks_vtx1, kB_2mu1trk, *dimuonTriggerObjectEL);
        makeFit_vtx1 = false;
        if (vtx1 && vtx1->chiSquared() < m_BplusToMuMuKaon_chi2) {
          xAOD::TrigBphys* trigBphys = makeTriggerObject(vtx1, xAOD::TrigBphys::BKMUMU, {PDG::mMuon, PDG::mMuon, PDG::mKaon}, dimuonTriggerObjectEL);
          ATH_CHECK( state.addTriggerObject(trigBphys) );
        }
      }

      // B_c+ -> J/psi(-> mu+ mu-) pi+
      if (m_BcToMuMuPion &&
          p_trk1.Pt() > m_BcToMuMuPion_minPionPt &&
          isInMassRange(p_dimuon.M(), m_BcToMuMuPion_dimuonMassRange) &&
          isInMassRange((p_dimuon + p_trk1.SetM(PDG::mPion)).M() - p_dimuon.M() + PDG::mJpsi, m_BcToMuMuPion_massRange)) {
        if (!vtx1 && makeFit_vtx1) {
          vtx1 = fit(state.context, trackParticleLinks_vtx1, kB_2mu1trk, *dimuonTriggerObjectEL);
          makeFit_vtx1 = false;
        }
        if (vtx1 && vtx1->chiSquared() < m_BcToMuMuPion_chi2) {
          xAOD::TrigBphys* trigBphys = makeTriggerObject(vtx1, xAOD::TrigBphys::BCPIMUMU, {PDG::mMuon, PDG::mMuon, PDG::mPion}, dimuonTriggerObjectEL);
          ATH_CHECK( state.addTriggerObject(trigBphys) );
        }
      }

      delete vtx1;
      vtx1 = nullptr;

      // dimuon + 2 tracks
      for (size_t itrk2 = itrk1 + 1; itrk2 < selectedTracks.size(); ++itrk2) {
        const xAOD::TrackParticle* trk2 = *selectedTracks[itrk2];
        if (isIdenticalTracks(mu1, trk2) || isIdenticalTracks(mu2, trk2)) continue;

        trackParticleLinks_vtx2[2] = selectedTracks[itrk1];
        trackParticleLinks_vtx2[3] = selectedTracks[itrk2];
        auto p_trk2 = trk2->genvecP4();
        auto charge2 = trk2->charge();

        xAOD::Vertex* vtx2 = nullptr;
        bool makeFit_vtx2 = true;

        // B_s0 -> mu+ mu- phi(-> K+ K-)
        if (m_BsToMuMuPhi1020 &&
            (!m_BsToMuMuPhi1020_rejectSameChargeTracks || charge1 * charge2 < 0.) &&
            p_trk1.Pt() > m_BsToMuMuPhi1020_minKaonPt &&
            p_trk2.Pt() > m_BsToMuMuPhi1020_minKaonPt &&
            isInMassRange((p_trk1.SetM(PDG::mKaon) + p_trk2.SetM(PDG::mKaon)).M(), m_BsToMuMuPhi1020_phiMassRange) &&
            isInMassRange((p_dimuon + p_trk1.SetM(PDG::mKaon) + p_trk2.SetM(PDG::mKaon)).M(), m_BsToMuMuPhi1020_massRange)) {
          vtx2 = fit(state.context, trackParticleLinks_vtx2, kB_2mu2trk, *dimuonTriggerObjectEL);
          makeFit_vtx2 = false;
          if (vtx2 && vtx2->chiSquared() < m_BsToMuMuPhi1020_chi2) {
            xAOD::TrigBphys* trigBphys = makeTriggerObject(vtx2, xAOD::TrigBphys::BSPHIMUMU, {PDG::mMuon, PDG::mMuon, PDG::mKaon, PDG::mKaon}, dimuonTriggerObjectEL);
            ATH_CHECK( state.addTriggerObject(trigBphys) );
          }
        }

        // B0 -> mu+ mu- K*0(-> K+ pi-)
        if (m_BdToMuMuKstar0 &&
            (!m_BdToMuMuKstar0_rejectSameChargeTracks || charge1 * charge2 < 0.) &&
            p_trk1.Pt() > m_BdToMuMuKstar0_minKaonPt &&
            p_trk2.Pt() > m_BdToMuMuKstar0_minPionPt &&
            isInMassRange((p_trk1.SetM(PDG::mKaon) + p_trk2.SetM(PDG::mPion)).M(), m_BdToMuMuKstar0_KstarMassRange) &&
            isInMassRange((p_dimuon + p_trk1.SetM(PDG::mKaon) + p_trk2.SetM(PDG::mPion)).M(), m_BdToMuMuKstar0_massRange)) {
          if (!vtx2 && makeFit_vtx2) {
            vtx2 = fit(state.context, trackParticleLinks_vtx2, kB_2mu2trk, *dimuonTriggerObjectEL);
            makeFit_vtx2 = false;
          }
          if (vtx2 && vtx2->chiSquared() < m_BdToMuMuKstar0_chi2) {
            xAOD::TrigBphys* trigBphys = makeTriggerObject(vtx2, xAOD::TrigBphys::BDKSTMUMU, {PDG::mMuon, PDG::mMuon, PDG::mKaon, PDG::mPion}, dimuonTriggerObjectEL);
            ATH_CHECK( state.addTriggerObject(trigBphys) );
          }
        }
        // anti-B0 -> mu+ mu- anti-K*0(-> K- pi+)
        if (m_BdToMuMuKstar0 &&
            (!m_BdToMuMuKstar0_rejectSameChargeTracks || charge1 * charge2 < 0.) &&
            p_trk1.Pt() > m_BdToMuMuKstar0_minPionPt &&
            p_trk2.Pt() > m_BdToMuMuKstar0_minKaonPt &&
            isInMassRange((p_trk1.SetM(PDG::mPion) + p_trk2.SetM(PDG::mKaon)).M(), m_BdToMuMuKstar0_KstarMassRange) &&
            isInMassRange((p_dimuon + p_trk1.SetM(PDG::mPion) + p_trk2.SetM(PDG::mKaon)).M(), m_BdToMuMuKstar0_massRange)) {
          if (!vtx2 && makeFit_vtx2) {
            vtx2 = fit(state.context, trackParticleLinks_vtx2, kB_2mu2trk, *dimuonTriggerObjectEL);
            makeFit_vtx2 = false;
          }
          if (vtx2 && vtx2->chiSquared() < m_BdToMuMuKstar0_chi2) {
            xAOD::TrigBphys* trigBphys = makeTriggerObject(vtx2, xAOD::TrigBphys::BDKSTMUMU, {PDG::mMuon, PDG::mMuon, PDG::mPion, PDG::mKaon}, dimuonTriggerObjectEL);
            ATH_CHECK( state.addTriggerObject(trigBphys) );
          }
        }

        // Lambda_b0 -> J/psi(-> mu+ mu-) p K-
        if (m_LambdaBToMuMuProtonKaon &&
            p_trk1.Pt() > m_LambdaBToMuMuProtonKaon_minProtonPt &&
            p_trk2.Pt() > m_LambdaBToMuMuProtonKaon_minKaonPt &&
            (p_trk1.SetM(PDG::mKaon) + p_trk2.SetM(PDG::mPion)).M() > m_LambdaBToMuMuProtonKaon_minKstarMass &&
            (p_trk1.SetM(PDG::mPion) + p_trk2.SetM(PDG::mKaon)).M() > m_LambdaBToMuMuProtonKaon_minKstarMass &&
            isInMassRange(p_dimuon.M(), m_LambdaBToMuMuProtonKaon_dimuonMassRange) &&
            isInMassRange((p_dimuon + p_trk1.SetM(PDG::mProton) + p_trk2.SetM(PDG::mKaon)).M() - p_dimuon.M() + PDG::mJpsi, m_LambdaBToMuMuProtonKaon_massRange)) {
          if (!vtx2 && makeFit_vtx2) {
            vtx2 = fit(state.context, trackParticleLinks_vtx2, kB_2mu2trk, *dimuonTriggerObjectEL);
            makeFit_vtx2 = false;
          }
          if (vtx2 && vtx2->chiSquared() < m_LambdaBToMuMuProtonKaon_chi2) {
            xAOD::TrigBphys* trigBphys = makeTriggerObject(vtx2, xAOD::TrigBphys::LBPQMUMU, {PDG::mMuon, PDG::mMuon, PDG::mProton, PDG::mKaon}, dimuonTriggerObjectEL);
            ATH_CHECK( state.addTriggerObject(trigBphys) );
          }
        }
        // anti-Lambda_b0 -> J/psi(-> mu+ mu-) anti-p K+
        if (m_LambdaBToMuMuProtonKaon &&
            p_trk1.Pt() > m_LambdaBToMuMuProtonKaon_minKaonPt &&
            p_trk2.Pt() > m_LambdaBToMuMuProtonKaon_minProtonPt &&
            (p_trk1.SetM(PDG::mKaon) + p_trk2.SetM(PDG::mPion)).M() > m_LambdaBToMuMuProtonKaon_minKstarMass &&
            (p_trk1.SetM(PDG::mPion) + p_trk2.SetM(PDG::mKaon)).M() > m_LambdaBToMuMuProtonKaon_minKstarMass &&
            isInMassRange(p_dimuon.M(), m_LambdaBToMuMuProtonKaon_dimuonMassRange) &&
            isInMassRange((p_dimuon + p_trk1.SetM(PDG::mKaon) + p_trk2.SetM(PDG::mProton)).M() - p_dimuon.M() + PDG::mJpsi, m_LambdaBToMuMuProtonKaon_massRange)) {
          if (!vtx2 && makeFit_vtx2) {
            vtx2 = fit(state.context, trackParticleLinks_vtx2, kB_2mu2trk, *dimuonTriggerObjectEL);
            makeFit_vtx2 = false;
          }
          if (vtx2 && vtx2->chiSquared() < m_LambdaBToMuMuProtonKaon_chi2) {
            xAOD::TrigBphys* trigBphys = makeTriggerObject(vtx2, xAOD::TrigBphys::LBPQMUMU, {PDG::mMuon, PDG::mMuon, PDG::mKaon, PDG::mProton}, dimuonTriggerObjectEL);
            ATH_CHECK( state.addTriggerObject(trigBphys) );
          }
        }

        delete vtx2;
        vtx2 = nullptr;

      }
    }
  }
  mon_nBPhysObject = state.trigBphysCollection->size() - mon_nBPhysObject;

  return StatusCode::SUCCESS;
}


StatusCode TrigBmumuxComboHypo::createDecisionObjects(TrigBmumuxState& state) const {

  for (const xAOD::TrigBphys* triggerObject : *state.trigBphysCollection) {
    // skip all dimuon trigger objects, they are already linked to the Bmumux trigger objects via lowerChainLink()
    if (triggerObject->particleType() == xAOD::TrigBphys::MULTIMU) continue;

    ATH_MSG_DEBUG( "Found xAOD::TrigBphys object: mass = " << triggerObject->mass() );

    auto triggerObjectEL = ElementLink<xAOD::TrigBphysContainer>(*state.trigBphysCollection, triggerObject->index());
    ATH_CHECK( triggerObjectEL.isValid() );

    const xAOD::TrigBphys* dimuonTriggerObject = triggerObject->lowerChain();
    if (!dimuonTriggerObject) {
      ATH_MSG_ERROR( "Failed to found a valid link for preceding dimuon trigger object" );
      return StatusCode::FAILURE;
    }

    // need to get the references to the original muon objects used to build the dimuon vertex
    // the position of this vertex in state.dimuons container is the same as for dimuonTriggerObject in trigBphysCollection
    // dimuon vertex has already been decorated with muon indices
    const xAOD::Vertex* dimuon = state.dimuons.get(dimuonTriggerObject->index());
    if ( !dimuon || !getMuonIndices.isAvailable(*dimuon) ) {
      ATH_MSG_ERROR( "Failed to find original muons the dimuon vertex had been built from" );
      return StatusCode::FAILURE;
    }

    // create a new output Decision object, backed by the 'decisions' container.
    Decision* decision = TrigCompositeUtils::newDecisionIn(state.decisions, comboHypoAlgNodeName());

    std::vector<const DecisionIDContainer*> previousDecisionIDs;
    for (const size_t& i : getMuonIndices(*dimuon)) {
      const auto& muon = state.muons.at(i);
      // attach all previous decisions: if the same previous decision is called twice, that's fine - internally takes care of that
      // we already have an array of links to the previous decisions, so there is no need to use TrigCompositeUtils::linkToPrevious()
      decision->addObjectCollectionLinks(TrigCompositeUtils::seedString(), muon.decisionLinks);
      previousDecisionIDs.push_back(&muon.decisionIDs);
    }

    // set mandatory link to the trigger object
    decision->setObjectLink<xAOD::TrigBphysContainer>(TrigCompositeUtils::featureString(), triggerObjectEL);

    for (const auto& tool : hypoTools()) {
      ATH_MSG_DEBUG( "Go to " << tool );
      ATH_CHECK( tool->decideOnSingleObject(decision, previousDecisionIDs) );
    }
  }

  return StatusCode::SUCCESS;
}


xAOD::Vertex* TrigBmumuxComboHypo::fit(const EventContext* context,
                                       const std::vector<ElementLink<xAOD::TrackParticleContainer>>& trackParticleLinks,
                                       Decay decay,
                                       const xAOD::TrigBphys* dimuon) const {
  ATH_MSG_DEBUG( "Perform vertex fit" );

  if (trackParticleLinks.size() < 2) {
    ATH_MSG_WARNING( "At least two tracks should be given to the vertex fitter" );
    return nullptr;
  }

  std::vector<const xAOD::TrackParticle*> tracklist(trackParticleLinks.size(), nullptr);
  std::transform(trackParticleLinks.begin(), trackParticleLinks.end(), tracklist.begin(),
                 [](const ElementLink<xAOD::TrackParticleContainer>& link){ return *link; });

  Amg::Vector3D startingPoint = Amg::Vector3D::Zero(3);
  if (dimuon) {
    startingPoint = Amg::Vector3D(dimuon->fitx(), dimuon->fity(), dimuon->fitz());
  }
  else {
    if (decay != Decay::kPsi_2mu) {
      ATH_MSG_WARNING( "Already fitted dimuon vertex should be provided for B -> mu1 mu2 trk1 .. trkN decay as a starting point for fitter" );
    }
    int flag = 0;
    int errorcode = 0;
    const Trk::Perigee& perigee1 = tracklist[0]->perigeeParameters();
    const Trk::Perigee& perigee2 = tracklist[1]->perigeeParameters();
    startingPoint = m_vertexPointEstimator->getCirclesIntersectionPoint(&perigee1, &perigee2, flag, errorcode);
    if (errorcode != 0) startingPoint = Amg::Vector3D::Zero(3);
  }
  ATH_MSG_DEBUG( "Starting point: (" << startingPoint(0) << ", " << startingPoint(1) << ", " << startingPoint(2) << ")" );

  auto fitterState = m_vertexFitter->makeState(*context);
  m_vertexFitter->setMassInputParticles(m_trkMass[static_cast<size_t>(decay)], *fitterState);
  xAOD::Vertex* vertex = m_vertexFitter->fit(tracklist, startingPoint, *fitterState);
  if (!vertex) {
    ATH_MSG_DEBUG( "Vertex fit fails" );
    return nullptr;
  }
  if (vertex->chiSquared() > 150. || (decay == Decay::kPsi_2mu && vertex->chiSquared() > m_dimuon_chi2)) {
    ATH_MSG_DEBUG( "Fit is successful, but vertex chi2 is too high, we are not going to save it (chi2 = " << vertex->chiSquared() << ")" );
    delete vertex;
    return nullptr;
  }
  ATH_MSG_DEBUG( "Fit is successful" );

  // update trackParticleLinks()
  vertex->clearTracks();
  vertex->setTrackParticleLinks(trackParticleLinks);

  return vertex;
}


xAOD::TrigBphys* TrigBmumuxComboHypo::makeTriggerObject(const xAOD::Vertex* vertex,
                                                        xAOD::TrigBphys::pType type,
                                                        const std::vector<double>& trkMass,
                                                        const ElementLink<xAOD::TrigBphysContainer>& dimuonLink) const {

  // refitted track momentum as a 4-vector for mass hypothesis defined by the given decay value
  GenVecFourMom_t momentum;
  std::vector<GenVecFourMom_t> momenta;
  if (!vertex->vxTrackAtVertexAvailable()) return nullptr;
  for (size_t i = 0; i < vertex->vxTrackAtVertex().size(); ++i) {
    const Trk::TrackParameters* perigee = vertex->vxTrackAtVertex()[i].perigeeAtVertex();
    if (!perigee) return nullptr;
    const Amg::Vector3D& p = perigee->momentum();
    momenta.emplace_back(p.x(), p.y(), p.z(), trkMass[i]);
    momentum += momenta.back();
  }

  auto result = new xAOD::TrigBphys();
  result->makePrivateStore();

  float mass = (type == xAOD::TrigBphys::MULTIMU ? momentum.M() : momentum.M() - (momenta[0] + momenta[1]).M() + PDG::mJpsi);
  result->initialise(0, momentum.Eta(), momentum.Phi(), momentum.Pt(), type, mass, xAOD::TrigBphys::EF);

  result->setFitmass(momentum.M());
  result->setFitx(vertex->x());
  result->setFity(vertex->y());
  result->setFitz(vertex->z());
  result->setFitchi2(vertex->chiSquared());
  result->setFitndof(vertex->numberDoF());

  // set all the particles associated with the decay
  result->setTrackParticleLinks(vertex->trackParticleLinks());

  // use lowerChainLink() as a link to the preceding dimuon trigger object
  if (type != xAOD::TrigBphys::MULTIMU) {
    result->setLowerChainLink(dimuonLink);
  }

  ATH_MSG_DEBUG(
    "TrigBphys object:\n\t  " <<
    "roiId:         " << result->roiId()  << "\n\t  " <<
    "particleType:  " << result->particleType() << "\n\t  " <<
    "level:         " << result->level() << "\n\t  " <<
    "eta:           " << result->eta() << "\n\t  " <<
    "phi:           " << result->phi() << "\n\t  " <<
    "mass:          " << result->mass() << "\n\t  " <<
    "fitmass:       " << result->fitmass() << "\n\t  " <<
    "chi2/NDF:      " << result->fitchi2() << " / " << result->fitndof() << "\n\t  " <<
    "vertex:        (" << result->fitx() << ", " << result->fity() << ", " << result->fitz() << ")" );

  return result;
}


bool TrigBmumuxComboHypo::isIdenticalTracks(const xAOD::TrackParticle* lhs, const xAOD::TrackParticle* rhs) const {

  if (lhs->charge() * rhs->charge() < 0.) return false;
  return (ROOT::Math::VectorUtil::DeltaR(lhs->genvecP4(), rhs->genvecP4()) < m_deltaR);
}


bool TrigBmumuxComboHypo::isIdenticalTracks(const xAOD::Muon* lhs, const xAOD::Muon* rhs) const {

  return isIdenticalTracks(*lhs->inDetTrackParticleLink(), *rhs->inDetTrackParticleLink());
}


bool TrigBmumuxComboHypo::passDimuonTrigger(const std::vector<const DecisionIDContainer*>& previousDecisionIDs) const {

  if (previousDecisionIDs.size() != 2) {
    return false;
  }

  for (const auto& tool : hypoTools()) {
    const std::vector<HLT::Identifier>& legDecisionIDs = tool->legDecisionIds();
    if (legDecisionIDs.size() != 2) continue;

    bool direct = true;
    bool inverse = true;
    for (size_t i = 0; i < 2; ++i) {
      if (direct && !TrigCompositeUtils::passed(legDecisionIDs[i].numeric(), *previousDecisionIDs[i])) direct = false;
      if (inverse && !TrigCompositeUtils::passed(legDecisionIDs[i].numeric(), *previousDecisionIDs[1-i])) inverse = false;
    }
    if (direct || inverse) return true;
  }
  return false;
}
