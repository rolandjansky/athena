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

// Simple representation of 1D component
struct Component
{
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

/** @brief  Method to calculate mode with MultiComponentState
 * state as input */
std::array<double, 10>
calculateMode(const MultiComponentState&);

/** @brief  Method to calculate mode*/
std::array<double, 10>
calculateMode(const std::array<std::vector<Component>, 5>& mixture);

} // namespace MultiComponentStateModeCalculator

} // namespace Trk

#endif
