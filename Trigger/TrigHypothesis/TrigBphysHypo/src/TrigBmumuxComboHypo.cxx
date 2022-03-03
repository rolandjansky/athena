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

#include "Math/GenVector/VectorUtil.h"
#include "Math/Vector2D.h"


using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::DecisionID;
using TrigCompositeUtils::DecisionIDContainer;
using ROOT::Math::XYVector;


const std::vector<std::vector<double>> TrigBmumuxComboHypo::s_trkMass{
  {PDG::mMuon, PDG::mMuon},                          // {Psi.mu1, Psi.mu2}
  {PDG::mMuon, PDG::mMuon, PDG::mKaon},              // {Psi.mu1, Psi.mu2, trk1}
  {PDG::mMuon, PDG::mMuon, PDG::mKaon, PDG::mKaon},  // {Psi.mu1, Psi.mu2, trk1, trk2}
  {PDG::mKaon, PDG::mKaon, PDG::mPion},              // {D_s+.K+, D_s+.K-, D_s+.pi+}
  {PDG::mPion, PDG::mPion, PDG::mKaon},              // {D+.pi+, D+.pi+, D+.K-}
  {PDG::mKaon, PDG::mPion},                          // {D0.K-, D0.pi+}
  {PDG::mMuon, PDG::mMuon, PDG::mPion}               // {Psi.mu1, Psi.mu2, D*+.pi+}
};

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
  ATH_CHECK( m_beamSpotKey.initialize() );

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

  SG::ReadCondHandle<InDet::BeamSpotData> beamSpotHandle {m_beamSpotKey, context};
  ATH_CHECK( beamSpotHandle.isValid() );

  auto state = std::make_unique<TrigBmumuxState>(context, *previousDecisionsHandle, *outputDecisionsHandle, trigBphysHandle.ptr(), *beamSpotHandle);

  ATH_CHECK( mergeMuonsFromDecisions(*state) );
  ATH_CHECK( findDimuonCandidates(*state) );

  if (!state->dimuons.empty()) {
    ATH_CHECK( mergeTracksFromViews(*state) );
    ATH_CHECK( findBmumuxCandidates(*state) );
    ATH_CHECK( createDecisionObjects(*state) );
  }

  ATH_MSG_DEBUG( "TrigBmumuxComboHypo::execute() terminates with StatusCode::SUCCESS" );
  return StatusCode::SUCCESS;
}


