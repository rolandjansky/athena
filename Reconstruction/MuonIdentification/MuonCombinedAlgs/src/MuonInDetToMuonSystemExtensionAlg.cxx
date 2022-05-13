/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonInDetToMuonSystemExtensionAlg.h"

#include "AthContainers/ConstDataVector.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "MuonLayerEvent/MuonSystemExtension.h"
#include "xAODTruth/TruthParticleContainer.h"
namespace {
    constexpr unsigned int num_sectors = 16;
    using RegionIndex = Muon::MuonStationIndex::DetectorRegionIndex;
    using LayerIndex = Muon::MuonStationIndex::LayerIndex;

    inline const Trk::PrepRawData* prepData(const std::shared_ptr<MuonHough::Hit>& hit) {
        if (hit->prd) return hit->prd;
        if (hit->tgc) return hit->tgc->phiCluster.hitList.front();
        return nullptr;
    }
    /// Helper struct to store the number of hits per chamber
    /// and the average theta of a chamber per hough seed
    struct hough_chamber {
        int nhits{0};
        double eta{0.};
        std::set<int> sectors;
    };

}  // namespace

MuonInDetToMuonSystemExtensionAlg::MuonInDetToMuonSystemExtensionAlg(const std::string& name, ISvcLocator* pSvcLocator) :
    AthReentrantAlgorithm(name, pSvcLocator) {}

StatusCode MuonInDetToMuonSystemExtensionAlg::initialize() {
    ATH_CHECK(m_muonSystemExtensionTool.retrieve());
    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_CHECK(m_edmHelperSvc.retrieve());
    ATH_CHECK(m_combTagMap.initialize());
    ATH_CHECK(m_inputCandidate.initialize());
    ATH_CHECK(m_bulkInDetCandKey.initialize());
    ATH_CHECK(m_stauInDetCandKey.initialize(!m_stauInDetCandKey.empty()));
    if (m_restrictExtension) { ATH_MSG_INFO("Use the Hough seeds to determine the sectors in the MS worth for being extrapolated to"); }
    ATH_CHECK(m_houghDataPerSectorVecKey.initialize(m_restrictExtension));
    ATH_CHECK(m_segmentKey.initialize(m_restrictExtension));
    if (m_restrictExtension) { ATH_CHECK(m_segmentSelector.retrieve()); }
    return StatusCode::SUCCESS;
}

