/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONCOMBITRACKMAKER_H
#define MUON_IMUONCOMBITRACKMAKER_H

#include <vector>

#include "GaudiKernel/IAlgTool.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "TrkTrack/TrackCollection.h"

static const InterfaceID IID_IMuonCombiTrackMaker("Muon::IMuonCombiTrackMaker", 1, 0);

namespace Trk {
    class Track;
}

namespace Muon {

    /** @brief The IMuonSegmentMaker is a pure virtual interface for tools to find tracks starting from MuonSegmentCombinations  */
    class IMuonCombiTrackMaker : virtual public IAlgTool {
    public:
        /** access to tool interface */
        static const InterfaceID& interfaceID();

        /** @brief find tracks starting from a MuonSegmentCombination
            @param combi a reference to a MuonSegmentCombination
            @return a pointer to a vector of tracks, the ownership of the tracks is passed to the client calling the tool.
        */
        virtual std::vector<Trk::Track*>* find(const MuonSegmentCombination& combi) const = 0;

        /** @brief find tracks starting from a MuonSegmentCombinationCollection
            @param combiCol a reference to a MuonSegmentCombinationCollection
            @return a pointer to a vector of tracks, the ownership of the tracks is passed to the client calling the tool.
        */
        virtual TrackCollection* find(const MuonSegmentCombinationCollection& combiCol) const = 0;
    };

    inline const InterfaceID& IMuonCombiTrackMaker::interfaceID() { return IID_IMuonCombiTrackMaker; }

}  // namespace Muon

#endif
