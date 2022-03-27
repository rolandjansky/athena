/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkMuons/dimuonTaggingTool.h"

#include "AthenaKernel/errorcheck.h"
#include "FourMomUtils/xAODP4Helpers.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackingPrimitives.h"

namespace {
    using MuonPassDecor = SG::WriteDecorHandle<xAOD::MuonContainer, bool>;

}
namespace DerivationFramework {
    dimuonTaggingTool::dimuonTaggingTool(const std::string& t, const std::string& n, const IInterface* p) : AthAlgTool(t, n, p) {
        declareInterface<DerivationFramework::IAugmentationTool>(this);
    }

    // Destructor
    dimuonTaggingTool::~dimuonTaggingTool() = default;

    // Athena initialize and finalize
    StatusCode dimuonTaggingTool::initialize() {
        ATH_MSG_VERBOSE("initialize() ...");

        // trigger decision tool, needed when there is trigger requirement
        if (m_orTrigs.size() > 0 || m_andTrigs.size() > 0) {
            if (m_trigDecisionTool.retrieve().isFailure()) {
                ATH_MSG_FATAL("Failed to retrieve tool: " << m_trigDecisionTool);
                return StatusCode::FAILURE;
            }
            ATH_MSG_INFO("Retrieved tool: " << m_trigDecisionTool);
        }

        // load the matching tool
        ATH_CHECK(m_matchTool.retrieve());
        ATH_MSG_INFO("Successfully retrived the TrigMatchTool!");

        ATH_CHECK(m_evtKey.initialize());
        ATH_CHECK(m_muonSGKey.initialize());
        ATH_CHECK(m_trackSGKey.initialize());
        ATH_CHECK(m_truthSGKey.initialize(!m_truthSGKey.empty()));

        m_muonKeepKey = m_muonSGKey.key() + "." + m_br_prefix.value();
        m_trkKeepKey = m_trackSGKey.key() + "." + m_br_prefix.value();
        m_skimmingKey = m_br_prefix + "DIMU_pass";
        ATH_CHECK(m_muonKeepKey.initialize());
        ATH_CHECK(m_trkKeepKey.initialize());
        ATH_CHECK(m_skimmingKey.initialize());

        m_invariantMassLow2 = m_invariantMassLow * std::abs(m_invariantMassLow);
        m_invariantMassHigh2 = m_invariantMassHigh * std::abs(m_invariantMassHigh);
        m_thinningConeSize2 = m_thinningConeSize * std::abs(m_thinningConeSize);

        return StatusCode::SUCCESS;
    }

    bool dimuonTaggingTool::checkTrigMatch(const xAOD::Muon* mu, const std::vector<std::string>& Trigs) const {
        for (const std::string& t : Trigs) {
            if (m_matchTool->match(mu, t)) return true;
        }
        return Trigs.empty();
    }

    StatusCode dimuonTaggingTool::addBranches() const {
        const EventContext& ctx = Gaudi::Hive::currentContext();

        SG::ReadHandle<xAOD::EventInfo> eventInfo{m_evtKey, ctx};
        if (!eventInfo.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve " << m_evtKey.fullKey());
            return StatusCode::FAILURE;
        }
        bool isMC = eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION);

        const xAOD::TrackParticleContainer* tracks{nullptr};
        if (!m_trackSGKey.empty()) {
            SG::ReadHandle<xAOD::TrackParticleContainer> handle{m_trackSGKey, ctx};
            if (!handle.isValid()) {
                ATH_MSG_FATAL("Failed to retrieve " << m_trackSGKey.fullKey());
                return StatusCode::FAILURE;
            }
            tracks = handle.cptr();
        }
        SG::WriteHandle<int> keepEventHandle{m_skimmingKey, ctx};
        ATH_CHECK(keepEventHandle.record(std::make_unique<int>(0)));
        int& keepEvent = *keepEventHandle;

        //// check Or triggers
        for (const std::string& or_trig : m_orTrigs) {
            if (m_trigDecisionTool->isPassed(or_trig)) {
                keepEvent = 100;
                break;
            }
        }

        //// check "and" triggers if didn't pass "Or" triggers
        if (!keepEvent && !m_andTrigs.empty()) {
            bool passAndTriggers = true;
            for (const std::string& and_trig : m_andTrigs) {
                if (m_trigDecisionTool->isPassed(and_trig)) {
                    passAndTriggers = false;
                    break;
                }
            }
            if (!passAndTriggers) return StatusCode::SUCCESS;
        }

        /// muon selection
        SG::ReadHandle<xAOD::MuonContainer> muons{m_muonSGKey, ctx};
        if (!muons.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve " << m_muonSGKey.fullKey());
            return StatusCode::FAILURE;
        }

