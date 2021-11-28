/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCombinedInDetCandidateAlg.h"

#include "MuonDetDescrUtils/MuonSectorMapping.h"
#include "MuonLayerEvent/MuonSystemExtension.h"
#include "xAODTruth/TruthParticleContainer.h"

namespace {
    static const SG::AuxElement::Accessor<ElementLink<xAOD::TruthParticleContainer>> acc_truth_link("truthParticleLink");
    constexpr unsigned int num_sectors = 16;
    using RegionIndex = Muon::MuonStationIndex::DetectorRegionIndex;
    using LayerIndex = Muon::MuonStationIndex::LayerIndex;
}  // namespace
using namespace MuonCombined;

MuonCombinedInDetCandidateAlg::MuonCombinedInDetCandidateAlg(const std::string& name, ISvcLocator* pSvcLocator) :
    AthReentrantAlgorithm(name, pSvcLocator) {}

StatusCode MuonCombinedInDetCandidateAlg::initialize() {
    ATH_CHECK(m_trackSelector.retrieve(DisableTool{m_trackSelector.empty()}));
    ATH_CHECK(m_muonSystemExtensionTool.retrieve());
    ATH_CHECK(m_indetTrackParticleLocation.initialize());
    ATH_CHECK(m_indetForwardTrackParticleLocation.initialize(m_doSiliconForwardMuons));
    ATH_CHECK(m_candidateCollectionName.initialize());
    ATH_CHECK(m_forwardTrackSelector.retrieve(DisableTool{!m_doSiliconForwardMuons}));

    if (m_restrictExtension) { 
        ATH_MSG_INFO("Use the Hough seeds to determine the sectors in the MS worth for being extrapolated to");
    }
    ATH_CHECK(m_houghDataPerSectorVecKey.initialize(m_restrictExtension)); 
   
    return StatusCode::SUCCESS;
}

