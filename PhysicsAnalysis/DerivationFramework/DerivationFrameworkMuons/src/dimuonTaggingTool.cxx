/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkMuons/dimuonTaggingTool.h"

#include "AthenaKernel/errorcheck.h"
#include "FourMomUtils/xAODP4Helpers.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/TrackingPrimitives.h"
#include "xAODTruth/TruthParticleContainer.h"

namespace DerivationFramework {
    dimuonTaggingTool::dimuonTaggingTool(const std::string& t, const std::string& n, const IInterface* p) :
        AthAlgTool(t, n, p) {
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
        std::unique_ptr<int> keepEvent(new int(0));
        std::unique_ptr<std::vector<int> > trkStatus(new std::vector<int>());
        ATH_CHECK(fillInfo(*keepEvent, *trkStatus));

        std::string DIMU_pass(m_br_prefix + "DIMU_pass");
        if (!evtStore()->contains<int>(DIMU_pass)) { CHECK(evtStore()->record(std::move(keepEvent), DIMU_pass)); }
        std::string DIMU_trkStatus(m_br_prefix + "DIMU_trkStatus");
        if (!evtStore()->contains<std::vector<int> >(DIMU_trkStatus)) { CHECK(evtStore()->record(std::move(trkStatus), DIMU_trkStatus)); }

        return StatusCode::SUCCESS;
    }

    StatusCode dimuonTaggingTool::fillInfo(int& keepEvent, std::vector<int>& trackMask) const {
        const EventContext& ctx = Gaudi::Hive::currentContext();
        SG::ReadHandle<xAOD::EventInfo> eventInfo{m_evtKey, ctx};
        if (!eventInfo.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve " << m_evtKey.fullKey());
            return StatusCode::FAILURE;
        }
        bool isMC = eventInfo->eventType(xAOD::EventInfo::IS_SIMULATION);

        SG::ReadHandle<xAOD::TrackParticleContainer> tracks{m_trackSGKey, ctx};
        if (!tracks.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve " << m_trackSGKey.fullKey());
            return StatusCode::FAILURE;
        }

        const unsigned int NTRACKS = tracks->size();
        trackMask.assign(NTRACKS, 0);
        SG::AuxElement::Decorator<int> deco_trkFlag1(m_br_prefix + "DIMU_Status");
        for (const xAOD::TrackParticle* trk : *tracks) deco_trkFlag1(*trk) = 0;

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
        for (const xAOD::Muon* mu_itr1 : *muons) {
            if (!passMuonCuts(mu_itr1, m_mu1PtMin, m_mu1AbsEtaMax, m_mu1Types, m_mu1Trigs, m_mu1IsoCuts)) continue;
            for (const xAOD::Muon* mu_itr2 : *muons) {
                if (mu_itr2 == mu_itr1) continue;
                if (!passMuonCuts(mu_itr2, m_mu2PtMin, m_mu2AbsEtaMax, m_mu2Types, m_mu2Trigs, m_mu2IsoCuts)) continue;
                if (!muonPairCheck(mu_itr1, mu_itr2)) continue;
                ++keepEvent;
            }
            if (m_useTrackProbe) {
                for (const xAOD::TrackParticle* probe_trk : *tracks) {
                    ///
                    if (probe_trk == mu_itr1->trackParticle(xAOD::Muon::TrackParticleType::InnerDetectorTrackParticle)) continue;
                    if (!passKinematicCuts(probe_trk, m_mu2PtMin, m_mu2AbsEtaMax)) continue;
                    if (!muonPairCheck(mu_itr1, probe_trk)) continue;
                    ++keepEvent;
                    trackMask[probe_trk->index()] += 100;
                    maskNearbyIDtracks(probe_trk, trackMask, tracks.cptr());
                }
            }
        }

        /// also mask tracks around truth muons.
        if (isMC) {
            const xAOD::TruthParticleContainer* truth(0);
            ATH_CHECK(evtStore()->retrieve(truth, "MuonTruthParticles"));
            for (auto mu_itr2 : *truth) maskNearbyIDtracks(mu_itr2, trackMask, tracks.cptr());
        }

        int i(0);
        for (auto mu : *tracks) {
            int code = trackMask[i++];
            deco_trkFlag1(*mu) = code >= 100 ? 1000 : code;
        }

        return StatusCode::SUCCESS;
    }

    void dimuonTaggingTool::maskNearbyIDtracks(const xAOD::IParticle* ref_part, std::vector<int>& trackMask,
                                               const xAOD::TrackParticleContainer* tracks) const {
        unsigned int i{0};
        for (const xAOD::TrackParticle* trk : *tracks) {
            if (xAOD::P4Helpers::deltaR2(ref_part, trk) < m_thinningConeSize2) trackMask[i] += 1;
            ++i;
        }
    }

    bool dimuonTaggingTool::passKinematicCuts(const xAOD::IParticle* mu, float ptMin, float absEtaMax) const {
        return !(!mu || mu->pt() < ptMin || std::abs(mu->eta()) > absEtaMax);
    }

    template <class probe_type> bool dimuonTaggingTool::muonPairCheck(const xAOD::Muon* mu1, const probe_type* mu2) const {
        if (m_requireOS && mu1->charge() * mu2->charge() > 0) return false;
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