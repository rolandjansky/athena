/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDTOOLS_MUONTRACKTAGTESTTOOL_H
#define MUONCOMBINEDTOOLS_MUONTRACKTAGTESTTOOL_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonCombinedToolInterfaces/IMuonTrackTagTool.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkParameters/TrackParameters.h"

namespace MuonCombined {

    class MuonTrackTagTestTool : public AthAlgTool, virtual public IMuonTrackTagTool {
    public:
        MuonTrackTagTestTool(const std::string& type, const std::string& name, const IInterface* parent);
        ~MuonTrackTagTestTool() = default;

        StatusCode initialize() override;

        double chi2(const Trk::TrackParameters& idParsAtEntry, const Trk::TrackParameters& msParsAtEntry) const override;
        double chi2(const Trk::Track& id, const Trk::Track& ms, const EventContext& ctx) const override;

    private:
        ToolHandle<Trk::IExtrapolator> m_extrapolator{
            this,
            "ExtrapolatorTool",
            "Trk::Extrapolator/AtlasExtrapolator",
        };
        SG::ReadCondHandleKey<Trk::TrackingGeometry> m_trackingGeometryReadKey{this, "TrackingGeometryReadKey", "",
                                                                               "Key of input TrackingGeometry"};

        // end LEGACY_TRKGEOM

        double m_chi2cut;
#ifdef MUONCOMBDEBUG
        bool m_truth;
#endif
        inline const Trk::TrackingVolume* getVolume(const std::string&& vol_name, const EventContext& ctx) const {
            /// Tracking geometry is provided by the TrackingGeometryAlg
            SG::ReadCondHandle<Trk::TrackingGeometry> handle(m_trackingGeometryReadKey, ctx);
            if (!handle.isValid()) {
                ATH_MSG_WARNING("Could not retrieve a valid tracking geometry");
                return nullptr;
            }
            return handle.cptr()->trackingVolume(vol_name);
            
        }
    };

}  // namespace MuonCombined

#endif
