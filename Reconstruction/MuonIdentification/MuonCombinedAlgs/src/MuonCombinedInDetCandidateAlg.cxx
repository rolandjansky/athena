/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedInDetCandidateAlg.h"

#include "MuonDetDescrUtils/MuonSectorMapping.h"
#include "MuonLayerEvent/MuonSystemExtension.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/xAODTruthHelpers.h"

using namespace MuonCombined;
MuonCombinedInDetCandidateAlg::MuonCombinedInDetCandidateAlg(const std::string& name, ISvcLocator* pSvcLocator) :
    AthReentrantAlgorithm(name, pSvcLocator) {}

StatusCode MuonCombinedInDetCandidateAlg::initialize() {
    ATH_CHECK(m_trackSelector.retrieve(DisableTool{m_trackSelector.empty()}));
    ATH_CHECK(m_muonSystemExtensionTool.retrieve());
    ATH_CHECK(m_indetTrackParticleLocation.initialize());
    ATH_CHECK(m_caloExtensionLocation.initialize());
    ATH_CHECK(m_indetForwardTrackParticleLocation.initialize(m_doSiliconForwardMuons));
    ATH_CHECK(m_caloFwdExtensionLocation.initialize(m_doSiliconForwardMuons && !m_caloFwdExtensionLocation.empty()));
    ATH_CHECK(m_candidateCollectionName.initialize());
    ATH_CHECK(m_forwardTrackSelector.retrieve(DisableTool{!m_doSiliconForwardMuons}));
    ATH_MSG_INFO("Successfully initialized using the following configuration --  SAF: "<<(m_doSiliconForwardMuons? "si":"no")<<", "
                << "MS extension bulk: "<<(m_extendBulk ? "si" : "no")
                <<", MS extension SAF: "<<( m_doSiliconForwardMuons && m_extendSAF ? "si" : "no")
                <<", Require MS estension: "<<( m_requireExtension? "si" : "no")<<" min pT for extension "<<m_extThreshold);
    return StatusCode::SUCCESS;
}

StatusCode MuonCombinedInDetCandidateAlg::execute(const EventContext& ctx) const {
    InDetCandidateCache output_cache{};
    unsigned int counter{0};  
    for (SG::ReadHandle<xAOD::TrackParticleContainer>& readHandle : m_indetTrackParticleLocation.makeHandles(ctx)) {
        if (!readHandle.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve " << readHandle.key());
            return StatusCode::FAILURE;
        }
        output_cache.inDetContainer = readHandle.cptr();
        output_cache.trackSelector = !m_trackSelector.empty() ? m_trackSelector.get() : nullptr;
        if (counter < m_caloExtensionLocation.size()){
            SG::ReadHandle<CaloExtensionCollection> caloExtension{m_caloExtensionLocation[counter], ctx};
            if (!caloExtension.isValid()) {
                ATH_MSG_FATAL("Failed to retrieve "<<m_caloExtensionLocation[counter].fullKey() );
                return StatusCode::FAILURE;
            }
            output_cache.extensionContainer = caloExtension.cptr();
        } else output_cache.extensionContainer = nullptr;
        ++counter;
        ATH_CHECK(create(ctx, output_cache));
    }
    if (m_doSiliconForwardMuons) {
        SG::ReadHandle<xAOD::TrackParticleContainer> readHandle{m_indetForwardTrackParticleLocation, ctx};
        if (!readHandle.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve " << readHandle.key());
            return StatusCode::FAILURE;
        }
        if (!m_caloFwdExtensionLocation.empty()) {
            SG::ReadHandle<CaloExtensionCollection> caloExtension{m_caloFwdExtensionLocation, ctx};
            if (!caloExtension.isValid()) {
                ATH_MSG_FATAL("Failed to retrieve "<<m_caloFwdExtensionLocation.fullKey());
                return StatusCode::FAILURE;
            }
            output_cache.extensionContainer = caloExtension.cptr();
        } else output_cache.extensionContainer = nullptr;

        output_cache.inDetContainer = readHandle.cptr();
        output_cache.trackSelector = !m_forwardTrackSelector.empty() ? m_forwardTrackSelector.get() : nullptr;
        output_cache.flagAsSAF = true;
        ATH_CHECK(create(ctx, output_cache));
    }

    SG::WriteHandle<InDetCandidateCollection> indetCandidateCollection(m_candidateCollectionName, ctx);
    ATH_CHECK(indetCandidateCollection.record(std::move(output_cache.outputContainer)));

    return StatusCode::SUCCESS;
}

