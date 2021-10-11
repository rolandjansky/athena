/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// IMuidMuonRecovery
//  tool interface to recover spectrometer tracks (meaning MS hit reallocation)
//  for likely spectrometer-indet matches which give combined fit problems.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef MUIDINTERFACES_IMUIDMUONRECOVERY_H
#define MUIDINTERFACES_IMUIDMUONRECOVERY_H

#include "GaudiKernel/IAlgTool.h"

namespace Trk {
    class Track;
}

namespace Rec {

    /** Interface ID for IMuidMuonRecovery*/

    /**@class IMuidMuonRecovery

    Base class for MuidMuonRecovery AlgTool


    @author niels.van.eldik@cern.ch
    */
    class IMuidMuonRecovery : virtual public IAlgTool {
    public:
        /**Virtual destructor*/
        virtual ~IMuidMuonRecovery() = default;

        /** AlgTool and IAlgTool interface methods */
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IMuidMuonRecovery("IMuidMuonRecovery", 1, 0);
            return IID_IMuidMuonRecovery;
        }

        /**IMuidMuonRecovery interface:
           algorithmic code for recovering muon spectrometer using the inner detector track */
        virtual std::unique_ptr<Trk::Track> recoverableMatch(const Trk::Track& indetTrack, const Trk::Track& spectrometerTrack,
                                                             const EventContext& ctx) const = 0;
    };

}  // namespace Rec

#endif  // MUIDINTERFACES_IMUIDMUONRECOVERY_H_H
