/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONSEGMENTPAIRMATCHINGTOOL_H
#define MUON_IMUONSEGMENTPAIRMATCHINGTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "Identifier/Identifier.h"

namespace Muon {

    class MuonSegment;

    /**
       @brief tool to match segments

    */
    class IMuonSegmentPairMatchingTool : virtual public IAlgTool {
    public:
        struct SegmentMatchResult {
            SegmentMatchResult() = default;
            Identifier chid_a;
            Identifier chid_b;
            int phiSector_a{-99999};
            int phiSector_b{-99999};
            double deltaTheta_a{-99999};
            double deltaTheta_b{-99999};
            double deltaTheta{-99999};
            double angleAC{-99999};
            double angleBC{-99999};
            double angleAB{-99999};
            double deltaPhipos{-99999};
            double deltaPhidir{-99999};
            double phiposerr_a{-99999};
            double phidirerr_a{-99999};
            double phiposerr_b{-99999};
            double phidirerr_b{-99999};
            double shorttube_a{-99999};
            double shorttube_b{-99999};
            bool matchFlag{false};  // false if anything went wrong with the calculation
        };

    public:
        /** @brief access to tool interface */
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IMuonSegmentPairMatchingTool("Muon::IMuonSegmentPairMatchingTool", 1, 0);
            return IID_IMuonSegmentPairMatchingTool;
        }

        /** @brief performance match and return result */
        virtual SegmentMatchResult matchResult(const MuonSegment& seg1, const MuonSegment& seg2) const = 0;

        virtual ~IMuonSegmentPairMatchingTool() = default;
    };

}  // namespace Muon

#endif
