/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONSEGMENTFITTINGTOOL_H
#define MUONSEGMENTFITTINGTOOL_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "MuonRecToolInterfaces/IMuonSegmentFittingTool.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "MuonRecToolInterfaces/IMuonTrackCleaner.h"

namespace Trk {
class PlaneSurface;
class LocalDirection;
class MeasurementBase;
}  // namespace Trk

namespace Muon {
class MuonSegment;
}

namespace Muon {

/**
    @class MuonSegmentFittingTool

    Implementation of an IMuonSegmentFittingTool.

*/
class MuonSegmentFittingTool : virtual public IMuonSegmentFittingTool, public AthAlgTool {
  public:
    MuonSegmentFittingTool(const std::string&, const std::string&, const IInterface*);

    virtual ~MuonSegmentFittingTool() = default;

    virtual StatusCode initialize();

    /** fit segment parameters + hits producing a track */
    Trk::Track* fit(const Amg::Vector3D& gpos, const Amg::Vector3D& gdir, const Trk::PlaneSurface& surf,
                    const std::vector<const Trk::MeasurementBase*>& rioVec) const;

    /** fit segment parameters + hits producing a track */
    Trk::Track* fit(const MuonSegment& segment) const;

    /** update the parameters of the segment using the track information */
    void updateSegmentParameters(const Trk::Track& track, const Trk::PlaneSurface& surf, Amg::Vector2D& segLocPos,
                                 Trk::LocalDirection& segLocDir, Amg::MatrixX& locerr) const;


    Trk::MagneticFieldProperties m_magFieldProperties;  //<! pmagnetic field properties

    ToolHandle<Trk::IPropagator> m_slPropagator{
        this,
        "SLPropagator",
        "Trk::RungeKuttaPropagator/AtlasRungeKuttaPropagator",
    };  //<! propagator, always use straightline
    ToolHandle<Trk::ITrackFitter> m_slTrackFitter{
        this,
        "SLFitter",
        "Trk::GlobalChi2Fitter/MCTBSLFitter",
    };  //<! fitter, always use straightline
    ToolHandle<Trk::ITrackFitter> m_curvedTrackFitter{
        this,
        "CurvedFitter",
        "Trk::GlobalChi2Fitter/MCTBFitter",
    };  //<! fitter, curved tracks
    ToolHandle<Muon::IMuonTrackCleaner> m_trackCleaner{
        this,
        "TrackCleaner",
        "Muon::MuonTrackCleaner/MuonTrackCleaner",
    };

    bool m_updatePrecisionCoordinate;  //<! flag to select update of precision coordinate in fit
};

}  // namespace Muon
#endif
