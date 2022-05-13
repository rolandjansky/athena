/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IRECIMuonTrackToSegmentAssociationTool_H
#define IRECIMuonTrackToSegmentAssociationTool_H

#include "GaudiKernel/IAlgTool.h"
#include "MuonSegment/MuonSegment.h"
#include "TrkSegment/SegmentCollection.h"
#include "TrkTrack/Track.h"

namespace MuonCombined {

    /** @class IMuonTrackToSegmentAssociationTool
        @brief Interface to associate the Muon segments to the muon track
     */

    class IMuonTrackToSegmentAssociationTool : virtual public IAlgTool {
    public:
        virtual ~IMuonTrackToSegmentAssociationTool() = default;
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IMuonTrackToSegmentAssociationTool("MuonCombined::IMuonTrackToSegmentAssociationTool", 1, 0);
            return IID_IMuonTrackToSegmentAssociationTool;
        }

        /**IMuonSegmentTagTool interface: build muons from ID and MuonSegments */
        virtual bool associatedSegments(const Trk::Track& track, const Trk::SegmentCollection* segments,
                                        std::vector<const Muon::MuonSegment*>& assocSegmentVec) const = 0;
    };

}  // namespace MuonCombined

#endif
