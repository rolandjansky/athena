/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkGaussianSumFilter/KLGaussianMixtureReduction.h"
#include "CxxUtils/features.h"
#include "CxxUtils/vec.h"
#include "TrkGaussianSumFilter/AlignedDynArray.h"
#include <limits>
#include <stdexcept>

#if !defined(__GNUC__)
#define __builtin_assume_aligned(X, N) X
#else
#if defined(__clang__)
#if !__has_builtin(__builtin_assume_aligned)
#define __builtin_assume_aligned(X, N) X
#endif
#endif
#endif

/**
 * @file  KLGaussianMixtureReduction.cxx
 * @author Anthony Morley , Christos Anastopoulos
 * @date 26th November 2019
 *
 * Implementation of KLGaussianMixtureReduction
 */

namespace {
using namespace GSFUtils;

/**
 * @brief Helper struct to map position in
 * triangular array to I, J indices
 */
struct triangularToIJ
{
  int16_t I = -1;
  int16_t J = -1;
};

std::vector<triangularToIJ>
createToIJ128()
{
  // Create a trangular array mapping for the maximum size
  // we will ever have 8 (max bethe heitle material) x16 (state components)
  // =128. 128 * (128-1)/2 = 8128
  //
  // The typical number we use is 6x12 = 72.
  //
  constexpr int16_t n = 128;
  constexpr int32_t nn = n * (n - 1) / 2;
  std::vector<triangularToIJ> indexMap(nn);
  for (int16_t i = 1; i < n; ++i) {
    const int32_t indexConst = (i - 1) * i / 2;
    for (int16_t j = 0; j < i; ++j) {
      indexMap[indexConst + j] = { i, j };
    }
  }
  return indexMap;
}
/**
 * Based on
 * https://www.sciencedirect.com/science/article/pii/089812218990103X
 * equation (16)
 * or
 * https://arxiv.org/pdf/2001.00727.pdf
 * equation (10)
 * but not accounting for weights
 * covI * invCovJ + covJ * invCovI + (mean1-mean2) (invcov+invcov) (mean1-mean2)
 */
float
symmetricKL(const Component1D& componentI, const Component1D& componentJ)
{
  const double meanDifference = componentI.mean - componentJ.mean;
  const double inverCovSum = componentI.invCov + componentJ.invCov;
  const double term1 = componentI.invCov * componentJ.cov;
  const double term2 = componentJ.invCov * componentI.cov;
  const double term3 = meanDifference * inverCovSum * meanDifference;
  return term1 + term2 + term3;
}

/**
 * Moment-preserving merge of two 1D components
 * for example see
 * Runnalls, Andrew R.(2007)
 * Kullback-Leibler approach to Gaussian mixture reduction
 * equations (2),(3),(4)
 */
void
combine(GSFUtils::Component1D& updated, GSFUtils::Component1D& removed)
{

  const double sumWeight = updated.weight + removed.weight;

  const double invSumWeight = 1. / sumWeight;
  const double weightI_IJ = updated.weight * invSumWeight;
  const double weightJ_IJ = removed.weight * invSumWeight;
  const double meanDiff = (updated.mean - removed.mean);

  const double sumMean = weightI_IJ * updated.mean + weightJ_IJ * removed.mean;

  const double sumVariance = weightI_IJ * updated.cov +
                             weightJ_IJ * removed.cov +
                             weightI_IJ * weightJ_IJ * meanDiff * meanDiff;

  updated.mean = sumMean;
  updated.cov = sumVariance;
  updated.invCov = 1. / sumVariance;
  updated.weight = sumWeight;

  // large numbers to enter the multiplications/sums
  // make distance large
  removed.mean = std::numeric_limits<float>::max();
  removed.cov = std::numeric_limits<float>::max();
  removed.invCov = std::numeric_limits<float>::max();
  removed.weight = -1;
}

/**
 * Recalculate the distances given a merged input
 * and return the minimum index/distance wrt to this
 * new component
 */
void
recalculateDistances(const Component1D* componentsIn,
                     float* distancesIn,
                     const int32_t mini,
                     const int32_t n)
{
  const Component1D* components = static_cast<const Component1D*>(
    __builtin_assume_aligned(componentsIn, alignment));

  float* distances =
    static_cast<float*>(__builtin_assume_aligned(distancesIn, alignment));

  const int32_t j = mini;
  const int32_t indexConst = (j - 1) * j / 2;
  // This is the component that has been updated
  // so we calculate distances wrt.
  const Component1D componentJ = components[j];

  // Rows
  for (int32_t i = 0; i < j; ++i) {
    const Component1D componentI = components[i];
    const int32_t index = indexConst + i;
    // if the component has been merged already
    // keep the distance wrt to it max always
    distances[index] = componentI.weight < 0
                         ? std::numeric_limits<float>::max()
                         : symmetricKL(componentI, componentJ);
  }
  // Columns
  for (int32_t i = j + 1; i < n; ++i) {
    const Component1D componentI = components[i];
    const int32_t index = (i - 1) * i / 2 + j;
    // if the component has been merged already
    // keep the distance wrt to it max always
    distances[index] = componentI.weight < 0
                         ? std::numeric_limits<float>::max()
                         : symmetricKL(componentI, componentJ);
  }
}

/**
 * Calculate the distances for all component pairs
 */
void
calculateAllDistances(const Component1D* componentsIn,
                      float* distancesIn,
                      const int32_t n)
{

  const Component1D* components = static_cast<const Component1D*>(
    __builtin_assume_aligned(componentsIn, alignment));
  float* distances =
    static_cast<float*>(__builtin_assume_aligned(distancesIn, alignment));

  for (int32_t i = 1; i < n; ++i) {
    const int32_t indexConst = (i - 1) * i / 2;
    const Component1D componentI = components[i];
    for (int32_t j = 0; j < i; ++j) {
      const Component1D componentJ = components[j];
      distances[indexConst + j] = symmetricKL(componentI, componentJ);
    }
  }
}

/**
 * Reset the distances wrt to a mini index
 */
void
resetDistances(float* distancesIn, const int32_t minj, const int32_t n)
{
  float* distances =
    static_cast<float*>(__builtin_assume_aligned(distancesIn, alignment));
  const int32_t j = minj;
  const int32_t indexConst = (j - 1) * j / 2;
  // Rows
  for (int32_t i = 0; i < j; ++i) {
    distances[indexConst + i] = std::numeric_limits<float>::max();
  }
  // Columns
  for (int32_t i = j + 1; i < n; ++i) {
    const int32_t index = (i - 1) * i / 2 + j;
    distances[index] = std::numeric_limits<float>::max();
  }
}

} // anonymous namespace
namespace GSFUtils {

/**
 * Merge the componentsIn and return
 * which componets got merged.
 */
std::vector<std::pair<int16_t, int16_t>>
findMerges(Component1D* componentsIn,
           const int16_t inputSize,
           const int16_t reducedSize)
{
  Component1D* components = static_cast<Component1D*>(
    __builtin_assume_aligned(componentsIn, alignment));

  // Sanity check. Function  throw on invalid inputs
  if (inputSize < 0 || inputSize > 128 || reducedSize > inputSize) {
    throw std::runtime_error("Invalid InputSize or reducedSize");
  }
  // We need just one for the full duration of a job
  const static std::vector<triangularToIJ> convert = createToIJ128();

  // Based on the inputSize allocate enough space for the pairwise distances
  const int16_t n = inputSize;
  const int32_t nn = n * (n - 1) / 2;
  // We work with a  multiple of 8*floats (32 bytes).
  const int32_t nn2 = (nn & 7) == 0 ? nn : nn + (8 - (nn & 7));
  AlignedDynArray<float, alignment> distances(
    nn2, std::numeric_limits<float>::max());

  // vector to be returned
  std::vector<std::pair<int16_t, int16_t>> merges;
  merges.reserve(inputSize - reducedSize);
  // initial distance calculation
  calculateAllDistances(components, distances.buffer(), n);

  // merge loop
  int32_t numberOfComponentsLeft = n;
  while (numberOfComponentsLeft > reducedSize) {
    // see if we have the next already
    const int32_t minIndex = findMinimumIndex(distances.buffer(), nn2);
    const triangularToIJ conversion = convert[minIndex];
    const int16_t mini = conversion.I;
    const int16_t minj = conversion.J;
    // Combine the 2 components
    combine(components[mini], components[minj]);
    // re-calculate distances wrt the new component at mini
    recalculateDistances(components, distances.buffer(), mini, n);
    // Reset old weights wrt the  minj position
    resetDistances(distances.buffer(), minj, n);
    // keep track and decrement
    merges.emplace_back(mini, minj);
    --numberOfComponentsLeft;
  } // end of merge while
  return merges;
}

/**
 * findMinimumIndex
 * Assume that the number of elements is a multiple
 * of 8 and is to be used for sizeable inputs.
 *
 * It uses the CxxUtils:vec class which provides
 * a degree of portability.
 *
 * avx2 gives us lanes 8 float wide
 * SSE4.1 gives us efficient blend
 * so we employ function multiversioning
 *
 * For non-sizeable inputs
 * std::distance(array, std::min_element(array, array + n))
 * can be good enough instead of calling this function.
 *
 * Note than the above "STL"  code in gcc
 * (up to 10.2 at least) this emits
 * a cmov which make it considerable slower
 * than the clang when the branch can
 * be well predicted.
 */
#if HAVE_FUNCTION_MULTIVERSIONING
#if defined(__x86_64__)
__attribute__((target("avx2"))) int32_t
findMinimumIndex(const float* distancesIn, const int n)
{
  using namespace CxxUtils;
  float* array = (float*)__builtin_assume_aligned(distancesIn, alignment);
  const vec<int, 8> increment = { 8, 8, 8, 8, 8, 8, 8, 8 };
  vec<int, 8> indicesIn = { 0, 1, 2, 3, 4, 5, 6, 7 };
  vec<int, 8> minindices = indicesIn;
  vec<float, 8> minvalues{};
  vec<float, 8> values{};
  vload(minvalues, array);
  for (int i = 8; i < n; i += 8) {
    // Load next 8 elements
    vload(values, array + i);
    // increment the indices
    indicesIn = indicesIn + increment;
    // Get a mask indicating when an element is less than the ones we have
    vec<int, 8> lt = values < minvalues;
    // blend select the indices to update
    vselect(minindices, indicesIn, minindices, lt);
    vmin(minvalues, values, minvalues);
  }
  // Do the final calculation scalar way
  int32_t minIndex = minindices[0];
  float minDistance = minvalues[0];
  for (int i = 1; i < 8; ++i) {
    if (minvalues[i] < minDistance) {
      minIndex = minindices[i];
      minDistance = minvalues[i];
    }
  }
  return minIndex;
}
__attribute__((target("sse4.1"))) int32_t
findMinimumIndex(const float* distancesIn, const int n)
{
  using namespace CxxUtils;
  float* array = (float*)__builtin_assume_aligned(distancesIn, alignment);
  // Do 2 vectors of 4 elements , so 8 at time
  const vec<int, 4> increment = { 8, 8, 8, 8 };
  vec<int, 4> indices1 = { 0, 1, 2, 3 };
  vec<int, 4> indices2 = { 4, 5, 6, 7 };
  vec<int, 4> minindices1 = indices1;
  vec<int, 4> minindices2 = indices2;
  vec<float, 4> minvalues1;
  vec<float, 4> minvalues2;
  vload(minvalues1, array);
  vload(minvalues2, array + 4);
  vec<float, 4> values1;
  vec<float, 4> values2;
  for (int i = 8; i < n; i += 8) {
    // Load 8 elements at a time
    vload(values1, array + i);     // first 4
    vload(values2, array + i + 4); // second 4
    // 1
    indices1 = indices1 + increment;
    vec<int, 4> lt1 = values1 < minvalues1;
    vselect(minindices1, indices1, minindices1, lt1);
    vmin(minvalues1, values1, minvalues1);
    // 2
    indices2 = indices2 + increment;
    vec<int, 4> lt2 = values2 < minvalues2;
    vselect(minindices2, indices2, minindices2, lt2);
    vmin(minvalues2, values2, minvalues2);
  }
  // Compare //1 with //2
  vec<int, 4> lt = minvalues1 < minvalues2;
  vselect(minindices1, minindices1, minindices2, lt);
  vmin(minvalues1, minvalues1, minvalues2);
  /*
   * Do the final calculation scalar way
   */
  size_t minIndex = minindices1[0];
  float minvalue = minvalues1[0];
  for (size_t i = 1; i < 4; ++i) {
    const float value = minvalues1[i];
    if (value < minvalue) {
      minvalue = value;
      minIndex = minindices1[i];
    }
  }
  return minIndex;
}
#endif // end of x86_64 versions
__attribute__((target("default")))
#endif // HAVE_FUNCTION_MULTIVERSIONING
int32_t
findMinimumIndex(const float* distancesIn, const int n)
{
  using namespace CxxUtils;
  float* array = (float*)__builtin_assume_aligned(distancesIn, alignment);
  const vec<int, 4> increment = { 8, 8, 8, 8 };
  vec<int, 4> indices1 = { 0, 1, 2, 3 };
  vec<int, 4> indices2 = { 4, 5, 6, 7 };
  vec<int, 4> minindices1 = indices1;
  vec<int, 4> minindices2 = indices2;
  vec<float, 4> minvalues1;
  vec<float, 4> minvalues2;
  vload(minvalues1, array);
  vload(minvalues2, array + 4);
  vec<float, 4> values1;
  vec<float, 4> values2;
  for (int i = 8; i < n; i += 8) {
    // Load 8 elements at a time
    vload(values1, array + i);     // first 4
    vload(values2, array + i + 4); // second 4
    // 1
    indices1 = indices1 + increment;
    vec<int, 4> lt1 = values1 < minvalues1;
    vselect(minindices1, indices1, minindices1, lt1);
    vmin(minvalues1, values1, minvalues1);
    // 2
    indices2 = indices2 + increment;
    vec<int, 4> lt2 = values2 < minvalues2;
    vselect(minindices2, indices2, minindices2, lt2);
    vmin(minvalues2, values2, minvalues2);
  }
  // Compare //1 with //2
  vec<int, 4> lt = minvalues1 < minvalues2;
  vselect(minindices1, minindices1, minindices2, lt);
  vmin(minvalues1, minvalues1, minvalues2);
  /*
   * Do the final calculation scalar way
   */
  size_t minIndex = minindices1[0];
  float minvalue = minvalues1[0];
  for (size_t i = 1; i < 4; ++i) {
    const float value = minvalues1[i];
    if (value < minvalue) {
      minvalue = value;
      minIndex = minindices1[i];
    }
  }
  return minIndex;
}

} // end namespace GSFUtils
