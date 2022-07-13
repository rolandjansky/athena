/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegContainerMergerAlg.h"

#include "MuonTrackMakerUtils/MuonTSOSHelper.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

namespace{
    inline double chi2(const Trk::Segment* seg){
        if(!seg || !seg->fitQuality()) return FLT_MAX;
        const Trk::FitQuality* ql = seg->fitQuality();
        return ql->chiSquared()  / ql->numberDoF();
    }

}

MuonSegContainerMergerAlg::MuonSegContainerMergerAlg(const std::string& name, ISvcLocator* pSvcLocator) :
    AthReentrantAlgorithm(name, pSvcLocator) {}

StatusCode MuonSegContainerMergerAlg::initialize() {
    /// Initialize the data dependencies
    ATH_CHECK(m_muonCandidateKeys.initialize());
    ATH_CHECK(m_tagMaps.initialize());
    if ( !(m_tagMaps.size() + m_muonCandidateKeys.size() ) ){
        ATH_MSG_FATAL("No candidates were given to read the segments from");
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Use the following Muon tags to dump the segments");
    for (const SG::ReadHandleKey<MuonCandidateCollection>& key : m_muonCandidateKeys){
        ATH_MSG_INFO(" *** "<<key.fullKey());
    }
    ATH_MSG_INFO("Use the following combined tags to dump the segments");
    for (SG::ReadHandleKey<MuonCombined::InDetCandidateToTagMap>& key : m_tagMaps){
        ATH_MSG_INFO(" --- "<<key.fullKey());
    }
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
        } else {
            ATH_MSG_VERBOSE("Retrieved "<<candidate_coll.fullKey()<< " with size "<<candidate_coll->size());
        }
        for (const MuonCombined::MuonCandidate* ms_cand : *candidate_coll) {
            const std::vector<const Muon::MuonSegment*>& assoc_segs = ms_cand->getSegments();
            if (assoc_segs.empty()) { ATH_MSG_WARNING("Muon candidate " << ms_cand->toString() << " does not have associated segments."); }
            for (const Muon::MuonSegment* seg : assoc_segs) { persistency_link->persistify(seg, out_container.get()); }
        }
    }
    /// Next proceed with the segment candidates the Combined tags
    std::vector<const MuonCombined::TagBase*> good_tags{};
    for (SG::ReadHandle<MuonCombined::InDetCandidateToTagMap>& tag_map : m_tagMaps.makeHandles(ctx)) {
        if (!tag_map.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve combined tag map "<<tag_map.fullKey());
            return StatusCode::FAILURE;
        } else {
            ATH_MSG_VERBOSE("Retrieved "<<tag_map.fullKey()<< " with size "<<tag_map->size());
        }
        good_tags.reserve(tag_map->size() + good_tags.size());
        for (const auto& tag_pair : *tag_map) {
            good_tags.push_back(tag_pair.second.get());
        }
    }
    std::stable_sort(good_tags.begin(),good_tags.end(),[](const MuonCombined::TagBase* a, const MuonCombined::TagBase* b){
        if (a->isCommissioning() != b->isCommissioning()) return b->isCommissioning();
        /// Sort according to the author
        const int auth_a = MuonCombined::authorRank(a->author());
        const int auth_b = MuonCombined::authorRank(b->author());
        if (auth_a != auth_b) return auth_a < auth_b;
        const Trk::Track* prim_a = a->primaryTrack();
        const Trk::Track* prim_b = b->primaryTrack();
        /// 2 times a combined muon track
        if (prim_a && prim_b) {
            const Trk::Perigee* per_a = prim_a->perigeeParameters();
            const Trk::Perigee* per_b = prim_b->perigeeParameters();
            return per_a->pT() > per_b->pT();        
        }
        /// One of them has a primary track
        if (prim_a || prim_b) return prim_a != nullptr;
        std::vector<const Muon::MuonSegment*> seg_a = a->associatedSegments();
        std::vector<const Muon::MuonSegment*> seg_b = b->associatedSegments();
        const size_t n_segs_a = seg_a.size();
        const size_t n_segs_b = seg_b.size();        
        if (n_segs_a != n_segs_b) return n_segs_a > n_segs_b;
        if (!n_segs_a) return false;
        return chi2(seg_a[0]) < chi2(seg_b[0]);        
    });
    
    for (const MuonCombined::TagBase* cmb_tag : good_tags) {
        std::vector<const Muon::MuonSegment*> assoc_segs = cmb_tag->associatedSegments();
        if (assoc_segs.empty() && cmb_tag->type() != xAOD::Muon::CaloTagged) {
            ATH_MSG_WARNING("Combined candidate " << cmb_tag->toString() << " does not have associated segments");
        }
        for (const Muon::MuonSegment* seg : assoc_segs) { 
            persistency_link->persistify(seg, out_container.get()); 
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

    std::vector<const Trk::Segment*> to_copy{};
    
    /// Retrieve the list of segments to be copied
    for (SG::ReadHandle<Trk::SegmentCollection>& inputSegColl : m_inputSegContainerName.makeHandles(ctx)) {
        if (!inputSegColl.isValid()) {
            ATH_MSG_FATAL("Failed to retrieve segment collection " << inputSegColl.fullKey());
            return StatusCode::FAILURE;
        }
        for (const Trk::Segment* seg : *inputSegColl) {
            /// The segment has already been used
            if (assoc_segs.count(seg)) continue;
            to_copy.emplace_back(seg);
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
        std::vector<const Trk::Segment*> resolved_copies{};
        for (const Trk::Track* res : *resolved_trks) {
            const Trk::Segment* seg = track_seg_map[res];
            /// Should never happen but we never know
            if (!seg) continue;
            resolved_copies.emplace_back(seg);
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