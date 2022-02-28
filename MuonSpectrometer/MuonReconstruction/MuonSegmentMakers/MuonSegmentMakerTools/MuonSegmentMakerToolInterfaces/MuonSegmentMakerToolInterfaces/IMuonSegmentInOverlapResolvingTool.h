/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONSEGMENTSOVERLAPRESOLVINGTOOL_H
#define MUON_IMUONSEGMENTSOVERLAPRESOLVINGTOOL_H

#include <iostream>
#include <string>
#include <vector>

#include "GaudiKernel/IAlgTool.h"
#include "GeoPrimitives/GeoPrimitives.h"

namespace Trk {
    class MeasurementBase;
}

namespace MuonGM {
    class MdtReadoutElement;
}

namespace Muon {
    class MuonSegment;

    /**
       @brief tool to match segments

    */
    class IMuonSegmentInOverlapResolvingTool : virtual public IAlgTool {
    public:
        typedef std::vector<const Trk::MeasurementBase*> MeasVec;
        typedef MeasVec::iterator MeasIt;
        typedef MeasVec::const_iterator MeasCit;

        struct SegmentPhiMatchResult {
            SegmentPhiMatchResult() = default;
            SegmentPhiMatchResult(Amg::Vector3D segDir1, Amg::Vector3D segDir2, double dYZ) :
                segmentDirection1(segDir1), segmentDirection2(segDir2), deltaYZ(dYZ) {}

            /** new segment direction for first segment with the update phi angle */
            Amg::Vector3D segmentDirection1{-999, -999, -999};

            /** new segment direction for second segment with the update phi angle */
            Amg::Vector3D segmentDirection2{-999, -999, -999};

            /** angular difference in the precision plane of the second segment after the phi angle correction */
            double deltaYZ{-999.};

            std::string toString() const;
        };

        struct SegmentGeometrySummary {
            const MuonGM::MdtReadoutElement* detEl{nullptr};
            bool hasMdt{false};
            double shortestChannelLength{
                -999.,
            };
            Amg::Transform3D globalToSeg{};
            Amg::Transform3D segToGlobal{};
            double roPosInSegFrame{-999.};
            double hvPosInSegFrame{-999.};

            /** check whether the position is inside the range */
            bool inBounds(double locPosX, double cut = 0.) {
                double dist = distanceFromReadout(locPosX);
                if (dist < -cut) return false;
                if (dist > shortestChannelLength + cut) return false;
                return true;
            }

            /** checks wether the position is inside the range, if not returns the position inside the range closest to the input */
            double positionInsideTube(double locPosX) {
                if (inBounds(locPosX)) return locPosX;
                double distFromRO = distanceFromReadout(locPosX);
                if (distFromRO < 0.) return roPosInSegFrame;
                return hvPosInSegFrame;
            }

            /** distance to readout, positive axis towards hv side */
            double distanceFromReadout(double locPosX) {
                double dist = locPosX - roPosInSegFrame;
                // if ro at large value than hv flip sign so that a positive dist is always in direction of the hv
                if (roPosInSegFrame > hvPosInSegFrame) dist *= -1.;
                return dist;
            }

            /** distance to tube center, positive axis towards hv side */
            double distanceFromCenter(double locPosX) {
                double dist = distanceFromReadout(locPosX) - 0.5 * shortestChannelLength;
                return dist;
            }

            std::string toString() const;
        };

        struct SegmentPositionMatchResult {
            SegmentPositionMatchResult() = default;
            SegmentPositionMatchResult(double distMin1, double distMinInTube1, double chLen1, double distMin2, double distMinInTube2,
                                       double chLen2, double res, bool good, Amg::Vector3D pos) :
                positionAlongTube1(distMin1),
                positionInTube1(distMinInTube1),
                channelLength1(chLen1),
                positionAlongTube2(distMin2),
                positionInTube2(distMinInTube2),
                channelLength2(chLen2),
                positionResidual(res),
                goodMatch(good),
                segmentPosition(pos) {}

            double positionAlongTube1{-999.};  // calculated new position along the tube in chamber of input segment
            double positionInTube1{-999.};     // calculated new position in the tube in chamber of input segment
            double channelLength1{-999.};      // the tube length of the shortest tube in chamber of input segment

            double positionAlongTube2{-999.};  // calculated new position along the tube in chamber of second segment
            double positionInTube2{-999.};     // calculated new position in the tube in chamber of second segment
            double channelLength2{-999.};      // the tube length of the shortest tube in chamber of second segment

            double positionResidual{-999.};  // residual of the segment positions in the precision plane of the second segment
            bool goodMatch{false};           // flag to indicate problems during match
            Amg::Vector3D segmentPosition{-999., -999., -999.};  // recalculated position of first segment

