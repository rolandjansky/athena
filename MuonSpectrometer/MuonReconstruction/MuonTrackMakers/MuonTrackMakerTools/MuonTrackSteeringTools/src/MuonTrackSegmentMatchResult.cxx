/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 * Interface for matching tracks to muon segments
 * ----------------------------------------------
 * Author       : Martin Woudstra
 * Creation Date: 06 September 2010
 ***************************************************************************/

// std

// other packages

// this package

#include "MuonTrackSegmentMatchResult.h"

namespace Muon {

    TrackSegmentMatchResult::TrackSegmentMatchResult() {
        clear();
        resetCuts();
    }

    TrackSegmentMatchResult::~TrackSegmentMatchResult() { clear(); }

    void TrackSegmentMatchResult::clear() {
        localPosXDiff = 0.0;
        localPosYDiff = 0.0;
        localAngleXDiff = 0.0;
        localAngleYDiff = 0.0;
        posXMeasErr2 = 0.0;
        posXPredErr2 = 0.0;
        posXAlignErr2 = 0.0;
        posXTotalErr2 = 0.0;
        posYMeasErr2 = 0.0;
        posYPredErr2 = 0.0;
        posYAlignErr2 = 0.0;
        posYTotalErr2 = 0.0;
        angleXMeasErr2 = 0.0;
        angleXPredErr2 = 0.0;
        angleXAlignErr2 = 0.0;
        angleXTotalErr2 = 0.0;
        angleYMeasErr2 = 0.0;
        angleYPredErr2 = 0.0;
        angleYAlignErr2 = 0.0;
        angleYTotalErr2 = 0.0;
        matchChiSquared = 0.0;
        trackChamberId = Identifier();
        segmentChamberId = Identifier();
        predictionCovariance.setZero();
        measuredCovariance.setZero();
        totalCovariance.setZero();
        diffVector.setZero();
        track = nullptr;
        segment = nullptr;
        havePosX = false;
        havePosY = false;
        haveAngleX = false;
        haveAngleY = false;
        havePosXError = false;
        havePosYError = false;
        haveAngleXError = false;
        haveAngleYError = false;
        haveMatchChiSquared = false;
        matchOK = true;
        reason = Unknown;
    }

    std::string TrackSegmentMatchResult::cutString(CutType cut) {
        if (static_cast<int>(cut) < 0 || static_cast<int>(cut) >= static_cast<int>(NumberOfCutTypes)) { return "CutTypeOutOfRange"; }

        switch (cut) {
            case PosXCut: return "PosXCut";
            case PosYCut: return "PosYCut";
            case AngXCut: return "AngleXCut";
            case AngYCut: return "AngleYCut";
            case PosXPullCut: return "PosXPullCut";
            case PosYPullCut: return "PosYPullCut";
            case AngXPullCut: return "AngleXPullCut";
            case AngYPullCut: return "AngleYPullCut";
            case MatchChiSquaredCut: return "ChiSquaredMatchCut";
            default: return "CutTypeNotInList";
        }

        return "";
    }

    std::string TrackSegmentMatchResult::reasonString(Reason r) {
        if (static_cast<int>(r) < 0 || static_cast<int>(r) >= static_cast<int>(NumberOfReasons)) { return "ReasonOutOfRange"; }
        switch (r) {
            case Unknown: return "Unknown";
            case NoCutsApplied: return "NoCutsApplied";
            case PassedAllCuts: return "PassedAllCuts";
            case PassedMatchChiSquaredCut: return "PassedMatchChiSquaredCut";
            case PassedPosAngleCuts: return "PassedPosAngleCuts";
            case FailedCuts: return "FailedCuts";
            case SegmentMatching: return "SegmentMatching";
            case NoSegmentPointer: return "NoSegmentPointer";
            case StereoAngleWithoutPhi: return "StereoAngleWithoutPhi";
            case NoClosestPars: return "NoClosestPars";
            case FieldNotOk: return "FieldNotOk";
            case NoClosestSegment: return "NoClosestSegment";
            case SegmentMatch: return "SegmentMatch";
            case NoMomentumWithMagField: return "NoMomentumWithMagField";
            case ExtrapolFailed: return "ExtrapolationFailed";
            case ExtrapolNoErrors: return "ExtrapolationNoErrors";
            case NoMeasErrors: return "NoMeasErrors";
            case AngleMeasErrFailed: return "AngleMeasErrFailed";
            case AnglePredErrFailed: return "AnglePredErrFailed";
            case LocalDirFailed: return "LocalDirFailed";

            default: return "ReasonNotInList";
        }
    }

    /** String with all cuts that were applied and failed */
    std::string TrackSegmentMatchResult::failedCutsString() const {
        std::string failedStr;
        for (int i = 0; i < (int)NumberOfCutTypes; ++i) {
            CutType cut = static_cast<CutType>(i);
            if (appliedCut(cut) && failedCut(cut)) {
                if (!failedStr.empty()) failedStr += ",";
                failedStr += TrackSegmentMatchResult::cutString(cut);
            }
        }
        return failedStr;
    }

    /** String with all cuts that were applied and passed */
    std::string TrackSegmentMatchResult::passedCutsString() const {
        std::string passedStr;
        for (int i = 0; i < (int)NumberOfCutTypes; ++i) {
            CutType cut = static_cast<CutType>(i);
            if (appliedCut(cut) && passedCut(cut)) {
                if (!passedStr.empty()) passedStr += ",";
                passedStr += cutString(cut);
            }
        }
        return passedStr;
    }

    std::string TrackSegmentMatchResult::reasonString() const { return reasonString(reason); }

    TrackSegmentMatchCuts::TrackSegmentMatchCuts() { clear(); }

    TrackSegmentMatchCuts::~TrackSegmentMatchCuts() {}

    void TrackSegmentMatchCuts::clear() {
        posXCut = 0.0;
        posYCut = 0.0;
        posXPullCut = 0.0;
        posYPullCut = 0.0;
        angleXCut = 0.0;
        angleYCut = 0.0;
        angleXPullCut = 0.0;
        angleYPullCut = 0.0;
        matchChiSquaredCut = 0.0;
        useTightCuts = false;
        cutOnPosX = false;
        cutOnPosY = false;
        cutOnPosXPull = false;
        cutOnPosYPull = false;
        cutOnAngleX = false;
        cutOnAngleY = false;
        cutOnAngleXPull = false;
        cutOnAngleYPull = false;
        cutOnMatchChiSquared = false;
    }

}  // namespace Muon