StatusCode MuonCombinedInDetCandidateAlg::execute(const EventContext& ctx) const {
    InDetCandidateCache output_cache{};

    if (m_restrictExtension) { ATH_CHECK(findHittedSectors(ctx, m_houghDataPerSectorVecKey, output_cache)); }

    for (SG::ReadHandle<xAOD::TrackParticleContainer>& readHandle : m_indetTrackParticleLocation.makeHandles(ctx)) {
        if (!readHandle.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve " << readHandle.key());
            return StatusCode::FAILURE;
        }
        output_cache.inDetContainer = readHandle.cptr();
        output_cache.trackSelector = !m_trackSelector.empty() ? m_trackSelector.get() : nullptr;
        ATH_CHECK(create(ctx, output_cache));
    }
    if (m_doSiliconForwardMuons) {
        SG::ReadHandle<xAOD::TrackParticleContainer> readHandle{m_indetForwardTrackParticleLocation, ctx};
        if (!readHandle.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve " << readHandle.key());
            return StatusCode::FAILURE;
        }
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
    unsigned int ntracks = 0;
    int trackIndex = -1;

    for (const xAOD::TrackParticle* tp : *output_cache.inDetContainer) {
        ++trackIndex;
        if (!isValidTrackParticle(output_cache.trackSelector, tp)) continue;

        ElementLink<xAOD::TrackParticleContainer> link(*output_cache.inDetContainer, trackIndex);
        if (!link.isValid()) {
            ATH_MSG_WARNING("Bad element link ");
            continue;
        } else if (*link != tp) {
            ATH_MSG_WARNING("Dereferenced Link not equal to TrackParticle, skipping track ");
            continue;
        }
        link.toPersistent();
        printTrackParticleInfo(tp, "Creating");

        if (msgLvl(MSG::VERBOSE) && acc_truth_link.isAvailable(*tp)) {
            const ElementLink<xAOD::TruthParticleContainer>& truthLink = acc_truth_link(*tp);
            if (truthLink.isValid()) {
                ATH_MSG_VERBOSE("  Truth particle: pdgId " << (*truthLink)->pdgId() << " type " << tp->auxdata<int>("truthType")
                                                           << " origin " << tp->auxdata<int>("truthOrigin") << " pt " << (*truthLink)->pt()
                                                           << " eta " << (*truthLink)->eta() << " phi " << (*truthLink)->phi());
            }
        }
        Muon::IMuonSystemExtensionTool::SystemExtensionCache cache;
        cache.candidate = std::make_unique<InDetCandidate>(link);
        cache.candidate->setSiliconAssociated(output_cache.flagAsSAF);  // Si-associated candidates don't need these
        cache.useHittedSectors = m_restrictExtension;
        cache.sectorsWithHits = &output_cache.hitted_sectors;
        // MuGirl only operates on ID tracks with pt at least this high
        if (tp->pt() < m_extThreshold) { continue; }
        if (!m_muonSystemExtensionTool->muonSystemExtension(ctx, cache)) continue;
        ++ntracks;
        output_cache.outputContainer->push_back(std::move(cache.candidate));
    }
    ATH_MSG_DEBUG("InDetCandidates selected " << ntracks);
    return StatusCode::SUCCESS;
}

bool MuonCombinedInDetCandidateAlg::isValidTrackParticle(const Trk::ITrackSelectorTool* currentTrackSelector,
                                                         const xAOD::TrackParticle* const tp) const {
    if (!tp->perigeeParameters().covariance()) {
        ATH_MSG_WARNING("InDet TrackParticle without perigee! ");
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
    uint8_t val;
    if (!tp.summaryValue(val, type)) return 0;
    return static_cast<int>(val);
}
StatusCode MuonCombinedInDetCandidateAlg::findHittedSectors(const EventContext& ctx,
                                                            const SG::ReadHandleKey<Muon::HoughDataPerSectorVec>& key,
                                                            InDetCandidateCache& output_cache) const {
    SG::ReadHandle<Muon::HoughDataPerSectorVec> readHandle{key, ctx};
    if (!readHandle.isValid()) {
        ATH_MSG_FATAL("Failed to retrieve the prep data container " << key.fullKey());
        return StatusCode::FAILURE;
    }
    const std::vector<Muon::HoughDataPerSec>& hough_data = readHandle->vec;
    /// Helper function to cound whether all three stations are actually complete
    auto count_finished = [&output_cache]() -> unsigned int {
        unsigned int n{0};
        for (const auto& sector_hits : output_cache.hitted_sectors) { n += sector_hits.second.size() >= num_sectors; }
        return n;
    };
    /// Helper
    static const Muon::MuonSectorMapping sector_mapping;
    for (const Muon::HoughDataPerSec& sector_data : hough_data) {
        /// The phi-hit vector has a size of 3 representing
        /// the forward-backward and barrel sections
        for (int det_region = 0; det_region < Muon::MuonStationIndex::DetectorRegionIndexMax; ++det_region) {
            const RegionIndex region_index = static_cast<RegionIndex>(det_region);
            const Muon::HoughDataPerSec::PhiMaximumVec& phi_hits = sector_data.phiMaxVec[det_region];
            for (const std::shared_ptr<MuonHough::MuonPhiLayerHough::Maximum>& hit : phi_hits) {
                output_cache.hitted_sectors[region_index].insert(hit->sector);
            }
            if (count_finished() >= RegionIndex::DetectorRegionIndexMax) {
                ATH_MSG_VERBOSE("The MS is filled up with Hough seeds. We do not need to search for them any longer");
                return StatusCode::SUCCESS;
            }
            for (int layer = 0; layer < Muon::MuonStationIndex::LayerIndexMax; ++layer) {
                const LayerIndex layer_index = static_cast<LayerIndex>(layer);
                const unsigned int hash = Muon::MuonStationIndex::sectorLayerHash(region_index, layer_index);

                const Muon::HoughDataPerSec::MaximumVec& eta_hits = sector_data.maxVec[hash];
                for (const std::shared_ptr<MuonHough::MuonLayerHough::Maximum>& maxima : eta_hits) {
                    for (const std::shared_ptr<MuonHough::Hit>& hit : maxima->hits) {
                        const Trk::PrepRawData* prep_data{hit->prd};
                        if (!prep_data && hit->tgc) { prep_data = hit->tgc->phiCluster.hitList.front(); }
                        if (!prep_data) continue;
                        const Amg::Vector3D glob_pos = prep_data->detectorElement()->center(prep_data->identify());
                        output_cache.hitted_sectors[region_index].insert(sector_mapping.getSector(glob_pos.phi()));
                    }
                }
                if (count_finished() >= RegionIndex::DetectorRegionIndexMax) {
                    ATH_MSG_VERBOSE("The MS is filled up with Hough seeds. We do not need to search for them any longer");
                    return StatusCode::SUCCESS;
                }            
            }
        }
    }
    return StatusCode::SUCCESS;
}