/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include <algorithm>
#include <numeric>
#include <iterator>

#include "TrigBmuxComboHypo.h"

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


const std::vector<std::vector<std::vector<double>>> TrigBmuxComboHypo::s_trkMass{
  {{PDG::mKaon, PDG::mPion}, {PDG::mMuon}},               // {D0.K-, D0.pi+}, {mu-}
  {{PDG::mKaon, PDG::mPion}, {PDG::mMuon, PDG::mPion}},   // {D*+.D0.K-, D*+.D0.pi+}, {mu-, D*+.pi+}
  {{PDG::mKaon, PDG::mKaon, PDG::mPion}, {PDG::mMuon}},   // {D_s+.K+, D_s+.K-, D_s+.pi+}, {mu-}
  {{PDG::mKaon, PDG::mPion, PDG::mPion}, {PDG::mMuon}},   // {D+.K-, D+.pi+, D+.pi+}, {mu-}
  {{PDG::mProton, PDG::mKaon, PDG::mPion}, {PDG::mMuon}}  // {Lambda_c+.p, Lambda_c+.K-, Lambda_c+.pi+}, {mu-}
};


TrigBmuxComboHypo::TrigBmuxComboHypo(const std::string& name, ISvcLocator* pSvcLocator)
    : ::ComboHypo(name, pSvcLocator) {}


