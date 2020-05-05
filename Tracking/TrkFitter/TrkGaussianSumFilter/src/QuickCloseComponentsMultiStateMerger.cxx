/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
      QuickCloseComponentsMultiStateMerger.cxx  -  description
      ---------------------------------------------------
begin                : Wednesday 3rd September 2008
author               : amorley
email                : Anthony.Morley@cern.ch
decription           : Implementation code for QuickCloseComponentsMultiStateMerger
                       class
*********************************************************************************/

#include "TrkGaussianSumFilter/QuickCloseComponentsMultiStateMerger.h"
#include "GaudiKernel/Chrono.h"
#include "TrkGaussianSumFilter/AlignedDynArray.h"
#include "TrkGaussianSumFilter/KLGaussianMixtureReduction.h"
#include "TrkGaussianSumFilter/MultiComponentStateCombiner.h"
#include "TrkParameters/TrackParameters.h"
#include <cstdint>
#include <limits>
using namespace GSFUtils;

Trk::QuickCloseComponentsMultiStateMerger::QuickCloseComponentsMultiStateMerger(const std::string& type,
                                                                                const std::string& name,
                                                                                const IInterface* parent)
  : AthAlgTool(type, name, parent)
{

  declareInterface<IMultiComponentStateMerger>(this);
}

Trk::QuickCloseComponentsMultiStateMerger::~QuickCloseComponentsMultiStateMerger() = default;

