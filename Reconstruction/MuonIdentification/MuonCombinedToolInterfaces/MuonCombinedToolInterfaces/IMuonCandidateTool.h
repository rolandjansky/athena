/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef MUONCOMBINED_IMUONCANDIDATETOOL_H
#define MUONCOMBINED_IMUONCANDIDATETOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "MuonCombinedEvent/MuonCandidateCollection.h"
#include "TrkTrack/TrackCollection.h"
#include "xAODTracking/TrackParticleContainer.h"

namespace MuonCombined {

    /** @class IMuonCandidateTool
        @brief interface for tools building MuonCandidateCollection from a TrackParticleCollection of spectrometer tracks

        @author Niels van Eldik
     */

    class IMuonCandidateTool : virtual public IAlgTool {
    public:
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IMuonCandidateTool("MuonCombined::IMuonCandidateTool", 1, 0);
            return IID_IMuonCandidateTool;
        }

        /**IMuonCandidateTool interface: build a MuonCandidateCollection from a TrackCollection of spectrometer tracks */
        virtual void create(const xAOD::TrackParticleContainer& tracks, MuonCandidateCollection& outputCollection,
                            TrackCollection& outputTracks, const EventContext& ctx) const = 0;

       
        virtual ~IMuonCandidateTool() = default;
    };

}  // namespace MuonCombined

#endif
