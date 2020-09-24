/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   MultiComponentStateModeCalculator.cxx
 * @date   Thursday 6th July 2006
 * @author Atkinson,Anthony Morley, Christos Anastopoulos
 *
 * Implementation code for MultiComponentStateModeCalculator
 */

#include "TrkGaussianSumFilter/MultiComponentStateModeCalculator.h"
#include "CxxUtils/phihelper.h"
#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"
#include <cmath>

namespace {
constexpr double invsqrt2PI =
  M_2_SQRTPI / (2. * M_SQRT2); // 1. / sqrt(2. * M_PI);

using namespace Trk::MultiComponentStateModeCalculator;

/** bried method to determine the value of the a gaussian distribution at a
 * given value */
double
gaus(double x, double mean, double sigma)
{
  // gauss = 1/(sigma * sqrt(2*pi)) * exp  ( -0.5 * ((x-mean)/sigma)^2 )
  // = (1/sqrt(2*pi))* (1/sigma)  * exp  (-0.5 * ((x-mean)*(1/sigma)) *
  // ((x-mean)*(1/sigma)) )
  //= invsqrt2PI * invertsigma * exp (-0.5 *z * z)
  const double invertsigma = 1. / sigma;
  const double z = (x - mean) * invertsigma;
  return (invsqrt2PI * invertsigma) * exp(-0.5 * z * z);
}

/** @brief method to determine the pdf of the cashed mixture at a given value*/
double
pdf(double x, int i, const std::array<std::vector<Component>, 5>& mixture)
{
  double pdf(0.);
  auto component = mixture[i].begin();
  for (; component != mixture[i].end(); ++component) {
    pdf += component->weight * gaus(x, component->mean, component->sigma);
  }
  return pdf;
}

/** @brief method to determine the first order derivative of the pdf at a given
 * value*/
double
d1pdf(double x, int i, const std::array<std::vector<Component>, 5>& mixture)
{

  double result(0.);
  auto component = mixture[i].begin();
  for (; component != mixture[i].end(); ++component) {
    double z = (x - component->mean) / component->sigma;
    result += -1. * component->weight * z *
              gaus(x, component->mean, component->sigma) / component->sigma;
  }
  return result;
}
/** @brief method to determine the second order derivative of the pdf at a given
 * value*/
double
d2pdf(double x, int i, const std::array<std::vector<Component>, 5>& mixture)
{

  double result(0.);
  auto component = mixture[i].begin();
  for (; component != mixture[i].end(); ++component) {
    double z = (x - component->mean) / component->sigma;
    result += component->weight / (component->sigma * component->sigma) *
              (z * z - 1.) * gaus(x, component->mean, component->sigma);
  }
  return result;
}

/** bried method to determine the width of the a gaussian distribution at a
 * given value */
double
width(int i, const std::array<std::vector<Component>, 5>& mixture)
{
  double pdf(0.);
  auto component = mixture[i].begin();
  for (; component != mixture[i].end(); ++component) {
    pdf += component->weight * component->sigma;
  }
  return pdf;
}

void
fillMixture(const Trk::MultiComponentState& multiComponentState,
            std::array<std::vector<Component>, 5>& mixture)
{
  constexpr Trk::ParamDefs parameter[5] = {
    Trk::d0, Trk::z0, Trk::phi, Trk::theta, Trk::qOverP
  };

  const size_t componentsNum = multiComponentState.size();
  for (size_t i = 0; i < 5; ++i) {
    mixture[i].clear();
    mixture[i].reserve(componentsNum);
  }

  // Loop over all the components in the multi-component state
  for (const Trk::ComponentParameters& component : multiComponentState) {

    // And then for each component over each 5 parameters
    for (size_t i = 0; i < 5; ++i) {

      const Trk::TrackParameters* componentParameters = component.first.get();
      const AmgSymMatrix(5)* measuredCov = componentParameters->covariance();
      if (!measuredCov) {
        return;
      }
      // Enums for Perigee //
      // d0=0, z0=1, phi0=2, theta=3, qOverP=4,
      double weight = component.second;
      double mean = componentParameters->parameters()[parameter[i]];
      // FIXME ATLASRECTS-598 this std::abs() should not be necessary... for
      // some reason cov(qOverP,qOverP) can be negative
      double sigma = sqrt(std::abs((*measuredCov)(parameter[i], parameter[i])));
      // Ensure that we don't have any problems with the cyclical nature of
      // phi Use first state as reference point
      if (i == 2) { // phi
        double deltaPhi =
          multiComponentState.begin()->first->parameters()[2] - mean;
        if (deltaPhi > M_PI) {
          mean += 2 * M_PI;
        } else if (deltaPhi < -M_PI) {
          mean -= 2 * M_PI;
        }
      }
      mixture[i].emplace_back(weight, mean, sigma);
    }

  }
}

double
findMode(double xStart,
         int i,
         const std::array<std::vector<Component>, 5>& mixture)
{

  int iteration(0);

  double tolerance(1.);

  double mode(xStart);

  while (iteration < 20 && tolerance > 1.e-8) {

    double previousMode(mode);
    double d2pdfVal = d2pdf(previousMode, i, mixture);

    if (d2pdfVal != 0.0) {
      mode = previousMode - d1pdf(previousMode, i, mixture) / d2pdfVal;
    } else {
      return xStart;
    }

    double pdfMode = pdf(mode, i, mixture);
    double pdfPreviousMode = pdf(previousMode, i, mixture);

    if ((pdfMode + pdfPreviousMode) != 0.0) {
      tolerance =
        std::abs(pdfMode - pdfPreviousMode) / (pdfMode + pdfPreviousMode);
    } else {
      return xStart;
    }

    ++iteration;
  }

  if (iteration >= 20) {
    return xStart;
  }

  return mode;
}

double
findRoot(double& result,
         double xlo,
         double xhi,
         double value,
         double i,
         const std::array<std::vector<Component>, 5>& mixture)
{
  // Do the root finding using the Brent-Decker method. Returns a boolean
  // status and loads 'result' with our best guess at the root if true. Prints
  // a warning if the initial interval does not bracket a single root or if
  // the root is not found after a fixed number of iterations.

  double a(xlo);
  double b(xhi);
  double fa = pdf(a, i, mixture) - value;
  double fb = pdf(b, i, mixture) - value;

  if (fb * fa > 0) {
    return false;
  }

  bool ac_equal(false);
  double fc = fb;
  double c(0);
  double d(0);
  double e(0);
  int MaxIterations = 20;
  double tolerance = 1.e-6;

  for (int iter = 0; iter <= MaxIterations; iter++) {

    if ((fb < 0 && fc < 0) || (fb > 0 && fc > 0)) {
      // Rename a,b,c and adjust bounding interval d
      ac_equal = true;
      c = a;
      fc = fa;
      d = b - a;
      e = b - a;
    }

    if (std::abs(fc) < std::abs(fb)) {
      ac_equal = true;
      a = b;
      b = c;
      c = a;
      fa = fb;
      fb = fc;
      fc = fa;
    }

    double tol = 0.5 * tolerance * std::abs(b);
    double m = 0.5 * (c - b);

    if (fb == 0 || std::abs(m) <= tol) {
      result = b;
      return true;
    }

    if (std::abs(e) < tol || std::abs(fa) <= std::abs(fb)) {
      // Bounds decreasing too slowly: use bisection
      d = m;
      e = m;
    } else {
      // Attempt inverse cubic interpolation
      double p;
      double q;
      double r;
      double s = fb / fa;

      if (ac_equal) {
        p = 2 * m * s;
        q = 1 - s;
      } else {
        q = fa / fc;
        r = fb / fc;
        p = s * (2 * m * q * (q - r) - (b - a) * (r - 1));
        q = (q - 1) * (r - 1) * (s - 1);
      }
      // Check whether we are in bounds
      if (p > 0) {
        q = -q;
      } else {
        p = -p;
      }

      double min1 = 3 * m * q - std::abs(tol * q);
      double min2 = std::abs(e * q);
      if (2 * p < (min1 < min2 ? min1 : min2)) {
        // Accept the interpolation
        e = d;
        d = p / q;
      } else {
        // Interpolation failed: use bisection.
        d = m;
        e = m;
      }
    }
    // Move last best guess to a
    a = b;
    fa = fb;
    // Evaluate new trial root
    if (std::abs(d) > tol) {
      b += d;
    } else {
      b += (m > 0 ? +tol : -tol);
    }
    fb = pdf(b, i, mixture) - value;
  }
  // Return our best guess if we run out of iterations
  result = b;

  return false;
}

} // end of anonymous namespace

