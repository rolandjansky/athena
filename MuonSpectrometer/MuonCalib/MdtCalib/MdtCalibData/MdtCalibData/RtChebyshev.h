/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// 23.03.2005, AUTHOR: OLIVER KORTNER
// Modified: 31.05.2006 by O. Kortner: major redesign:
//                                     driftvelocity implemented,
//                                     resolution has been removed,
//                                     set-method have been removed
//           04.06.2006 by O. Kortner: bug in constructor fixed,
//                                     doxygen entry updated.
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifndef MUONCALIB_RTCHEBYSHEV_H
#define MUONCALIB_RTCHEBYSHEV_H

//////////////////
// HEADER FILES //
//////////////////

// standard C++ //
#include <cstdlib>
#include <iostream>

// STL //
#include <vector>

// MDT calibration //
#include "MdtCalibData/IRtRelation.h"
#include "MuonCalibMath/Tschebyscheff_polynomial.h"

namespace MuonCalib {
    /**
    @class  RtChebyshev
    This class contains the implementation of an r(t) relationship
    parametrized by a linear combination of Chebyshev polyonomials.

    Convention:

    @f[ r(t) = \sum_{k=0}^{K}
                         p_k*T_k(2*(t-0.5*(tupper+tlower))/(tupper-tlower) @f]
    where T_k is the Chebyshev polynomial of k-th order,
    tupper and tlower are upper and lower drift-time bounds.

    Units: [t] = ns, [r] = mm, [v] = mm/ns.

    @author Oliver.Kortner@cern.ch
    @date 04.06.2006
    */

    class RtChebyshev : public IRtRelation {
    private:
        // Chebyshev polyonomials //
        const Tschebyscheff_polynomial* m_Chebyshev;  //!< pointer to the Chebyshev polynomials

        // private methods //
        /** initialization method */
        void _init(void);

    public:
        // Constructors
        /** initialization constructor,

        size of ParVec - 2 = order of the r(t) polynomial,

        ParVec[0] = t_low (smallest allowed drift time),
        ParVec[1] = t_up (largest allowed drift time).
        ParVec[2...] = parameters of the Chebyshev polynomial

        */

        explicit RtChebyshev(const ParVec& vec) : IRtRelation(vec) { _init(); }

        // Methods //
        // methods required by the base classes //
        std::string name(void) const;  //!< get the class name

        double radius(double t) const;
        //!< get the radius corresponding to the drift time t;
        //!< if t is not within [t_low, t_up] an unphysical radius of 99999 is returned

        //!< get the drift velocity
        double driftvelocity(double t) const;

        // get-methods specific to the RtChebyshev class //
        //!< get the lower drift-time bound
        double tLower(void) const;
        //!< get the upper drift-time bound
        double tUpper(void) const;

        //!< get the number of parameters used to describe the r(t) relationship
        unsigned int numberOfRtParameters(void) const;

        //!< get the coefficients of the r(t) polynomial
        std::vector<double> rtParameters(void) const;

        //!< get the reduced time which is the argument of the Chebyshev polynomial
        double get_reduced_time(const double& t) const;
    };
}  // namespace MuonCalib

#endif
