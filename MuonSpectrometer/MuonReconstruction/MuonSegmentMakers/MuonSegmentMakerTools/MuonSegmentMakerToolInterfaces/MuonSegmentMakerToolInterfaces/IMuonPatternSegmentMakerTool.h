/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONPATTERNSEGMENTMAKERTOOL_H
#define MUON_IMUONPATTERNSEGMENTMAKERTOOL_H

#include <vector>

#include "GaudiKernel/IAlgTool.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"

namespace Trk {
    class Track;
}

namespace Muon {

    /** @brief The IMuonSegmentMaker is a pure virtual interface for tools to find tracks starting from MuonSegmentCombinations  */
    class IMuonPatternSegmentMakerTool : virtual public IAlgTool {
    public:
        /** access to tool interface */
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IMuonPatternSegmentMakerTool("Muon::IMuonPatternSegmentMakerTool", 1, 0);
            return IID_IMuonPatternSegmentMakerTool;
        }

        /** @brief find tracks starting from a MuonSegmentCombination
            @param combi a reference to a MuonSegmentCombination
            @return a pointer to a vector of tracks, the ownership of the tracks is passed to the client calling the tool.
        */
        virtual MuonSegmentCombination* find(const MuonPatternCombination& pattern) const = 0;

        /** @brief find tracks starting from a MuonSegmentCombinationCollection
            @param combiCol a reference to a MuonSegmentCombinationCollection
            @return a pointer to a vector of tracks, the ownership of the tracks is passed to the client calling the tool.
        */
        virtual MuonSegmentCombinationCollection* find(const MuonPatternCombinationCollection& patterns) const = 0;

        virtual ~IMuonPatternSegmentMakerTool() = default;
    };

}  // namespace Muon

#endif
