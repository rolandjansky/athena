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

#include "MuonTrackFindingEvent/MuonTrackSegmentMatchResult.h"

namespace Muon {


  TrackSegmentMatchResult::TrackSegmentMatchResult() {
    clear();
    resetCuts();
  }

  TrackSegmentMatchResult::~TrackSegmentMatchResult() {
    clear();
  }
  
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
    track = 0;
    segment = 0;
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

  
  std::string TrackSegmentMatchResult::cutString( CutType cut ) {
    static const char* cutStrings[NumberOfCutTypes] = {};
    static bool initList = false;
    if (!initList) {
      initList = true;
      cutStrings[PosXCut] = "PosXCut";
      cutStrings[PosYCut] = "PosYCut";
      cutStrings[AngXCut] = "AngleXCut";
      cutStrings[AngYCut] = "AngleYCut";
      cutStrings[PosXPullCut] = "PosXPullCut";
      cutStrings[PosYPullCut] = "PosYPullCut";
      cutStrings[AngXPullCut] = "AngleXPullCut";
      cutStrings[AngYPullCut] = "AngleYPullCut";
      cutStrings[MatchChiSquaredCut] = "ChiSquaredMatchCut";
    }
    if ( (int)cut < 0 || (int)cut >= (int)NumberOfCutTypes ) return "CutTypeOutOfRange";
    const char* str = cutStrings[cut];
    if ( !str ) return "CutTypeNotInList";
    return str;
  }
    

  std::string TrackSegmentMatchResult::reasonString( Reason r ) {
    static const char* reasonStrings[NumberOfReasons] = {};
    static bool initList = false;
    if (!initList) {
      initList = true;
      reasonStrings[Unknown] = "Unknown";
      reasonStrings[NoCutsApplied] = "NoCutsApplied";
      reasonStrings[PassedAllCuts] = "PassedAllCuts";
      reasonStrings[PassedMatchChiSquaredCut] = "PassedMatchChiSquaredCut";
      reasonStrings[PassedPosAngleCuts] = "PassedPosAngleCuts";
      reasonStrings[FailedCuts] = "FailedCuts";
      reasonStrings[SegmentMatching] = "SegmentMatching";
      reasonStrings[NoSegmentPointer] = "NoSegmentPointer";
      reasonStrings[StereoAngleWithoutPhi] = "StereoAngleWithoutPhi";
      reasonStrings[NoClosestPars] = "NoClosestPars";
      reasonStrings[FieldNotOk] = "FieldNotOk";
      reasonStrings[NoClosestSegment] = "NoClosestSegment";
      reasonStrings[SegmentMatch] = "SegmentMatch";
      reasonStrings[NoMomentumWithMagField] = "NoMomentumWithMagField";
      reasonStrings[ExtrapolFailed] = "ExtrapolationFailed";
      reasonStrings[ExtrapolNoErrors] = "ExtrapolationNoErrors";
      reasonStrings[NoMeasErrors] = "NoMeasErrors";
      reasonStrings[AngleMeasErrFailed] = "AngleMeasErrFailed";
      reasonStrings[AnglePredErrFailed] = "AnglePredErrFailed";
      reasonStrings[LocalDirFailed] = "LocalDirFailed";
    }
    if ( (int)r < 0 || (int)r >= NumberOfReasons ) return "ReasonOutOfRange";
    const char* str = reasonStrings[r];
    if ( !str ) return "ReasonNotInList";
    return str;
  }


  /** String with all cuts that were applied and failed */
  std::string TrackSegmentMatchResult::failedCutsString() const {
    std::string failedStr;
    for ( int i = 0; i < (int)NumberOfCutTypes; ++i ) {
      CutType cut = static_cast<CutType>(i);
      if ( appliedCut(cut) && failedCut(cut) ) {
        if ( !failedStr.empty() ) failedStr += ",";
        failedStr += TrackSegmentMatchResult::cutString(cut);
      }
    }
    return failedStr;
  }
  
  
  /** String with all cuts that were applied and passed */
  std::string TrackSegmentMatchResult::passedCutsString() const {
    std::string passedStr;
    for ( int i = 0; i < (int)NumberOfCutTypes; ++i ) {
      CutType cut = static_cast<CutType>(i);
      if ( appliedCut(cut) && passedCut(cut) ) {
        if ( !passedStr.empty() ) passedStr += ",";
        passedStr += cutString(cut);
      }
    }
    return passedStr;
  }

  std::string TrackSegmentMatchResult::reasonString() const {
    return reasonString( reason );
  }

  TrackSegmentMatchCuts::TrackSegmentMatchCuts() {
    clear();
  }

  TrackSegmentMatchCuts::~TrackSegmentMatchCuts() {
  }

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

  
} // namespace Muon