            /** check whether the calculated segment position is consistent with the two chambers */
            bool inBounds(double cut) {
                if (fabs(positionAlongTube1 - positionInTube1) > cut) return false;
                if (fabs(positionAlongTube2 - positionInTube2) > cut) return false;
                return true;
            }

            std::string toString() const;
        };

        struct SegmentMatchResult {
            SegmentMatchResult() = default;
            SegmentPhiMatchResult phiResult{};        // result of phi match
            double angularDifferencePhi{-999};        // angular difference of the phi from phi match and the phi from the difference of the
                                                      // recalculate positions
            double averagePhiHitPullSegment1{-999.};  // average pull of the phi hits on the first segment with the recalculated parameters
            double averagePhiHitPullSegment2{-999.};  // average pull of the phi hits on the second segment with the recalculated parameters
            SegmentPositionMatchResult segmentResult1{};  // result of recalculation of position of first segment
            SegmentPositionMatchResult segmentResult2{};  // result of recalculation of position of second segment

            bool goodMatch() const;  // check whether matching was ok

            std::string toString() const;  // dump match result
        };

    public:
        /** @brief access to tool interface */
        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IMuonSegmentInOverlapResolvingTool("Muon::IMuonSegmentInOverlapResolvingTool", 1, 0);
            return IID_IMuonSegmentInOverlapResolvingTool;
        }

        /** @brief performance match and return result */
        virtual SegmentMatchResult matchResult(const EventContext& ctx, const MuonSegment& seg1, const MuonSegment& seg2) const = 0;

        /** @brief calculate the position along a tube direction of the first segment
            that results in the smallest position residual in the precision plane of the second segment given
            the segment direction for segment 1 in global coordinates */
        virtual SegmentPositionMatchResult bestPositionAlongTubeMatch(const MuonSegment& seg1, const MuonSegment& seg2,
                                                                      const Amg::Vector3D& segDir1Min) const = 0;

        /** calculate the angle phi for which the angular residual in the precision plane of
            the second segment is zero */
        virtual SegmentPhiMatchResult bestPhiMatch(const MuonSegment& seg1, const MuonSegment& seg2) const = 0;

        /** recalculate the segment direction give a new angle phi, keeps the angle in the precision plane fixed */
        virtual Amg::Vector3D updateSegmentDirection(const MuonSegment& seg, double phi) const = 0;

        /** calculate geometrical information for a segment */
        virtual SegmentGeometrySummary segmentGeometrySummary(const MuonSegment& seg) const = 0;

        virtual ~IMuonSegmentInOverlapResolvingTool() = default;
    };

    inline std::string IMuonSegmentInOverlapResolvingTool::SegmentGeometrySummary::toString() const {
        std::ostringstream sout;
        sout << " Segment geometry: position ro " << roPosInSegFrame << " hv " << hvPosInSegFrame;
        return sout.str();
    }

    inline std::string IMuonSegmentInOverlapResolvingTool::SegmentPhiMatchResult::toString() const {
        std::ostringstream sout;
        sout << " Matching angle phi " << segmentDirection1.phi() << " deltaYZ " << deltaYZ;
        return sout.str();
    }

    inline std::string IMuonSegmentInOverlapResolvingTool::SegmentPositionMatchResult::toString() const {
        std::ostringstream sout;
        sout << " Segment chamber: ";
        if (goodMatch)
            sout << " position " << std::setw(5) << (int)positionAlongTube1 << " in tube " << std::setw(5) << (int)positionInTube1
                 << std::endl
                 << " Matching chamber: position " << std::setw(5) << (int)positionAlongTube2 << " in tube " << std::setw(5)
                 << (int)positionInTube2 << std::endl
                 << " position residual " << std::setw(5) << (int)positionResidual;
        else
            sout << "match failed";
        return sout.str();
    }

    inline std::string IMuonSegmentInOverlapResolvingTool::SegmentMatchResult::toString() const {
        std::ostringstream sout;
        if (goodMatch()) {
            sout << phiResult.toString() << " dphi " << angularDifferencePhi << " phi Pull1 " << averagePhiHitPullSegment1 << " phi Pull2  "
                 << averagePhiHitPullSegment2 << " phi pos1 first " << segmentResult1.segmentPosition.phi() << " phi pos2 second "
                 << segmentResult2.segmentPosition.phi() << std::endl
                 << " segment1  " << segmentResult1.toString() << std::endl
                 << " segment " << segmentResult2.toString();
        } else {
            if (!segmentResult1.goodMatch)
                sout << " Match of first segment failed";
            else if (!segmentResult2.goodMatch)
                sout << " Match of second segment failed";
            else
                sout << " unknown failure";
        }
        return sout.str();
    }

    inline bool IMuonSegmentInOverlapResolvingTool::SegmentMatchResult::goodMatch() const {
        return segmentResult1.goodMatch && segmentResult2.goodMatch;
    }
}  // namespace Muon

#endif
