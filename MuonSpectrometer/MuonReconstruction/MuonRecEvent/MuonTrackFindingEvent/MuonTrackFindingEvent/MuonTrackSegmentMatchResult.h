/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 * Author       : Martin Woudstra
 ***************************************************************************/

#ifndef MUONTRACKSEGMENTMATCHRESULT_H
#define MUONTRACKSEGMENTMATCHRESULT_H

// std
#include <string>
// other packages
#include "EventPrimitives/EventPrimitives.h"
#include "Identifier/Identifier.h"

namespace Trk {
  class Track;
}

// this package


namespace Muon {

  class MuonSegment;
  
  
  /** Class with matching return information */
  struct TrackSegmentMatchResult {

    //
    // nested types
    //
    enum CutType {
      PosXCut,
      PosYCut,
      PosXPullCut,
      PosYPullCut,
      AngXCut,
      AngYCut,
      AngXPullCut,
      AngYPullCut,
      MatchChiSquaredCut,
      NumberOfCutTypes
    };
        
    /** @brief turn enum CutType into a string */
    static std::string cutString( CutType cut );

    enum Reason {
      Unknown,
      NoCutsApplied,
      PassedAllCuts,
      PassedMatchChiSquaredCut,
      PassedPosAngleCuts,
      FailedCuts,
      SegmentMatching,
      NoSegmentPointer,
      StereoAngleWithoutPhi,
      NoClosestPars,
      FieldNotOk,
      NoClosestSegment,
      SegmentMatch,
      NoMomentumWithMagField,
      ExtrapolFailed,
      ExtrapolNoErrors,
      NoMeasErrors,
      PredLocalAnglesFailed,
      AngleMeasErrFailed,
      AnglePredErrFailed,
      CovarInverseFailed,
      LocalDirFailed,
      NumberOfReasons
    };

    /** @brief turn enum Reason into a string */
    static std::string reasonString( Reason r );




    //
    // public data members
    //
    double localPosXDiff;
    double localPosYDiff;
    double localAngleXDiff;
    double localAngleYDiff;
    double posXMeasErr2;
    double posYMeasErr2;
    double posXPredErr2;
    double posYPredErr2;
    double posXAlignErr2;
    double posYAlignErr2;
    double posXTotalErr2; // measured + predicted + alignment
    double posYTotalErr2; // measured + predicted + alignment
    double angleXMeasErr2;
    double angleYMeasErr2;
    double angleXPredErr2;
    double angleYPredErr2;
    double angleXAlignErr2;
    double angleYAlignErr2;
    double angleXTotalErr2; // measured + predicted + alignment
    double angleYTotalErr2; // measured + predicted + alignment
    double matchChiSquared;
    Amg::MatrixX predictionCovariance;
    Amg::MatrixX measuredCovariance;
    Amg::MatrixX totalCovariance; // measured + predicted + alignment
    Amg::VectorX diffVector;
    Identifier trackChamberId;   // ID of the MDT/CSC chamber on the track closest to segment
    Identifier segmentChamberId; // ID of the MDT/CSC chamber of the segment
    const Trk::Track* track;
    const MuonSegment* segment;
    Reason reason; // reason for acceptance or failure
    bool havePosX; // have X position difference available
    bool havePosY; // have Y position difference available
    bool havePosXError; // have total error on X position difference available
    bool havePosYError; // have total error on Y position difference available
    bool haveAngleX; // have X angle difference available
    bool haveAngleY; // have Y angle difference available
    bool haveAngleXError; // have total error on X angle difference available
    bool haveAngleYError; // have total error on X angle difference available
    bool haveMatchChiSquared; // have total match chi-squared available
    bool matchOK; // Final result: was the match OK or not

    //
    // public functions
    //
    TrackSegmentMatchResult();
    virtual ~TrackSegmentMatchResult();

    virtual void clear();
      
    /** Reset all cuts (applied & passed) */
    void resetCuts() {
      m_failedCuts = 0;
      m_passedCuts = 0;
    }

      
    /** Set cut applied and cut passed */
    void setCutPassed( CutType cut ) {
      m_passedCuts |= (1<<cut);
    }
      
    /** Set cut applied, but not cut passed */
    void setCutFailed( CutType cut ) {
      m_failedCuts |= (1<<cut);
    }

    /** Did it pass the cut? Returns true if cut was not applied or passed */
    bool passedCut( CutType cut ) const {
      return !(m_failedCuts & (1<<cut));
    }

    /** Did it fail the cut? Returns true if cut was applied and failed */
    bool failedCut( CutType cut ) const {
      return (m_failedCuts & (1<<cut));
    }

    /** Was the cut applied ? */
    bool appliedCut( CutType cut ) const {
      return (m_passedCuts & (1<<cut)) || (m_failedCuts & (1<<cut));
    }


    /** Was any cut applied ? */
    bool appliedAnyCut() const {
      return m_passedCuts || m_failedCuts;
    }

    /** Did all of the cuts that were applied pass? */
    bool passedAllCuts() const {
      return !m_failedCuts;
    }

    /** String with all cuts that were applied and failed */
    std::string failedCutsString() const;

    /** String with all cuts that were applied and passed */
    std::string passedCutsString() const;
      
    std::string reasonString() const;
      
  private:
    //
    // private data members
    //
    int m_failedCuts; // bitpattern with failed cuts (bits given by enum CutType)
    int m_passedCuts; // bitpattern with passed cuts (bits given by enum CutType)
      
  }; // end of class TrackSegmentMatchResult

    
  struct TrackSegmentMatchCuts {
    double posXCut;
    double posYCut;
    double posXPullCut;
    double posYPullCut;
    double angleXCut;
    double angleYCut;
    double angleXPullCut;
    double angleYPullCut;
    double matchChiSquaredCut;
    bool useTightCuts;
    bool cutOnPosX;
    bool cutOnPosY;
    bool cutOnPosXPull;
    bool cutOnPosYPull;
    bool cutOnAngleX;
    bool cutOnAngleY;
    bool cutOnAngleXPull;
    bool cutOnAngleYPull;
    bool cutOnMatchChiSquared;

    virtual void clear();
      
    TrackSegmentMatchCuts();
    virtual ~TrackSegmentMatchCuts();
      
  };


  

} // namespace MUON

#endif // MUONCOMBITRACKMAKER_MUONTRACKSEGMENTMATCHRESULT_H
