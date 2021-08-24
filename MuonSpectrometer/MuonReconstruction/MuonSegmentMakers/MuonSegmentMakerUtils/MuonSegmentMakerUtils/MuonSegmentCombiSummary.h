/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_MUONSEGMENTCOMBISUMMARY_H
#define MUON_MUONSEGMENTCOMBISUMMARY_H

#include <memory>
#include <set>
#include <vector>

#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "MuonStationIndex/MuonStationIndex.h"

namespace Muon {

    class MuonSegment;

    class MuonSegmentCombiSummary {
    public:
        // vector of segments
        typedef std::vector<MuonSegment*> SegVec;

        // vector of SegVec per chamber, indexed using MuonStationIndex::ChIndex
        typedef std::vector<SegVec> ChSegVec;

    public:
        MuonSegmentCombiSummary(const MuonSegmentCombination& segment);
        MuonSegmentCombiSummary(std::unique_ptr<MuonSegmentCombination> segment);
        MuonSegmentCombiSummary& operator=(MuonSegmentCombiSummary&& other) = default;
        MuonSegmentCombiSummary(MuonSegmentCombiSummary&&) = default;

        /// States whether the Summary has ownership over the contained segment or not
        bool has_ownership() const;
        /// Returns the pointer used to create the segment collection
        const MuonSegmentCombination* get() const;
        /// Releases the ownership over the SegmentCombination if the segment was constructed
        /// with a unique_ptr
        std::unique_ptr<MuonSegmentCombination> release();
        virtual ~MuonSegmentCombiSummary() = default;

        /** access to combi */
        const MuonSegmentCombination& segmentCombination() const;
        // access to all segments
        ChSegVec& segmentsPerLayer();

        // access to segments in a given chamber layer
        SegVec& chamberSegments(unsigned int chIndex);

        // counter for station layers
        std::set<MuonStationIndex::StIndex> stations;         //!< number of station layers
        std::set<MuonStationIndex::StIndex> stationsGood;     //!< number of station layers with good segment
        std::set<MuonStationIndex::StIndex> stationsTrigger;  //!< number of station layers with trigger hit

        unsigned int nsegments{0};

    private:
        ChSegVec m_segmentsPerChamberLayer{MuonStationIndex::ChIndexMax};
        /// This pointer is valid if the summary does hold the
        std::unique_ptr<MuonSegmentCombination> m_owner_combi{};
        const MuonSegmentCombination* m_observer_ptr{nullptr};
    };

}  // namespace Muon

#endif
