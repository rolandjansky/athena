/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentTagAlg.h"

#include <MuonSegment/MuonSegment.h>

MuonSegmentTagAlg::MuonSegmentTagAlg(const std::string& name, ISvcLocator* pSvcLocator) : AthReentrantAlgorithm(name, pSvcLocator) {}

StatusCode MuonSegmentTagAlg::initialize() {
    ATH_CHECK(m_muonSegmentTagTool.retrieve());
    ATH_CHECK(m_indetCandidateCollectionName.initialize());
    ATH_CHECK(m_muonSegmentCollectionName.initialize());
    ATH_CHECK(m_tagMap.initialize());

    return StatusCode::SUCCESS;
}

StatusCode MuonSegmentTagAlg::execute(const EventContext& ctx) const {
    SG::ReadHandle<InDetCandidateCollection> indetCandidateCollection(m_indetCandidateCollectionName, ctx);
    if (!indetCandidateCollection.isValid()) {
        ATH_MSG_ERROR("Could not read " << m_indetCandidateCollectionName);
        return StatusCode::FAILURE;
    }
    SG::ReadHandle<Trk::SegmentCollection> segments(m_muonSegmentCollectionName, ctx);
    if (!segments.isValid()) {
        ATH_MSG_FATAL("Unable to retrieve " << m_muonSegmentCollectionName);
        return StatusCode::FAILURE;
    }
    // Convert the segments to a MuonSegment pointers
    std::vector<const Muon::MuonSegment*> segmentVec{};
    segmentVec.reserve(segments->size());
    for (const Trk::Segment* trk_seg : *segments) {
        const Muon::MuonSegment* muon_seg = dynamic_cast<const Muon::MuonSegment*>(trk_seg);
        /// That should never happen but let's be sure
        if (!muon_seg) {
            ATH_MSG_WARNING("Failed to convert the Muon segment");
            continue;
        }
        segmentVec.push_back(muon_seg);
    }
    std::unique_ptr<MuonCombined::InDetCandidateToTagMap> tagMap = std::make_unique<MuonCombined::InDetCandidateToTagMap>();

    m_muonSegmentTagTool->tag(ctx, *indetCandidateCollection, segmentVec, tagMap.get());

    SG::WriteHandle<MuonCombined::InDetCandidateToTagMap> writeHandle(m_tagMap, ctx);
    ATH_CHECK(writeHandle.record(std::move(tagMap)));

    return StatusCode::SUCCESS;
}
