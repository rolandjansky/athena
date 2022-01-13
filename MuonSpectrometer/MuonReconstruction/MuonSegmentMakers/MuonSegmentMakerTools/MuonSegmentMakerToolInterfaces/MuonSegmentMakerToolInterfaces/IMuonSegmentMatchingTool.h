/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONSEGMENTMATCHINGTOOL_H
#define MUON_IMUONSEGMENTMATCHINGTOOL_H

#include "GaudiKernel/IAlgTool.h"

namespace Trk {
    class MeasurementBase;
}

namespace MuonGM {
    class MdtReadoutElement;
}

namespace Muon {

    class MuonSegment;

    /**
       @brief tool to match segments

    */
    class IMuonSegmentMatchingTool : virtual public IAlgTool {
    public:
        /** @brief access to tool interface */
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IMuonSegmentMatchingTool("Muon::IMuonSegmentMatchingTool", 1, 0);

            return IID_IMuonSegmentMatchingTool;
        }

        /** @brief match two segments */
        virtual bool match(const EventContext& ctx, const MuonSegment& seg1, const MuonSegment& seg2) const = 0;

        virtual ~IMuonSegmentMatchingTool() = default;
    };

}  // namespace Muon

#endif
