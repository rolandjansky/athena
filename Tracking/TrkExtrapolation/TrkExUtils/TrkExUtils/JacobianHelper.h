/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef Jacobian__Helper__H
#define Jacobian__Helper__H

#include "CxxUtils/restrict.h"
#include "CxxUtils/vec.h"

namespace Trk {

    
    /////////////////////////////////////////////////////////////////////////////////
    ///  This provides an inline helper function for updating 
    ///  the jacobian during Runge-Kutta propagation. 
    ///  Used in several clients, put in here to avoid code duplication
    ///  Based on the existing implementation, but using vectorisation 
    ///  to improve speed by approximately 30%. 
    ///
    ///  The mathematics behind this procedure are documented in JINST 4 (2009) P04016 
    ///  also found as http://cds.cern.ch/record/1114177 / https://inspirehep.net/literature/826186
    /// 
    ///  In this implementation, we preserve the conventions of client code,
    ///  hence the use of array pointers rather than std constructs. 
    /////////////////////////////////////////////////////////////////////////////////

    inline void propJacobian(double* __restrict__ P,      /// P is a 42-element array containing global coordinates, direction,
                                                          /// inverse momentum and Jacobian of transformation. 
                                                          /// Its elements are updated in-place by this method. 
                const double* __restrict__ H0,            /// H0, H1, and H3 represent the field at the three points 
                const double* __restrict__ H1,            /// eachj is a 3-element array 
                const double* __restrict__ H2,

                const double* __restrict__ A,             /// A represents the direction vector before propagation      
                                                          /// It is a 3-vector          
                const double* __restrict__ A0,            /// A0, A3, A4, and A6 represent cross products of the direction  
                const double* __restrict__ A3,            /// with the field across the 3 points. Each is a 3-vector.      
                const double* __restrict__ A4,
                const double* __restrict__ A6,
                const double S3)                          /// S3 is a step length
    {
        using namespace CxxUtils;
        using vec2 = CxxUtils::vec<double, 2>;

        vec2 d23A_0{ P[24], P[31] };
        vec2 d23A_1{ P[25], P[32] };
        vec2 d23A_2{ P[26], P[33] };
        const double d4A_0 = P[38];
        const double d4A_1 = P[39];
        const double d4A_2 = P[40];
        // H
        const double H0_0 = H0[0];
        const double H0_1 = H0[1];
        const double H0_2 = H0[2];
        const double H1_0 = H1[0];
        const double H1_1 = H1[1];
        const double H1_2 = H1[2];
        const double H2_0 = H2[0];
        const double H2_1 = H2[1];
        const double H2_2 = H2[2];
        //
        vec2 d23A0 = H0_2 * d23A_1 - H0_1 * d23A_2;
        vec2 d23B0 = H0_0 * d23A_2 - H0_2 * d23A_0;
        vec2 d23C0 = H0_1 * d23A_0 - H0_0 * d23A_1;
        const double d4A0 = (A0[0] + H0_2 * d4A_1) - H0_1 * d4A_2;
        const double d4B0 = (A0[1] + H0_0 * d4A_2) - H0_2 * d4A_0;
        const double d4C0 = (A0[2] + H0_1 * d4A_0) - H0_0 * d4A_1;
        //
        vec2 d23A2 = d23A0 + d23A_0;
        vec2 d23B2 = d23B0 + d23A_1;
        vec2 d23C2 = d23C0 + d23A_2;
        const double d4A2 = d4A0 + d4A_0;
        const double d4B2 = d4B0 + d4A_1;
        const double d4C2 = d4C0 + d4A_2;
        const double d0 = d4A_0 - A[0];
        const double d1 = d4A_1 - A[1];
        const double d2 = d4A_2 - A[2];
        //
        vec2 d23A3 = (d23A_0 + d23B2 * H1_2) - d23C2 * H1_1;
        vec2 d23B3 = (d23A_1 + d23C2 * H1_0) - d23A2 * H1_2;
        vec2 d23C3 = (d23A_2 + d23A2 * H1_1) - d23B2 * H1_0;
        const double d4A3 = ((A3[0] + d0) + d4B2 * H1_2) - d4C2 * H1_1;
        const double d4B3 = ((A3[1] + d1) + d4C2 * H1_0) - d4A2 * H1_2;
        const double d4C3 = ((A3[2] + d2) + d4A2 * H1_1) - d4B2 * H1_0;
        //
        vec2 d23A4 = (d23A_0 + d23B3 * H1_2) - d23C3 * H1_1;
        vec2 d23B4 = (d23A_1 + d23C3 * H1_0) - d23A3 * H1_2;
        vec2 d23C4 = (d23A_2 + d23A3 * H1_1) - d23B3 * H1_0;
        const double d4A4 = ((A4[0] + d0) + d4B3 * H1_2) - d4C3 * H1_1;
        const double d4B4 = ((A4[1] + d1) + d4C3 * H1_0) - d4A3 * H1_2;
        const double d4C4 = ((A4[2] + d2) + d4A3 * H1_1) - d4B3 * H1_0;
        //
        vec2 d23A5 = 2. * d23A4 - d23A_0;
        vec2 d23B5 = 2. * d23B4 - d23A_1;
        vec2 d23C5 = 2. * d23C4 - d23A_2;
        const double d4A5 = 2. * d4A4 - d4A_0;
        const double d4B5 = 2. * d4B4 - d4A_1;
        const double d4C5 = 2. * d4C4 - d4A_2;
        //
        vec2 d23A6 = d23B5 * H2_2 - d23C5 * H2_1;
        vec2 d23B6 = d23C5 * H2_0 - d23A5 * H2_2;
        vec2 d23C6 = d23A5 * H2_1 - d23B5 * H2_0;
        double d4A6 = d4B5 * H2_2 - d4C5 * H2_1;
        double d4B6 = d4C5 * H2_0 - d4A5 * H2_2;
        double d4C6 = d4A5 * H2_1 - d4B5 * H2_0;

        vec2 dR23_A = (d23A2 + d23A3 + d23A4) * S3;
        vec2 dR23_B = (d23B2 + d23B3 + d23B4) * S3;
        vec2 dR23_C = (d23C2 + d23C3 + d23C4) * S3;

        vec2 res23_0 = ((d23A0 + 2. * d23A3) + (d23A5 + d23A6)) * (1. / 3.);
        vec2 res23_1 = ((d23B0 + 2. * d23B3) + (d23B5 + d23B6)) * (1. / 3.);
        vec2 res23_2 = ((d23C0 + 2. * d23C3) + (d23C5 + d23C6)) * (1. / 3.);

        /// now update the parameter array in the slots representing the Jacobian
        P[21] += dR23_A[0];
        P[22] += dR23_B[0];
        P[23] += dR23_C[0];
        P[24] = res23_0[0];
        P[25] = res23_1[0];
        P[26] = res23_2[0];

        P[28] += dR23_A[1];
        P[29] += dR23_B[1];
        P[30] += dR23_C[1];
        P[31] = res23_0[1];
        P[32] = res23_1[1];
        P[33] = res23_2[1];

        P[35] += (d4A2 + d4A3 + d4A4) * S3;
        P[36] += (d4B2 + d4B3 + d4B4) * S3;
        P[37] += (d4C2 + d4C3 + d4C4) * S3;
        P[38] = ((d4A0 + 2. * d4A3) + (d4A5 + d4A6 + A6[0])) * (1. / 3.);
        P[39] = ((d4B0 + 2. * d4B3) + (d4B5 + d4B6 + A6[1])) * (1. / 3.);
        P[40] = ((d4C0 + 2. * d4C3) + (d4C5 + d4C6 + A6[2])) * (1. / 3.);
    }
}

#endif // Jacobian__Helper__H