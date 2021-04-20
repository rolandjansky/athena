/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONCANDIDATETRACKBUILDERTOOL
#define MUON_IMUONCANDIDATETRACKBUILDERTOOL

#include <vector>

#include "GaudiKernel/IAlgTool.h"
#include "MuonLayerEvent/MuonCandidate.h"
#include "MuonLayerEvent/MuonLayerRecoData.h"

namespace Trk {
    class Track;
}

namespace Muon {

    /** Interface for a tool to build tracks out of a MuonCandidate  */
    class IMuonCandidateTrackBuilderTool : virtual public IAlgTool {
    public:
        /** IAlgTool interface */
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IMuonCandidateTrackBuilderTool("Muon::IMuonCandidateTrackBuilderTool", 1, 0);
            return IID_IMuonCandidateTrackBuilderTool;
        }
        /** @brief build a track out of a MuonCandidate */
        virtual Trk::Track* buildCombinedTrack(const Trk::Track& idTrack, const MuonCandidate& candidate) const = 0;
    };

}  // namespace Muon

#endif  // IMuonCandidateTrackBuilderTool_H
