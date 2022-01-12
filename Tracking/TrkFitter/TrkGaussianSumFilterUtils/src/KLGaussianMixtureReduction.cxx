/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkGaussianSumFilterUtils/KLGaussianMixtureReduction.h"
#include "TrkGaussianSumFilterUtils/AlignedDynArray.h"
#include "TrkGaussianSumFilterUtils/GsfConstants.h"
//
#include "CxxUtils/assume_aligned.h"
#include "CxxUtils/features.h"
#include "CxxUtils/restrict.h"
#include "CxxUtils/vec.h"
#include "CxxUtils/vectorize.h"
//
#include <limits>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

ATH_ENABLE_VECTORIZATION;
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
  int8_t I = -1;
  int8_t J = -1;
};

/**
 * @brief Helper method to precalucate
 * the mapping of an triangular array index
 * to the corresponding I,J
 */
std::vector<triangularToIJ>
createToIJMaxRowCols()
{
  constexpr int32_t n = GSFConstants::maxComponentsAfterConvolution;
  constexpr int32_t nn = n * (n - 1) / 2;
  std::vector<triangularToIJ> indexMap(nn);
  for (int8_t i = 1; i < n; ++i) {
    const int32_t indexConst = (i - 1) * i / 2;
    for (int8_t j = 0; j < i; ++j) {
      indexMap[indexConst + j] = { i, j };
    }
  }
  return indexMap;
}

/**
 * @brief we need one tringular to IJ map for the full job
 * so precalculate it here, const and static since inside
 * the anonymous namespace
 */
const std::vector<triangularToIJ> convert = createToIJMaxRowCols();

/**
 * @brief given a number of components n
 * return the number pairwise distance
 * padding so as to be a multiple of 8
 */
int32_t
numDistances8(const int32_t n)
{
  const int32_t nn = n * (n - 1) / 2;
  const int32_t nn2 = (nn & 7) == 0 ? nn : nn + (8 - (nn & 7));
  return nn2;
}

int32_t
numDistances8(const int32_t n, float* distancesIn)
{
  const int32_t nn = n * (n - 1) / 2;
  const int32_t nn2 = (nn & 7) == 0 ? nn : nn + (8 - (nn & 7));
  // Make sure the extra elements are set to max
  // set the loop for the minimum can not find them
  std::fill(
    distancesIn + nn, distancesIn + nn2, std::numeric_limits<float>::max());
  return nn2;
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
symmetricKL(const Component1D& ATH_RESTRICT componentI,
            const Component1D& ATH_RESTRICT componentJ)
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
combine(GSFUtils::Component1D& ATH_RESTRICT updated,
        GSFUtils::Component1D& ATH_RESTRICT removed)
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
}

/**
 * @brief Reset the distances wrt to a mini index
 * @c componentsIn are the input components
 * @c distancesIn is the array of distances
 * @c minj is the index of the element we merged from (remove)
 * @c n is the components before the removal
 *
 * 1. We set the distances wrt to minj to a large value
 * 2. We swap the distances of the last element
 * with them. So they go to the end of the array
 *
 * After this the remaining components are n-1
 * which we return
 */
int32_t
resetDistances(
  Component1D* ATH_RESTRICT componentsIn,
  std::array<int8_t, GSFConstants::maxComponentsAfterConvolution>& mergingIndex,
  float* ATH_RESTRICT distancesIn,
  const int32_t minj,
  const int32_t n)
{
  float* distances =
    CxxUtils::assume_aligned<GSFConstants::alignment>(distancesIn);

  Component1D* components =
    CxxUtils::assume_aligned<GSFConstants::alignment>(componentsIn);

  const int32_t j = minj;
  const int32_t last = (n - 1);
  // Look at KLGaussianMixtureReduction.h
  // for how things are indexed for triangular arrays
  const int32_t indexOffsetJ = (j - 1) * j / 2;
  const int32_t indexOffsetLast = (last - 1) * last / 2;
  int32_t movedElements = 0;

  // Rows
  for (int32_t i = 0; i < j; ++i, ++movedElements) {
    std::swap(distances[indexOffsetJ + i],
              distances[indexOffsetLast + movedElements]);
  }

  // This is for  the distance of the minj
  // with the last so we do not need swap
  // Also if minj is the last the element does not exist
  // as we do not keep distance to self
  if (j != last) {
    ++movedElements;
  }

  // The columns
  for (int32_t i = j + 1; i < last; ++i, ++movedElements) {
    const int32_t index = (i - 1) * i / 2 + j;
    std::swap(distances[index], distances[indexOffsetLast + movedElements]);
  }
  // And now swap the components
  std::swap(components[minj], components[last]);
  std::swap(mergingIndex[minj], mergingIndex[last]);
  return last;
}

/**
 * @brief Recalculate the distances given a merged input
 * @c componentsIn are the input components
 * @c distancesIn is the array of distances
 * @c mini is the index of the element we merged to (keep)
 * @c n is the components before the removal
 */