StatusCode TrigBmuxComboHypo::initialize() {
  ATH_MSG_DEBUG( "TrigBmuxComboHypo::initialize()" );

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


StatusCode TrigBmuxComboHypo::execute(const EventContext& context) const {

  ATH_MSG_DEBUG( "TrigBmuxComboHypo::execute() starts" );

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

  auto state = std::make_unique<TrigBmuxState>(context, *previousDecisionsHandle, *outputDecisionsHandle, trigBphysHandle.ptr(), *beamSpotHandle);

  ATH_CHECK( findBmuxCandidates(*state) );
  ATH_CHECK( createDecisionObjects(*state) );

  ATH_MSG_DEBUG( "TrigBmuxComboHypo::execute() terminates with StatusCode::SUCCESS" );
  return StatusCode::SUCCESS;
}


StatusCode TrigBmuxComboHypo::findBmuxCandidates(TrigBmuxState& state) const {

  auto& muons = state.muons;
  muons.clear();

  // monitored variables
  std::vector<int> nTrk;
  auto mon_nMuon = Monitored::Scalar<int>("nMuon", 0);
  auto mon_nTrk = Monitored::Collection("nTrk", nTrk);
  auto mon_nBPhysObject = Monitored::Scalar<int>("nBPhysObject", 0);

  auto group = Monitored::Group(m_monTool,
    mon_nMuon, mon_nTrk, mon_nBPhysObject);

  for (const Decision* decision : state.previousDecisions()) {
    if (!TrigCompositeUtils::isAnyIDPassing(decision, m_allowedIDs)) continue;

    auto decisionEL = TrigCompositeUtils::decisionToElementLink(decision, state.context());
    ATH_CHECK( decision->hasObjectLink(TrigCompositeUtils::featureString(), ClassID_traits<xAOD::MuonContainer>::ID()) );
    auto muonEL = decision->objectLink<xAOD::MuonContainer>(TrigCompositeUtils::featureString());
    const auto muon = *muonEL;
    if (!muon->trackParticle(xAOD::Muon::TrackParticleType::CombinedTrackParticle)) continue;
    if (!muon->trackParticle(xAOD::Muon::TrackParticleType::InnerDetectorTrackParticle)) continue;
    const auto muonInDetTrack = muon->trackParticle(xAOD::Muon::TrackParticleType::InnerDetectorTrackParticle);
    auto p_mu = muonInDetTrack->genvecP4();
    p_mu.SetM(PDG::mMuon);
    double z0_mu = (m_trkZ0 > 0. ? getTrkImpactParameterZ0(*muonInDetTrack, state.beamSpotPosition()) : -1.);
    mon_nMuon++;

    // add muon from decision to state.leptons
    DecisionIDContainer decisionIDs;
    TrigCompositeUtils::decisionIDs(decision, decisionIDs);
    size_t muonIndex = muons.size();
    muons.push_back({muonEL, decisionEL, decisionIDs});

    ATH_MSG_DEBUG( "Found muon (CombinedTrackParticle) pt/eta/phi/q: " << muon->pt() << " / " << muon->eta() << " / " << muon->phi() << " / " << muon->charge() );

    auto viewLinkInfo = TrigCompositeUtils::findLink<ViewContainer>(decision, TrigCompositeUtils::viewString(), true);
    ATH_CHECK( viewLinkInfo.isValid() );
    auto view = *viewLinkInfo.link;

    auto tracksHandle = ViewHelper::makeHandle(view, m_trackParticleContainerKey, state.context());
    ATH_CHECK( tracksHandle.isValid() );
    ATH_MSG_DEBUG( "Tracks container " << m_trackParticleContainerKey << " size: " << tracksHandle->size() );

    std::vector<ElementLink<xAOD::TrackParticleContainer>> tracks;
    tracks.reserve(tracksHandle->size());
    for (size_t idx = 0; idx < tracksHandle->size(); ++idx) {
      const auto track = tracksHandle->get(idx);
      if (track->definingParametersCovMatrixVec().empty() || isIdenticalTracks(track, muonInDetTrack) || (m_trkZ0 > 0. && std::abs(getTrkImpactParameterZ0(*track, state.beamSpotPosition()) - z0_mu) > m_trkZ0)) continue;
      tracks.emplace_back(ViewHelper::makeLink<xAOD::TrackParticleContainer>(view, tracksHandle, idx));
    }
    nTrk.push_back(tracks.size());

    std::sort(tracks.begin(), tracks.end(), [](const auto& lhs, const auto& rhs){ return ((*lhs)->pt() > (*rhs)->pt()); });

    if (msgLvl(MSG::DEBUG)) {
      ATH_MSG_DEBUG( "Dump found tracks before vertex fit: " << tracks.size() << " candidates" );
      for (const auto& trackEL : tracks) {
        const xAOD::TrackParticle* track = *trackEL;
        ATH_MSG_DEBUG( " -- track pt/eta/phi/q: " << track->pt() << " / " << track->eta() << " / " << track->phi() << " / " << track->charge() );
      }
    }

    size_t iterations = 0;
    bool isOverWarningThreshold = false;
    for (size_t itrk1 = 0; itrk1 < tracks.size(); ++itrk1) {
      const xAOD::TrackParticle* trk1 = *tracks[itrk1];
      auto p_trk1 = trk1->genvecP4();
      auto charge1 = trk1->charge();

      for (size_t itrk2 = 0; itrk2 < tracks.size(); ++itrk2) {
        if (itrk2 == itrk1) continue;
        const xAOD::TrackParticle* trk2 = *tracks[itrk2];
        auto p_trk2 = trk2->genvecP4();
        auto charge2 = trk2->charge();

        // D0(-> K- pi+)
        auto D0 = ElementLink<xAOD::TrigBphysContainer>();
        std::unique_ptr<xAOD::Vertex> vtx_D0;
        xAOD::TrackParticle::GenVecFourMom_t p_D0;
        if (m_BToD0 &&
            charge1 * charge2 < 0. &&
            p_trk1.Pt() > m_BToD0_minD0KaonPt &&
            p_trk2.Pt() > m_BToD0_minD0PionPt &&
            (p_trk1 + p_trk2).Pt() > m_BToD0_minD0Pt &&
            isInMassRange((p_trk1.SetM(PDG::mKaon) + p_trk2.SetM(PDG::mPion)).M(), m_BToD0_D0MassRange) &&
            isInMassRange((p_mu + p_trk1.SetM(PDG::mKaon) + p_trk2.SetM(PDG::mPion)).M(), m_BToD0_massRange)) {
          vtx_D0 = fit(state.context(), {muon->inDetTrackParticleLink(), tracks[itrk1], tracks[itrk2]}, kD0);
          ++iterations;

          if (vtx_D0 && vtx_D0->chiSquared() < m_BToD0_chi2) {
            ATH_MSG_DEBUG( "Partially reconstructed B+ -> mu+ nu_mu anti-D0(-> K+ pi-) candidate has been created from { " << itrk1 << ", " << itrk2 << " }" );
            if (m_makeCascadeFit) {
              auto result = fitCascade(state.context(), {muon->inDetTrackParticleLink(), tracks[itrk1], tracks[itrk2]}, kD0);
              // all fitted (muon + D0) candidates should be taken into account to avoid bias in (muon + D*-) sample
              if (result &&
                  result->fitChi2() < m_cascadeChi2 && result->nDoF() == 2) {
                std::vector<xAOD::TrigBphys*> triggerObjects = state.addTriggerObjects(2, muonIndex);
                ATH_CHECK( fillTriggerObjects(triggerObjects, xAOD::TrigBphys::DZKPI, *result, state.beamSpotPosition()) );
                p_D0 = ROOT::Math::PtEtaPhiMVector(triggerObjects[0]->pt(), triggerObjects[0]->eta(), triggerObjects[0]->phi(), triggerObjects[0]->fitmass());
                D0 = ElementLink<xAOD::TrigBphysContainer>(m_trigBphysContainerKey.key(), state.trigBphysCollection().size() - 1);
                ATH_CHECK( D0.isValid() );

                // only candidates with proper Lxy values should fire B2D0MUX trigger
                if (triggerObjects[1]->lxy() > m_BToD0_LxyB && triggerObjects[0]->lxy() > m_BToD0_LxyD0) {
                  triggerObjects[1]->setParticleType(xAOD::TrigBphys::B2D0MUX);
                  mon_nBPhysObject++;
                }
              }
            }
            else {
              xAOD::TrigBphys* triggerObject = state.addTriggerObject(muonIndex);
              ATH_CHECK( fillTriggerObject(*triggerObject, xAOD::TrigBphys::B2D0MUX, *vtx_D0, s_trkMass[kD0][0]) );
              p_D0 = ROOT::Math::PtEtaPhiMVector(triggerObject->pt(), triggerObject->eta(), triggerObject->phi(), triggerObject->fitmass());
              triggerObject->setMass((p_mu + p_D0).M());
              D0 = ElementLink<xAOD::TrigBphysContainer>(m_trigBphysContainerKey.key(), state.trigBphysCollection().size() - 1);
              ATH_CHECK( D0.isValid() );
              mon_nBPhysObject++;
            }
          }
        }  // end of D0

        for (size_t itrk3 = 0; itrk3 < tracks.size(); ++itrk3) {
          if (itrk3 == itrk1 || itrk3 == itrk2) continue;
          const xAOD::TrackParticle* trk3 = *tracks[itrk3];
          auto p_trk3 = trk3->genvecP4();
          auto charge3 = trk3->charge();

          std::vector<ElementLink<xAOD::TrackParticleContainer>> trackParticleLinks = {muon->inDetTrackParticleLink(), tracks[itrk1], tracks[itrk2], tracks[itrk3]};

          // D*-(-> anti-D0(-> K+ pi-) pi-)
          if (m_BToD0_makeDstar && D0.isValid() &&
              p_trk3.Pt() > m_BToD0_minDstarPionPt &&
              (p_trk1 + p_trk2 + p_trk3).Pt() > m_BToD0_minDstarPt &&
              isInMassRange((p_D0 + p_trk3.SetM(PDG::mPion)).M() - p_D0.M() + PDG::mD0, m_BToD0_DstarMassRange)) {

            bool makeDstar = true;
            if (m_BToD0_maxDstarPionZ0 > 0.) {
              std::unique_ptr<const Trk::Perigee> perigee(m_trackToVertexTool->perigeeAtVertex(*trk3, vtx_D0->position()));
              if (std::abs(perigee->parameters()[Trk::z0]) > m_BToD0_maxDstarPionZ0) makeDstar = false;
            }

            if (makeDstar) {
              if (m_makeCascadeFit) {
                auto result = fitCascade(state.context(), trackParticleLinks, kDstar);
                if (result &&
                    result->fitChi2() < m_cascadeChi2 && result->nDoF() == 4 &&
                    Lxy(state.beamSpotPosition(), result->vertices()[1]->position(), result->getParticleMoms()[1]) > m_BToD0_LxyBd &&
                    Lxy(result->vertices()[1]->position(), result->vertices()[0]->position(), result->getParticleMoms()[0]) > m_BToD0_LxyD0) {

                  std::vector<xAOD::TrigBphys*> triggerObjects = state.addTriggerObjects(2, muonIndex);
                  ATH_CHECK( fillTriggerObjects(triggerObjects, xAOD::TrigBphys::BD2DSTMUX, *result, state.beamSpotPosition()) );
                  triggerObjects[1]->setSecondaryDecayLink(D0);
                  mon_nBPhysObject++;
                }
              }
              else {
                xAOD::TrackParticle::GenVecFourMom_t p_Dstar = p_D0 + p_trk3.SetM(PDG::mPion);
                xAOD::TrigBphys* Dstar = state.addTriggerObject(muonIndex);
                Dstar->initialise(0, p_Dstar.Eta(), p_Dstar.Phi(), p_Dstar.Pt(), xAOD::TrigBphys::BD2DSTMUX, (p_mu + p_Dstar).M(), xAOD::TrigBphys::EF);
                Dstar->setFitmass((p_D0 + p_trk3).M() - p_D0.M() + PDG::mD0);
                Dstar->setTrackParticleLinks(trackParticleLinks);
                Dstar->setLowerChainLink(D0);

                ATH_MSG_DEBUG(
                  "TrigBphys object:\n\t  " <<
                  "roiId:         " << Dstar->roiId()  << "\n\t  " <<
                  "particleType:  " << Dstar->particleType() << "\n\t  " <<
                  "level:         " << Dstar->level() << "\n\t  " <<
                  "eta:           " << Dstar->eta() << "\n\t  " <<
                  "phi:           " << Dstar->phi() << "\n\t  " <<
                  "mass:          " << Dstar->mass() << "\n\t  " <<
                  "fitmass:       " << Dstar->fitmass() << "\n\t  " <<
                  "chi2/NDF:      " << Dstar->fitchi2() << " / " << Dstar->fitndof() << "\n\t  " <<
                  "vertex:        (" << Dstar->fitx() << ", " << Dstar->fity() << ", " << Dstar->fitz() << ")" );

                mon_nBPhysObject++;
              }
            }
          }  // end of D*-

          // B0 -> mu+ nu_mu D-(-> K+ pi- pi-)
          if (m_BdToD &&
              itrk3 > itrk2 &&
              charge1 * charge2 < 0. && charge2 * charge3 > 0. &&
              p_trk1.Pt() > m_BdToD_minKaonPt &&
              p_trk2.Pt() > m_BdToD_minPionPt &&
              p_trk3.Pt() > m_BdToD_minPionPt &&
              (p_trk1 + p_trk2 + p_trk3).Pt() > m_BdToD_minDPt &&
              isInMassRange((p_trk1.SetM(PDG::mKaon) + p_trk2.SetM(PDG::mPion) + p_trk3.SetM(PDG::mPion)).M(), m_BdToD_DMassRange) &&
              isInMassRange((p_mu + p_trk1.SetM(PDG::mKaon) + p_trk2.SetM(PDG::mPion) + p_trk3.SetM(PDG::mPion)).M(), m_BdToD_massRange)) {
            auto vtx_D = fit(state.context(), trackParticleLinks, kDplus);
            ++iterations;

            if (vtx_D && vtx_D->chiSquared() < m_BdToD_chi2) {
              ATH_MSG_DEBUG( "Partially reconstructed B0 -> mu+ nu_mu D-(-> K+ pi- pi-) candidate has been created from { " << itrk1 << ", " << itrk2 << ", " << itrk3 << " }" );
              if (m_makeCascadeFit) {
                auto result = fitCascade(state.context(), trackParticleLinks, kDplus);
                if (result &&
                    result->fitChi2() < m_cascadeChi2 && result->nDoF() == 4 &&
                    Lxy(state.beamSpotPosition(), result->vertices()[1]->position(), result->getParticleMoms()[1]) > m_BdToD_LxyBd &&
                    Lxy(result->vertices()[1]->position(), result->vertices()[0]->position(), result->getParticleMoms()[0]) > m_BdToD_LxyD) {

                  std::vector<xAOD::TrigBphys*> triggerObjects = state.addTriggerObjects(2, muonIndex);
                  ATH_CHECK( fillTriggerObjects(triggerObjects, xAOD::TrigBphys::BD2DMMUX, *result, state.beamSpotPosition()) );
                  mon_nBPhysObject++;
                }
              }
              else {
                xAOD::TrigBphys* triggerObject = state.addTriggerObject(muonIndex);
                ATH_CHECK( fillTriggerObject(*triggerObject, xAOD::TrigBphys::BD2DMMUX, *vtx_D, s_trkMass[kDplus][0]) );
                xAOD::TrackParticle::GenVecFourMom_t p_D(ROOT::Math::PtEtaPhiMVector(triggerObject->pt(), triggerObject->eta(), triggerObject->phi(), triggerObject->fitmass()));
                triggerObject->setMass((p_mu + p_D).M());
                mon_nBPhysObject++;
              }
            }
          }  // end of B0

          // B_s0 -> mu+ nu_mu D_s-(->phi(-> K+ K-) pi-)
          if (m_BsToDs &&
              itrk2 > itrk1 &&
              charge1 * charge2 < 0. &&
              p_trk1.Pt() > m_BsToDs_minKaonPt &&
              p_trk2.Pt() > m_BsToDs_minKaonPt &&
              p_trk3.Pt() > m_BsToDs_minPionPt &&
              (p_trk1 + p_trk2 + p_trk3).Pt() > m_BsToDs_minDsPt &&
              isInMassRange((p_trk1.SetM(PDG::mKaon) + p_trk2.SetM(PDG::mKaon)).M(), m_BsToDs_phiMassRange) &&
              isInMassRange((p_trk1.SetM(PDG::mKaon) + p_trk2.SetM(PDG::mKaon) + p_trk3.SetM(PDG::mPion)).M(), m_BsToDs_DsMassRange) &&
              isInMassRange((p_mu + p_trk1.SetM(PDG::mKaon) + p_trk2.SetM(PDG::mKaon) + p_trk3.SetM(PDG::mPion)).M(), m_BsToDs_massRange)) {
            auto vtx_Ds = fit(state.context(), trackParticleLinks, kDs);
            ++iterations;

            if (vtx_Ds && vtx_Ds->chiSquared() < m_BsToDs_chi2) {
              ATH_MSG_DEBUG( "Partially reconstructed B_s0 -> mu+ nu_mu D_s-(->phi(-> K+ K-) pi-) candidate has been created from { " << itrk1 << ", " << itrk2 << ", " << itrk3 << " }" );
              if (m_makeCascadeFit) {
                auto result = fitCascade(state.context(), trackParticleLinks, kDs);
                if (result &&
                    result->fitChi2() < m_cascadeChi2 && result->nDoF() == 4 &&
                    Lxy(state.beamSpotPosition(), result->vertices()[1]->position(), result->getParticleMoms()[1]) > m_BsToDs_LxyBs &&
                    Lxy(result->vertices()[1]->position(), result->vertices()[0]->position(), result->getParticleMoms()[0]) > m_BsToDs_LxyDs) {

                  std::vector<xAOD::TrigBphys*> triggerObjects = state.addTriggerObjects(2, muonIndex);
                  ATH_CHECK( fillTriggerObjects(triggerObjects, xAOD::TrigBphys::BS2DSMUX, *result, state.beamSpotPosition()) );
                  mon_nBPhysObject++;
                }
              }
              else {
                xAOD::TrigBphys* triggerObject = state.addTriggerObject(muonIndex);
                ATH_CHECK( fillTriggerObject(*triggerObject, xAOD::TrigBphys::BS2DSMUX, *vtx_Ds, s_trkMass[kDs][0]) );
                xAOD::TrackParticle::GenVecFourMom_t p_Ds(ROOT::Math::PtEtaPhiMVector(triggerObject->pt(), triggerObject->eta(), triggerObject->phi(), triggerObject->fitmass()));
                triggerObject->setMass((p_mu + p_Ds).M());
                mon_nBPhysObject++;
              }
            }
          }  // end of B_s0

          // Lambda_b0 -> mu+ nu_mu anti-Lambda_c-(-> anti-p K+ pi-)
          if (m_LambdaBToLambdaC &&
              charge1 * charge3 > 0. && charge1 * charge2 < 0. &&
              p_trk1.Pt() > m_LambdaBToLambdaC_minProtonPt &&
              p_trk2.Pt() > m_LambdaBToLambdaC_minKaonPt &&
              p_trk3.Pt() > m_LambdaBToLambdaC_minPionPt &&
              (p_trk1 + p_trk2 + p_trk3).Pt() > m_LambdaBToLambdaC_minLambdaCPt &&
              isInMassRange((p_trk1.SetM(PDG::mProton) + p_trk2.SetM(PDG::mKaon) + p_trk3.SetM(PDG::mPion)).M(), m_LambdaBToLambdaC_LambdaCMassRange) &&
              isInMassRange((p_mu + p_trk1.SetM(PDG::mProton) + p_trk2.SetM(PDG::mKaon) + p_trk3.SetM(PDG::mPion)).M(), m_LambdaBToLambdaC_massRange)) {
            auto vtx_LambdaC = fit(state.context(), trackParticleLinks, kLambdaC);
            ++iterations;

            if (vtx_LambdaC && vtx_LambdaC->chiSquared() < m_LambdaBToLambdaC_chi2) {
              ATH_MSG_DEBUG( "Partially reconstructed Lambda_b0 -> mu+ nu_mu anti-Lambda_c-(-> anti-p K+ pi-) candidate has been created from { " << itrk1 << ", " << itrk2 << ", " << itrk3 << " }" );
              if (m_makeCascadeFit) {
                auto result = fitCascade(state.context(), trackParticleLinks, kLambdaC);
                if (result &&
                    result->fitChi2() < m_cascadeChi2 && result->nDoF() == 4 &&
                    Lxy(state.beamSpotPosition(), result->vertices()[1]->position(), result->getParticleMoms()[1]) > m_LambdaBToLambdaC_LxyLb &&
                    Lxy(result->vertices()[1]->position(), result->vertices()[0]->position(), result->getParticleMoms()[0]) > m_LambdaBToLambdaC_LxyLc) {

                  std::vector<xAOD::TrigBphys*> triggerObjects = state.addTriggerObjects(2, muonIndex);
                  ATH_CHECK( fillTriggerObjects(triggerObjects, xAOD::TrigBphys::LB2LCMUX, *result, state.beamSpotPosition()) );
                  mon_nBPhysObject++;
                }
              }
              else {
                xAOD::TrigBphys* triggerObject = state.addTriggerObject(muonIndex);
                ATH_CHECK( fillTriggerObject(*triggerObject, xAOD::TrigBphys::LB2LCMUX, *vtx_LambdaC, s_trkMass[kLambdaC][0]) );
                xAOD::TrackParticle::GenVecFourMom_t p_LambdaC(ROOT::Math::PtEtaPhiMVector(triggerObject->pt(), triggerObject->eta(), triggerObject->phi(), triggerObject->fitmass()));
                triggerObject->setMass((p_mu + p_LambdaC).M());
                mon_nBPhysObject++;
              }
            }
          }  // end of Lambda_b0

        }
        vtx_D0.reset();

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
  }
  return StatusCode::SUCCESS;
}


StatusCode TrigBmuxComboHypo::createDecisionObjects(TrigBmuxState& state) const {

  size_t idx = 0;
  for (const xAOD::TrigBphys* triggerObject : state.trigBphysCollection()) {
    if (triggerObject->particleType() == xAOD::TrigBphys::UNKNOWNPTYPE) continue;

    ATH_MSG_DEBUG( "Found xAOD::TrigBphys object: mass = " << triggerObject->mass() );

    auto triggerObjectEL = ElementLink<xAOD::TrigBphysContainer>(state.trigBphysCollection(), triggerObject->index());
    ATH_CHECK( triggerObjectEL.isValid() );

    const auto& muon = state.muons.at(state.trigBphysMuonIndices.at(idx));

    // create a new output Decision object, backed by the 'decisions' container.
    Decision* decision = TrigCompositeUtils::newDecisionIn(&state.decisions(), TrigCompositeUtils::comboHypoAlgNodeName());

    // add link to the previous decision
    decision->addObjectCollectionLink(TrigCompositeUtils::seedString(), muon.decisionLink);

    // set mandatory link to the trigger object
    decision->setObjectLink<xAOD::TrigBphysContainer>(TrigCompositeUtils::featureString(), triggerObjectEL);

    std::vector<const DecisionIDContainer*> previousDecisionIDs(1, &muon.decisionIDs);
    for (const auto& tool : hypoTools()) {
      ATH_MSG_DEBUG( "Go to " << tool );
      ATH_CHECK( tool->decideOnSingleObject(decision, previousDecisionIDs) );
    }
    ++idx;
  }

  return StatusCode::SUCCESS;
}


std::unique_ptr<xAOD::Vertex> TrigBmuxComboHypo::fit(
    const EventContext& context,
    const std::vector<ElementLink<xAOD::TrackParticleContainer>>& trackParticleLinks,
    Decay decay) const {

  ATH_MSG_DEBUG( "Perform vertex fit" );

  if (trackParticleLinks.size() < 3) {
    ATH_MSG_WARNING( "At least muon and two tracks should be given to the vertex fitter" );
    return std::unique_ptr<xAOD::Vertex>(nullptr);
  }

  std::vector<const xAOD::TrackParticle*> tracklist(trackParticleLinks.size() - 1, nullptr);
  std::transform(trackParticleLinks.begin() + 1, trackParticleLinks.end(), tracklist.begin(),
                 [](const ElementLink<xAOD::TrackParticleContainer>& link){ return *link; });

  Amg::Vector3D startingPoint = Amg::Vector3D::Zero(3);
  int flag = 0;
  int errorcode = 0;
  const Trk::Perigee& perigee1 = tracklist[0]->perigeeParameters();
  const Trk::Perigee& perigee2 = tracklist[1]->perigeeParameters();
  startingPoint = m_vertexPointEstimator->getCirclesIntersectionPoint(&perigee1, &perigee2, flag, errorcode);
  if (errorcode != 0) startingPoint = Amg::Vector3D::Zero(3);
  ATH_MSG_DEBUG( "Starting point: (" << startingPoint(0) << ", " << startingPoint(1) << ", " << startingPoint(2) << ")" );

  auto fitterState = m_vertexFitter->makeState(context);
  m_vertexFitter->setMassInputParticles(s_trkMass[static_cast<size_t>(decay)][0], *fitterState);

  std::unique_ptr<xAOD::Vertex> vertex(m_vertexFitter->fit(tracklist, startingPoint, *fitterState));
  if (!vertex) {
    ATH_MSG_DEBUG( "Vertex fit fails" );
    return vertex;
  }
  if (vertex->chiSquared() > 50.) {
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


std::unique_ptr<Trk::VxCascadeInfo> TrigBmuxComboHypo::fitCascade(
    const EventContext& context,
    const std::vector<ElementLink<xAOD::TrackParticleContainer>>& trackParticleLinks,
    Decay decay) const {

  if (trackParticleLinks.size() != (decay == kD0 ? 3 : 4)) {
    ATH_MSG_WARNING( "fitCascade(): " << (decay == kD0 ? 3 : 4) << " tracks required for decay " << decay << ", but trackParticleLinks size = " << trackParticleLinks.size() );
    return std::unique_ptr<Trk::VxCascadeInfo>(nullptr);
  }

  std::vector<std::vector<ElementLink<xAOD::TrackParticleContainer>>> vtx_trackParticleLinks;
  switch (decay) {

    case kD0:           // B+ -> mu+ nu_mu anti-D0(-> K+ pi-)
      vtx_trackParticleLinks = {{trackParticleLinks[1], trackParticleLinks[2]},   // B+.anti-D0.K+, B+.anti-D0.pi-
                                {trackParticleLinks[0]}};                         // B+.mu+
      break;

    case kDstar:        // B0 -> mu+ nu_mu D*-(-> anti-D0(-> K+ pi-) pi-)
      vtx_trackParticleLinks = {{trackParticleLinks[1], trackParticleLinks[2]},   // B0.D*-.anti-D0.K+, B0.D*-.anti-D0.pi-
                                {trackParticleLinks[0], trackParticleLinks[3]}};  // B0.mu+, B0.D*-.pi-
      break;

    case kDs:           // B_s0 -> mu+ nu_mu D_s-(->phi(-> K+ K-) pi-)
    case kDplus:        // B0 -> mu+ nu_mu D-(-> K+ pi- pi-)
    case kLambdaC:      // anti-Lambda_b0 -> mu+ nu_mu anti-Lambda_c-(-> anti-p K+ pi-)
      vtx_trackParticleLinks = {{trackParticleLinks[1], trackParticleLinks[2], trackParticleLinks[3]},  // B_s0.D_s-.phi.K+, B_s0.D_s-.phi.K-, B_s0.D_s-.pi-
                                {trackParticleLinks[0]}};                                               // B_s0.mu+
      break;

    default:
      ATH_MSG_WARNING( "fitCascade(): decay " << decay << " has not been defined in McTopologyTool" );
      return std::unique_ptr<Trk::VxCascadeInfo>(nullptr);
      break;
  }

  std::vector<std::vector<const xAOD::TrackParticle*>> vtx_tracks(vtx_trackParticleLinks.size());
  for (size_t i = 0; i < vtx_trackParticleLinks.size(); ++i) {
    for (size_t j = 0; j < vtx_trackParticleLinks[i].size(); ++j) {
      vtx_tracks[i].push_back(*vtx_trackParticleLinks[i][j]);
    }
  }

  auto state = m_vertexFitter->makeState(context);
  m_vertexFitter->setRobustness(0, *state);

  std::vector<Trk::VertexID> precedingVertices(1, m_vertexFitter->startVertex(vtx_tracks[0], s_trkMass[static_cast<size_t>(decay)][0], *state));
  m_vertexFitter->nextVertex(vtx_tracks[1], s_trkMass[static_cast<size_t>(decay)][1], precedingVertices, *state);
  std::unique_ptr<Trk::VxCascadeInfo> result(m_vertexFitter->fitCascade(*state));

  if (result) {
    ATH_MSG_DEBUG( "Cascade fit is successful: chi2 = " << result->fitChi2() << "; NDF = " << result->nDoF() );
    result->setSVOwnership(true);

    size_t i = 0;
    for (xAOD::Vertex* vertex : result->vertices()) {
      vertex->clearTracks();
      vertex->setTrackParticleLinks(vtx_trackParticleLinks[i++]);
    }
  }

  return result;
}


StatusCode TrigBmuxComboHypo::fillTriggerObject(
    xAOD::TrigBphys& triggerObject,
    xAOD::TrigBphys::pType type,
    const xAOD::Vertex& vertex,
    const std::vector<double>& trkMass) const {

  // refitted track momentum as a 4-vector for mass hypothesis defined by the given decay value
  xAOD::TrackParticle::GenVecFourMom_t momentum;
  std::vector<xAOD::TrackParticle::GenVecFourMom_t> momenta;
  ATH_CHECK( vertex.vxTrackAtVertexAvailable() );
  ATH_CHECK( vertex.vxTrackAtVertex().size() == trkMass.size() );
  for (size_t i = 0; i < vertex.vxTrackAtVertex().size(); ++i) {
    const Trk::TrackParameters* perigee = vertex.vxTrackAtVertex()[i].perigeeAtVertex();
    ATH_CHECK( perigee != nullptr );
    const Amg::Vector3D& p = perigee->momentum();
    momenta.emplace_back(p.x(), p.y(), p.z(), trkMass[i]);
    momentum += momenta.back();
  }

  triggerObject.initialise(0, momentum.Eta(), momentum.Phi(), momentum.Pt(), type, momentum.M(), xAOD::TrigBphys::EF);

  triggerObject.setFitmass(momentum.M());
  triggerObject.setFitx(vertex.x());
  triggerObject.setFity(vertex.y());
  triggerObject.setFitz(vertex.z());
  triggerObject.setFitchi2(vertex.chiSquared());
  triggerObject.setFitndof(vertex.numberDoF());

  // set all the particles associated with the decay
  triggerObject.setTrackParticleLinks(vertex.trackParticleLinks());

  ATH_MSG_DEBUG(
    "TrigBphys object:\n\t  " <<
    "roiId:         " << triggerObject.roiId()  << "\n\t  " <<
    "particleType:  " << triggerObject.particleType() << "\n\t  " <<
    "level:         " << triggerObject.level() << "\n\t  " <<
    "eta:           " << triggerObject.eta() << "\n\t  " <<
    "phi:           " << triggerObject.phi() << "\n\t  " <<
    "mass:          " << triggerObject.mass() << "\n\t  " <<
    "fitmass:       " << triggerObject.fitmass() << "\n\t  " <<
    "chi2/NDF:      " << triggerObject.fitchi2() << " / " << triggerObject.fitndof() << "\n\t  " <<
    "vertex:        (" << triggerObject.fitx() << ", " << triggerObject.fity() << ", " << triggerObject.fitz() << ")" );

  return StatusCode::SUCCESS;
}


StatusCode TrigBmuxComboHypo::fillTriggerObjects(
    std::vector<xAOD::TrigBphys*>& triggerObjects,
    xAOD::TrigBphys::pType type,
    const Trk::VxCascadeInfo& vxCascadeInfo,
    const Amg::Vector3D& beamSpotPosition) const {

  const std::vector<xAOD::Vertex*>& vertices = vxCascadeInfo.vertices();
  const std::vector<std::vector<TLorentzVector>>& momenta = vxCascadeInfo.getParticleMoms();
  ATH_CHECK( triggerObjects.size() == vertices.size() );

  for (size_t i = 0; i < vertices.size(); ++i) {
    const xAOD::Vertex* vertex = vertices[i];
    TLorentzVector momentum = std::accumulate(momenta[i].begin(), momenta[i].end(), TLorentzVector());
    xAOD::TrigBphys* triggerObject = triggerObjects[i];
    triggerObject->initialise(0, momentum.Eta(), momentum.Phi(), momentum.Pt(), xAOD::TrigBphys::UNKNOWNPTYPE, momentum.M(), xAOD::TrigBphys::EF);
    triggerObject->setFitmass(momentum.M());
    triggerObject->setFitx(vertex->x());
    triggerObject->setFity(vertex->y());
    triggerObject->setFitz(vertex->z());
    triggerObject->setTrackParticleLinks(vertex->trackParticleLinks());
  }

  // D0/D(*)-/D_s-/Lambda_c+
  triggerObjects[0]->setLxy(Lxy(vertices[1]->position(), vertices[0]->position(), momenta[0]));
  triggerObjects[0]->setSecondaryDecayLink(ElementLink<xAOD::TrigBphysContainer>(m_trigBphysContainerKey.key(), triggerObjects[1]->index()));
  ATH_CHECK( triggerObjects[0]->secondaryDecayLink().isValid() );

  // B+/B0/B_s0/Lambda_b0
  triggerObjects[1]->setParticleType(type);
  triggerObjects[1]->setFitchi2(vxCascadeInfo.fitChi2());
  triggerObjects[1]->setFitndof(vxCascadeInfo.nDoF());
  triggerObjects[1]->setLxy(Lxy(beamSpotPosition, vertices[1]->position(), momenta[1]));
  triggerObjects[1]->setLowerChainLink(ElementLink<xAOD::TrigBphysContainer>(m_trigBphysContainerKey.key(), triggerObjects[0]->index()));
  ATH_CHECK( triggerObjects[1]->lowerChainLink().isValid() );

  if (msgLvl(MSG::DEBUG)) {
    const xAOD::TrigBphys* vtx0 = triggerObjects[0];
    const xAOD::TrigBphys* vtx1 = triggerObjects[1];
    ATH_MSG_DEBUG(
      "TrigBphys object:\n\t  " <<
      "particleType:  " << vtx1->particleType() << "\n\t  " <<
      "chi2/NDF:      " << vtx1->fitchi2() << " / " << vtx1->fitndof() << "\n\t  " <<
      "vertex0:       (" << vtx0->fitx() << ", " << vtx0->fity() << ", " << vtx0->fitz() << ")\n\t  " <<
      "vertex1:       (" << vtx1->fitx() << ", " << vtx1->fity() << ", " << vtx1->fitz() << ")\n\t  " <<
      "Lxy(B):        " << vtx1->lxy() << "\n\t  " <<
      "Lxy(D):        " << vtx0->lxy() );
  }

  return StatusCode::SUCCESS;
}


bool TrigBmuxComboHypo::isIdenticalTracks(const xAOD::TrackParticle* lhs, const xAOD::TrackParticle* rhs) const {

  if (lhs->charge() * rhs->charge() < 0.) return false;
  return (ROOT::Math::VectorUtil::DeltaR(lhs->genvecP4(), rhs->genvecP4()) < m_deltaR);
}


double TrigBmuxComboHypo::getTrkImpactParameterZ0(const xAOD::TrackParticle& track, const Amg::Vector3D& vertex) const {

  std::unique_ptr<const Trk::Perigee> perigee(m_trackToVertexTool->perigeeAtVertex(track, vertex));
  double z0 = perigee->parameters()[Trk::z0];
  return z0;
}


double TrigBmuxComboHypo::Lxy(const Amg::Vector3D& productionVertex, const Amg::Vector3D& decayVertex, const std::vector<TLorentzVector>& momenta) const {

  XYVector R(decayVertex.x() - productionVertex.x(), decayVertex.y() - productionVertex.y());
  XYVector pT;
  for (const auto& p : momenta) {
    pT += XYVector(p.Px(), p.Py());
  }
  return R.Dot(pT.unit());
}
