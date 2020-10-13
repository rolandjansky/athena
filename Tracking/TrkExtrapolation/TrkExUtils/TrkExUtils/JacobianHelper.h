/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  This provides a few inline helper functions for updating 
//  the jacobian during Runge-Kutta propagation. 
/////////////////////////////////////////////////////////////////////////////////

#ifndef Jacobian__Helper__H
#define Jacobian__Helper__H

#include "CxxUtils/restrict.h"
#include "CxxUtils/vec.h"

namespace Trk {

    #define theJacProp JacProp

    inline void
    JacProp(double* __restrict__ P,
            const double* __restrict__ H0,
            const double* __restrict__ H1,
            const double* __restrict__ H2,
            const double* __restrict__ A,
            const double* __restrict__ A0,
            const double* __restrict__ A3,
            const double* __restrict__ A4,
            const double* __restrict__ A6,
            const double S3)
    {
    double* d2A = &P[24];
    double* d3A = &P[31];
    double* d4A = &P[38];
    double d2A0 = H0[2] * d2A[1] - H0[1] * d2A[2];
    double d2B0 = H0[0] * d2A[2] - H0[2] * d2A[0];
    double d2C0 = H0[1] * d2A[0] - H0[0] * d2A[1];
    double d3A0 = H0[2] * d3A[1] - H0[1] * d3A[2];
    double d3B0 = H0[0] * d3A[2] - H0[2] * d3A[0];
    double d3C0 = H0[1] * d3A[0] - H0[0] * d3A[1];
    double d4A0 = (A0[0] + H0[2] * d4A[1]) - H0[1] * d4A[2];
    double d4B0 = (A0[1] + H0[0] * d4A[2]) - H0[2] * d4A[0];
    double d4C0 = (A0[2] + H0[1] * d4A[0]) - H0[0] * d4A[1];
    double d2A2 = d2A0 + d2A[0];
    double d2B2 = d2B0 + d2A[1];
    double d2C2 = d2C0 + d2A[2];
    double d3A2 = d3A0 + d3A[0];
    double d3B2 = d3B0 + d3A[1];
    double d3C2 = d3C0 + d3A[2];
    double d4A2 = d4A0 + d4A[0];
    double d4B2 = d4B0 + d4A[1];
    double d4C2 = d4C0 + d4A[2];
    double d0 = d4A[0] - A[0];
    double d1 = d4A[1] - A[1];
    double d2 = d4A[2] - A[2];
    double d2A3 = (d2A[0] + d2B2 * H1[2]) - d2C2 * H1[1];
    double d2B3 = (d2A[1] + d2C2 * H1[0]) - d2A2 * H1[2];
    double d2C3 = (d2A[2] + d2A2 * H1[1]) - d2B2 * H1[0];
    double d3A3 = (d3A[0] + d3B2 * H1[2]) - d3C2 * H1[1];
    double d3B3 = (d3A[1] + d3C2 * H1[0]) - d3A2 * H1[2];
    double d3C3 = (d3A[2] + d3A2 * H1[1]) - d3B2 * H1[0];
    double d4A3 = ((A3[0] + d0) + d4B2 * H1[2]) - d4C2 * H1[1];
    double d4B3 = ((A3[1] + d1) + d4C2 * H1[0]) - d4A2 * H1[2];
    double d4C3 = ((A3[2] + d2) + d4A2 * H1[1]) - d4B2 * H1[0];
    double d2A4 = (d2A[0] + d2B3 * H1[2]) - d2C3 * H1[1];
    double d2B4 = (d2A[1] + d2C3 * H1[0]) - d2A3 * H1[2];
    double d2C4 = (d2A[2] + d2A3 * H1[1]) - d2B3 * H1[0];
    double d3A4 = (d3A[0] + d3B3 * H1[2]) - d3C3 * H1[1];
    double d3B4 = (d3A[1] + d3C3 * H1[0]) - d3A3 * H1[2];
    double d3C4 = (d3A[2] + d3A3 * H1[1]) - d3B3 * H1[0];
    double d4A4 = ((A4[0] + d0) + d4B3 * H1[2]) - d4C3 * H1[1];
    double d4B4 = ((A4[1] + d1) + d4C3 * H1[0]) - d4A3 * H1[2];
    double d4C4 = ((A4[2] + d2) + d4A3 * H1[1]) - d4B3 * H1[0];
    double d2A5 = 2. * d2A4 - d2A[0];
    double d2B5 = 2. * d2B4 - d2A[1];
    double d2C5 = 2. * d2C4 - d2A[2];
    double d3A5 = 2. * d3A4 - d3A[0];
    double d3B5 = 2. * d3B4 - d3A[1];
    double d3C5 = 2. * d3C4 - d3A[2];
    double d4A5 = 2. * d4A4 - d4A[0];
    double d4B5 = 2. * d4B4 - d4A[1];
    double d4C5 = 2. * d4C4 - d4A[2];
    double d2A6 = d2B5 * H2[2] - d2C5 * H2[1];
    double d2B6 = d2C5 * H2[0] - d2A5 * H2[2];
    double d2C6 = d2A5 * H2[1] - d2B5 * H2[0];
    double d3A6 = d3B5 * H2[2] - d3C5 * H2[1];
    double d3B6 = d3C5 * H2[0] - d3A5 * H2[2];
    double d3C6 = d3A5 * H2[1] - d3B5 * H2[0];
    double d4A6 = d4B5 * H2[2] - d4C5 * H2[1];
    double d4B6 = d4C5 * H2[0] - d4A5 * H2[2];
    double d4C6 = d4A5 * H2[1] - d4B5 * H2[0];

    double* dR = &P[21];
    dR[0] += (d2A2 + d2A3 + d2A4) * S3;
    dR[1] += (d2B2 + d2B3 + d2B4) * S3;
    dR[2] += (d2C2 + d2C3 + d2C4) * S3;
    d2A[0] = ((d2A0 + 2. * d2A3) + (d2A5 + d2A6)) * (1. / 3.);
    d2A[1] = ((d2B0 + 2. * d2B3) + (d2B5 + d2B6)) * (1. / 3.);
    d2A[2] = ((d2C0 + 2. * d2C3) + (d2C5 + d2C6)) * (1. / 3.);

    dR = &P[28];
    dR[0] += (d3A2 + d3A3 + d3A4) * S3;
    dR[1] += (d3B2 + d3B3 + d3B4) * S3;
    dR[2] += (d3C2 + d3C3 + d3C4) * S3;
    d3A[0] = ((d3A0 + 2. * d3A3) + (d3A5 + d3A6)) * (1. / 3.);
    d3A[1] = ((d3B0 + 2. * d3B3) + (d3B5 + d3B6)) * (1. / 3.);
    d3A[2] = ((d3C0 + 2. * d3C3) + (d3C5 + d3C6)) * (1. / 3.);

    dR = &P[35];
    dR[0] += (d4A2 + d4A3 + d4A4) * S3;
    dR[1] += (d4B2 + d4B3 + d4B4) * S3;
    dR[2] += (d4C2 + d4C3 + d4C4) * S3;
    d4A[0] = ((d4A0 + 2. * d4A3) + (d4A5 + d4A6 + A6[0])) * (1. / 3.);
    d4A[1] = ((d4B0 + 2. * d4B3) + (d4B5 + d4B6 + A6[1])) * (1. / 3.);
    d4A[2] = ((d4C0 + 2. * d4C3) + (d4C5 + d4C6 + A6[2])) * (1. / 3.);
    }

