/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
//   get quality of combined track match
//////////////////////////////////////////////////////////////////

#ifndef MUIDINTERFACES_IMUONMATCHQUALITY_H
#define MUIDINTERFACES_IMUONMATCHQUALITY_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkTrack/Track.h"

namespace Rec {

    /**@class IMuonMatchQuality

    Base class for MuonMatchQuality AlgTool

    @author Alan.Poppleton@cern.ch
    */
    class IMuonMatchQuality : virtual public IAlgTool {
    public:
        /**Virtual destructor*/
        virtual ~IMuonMatchQuality() = default;

        /** AlgTool and IAlgTool interface methods */
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IMuonMatchQuality("IMuonMatchQuality", 1, 0);
            return IID_IMuonMatchQuality;
        }

        /**IMuonMatchQuality interface:
           match chiSquared between two tracks expressed at same inner (IP) surface,
           expected to handle indet with extrapolated spectrometer track or combined with constituent track */
        virtual double innerMatchChi2(const Trk::Track& track1, const Trk::Track& track2, const EventContext& ctx) const = 0;

        /**IMuonMatchQuality interface:
           degrees of freedom for chi2 match at IP */
        virtual int innerMatchDOF(const Trk::Track& track1, const Trk::Track& track2) const = 0;

        /**IMuonMatchQuality interface:
           match probability for chi2 match at IP */
        virtual double innerMatchProbability(const Trk::Track& track1, const Trk::Track& track2, const EventContext& ctx) const = 0;

        /**IMuonMatchQuality interface:
           degrees of freedom, chi2, probability  for chi2 match at IP */
        virtual std::pair<int, std::pair<double, double> > innerMatchAll(const Trk::Track& track1, const Trk::Track& track2,
                                                                         const EventContext& ctx) const = 0;

        /**IMuonMatchQuality interface:
           match chiSquared between two tracks expressed at first muon spectrometer hit,
           extrapolates indet to first hit of spectrometer track */
        virtual double outerMatchChi2(const Trk::Track& track1, const Trk::Track& track2, const EventContext& ctx) const = 0;

        /**IMuonMatchQuality interface:
           degrees of freedom for chi2 match at first MS hit */
        virtual int outerMatchDOF(const Trk::Track& track1, const Trk::Track& track2) const = 0;

        /**IMuonMatchQuality interface:
           match probability for chi2 match at first MS hit */
        virtual double outerMatchProbability(const Trk::Track& track1, const Trk::Track& track2, const EventContext& ctx) const = 0;

        /**IMuonMatchQuality interface:
           check the track perigee parameters are expressed at the same surface */
        virtual bool shareOrigin(const Trk::Track& track1, const Trk::Track& track2) const = 0;

        /**IMuonMatchQuality interface:
           as inner match chiSquared but simplified to just use diagonal errors */
        virtual double simpleChi2(const Trk::Track& track1, const Trk::Track& track2, const EventContext& ctx) const = 0;
    };

}  // namespace Rec

#endif  // MUIDINTERFACES_IMUONMATCHQUALITY_H
