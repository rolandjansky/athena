/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegContainerMergerAlg.h"

#include "MuonTrackMakerUtils/MuonTSOSHelper.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

MuonSegContainerMergerAlg::MuonSegContainerMergerAlg(const std::string& name, ISvcLocator* pSvcLocator) :
    AthReentrantAlgorithm(name, pSvcLocator) {}

StatusCode MuonSegContainerMergerAlg::initialize() {
    /// Initialize the data dependencies
    ATH_CHECK(m_muonCandidateKeys.initialize());
    ATH_CHECK(m_tagMaps.initialize());
    ATH_CHECK(m_segTrkContainerName.initialize());
    ATH_CHECK(m_assocMapKey.initialize());
    ATH_CHECK(m_inputSegContainerName.initialize(m_saveUnassocSegs));
    ATH_CHECK(m_unassocTrkContainerName.initialize(m_saveUnassocSegs));
    if (m_solveAmbi) ATH_CHECK(m_ambiguityProcessor.retrieve());

    return StatusCode::SUCCESS;
}
StatusCode MuonSegContainerMergerAlg::execute(const EventContext& ctx) const {
    /// Create the output segment containers
    std::unique_ptr<MuonCombined::MuonTagToSegMap> persistency_link = std::make_unique<MuonCombined::MuonTagToSegMap>();
    std::unique_ptr<Trk::SegmentCollection> out_container = std::make_unique<Trk::SegmentCollection>();

    /// Copy over all segments stemming from the Muon Candidates
    for (SG::ReadHandle<MuonCandidateCollection>& candidate_coll : m_muonCandidateKeys.makeHandles(ctx)) {
        if (!candidate_coll.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve Muon segment candidates " << candidate_coll.fullKey());
            return StatusCode::FAILURE;
        }
        for (const MuonCombined::MuonCandidate* ms_cand : *candidate_coll) {
            const std::vector<const Muon::MuonSegment*>& assoc_segs = ms_cand->getSegments();
            if (assoc_segs.empty()) { ATH_MSG_WARNING("Muon candidate " << ms_cand->toString() << " does not have associated segments."); }
            for (const Muon::MuonSegment* seg : assoc_segs) { persistency_link->persistify(seg, out_container.get()); }
        }
    }
    /// Next proceed with the segment candidates the Combined tags
    for (SG::ReadHandle<MuonCombined::InDetCandidateToTagMap>& tag_map : m_tagMaps.makeHandles(ctx)) {
        if (!tag_map.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve combined tag map "<<tag_map.fullKey());
            return StatusCode::FAILURE;
        }
        for (const auto& tag_pair : *tag_map) {
            std::vector<const Muon::MuonSegment*> assoc_segs = tag_pair.second->associatedSegments();
            if (assoc_segs.empty() && tag_pair.second->type() != xAOD::Muon::CaloTagged) {
                ATH_MSG_WARNING("Combined candidate " << tag_pair.second->toString() << " does not have associated segments");
            }
            for (const Muon::MuonSegment* seg : assoc_segs) { persistency_link->persistify(seg, out_container.get()); }
        }
    }
    /// Retrieve the list of associated segments
    std::set<const Trk::Segment*> assoc_segs = persistency_link->getPersistifiedSegments();
    /// Write the segment container & the map to the store gate
    SG::WriteHandle<Trk::SegmentCollection> usedSegWriteHandle{m_segTrkContainerName, ctx};
    ATH_CHECK(usedSegWriteHandle.record(std::move(out_container)));
    SG::WriteHandle<MuonCombined::MuonTagToSegMap> assocMapWriteHandle{m_assocMapKey, ctx};
    ATH_CHECK(assocMapWriteHandle.record(std::move(persistency_link)));
    /// If the remainder segment container is not needed we can quit here
    if (!m_saveUnassocSegs) return StatusCode::SUCCESS;

    std::set<const Trk::Segment*> to_copy{};

    /// Retrieve the list of segments to be copied
    for (SG::ReadHandle<Trk::SegmentCollection>& inputSegColl : m_inputSegContainerName.makeHandles(ctx)) {
        if (!inputSegColl.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve segment collection " << inputSegColl.fullKey());
            return StatusCode::FAILURE;
        }
        for (const Trk::Segment* seg : *inputSegColl) {
            /// The segment has already been used
            if (assoc_segs.count(seg)) continue;
            to_copy.insert(seg);
        }
    }
    /// Solve ambiguities between the segments to keep the container size small
    if (m_solveAmbi && to_copy.size() > 1) {
        std::map<const Trk::Track*, const Trk::Segment*> track_seg_map{};
        TrackCollection ambi_tracks{};
        /// Convert the segments to tracks
        for (const Trk::Segment* seg : to_copy) {
            Trk::TrackStates tsos{};
            Trk::TrackInfo dummy_info{};
            for (const Trk::MeasurementBase* meas : seg->containedMeasurements()) {
                tsos.push_back(Muon::MuonTSOSHelper::createMeasTSOS(meas->uniqueClone(), nullptr, Trk::TrackStateOnSurface::Measurement));
            }
            std::unique_ptr<Trk::Track> trk =
                std::make_unique<Trk::Track>(dummy_info, std::move(tsos), seg->fitQuality() ? seg->fitQuality()->clone() : nullptr);
            track_seg_map[trk.get()] = seg;
            ambi_tracks.push_back(std::move(trk));
        }
        std::unique_ptr<const TrackCollection> resolved_trks{m_ambiguityProcessor->process(&ambi_tracks)};
        std::set<const Trk::Segment*> resolved_copies{};
        for (const Trk::Track* res : *resolved_trks) {
            const Trk::Segment* seg = track_seg_map[res];
            /// Should never happen but we never know
            if (!seg) continue;
            resolved_copies.insert(seg);
        }
        ATH_MSG_DEBUG("Number of segments before the ambiguity solving " << to_copy.size() << " vs. after solving "
                                                                         << resolved_copies.size());
        to_copy = std::move(resolved_copies);
    }
    out_container = std::make_unique<Trk::SegmentCollection>();
    for (const Trk::Segment* seg : to_copy) {
        /// Dynamic cast to the MuonSegment pointer
        const Muon::MuonSegment* muon_seg = dynamic_cast<const Muon::MuonSegment*>(seg);
        if (!muon_seg) continue;
        std::unique_ptr<Muon::MuonSegment> copy = std::make_unique<Muon::MuonSegment>(*muon_seg);
        out_container->push_back(std::move(copy));
    }
    SG::WriteHandle<Trk::SegmentCollection> unUsedSegWriteHandle{m_unassocTrkContainerName, ctx};
    ATH_CHECK(unUsedSegWriteHandle.record(std::move(out_container)));
    return StatusCode::SUCCESS;
}