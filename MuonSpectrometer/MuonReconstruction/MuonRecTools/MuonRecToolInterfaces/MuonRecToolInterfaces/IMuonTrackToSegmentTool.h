/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONTRACKTOSEGMENTTOO_H
#define MUON_IMUONTRACKTOSEGMENTTOO_H

#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IMuonTrackToSegmentTool("Muon::IMuonTrackToSegmentTool", 1, 0);

namespace Trk {
    class Track;
}

namespace Muon {
    class MuonSegment;
}

namespace Muon {

    /** @brief The IMuonSegmentMaker is a pure virtual interface for tools to find tracks starting from MuonSegmentCombinations  */
    class IMuonTrackToSegmentTool : virtual public IAlgTool {
    public:
        /** access to tool interface */
        static const InterfaceID& interfaceID();

        /** @brief convert track to segment */
        virtual MuonSegment* convert(const EventContext& ctx, const Trk::Track& track) const = 0;
    };

    inline const InterfaceID& IMuonTrackToSegmentTool::interfaceID() { return IID_IMuonTrackToSegmentTool; }

}  // namespace Muon

#endif
