/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   MultiComponentStateModeCalculator.h
 * @date   Thursday 6th July 2006
 * @author Atkinson,Anthony Morley, Christos Anastopoulos
 *
 *  Calculate the mode of the gaussian mixture and the relevant
 *  uncertainties
 */

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

/** @brief  Method to calculate mode*/
std::array<double, 10>
calculateMode(const MultiComponentState&);

/**
 * method to extract the weight, mean and sigma values from the
  multi-component state*/
void
fillMixture(const MultiComponentState&,
            std::array<std::vector<Component>, 5>& mixture);

/**
 * method to find the mode using the Newton-Raphson method based on a
 *   starting guess
 */
double
findMode(double, int, const std::array<std::vector<Component>, 5>& mixture);

double
findModeGlobal(double,
               int,
               const std::array<std::vector<Component>, 5>& mixture);

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
