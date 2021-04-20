/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONSEGMENTFITTINGTOOL_H
#define MUON_IMUONSEGMENTFITTINGTOOL_H

#include <vector>

#include "EventPrimitives/EventPrimitives.h"
#include "GaudiKernel/IAlgTool.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {
    class PlaneSurface;
    class LocalDirection;
    class MeasurementBase;
    class Track;
}  // namespace Trk

namespace Muon {
    class MuonSegment;
}

static const InterfaceID IID_IMuonSegmentFittingTool("Muon::IMuonSegmentFittingTool", 1, 0);

namespace Muon {

    /** @brief The IMuonSegmentMaker is a pure virtual interface for tools to find tracks starting from MuonSegmentCombinations  */
    class IMuonSegmentFittingTool : virtual public IAlgTool {
    public:
        /** access to tool interface */
        static const InterfaceID& interfaceID();

        /** @brief fit segment parameters + hits producing a track. The caller should ensure the track gets deleted. */
        virtual Trk::Track* fit(const Amg::Vector3D& gpos, const Amg::Vector3D& gdir, const Trk::PlaneSurface& surf,
                                const std::vector<const Trk::MeasurementBase*>& rioVec) const = 0;

        /** fit segment parameters + hits producing a track. The caller should ensure the track gets deleted. */
        virtual Trk::Track* fit(const MuonSegment& segment) const = 0;

        /** update the parameters of the segment using the track information */
        virtual void updateSegmentParameters(const Trk::Track& track, const Trk::PlaneSurface& surf, Amg::Vector2D& segLocPos,
                                             Trk::LocalDirection& segLocDir, Amg::MatrixX& locerr) const = 0;
    };

    inline const InterfaceID& IMuonSegmentFittingTool::interfaceID() { return IID_IMuonSegmentFittingTool; }

}  // namespace Muon

#endif
