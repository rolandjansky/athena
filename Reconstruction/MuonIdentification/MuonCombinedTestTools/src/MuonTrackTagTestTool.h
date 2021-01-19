/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDTOOLS_MUONTRACKTAGTESTTOOL_H
#define MUONCOMBINEDTOOLS_MUONTRACKTAGTESTTOOL_H

#include "MuonCombinedToolInterfaces/IMuonTrackTagTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkGeometry/TrackingGeometry.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkParameters/TrackParameters.h"

#include <mutex>
#include <string>

namespace Trk {
  class TrackingVolume;
}

// LEGACY_TRKGEOM
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"
#include "TrkGeometry/TrackingGeometry.h"
// end LEGACY_TRKGEOM

namespace MuonCombined {

class MuonTrackTagTestTool : public AthAlgTool, virtual public IMuonTrackTagTool {

  public:
    MuonTrackTagTestTool(const std::string& type, const std::string& name, const IInterface* parent);
    ~MuonTrackTagTestTool()=default;

    StatusCode initialize();

    double chi2(const Trk::TrackParameters& idParsAtEntry, const Trk::TrackParameters& msParsAtEntry) const;
    double chi2(const Trk::Track& id, const Trk::Track& ms) const;

  private:
    ToolHandle<Trk::IExtrapolator> m_extrapolator{this,"ExtrapolatorTool","Trk::Extrapolator/AtlasExtrapolator",};
    SG::ReadCondHandleKey<Trk::TrackingGeometry> m_trackingGeometryReadKey{this, "TrackingGeometryReadKey", "", "Key of input TrackingGeometry"};

    // LEGACY_TRKGEOM
    mutable ServiceHandle<Trk::ITrackingGeometrySvc> m_trackingGeometrySvc ATLAS_THREAD_SAFE {this,"TrackingGeometrySvc","AtlasTrackingGeometrySvc"};  // Services are assumed to be thread-safe
    mutable std::once_flag m_trackingOnceFlag ATLAS_THREAD_SAFE;
    mutable const Trk::TrackingGeometry* m_trackingGeometry ATLAS_THREAD_SAFE;  // Initialized with call_once, then used read-only
    mutable const Trk::TrackingVolume* m_msEntrance ATLAS_THREAD_SAFE;  // Initialized with call_once, then used read-
    // end LEGACY_TRKGEOM

    double m_chi2cut;
#ifdef MUONCOMBDEBUG
    bool m_truth;
#endif
};

}  // namespace MuonCombined


#endif
