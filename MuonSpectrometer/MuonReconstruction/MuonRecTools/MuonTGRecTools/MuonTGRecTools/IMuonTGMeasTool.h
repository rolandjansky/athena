/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGRECTOOLS_IMUONTGMEASTOOL_H
#define MUONTGRECTOOLS_IMUONTGMEASTOOL_H

#include "EventPrimitives/EventPrimitives.h"
#include "GaudiKernel/IAlgTool.h"
#include "MuonTGRecTools/MuonTGHits.h"
#include "MuonTGRecTools/MuonTGSegments.h"
#include "TrkGeometry/TrackingGeometry.h"

namespace Muon {

    /** @class IMuonTGMeasTool

        contain application tools for (muon) tracking geometry

        @author Sarka.Todorova@cern.ch
        */

    class IMuonTGMeasTool : virtual public IAlgTool {
    public:
        /** Virtual destructor */
        virtual ~IMuonTGMeasTool() = default;

        /** AlgTool interface method */
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IMuonTGMeasTool("IMuonTGMeasTool", 1, 0);
            return IID_IMuonTGMeasTool;
        };

        virtual const Trk::TrackParameters* layerToDetEl(const Trk::Layer*, const Trk::TrackParameters*, Identifier) const = 0;
        virtual const Trk::TrackParameters* detElToLayer(const Trk::Layer*, const Trk::TrackParameters*, Identifier) const = 0;
        virtual const Trk::RIO_OnTrack* measToLayer(const Trk::Layer*, const Trk::TrackParameters*, const Trk::RIO_OnTrack*) const = 0;
        virtual double residual(const Trk::Layer*, const Trk::TrackParameters*, const Trk::RIO_OnTrack*) const = 0;
        virtual double residual(const Trk::Layer*, const Trk::TrackParameters*, Identifier) const = 0;
        virtual double residual(const Trk::TrackParameters*, const Trk::RIO_OnTrack*) const = 0;
        virtual double residual(const Trk::TrackParameters*, Identifier&) const = 0;
        virtual const Identifier nearestDetEl(const Trk::Layer*, const Trk::TrackParameters*, bool measPhi, double& pitch) const = 0;
        virtual const Trk::Layer* associatedLayer(Identifier id, Amg::Vector3D& gp) const = 0;
        virtual const Trk::Layer* associatedLayer(Identifier id, const Trk::TrackingVolume* vol) const = 0;
        virtual const Trk::Layer* match(Identifier id, const Trk::Layer* lay) const = 0;
    };

}  // namespace Muon

#endif  // MUONTGRECTOOLS_IMUONTGMEASTOOL_H