std::array<double, 10>
Trk::MultiComponentStateModeCalculator::calculateMode(
  const Trk::MultiComponentState& multiComponentState)
{
  // Check to see if the multi-component state is measured
  if (!MultiComponentStateHelpers::isMeasured(multiComponentState)) {
    return {};
  }

  std::array<std::vector<Component>, 5> mixture;

  fillMixture(multiComponentState, mixture);
  return calculateMode(mixture);
}

std::array<double, 10>
Trk::MultiComponentStateModeCalculator::calculateMode(
  const std::array<std::vector<Component>, 5>& mixture)
{
  std::array<double, 10> modes{};
  /* loop over the 5 direction , d0,z0,phi,theta,qOverP*/

  for (int i = 0; i < 5; i++) {

    double largerPdfComponent = 0.0;
    double largerMeanComponent = 0.0;
    /*
     * Loop over the mixture in the ith direction and find the  component
     * whose mean give the larger value for the Gaussian Mixture pdf.
     * This should be a good enough starting point for the mode
     * finding in this direction
     */
    for (const Component& component : mixture[i]) {
      double pdfValue = pdf(component.mean, i, mixture);
      if (pdfValue > largerPdfComponent) {
        largerPdfComponent = pdfValue;
        largerMeanComponent = component.mean;
      }
    }
    modes[i] = findMode(largerMeanComponent, i, mixture);
    // Calculate the FWHM and return this back so that it can be used to correct
    // the covariance matrix
    if (largerMeanComponent != modes[i]) {
      // mode calculation was successful now calulate FWHM
      double currentWidth = width(i, mixture);
      modes[i + 5] = -1; // Failure is flagged with a value less than 0;

      double pdfVal = pdf(modes[i], i, mixture);
      double highX(0);
      double lowX(0);

      double upperbound = modes[i] + 1.5 * currentWidth;
      while (true) {
        if (pdf(upperbound, i, mixture) > pdfVal * 0.5) {
          upperbound += currentWidth;
        } else {
          break;
        }
      }

      bool highXFound =
        findRoot(highX, modes[i], upperbound, pdfVal * 0.5, i, mixture);

      double lowerbound = modes[i] - 1.5 * currentWidth;
      while (true) {
        if (pdf(lowerbound, i, mixture) > pdfVal * 0.5) {
          lowerbound -= currentWidth;
        } else {
          break;
        }
      }
      bool lowXFound =
        findRoot(lowX, lowerbound, modes[i], pdfVal * 0.5, i, mixture);
      if (highXFound && lowXFound) {
        double FWHM = highX - lowX;
        modes[i + 5] = FWHM / 2.35482; // 2 * sqrt( 2* log(2))
      }
      // Ensure that phi is between -pi and pi
      if (i == 2) {
        modes[i] = CxxUtils::wrapToPi(modes[i]);
      }
    }
  }
  return modes;
}
