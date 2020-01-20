/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/***********************************************************************************
      MultiComponentStateModeCalculator.h  -  description
      ---------------------------------------------------
begin                : Thursday 6th July 2006
author               : atkinson
email                : Tom.Atkinson@cern.ch
description          : Class to calculate the mode (q/p) of a gaussian mixtureArray
***********************************************************************************/

#ifndef Trk_MultiComponentStateModeCalculator_H
#define Trk_MultiComponentStateModeCalculator_H

#include "TrkMultiComponentStateOnSurface/MultiComponentState.h"
#include <array>
#include <vector>

namespace Trk {
namespace MultiComponentStateModeCalculator {

struct Mixture {
  // Default ctors/dtor/assignment operators
  Mixture() = default;
  ~Mixture() = default;
  Mixture(const Mixture &) = default;
  Mixture &operator=(const Mixture &) = default;
  Mixture(Mixture &&) = default;
  Mixture &operator=(Mixture &&) = default;
  // Constructor with arguments
  Mixture(double aWeight, double aMean, double aSigma)
      : weight(aWeight), mean(aMean), sigma(aSigma) {}
  double weight = 0;
  double mean = 0;
  double sigma = 0;
};

//!< IMultiComponentStateModeCalculator interface method to calculate mode
std::array<double,10> calculateMode(const MultiComponentState &);

//!< Private method to extract the weight, mean and sigma values from the multi-component state
void fillMixture(const MultiComponentState &, std::array<std::vector<Mixture>, 5> &mixture);

//!< Private method to find the mode using the Newton-Raphson method based on a starting guess
double findMode(double, int, const std::array<std::vector<Mixture>, 5> &mixture);

//!< Private method to determine the pdf of the cashed mixture at a given value
double pdf(double, int, const std::array<std::vector<Mixture>, 5> &mixture);

//!< Private method to determine the first order derivative of the pdf at a given value
double d1pdf(double, int, const std::array<std::vector<Mixture>, 5> &mixture);

//!< Private method to determine the second order derivative of the pdf at a given value
double d2pdf(double, int, const std::array<std::vector<Mixture>, 5> &mixture);

//!< Private method to determine the value of the a gaussian distribution at a given value
double gaus(double x, double mean, double sigma);

double findModeGlobal(double, int, const std::array<std::vector<Mixture>, 5> &mixture);

double width(int i, const std::array<std::vector<Mixture>, 5> &mixture);

double findRoot(double &result, double xlo, double xhi, double value, double i,
                const std::array<std::vector<Mixture>, 5> &mixture);

} // namespace MultiComponentStateModeCalculator

} // namespace Trk

#endif
