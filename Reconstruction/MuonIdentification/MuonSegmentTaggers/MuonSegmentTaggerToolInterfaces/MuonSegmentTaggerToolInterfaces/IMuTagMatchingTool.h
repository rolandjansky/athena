/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMuTagMatchingTool_H
#define IMuTagMatchingTool_H

#include "GaudiKernel/IAlgTool.h"
#include "MuonCombinedEvent/MuonSegmentInfo.h"
#include "MuonCombinedEvent/MuonSegmentTagSurfaces.h"

#include "MuonSegmentMakerToolInterfaces/IMuonSegmentHitSummaryTool.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"
/**
     @class IMuTagMatchingTool
  */

namespace Muon {
    class MuonSegment;
}

class IMuTagMatchingTool : virtual public IAlgTool {
public:
    virtual ~IMuTagMatchingTool() = default;
    static const InterfaceID& interfaceID() {
        static const InterfaceID IID_IMuTagMatchingTool("IMuTagMatchingTool", 1, 0);
        return IID_IMuTagMatchingTool;
    }

    /// Enum encoding the StationNames (BI, BO, EIA, etc.)
    using SurfDef = MuonCombined::MuonSegmentTagSurfaces::SurfDef;

    virtual bool match(const Trk::TrackParameters& atSurface, const Muon::MuonSegment& segment, int surfaceName) const = 0;

    virtual bool surfaceMatch(const Muon::MuonSegment& segment, int surfaceName) const = 0;

    virtual bool phiMatch(const Trk::TrackParameters& atSurface, const Muon::MuonSegment& segment) const = 0;

    virtual bool thetaMatch(const Trk::TrackParameters& atSurface, const Muon::MuonSegment& segment) const = 0;

    virtual bool rMatch(const Trk::TrackParameters& atSurface, const Muon::MuonSegment& segment) const = 0;

    /** Get extrapolation at MS entrance level*/
    virtual std::unique_ptr<Trk::TrackParameters> ExtrapolateTrktoMSEntrance(const EventContext& ctx, const Trk::Track& pTrack,
                                                                                   Trk::PropDirection direction) const = 0;

    /** Get extrapolation at MSSurface level*/
    virtual std::unique_ptr<Trk::TrackParameters> ExtrapolateTrktoMSSurface(const EventContext& ctx, const Trk::Surface& surface,
                                                                                  const Trk::TrackParameters& pTrack,
                                                                                  Trk::PropDirection direction) const = 0;

    /** Get extrapolation at Segment Plane Surface level*/
    virtual std::shared_ptr<Trk::AtaPlane> ExtrapolateTrktoSegmentSurface(const EventContext& ctx, const Muon::MuonSegment& segment,
                                                                                const Trk::TrackParameters& pTrack,
                                                                                Trk::PropDirection direction) const = 0;

    virtual bool matchSegmentPosition(const MuonCombined::MuonSegmentInfo& info, bool idHasEtaHits) const = 0;

    virtual bool matchSegmentDirection(const MuonCombined::MuonSegmentInfo& info, bool idHasEtaHits) const = 0;

    virtual bool matchPtDependentPull(const MuonCombined::MuonSegmentInfo& info, const Trk::Track& trk) const = 0;

    virtual bool matchDistance(const MuonCombined::MuonSegmentInfo& info) const = 0;

    virtual bool matchCombinedPull(const MuonCombined::MuonSegmentInfo& info) const = 0;

    virtual MuonCombined::MuonSegmentInfo muTagSegmentInfo(const EventContext& ctx, const Trk::Track* track, const Muon::MuonSegment& segment,
                                                           std::shared_ptr<const Trk::AtaPlane> exTrack) const = 0;
};

#endif  // IMuTagMatchingTool_H
