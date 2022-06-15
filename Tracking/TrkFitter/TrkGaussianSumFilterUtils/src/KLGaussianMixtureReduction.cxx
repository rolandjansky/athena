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
#include <cmath>
#include <limits>
#include <numeric>
#include <stdexcept>
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

/**
 * The methods in the anonymous namespace
 * are used once in the findMerges.
 * Typically they are getting inlined.
 * But we want to enforce that as much we can.
 * especially for he ones we want the
 * function multiversioning to be "applied".
 */

using namespace GSFUtils;

/**
 * Based on
 * https://www.sciencedirect.com/science/article/pii/089812218990103X
 * equation (16)
 * or
 * https://arxiv.org/pdf/2001.00727.pdf
 * equation (10)
 * but not accounting for weights
 * covI * invCovJ + covJ * invCovI + (mean1-mean2) (invcov+invcov) (mean1-mean2)
 * We use doubles for the intermediate calculations
 * but we store the final distance in a float
 */
#if (defined(__GNUC__) || defined(__clang__))
[[gnu::always_inline]]
#endif
inline float
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
#if (defined(__GNUC__) || defined(__clang__))
[[gnu::always_inline]]
#endif
inline void
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

/*
 * We rely on a fast simd implemantation for
 * finding the index of the minimum in an array.
 * We assume that the number of elements
 * is a multiple of 16.
 * We use the CxxUtils:vec class which provides
 * a degree of portability instead of using
 * directly the intrinsics.
 * For now we target 128 bit wide vectors
 */