StatusCode MuonInDetToMuonSystemExtensionAlg::execute(const EventContext& ctx) const {
    InDetCandidateCache output_cache{};

    SG::ReadHandle<InDetCandidateCollection> input_container{m_inputCandidate, ctx};
    if (!input_container.isValid()) {
        ATH_MSG_FATAL("Failed to retrieve " << m_inputCandidate.fullKey());
        return StatusCode::FAILURE;
    }
    output_cache.input_candidates = input_container.cptr();
    ATH_CHECK(selectCandidates(ctx, output_cache));

    if (m_restrictExtension) {
        ATH_CHECK(findSegments(ctx, output_cache));
        ATH_CHECK(findHitSectors(ctx, output_cache));
    }

    ATH_MSG_DEBUG("Find the inner detector candidates to be used for MuGirl / Segment tagging");
    ATH_CHECK(create(ctx, output_cache));
    SG::WriteHandle<InDetCandidateCollection> indetCandidateCollection(m_bulkInDetCandKey, ctx);
    ATH_CHECK(indetCandidateCollection.record(std::move(output_cache.outputContainer)));

    ATH_CHECK(createStaus(ctx, indetCandidateCollection.ptr(), output_cache.tag_map));

    return StatusCode::SUCCESS;
}
StatusCode MuonInDetToMuonSystemExtensionAlg::selectCandidates(const EventContext& ctx, InDetCandidateCache& out_cache) const {
    SG::ReadHandle<MuonCombined::InDetCandidateToTagMap> tag_map{m_combTagMap, ctx};
    if (!tag_map.isValid()) {
        ATH_MSG_FATAL("Failed to retrieve the tag map from " << tag_map.fullKey());
        return StatusCode::FAILURE;
    }
    if (tag_map->empty()) {
        ATH_MSG_DEBUG("No combined candidates in this event");
        return StatusCode::SUCCESS;
    }

    for (const auto& combined_tags : *tag_map) {
        if (combined_tags.second->author() != xAOD::Muon::MuidCo) {
            ATH_MSG_WARNING("Found a non MuidCo tag.... Please check " << combined_tags.second->toString());
            continue;
        }
        const MuonCombined::CombinedFitTag* cmb_tag = dynamic_cast<const MuonCombined::CombinedFitTag*>(combined_tags.second.get());
        MuidCoCache cache{combined_tags.first, cmb_tag};
        out_cache.tag_map.push_back(std::move(cache));
        out_cache.excluded_trks.insert(combined_tags.first);
        for (const Muon::MuonSegment* muon_seg : cmb_tag->associatedSegments()) {
            out_cache.combined_segs.insert(muon_seg);
            maskHits(muon_seg, out_cache);
        }
    }
    return StatusCode::SUCCESS;
}
StatusCode MuonInDetToMuonSystemExtensionAlg::create(const EventContext& ctx, InDetCandidateCache& out_cache) const {
    //// Loop over the tag candidates
    for (const MuonCombined::InDetCandidate* candidate : *out_cache.input_candidates) {
        if (out_cache.excluded_trks.count(candidate)) {
            ATH_MSG_VERBOSE("Inner detector track " << candidate->toString() << " has already been used by MuidCo");
            continue;
        }
        /// We want these tracks to be extrapolated through the spectrometer reject those no meeting
        /// the basic requirements
        if (candidate->indetTrackParticle().pt() < m_extThreshold || (!m_extendSAF && candidate->isSiliconAssociated()) ||
            (!m_extendBulk && !candidate->isSiliconAssociated())) {
            ATH_MSG_VERBOSE("Inner detector track " << candidate->toString() << " is too soft");
            continue;
        }

        /// Prepare the system extension
        Muon::IMuonSystemExtensionTool::SystemExtensionCache cache;
        cache.candidate = std::make_unique<MuonCombined::InDetCandidate>(candidate->indetTrackParticleLink());
        cache.candidate->setSiliconAssociated(candidate->isSiliconAssociated());
        cache.candidate->setExtension(candidate->getCaloExtension());

        const Muon::MuonSystemExtension* extension = candidate->getExtension();
        /// The candidate already has somehow a system extension --> it's just a matter of copying it
        if (extension) {
            /// The system extension does not own the MuonEntryLayerIntersection()-> it's fine
            std::vector<Muon::MuonSystemExtension::Intersection> intersects = extension->layerIntersections();
            cache.candidate->setExtension(
                std::make_unique<Muon::MuonSystemExtension>(&extension->muonEntryLayerIntersection(), std::move(intersects)));
        } else {
            /// If the id candidate has a matching segment, then we know that this track can be potentially reconstructed by MuTagIMO
            const bool seg_match = !m_restrictExtension || hasMatchingSegment(*candidate, out_cache);

            if (!seg_match && !hasMatchingSeed(*candidate, out_cache)) continue;
            cache.useHitSectors = !seg_match;
            cache.sectorsWithHits = &out_cache.hit_sectors;
            cache.createSystemExtension = true;
            cache.requireSystemExtension = true;

            if (!m_muonSystemExtensionTool->muonSystemExtension(ctx, cache)) {
                ATH_MSG_VERBOSE("Extension failed");
                continue;
            }
        }
        out_cache.outputContainer->push_back(std::move(cache.candidate));
    }
    if (msgLevel(MSG::DEBUG)) {
        std::stringstream sstr;
        for (const MuonCombined::InDetCandidate* extended : *out_cache.outputContainer) {
            sstr << "   * " << extended->toString() << std::endl;
        }
        if (!out_cache.excluded_trks.empty()) {
            sstr << std::endl << " ======== The following tracks were already successfully combined  ======== " << std::endl;
            for (const MuonCombined::InDetCandidate* combined : out_cache.excluded_trks) {
                sstr << "   = " << combined->toString() << std::endl;
            }
        }
        if (out_cache.excluded_trks.size() + out_cache.outputContainer->size() != out_cache.input_candidates->size()) {
            sstr << std::endl << " ++++++++ The following candidates were not extrapolated  ++++++++  " << std::endl;
            const InDetCandidateCollection::const_iterator begin = out_cache.outputContainer->begin();
            const InDetCandidateCollection::const_iterator end = out_cache.outputContainer->end();

            for (const MuonCombined::InDetCandidate* rejected : *out_cache.input_candidates) {
                if (out_cache.excluded_trks.count(rejected) ||
                    std::find_if(begin, end,
                                 [rejected](const MuonCombined::InDetCandidate* extended) { return (*rejected) == (*extended); }) != end)
                    continue;
                sstr << "   + " << rejected->toString() << std::endl;
            }
        }
        ATH_MSG_DEBUG("Extrapolated " << out_cache.outputContainer->size() << " out of " << out_cache.input_candidates->size()
                                      << " tracks through the spectrometer" << std::endl
                                      << std::endl
                                      << sstr.str());
    }
    return StatusCode::SUCCESS;
}

