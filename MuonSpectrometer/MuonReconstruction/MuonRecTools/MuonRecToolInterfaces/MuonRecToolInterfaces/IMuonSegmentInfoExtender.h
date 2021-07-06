/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONSEGMENTINFOEXTENDER_H
#define MUON_IMUONSEGMENTINFOEXTENDER_H

#include <vector>

#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IMuonSegmentInfoExtender("Muon::IMuonSegmentInfoExtender", 1, 0);

namespace Trk {
    class Track;
}

namespace Muon {

    class MuPatSegment;

    /** @brief The IMuonSegmentInfoExtender is a pure virtual interface for tools extending muon segments with
        additional information

        The following interface is available.
        @code
          virtual void extendInfo( MuPatSegment * segment ) = 0;
        @endcode

    */
    class IMuonSegmentInfoExtender : virtual public IAlgTool {
    public:
        /** access to tool interface */
        static const InterfaceID& interfaceID();

        /** @brief interface for tools to add information to the muon segment that has already been created
                @param segment the segment whose info you wish to extend
        */
        virtual void extendInfo(MuPatSegment* segment) const = 0;
    };

    inline const InterfaceID& IMuonSegmentInfoExtender::interfaceID() { return IID_IMuonSegmentInfoExtender; }
}  // namespace Muon

#endif  // MUON_IMUONSEGMENTINFOEXTENDER_H
