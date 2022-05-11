/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include <MuonCombinedEvent/MuonTagToSegMap.h>

namespace MuonCombined {

    unsigned int MuonTagToSegMap::persistify(const Muon::MuonSegment* assoc_seg, Trk::SegmentCollection* container) {
        unsigned int curr_size = container->size();
        unsigned int test_stored = linkIndex(assoc_seg);
        if (test_stored < curr_size) return test_stored;
        std::unique_ptr<Muon::MuonSegment> copy = std::make_unique<Muon::MuonSegment>(*assoc_seg);
        m_map.emplace_back(assoc_seg, copy.get(), curr_size);
        container->push_back(std::move(copy));
        return curr_size;
    }

    unsigned int MuonTagToSegMap::linkIndex(const Trk::Segment* storegate) const {
        LinkMap::const_iterator itr =
            std::find_if(m_map.begin(), m_map.end(), [storegate](const SegmentLink& link) { return link.storegate == storegate; });
        if (itr != m_map.end()) return itr->index;
        return -1;
    }

    /// Returns the set of all store gate segments that were persitified
    std::set<const Trk::Segment*> MuonTagToSegMap::getPersistifiedSegments() const {
        std::set<const Trk::Segment*> ret;
        for (const SegmentLink& link : m_map) ret.insert(link.storegate);
        return ret;
    }

}  // namespace MuonCombined