StatusCode MuonInDetToMuonSystemExtensionAlg::findHitSectors(const EventContext& ctx, InDetCandidateCache& output_cache) const {
    SG::ReadHandle<Muon::HoughDataPerSectorVec> readHandle{m_houghDataPerSectorVecKey, ctx};
    if (!readHandle.isValid()) {
        ATH_MSG_FATAL("Failed to retrieve the prep data container " << m_houghDataPerSectorVecKey.fullKey());
        return StatusCode::FAILURE;
    }
    const std::vector<Muon::HoughDataPerSec>& hough_data = readHandle->vec;
    /// Helper function to cound whether all three stations are actually complete
    auto count_finished = [&output_cache]() -> unsigned int {
        unsigned int n{0};
        for (const auto& sector_hits : output_cache.hit_sectors) { n += sector_hits.second.size() >= num_sectors; }
        return n;
    };

    /// The phi-hit vector has a size of 3 representing
    /// the forward-backward and barrel sections
    for (int det_region = 0; det_region < Muon::MuonStationIndex::DetectorRegionIndexMax; ++det_region) {
        const RegionIndex region_index = static_cast<RegionIndex>(det_region);
        for (int layer = 0; layer < Muon::MuonStationIndex::LayerIndexMax; ++layer) {
            const LayerIndex layer_index = static_cast<LayerIndex>(layer);
            const unsigned int hash = Muon::MuonStationIndex::sectorLayerHash(region_index, layer_index);
            /// Hits built into a MuidCo track
            const std::set<Identifier>& masked_hits = output_cache.consumed_hits[hash];

            /// Subtract the hits used by MuidCo from the hough seeds
            auto num_hough_hits = [&masked_hits, this](const std::shared_ptr<MuonHough::MuonLayerHough::Maximum>& hough_maximum) {
                std::map<Identifier, hough_chamber> chamber_counts;
                for (const std::shared_ptr<MuonHough::Hit>& hough_hit : hough_maximum->hits) {
                    const Trk::PrepRawData* prep = prepData(hough_hit);
                    const Identifier chId = prep->identify();
                    if (masked_hits.count(chId)) {
                        ATH_MSG_VERBOSE("Do not reuse hit " << m_idHelperSvc->toString(chId));
                        continue;
                    }
                    ATH_MSG_VERBOSE("Count houg hit " << m_idHelperSvc->toString(chId));
                    // Split the seeds into the chambers
                    hough_chamber& chamber = chamber_counts[m_idHelperSvc->chamberId(chId)];
                    ++chamber.nhits;
                    const Amg::Vector3D glob_pos = prep->detectorElement()->center(chId);
                    chamber.eta += glob_pos.eta();
                    chamber.sectors.insert(m_sector_mapping.getSector(glob_pos.phi()));
                }
                /// Find the chamber with the largest number of hits
                hough_chamber N{};
                for (auto& ch_it : chamber_counts) {
                    if (N.nhits < ch_it.second.nhits) { N = std::move(ch_it.second); }
                }
                /// Average the theta
                N.eta /= std::max(N.nhits, 1);
                return N;
            };

            for (const Muon::HoughDataPerSec& sector_data : hough_data) {
                const Muon::HoughDataPerSec::MaximumVec& eta_hits = sector_data.maxVec[hash];
                /// Loop over the maxima & find the hit sectors
                for (const std::shared_ptr<MuonHough::MuonLayerHough::Maximum>& maxima : eta_hits) {
                    /// Remove all hits from the seed that are built into a segment
                    const hough_chamber effect_hits = num_hough_hits(maxima);
                    /// If the seed has still more than the minimum. Then let's mark the sector as hit
                    if (m_houghMin > effect_hits.nhits) continue;
                    ATH_MSG_VERBOSE("Hough maximum in " << Muon::MuonStationIndex::regionName(region_index) << ", "
                                                        << Muon::MuonStationIndex::layerName(layer_index)
                                                        << ", hits: " << effect_hits.nhits);

                    for (const int sector : effect_hits.sectors) {
                        output_cache.hit_sectors[region_index].insert(sector);
                        /// Cache the eta value additionally
                        output_cache.eta_seeds[sector].push_back(effect_hits.eta);
                    }
                }
                if (count_finished() >= RegionIndex::DetectorRegionIndexMax) {
                    ATH_MSG_VERBOSE("The MS is filled up with Hough seeds. We do not need to search for them any longer");
                    break;
                }
            }
        }
        for (auto& theta_pair : output_cache.eta_seeds) { std::sort(theta_pair.second.begin(), theta_pair.second.end()); }
    }
    output_cache.consumed_hits.clear();
    return StatusCode::SUCCESS;
}
StatusCode MuonInDetToMuonSystemExtensionAlg::findSegments(const EventContext& ctx, InDetCandidateCache& output_cache) const {
    SG::ReadHandle<Trk::SegmentCollection> segmentContainer{m_segmentKey, ctx};
    if (!segmentContainer.isValid()) {
        ATH_MSG_FATAL("Failed to retrieve the Muon segment container " << m_segmentKey.fullKey());
        return StatusCode::FAILURE;
    }
    for (const Trk::Segment* trk_segment : *segmentContainer) {
        // Which in reality is a Muon segment
        const Muon::MuonSegment* muon_segment = dynamic_cast<const Muon::MuonSegment*>(trk_segment);
        if (!muon_segment) {
            ATH_MSG_WARNING("How can it be that a Muon segment is not a muon segment?");
            continue;
        }
        // Check if the segment satisfies the quality criteria
        // if (m_segmentSelector->quality(*muon_segment) < m_segmentQuality) continue;
        /// Check if the segment is part of a MuidCo muon
        if (output_cache.combined_segs.count(muon_segment)) {
            ATH_MSG_VERBOSE("Segment was already used in combined fit");
            continue;
        }
        /// Mask all hits from the Hough seeds
        maskHits(muon_segment, output_cache);
        output_cache.candidate_segments.push_back(muon_segment);
    }
    output_cache.combined_segs.clear();
    return StatusCode::SUCCESS;
}
bool MuonInDetToMuonSystemExtensionAlg::hasMatchingSegment(const MuonCombined::InDetCandidate& id_cand,
                                                           const InDetCandidateCache& cache) const {
    const Trk::TrackParameters* ms_entry = id_cand.getCaloExtension()->muonEntryLayerIntersection();
    const double ThetaID = ms_entry->position().theta();
    std::vector<int> id_sectors;
    m_sector_mapping.getSectors(ms_entry->position().phi(), id_sectors);
    auto sector_match = [&id_sectors, this](const Amg::Vector3D& seg_pos) -> bool {
        std::vector<int> seg_sectors;
        m_sector_mapping.getSectors(seg_pos.phi(), seg_sectors);
        return std::find_if(id_sectors.begin(), id_sectors.end(), [&seg_sectors](const int id_sec) {
                   return std::find(seg_sectors.begin(), seg_sectors.end(), id_sec) != seg_sectors.end();
               }) != id_sectors.end();
    };
    for (const Muon::MuonSegment* itSeg : cache.candidate_segments) {
        const Amg::Vector3D pos = itSeg->globalPosition();
        const double dTheta = pos.theta() - ThetaID;
        const bool theta_match = std::abs(dTheta) < 0.2;
        if (!theta_match) {
            ATH_MSG_VERBOSE("dTheta cut failed");
            continue;
        }
        if (!sector_match(pos)) {
            ATH_MSG_VERBOSE("dPhi cut failed");
            continue;
        }
        return true;
    }
    return false;
}
bool MuonInDetToMuonSystemExtensionAlg::hasMatchingSeed(const MuonCombined::InDetCandidate& id_cand,
                                                        const InDetCandidateCache& cache) const {
    const Trk::TrackParameters* ms_entry = id_cand.getCaloExtension()->muonEntryLayerIntersection();
    const double etaID = ms_entry->position().eta();
    std::vector<int> id_sectors;
    m_sector_mapping.getSectors(ms_entry->position().phi(), id_sectors);
    for (const int& sect : id_sectors) {
        std::map<int, std::vector<double>>::const_iterator theta_itr = cache.eta_seeds.find(sect);
        if (theta_itr == cache.eta_seeds.end()) continue;
        if (std::find_if(theta_itr->second.begin(), theta_itr->second.end(),
                         [&etaID](const double eta_seed) { return std::abs(etaID - eta_seed) < 0.1; }) != theta_itr->second.end())
            return true;
    }
    return false;
}