StatusCode TrigBmumuxComboHypo::mergeMuonsFromDecisions(TrigBmumuxState& state) const {

  auto& muons = state.muons;
  muons.clear();

  // all muons from views are already connected with previous decisions by TrigMuonEFHypoAlg
  for (const Decision* decision : state.previousDecisions()) {
    ATH_CHECK( decision->hasObjectLink(TrigCompositeUtils::featureString(), ClassID_traits<xAOD::MuonContainer>::ID()) );
    auto muonEL = decision->objectLink<xAOD::MuonContainer>(TrigCompositeUtils::featureString());
    const xAOD::Muon* muon = *muonEL;
    if (!muon->trackParticle(xAOD::Muon::TrackParticleType::CombinedTrackParticle)) continue;

    auto decisionEL = TrigCompositeUtils::decisionToElementLink(decision, state.context());
    auto itr = std::find_if(muons.begin(), muons.end(), [this, muon = muon](const auto& x){ return isIdenticalTracks(muon, *x.link); });
    if (itr == muons.end()) {
      muons.push_back({muonEL, std::vector<ElementLink<DecisionContainer>>(1, decisionEL), DecisionIDContainer()});
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
    for (const ElementLink<xAOD::TrigCompositeContainer>& decisionEL : item.decisionLinks) {
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
  for (const Decision* decision : state.previousDecisions()) {
    auto viewLinkInfo = TrigCompositeUtils::findLink<ViewContainer>(decision, TrigCompositeUtils::viewString(), true);
    ATH_CHECK( viewLinkInfo.isValid() );
    auto viewEL = viewLinkInfo.link;

    auto tracksHandle = ViewHelper::makeHandle(*viewEL, m_trackParticleContainerKey, state.context());
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
      auto vertex = fit(state.context(), trackParticleLinks);
      if (!vertex) continue;

      // convert vertex to trigger object and add it to the output xAOD::TrigBphysContainer
      xAOD::TrigBphys* trigBphys = makeTriggerObject(state, *vertex);
      if (!trigBphys) {
        ATH_MSG_ERROR( "xAOD::Vertex could not be converted to xAOD::TrigBphys object: please enable MakeExtendedVertex option in vertex fitter " << m_vertexFitter->name() );
        return StatusCode::FAILURE;
      }

      state.dimuons.push_back(vertex.release());
      state.trigBphysMuonIndices.emplace_back(std::array<size_t, 2>{itrk1, itrk2});
      state.trigBphysCollection().push_back(trigBphys);
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
  mon_nBPhysObject = state.trigBphysCollection().size();

  for (size_t idx = 0; idx < state.dimuons.size(); ++idx) {
    const xAOD::Vertex* dimuon = state.dimuons.get(idx);

    auto dimuonTriggerObjectEL = ElementLink<xAOD::TrigBphysContainer>(state.trigBphysCollection(), idx);
    ATH_CHECK( dimuonTriggerObjectEL.isValid() );

    // vtx1 = {mu1, mu2, trk1}
    std::vector<ElementLink<xAOD::TrackParticleContainer>> trackParticleLinks_vtx1(dimuon->trackParticleLinks());
    trackParticleLinks_vtx1.emplace_back();

    // vtx2 = {mu1, mu2, trk1, trk2}
    std::vector<ElementLink<xAOD::TrackParticleContainer>> trackParticleLinks_vtx2(trackParticleLinks_vtx1);
    trackParticleLinks_vtx2.emplace_back();

    // vtx3 = {trk1, trk2, trk3}
    std::vector<ElementLink<xAOD::TrackParticleContainer>> trackParticleLinks_vtx3(3);

    const xAOD::TrackParticle* mu1 = *trackParticleLinks_vtx1[0];
    const xAOD::TrackParticle* mu2 = *trackParticleLinks_vtx1[1];
    auto p_dimuon = mu1->genvecP4().SetM(PDG::mMuon) + mu2->genvecP4().SetM(PDG::mMuon);

    // check impact parameter of the track with respect to the fitted dimuon vertex
    // we can safely omit tracks with large z0
    std::vector<ElementLink<xAOD::TrackParticleContainer>> selectedTracks;
    selectedTracks.reserve(tracks.size());
    std::vector<double> selectedTrackZ0;
    selectedTrackZ0.reserve(tracks.size());
    if (m_trkZ0 > 0.) {
      for (const auto& trackEL : tracks) {
        const Trk::Perigee* perigee = m_trackToVertexTool->perigeeAtVertex(**trackEL, dimuon->position());
        if (fabs(perigee->parameters()[Trk::z0]) < m_trkZ0) {
          selectedTracks.push_back(trackEL);
          selectedTrackZ0.push_back(perigee->parameters()[Trk::z0]);
        }
        delete perigee;
      }
      ATH_MSG_DEBUG( "Found " << selectedTracks.size() << " tracks consistent with dimuon vertex " << idx );
    }
    else {
      std::copy(tracks.begin(), tracks.end(), std::back_inserter(selectedTracks));
    }
    nSelectedTrk.push_back(selectedTracks.size());

    size_t iterations = 0;
    bool isOverWarningThreshold = false;
    // dimuon + 1 track
    for (size_t itrk1 = 0; itrk1 < selectedTracks.size(); ++itrk1) {
      const xAOD::TrackParticle* trk1 = *selectedTracks[itrk1];
      if (isIdenticalTracks(mu1, trk1) || isIdenticalTracks(mu2, trk1)) continue;

      trackParticleLinks_vtx1[2] = selectedTracks[itrk1];
      auto p_trk1 = trk1->genvecP4();
      auto charge1 = trk1->charge();

      std::unique_ptr<xAOD::Vertex> vtx1;
      bool makeFit_vtx1 = true;

      // B+ -> mu+ mu- K+
      if (m_BplusToMuMuKaon &&
          p_trk1.Pt() > m_BplusToMuMuKaon_minKaonPt &&
          isInMassRange((p_dimuon + p_trk1.SetM(PDG::mKaon)).M(), m_BplusToMuMuKaon_massRange)) {
        vtx1 = fit(state.context(), trackParticleLinks_vtx1, kB_2mu1trk, dimuon);
        makeFit_vtx1 = false;
        ++iterations;
        if (vtx1 && vtx1->chiSquared() < m_BplusToMuMuKaon_chi2) {
          xAOD::TrigBphys* trigBphys = makeTriggerObject(state, *vtx1, xAOD::TrigBphys::BKMUMU, {PDG::mMuon, PDG::mMuon, PDG::mKaon}, dimuonTriggerObjectEL);
          ATH_CHECK( state.addTriggerObject(trigBphys) );
        }
      }

      // B_c+ -> J/psi(-> mu+ mu-) pi+
      if (m_BcToMuMuPion &&
          p_trk1.Pt() > m_BcToMuMuPion_minPionPt &&
          isInMassRange(p_dimuon.M(), m_BcToMuMuPion_dimuonMassRange) &&
          isInMassRange((p_dimuon + p_trk1.SetM(PDG::mPion)).M() - p_dimuon.M() + PDG::mJpsi, m_BcToMuMuPion_massRange)) {
        if (!vtx1 && makeFit_vtx1) {
          vtx1 = fit(state.context(), trackParticleLinks_vtx1, kB_2mu1trk, dimuon);
          makeFit_vtx1 = false;
          ++iterations;
        }
        if (vtx1 && vtx1->chiSquared() < m_BcToMuMuPion_chi2) {
          xAOD::TrigBphys* trigBphys = makeTriggerObject(state, *vtx1, xAOD::TrigBphys::BCPIMUMU, {PDG::mMuon, PDG::mMuon, PDG::mPion}, dimuonTriggerObjectEL);
          ATH_CHECK( state.addTriggerObject(trigBphys) );
        }
      }
      vtx1.reset();

      // dimuon + 2 tracks
      for (size_t itrk2 = itrk1 + 1; itrk2 < selectedTracks.size(); ++itrk2) {
        const xAOD::TrackParticle* trk2 = *selectedTracks[itrk2];
        if (isIdenticalTracks(mu1, trk2) || isIdenticalTracks(mu2, trk2)) continue;

        trackParticleLinks_vtx2[2] = selectedTracks[itrk1];
        trackParticleLinks_vtx2[3] = selectedTracks[itrk2];
        auto p_trk2 = trk2->genvecP4();
        auto charge2 = trk2->charge();

        std::unique_ptr<xAOD::Vertex> vtx2;
        bool makeFit_vtx2 = true;

        // B_s0 -> mu+ mu- phi(-> K+ K-)
        if (m_BsToMuMuPhi1020 &&
            (!m_BsToMuMuPhi1020_rejectSameChargeTracks || charge1 * charge2 < 0.) &&
            p_trk1.Pt() > m_BsToMuMuPhi1020_minKaonPt &&
            p_trk2.Pt() > m_BsToMuMuPhi1020_minKaonPt &&
            isInMassRange((p_trk1.SetM(PDG::mKaon) + p_trk2.SetM(PDG::mKaon)).M(), m_BsToMuMuPhi1020_phiMassRange) &&
            isInMassRange((p_dimuon + p_trk1.SetM(PDG::mKaon) + p_trk2.SetM(PDG::mKaon)).M(), m_BsToMuMuPhi1020_massRange)) {
          vtx2 = fit(state.context(), trackParticleLinks_vtx2, kB_2mu2trk, dimuon);
          makeFit_vtx2 = false;
          ++iterations;
          if (vtx2 && vtx2->chiSquared() < m_BsToMuMuPhi1020_chi2) {
            xAOD::TrigBphys* trigBphys = makeTriggerObject(state, *vtx2, xAOD::TrigBphys::BSPHIMUMU, {PDG::mMuon, PDG::mMuon, PDG::mKaon, PDG::mKaon}, dimuonTriggerObjectEL);
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
            vtx2 = fit(state.context(), trackParticleLinks_vtx2, kB_2mu2trk, dimuon);
            makeFit_vtx2 = false;
            ++iterations;
          }
          if (vtx2 && vtx2->chiSquared() < m_BdToMuMuKstar0_chi2) {
            xAOD::TrigBphys* trigBphys = makeTriggerObject(state, *vtx2, xAOD::TrigBphys::BDKSTMUMU, {PDG::mMuon, PDG::mMuon, PDG::mKaon, PDG::mPion}, dimuonTriggerObjectEL);
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
            vtx2 = fit(state.context(), trackParticleLinks_vtx2, kB_2mu2trk, dimuon);
            makeFit_vtx2 = false;
            ++iterations;
          }
          if (vtx2 && vtx2->chiSquared() < m_BdToMuMuKstar0_chi2) {
            xAOD::TrigBphys* trigBphys = makeTriggerObject(state, *vtx2, xAOD::TrigBphys::BDKSTMUMU, {PDG::mMuon, PDG::mMuon, PDG::mPion, PDG::mKaon}, dimuonTriggerObjectEL);
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
            vtx2 = fit(state.context(), trackParticleLinks_vtx2, kB_2mu2trk, dimuon);
            makeFit_vtx2 = false;
            ++iterations;
          }
          if (vtx2 && vtx2->chiSquared() < m_LambdaBToMuMuProtonKaon_chi2 && Lxy(state.beamSpotPosition(), *vtx2) > 0.) {
            xAOD::TrigBphys* trigBphys = makeTriggerObject(state, *vtx2, xAOD::TrigBphys::LBPQMUMU, {PDG::mMuon, PDG::mMuon, PDG::mProton, PDG::mKaon}, dimuonTriggerObjectEL);
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
            vtx2 = fit(state.context(), trackParticleLinks_vtx2, kB_2mu2trk, dimuon);
            makeFit_vtx2 = false;
            ++iterations;
          }
          if (vtx2 && vtx2->chiSquared() < m_LambdaBToMuMuProtonKaon_chi2 && Lxy(state.beamSpotPosition(), *vtx2) > 0.) {
            xAOD::TrigBphys* trigBphys = makeTriggerObject(state, *vtx2, xAOD::TrigBphys::LBPQMUMU, {PDG::mMuon, PDG::mMuon, PDG::mKaon, PDG::mProton}, dimuonTriggerObjectEL);
            ATH_CHECK( state.addTriggerObject(trigBphys) );
          }
        }
        vtx2.reset();

        for (size_t itrk3 = 0; itrk3 < selectedTracks.size(); ++itrk3) {
          const xAOD::TrackParticle* trk3 = *selectedTracks[itrk3];
          if (itrk3 == itrk1 || itrk3 == itrk2 || isIdenticalTracks(mu1, trk3) || isIdenticalTracks(mu2, trk3)) continue;

          trackParticleLinks_vtx3[0] = selectedTracks[itrk1];
          trackParticleLinks_vtx3[1] = selectedTracks[itrk2];
          trackParticleLinks_vtx3[2] = selectedTracks[itrk3];
          auto p_trk3 = trk3->genvecP4();
          auto charge3 = trk3->charge();

          std::unique_ptr<xAOD::Vertex> vtx3;
          bool makeFit_vtx3 = true;

          // B_c+ -> J/psi(-> mu+ mu-) D_s+(->phi(-> K+ K-) pi+)
          p_trk1.SetM(PDG::mKaon);  // D_s+.phi.K+
          p_trk2.SetM(PDG::mKaon);  // D_s+.phi.K-
          p_trk3.SetM(PDG::mPion);  // D_s+.pi+
          if (m_BcToDsMuMu &&
              charge1 * charge2 < 0. &&
              p_trk1.Pt() > m_BcToDsMuMu_minKaonPt &&
              p_trk2.Pt() > m_BcToDsMuMu_minKaonPt &&
              p_trk3.Pt() > m_BcToDsMuMu_minPionPt &&
              isInMassRange(p_dimuon.M(), m_BcToDsMuMu_dimuonMassRange) &&
              isInMassRange((p_trk1 + p_trk2).M(), m_BcToDsMuMu_phiMassRange) &&
              isInMassRange((p_trk1 + p_trk2 + p_trk3).M(), m_BcToDsMuMu_DsMassRange) &&
              isInMassRange((p_dimuon + p_trk1 + p_trk2 + p_trk3).M() - p_dimuon.M() + PDG::mJpsi, m_BcToDsMuMu_massRange)) {
            if (!vtx3 && makeFit_vtx3) {
              vtx3 = fit(state.context(), trackParticleLinks_vtx3, kDs, dimuon);
              makeFit_vtx3 = false;
              ++iterations;
            }
            if (vtx3 && vtx3->chiSquared() < m_BcToDsMuMu_chi2) {
              xAOD::TrigBphys* trigBphys = makeTriggerObject(state, *vtx3, xAOD::TrigBphys::BCDSMUMU, {PDG::mKaon, PDG::mKaon, PDG::mPion}, dimuonTriggerObjectEL);
              ATH_CHECK( state.addTriggerObject(trigBphys) );
            }
          }

          // B_c+ -> J/psi(-> mu+ mu-) D+(-> K- pi+ pi+)
          p_trk1.SetM(PDG::mPion);  // D+.pi+
          p_trk2.SetM(PDG::mPion);  // D+.pi+
          p_trk3.SetM(PDG::mKaon);  // D+.K-
          if (m_BcToDplusMuMu &&
              charge1 * charge2 > 0. && charge1 * charge3 < 0. &&
              p_trk1.Pt() > m_BcToDplusMuMu_minPionPt &&
              p_trk2.Pt() > m_BcToDplusMuMu_minPionPt &&
              p_trk3.Pt() > m_BcToDplusMuMu_minKaonPt &&
              isInMassRange(p_dimuon.M(), m_BcToDplusMuMu_dimuonMassRange) &&
              isInMassRange((p_trk1 + p_trk2 + p_trk3).M(), m_BcToDplusMuMu_DplusMassRange) &&
              isInMassRange((p_dimuon + p_trk1 + p_trk2 + p_trk3).M() - p_dimuon.M() + PDG::mJpsi, m_BcToDplusMuMu_massRange)) {
            if (!vtx3 && makeFit_vtx3) {
              vtx3 = fit(state.context(), trackParticleLinks_vtx3, kDplus, dimuon);
              makeFit_vtx3 = false;
              ++iterations;
            }
            if (vtx3 && vtx3->chiSquared() < m_BcToDplusMuMu_chi2 && Lxy(dimuon->position(), *vtx3) > 0.) {
              xAOD::TrigBphys* trigBphys = makeTriggerObject(state, *vtx3, xAOD::TrigBphys::BCDPMUMU, {PDG::mPion, PDG::mPion, PDG::mKaon}, dimuonTriggerObjectEL);
              ATH_CHECK( state.addTriggerObject(trigBphys) );
            }
          }
          vtx3.reset();

        }
      }

      if (iterations > m_fitAttemptsWarningThreshold && !isOverWarningThreshold) {
        ATH_MSG_WARNING( iterations << " combinations for vertex fitter have been processed; " << mon_nBPhysObject << " vertices have been created" );
        isOverWarningThreshold = true;
      }
      if (iterations > m_fitAttemptsBreakThreshold) {
        ATH_MSG_WARNING( "the number of fit attempts has exceeded the limit; breaking the loop at this point" );
        break;
      }
    }

    iterations = 0;
    isOverWarningThreshold = false;
    // B_c+ -> J/psi(-> mu+ mu-) D*+(-> D0(-> K- pi+) pi+)
    if (m_BcToDstarMuMu && isInMassRange(p_dimuon.M(), m_BcToDstarMuMu_dimuonMassRange)) {
      std::vector<ElementLink<xAOD::TrackParticleContainer>> trackParticleLinks_D0(2);  // {K-, pi+}

      for (size_t itrk1 = 0; itrk1 < selectedTracks.size(); ++itrk1) {
        const xAOD::TrackParticle* trk1 = *selectedTracks[itrk1];
        if (isIdenticalTracks(mu1, trk1) || isIdenticalTracks(mu2, trk1)) continue;

        trackParticleLinks_D0[0] = selectedTracks[itrk1];
        auto p_trk1 = trk1->genvecP4();
        p_trk1.SetM(PDG::mKaon);
        auto charge1 = trk1->charge();

        for (size_t itrk2 = 0; itrk2 < selectedTracks.size(); ++itrk2) {
          if (itrk2 == itrk1) continue;
          const xAOD::TrackParticle* trk2 = *selectedTracks[itrk2];
          if (isIdenticalTracks(mu1, trk2) || isIdenticalTracks(mu2, trk2)) continue;

          trackParticleLinks_D0[1] = selectedTracks[itrk2];
          auto p_trk2 = trk2->genvecP4();
          p_trk2.SetM(PDG::mPion);
          auto charge2 = trk2->charge();

          std::unique_ptr<xAOD::Vertex> D0;
          if (charge1 * charge2 < 0. &&
              p_trk1.Pt() > m_BcToDstarMuMu_minD0KaonPt &&
              p_trk2.Pt() > m_BcToDstarMuMu_minD0PionPt &&
              isInMassRange((p_trk1 + p_trk2).M(), m_BcToDstarMuMu_D0MassRange) &&
              isInMassRange((p_dimuon + p_trk1 + p_trk2).M() - p_dimuon.M() + PDG::mJpsi, m_BcToDstarMuMu_massRange)) {
            D0 = fit(state.context(), trackParticleLinks_D0, kD0, dimuon);
            ++iterations;
          }
          bool isValidD0 = false;
          if (D0 && D0->chiSquared() < m_BcToDstarMuMu_chi2 && Lxy(dimuon->position(), *D0) > 0.) {
            isValidD0 = true;
            ATH_MSG_DEBUG( "Partially reconstructed B_c+(-> mu+ mu- D0 X) candidate has been created" );
            xAOD::TrigBphys* trigBphys = makeTriggerObject(state, *D0, xAOD::TrigBphys::DZKPI, s_trkMass[kD0], dimuonTriggerObjectEL);
            ATH_CHECK( state.addTriggerObject(trigBphys) );
          }

          if (m_BcToDstarMuMu_makeDstar && isValidD0) {  // full B_c+ reconstruction
            xAOD::TrackParticle::GenVecFourMom_t p_D0 = momentum(*D0, s_trkMass[kD0]);

            for (size_t itrk3 = 0; itrk3 < selectedTracks.size(); ++itrk3) {
              const xAOD::TrackParticle* trk3 = *selectedTracks[itrk3];
              if (itrk3 == itrk1 || itrk3 == itrk2 || isIdenticalTracks(mu1, trk3) || isIdenticalTracks(mu2, trk3)) continue;

              // J/psi + pion from D*+
              trackParticleLinks_vtx1[2] = selectedTracks[itrk3];
              auto p_trk3 = trk3->genvecP4();
              p_trk3.SetM(PDG::mPion);

              if (p_trk3.Pt() > m_BcToDstarMuMu_minDstarPionPt &&
                  (m_BcToDstarMuMu_maxDstarPionZ0 < 0. || fabs(selectedTrackZ0[itrk3]) < m_BcToDstarMuMu_maxDstarPionZ0) &&
                  isInMassRange((p_D0 + p_trk3).M() - p_D0.M() + PDG::mD0, m_BcToDstarMuMu_DstarMassRange)) {
                auto Bc_vtx1 = fit(state.context(), trackParticleLinks_vtx1, kB_PsiPi);
                ++iterations;

                if (Bc_vtx1 && Bc_vtx1->chiSquared() < m_BcToDstarMuMu_chi2) {
                  ATH_MSG_DEBUG( "Decay vertex(mu+ mu- D*+.pi+) for B_c+ candidate has been created" );
                  xAOD::TrigBphys* triggerObject_vtx1 = makeTriggerObject(state, *Bc_vtx1, xAOD::TrigBphys::DSTDZPI, s_trkMass[kB_PsiPi], dimuonTriggerObjectEL);
                  auto triggerObjectEL_vtx1 = ElementLink<xAOD::TrigBphysContainer>(state.trigBphysCollection(), state.trigBphysCollection().size());
                  ATH_CHECK( state.addTriggerObject(triggerObject_vtx1) );
                  ATH_CHECK( triggerObjectEL_vtx1.isValid() );

                  // refit D0 vertex
                  auto Bc_vtx2 = fit(state.context(), trackParticleLinks_D0, kD0, Bc_vtx1.get());
                  ++iterations;
                  if (Bc_vtx2 && Bc_vtx2->chiSquared() < m_BcToDstarMuMu_chi2) {
                    ATH_MSG_DEBUG( "Fully reconstructed B_c+(-> mu+ mu- D*+) candidate has been created" );
                    xAOD::TrigBphys* triggerObject_vtx2 = makeTriggerObject(state, *Bc_vtx2, xAOD::TrigBphys::BCDSTMUMU, s_trkMass[kD0], triggerObjectEL_vtx1);
                    ATH_CHECK( state.addTriggerObject(triggerObject_vtx2) );
                  }
                }
              }
            }
          }  // end of full B_c+ reconstruction

        }
      }

      if (iterations > m_fitAttemptsWarningThreshold && !isOverWarningThreshold) {
        ATH_MSG_WARNING( iterations << " combinations for vertex fitter have been processed; " << mon_nBPhysObject << " vertices have been created" );
        isOverWarningThreshold = true;
      }
      if (iterations > m_fitAttemptsBreakThreshold) {
        ATH_MSG_WARNING( "the number of fit attempts has exceeded the limit; breaking the loop at this point" );
        break;
      }
    }  // end of B_c+ -> J/psi D*+ topology

  }  // end of dimuon loop
  mon_nBPhysObject = state.trigBphysCollection().size() - mon_nBPhysObject;

  return StatusCode::SUCCESS;
}


StatusCode TrigBmumuxComboHypo::createDecisionObjects(TrigBmumuxState& state) const {

  for (const xAOD::TrigBphys* triggerObject : state.trigBphysCollection()) {
    // skip all dimuon trigger objects, they are already linked to the Bmumux trigger objects via lowerChainLink()
    if (triggerObject->particleType() == xAOD::TrigBphys::MULTIMU || triggerObject->particleType() == xAOD::TrigBphys::DSTDZPI) continue;

    ATH_MSG_DEBUG( "Found xAOD::TrigBphys object: mass = " << triggerObject->mass() );

    auto triggerObjectEL = ElementLink<xAOD::TrigBphysContainer>(state.trigBphysCollection(), triggerObject->index());
    ATH_CHECK( triggerObjectEL.isValid() );

    const xAOD::TrigBphys* dimuonTriggerObject = triggerObject->lowerChain();
    if (triggerObject->particleType() == xAOD::TrigBphys::BCDSTMUMU && dimuonTriggerObject) dimuonTriggerObject = dimuonTriggerObject->lowerChain();
    if (!dimuonTriggerObject) {
      ATH_MSG_ERROR( "Failed to found a valid link for preceding dimuon trigger object" );
      return StatusCode::FAILURE;
    }

    // need to get the references to the original muon objects used to build the dimuon vertex
    // the position of this vertex in state.dimuons container is the same as for dimuonTriggerObject in trigBphysCollection
    // dimuon vertex has already been decorated with muon indices
    auto muonindex = dimuonTriggerObject->index();
    const xAOD::Vertex* dimuon = state.dimuons.get(muonindex);
    if ( !dimuon || muonindex >= state.trigBphysMuonIndices.size() ) {
      ATH_MSG_ERROR( "Failed to find original muons the dimuon vertex had been built from" );
      return StatusCode::FAILURE;
    }

    // create a new output Decision object, backed by the 'decisions' container.
    Decision* decision = TrigCompositeUtils::newDecisionIn(&state.decisions(), TrigCompositeUtils::comboHypoAlgNodeName());

    std::vector<const DecisionIDContainer*> previousDecisionIDs;
    for (const size_t& i : state.trigBphysMuonIndices.at(muonindex)) {
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


std::unique_ptr<xAOD::Vertex> TrigBmumuxComboHypo::fit(
    const EventContext& context,
    const std::vector<ElementLink<xAOD::TrackParticleContainer>>& trackParticleLinks,
    Decay decay,
    const xAOD::Vertex* dimuon) const {

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
    startingPoint = Amg::Vector3D(dimuon->x(), dimuon->y(), dimuon->z());
  }
  else {
    if (decay != Decay::kPsi_2mu && decay != Decay::kB_PsiPi) {
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

  auto fitterState = m_vertexFitter->makeState(context);
  m_vertexFitter->setMassInputParticles(s_trkMass[static_cast<size_t>(decay)], *fitterState);

  // the combined momentum of D+/D_s+ candidate is constrained to point to the dimuon vertex
  if (decay == Decay::kDs || decay == Decay::kDplus || decay == Decay::kD0) {
    m_vertexFitter->setVertexForConstraint(*dimuon, *fitterState);
    m_vertexFitter->setCnstType(8, *fitterState);
  }

  std::unique_ptr<xAOD::Vertex> vertex(m_vertexFitter->fit(tracklist, startingPoint, *fitterState));
  if (!vertex) {
    ATH_MSG_DEBUG( "Vertex fit fails" );
    return vertex;
  }
  if (vertex->chiSquared() > 150. || (decay == Decay::kPsi_2mu && vertex->chiSquared() > m_dimuon_chi2)) {
    ATH_MSG_DEBUG( "Fit is successful, but vertex chi2 is too high, we are not going to save it (chi2 = " << vertex->chiSquared() << ")" );
    vertex.reset();
    return vertex;
  }
  ATH_MSG_DEBUG( "Fit is successful" );

  // update trackParticleLinks()
  vertex->clearTracks();
  vertex->setTrackParticleLinks(trackParticleLinks);

  return vertex;
}


xAOD::TrigBphys* TrigBmumuxComboHypo::makeTriggerObject(
    TrigBmumuxState& state,
    const xAOD::Vertex& vertex,
    xAOD::TrigBphys::pType type,
    const std::vector<double>& trkMass,
    const ElementLink<xAOD::TrigBphysContainer>& dimuonLink) const {

  const xAOD::TrigBphys* dimuon = (type != xAOD::TrigBphys::MULTIMU ? *dimuonLink : nullptr);
  bool isCascadeDecay = (type == xAOD::TrigBphys::BCDSMUMU || type == xAOD::TrigBphys::BCDPMUMU || type == xAOD::TrigBphys::DZKPI || type == xAOD::TrigBphys::BCDSTMUMU);

  // refitted track momentum as a 4-vector for mass hypothesis defined by the given decay value
  xAOD::TrackParticle::GenVecFourMom_t momentum;
  std::vector<xAOD::TrackParticle::GenVecFourMom_t> momenta;
  if (!vertex.vxTrackAtVertexAvailable()) return nullptr;
  for (size_t i = 0; i < vertex.vxTrackAtVertex().size(); ++i) {
    const Trk::TrackParameters* perigee = vertex.vxTrackAtVertex()[i].perigeeAtVertex();
    if (!perigee) return nullptr;
    const Amg::Vector3D& p = perigee->momentum();
    momenta.emplace_back(p.x(), p.y(), p.z(), trkMass[i]);
    momentum += momenta.back();
  }
  if (isCascadeDecay) {
    momentum += ROOT::Math::PtEtaPhiMVector(dimuon->pt(), dimuon->eta(), dimuon->phi(), dimuon->mass());
  }

  auto result = new xAOD::TrigBphys();
  result->makePrivateStore();

  float mass = momentum.M();
  if (type == xAOD::TrigBphys::BCDSTMUMU) {
    mass += PDG::mD0 - (momenta[0] + momenta[1]).M();
  }
  else if (type != xAOD::TrigBphys::MULTIMU) {
    mass += PDG::mJpsi - (isCascadeDecay ? dimuon->mass() : (momenta[0] + momenta[1]).M());
  }

  result->initialise(0, momentum.Eta(), momentum.Phi(), momentum.Pt(), type, mass, xAOD::TrigBphys::EF);

  result->setFitmass(momentum.M());
  result->setFitx(vertex.x());
  result->setFity(vertex.y());
  result->setFitz(vertex.z());
  result->setFitchi2(vertex.chiSquared());
  result->setFitndof(vertex.numberDoF());

  Amg::Vector3D productionVertex = (isCascadeDecay ? Amg::Vector3D(dimuon->fitx(), dimuon->fity(), dimuon->fitz()) : state.beamSpotPosition());
  result->setLxy(Lxy(productionVertex, vertex));

  // set all the particles associated with the decay
  result->setTrackParticleLinks(vertex.trackParticleLinks());

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
    "vertex:        (" << result->fitx() << ", " << result->fity() << ", " << result->fitz() << ")" << "\n\t  " <<
    "Lxy:           " << result->lxy() );

  return result;
}


bool TrigBmumuxComboHypo::isIdenticalTracks(const xAOD::TrackParticle* lhs, const xAOD::TrackParticle* rhs) const {

  if (lhs->charge() * rhs->charge() < 0.) return false;
  return (ROOT::Math::VectorUtil::DeltaR(lhs->genvecP4(), rhs->genvecP4()) < m_deltaR);
}


bool TrigBmumuxComboHypo::isIdenticalTracks(const xAOD::Muon* lhs, const xAOD::Muon* rhs) const {

  return isIdenticalTracks(*lhs->inDetTrackParticleLink(), *rhs->inDetTrackParticleLink());
}


double TrigBmumuxComboHypo::Lxy(const Amg::Vector3D& productionVertex, const xAOD::Vertex& decayVertex) const {

  XYVector R(decayVertex.x() - productionVertex.x(), decayVertex.y() - productionVertex.y());
  XYVector pT;

  if (!decayVertex.vxTrackAtVertexAvailable()) return -100.;
  const auto& tracks = decayVertex.vxTrackAtVertex();
  for (const auto& track : tracks) {
    const Trk::TrackParameters* perigee = track.perigeeAtVertex();
    if (!perigee) return -100.;
    const Amg::Vector3D& momentum = perigee->momentum();
    pT += XYVector(momentum.x(), momentum.y());
  }
  return R.Dot(pT.unit());
}


xAOD::TrackParticle::GenVecFourMom_t TrigBmumuxComboHypo::momentum(const xAOD::Vertex& vertex, const std::vector<double>& trkMass) const {

  xAOD::TrackParticle::GenVecFourMom_t momentum;
  for (size_t i = 0; i < vertex.vxTrackAtVertex().size(); ++i) {
    const Trk::TrackParameters* perigee = vertex.vxTrackAtVertex()[i].perigeeAtVertex();
    const Amg::Vector3D& p = perigee->momentum();
    momentum += xAOD::TrackParticle::GenVecFourMom_t(p.x(), p.y(), p.z(), trkMass[i]);
  }
  return momentum;
}


bool TrigBmumuxComboHypo::passDimuonTrigger(const std::vector<const DecisionIDContainer*>& previousDecisionIDs) const {

  if (previousDecisionIDs.size() != 2) {
    ATH_MSG_WARNING( "TrigBmumuxComboHypo::passDimuonTrigger() expects exactly two containers with previous decision IDs" );
    return false;
  }

  for (const auto& tool : hypoTools()) {
    const std::vector<HLT::Identifier>& legDecisionIDs = tool->legDecisionIds();
    if (legDecisionIDs.size() == 1 && tool->legMultiplicity().at(0) >= 2) {
      // trigger with symmetric legs like HLT_2mu4_bBmumux_BsmumuPhi_L12MU4
      const DecisionID id = legDecisionIDs[0].numeric();
      if (TrigCompositeUtils::passed(id, *previousDecisionIDs[0]) && TrigCompositeUtils::passed(id, *previousDecisionIDs[1])) return true;
    }
    else if (legDecisionIDs.size() == 2) {
      // trigger with asymmetric legs like HLT_mu6_mu4_bBmumux_BsmumuPhi_L1MU6_2MU4
      bool direct = true;
      bool inverse = true;
      for (size_t i = 0; i < 2; ++i) {
        if (direct && !TrigCompositeUtils::passed(legDecisionIDs[i].numeric(), *previousDecisionIDs[i])) direct = false;
        if (inverse && !TrigCompositeUtils::passed(legDecisionIDs[i].numeric(), *previousDecisionIDs[1-i])) inverse = false;
      }
      if (direct || inverse) return true;
    }
    else {
      ATH_MSG_WARNING( "TrigBmumuxComboHypo can not check decisions for " << tool->name() );
    }
  }
  return false;
}
