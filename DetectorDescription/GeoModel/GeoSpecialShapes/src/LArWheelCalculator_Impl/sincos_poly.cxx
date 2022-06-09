/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoSpecialShapes/LArWheelCalculator.h"

#include "CxxUtils/sincos.h"

#ifdef PORTABLE_LAR_SHAPE
#define SINCOSPOLY_USE_EIGEN 1
#endif

#ifndef SINCOSPOLY_USE_EIGEN
#include "TMath.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "TMatrixDLazy.h"
#include "TDecompLU.h"
#include "TDecompSVD.h"
typedef TVectorD     VECTOR;
typedef TMatrixD     MATRIX;
typedef TMatrixDSym  SYMMATRIX;
#else
#include "Eigen/Dense"
typedef Eigen::VectorXd  VECTOR;
typedef Eigen::MatrixXd  MATRIX;
typedef Eigen::MatrixXd  SYMMATRIX;
#define Int_t    int
#define Double_t double
#endif

#include <sys/time.h>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <mutex>

#define DEBUGPRINT 0
#include "GeoModelKernel/Units.h"
using namespace GeoModelKernelUnits;

#ifndef SINCOSPOLY_USE_EIGEN
template<typename T, typename Q>
std::ostream & operator << (std::ostream & ostr, const TVectorT<T> & v)
{
  std::ios_base::fmtflags save_flags(ostr.flags());
  ostr << '[';
  ostr << std::scientific;
  for(Int_t idx=v.GetLwb();idx<v.GetUpb();idx++) {
    ostr << v[idx] << ", ";
  }
  ostr << v[v.GetUpb()];
  ostr << ']';
  ostr.flags(save_flags);
  return ostr;
}
#endif

// find best approximation of y values using linear combination of basis functions in bf
static VECTOR findLinearApproximation(
    const Int_t dataLen, const Int_t nBasisFuntions,
    const VECTOR &y, const MATRIX & bf)
{
#ifndef SINCOSPOLY_USE_EIGEN
  SYMMATRIX A(nBasisFuntions);
  VECTOR    vY(nBasisFuntions);
#else
  SYMMATRIX A=SYMMATRIX::Zero(nBasisFuntions,nBasisFuntions);
  VECTOR    vY=VECTOR::Zero(nBasisFuntions);
#endif
  for(Int_t j = 0; j < nBasisFuntions; ++ j){
    for(Int_t k = 0; k < nBasisFuntions; ++ k){
      Double_t Ajk = 0.0;
      for(Int_t i = 0; i < dataLen; ++ i){
        Ajk += bf(j, i) * bf(k, i);
      }
      A(j, k) = Ajk;
    }
  }

  for(Int_t k = 0; k < nBasisFuntions; ++ k){
    Double_t vYk = 0.0;
    for(Int_t i = 0; i < dataLen; ++ i){
      vYk += y[i]*bf(k,i);
    }
    vY[k] = vYk;
  }
#ifndef SINCOSPOLY_USE_EIGEN 
  SYMMATRIX Ainv(A);
  Ainv.Invert();
  return Ainv*vY;
#else
  return A.inverse()*vY;
#endif
}

using namespace CLHEP;

/// Helper functions to calculate polynomial approximations
namespace {
  const Int_t nBasisFunctions = LARWC_SINCOS_POLY + 1;

  /// Helper struct to return sine/cosine values from functions
  struct sincos_params {
    std::array<double, nBasisFunctions> sin{};
    std::array<double, nBasisFunctions> cos{};
  };

