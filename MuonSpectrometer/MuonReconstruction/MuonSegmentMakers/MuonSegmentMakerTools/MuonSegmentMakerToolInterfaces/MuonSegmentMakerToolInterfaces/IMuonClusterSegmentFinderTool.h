/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONROADFINDERTOOL_H
#define MUON_IMUONROADFINDERTOOL_H

#include <vector>

#include "GaudiKernel/IAlgTool.h"
#include "MuonPattern/MuonPatternCombinationCollection.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonSegment/MuonSegment.h"
#include "TrkSegment/SegmentCollection.h"

namespace Trk {
    class Track;
}

namespace Muon {

    class IMuonClusterSegmentFinderTool : virtual public IAlgTool {
    public:
        /** access to tool interface */
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IMuonClusterSegmentFinderTool("Muon::IMuonClusterSegmentFinderTool", 1, 0);
            return IID_IMuonClusterSegmentFinderTool;
        }

        virtual void find(const EventContext& ctx, std::vector<const Muon::MuonClusterOnTrack*>& clusters, std::vector<std::unique_ptr<Muon::MuonSegment>>& segments,
                          Trk::SegmentCollection* segColl) const = 0;

        virtual ~IMuonClusterSegmentFinderTool() = default;
    };

}  // namespace Muon

#endif
