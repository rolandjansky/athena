/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#ifndef MUONCALIB_RtSpline_H
#define MUONCALIB_RtSpline_H

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

// root
class TSpline3;

namespace MuonCalib {
    /**
    @class  RtSpline
    This class contains the implementation of an r(t) relationship
    parameterized as support points to a cubic spline

    @author Felix.Rauscher@cern.ch
    @date 07.08.2007
    */

    class RtSpline : public IRtRelation {
    private:
        // Spline Class//
        TSpline3 *p_sp3;

        // private methods //
        /** initialization method */
        void _init(void);

    public:
        // Constructors
        /** initialization constructor,

        2 * (size of ParVec)  = number of points

        ParVec[    2n] = x coordinate of n_th support point
        ParVec[1 + 2n] = y coordinate of n_th support point

        */
        explicit RtSpline(const ParVec &vec) : IRtRelation(vec) { _init(); }

        ~RtSpline();

        // Methods //
        // methods required by the base classes //
        inline std::string name(void) const { return "RtSpline"; }

        //!< get the class name
        double radius(double t) const;
        //!< get the radius corresponding to the
        //!< drift time t;
        //!< 0 or 14.6 is returned if t is outside
        //!< the range
        double driftvelocity(double t) const;
        //!< get the drift velocity

        // get-methods specific to the RtSpline class //
        double tLower(void) const;
        //!< get the lower drift-time bound
        double tUpper(void) const;
        //!< get the upper drift-time bound
    };
}  // namespace MuonCalib

#endif
