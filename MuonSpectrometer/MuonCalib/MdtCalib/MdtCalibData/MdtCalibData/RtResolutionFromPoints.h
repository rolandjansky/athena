/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 11.04.2007, AUTHOR: OLIVER KORTNER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef MuonCalib_ResolutionFromPointsH
#define MuonCalib_ResolutionFromPointsH

//::::::::::::::::::
//:: HEADER FILES ::
//::::::::::::::::::

// STL //
#include <vector>

// MuonCalib //
#include "MdtCalibData/RtResolutionChebyshev.h"
#include "MdtCalibData/RtResolutionLookUp.h"
#include "MuonCalibMath/SamplePoint.h"

/// \class RtResolutionFromPoints
/// This class allows the user to retrieve an RtResolutionChebyshev or
/// RtResolutionLookUp object corresponding to a set of (t. resolution) points.

namespace MuonCalib {

    class RtResolutionFromPoints {
    public:
        // Constructor //
        RtResolutionFromPoints(void) {}
        ///< Default constructor.

        // Methods //

        ///< get an RtResolutionChebyshev resembling the sigma(t) function as described by the
        ///< sample points in the vector "sample_points"; use Chebyshev polynomials up to order
        ///< "order"; x1 coordinate of the sample points contains the drift time, x2 the
        ///< corresponding radius; the method takes the minimum and maximum x1 values in the set of
        ///< sample points a lower and upper limits in RtResolutionChebyshev
        static RtResolutionChebyshev getRtResolutionChebyshev(const std::vector<SamplePoint> &sample_points, const unsigned int &order);

        ///< get an RtResolutionLookUp resembling the sigma(t) function as
        ///< described by the sample points in the vector "sample_points";
        ///< x1 coordinate of the sample points contains the drift time,
        ///< x2 the corresponding radius; the method takes the minimum and
        ///< maximum x1 values in the set of sample points a lower and upper
        ///< limits in RtResolutionLookUp
        static RtResolutionLookUp getRtResolutionLookUp(const std::vector<SamplePoint> &sample_points);

    private:
        // get the minimimum and maximum x1 coordinate of the given sample points
        static void get_min_max(const std::vector<SamplePoint> &sample_points, double &x_min, double &x_max);
    };

}  // namespace MuonCalib

#endif
