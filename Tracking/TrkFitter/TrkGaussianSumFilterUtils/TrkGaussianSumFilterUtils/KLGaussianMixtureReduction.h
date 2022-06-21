/*
  Copyright (C) 2002-20222 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  KLGaussianMixtureReduction.h
 * @author Anthony Morley , Christos Anastopoulos
 * @date 26th November 2019
 *
 *
 * @brief Utilities to facilitate the calculation of the
 * KL divergence/distance between components of the mixture
 * and the merging of similar componets together.
 */

#ifndef KLGaussianMixReductionUtils_H
#define KLGaussianMixReductionUtils_H

#include "TrkGaussianSumFilterUtils/GsfConstants.h"
//
#include <array>
#include <cstdint>

namespace GSFUtils {

/**
 * @brief struct representing 1D component
 */
struct Component1D
{
  double mean = 0.;
  double cov = 0.;
  double invCov = 1e10;
  double weight = 0.;
};
/**
 * @brief struct representing an array of 1D component.
 * with the maximum size we can have after convolution
 * with material effects.
 */
struct Component1DArray
{
  alignas(GSFConstants::alignment)
    std::array<Component1D,
               GSFConstants::maxComponentsAfterConvolution> components{};
  int32_t numComponents = 0;
};
/**
 * @brief struct representing an array or the merges.
 * The merge is from the element in positon 'From'
 * to the element in position 'To'
 */
struct MergeArray
{
  struct merge
  {
    int8_t To = 0;
    int8_t From = 0;
  };
  std::array<merge, GSFConstants::maxComponentsAfterConvolution> merges{};
  int32_t numMerges = 0;
};

/**
 * @brief Find the order in which the components need to
 * be merged. Returns an MergeArray  with the merges
 * (To,From).
 * The index of the merged From is always smaller than
 * the To (RHS is smaller than LHS)
 *
 * @c Component1DArray : Array of simplified 1D components
 * used to calculate the merge order using q/p. Its size
 * can not exceed GSFConstants::maxComponentsAfterConvolution
 *
 * @c reducedSize  The size we want to reduce the mixture to.
 * Needs to be smaller than the numComponents of the componentsIn
 * array
 */
MergeArray
findMerges(const Component1DArray& componentsIn, const int8_t reducedSize);

} // namespace KLGaussianMixtureReduction

#endif
