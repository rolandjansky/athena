/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

    static const InterfaceID IID_IMuonSegmentTagTool("MuonCombined::IMuonSegmentTagTool", 1, 0);

    /** @class IMuonSegmentTagTool
        @brief interface for tools building muons from ID and MuonSegments

        @author Niels van Eldik
     */

    class IMuonSegmentTagTool : virtual public IAlgTool {
    public:
        using SegmentMap = std::map<const Muon::MuonSegment*, ElementLink<xAOD::MuonSegmentContainer> >;

        static const InterfaceID& interfaceID();

        /**IMuonSegmentTagTool interface: build muons from ID and MuonSegments */
        virtual void tag(const InDetCandidateCollection& inDetCandidates, const xAOD::MuonSegmentContainer& segments,
                         InDetCandidateToTagMap* tagMap) const = 0;
        virtual void tag(const InDetCandidateCollection& inDetCandidates, const std::vector<const Muon::MuonSegment*>& segments,
                         SegmentMap* segmentToxAODSegmentMap, InDetCandidateToTagMap* tagMap) const = 0;
    };

    inline const InterfaceID& IMuonSegmentTagTool::interfaceID() { return IID_IMuonSegmentTagTool; }

}  // namespace MuonCombined

#endif
