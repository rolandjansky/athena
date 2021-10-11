/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef IRECMUONCOMBINEDTAGTOOL_H
#define IRECMUONCOMBINEDTAGTOOL_H

#include <vector>

#include "GaudiKernel/IAlgTool.h"
#include "TrkTrack/TrackCollection.h"

namespace MuonCombined {
    class InDetCandidate;
    class MuonCandidate;
    class InDetCandidateToTagMap;

    /** @class IMuonCombinedTagTool
        @brief interface for tools building combined muons from a MuonCandidate and a set of ID tracks

        @author Niels van Eldik
     */

    class IMuonCombinedTagTool : virtual public IAlgTool {
    public:
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IMuonCombinedTagTool("MuonCombined::IMuonCombinedTagTool", 1, 0);
            return IID_IMuonCombinedTagTool;
        }

        /**IMuonCombinedTagTool interface: build combined  muons from a muon and a vector of indet candidates */
        virtual void combine(const MuonCandidate& muonCandidate, const std::vector<const InDetCandidate*>& indetCandidates,
                             InDetCandidateToTagMap& tagMap, TrackCollection* combTracks, TrackCollection* METracks,
                             const EventContext& ctx) const = 0;

        virtual ~IMuonCombinedTagTool() = default;
    };

}  // namespace MuonCombined

#endif