#if (defined(__GNUC__) || defined(__clang__))
[[gnu::always_inline]]
#endif
inline int32_t
findMinimumIndex(const float* distancesIn, const int n)
{
  using namespace CxxUtils;
  const float* array = assume_aligned<GSFConstants::alignment>(distancesIn);
  // Do 4 vectors of 4 elements , so 16 at time
  const vec<int, 4> increment = { 16, 16, 16, 16 };
  // setup indices
  vec<int, 4> indices1 = { 0, 1, 2, 3 };
  vec<int, 4> indices2 = { 4, 5, 6, 7 };
  vec<int, 4> indices3 = { 8, 9, 10, 11 };
  vec<int, 4> indices4 = { 12, 13, 14, 15 };
  // initial min indices
  vec<int, 4> minindices1 = indices1;
  vec<int, 4> minindices2 = indices2;
  vec<int, 4> minindices3 = indices3;
  vec<int, 4> minindices4 = indices4;
  // initial minvalue
  vec<float, 4> minvalues1;
  vec<float, 4> minvalues2;
  vec<float, 4> minvalues3;
  vec<float, 4> minvalues4;
  vload(minvalues1, array);
  vload(minvalues2, array + 4);
  vload(minvalues3, array + 8);
  vload(minvalues4, array + 12);
  // values
  vec<float, 4> values1;
  vec<float, 4> values2;
  vec<float, 4> values3;
  vec<float, 4> values4;
  // Main loop 16 elements at a time
  // using vec/for SIMD
  for (int i = 16; i < n; i += 16) {
    // 1
    vload(values1, array + i); // 0-3
    indices1 = indices1 + increment;
    vec<int, 4> lt1 = values1 < minvalues1;
    vselect(minindices1, indices1, minindices1, lt1);
    vmin(minvalues1, values1, minvalues1);
    // 2
    vload(values2, array + i + 4); // 4-7
    indices2 = indices2 + increment;
    vec<int, 4> lt2 = values2 < minvalues2;
    vselect(minindices2, indices2, minindices2, lt2);
    vmin(minvalues2, values2, minvalues2);
    // 3
    vload(values3, array + i + 8); // 8-11
    indices3 = indices3 + increment;
    vec<int, 4> lt3 = values3 < minvalues3;
    vselect(minindices3, indices3, minindices3, lt3);
    vmin(minvalues3, values3, minvalues3);
    // 4
    vload(values4, array + i + 12); // 12-15
    indices4 = indices4 + increment;
    vec<int, 4> lt4 = values4 < minvalues4;
    vselect(minindices4, indices4, minindices4, lt4);
    vmin(minvalues4, values4, minvalues4);
  }
  // SIMD compare 1 with 2 , minimum becomes 1
  vec<int, 4> lt12 = minvalues1 < minvalues2;
  vselect(minindices1, minindices1, minindices2, lt12);
  vmin(minvalues1, minvalues1, minvalues2);
  // SIMD compare 3 with 4 minimum becomes 3
  vec<int, 4> lt34 = minvalues3 < minvalues4;
  vselect(minindices3, minindices3, minindices4, lt34);
  vmin(minvalues3, minvalues3, minvalues4);
  // SIMD compare 1 with 3 minimum becomes 1
  vec<int, 4> lt = minvalues1 < minvalues3;
  vselect(minindices1, minindices1, minindices3, lt);
  vmin(minvalues1, minvalues1, minvalues3);

  // Do the final calculation scalar way
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
/**
 * Given a number of components n
 * we need padding to use the
 * simd findMinimumIndex
 */
#if (defined(__GNUC__) || defined(__clang__))
[[gnu::always_inline]]
#endif
constexpr inline int32_t
extraElements(const int32_t nn)
{
  const int32_t remainder = (nn & 15);
  return remainder == 0 ? 0 : (16 - remainder);
}

#if (defined(__GNUC__) || defined(__clang__))
[[gnu::always_inline]]
#endif
constexpr inline int32_t
numDistances(const int32_t n)
{
  const int32_t npadded = n + extraElements(n);
  return npadded;
}

#if (defined(__GNUC__) || defined(__clang__))
[[gnu::always_inline]]
#endif
inline int32_t
numDistances(const int32_t n, float* distancesIn)
{
  const int32_t npadded = n + extraElements(n);
  // Make sure the padded elements are set to max
  std::fill(
    distancesIn + n, distancesIn + npadded, std::numeric_limits<float>::max());
  return npadded;
}

/*
 * This is a O(N^3) time N^2 space algorithm.
 * It can be seen as tailored implementation for
 * our problem of the basic algorithm
 * for Hierarchical Clustering.
 *
 * We rely on the fast findMinimunIndex above
 * and a triangular array representation to
 * reduce the actual time for our problem
 * (max N=72)
 *
 * We opt for fixed size arrays of max N(=72) elements,
 * but dynamic arrays of ~ N*(N-1)/2 elements.
 *
 * Existing alternatives in the literature:
 * - 1. O(N^3) worst-case time , O(n^2) best
 * - 2. O(N^2 log(N)) worst-time
 * See :
 * "Modern hierarchical, agglomerative clustering algorithms"
 *  https://arxiv.org/abs/1109.2378
 *  or
 * "Efficient algorithms for agglomerative hierarchical clustering methods"
 *
 * What we found in the past:
 * - We seem to hit the O(N^3)/worst case of Alg 1
 * quite often.
 * - Alg 2 had significant overhead
 * from the required data-structures.
 * We could revisit these in the future.
 */

/*
 * Pairwise distances implementation:
 * Assuming N total elements 0... N-1,
 * the pairwise distance matrix
 * can be represented in a triangular array: <br>
 * [ (1,0) ] <br>
 * [ (2,0), (2,1) ] <br>
 * [ (3,0), (3,1), (3,2)] <br>
 * [ (4,0), (4,1), (4,2) , (4,3) <br>
 * [.............................] <br>
 * [(N-1,0),(N-1,1),(N-1,2),(N-1,3) ... (N-1,N-2)]<br>
 * With size 1+2+3+ .... (N-1) = N*(N-1)/2
 *
 * The lexicographical storage allocation function is
 * Loc(i,j) = i*(i-1)/2 + j <br>
 * e.g : <br>
 * (1,0) => 1 *(1-1)/2 + 0 => 0 <br>
 * (2,0) => 2 *(2-1)/2 + 0 => 1 <br>
 * (2,1) => 2 *(2-1)/2 + 1 => 2 <br>
 * (3,0) => 3 * (3-1)/2 +0 => 3 <br>
 * Leading to <br>
 * [(1,0),(2,0),(2,1),(3,0),(3,1),(3,2).... (N-1,N-2)]
 *
 * The N-1 Rows  map to the value K of the 1st element in the pair
 * 1,2,3,..,N-1. <br>
 * Each Row has size K  and starts at array positions K*(K-1)/2 <br>
 * e.g <br>
 * The row for element 1 starts at array position 0. <br>
 * The row for element 2 starts at array position 1. <br>
 * The row for element N-1  starts at array positon (N-1)*(N-2)/2 <br>
 *
 * The N-1 Columns map to the value K of the second  element in the pair <br>
 * K= 0,1,2 .., N-2 <br>
 * The array positions follows (i-1)*i/2+K <br>
 * where i : K+1 .... N-1 [for(i=K+1;i<N;++i) <br>
 * e.g <br>
 * 0 appears as 2nd element in the pair at array positions [0,1,3,6...] <br>
 * 1 appears as 2nd element in the pair at array positions [2,4,7...] <br>
 * 2 appears as 2nd element in the pair at array positions [5,8,12....] <br>
 * N-2 appears as 2nd element once at position [N(N-1)/2-1] <br>
 */

/**
 * @brief Helper struct to map position in
 * triangular array to matrix(I, J) indices
 */
struct triangularToIJ
{
  int8_t I = -1;
  int8_t J = -1;
};
/**
 * @brief Helper method to convert
 * from index in triangular array
 * to matrix(I,J)
 */
#if (defined(__GNUC__) || defined(__clang__))
[[gnu::always_inline]]
#endif
inline triangularToIJ
convert(int32_t idx)
{
  // We prefer to preMap the maximum 2556 elements.
  // Alternatively one can use the following
  // if pre-mapping becomes an issue
  // (see https://hal.archives-ouvertes.fr/hal-02047514/document)
  //  int8_t i = std::floor((std::sqrt(1 + 8 * idx) + 1) / 2);
  //  int8_t j = idx - (i - 1) * i / 2;
  static const std::vector<triangularToIJ> preMap = []() {
    constexpr int32_t n = GSFConstants::maxComponentsAfterConvolution;
    constexpr size_t nn = n * (n - 1) / 2;
    std::vector<triangularToIJ> indexMap(nn);
    for (int8_t i = 1; i < n; ++i) {
      const int32_t indexConst = (i - 1) * i / 2;
      for (int8_t j = 0; j < i; ++j) {
        indexMap[indexConst + j] = { i, j };
      }
    }
    return indexMap;
  }();
  return preMap[idx];
}

/**
 * @brief Calculate the distances for all component pairs
 * @c componentsIn is the array of componets
 * @distancesIn is the array of distances to fill
 * @n is the number of components
 */
#if (defined(__GNUC__) || defined(__clang__))
[[gnu::always_inline]]
#endif
inline void
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
/**
 * @brief update the distances wrt to the merged
 * components
 * @c componentsIn are the input components
 * @c distancesIn is the array of distances
 * @c minFrom is the index of the element we merged from (remove)
 * @c minTo is the index of the element we merged to (keep)
 * @c n is the components before the removal
 */
#if (defined(__GNUC__) || defined(__clang__))
[[gnu::always_inline]]
#endif
inline int32_t
updateDistances(
  Component1D* ATH_RESTRICT componentsIn,
  std::array<int8_t, GSFConstants::maxComponentsAfterConvolution>& mergingIndex,
  float* ATH_RESTRICT distancesIn,
  int32_t minFrom,
  int32_t minTo,
  int32_t n)
{
  float* distances =
    CxxUtils::assume_aligned<GSFConstants::alignment>(distancesIn);
  Component1D* components =
    CxxUtils::assume_aligned<GSFConstants::alignment>(componentsIn);
  // We swap the last elements with the ones indexed by minFrom.
  // After this the remaining components we care about
  // are n-1 which we return
  const int32_t last = (n - 1);
  const int32_t indexOffsetJ = (minFrom - 1) * minFrom / 2;
  const int32_t indexOffsetLast = (last - 1) * last / 2;
  // we do no need to swap the last with itself
  if (minFrom != last) {
    // Rows in distance matrix
    for (int32_t i = 0; i < minFrom; ++i) {
      std::swap(distances[indexOffsetJ + i], distances[indexOffsetLast + i]);
    }
    // Columns in distance matrix
    for (int32_t i = minFrom + 1; i < last; ++i) {
      const int32_t index = (i - 1) * i / 2 + minFrom;
      std::swap(distances[index], distances[indexOffsetLast + i]);
    }
    // swap the components
    std::swap(components[minFrom], components[last]);
    std::swap(mergingIndex[minFrom], mergingIndex[last]);
  }
  // In case minTo was indexing the last now it should
  // be indexing minFrom due to the above swapping
  if (minTo == last) {
    minTo = minFrom;
  }
  const int32_t indexConst = (minTo - 1) * minTo / 2;
  // This is the component that has been updated
  const Component1D componentJ = components[minTo];
  // Rows in distance matrix
  for (int32_t i = 0; i < minTo; ++i) {
    const Component1D componentI = components[i];
    const int32_t index = indexConst + i;
    distances[index] = symmetricKL(componentI, componentJ);
  }
  // Columns in distance matrix
  for (int32_t i = minTo + 1; i < last; ++i) {
    const Component1D componentI = components[i];
    const int32_t index = (i - 1) * i / 2 + minTo;
    distances[index] = symmetricKL(componentI, componentJ);
  }
  return last;
}
/**
 * Return which components need to be merged.
 */
#if HAVE_TARGET_CLONES
#if defined(__x86_64__)
[[gnu::target_clones("sse4.2,default")]]
#endif // end of x86_64 versions
#endif // HAVE_TARGET_CLONES
MergeArray
findMergesImpl(const Component1DArray& componentsIn,
               const int32_t n,
               const int8_t reducedSize)
{
  // copy the array for internal use
  Component1DArray copyComponents(componentsIn);
  Component1D* components = CxxUtils::assume_aligned<GSFConstants::alignment>(
    copyComponents.components.data());
  // Based on the inputSize n allocate enough space for the pairwise distances
  int32_t nn = n * (n - 1) / 2;
  int32_t nnpadded = numDistances(nn);
  AlignedDynArray<float, GSFConstants::alignment> distances(
    nnpadded, std::numeric_limits<float>::max());
  // initial distance calculation
  calculateAllDistances(components, distances.buffer(), n);
  // As we merge keep track where things moved
  std::array<int8_t, GSFConstants::maxComponentsAfterConvolution>
    mergingIndex{};
  std::iota(mergingIndex.begin(), mergingIndex.end(), 0);
  // Result to be returned
  MergeArray result{};
  int32_t numberOfComponentsLeft = n;
  // merge loop
  while (numberOfComponentsLeft > reducedSize) {
    // find pair with minimum distance
    const int32_t minIndex = findMinimumIndex(distances.buffer(), nnpadded);
    const triangularToIJ conversion = convert(minIndex);
    int8_t minTo = conversion.I;
    int8_t minFrom = conversion.J;
    // This is the convention we had so retained.
    if (mergingIndex[minTo] < mergingIndex[minFrom]) {
      std::swap(minTo, minFrom);
    }
    // prepare what to return
    const int8_t miniToreturn = mergingIndex[minTo];
    const int8_t minjToreturn = mergingIndex[minFrom];
    result.merges[result.numMerges] = { miniToreturn, minjToreturn };
    ++result.numMerges;
    // Combine
    combine(components[minTo], components[minFrom]);
    // update distances
    numberOfComponentsLeft = updateDistances(components,
                                             mergingIndex,
                                             distances.buffer(),
                                             minFrom,
                                             minTo,
                                             numberOfComponentsLeft);

    // number of remaining distances dividable by 16
    nn = numberOfComponentsLeft * (numberOfComponentsLeft - 1) / 2;
    nnpadded = numDistances(nn, distances.buffer());
  } // end of merge while
  return result;
}

} // anonymous namespace

namespace GSFUtils {

MergeArray
findMerges(const Component1DArray& componentsIn, const int8_t reducedSize)
{
  const int32_t n = componentsIn.numComponents;
  if (n < 0 || n > GSFConstants::maxComponentsAfterConvolution ||
      reducedSize > n) {
    throw std::runtime_error("findMerges :Invalid InputSize or reducedSize");
  }
  return findMergesImpl(componentsIn, n, reducedSize);
}
} // end namespace GSFUtils
