/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDEVENT_MUONTAGTOSEGMAP_H
#define MUONCOMBINEDEVENT_MUONTAGTOSEGMAP_H

#include <AthenaKernel/CLASS_DEF.h>
#include <MuonSegment/MuonSegment.h>
#include <TrkSegment/SegmentCollection.h>

#include <set>

/**
 * @brief The MuonTagToSegMap is an auxillary construct that links the MuonSegments associated with a combined TagMap to a second segment
 * collection that can be dumped to the output root file. The mapping becomes neccessary as different muon algorithms might create their own
 * MuonSegments that are saved in different containers. For the final output a single merged container is required.
 *
 */
namespace MuonCombined {
    class MuonTagToSegMap {
    public:
        MuonTagToSegMap() = default;
        ~MuonTagToSegMap() = default;

        /// Pushes back the segment cached in the store gate back to the new segment container
        /// meant for write-out, if the segment has not already been added to the container.
        /// Returns the position index of the new segment in the container
        unsigned int persistify(const Muon::MuonSegment* assoc_seg, Trk::SegmentCollection* container);

        /// Returns the set of all store gate segments that were persitified
        std::set<const Trk::Segment*> getPersistifiedSegments() const;

        /// Returns the index of the persistent in the output container
        /// In case, that the segment has not been persistified yet, -1 is returned
        unsigned int linkIndex(const Trk::Segment* storegate) const;

    private:
        /// Helper struct to cache the mapping between the segment in the store gate,
        /// that's associated with the TagBase & the one that will be written on disk
        /// The index parameters decodes the position of the persistent segment in its correpsonding
        /// container
        struct SegmentLink {
            SegmentLink() = default;
            SegmentLink(const Muon::MuonSegment* _gate, const Muon::MuonSegment* _pers, unsigned int idx) :
                storegate{_gate}, persistent{_pers}, index{idx} {}

            const Trk::Segment* storegate{nullptr};
            const Trk::Segment* persistent{nullptr};
            unsigned int index{0};
        };

        using LinkMap = std::vector<SegmentLink>;
        LinkMap m_map{};
    };

}  // namespace MuonCombined
CLASS_DEF(MuonCombined::MuonTagToSegMap, 170225894, 1)

#endif