        TrackPassDecor trk_decor{m_trkKeepKey, ctx};
        MuonPassDecor muo_decor{m_muonKeepKey, ctx};
        if (!muons->empty()) muo_decor(*muons->at(0)) = false;
        if (!trk_decor->empty()) trk_decor(*trk_decor->at(0)) = false;

        for (const xAOD::Muon* mu_itr1 : *muons) {
            if (!passMuonCuts(mu_itr1, m_mu1PtMin, m_mu1AbsEtaMax, m_mu1Types, m_mu1Trigs, m_mu1IsoCuts)) continue;
            for (const xAOD::Muon* mu_itr2 : *muons) {
                if (mu_itr2 == mu_itr1) continue;
                if (!passMuonCuts(mu_itr2, m_mu2PtMin, m_mu2AbsEtaMax, m_mu2Types, m_mu2Trigs, m_mu2IsoCuts)) continue;
                if (!muonPairCheck(mu_itr1, mu_itr2)) continue;
                muo_decor(*mu_itr1) = true;
                muo_decor(*mu_itr2) = true;
                ++keepEvent;
            }
            if (m_useTrackProbe) {
                for (const xAOD::TrackParticle* probe_trk : *tracks) {  ///
                    if (probe_trk == mu_itr1->trackParticle(xAOD::Muon::TrackParticleType::InnerDetectorTrackParticle)) continue;
                    if (!passKinematicCuts(probe_trk, m_mu2PtMin, m_mu2AbsEtaMax)) continue;
                    if (!muonPairCheck(mu_itr1, probe_trk)) continue;
                    trk_decor(*probe_trk) = true;
                    ++keepEvent;
                    maskNearbyIDtracks(probe_trk, trk_decor);
                }
            }
        }

        /// also mask tracks around truth muons.
        if (isMC && !m_truthSGKey.empty()) {
            SG::ReadHandle<xAOD::TruthParticleContainer> truth{m_truthSGKey, ctx};
            if (!truth.isValid()) {
                ATH_MSG_FATAL("Failed to retrieve truth container " << m_truthSGKey.fullKey());
                return StatusCode::FAILURE;
            }
            for (const xAOD::TruthParticle* mu_itr2 : *truth) maskNearbyIDtracks(mu_itr2, trk_decor);
        }

        return StatusCode::SUCCESS;
    }
    void dimuonTaggingTool::maskNearbyIDtracks(const xAOD::IParticle* ref_part, TrackPassDecor& decor) const {
        for (const xAOD::TrackParticle* trk : *decor) {
            if (xAOD::P4Helpers::deltaR2(ref_part, trk) < m_thinningConeSize2) decor(*trk) = true;
        }
    }

    bool dimuonTaggingTool::passKinematicCuts(const xAOD::IParticle* mu, float ptMin, float absEtaMax) const {
        return !(!mu || mu->pt() < ptMin || std::abs(mu->eta()) > absEtaMax);
    }

    template <class probe_type> bool dimuonTaggingTool::muonPairCheck(const xAOD::Muon* mu1, const probe_type* mu2) const {
        if (m_requireOS != (mu1->charge() * mu2->charge() < 0)) return false;
        if (m_dPhiMin > 0 && std::abs(xAOD::P4Helpers::deltaPhi(mu1, mu2)) < m_dPhiMin) return false;
        const float mass2 = (mu1->p4() + mu2->p4()).M2();
        return !(mass2 < m_invariantMassLow2 || (m_invariantMassHigh > 0. && mass2 > m_invariantMassHigh2));
    }

    bool dimuonTaggingTool::passMuonCuts(const xAOD::Muon* mu, const float ptMin, const float absEtaMax, const std::vector<int>& types,
                                         const std::vector<std::string>& trigs, const std::map<int, double> muIsoCuts) const {
        if (!passMuonCuts(mu, ptMin, absEtaMax, types, trigs)) return false;
        /// isolation cuts. Mutiple cuts allowed and return the logical AND results.
        for (std::map<int, double>::const_iterator it = muIsoCuts.begin(); it != muIsoCuts.end(); ++it) {
            float isoValue = 0;
            const xAOD::Iso::IsolationType isoType = static_cast<xAOD::Iso::IsolationType>(it->first);
            if (!(mu->isolation(isoValue, isoType)) || isoValue > it->second) return false;
        }
        return true;
    }

    bool dimuonTaggingTool::passMuonCuts(const xAOD::Muon* mu, const float ptMin, const float absEtaMax, const std::vector<int>& types,
                                         const std::vector<std::string>& trigs) const {
        if (!passKinematicCuts(mu, ptMin, absEtaMax)) return false;
        if (!types.empty() &&
            std::find_if(types.begin(), types.end(), [mu](const int& type) { return type == mu->muonType(); }) == types.end())
            return false;

        return checkTrigMatch(mu, trigs);
    }
}  // namespace DerivationFramework