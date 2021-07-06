/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef IRECMUONCOMBINEDTOOL_H
#define IRECMUONCOMBINEDTOOL_H

#include <vector>

#include "GaudiKernel/IAlgTool.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "MuonCombinedEvent/InDetCandidateToTagMap.h"
#include "MuonCombinedEvent/MuonCandidateCollection.h"
#include "TrkTrack/TrackCollection.h"

namespace MuonCombined {

    /** @class IMuonCombinedTool
        @brief interface for tools building combined muons from ID and Muon candidates

        @author Niels van Eldik
     */

    class IMuonCombinedTool : virtual public IAlgTool {
    public:
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IMuonCombinedTool("MuonCombined::IMuonCombinedTool", 1, 0);
            return IID_IMuonCombinedTool;
        }

        /**IMuonCombinedTool interface: build combined muons from ID and MS candidates */
        virtual void combine(const MuonCandidateCollection& muonCandidates, const InDetCandidateCollection& inDetCandidates,
                             std::vector<InDetCandidateToTagMap*> tagMaps, TrackCollection* combTracks, TrackCollection* METracks,
                             const EventContext& ctx) const = 0;

        virtual ~IMuonCombinedTool() = default;
    };

}  // namespace MuonCombined

#endif