StatusCode
Trk::QuickCloseComponentsMultiStateMerger::initialize()
{
  if (m_maximumNumberOfComponents <= 0) {
    ATH_MSG_FATAL("Attempting to merge multi-state into zero components... stop being silly!");
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("Initialisation of " << type() << " under instance " << name() << " was successful");
  return StatusCode::SUCCESS;
}

StatusCode
Trk::QuickCloseComponentsMultiStateMerger::finalize()
{
  ATH_MSG_INFO("Finalisation of " << type() << " under instance " << name() << " was successful");
  return StatusCode::SUCCESS;
}

Trk::MultiComponentState
Trk::QuickCloseComponentsMultiStateMerger::merge(Trk::MultiComponentState statesToMerge) const
{
  // Assembler Cache
  MultiComponentStateAssembler::Cache cache;

  if (statesToMerge.size() <= m_maximumNumberOfComponents) {
    MultiComponentStateAssembler::addMultiState(cache, std::move(statesToMerge));
    return MultiComponentStateAssembler::assembledState(cache);
  }

  // Scan all components for covariance matrices. If one or more component
  // is missing an error matrix, component reduction is impossible.
  bool componentWithoutMeasurement = false;
  Trk::MultiComponentState::const_iterator component = statesToMerge.cbegin();
  for (; component != statesToMerge.cend(); ++component) {
    const AmgSymMatrix(5)* measuredCov = component->first->covariance();
    if (!measuredCov) {
      componentWithoutMeasurement = true;
      break;
    }
  }
  if (componentWithoutMeasurement) {
    // Sort to select the one with the largest weight
    std::sort(statesToMerge.begin(),
              statesToMerge.end(),
              [](const ComponentParameters& x, const ComponentParameters& y) { return x.second > y.second; });

    Trk::ComponentParameters dummyCompParams(statesToMerge.begin()->first->clone(), 1.);
    Trk::MultiComponentState returnMultiState;
    returnMultiState.push_back(std::move(dummyCompParams));
    return returnMultiState;
  }

  return mergeFullDistArray(cache, statesToMerge);
}

Trk::MultiComponentState
Trk::QuickCloseComponentsMultiStateMerger::mergeFullDistArray(MultiComponentStateAssembler::Cache& cache,
                                                              Trk::MultiComponentState& statesToMerge) const
{
  /*
   * Allocate, and initialize the needed arrays
   */
  const int32_t n = statesToMerge.size();
  const int32_t nn = (n + 1) * n / 2;
  const int32_t nn2 = (nn & 7) == 0 ? nn : nn + (8 - (nn & 7)); // make sure it is a multiplet of 8
  
  // Array to store all of the distances between components
  AlignedDynArray<float, alignment> distances(nn2,
                                              std::numeric_limits<float>::max()); 
  AlignedDynArray<Component1D, alignment> components(n);    // Arrayfor each component
  // Needed to convert the triangular index to (i,j)
  std::vector<triangularToIJ> convert(nn2, { -1, -1 });
  // Calculate indicies
  for (int32_t i = 0; i < n; ++i) {
    const int indexConst = (i + 1) * i / 2;
    for (int32_t j = 0; j <= i; ++j) {
      int32_t index = indexConst + j;
      convert[index].I = i;
      convert[index].J = j;
    }
  }

  // Create an array of all components to be merged
  for (int32_t i = 0; i < n; ++i) {
    const AmgSymMatrix(5)* measuredCov = statesToMerge[i].first->covariance();
    const AmgVector(5)& parameters = statesToMerge[i].first->parameters();
    // Fill in infomation
    const double cov= measuredCov ? (*measuredCov)(Trk::qOverP, Trk::qOverP) : -1.;
    components[i].mean= parameters[Trk::qOverP];
    components[i].cov = cov;
    components[i].invCov = cov > 0 ? 1./cov : 1e10;
  }
  // Calculate distances for all pairs
  // This loop can be vectorised
  calculateAllDistances(components, distances, n);

  /*
   *  Loop over all components until you reach the target amount
   */
  unsigned int numberOfComponents = n;
  int32_t minIndex = -1;
  int32_t nextMinIndex = -1;

  while (numberOfComponents > m_maximumNumberOfComponents) {

    /* 
     * Find the minimum index 
     * Do it only if we do not have a good new guess
     */
    if(nextMinIndex<1){
      minIndex = findMinimumIndex(distances, nn2);
    } else {
      minIndex = nextMinIndex;
    }
    /* reset the nextMinindex*/
    nextMinIndex=-1;
    /*Keep track of the current minimum value*/
    float currentMinValue=distances[minIndex];

    /*convert the index in an (i,j) pair*/
    int32_t mini = convert[minIndex].I;
    int32_t minj = convert[minIndex].J;
    /*
     * Combine the components to be merged
     * statesToMerge[mini] becomes the merged
     * statesToMerge[minj] is set to dummy values
     */
    MultiComponentStateCombiner::combineWithWeight(statesToMerge[mini], statesToMerge[minj]);
    statesToMerge[minj].first.reset();
    statesToMerge[minj].second = 0.;
    /*
     * set relevant distances
     */
    const AmgSymMatrix(5)* measuredCov = statesToMerge[mini].first->covariance();
    const AmgVector(5)& parameters = statesToMerge[mini].first->parameters();
    const double cov = (*measuredCov)(Trk::qOverP, Trk::qOverP);
    
    components[mini].mean= parameters[Trk::qOverP];
    components[mini].cov = cov;
    components[mini].invCov = cov > 0 ? 1./cov : 1e10;
    
    components[minj].mean = 0.;
    components[minj].cov = 0.;
    components[minj].invCov = 1e10;
    
    // re-calculate distances wrt the new component at mini
    int32_t possibleNextMin = recalculateDistances(components, distances, mini, n);
    //We might already got something smaller than the previous minimum
    //we can therefore use the new one directly
    if (possibleNextMin > 0 && distances[possibleNextMin] < currentMinValue) {
      nextMinIndex = possibleNextMin;
    }
    // Reset old weights wrt the  minj position
    resetDistances(distances, minj, n);
    // Decrement the number of components
    --numberOfComponents;
  } // end of merge while

  // Build final state containing both merged and unmerged components
  for (auto& state : statesToMerge) {
    // Avoid merge ones
    if (!state.first) {
      continue;
    }
    /*
     * Add componets to the state be prepared for assembly
     * and update the relevant weight
     */
    cache.multiComponentState.push_back(ComponentParameters(state.first.release(), state.second));
    cache.validWeightSum += state.second;
  }
  Trk::MultiComponentState mergedState = MultiComponentStateAssembler::assembledState(cache);
  // Clear the state vector
  statesToMerge.clear();
  return mergedState;
}
