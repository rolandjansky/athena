/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 29.07.2008, AUTHOR: OLIVER KORTNER
// Modified: 20.03.2009 by O. Kortner, additional method ,,performSmoothing''
//                      with better performance than the old method added;
//                      softer chi^2 cuts in addResidualsFromSegment.
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef MuonCalib_AdaptiveResidualSmoothingH
#define MuonCalib_AdaptiveResidualSmoothingH

//:::::::::::::::::::::::::::::::::::::
//:: CLASS AdaptiveResidualSmoothing ::
//:::::::::::::::::::::::::::::::::::::

/// \class AdaptiveResidualSmoothing
///
/// This class is used to modify the r-t relationship to give smooth residuals
/// centred at 0. The binning of the residual distribution is adapted to the
/// statistics. The user can set the number of entries per bin. The user can
/// decide whether the track fits should be performed with a straight or
/// parabolic curve.
///
/// \author Oliver.Kortner@CERN.CH
///
/// \date 28.07.2008

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// STL //
#include <vector>

// MuonCalib //
#include "MdtCalibData/RtRelationLookUp.h"
#include "MdtCalibFitters/CurvedPatRec.h"
#include "MdtCalibFitters/StraightPatRec.h"
#include "MuonCalibMath/DataPoint.h"

namespace MuonCalib {

    class IRtRelation;

    class AdaptiveResidualSmoothing {
    public:
        // Constructor //
        AdaptiveResidualSmoothing(void);
        ///< Default constructor.

        // Methods //
        void clear(void);
        ///< clear the memory of the class
        void addResidual(const double& radius, const double& residual);
        ///< add the residual at the given radius
        bool addResidualsFromSegment(MuonCalibSegment& seg, bool curved, double road_width);
        ///< reconstruct the given segment and
        ///< store the residuals; if curved is true
        ///< a curved segment fit is performed,
        ///< otherwise a straight segment is fitted
        ///< to the drift radii; the user must
        ///< set the road width used in the pattern
        ///< recognition;
        ///< returns true in case of success
        RtRelationLookUp performSmoothing(const IRtRelation& rt_rel, unsigned int nb_entries_per_bin, bool fix_t_min, bool fix_t_max);
        ///< use the stored residuals to improve
        ///< the given r-t relationship to give
        ///< smoother residuals; the user has to
        ///< set the number of entries per radial
        ///< bin; the user can request that the
        ///< radii for the minimum and maximum
        ///< drift time are untouched
        RtRelationLookUp performSmoothing(const IRtRelation& rt_rel, const bool& fix_t_min, const bool& fix_t_max);
        ///< use the stored residuals to improve
        ///< the given r-t relationship to give
        ///< smoother residuals; the user can
        ///< request that the radii for the minimum
        ///< and maximum drift time are untouched

    private:
        std::vector<DataPoint> m_residual_point;  // vector of residual points
        StraightPatRec m_sfitter;                 // straight-line fitter
        CurvedPatRec m_cfitter;                   // curved-line fitter
        double t_from_r(const IRtRelation& rt_rel, const double& r);
        // get t(r) for the given r-t relationship,
        // the method is auxiliary and not optimized;
        // it will disappear when the t(r) will be
        // available in the MuonCalib framework
    };

}  // namespace MuonCalib

#endif