StatusCode MuonInDetToMuonSystemExtensionAlg::createStaus(const EventContext& ctx, const InDetCandidateCollection* ext_candidates,
                                                          const MuidCoVector& tag_map) const {
    if (m_stauInDetCandKey.empty()) {
        ATH_MSG_VERBOSE("No candidates for stau reconstruction will be written");
        return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("Now find the candidates to be considered for MuGirlStau");
    InDetCandidateCache stau_cache{};
    stau_cache.input_candidates = ext_candidates;

    /// This will copy the existing candidates to the new container without calling the system extension tool
    ATH_CHECK(create(ctx, stau_cache));
    /// For the staus it is quite unclear what is the overlap between MuidCo and MuGirlStau.
    /// So in order to be on the safe side we add the MuidCo candidate tracks as well, but
    /// we also try to avoid the extrapolator like a cat does for a bubble bath. By taking
    /// the perigee parameters of the associated muon segments, we have a much much shorter path
    /// to extrapolate the track to the MuonLayerInterSection surfaces.
    for (const MuidCoCache& idMuidCo : tag_map) {
        Muon::IMuonSystemExtensionTool::SystemExtensionCache cache;
        cache.candidate = std::make_unique<MuonCombined::InDetCandidate>(idMuidCo.id_trk->indetTrackParticleLink());
        cache.candidate->setSiliconAssociated(idMuidCo.id_trk->isSiliconAssociated());
        cache.candidate->setExtension(idMuidCo.id_trk->getCaloExtension());
        cache.useHitSectors = false;
        cache.createSystemExtension = true;
        cache.requireSystemExtension = true;

        if (!m_muonSystemExtensionTool->muonLayerInterSections(ctx, *idMuidCo.cmb_trk, cache)) {
            ATH_MSG_DEBUG("Could not determine the intersections. Although that should be possible");
            continue;
        }
        stau_cache.outputContainer->push_back(std::move(cache.candidate));
    }

    SG::WriteHandle<InDetCandidateCollection> indetCandidateCollection(m_stauInDetCandKey, ctx);
    ATH_CHECK(indetCandidateCollection.record(std::move(stau_cache.outputContainer)));
    return StatusCode::SUCCESS;
}

void MuonInDetToMuonSystemExtensionAlg::maskHits(const Muon::MuonSegment* muon_seg, InDetCandidateCache& output_cache) const {
    const Identifier seg_id = m_edmHelperSvc->chamberId(*muon_seg);
    const LayerIndex layer_index = m_idHelperSvc->layerIndex(seg_id);
    const RegionIndex region_index = m_idHelperSvc->regionIndex(seg_id);
    const unsigned int hash = Muon::MuonStationIndex::sectorLayerHash(region_index, layer_index);
    std::set<Identifier>& id_set = output_cache.consumed_hits[hash];
    for (const Trk::MeasurementBase* meas : muon_seg->containedMeasurements()) {
        const Identifier meas_id = m_edmHelperSvc->getIdentifier(*meas);
        id_set.insert(meas_id);
    }
}
