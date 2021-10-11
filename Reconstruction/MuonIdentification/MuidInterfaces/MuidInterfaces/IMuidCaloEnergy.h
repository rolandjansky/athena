/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// IMuidCaloEnergy
//  tool interface for estimating the energy loss of a muon with its error.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef MUIDINTERFACES_IMUIDCALOENERGY_H
#define MUIDINTERFACES_IMUIDCALOENERGY_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkParameters/TrackParameters.h"

class CaloEnergy;

namespace Trk {
    class TrackStateOnSurface;
}

namespace Rec {

    /**@class IMuidCaloEnergy

    Base class for MuidCaloEnergy AlgTool

    @author Alan.Poppleton@cern.ch
    */
    class IMuidCaloEnergy : virtual public IAlgTool {
    public:
        /**Virtual destructor*/
        virtual ~IMuidCaloEnergy() = default;

        /** AlgTool and IAlgTool interface methods */
        static const InterfaceID& interfaceID() {
            /** Interface ID for IMuidCaloEnergy*/
            static const InterfaceID IID_IMuidCaloEnergy("IMuidCaloEnergy", 1, 0);

            return IID_IMuidCaloEnergy;
        }

        /**IMuidCaloEnergy interface:
           to get the total energyLoss in the calorimeters */
        virtual std::unique_ptr<CaloEnergy> energyLoss(const EventContext& ctx, double trackMomentum, double eta, double phi) const = 0;

        /**IMuidCaloEnergy interface:
           TrackStateOnSurface for parameters and energyLoss at the calorimeter mid-surface */
        virtual std::unique_ptr<Trk::TrackStateOnSurface> trackStateOnSurface(
            const EventContext& ctx, const Trk::TrackParameters& middleParameters, const Trk::TrackParameters* innerParameters = nullptr,
            const Trk::TrackParameters* outerParameters = nullptr) const = 0;
    };

}  // namespace Rec

#endif  // MUIDINTERFACES_IMUIDCALOENERGY_H
