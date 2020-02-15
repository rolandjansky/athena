/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

struct Component
{
  // Default ctors/dtor/assignment operators
  Component() = default;
  ~Component() = default;
  Component(const Component&) = default;
  Component& operator=(const Component&) = default;
  Component(Component&&) = default;
  Component& operator=(Component&&) = default;
  // Constructor with arguments
  Component(double aWeight, double aMean, double aSigma)
    : weight(aWeight)
    , mean(aMean)
    , sigma(aSigma)
  {}
  double weight = 0;
  double mean = 0;
  double sigma = 0;
};

//!< IMultiComponentStateModeCalculator interface method to calculate mode
std::array<double, 10>
calculateMode(const MultiComponentState&);

//!<  method to extract the weight, mean and sigma values from the multi-component state
void
fillMixture(const MultiComponentState&, std::array<std::vector<Component>, 5>& mixture);

//!<  method to find the mode using the Newton-Raphson method based on a starting guess
double
findMode(double, int, const std::array<std::vector<Component>, 5>& mixture);

//!<  method to determine the pdf of the cashed mixture at a given value
double
pdf(double, int, const std::array<std::vector<Component>, 5>& mixture);

//!<  method to determine the first order derivative of the pdf at a given value
double
d1pdf(double, int, const std::array<std::vector<Component>, 5>& mixture);

//!<  method to determine the second order derivative of the pdf at a given value
double
d2pdf(double, int, const std::array<std::vector<Component>, 5>& mixture);

//!<  method to determine the value of the a gaussian distribution at a given value
double
gaus(double x, double mean, double sigma);

double
findModeGlobal(double, int, const std::array<std::vector<Component>, 5>& mixture);

double
width(int i, const std::array<std::vector<Component>, 5>& mixture);

double
findRoot(double& result,
         double xlo,
         double xhi,
         double value,
         double i,
         const std::array<std::vector<Component>, 5>& mixture);

} // namespace MultiComponentStateModeCalculator

} // namespace Trk

#endif
