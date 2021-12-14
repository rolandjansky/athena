/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkGaussianSumFilterUtils/KLGaussianMixtureReduction.h"
#include "TrkGaussianSumFilterUtils/AlignedDynArray.h"
#include "TrkGaussianSumFilterUtils/GsfConstants.h"
//
#include "CxxUtils/features.h"
#include "CxxUtils/restrict.h"
#include "CxxUtils/vec.h"
#include "CxxUtils/vectorize.h"
//
#include <limits>
#include <stdexcept>
#include <vector>

#if !defined(__GNUC__)
#define __builtin_assume_aligned(X, N) X
#else
#if defined(__clang__)
#if !__has_builtin(__builtin_assume_aligned)
#define __builtin_assume_aligned(X, N) X
#endif
#endif
#endif

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

/*
 * these are q/P , 
 * mean = 1e9 means P=1e-9 MeV
 * 1e-3 ev
 * we want to approximate a delta function
 * at some impossible q/P with a normal.
 */
constexpr double largeMean = 1e9 ;
constexpr double tinySigma = 1e-15;
constexpr double invertTinySigma = 1e15 ;

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
// We need just one for the full duration of a job so static const
const std::vector<triangularToIJ> convert = createToIJMaxRowCols();

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
  // approximate a delta function
  // as a Gaussian
  // with large mean a small sigma
  // the KL distance to it
  // will be large
  removed.mean = largeMean;
  removed.cov = tinySigma;
  removed.invCov = invertTinySigma;
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
    __builtin_assume_aligned(componentsIn, GSFConstants::alignment));

  float* distances = static_cast<float*>(
    __builtin_assume_aligned(distancesIn, GSFConstants::alignment));

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
 * Calculate the distances for all component pairs
 */
void
calculateAllDistances(const Component1D* componentsIn,
                      float* distancesIn,
                      const int32_t n)
{

  const Component1D* components = static_cast<const Component1D*>(
    __builtin_assume_aligned(componentsIn, GSFConstants::alignment));

  float* distances = static_cast<float*>(
    __builtin_assume_aligned(distancesIn, GSFConstants::alignment));

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
  float* distances = static_cast<float*>(
    __builtin_assume_aligned(distancesIn, GSFConstants::alignment));

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
MergeArray
findMerges(Component1DArray& componentsIn, const int8_t reducedSize)
{
  const int32_t n = componentsIn.numComponents;
  // Sanity check. Function  throw on invalid inputs
  if (n < 0 || n > GSFConstants::maxComponentsAfterConvolution ||
      reducedSize > n) {
    throw std::runtime_error("findMerges :Invalid InputSize or reducedSize");
  }

  Component1D* components = static_cast<Component1D*>(__builtin_assume_aligned(
    componentsIn.components.data(), GSFConstants::alignment));

  // Based on the inputSize n allocate enough space for the pairwise distances
  // We work with a  multiple of 8*floats (32 bytes).
  const int32_t nn = n * (n - 1) / 2;
  const int32_t nn2 = (nn & 7) == 0 ? nn : nn + (8 - (nn & 7));
  AlignedDynArray<float, GSFConstants::alignment> distances(
    nn2, std::numeric_limits<float>::max());
  // initial distance calculation
  calculateAllDistances(components, distances.buffer(), n);
  // keep track of where we are
  int32_t numberOfComponentsLeft = n;
  // Result to  returned
  MergeArray result{};
  // merge loop
  while (numberOfComponentsLeft > reducedSize) {
    // see if we have the next already
    const int32_t minIndex = findMinimumIndex(distances.buffer(), nn2);
    const triangularToIJ conversion = convert[minIndex];
    const int8_t mini = conversion.I;
    const int8_t minj = conversion.J;
    // Combine the 2 components
    combine(components[mini], components[minj]);
    // Reset all distances wrt  to minj to float max
    // so they can not be re-found as minimum distances.
    resetDistances(distances.buffer(), minj, n);
    // Set minj as merged
    // re-calculate distances wrt the new component at mini
    recalculateDistances(components, distances.buffer(), mini, n);
    // keep track and decrement
    result.merges[result.numMerges] = { mini, minj };
    ++result.numMerges;
    --numberOfComponentsLeft;
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
 * avx2 gives us lanes 8 float wide
 * SSE4.1 gives us efficient blend
 * so we employ function multiversioning
 *
 * For non-sizeable inputs
 * std::distance(array, std::min_element(array, array + n))
 * can be good enough instead of calling this function.
 *
 * Note than the above "STL"  code in gcc
 * (up to 10.2 at least) emits
 * a cmov which make it considerable slower
 * than the clang when the branch can
 * be well predicted.
 */
#if HAVE_FUNCTION_MULTIVERSIONING
#if defined(__x86_64__)
__attribute__((target("avx2")))

int32_t
findMinimumIndex(const float* distancesIn, const int n)
{
  using namespace CxxUtils;
  float* array = static_cast<float*>(
    __builtin_assume_aligned(distancesIn, GSFConstants::alignment));
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
__attribute__((target("sse4.1")))

int32_t
findMinimumIndex(const float* distancesIn, const int n)
{
  using namespace CxxUtils;
  float* array = static_cast<float*>(
    __builtin_assume_aligned(distancesIn, GSFConstants::alignment));
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
  float* array = static_cast<float*>(
    __builtin_assume_aligned(distancesIn, GSFConstants::alignment));
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
