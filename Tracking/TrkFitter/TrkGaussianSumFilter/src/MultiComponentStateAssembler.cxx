/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
                        MultiComponentStateAssembler.cxx  -  description
                        ----------------------------------------------------
begin                : Monday 20th December 2004
author               : atkinson
email                : Tom.Atkinson@cern.ch
description          : Implementation code for MultiComponentStateAssembler
                        class.
*********************************************************************************/

#include "TrkGaussianSumFilter/MultiComponentStateAssembler.h"
#include "TrkGaussianSumFilter/SortingClasses.h"
#include "TrkParameters/TrackParameters.h"

Trk::MultiComponentStateAssembler::MultiComponentStateAssembler(const std::string& type,
                                                                const std::string& name,
                                                                const IInterface* parent)
  : AthAlgTool(type, name, parent)
{
  declareInterface<IMultiComponentStateAssembler>(this);
}

Trk::MultiComponentStateAssembler::~MultiComponentStateAssembler() {}

StatusCode
Trk::MultiComponentStateAssembler::initialize()
{
  ATH_MSG_DEBUG("Initialisation of " << name() << " successful \n");
  return StatusCode::SUCCESS;
}

StatusCode
Trk::MultiComponentStateAssembler::finalize()
{
  ATH_MSG_DEBUG("Finalisation of " << name() << " successful \n");
  return StatusCode::SUCCESS;
}

void
Trk::MultiComponentStateAssembler::reset(Cache& cache) const
{
  ATH_MSG_VERBOSE("Resetting the MultiComponentStateAssembler: " << name() << "\n");
  cache.assemblyDone = false;
  if (!cache.multiComponentState.empty()) {
    cache.multiComponentState.clear();
  }
  cache.validWeightSum = 0.;
  cache.invalidWeightSum = 0.;
}

void
Trk::MultiComponentStateAssembler::status(const Cache& cache) const
{
  ATH_MSG_DEBUG("**************** Current status of assembler: "
                << name() << " **************** \n "
                << "Number of components in cached state: " << cache.multiComponentState.size() << "\n"
                << "Assembly done? (Bool): " << cache.assemblyDone << "\n"
                << "Total valid weight:    " << cache.validWeightSum << "\n"
                << "Total invalid weight:  " << cache.invalidWeightSum << "\n"
                << "**************** End assembler status **************** \n");
}

bool
Trk::MultiComponentStateAssembler::addComponent(Cache& cache, SimpleComponentParameters&& componentParameters) const
{
  if (cache.assemblyDone) {
    ATH_MSG_WARNING("Trying to add state after assembly... returning false \n");
    return false;
  }
  
  cache.validWeightSum += componentParameters.second; 
  cache.multiComponentState.emplace_back(componentParameters.first.release(),componentParameters.second);
  return true;
}

bool
Trk::MultiComponentStateAssembler::addMultiState(Cache& cache, SimpleMultiComponentState&& multiComponentState) const
{
  if (cache.assemblyDone) {
    ATH_MSG_WARNING("Trying to add state after assembly... returning false \n");
    return false;
  }
  double sumW(0.);
  for (auto& component : multiComponentState) {
    sumW += component.second;
    cache.multiComponentState.emplace_back(component.first.release(),component.second);
  }
  multiComponentState.clear();
  cache.validWeightSum += sumW;
  return true;
}

bool
Trk::MultiComponentStateAssembler::addInvalidComponentWeight(Cache& cache, const double invalidComponentWeight) const
{
  cache.invalidWeightSum += invalidComponentWeight;
  return true;
}

bool
Trk::MultiComponentStateAssembler::prepareStateForAssembly(Cache& cache) const
{
  // Protect against empty state
  if (!isStateValid(cache)) {
    ATH_MSG_DEBUG("State is not valid... returning false \n");
    return false;
  }

  // Check for minimum fraction of valid states
  double den = cache.validWeightSum + cache.invalidWeightSum;
  double validWeightFraction = den > 0 ? cache.validWeightSum / den : 0;
  if (cache.invalidWeightSum > 0. && validWeightFraction < m_minimumValidFraction) {
    ATH_MSG_DEBUG("Insufficient valid states in the state... returning false \n");
    return false;
  }
  // Check to see assembly has not already been done
  if (cache.assemblyDone) {
    ATH_MSG_DEBUG("Assembly of state already complete... returning true \n");
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
    SimpleComponentParameters dummySmallestWeight(nullptr, m_minimumFractionalWeight * totalWeight);
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
    ATH_MSG_DEBUG("After removal of small weights, state is invalid... returning false \n");
    return false;
  }
  // Set assembly flag
  cache.assemblyDone = true;
  return true;
}

std::unique_ptr<Trk::MultiComponentState>
Trk::MultiComponentStateAssembler::assembledState(Cache& cache) const
{

  ATH_MSG_DEBUG("Finalising assembly... no specified reweighting \n");
  if (!prepareStateForAssembly(cache)) {
    ATH_MSG_DEBUG("Unable to prepare state for assembly... returning 0 \n");
    return 0;
  }
  if (cache.invalidWeightSum > 0. || cache.validWeightSum <= 0.) {
    double totalWeight = cache.validWeightSum + cache.invalidWeightSum;
    std::unique_ptr<Trk::MultiComponentState> stateAssembly = doStateAssembly(cache, totalWeight);
    return stateAssembly;
  }
  return doStateAssembly(cache, cache.validWeightSum);
}

std::unique_ptr<Trk::MultiComponentState>
Trk::MultiComponentStateAssembler::assembledState(Cache& cache, const double newWeight) const
{

  ATH_MSG_DEBUG("Finalising assembly with reweighting of components \n");

  if (!prepareStateForAssembly(cache)) {
    ATH_MSG_DEBUG("Unable to prepare state for assembly... returing nullptr \n");
    return nullptr;
  }
  return doStateAssembly(cache, newWeight);
}

std::unique_ptr<Trk::MultiComponentState>
Trk::MultiComponentStateAssembler::doStateAssembly(Cache& cache, const double newWeight) const
{
  if (!isStateValid(cache)) {
    ATH_MSG_DEBUG("Cached state is empty... returning 0 \n");
    return nullptr;
  }
  if (cache.validWeightSum <= 0.) {
    ATH_MSG_DEBUG("Cached state has a invalid weight -- setting all weights equal : " << cache.validWeightSum);
    if (!cache.multiComponentState.empty()) {
      double fixedWeights = 1. / (double)cache.multiComponentState.size();
      for (auto& component : cache.multiComponentState) {
        component.second = fixedWeights;
      }
    }
    std::unique_ptr<Trk::MultiComponentState> assembledState = std::make_unique<Trk::MultiComponentState>();
    for (auto& component : cache.multiComponentState) {
      Trk::ComponentParameters cp(component.first.release(), component.second);
      assembledState->push_back(cp);
    }
    // Reset the cache before leaving
    this->reset(cache);
    ATH_MSG_DEBUG("Assembeled state created with size: " << assembledState->size());
    return assembledState;
  }

  std::unique_ptr<Trk::MultiComponentState> assembledState = std::make_unique<Trk::MultiComponentState>();
  double scalingFactor = cache.validWeightSum > 0. ? newWeight / cache.validWeightSum : 1.;
  for (auto& component : cache.multiComponentState) {
    Trk::ComponentParameters cp(component.first.release(), component.second * scalingFactor);
    assembledState->push_back(cp);
  }

  // Reset the cache before leaving
  this->reset(cache);

  ATH_MSG_DEBUG("Assembeled state created with size: " << assembledState->size());

  return assembledState;
}
