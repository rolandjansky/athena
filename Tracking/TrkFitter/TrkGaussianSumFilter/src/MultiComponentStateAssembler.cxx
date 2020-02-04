/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
                        MultiComponentStateAssembler.cxx  -  description
                        ----------------------------------------------------
begin                : Monday 20th December 2004
author               : atkinson,morley,anastopoulos
email                : Tom.Atkinson@cern.ch
description          : Implementation code for MultiComponentStateAssembler
                        class.
*********************************************************************************/

#include "TrkGaussianSumFilter/MultiComponentStateAssembler.h"
#include "TrkMultiComponentStateOnSurface/ComponentParameters.h"

namespace {
class SortByLargerSimpleComponentWeight
{
public:
  SortByLargerSimpleComponentWeight() = default;
  bool operator()(const Trk::ComponentParameters& firstComponent, const Trk::ComponentParameters& secondComponent) const

  {
    return firstComponent.second > secondComponent.second;
  }
};
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
Trk::MultiComponentStateAssembler::addComponent(Cache& cache, ComponentParameters&& componentParameters)
{
  if (cache.assemblyDone) {
    return false;
  }
  cache.validWeightSum += componentParameters.second;
  cache.multiComponentState.emplace_back(componentParameters.first.release(), componentParameters.second);
  return true;
}

bool
Trk::MultiComponentStateAssembler::addMultiState(Cache& cache, Trk::MultiComponentState&& multiComponentState)
{
  if (cache.assemblyDone) {
    return false;
  }
  double sumW(0.);
  for (auto& component : multiComponentState) {
    sumW += component.second;
    cache.multiComponentState.emplace_back(component.first.release(), component.second);
  }
  multiComponentState.clear();
  cache.validWeightSum += sumW;
  return true;
}

bool
Trk::MultiComponentStateAssembler::addInvalidComponentWeight(Cache& cache, const double invalidComponentWeight)
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
  if (cache.invalidWeightSum > 0. && validWeightFraction < cache.minimumValidFraction) {
    return false;
  }
  // Check to see assembly has not already been done
  if (cache.assemblyDone) {
    return true;
  }

  /*
   * Sort Multi-Component State by weights
   */
  std::sort(cache.multiComponentState.begin(), cache.multiComponentState.end(), SortByLargerSimpleComponentWeight());

  double totalWeight(cache.validWeightSum + cache.invalidWeightSum);
  if (totalWeight != 0.) {
    /*
     * All elements where
     * !comp,!(value>element),element>=value is true
     * are before the value (i.e ordered descending).
     * return the 1st element where (element<value)
     */
    ComponentParameters dummySmallestWeight(nullptr, cache.minimumFractionalWeight * totalWeight);
    auto lower_than = std::upper_bound(cache.multiComponentState.begin(),
                                       cache.multiComponentState.end(),
                                       dummySmallestWeight,
                                       SortByLargerSimpleComponentWeight());
    /*
     * reverse iterate , so as to delete removing the last
     */
    auto lower_than_reverse = std::make_reverse_iterator(lower_than);
    for (auto itr = cache.multiComponentState.rbegin(); itr != lower_than_reverse; ++itr) {
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

std::unique_ptr<Trk::MultiComponentState>
Trk::MultiComponentStateAssembler::assembledState(Cache& cache)
{
  if (!prepareStateForAssembly(cache)) {
    return nullptr;
  }
  if (cache.invalidWeightSum > 0. || cache.validWeightSum <= 0.) {
    double totalWeight = cache.validWeightSum + cache.invalidWeightSum;
    std::unique_ptr<Trk::MultiComponentState> stateAssembly = doStateAssembly(cache, totalWeight);
    return stateAssembly;
  }
  return doStateAssembly(cache, cache.validWeightSum);
}

std::unique_ptr<Trk::MultiComponentState>
Trk::MultiComponentStateAssembler::assembledState(Cache& cache, const double newWeight)
{
  if (!prepareStateForAssembly(cache)) {
    return nullptr;
  }
  return doStateAssembly(cache, newWeight);
}

std::unique_ptr<Trk::MultiComponentState>
Trk::MultiComponentStateAssembler::doStateAssembly(Cache& cache, const double newWeight)
{
  if (!isStateValid(cache)) {
    return nullptr;
  }
  if (cache.validWeightSum <= 0.) {
    if (!cache.multiComponentState.empty()) {
      double fixedWeights = 1. / (double)cache.multiComponentState.size();
      for (auto& component : cache.multiComponentState) {
        component.second = fixedWeights;
      }
    }
    std::unique_ptr<Trk::MultiComponentState> assembledState = std::make_unique<Trk::MultiComponentState>();
    for (auto& component : cache.multiComponentState) {
      assembledState->emplace_back(component.first.release(), component.second);
    }
    // Reset the cache before leaving
    reset(cache);
    return assembledState;
  }

  std::unique_ptr<Trk::MultiComponentState> assembledState = std::make_unique<Trk::MultiComponentState>();
  double scalingFactor = cache.validWeightSum > 0. ? newWeight / cache.validWeightSum : 1.;
  for (auto& component : cache.multiComponentState) {
    assembledState->emplace_back(component.first.release(), component.second * scalingFactor);
  }

  // Reset the cache before leaving
  reset(cache);

  return assembledState;
}