StatusCode MuonCombinedInDetCandidateAlg::create(const EventContext& ctx, InDetCandidateCache& output_cache) const {
    ATH_MSG_DEBUG("Producing InDetCandidates for " << output_cache.inDetContainer->size());
    for (const xAOD::TrackParticle* tp : *output_cache.inDetContainer) {
        if (!isValidTrackParticle(output_cache.trackSelector, tp)) continue;

        ElementLink<xAOD::TrackParticleContainer> link(*output_cache.inDetContainer, tp->index());
        if (!link.isValid()) {
            ATH_MSG_WARNING("Bad element link ");
            continue;
        } else if (*link != tp) {
            ATH_MSG_WARNING("Dereferenced Link not equal to TrackParticle, skipping track ");
            continue;
        }
        link.toPersistent();
        printTrackParticleInfo(tp, "Creating");

        if (msgLvl(MSG::VERBOSE)) {
            const xAOD::TruthParticle* truth_part = xAOD::TruthHelpers::getTruthParticle(*tp);
            if (truth_part) {
                ATH_MSG_VERBOSE("  Truth particle: pdgId " << truth_part->pdgId() << " type " << tp->auxdata<int>("truthType")
                                                           << " origin " << tp->auxdata<int>("truthOrigin") << " pt " << truth_part->pt()
                                                           << " eta " << truth_part->eta() << " phi " << truth_part->phi());
            }
        }
        Muon::IMuonSystemExtensionTool::SystemExtensionCache cache;
        cache.extensionContainer = output_cache.extensionContainer;
        cache.candidate = std::make_unique<InDetCandidate>(link);
        cache.candidate->setSiliconAssociated(output_cache.flagAsSAF);  // Si-associated candidates don't need these
        cache.useHitSectors = false;
        if (tp->pt() < m_extThreshold) continue;
        /// MuGirl only operates on ID tracks with pt at least this high   
        cache.createSystemExtension = (tp->pt() >= m_extThreshold) && 
                                      ( (m_extendSAF && cache.candidate->isSiliconAssociated()) ||
                                        (m_extendBulk && !cache.candidate->isSiliconAssociated()));
        cache.requireSystemExtension = m_requireExtension;   
        if (!m_muonSystemExtensionTool->muonSystemExtension(ctx, cache)) continue;
        output_cache.outputContainer->push_back(std::move(cache.candidate));
    }
    ATH_MSG_DEBUG("InDetCandidates selected " << output_cache.outputContainer->size());
    return StatusCode::SUCCESS;
}

bool MuonCombinedInDetCandidateAlg::isValidTrackParticle(const Trk::ITrackSelectorTool* currentTrackSelector,
                                                         const xAOD::TrackParticle* const tp) const {
    if (!tp->perigeeParameters().covariance()) {
        ATH_MSG_WARNING("InDet TrackParticle without perigee! ");
        return false;
    }
    if (!tp->track()) {
        ATH_MSG_WARNING("The track particle has not an associated track");
        return false;
    }
    if (currentTrackSelector && !currentTrackSelector->decision(*tp)) {
        if (msgLvl(MSG::VERBOSE) && tp->pt() > 5000.) printTrackParticleInfo(tp, "Discarding");
        return false;
    }
    return true;
}

void MuonCombinedInDetCandidateAlg::printTrackParticleInfo(const xAOD::TrackParticle* const tp, const std::string& what) const {
    ATH_MSG_DEBUG(what << " InDet TrackParticle: pt " << tp->pt() << " eta " << tp->eta() << " phi " << tp->phi() << " Pixel "
                       << getCount(*tp, xAOD::numberOfInnermostPixelLayerHits) + getCount(*tp, xAOD::numberOfPixelHits) << " SCT "
                       << getCount(*tp, xAOD::numberOfSCTHits) << " TRT " << getCount(*tp, xAOD::numberOfTRTHits));
}

int MuonCombinedInDetCandidateAlg::getCount(const xAOD::TrackParticle& tp, xAOD::SummaryType type) const {
    uint8_t val{0};
    if (!tp.summaryValue(val, type)) return 0;
    return static_cast<int>(val);
}
