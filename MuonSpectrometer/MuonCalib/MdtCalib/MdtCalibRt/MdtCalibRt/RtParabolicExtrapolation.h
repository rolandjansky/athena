/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#ifndef MuonCalib_RtParabolicExtrapolationH
#define MuonCalib_RtParabolicExtrapolationH

/// \class RtParabolicExtrapolation
///
/// This class is used to fit a parabola to the r(t) curve for in
/// $[r_{min}, r_{max}]$ and to set r(t) according to this parabola for
/// $r_{ext}>r>r_{max}$, if $r_{ext}>r_{max}$, and $r_{ext}<r<r_{min}$,
/// if $r_{ext}<r_{min}$. The boundaries of the fit interval can be changed by
/// the user.
///
/// \author Oliver.Kortner@CERN.CH
///
/// \date 28.02.2009

#include <MuonCalibMath/SamplePoint.h>

#include <vector>

namespace MuonCalib {
    class IRtRelation;
    class RtRelationLookUp;

    class RtParabolicExtrapolation {
    public:
        // Constructors //
        RtParabolicExtrapolation(void);
        ///< Default constructor.

        // Methods //
        RtRelationLookUp getRtWithParabolicExtrapolation(const IRtRelation& in_rt, const double& r_min = 13.0,
                                                         const double& r_max = 14.0) const;
        ///< get an r-t relationship which is
        ///< equivalent to the input relationship
        ///< in_rt for r<r_min and has r(t) for
        ///< r>r_max according to a parabola fitted
        ///< in [r_min, r_max];
        ///< this method is there for backward
        ///< compatibility
        RtRelationLookUp getRtWithParabolicExtrapolation(const IRtRelation& in_rt, const double& r_min, const double& r_max,
                                                         const double& r_ext, const std::vector<SamplePoint>& add_fit_points) const;
        ///< The method fits a parabola to the r-t
        ///< points in $[r_{min}, r_{max}]$ and the
        ///< additional user points add_fit_points.
        ///< The input r-t relationship in_rt is
        ///< replaced by the parabola in
        ///< $[r_{ext}, r_{min}]$ is
        ///< $r_{ext}<r_{min}$ and in
        ///< $[r_{max}, r_{ext}]$ if
        ///< $r_{ext}>r_{max}$.

    private:
        double t_from_r(const double& r, const IRtRelation& in_rt) const;
        // get t(r) as defined in in_rt
        double get_max_t_at_r(const double& r, const IRtRelation& in_rt) const;
        // get the largest time of the r-t
        //  relationship in_rt corresponding to
        // the given radius r
    };

}  // namespace MuonCalib

#endif