  /// Calculate polynomial approximations for given wheel radii
  sincos_params calc_params(LArWheelCalculator& lwc, double Rmin, double Rmax) {

    sincos_params p;

    const Double_t Rstep = 1.*mm;
    const Int_t nrPoints = (Rmax - Rmin) * (1./Rstep);
    const Int_t dataLen = nrPoints + 1;
#ifndef SINCOSPOLY_USE_EIGEN
    VECTOR x(dataLen);  // angle points
    VECTOR ysin(dataLen);  // to be approximated function values at angle points - sin
    VECTOR ycos(dataLen);  // to be approximated function values at angle points - cos
    MATRIX bf(nBasisFunctions, dataLen); // Matrix of values of basis functions at angle points
#else
    VECTOR x=VECTOR::Zero(dataLen);  // angle points
    VECTOR ysin=VECTOR::Zero(dataLen);  // to be approximated function values at angle points - sin
    VECTOR ycos=VECTOR::Zero(dataLen);  // to be approximated function values at angle points - cos
    MATRIX bf=MATRIX::Zero(nBasisFunctions, dataLen); // Matrix of values of basis functions at angle points
#endif
    for(Int_t i = 0; i < dataLen; ++ i){
      const Double_t a = Rmin + i * Rstep;
      x[i] = a;
      CxxUtils::sincos scalpha(lwc.parameterized_slant_angle(a));
      ysin[i] = scalpha.sn;
      ycos[i] = scalpha.cs;
      for(Int_t n = 0; n < nBasisFunctions; ++ n) {
        bf(n, i) = pow(a, n);
      }
    }

    VECTOR params_sin =
      findLinearApproximation(dataLen, nBasisFunctions, ysin, bf);
    VECTOR params_cos =
      findLinearApproximation(dataLen, nBasisFunctions, ycos, bf);

    for(Int_t i = 0; i < nBasisFunctions; ++ i){
      p.sin[i] = params_sin[i];
      p.cos[i] = params_cos[i];
    }

  //
  // Nothing below is needed unless for debugging
  //
#if DEBUGPRINT
  std::cout << "LARWC_SINCOS_POLY: " << LARWC_SINCOS_POLY << std::endl;
  std::cout << "sin params:" << params_sin << std::endl;
  std::cout << "cos params:" << params_cos << std::endl;

  double dsinr = 0., dcosr = 0.;
  double dtrigr = 0;

  double dsin = 0., dcos = 0.;
  double dtrig = 0.;
  for(double r = Rmin + 40.; r < Rmax - 40.; r += Rstep / 10.){
    CxxUtils::sincos scalpha(lwc.parameterized_slant_angle(r));
    double sin_a, cos_a;
    double sin_a_v, cos_a_v;
    lwc.parameterized_sincos(r, sin_a, cos_a);
    lwc.m_vsincos_par.eval(r, sin_a_v, cos_a_v);
    std::streamsize ss = std::cout.precision();
    std::cout.precision(16);
    std::cout << "def: " << r << " " << sin_a << " " << cos_a << std::endl;
    std::cout << "vec: " << r << " " << sin_a_v << " " << cos_a_v << std::endl;
    std::cout << "dif: " << r << " " << (sin_a - sin_a_v) / sin_a << " " << (cos_a - cos_a_v) / cos_a << std::endl;
    std::cout.precision(ss);
    double ds = fabs(scalpha.sn - sin_a);
    if(ds > dsin){
      dsin = ds;
      dsinr = r;
    }
    double dc = fabs(scalpha.cs - cos_a);
    if(dc > dcos){
      dcos = dc;
      dcosr = r;
    }
    double dt = fabs(sin_a*sin_a + cos_a*cos_a - 1.);
    if(dt > dtrig){
      dtrig = dt;
      dtrigr = r;
    }
  }

  std::cout << "Max. difference: " << std::endl
            << "\tsin: " << dsin << " at " << dsinr << std::endl
            << "\tcos: " << dcos << " at " << dcosr << std::endl
            << "\tsin^2+cos^2: " << dtrig << " at " << dtrigr << std::endl;
#endif

#ifdef HARDDEBUG
  TVectorD y_test(dataLen);
  const Int_t nIter=10000;
  std::cout << "Perfomance test started, " << nIter << " iterations" << std::endl;

  double y_testsin[dataLen] ={};
  double y_testcos[dataLen] ={};
  struct timeval tvsincos_start, tvsincos_stop;
  gettimeofday(&tvsincos_start, 0);
  for(Int_t iIter=0;iIter<nIter;iIter++) {
    for(Int_t i=0;i<dataLen;i++) {
      sincos(parameterized_slant_angle(x[i]), &y_testsin[i], &y_testcos[i]);
    }
  }
  gettimeofday(&tvsincos_stop, 0);
  double timeSinCos=(tvsincos_stop.tv_sec-tvsincos_start.tv_sec + 1E-6*(tvsincos_stop.tv_usec-tvsincos_start.tv_usec))/nIter;

  std::cout.unsetf ( std::ios::fixed | std::ios::scientific );
  std::cout << "Time to fill 2x" << dataLen << " elements using sincos function: " << timeSinCos << std::endl;

  struct timeval tvpoly_start, tvpoly_stop;
  gettimeofday(&tvpoly_start, 0);
  for(Int_t iIter=0;iIter<nIter;iIter++) {
    for(Int_t i=0;i<dataLen;i++) {
      lwc.parameterized_sincos(x[i], y_testsin[i], y_testcos[i]);
    }
  }
  gettimeofday(&tvpoly_stop, 0);
  double timePoly=(tvpoly_stop.tv_sec - tvpoly_start.tv_sec + 1E-6*(tvpoly_stop.tv_usec - tvpoly_start.tv_usec))/nIter;
  std::cout << "Time to fill 2x" << dataLen << " elements using approximation sin&cos: " << timePoly << std::endl;
  std::cout.unsetf ( std::ios::fixed | std::ios::scientific );
  std::cout << "Approximation is " << timeSinCos/timePoly << " faster " << std::endl;
#endif

    return p;
  }

  /// Calculate (and cache) inner wheel parameterization
  const sincos_params& inner_params(LArWheelCalculator& lwc) {
    static const sincos_params p = calc_params(lwc, 250.*mm, 750.*mm);
    return p;
  }

  /// Calculate (and cache) outer wheel parameterization
  const sincos_params& outer_params(LArWheelCalculator& lwc) {
    static const sincos_params p = calc_params(lwc, 560.*mm, 2090.*mm);
    return p;
  }
}

void LArWheelCalculator::fill_sincos_parameterization()
{
  // The polynomial approximations are calculated once per side, and stored
  // as statics for reuse in successive calculator instances.
  const sincos_params& p = m_isInner ? inner_params(*this) : outer_params(*this);

  // Fill the parameters into our instances variables
  m_sin_parametrization = p.sin;
  m_cos_parametrization = p.cos;

  // Parameterization for the vectorized sincos calculation
  // see ATLASSIM-4753 for details
  // s4, s5, c4, c5
  // s2, s3, c2, c3
  // s0, s1, c0, c1
  m_vsincos_par.param_0[0] = m_sin_parametrization[4];
  m_vsincos_par.param_0[1] = m_sin_parametrization[5];
  m_vsincos_par.param_0[2] = m_cos_parametrization[4];
  m_vsincos_par.param_0[3] = m_cos_parametrization[5];
  m_vsincos_par.param_1[0] = m_sin_parametrization[2];
  m_vsincos_par.param_1[1] = m_sin_parametrization[3];
  m_vsincos_par.param_1[2] = m_cos_parametrization[2];
  m_vsincos_par.param_1[3] = m_cos_parametrization[3];
  m_vsincos_par.param_2[0] = m_sin_parametrization[0];
  m_vsincos_par.param_2[1] = m_sin_parametrization[1];
  m_vsincos_par.param_2[2] = m_cos_parametrization[0];
  m_vsincos_par.param_2[3] = m_cos_parametrization[1];
}
