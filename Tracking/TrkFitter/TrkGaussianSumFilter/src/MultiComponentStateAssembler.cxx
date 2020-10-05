/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   MultiComponentStateAssembler.cxx
 * @date   Monday 20th December 2004
 * @author Atkinson,Anthony Morley, Christos Anastopoulos
 *
 * Implementation code for MultiComponentStateAssembler
 */

#include "TrkGaussianSumFilter/MultiComponentStateAssembler.h"
#include "TrkMultiComponentStateOnSurface/ComponentParameters.h"

namespace {

using namespace Trk::MultiComponentStateAssembler;

/** @brief Helper for ordering by larger to smaller weight*/
class SortByLargerSimpleComponentWeight
{
public:
  SortByLargerSimpleComponentWeight() = default;
  bool operator()(const Trk::ComponentParameters& firstComponent,
                  const Trk::ComponentParameters& secondComponent) const
  {
    return firstComponent.second > secondComponent.second;
  }
};

/** Method to check the validity of of the cached state */
inline bool
isStateValid(const Cache& cache)
{
  return !cache.multiComponentState.empty();
}

/** Method to assemble state with correct weightings */
Trk::MultiComponentState
doStateAssembly(Cache& cache, const double newWeight)
{
  if (!isStateValid(cache)) {
    return {};
  }
  const size_t cacheSize = cache.multiComponentState.size();

  if (cache.validWeightSum <= 0.) {
    if (!cache.multiComponentState.empty()) {
      const double fixedWeights = 1. / static_cast<double>(cacheSize);
      for (auto& component : cache.multiComponentState) {
        component.second = fixedWeights;
      }
    }
    Trk::MultiComponentState assembledState;
    assembledState.reserve(cacheSize);
    for (auto& component : cache.multiComponentState) {
      assembledState.emplace_back(component.first.release(), component.second);
    }
    // Reset the cache before leaving
    reset(cache);
    return assembledState;
  }
  Trk::MultiComponentState assembledState;
  assembledState.reserve(cacheSize);
  const double scalingFactor =
    cache.validWeightSum > 0. ? newWeight / cache.validWeightSum : 1.;
  for (auto& component : cache.multiComponentState) {
    assembledState.emplace_back(component.first.release(),
                                component.second * scalingFactor);
  }
  // Reset the cache before leaving
  reset(cache);
  return assembledState;
}
}

void
Trk::MultiComponentStateAssembler::reset(Cache& cache)
{
  cache.assemblyDone = false;
  if (!cache.multiComponentState.empty()) {
    cache.multiComponentState.clear();
  }
  cache.validWeightSum = 0.;
  cache.invalidWeightSum = 0.;
}

bool
Trk::MultiComponentStateAssembler::addComponent(
  Cache& cache,
  ComponentParameters&& componentParameters)
{
  if (cache.assemblyDone) {
    return false;
  }
  cache.validWeightSum += componentParameters.second;
  cache.multiComponentState.emplace_back(componentParameters.first.release(),
                                         componentParameters.second);
  return true;
}

bool
Trk::MultiComponentStateAssembler::addMultiState(
  Cache& cache,
  Trk::MultiComponentState&& multiComponentState)
{
  if (cache.assemblyDone) {
    return false;
  }
  double sumW(0.);
  for (auto& component : multiComponentState) {
    sumW += component.second;
    cache.multiComponentState.emplace_back(component.first.release(),
                                           component.second);
  }
  multiComponentState.clear();
  cache.validWeightSum += sumW;
  return true;
}

bool
Trk::MultiComponentStateAssembler::addInvalidComponentWeight(
  Cache& cache,
  const double invalidComponentWeight)
{
  cache.invalidWeightSum += invalidComponentWeight;
  return true;
}

bool
Trk::MultiComponentStateAssembler::prepareStateForAssembly(Cache& cache)
{
  // Protect against empty state
  if (!isStateValid(cache)) {
    return false;
  }

  // Check for minimum fraction of valid states
  double den = cache.validWeightSum + cache.invalidWeightSum;
  double validWeightFraction = den > 0 ? cache.validWeightSum / den : 0;
  if (cache.invalidWeightSum > 0. &&
      validWeightFraction < cache.minimumValidFraction) {
    return false;
  }
  // Check to see assembly has not already been done
  if (cache.assemblyDone) {
    return true;
  }

  // Sort Multi-Component State by weights
  std::sort(cache.multiComponentState.begin(),
            cache.multiComponentState.end(),
            SortByLargerSimpleComponentWeight());

  double totalWeight(cache.validWeightSum + cache.invalidWeightSum);
  if (totalWeight != 0.) {

    // All elements where
    //! comp,!(value>element),element>=value is true
    // are before the value (i.e ordered descending).
    // return the 1st element where (element<value)
    ComponentParameters dummySmallestWeight(
      nullptr, cache.minimumFractionalWeight * totalWeight);
    auto lower_than = std::upper_bound(cache.multiComponentState.begin(),
                                       cache.multiComponentState.end(),
                                       dummySmallestWeight,
                                       SortByLargerSimpleComponentWeight());

    // reverse iterate , so as to delete removing the last
    auto lower_than_reverse = std::make_reverse_iterator(lower_than);
    for (auto itr = cache.multiComponentState.rbegin();
         itr != lower_than_reverse;
         ++itr) {
      cache.multiComponentState.erase(itr.base() - 1);
    }
  }
  // Now recheck to make sure the state is now still valid
  if (!isStateValid(cache)) {
    return false;
  }
  // Set assembly flag
  cache.assemblyDone = true;
  return true;
}

Trk::MultiComponentState
Trk::MultiComponentStateAssembler::assembledState(Cache& cache)
{
  if (!prepareStateForAssembly(cache)) {
    return {};
  }
  if (cache.invalidWeightSum > 0. || cache.validWeightSum <= 0.) {
    const double totalWeight = cache.validWeightSum + cache.invalidWeightSum;
    return doStateAssembly(cache, totalWeight);
  }
  return doStateAssembly(cache, cache.validWeightSum);
}

Trk::MultiComponentState
Trk::MultiComponentStateAssembler::assembledState(Cache& cache,
                                                  const double newWeight)
{
  if (!prepareStateForAssembly(cache)) {
    return {};
  }
  return doStateAssembly(cache, newWeight);
}
