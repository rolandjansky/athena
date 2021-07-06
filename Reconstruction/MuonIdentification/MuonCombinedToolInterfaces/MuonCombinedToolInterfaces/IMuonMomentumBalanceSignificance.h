/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IMuonMomentumBalanceSignificance.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef IMUONMOMENTUMBALANCESIGNIFICANCE_H
#define IMUONMOMENTUMBALANCESIGNIFICANCE_H

#include "GaudiKernel/IAlgTool.h"
#include "xAODMuon/Muon.h"

namespace Trk {
    class Track;
}

namespace Rec {

    static const InterfaceID IID_IMuonMomentumBalanceSignificance("Rec::IMuonScatteringAngleSignificance", 1, 0);

    /** @class IMuonMomentumBalanceSignificance
        @brief interface providing a significance value for the balance between
               the separate ID and MS momentum measurements.

        @author Alan Poppleton, Wolfgang Liebig
     */

    class IMuonMomentumBalanceSignificance : virtual public IAlgTool {
    public:
        static const InterfaceID& interfaceID();

        /** @brief return significance of momentum (im-)balance, starting from the full muon object
         */
        virtual double momentumBalanceSignificance(const xAOD::Muon& muon) const = 0;

        /** @brief return significance of momentum (im-)balance, starting from the track object
                   (which needs to be a full ID-MS track)
          */
        virtual double momentumBalanceSignificance(const Trk::Track& track) const = 0;
    };

    inline const InterfaceID& Rec::IMuonMomentumBalanceSignificance::interfaceID() { return IID_IMuonMomentumBalanceSignificance; }

}  // namespace Rec

#endif