void
recalculateDistances(const Component1D* componentsIn,
                     float* distancesIn,
                     const int32_t mini,
                     const int32_t n)
{
  const Component1D* components =
    CxxUtils::assume_aligned<GSFConstants::alignment>(componentsIn);

  float* distances =
    CxxUtils::assume_aligned<GSFConstants::alignment>(distancesIn);

  const int32_t j = mini;
  const int32_t indexConst = (j - 1) * j / 2;
  // This is the component that has been updated
  // so we calculate distances wrt.
  const Component1D componentJ = components[j];

  // Rows
  for (int32_t i = 0; i < j; ++i) {
    // only change for non-merged components
    const Component1D componentI = components[i];
    const int32_t index = indexConst + i;
    distances[index] = symmetricKL(componentI, componentJ);
  }
  // Columns
  for (int32_t i = j + 1; i < n; ++i) {
    // only change for non-merged components
    const Component1D componentI = components[i];
    const int32_t index = (i - 1) * i / 2 + j;
    distances[index] = symmetricKL(componentI, componentJ);
  }
}

/**
 * @brief Calculate the distances for all component pairs
 * @c componentsIn is the array of componets
 * @distancesIn is the array of distances to fill
 * @n is the number of components
 *
 * We need this once in the beginning to initialize
 * the distance map.
 */
void
calculateAllDistances(const Component1D* componentsIn,
                      float* distancesIn,
                      const int32_t n)
{

  const Component1D* components =
    CxxUtils::assume_aligned<GSFConstants::alignment>(componentsIn);

  float* distances =
    CxxUtils::assume_aligned<GSFConstants::alignment>(distancesIn);

  for (int32_t i = 1; i < n; ++i) {
    const int32_t indexConst = (i - 1) * i / 2;
    const Component1D componentI = components[i];
    for (int32_t j = 0; j < i; ++j) {
      const Component1D componentJ = components[j];
      distances[indexConst + j] = symmetricKL(componentI, componentJ);
    }
  }
}

} // anonymous namespace
namespace GSFUtils {

/**
 * Merge the componentsIn and return
 * which componets got merged.
 */
MergeArray
findMerges(const Component1DArray& componentsIn, const int8_t reducedSize)
{

  const int32_t n = componentsIn.numComponents;
  // Sanity check. Function  throw on invalid inputs
  if (n < 0 || n > GSFConstants::maxComponentsAfterConvolution ||
      reducedSize > n) {
    throw std::runtime_error("findMerges :Invalid InputSize or reducedSize");
  }
  // copy the array for internal use
  Component1DArray copyComponents(componentsIn);
  Component1D* components = CxxUtils::assume_aligned<GSFConstants::alignment>(
    copyComponents.components.data());
  // Based on the inputSize n allocate enough space for the pairwise distances
  int32_t nn2 = numDistances8(n);
  AlignedDynArray<float, GSFConstants::alignment> distances(
    nn2, std::numeric_limits<float>::max());
  // initial distance calculation
  calculateAllDistances(components, distances.buffer(), n);
  // As we merge keep track where things moved
  std::array<int8_t, GSFConstants::maxComponentsAfterConvolution> mergingIndex{};
  std::iota(mergingIndex.begin(), mergingIndex.end(), 0);
  // Result to be returned
  MergeArray result{};
  int32_t numberOfComponentsLeft = n;
  // merge loop
  while (numberOfComponentsLeft > reducedSize) {
    // find pair with minimum distance
    const int32_t minIndex = findMinimumIndex(distances.buffer(), nn2);
    const triangularToIJ conversion = convert[minIndex];
    int8_t mini = conversion.I;
    int8_t minj = conversion.J;
    // This is the convention we had so retained.
    if (mergingIndex[mini] < mergingIndex[minj]) {
      std::swap(mini, minj);
    }
    // prepare what to return
    const int8_t miniToreturn = mergingIndex[mini];
    const int8_t minjToreturn = mergingIndex[minj];
    result.merges[result.numMerges] = { miniToreturn, minjToreturn };
    ++result.numMerges;
    // do all operation before we reset.
    // Reset will move things around
    combine(components[mini], components[minj]);
    recalculateDistances(
      components, distances.buffer(), mini, numberOfComponentsLeft);
    // Call reset this also reduces the number of components left
    numberOfComponentsLeft = resetDistances(components,
                                            mergingIndex,
                                            distances.buffer(),
                                            minj,
                                            numberOfComponentsLeft);

    // number of remaining distances dividable by 8
    nn2 = numDistances8(numberOfComponentsLeft,distances.buffer());
  } // end of merge while
  return result;
}

/**
 * findMinimumIndex
 * Assume that the number of elements is a multiple
 * of 8 and is to be used for sizeable inputs.
 *
 * It uses the CxxUtils:vec class which provides
 * a degree of portability.
 *
 * SSE4.1 gives us efficient blend
 * so we employ function multiversioning
 * An AVX version could gives us directly
 * lanes 8 float wide
 *
 * For non-sizeable inputs
 * std::distance(array, std::min_element(array, array + n))
 * can be good enough instead of calling this function.
 */
#if HAVE_FUNCTION_MULTIVERSIONING
#if defined(__x86_64__)
__attribute__((target("sse4.1")))
int32_t
findMinimumIndex(const float* distancesIn, const int n)
{
  using namespace CxxUtils;
  const float* array = assume_aligned<GSFConstants::alignment>(distancesIn);
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
  const float* array = assume_aligned<GSFConstants::alignment>(distancesIn);
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
