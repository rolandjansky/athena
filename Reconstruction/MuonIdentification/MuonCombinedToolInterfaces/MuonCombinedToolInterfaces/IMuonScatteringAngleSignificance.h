/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef IMUONSCATTERINGANGLESIGNIFICANCE_H
#define IMUONSCATTERINGANGLESIGNIFICANCE_H

#include "GaudiKernel/IAlgTool.h"
#include "MuidEvent/ScatteringAngleSignificance.h"
#include "xAODMuon/Muon.h"

namespace Trk {
    class Track;
}

namespace Rec {

    /** @class IMuonScatteringAngleSignificance
        @brief interface providing a the scattering angle significance.

        @author Alan Poppleton, Wolfgang Liebig
     */

    class IMuonScatteringAngleSignificance : virtual public IAlgTool {
    public:
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IMuonScatteringAngleSignificance("Rec::IMuonScatteringAngleSignificance", 1, 0);
            return IID_IMuonScatteringAngleSignificance;
        }

        virtual ~IMuonScatteringAngleSignificance() = default;

        /** @brief return significance of scattering angle pattern for tracks,
            starting from the full muon object.
          */
        virtual ScatteringAngleSignificance scatteringAngleSignificance(const xAOD::Muon& muon) const = 0;

        /** @brief return significance of scattering angle pattern for tracks, starting
             from the track object (track unslimming foreseen where needed).
          */
        virtual ScatteringAngleSignificance scatteringAngleSignificance(const Trk::Track& track) const = 0;
    };

}  // namespace Rec

#endif
