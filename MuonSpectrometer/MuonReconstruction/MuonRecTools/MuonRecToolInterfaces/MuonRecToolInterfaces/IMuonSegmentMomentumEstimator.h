/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONSEGMENTMOMENTUMESTIMATOR_H
#define MUON_IMUONSEGMENTMOMENTUMESTIMATOR_H

#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IMuonSegmentMomentumEstimator("Muon::IMuonSegmentMomentumEstimator", 1, 0);

namespace Muon {

    class MuonSegment;

    class IMuonSegmentMomentumEstimator : virtual public IAlgTool {
    public:
        ///////////////////////////////////////////////////////////////////
        // Standard tool methods
        ///////////////////////////////////////////////////////////////////

        static const InterfaceID& interfaceID();

        /** Calculate Momentum Estimate for 2 segments */
        virtual void fitMomentum2Segments(const EventContext& ctx, const MuonSegment* segment1, const MuonSegment* segment2, double& signedMomentum) const = 0;
        virtual void fitMomentumVectorSegments(const EventContext& ctx, const std::vector<const MuonSegment*>, double& signedMomentum) const = 0;
    };

    inline const InterfaceID& IMuonSegmentMomentumEstimator::interfaceID() { return IID_IMuonSegmentMomentumEstimator; }
}  // namespace Muon

#endif  // IMuonSegmentMomentumEstimator_H
