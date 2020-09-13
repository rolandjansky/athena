/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @brief vectorized version of parametrized sincos
 * @ author Miha Muskinja, Chistos
 */
#ifndef VEC_PARAMETRIZED_SINCOS_H
#define VEC_PARAMETRIZED_SINCOS_H

#include "CxxUtils/features.h"
#include "CxxUtils/restrict.h"
#include "CxxUtils/vec.h"
struct vsincos_par
{

  alignas(32) double sincos_parametrization[3][4] = {};

#if HAVE_FUNCTION_MULTIVERSIONING
#if defined(__x86_64__)

  __attribute__((target("avx2,fma")))

  void
  evaluate(const double r,
           double& ATH_RESTRICT sin_a,
           double& ATH_RESTRICT cos_a) const ATH_RESTRICT
  {
    // P0 = s4x^4 + s2x^2 + s0
    // P1 = s5x^4 + s3x^3 + s1
    // P2 = c4x^4 + c2x^2 + c0
    // P3 = c5x^4 + c3x^2 + c1

    // [s4, s5, c4, c5]
    const double r2 = r * r;
    CxxUtils::vec<double, 4> P = {
      sincos_parametrization[0][0],
      sincos_parametrization[0][1],
      sincos_parametrization[0][2],
      sincos_parametrization[0][3],
    };

    // [s4, s5, c4, c5] * r2 * r2
    // +
    // [s2, s3, c3, c3] * r2
    // +
    // s0, s1, c0, c1
    CxxUtils::vec<double, 4> param_1 = {
      sincos_parametrization[1][0],
      sincos_parametrization[1][1],
      sincos_parametrization[1][2],
      sincos_parametrization[1][3],
    };
    CxxUtils::vec<double, 4> param_2 = {
      sincos_parametrization[2][0],
      sincos_parametrization[2][1],
      sincos_parametrization[2][2],
      sincos_parametrization[2][3],
    };

    P = r2 * P + param_1;
    P = r2 * P + param_2;
    CxxUtils::vec<double, 4> P2 = { P[1], P[0], P[3], P[2] };
    CxxUtils::vec<double, 4> res = r * P2 + P;
    sin_a = res[0];
    cos_a = res[2];
  }

  __attribute__((target("avx2")))

  void
  eval(const double r,
       double& ATH_RESTRICT sin_a,
       double& ATH_RESTRICT cos_a) const ATH_RESTRICT
  {
    // P0 = s4x^4 + s2x^2 + s0
    // P1 = s5x^4 + s3x^3 + s1
    // P2 = c4x^4 + c2x^2 + c0
    // P3 = c5x^4 + c3x^2 + c1

    // [s4, s5, c4, c5]
    const double r2 = r * r;
    CxxUtils::vec<double, 4> P = {
      sincos_parametrization[0][0],
      sincos_parametrization[0][1],
      sincos_parametrization[0][2],
      sincos_parametrization[0][3],
    };

    // [s4, s5, c4, c5] * r2 * r2
    // +
    // [s2, s3, c3, c3] * r2
    // +
    // s0, s1, c0, c1
    CxxUtils::vec<double, 4> param_1 = {
      sincos_parametrization[1][0],
      sincos_parametrization[1][1],
      sincos_parametrization[1][2],
      sincos_parametrization[1][3],
    };
    CxxUtils::vec<double, 4> param_2 = {
      sincos_parametrization[2][0],
      sincos_parametrization[2][1],
      sincos_parametrization[2][2],
      sincos_parametrization[2][3],
    };

    P = r2 * P + param_1;
    P = r2 * P + param_2;
    CxxUtils::vec<double, 4> P2 = { P[1], P[0], P[3], P[2] };
    CxxUtils::vec<double, 4> res = r * P2 + P;
    sin_a = res[0];
    cos_a = res[2];
  }

  __attribute__((target("default")))
#endif // x86
#endif // FMV

  void
  eval(const double r,
       double& ATH_RESTRICT sin_a,
       double& ATH_RESTRICT cos_a) const ATH_RESTRICT
  {
    // P0 = s4x^4 + s2x^2 + s0
    // P1 = s5x^4 + s3x^3 + s1
    // P2 = c4x^4 + c2x^2 + c0
    // P3 = c5x^4 + c3x^2 + c1

    // [s4, s5, c4, c5]
    const double r2 = r * r;
    CxxUtils::vec<double, 4> P = {
      sincos_parametrization[0][0],
      sincos_parametrization[0][1],
      sincos_parametrization[0][2],
      sincos_parametrization[0][3]
    };

    // [s4, s5, c4, c5] * r2 * r2
    // +
    // [s2, s3, c3, c3] * r2
    // +
    // s0, s1, c0, c1
    CxxUtils::vec<double, 4> param_1 = {
      sincos_parametrization[1][0],
      sincos_parametrization[1][1],
      sincos_parametrization[1][2],
      sincos_parametrization[1][3]
    };
    CxxUtils::vec<double, 4> param_2 = {
      sincos_parametrization[2][0],
      sincos_parametrization[2][1],
      sincos_parametrization[2][2],
      sincos_parametrization[2][3]
    };

    P = r2 * P + param_1;
    P = r2 * P + param_2;
    sin_a = r * P[1] + P[0];
    cos_a = r * P[3] + P[2];
  }
};

#endif

