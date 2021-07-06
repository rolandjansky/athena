/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 13.07.2008, AUTHOR: OLIVER KORTNER
// Modified: 04.08.2008 by O. Kortner, estimated direction of incidence can be
//                                     set.
//           07.08.2008 by O. Kortner, bug fig in the pattern recognition.
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

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

namespace MuonCalib {

    class CurvedCandidateFinder {
    public:
        // Constructors //
        CurvedCandidateFinder(const std::vector<const MdtCalibHitBase *> &hits);
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
        std::vector<const MdtCalibHitBase *> m_hits;  // vector hits used in the
                                                      // candidate search

        // candidates //
        std::vector<CurvedLine> m_candidates;  // vector of candidate lines
    };

}  // namespace MuonCalib

#endif
