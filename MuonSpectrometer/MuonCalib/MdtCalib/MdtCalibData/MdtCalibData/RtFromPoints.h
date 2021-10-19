/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 30.06.2006, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef MuonCalib_RtFromPointsH
#define MuonCalib_RtFromPointsH

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// STL //
#include <vector>

// MuonCalib //
#include "MdtCalibData/RtChebyshev.h"
#include "MdtCalibData/RtRelationLookUp.h"
#include "MuonCalibMath/SamplePoint.h"

namespace MuonCalib {

    /// \class RtFromPoints
    /// This class allows the user to retrieve an RtChebyshev or RtRelationLookUp
    /// object corresponding to a set of (r, t) points.

    class RtFromPoints {
    public:
        // Constructor //
        RtFromPoints(void) {}
        ///< Default constructor.

        // Methods //

        ///< get an RtChebyshev resembling the r(t) function as described by the
        ///< sample points in the vector "sample_points"; use Chebyshev
        ///< polynomials up to order "order"; x1 coordinate of the sample points
        ///< contains the drift time, x2 the corresponding radius;
        ///< the method takes the minimum and maximum x1 values in the set of
        ///< sample points a lower and upper limits in RtChebyshev
        static RtChebyshev getRtChebyshev(const std::vector<SamplePoint>& sample_points, const unsigned int& order);

        ///< get an RtRelationLookUp resembling the r(t) function as
        ///< described by the sample points in  the vector "sample_points";
        ///< x1 coordinate of the sample points contains the drift time,
        ///< x2 the corresponding radius; the method takes the minimum and
        ///< maximum x1 values in the set of sample points a lower and upper
        ///< limits in RtRelationLookUp
        static RtRelationLookUp getRtRelationLookUp(const std::vector<SamplePoint>& sample_points);

    private:
        // get the minimimum and maximum x1 coordinate of the given sample points
        static void get_min_max(const std::vector<SamplePoint>& sample_points, double& x_min, double& x_max);
    };

}  // namespace MuonCalib

#endif
