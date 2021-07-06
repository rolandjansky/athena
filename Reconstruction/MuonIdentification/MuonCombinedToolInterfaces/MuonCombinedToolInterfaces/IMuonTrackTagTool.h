/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDTOOLS_IMUONTRACKTAGTOOL_H
#define MUONCOMBINEDTOOLS_IMUONTRACKTAGTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"

namespace MuonCombined {

    class IMuonTrackTagTool : virtual public IAlgTool {
    public:
        virtual ~IMuonTrackTagTool() = default;
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IMuonTrackTagTool("MuonCombined::IMuonTrackTagTool", 1, 0);
            return IID_IMuonTrackTagTool;
        }

        virtual double chi2(const Trk::TrackParameters& idParsAtEntry, const Trk::TrackParameters& msParsAtEntry) const = 0;

        virtual double chi2(const Trk::Track& id, const Trk::Track& ms, const EventContext& ctx) const = 0;
    };

}  // namespace MuonCombined

#endif
