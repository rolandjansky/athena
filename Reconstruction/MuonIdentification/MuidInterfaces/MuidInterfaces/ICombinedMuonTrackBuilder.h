/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ICombinedMuonTrackBuilder
//  interface to build and fit a combined muon from input track(s)
//  and/or MeasurementSet, gathering material effects along the
//  track (in particular for the calorimeter).
//
///////////////////////////////////////////////////////////////////

#ifndef MUIDINTERFACES_ICOMBINEDMUONTRACKBUILDER_H
#define MUIDINTERFACES_ICOMBINEDMUONTRACKBUILDER_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkFitterUtils/FitterTypes.h"
#include "TrkParameters/TrackParameters.h"

namespace Trk {
    class Track;
    class Vertex;
}  // namespace Trk

namespace Rec {

    /** Interface ID for ICombinedMuonTrackBuilder*/

    /**@class ICombinedMuonTrackBuilder

    Base class for CombinedMuonTrackBuilder AlgTool

    @author Alan.Poppleton@cern.ch
    */
    class ICombinedMuonTrackBuilder : virtual public IAlgTool {
    public:
        /**Virtual destructor*/
        virtual ~ICombinedMuonTrackBuilder() = default;

        /** AlgTool and IAlgTool interface methods */
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_ICombinedMuonTrackBuilder("ICombinedMuonTrackBuilder", 1, 0);
            return IID_ICombinedMuonTrackBuilder;
        }
        /**ICombinedMuonTrackBuilder interface: build and fit combined ID/Calo/MS track */
        virtual std::unique_ptr<Trk::Track> combinedFit(const Trk::Track& indetTrack, const Trk::Track& extrapolatedTrack,
                                                        const Trk::Track& spectrometerTrack, const EventContext& ctx) const = 0;

        /**ICombinedMuonTrackBuilder interface:
           build and fit indet track extended to include MS Measurement set.
           Adds material effects as appropriate plus calo energy-loss treatment */
        virtual std::unique_ptr<Trk::Track> indetExtension(const Trk::Track& indetTrack,
                                                           const Trk::MeasurementSet& spectrometerMeasurements, const EventContext& ctx,
                                                           const Trk::TrackParameters* innerParameters = nullptr,
                                                           const Trk::TrackParameters* middleParameters = nullptr,
                                                           const Trk::TrackParameters* outerParameters = nullptr) const = 0;

        /**ICombinedMuonTrackBuilder interface:
           propagate to perigee adding calo energy-loss and material to MS track */
        virtual std::unique_ptr<Trk::Track> standaloneFit(const Trk::Track& spectrometerTrack, const EventContext& ctx,
                                                          const Trk::Vertex* vertex = nullptr, float bs_x = 0., float bs_y = 0.,
                                                          float bs_z = 0.) const = 0;

        /**ICombinedMuonTrackBuilder interface:
           refit a track removing any indet measurements with optional addition of pseudoMeasurements */
        virtual std::unique_ptr<Trk::Track> standaloneRefit(const Trk::Track& combinedTrack, const EventContext& ctx, float bs_x = 0.,
                                                            float bs_y = 0., float bs_z = 0.) const = 0;

        /*refit a track*/
        virtual std::unique_ptr<Trk::Track> fit(Trk::Track& track, const EventContext& ctx, const Trk::RunOutlierRemoval runOutlier = false,
                                                const Trk::ParticleHypothesis particleHypothesis = Trk::muon) const = 0;
    };

}  // namespace Rec

#endif  // MUIDINTERFACES_ICOMBINEDMUONTRACKBUILDER_H
