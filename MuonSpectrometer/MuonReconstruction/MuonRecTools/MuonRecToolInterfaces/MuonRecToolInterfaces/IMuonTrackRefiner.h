/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONTRACKREFINER_H
#define MUON_IMUONTRACKREFINER_H

#include <memory>
#include <vector>

#include "GaudiKernel/IAlgTool.h"

namespace Trk {
    class Track;
    class MeasurementBase;
}  // namespace Trk

namespace Muon {

    class MuPatTrack;
    class MuPatHit;
    class GarbageContainer;

    /** @brief The IMuonTrackRefiner is a pure virtual interface for tools which refine the hit content of a given track

        The following interface is available.
        @code
           MuPatTrack* refine( MuPatTrack& track );
        @endcode

    */
    class IMuonTrackRefiner : virtual public IAlgTool {
    public:
        /** access to tool interface */
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IMuonTrackRefiner("Muon::IMuonTrackRefiner", 1, 0);
            return IID_IMuonTrackRefiner;
        }

        /** @brief interface for tools which refine the hit content of a given track
            @param track input track
            @return new refined track. Pointer could be zero, ownership passed to caller
        */
        virtual void refine(const EventContext& ctx, MuPatTrack& track, GarbageContainer& trash_bin) const = 0;
    };
}  // namespace Muon

#endif  // IMuonTrackRefiner_H