    /*
    * d2A, d3A, d4A are a 
    * d23A {d2A,d3A} vector (2 doubles) and 
    * d4A double 
    *
    * Basically more like 1st buddle calculation
    * together 
    */
    inline void
    JacPropVec(double* __restrict__ P,
                const double* __restrict__ H0,
                const double* __restrict__ H1,
                const double* __restrict__ H2,
                const double* __restrict__ A,
                const double* __restrict__ A0,
                const double* __restrict__ A3,
                const double* __restrict__ A4,
                const double* __restrict__ A6,
                const double S3)
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



    /*
    * Something like the
    * d4A6
    * d4B6
    * d4C6
    * becomes d46_xy (the A,B) , d46_z (the C)
    * e.g A ->x , B -> y, C->z
    *
    * The code can be written using cross product 
    * like operations 
    */
    inline void
    JacPropVec2(double* __restrict__ P,
            const double* __restrict__ H0,
            const double* __restrict__ H1,
            const double* __restrict__ H2,
            const double* __restrict__ A,
            const double* __restrict__ A0,
            const double* __restrict__ A3,
            const double* __restrict__ A4,
            const double* __restrict__ A6,
            const double S3)
    {

    using namespace CxxUtils;
    using vec2 = CxxUtils::vec<double, 2>;

    // d2 part
    vec2 d2R_xy{};
    vload(d2R_xy, &P[21]);
    vec2 d2R_z{ P[23] };
    vec2 d2_xy;
    vload(d2_xy, &P[24]);
    vec2 d2_yz;
    vload(d2_yz, &P[25]);
    vec2 d2_zx{};
    vblend<1, 2>(d2_zx, d2_yz, d2_xy);

    // d3 part
    vec2 d3R_xy{};
    vload(d3R_xy, &P[28]);
    vec2 d3R_z{ P[30] };
    vec2 d3_xy;
    vload(d3_xy, &P[31]);
    vec2 d3_yz;
    vload(d3_yz, &P[32]);
    vec2 d3_zx{};
    vblend<1, 2>(d3_zx, d3_yz, d3_xy);

    // d4 part
    vec2 d4R_xy{};
    vload(d4R_xy, &P[35]);
    vec2 d4R_z{ P[37] };
    vec2 d4_xy;
    vload(d4_xy, &P[38]);
    vec2 d4_yz;
    vload(d4_yz, &P[39]);
    vec2 d4_zx{};
    vblend<1, 2>(d4_zx, d4_yz, d4_xy);

    // H0
    vec2 H0_yz;
    vload(H0_yz, &H0[1]);
    vec2 H0_xy;
    vload(H0_xy, &H0[0]);
    vec2 H0_zx{};
    vblend<1, 2>(H0_zx, H0_yz, H0_xy);
    // H1
    vec2 H1_yz;
    vload(H1_yz, &H1[1]);
    vec2 H1_xy;
    vload(H1_xy, &H1[0]);
    vec2 H1_zx{};
    vblend<1, 2>(H1_zx, H1_yz, H1_xy);
    // H2
    vec2 H2_yz;
    vload(H2_yz, &H2[1]);
    vec2 H2_xy;
    vload(H2_xy, &H2[0]);
    vec2 H2_zx{};
    vblend<1, 2>(H2_zx, H2_yz, H2_xy);

    // As
    vec2 A0_xy;
    vload(A0_xy, &A0[0]);
    vec2 A0_zx = { A0[2], A0[0] };
    vec2 A_xy;
    vload(A_xy, &A[0]);
    vec2 A_zx = { A[2], A[0] };
    vec2 A3_xy;
    vload(A3_xy, &A3[0]);
    vec2 A3_zx = { A3[2], A3[0] };
    vec2 A4_xy;
    vload(A4_xy, &A4[0]);
    vec2 A4_zx = { A4[2], A4[0] };
    vec2 A6_xy;
    vload(A6_xy, &A6[0]);
    vec2 A6_zx = { A6[2], A6[0] };

    //
    vec2 d20_xy = H0_zx * d2_yz - H0_yz * d2_zx;
    vec2 d20_zx = H0_yz * d2_xy - H0_xy * d2_yz;
    vec2 d30_xy = H0_zx * d3_yz - H0_yz * d3_zx;
    vec2 d30_zx = H0_yz * d3_xy - H0_xy * d3_yz;
    vec2 d40_xy = (A0_xy + H0_zx * d4_yz) - H0_yz * d4_zx;
    vec2 d40_zx = (A0_zx + H0_yz * d4_xy) - H0_xy * d4_yz;
    //
    vec2 d22_xy = d20_xy + d2_xy;
    vec2 d22_zx = d20_zx + d2_zx;
    vec2 d22_yz{};
    vblend<1, 2>(d22_yz, d22_xy, d22_zx);
    vec2 d32_xy = d30_xy + d3_xy;
    vec2 d32_zx = d30_zx + d3_zx;
    vec2 d32_yz{};
    vblend<1, 2>(d32_yz, d32_xy, d32_zx);
    vec2 d42_xy = d40_xy + d4_xy;
    vec2 d42_zx = d40_zx + d4_zx;
    vec2 d42_yz{};
    vblend<1, 2>(d42_yz, d42_xy, d42_zx);
    vec2 d_xy = d4_xy - A_xy;
    vec2 d_zx = d4_zx - A_zx;
    //
    vec2 d23_xy = (d2_xy + d22_yz * H1_zx) - d22_zx * H1_yz;
    vec2 d23_zx = (d2_zx + d22_xy * H1_yz) - d22_yz * H1_xy;
    vec2 d23_yz{};
    vblend<1, 2>(d23_yz, d23_xy, d23_zx);
    vec2 d33_xy = (d3_xy + d32_yz * H1_zx) - d32_zx * H1_yz;
    vec2 d33_zx = (d3_zx + d32_xy * H1_yz) - d32_yz * H1_xy;
    vec2 d33_yz{};
    vblend<1, 2>(d33_yz, d33_xy, d33_zx);
    vec2 d43_xy = ((A3_xy + d_xy) + d42_yz * H1_zx) - d42_zx * H1_yz;
    vec2 d43_zx = ((A3_zx + d_zx) + d42_xy * H1_yz) - d42_yz * H1_xy;
    vec2 d43_yz{};
    vblend<1, 2>(d43_yz, d43_xy, d43_zx);
    //
    vec2 d24_xy = (d2_xy + d23_yz * H1_zx) - d23_zx * H1_yz;
    vec2 d24_zx = (d2_zx + d23_xy * H1_yz) - d23_yz * H1_xy;
    vec2 d34_xy = (d3_xy + d33_yz * H1_zx) - d33_zx * H1_yz;
    vec2 d34_zx = (d3_zx + d33_xy * H1_yz) - d33_yz * H1_xy;
    vec2 d44_xy = ((A4_xy + d_xy) + d43_yz * H1_zx) - d43_zx * H1_yz;
    vec2 d44_zx = ((A4_zx + d_zx) + d43_xy * H1_yz) - d43_yz * H1_xy;
    //
    vec2 d25_xy = 2. * d24_xy - d2_xy;
    vec2 d25_zx = 2. * d24_zx - d2_zx;
    vec2 d25_yz{};
    vblend<1, 2>(d25_yz, d25_xy, d25_zx);
    vec2 d35_xy = (d34_xy + d34_xy) - d3_xy;
    vec2 d35_zx = (d34_zx + d34_zx) - d3_zx;
    vec2 d35_yz{};
    vblend<1, 2>(d35_yz, d35_xy, d35_zx);
    vec2 d45_xy = 2 * d44_xy - d4_xy;
    vec2 d45_zx = 2 * d44_zx - d4_zx;
    vec2 d45_yz{};
    vblend<1, 2>(d45_yz, d45_xy, d45_zx);
    //
    vec2 d26_xy = d25_yz * H2_zx - d25_zx * H2_yz;
    vec2 d26_zx = d25_xy * H2_yz - d25_yz * H2_xy;
    vec2 d36_xy = d35_yz * H2_zx - d35_zx * H2_yz;
    vec2 d36_zx = d35_xy * H2_yz - d35_yz * H2_xy;
    vec2 d46_xy = d45_yz * H2_zx - d45_zx * H2_yz;
    vec2 d46_zx = d45_xy * H2_yz - d45_yz * H2_xy;
    //
    d2R_xy += (d22_xy + d23_xy + d24_xy) * S3;
    d2R_z += ((d22_zx + d23_zx + d24_zx) * S3);
    d3R_xy += (d32_xy + d33_xy + d34_xy) * S3;
    d3R_z += (d32_zx + d33_zx + d34_zx) * S3;
    d4R_xy += (d42_xy + d43_xy + d44_xy) * S3;
    d4R_z += (d42_zx + d43_zx + d44_zx) * S3;

    // store back d2
    vstore(&P[21], d2R_xy);
    P[23] = d2R_z[0];
    vstore(&P[24], ((d20_xy + 2 * d23_xy) + (d25_xy + d26_xy)) * (1. / 3.));
    P[26] = (((d20_zx + 2 * d23_zx) + (d25_zx + d26_zx))[0] * (1. / 3.));

    // store back d3
    vstore(&P[28], d3R_xy);
    P[30] = d3R_z[0];
    vstore(&P[31], ((d30_xy + 2 * d33_xy) + (d35_xy + d36_xy)) * (1. / 3.));
    P[33] = (((d30_zx + 2 * d33_zx) + (d35_zx + d36_zx))[0] * (1. / 3.));

    // store d4 part
    vstore(&P[35], d4R_xy);
    P[37] = d4R_z[0];
    vstore(&P[38],
            ((d40_xy + 2 * d43_xy) + (d45_xy + d46_xy + A6_xy)) * (1. / 3.));
    P[40] = (((d40_zx + 2 * d43_zx) + (d45_zx + d46_zx + A6_zx))[0] * (1. / 3.));
    }
}

#endif // Jacobian__Helper__H