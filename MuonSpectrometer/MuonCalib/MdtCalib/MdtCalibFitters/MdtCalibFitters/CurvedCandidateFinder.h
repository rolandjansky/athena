/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonCalib_CurvedCandidateFinderH
#define MuonCalib_CurvedCandidateFinderH

//:::::::::::::::::::::::::::::::::
//:: CLASS CurvedCandidateFinder ::
//:::::::::::::::::::::::::::::::::

/// \class CurvedCandidateFinder
///
/// This class searches for candidates of curved parabolic lines connecting
/// all hits given to the class within a user defined search road.
///
/// \author Oliver.Kortner@CERN.CH
///
/// \date 13.07.2008

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// STL //
#include <vector>

// MuonCalib //
#include "MdtCalibFitters/CurvedLine.h"
#include "MuonCalibEventBase/MdtCalibHitBase.h"
#include "MuonCalibEventBase/MuonCalibSegment.h"

namespace MuonCalib {

    class CurvedCandidateFinder {
    public:
        using MdtHitVec = MuonCalibSegment::MdtHitVec;
        using MdtHitPtr = MuonCalibSegment::MdtHitPtr;
        // Constructors //
        CurvedCandidateFinder(const MdtHitVec &hits);
        ///< Constructor
        ///< @param hits Vector of hits used in the candidate finding.

        // Methods //
        const std::vector<CurvedLine> &getCandidates(const double &road_width);
        ///< get all candidates connecting all hits
        ///< within the given road width (mm)
        const std::vector<CurvedLine> &getCandidates(const double &road_width, const Amg::Vector3D &est_dir);
        ///< get all candidates connecting all hits
        ///< within the given road width (mm);
        ///< est_dir is the estimated direction of
        ///< incidence

    private:
        // hits //
        MdtHitVec m_hits;  // vector hits used in the
                           // candidate search

        // candidates //
        std::vector<CurvedLine> m_candidates;  // vector of candidate lines
    };

}  // namespace MuonCalib

#endif
