/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef IMUONMUONCURVEDSEGMENTCOMBINER_H
#define IMUONMUONCURVEDSEGMENTCOMBINER_H

#include "GaudiKernel/IAlgTool.h"
#include "MuonEDM_AssociationObjects/MuonSegmentCombPatternCombAssociationMap.h"
#include "MuonSegment/MuonSegmentCombinationCollection.h"

namespace Muon {

    class IMuonCurvedSegmentCombiner : virtual public IAlgTool {
    public:
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IMuonCurvedSegmentCombiner("Muon::IMuonCurvedSegmentCombiner", 1, 0);
            return IID_IMuonCurvedSegmentCombiner;
        }

        virtual std::unique_ptr<MuonSegmentCombinationCollection> combineSegments(
            const MuonSegmentCombinationCollection& mdtCombiColl, const MuonSegmentCombinationCollection& csc4DCombiColl,
            const MuonSegmentCombinationCollection& csc2DCombiColl, MuonSegmentCombPatternCombAssociationMap* segPattMap) const = 0;
        virtual ~IMuonCurvedSegmentCombiner() = default;
    };

}  // namespace Muon

#endif
