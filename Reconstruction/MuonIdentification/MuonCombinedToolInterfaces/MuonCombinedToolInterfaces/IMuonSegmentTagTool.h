/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMuonSegmentTagTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IRECMUONSEGMENTTAGTOOL_H
#define IRECMUONSEGMENTTAGTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "MuonCombinedEvent/InDetCandidateCollection.h"
#include "MuonCombinedEvent/InDetCandidateToTagMap.h"
#include "xAODMuon/MuonSegmentContainer.h"

namespace MuonCombined {

    /** @class IMuonSegmentTagTool
        @brief interface for tools building muons from ID and MuonSegments

        @author Niels van Eldik
     */

    class IMuonSegmentTagTool : virtual public IAlgTool {
    public:
        virtual ~IMuonSegmentTagTool() = default;
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IMuonSegmentTagTool("MuonCombined::IMuonSegmentTagTool", 1, 0);
            return IID_IMuonSegmentTagTool;
        }

        virtual void tag(const EventContext& ctx, const InDetCandidateCollection& inDetCandidates,
                         const std::vector<const Muon::MuonSegment*>& segments, InDetCandidateToTagMap* tagMap) const = 0;
    };

}  // namespace MuonCombined

#endif
