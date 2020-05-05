/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "TrkGaussianSumFilter/KLGaussianMixtureReduction.h"
#include "CxxUtils/features.h"
#include "CxxUtils/vectorize.h"
#include <algorithm>
#include <limits>

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

namespace GSFUtils {
/*
 * findMinimumIndex
 *
 * For FindMinimumIndex at x86_64 we have
 * AVX2 and SSE versions
 * These assume that the number of elements is a multiple
 * of 8 and are to be used for sizeable inputs.
 *
 * We also provide a default "scalar" implementation
 *
 * FindMinimumSTL
 * One of the issues we have see in that gcc8.3 and clang8 (02/2020)
 * optimise differently the STL version. See also
 * https://its.cern.ch/jira/projects/ATLASRECTS/issues/ATLASRECTS-5244
 *
 * We also provide FindMinimumPair that returns the two smallest values
 *
 */
#if HAVE_FUNCTION_MULTIVERSIONING
#if defined(__x86_64__)
#include <immintrin.h>
/*
 *
 * AVX2 intrinsics used :
 *
 * _mm256_set1_epi32
 *  Broadcast 32-bit integer a to all elements of dst. This intrinsic may
 * generate the vpbroadcastd.
 *
 *  _mm256_setr_epi32
 *  Set packed 32-bit integers in dst with the supplied values in reverse order.
 *
 *  _mm256_load_ps
 *  Load 256-bits (composed of 8 packed single-precision (32-bit) floating-point
 * elements) from memory into dst. mem_addr must be aligned on a 32-byte
 * boundary or a general-protection exception may be generated.
 *
 *  _mm256_add_epi32
 *  Add packed 32-bit integers in a and b, and store the results in dst.
 *
 *   _mm256_cmp_ps
 *   Compare packed single-precision (32-bit) floating-point elements in a and b
 * based on the comparison operand specified by imm8, and store the results in
 * dst.
 *
 *   _mm256_min_ps
 *   Compare packed single-precision (32-bit) floating-point elements in a and
 * b, and store packed minimum values in dst.
 *
 *    _mm256_blendv_epi8
 *    Blend packed 8-bit integers from a and b using mask, and store the results
 * in dst.
 */
__attribute__((target("avx2"))) 
int32_t
findMinimumIndex(const floatPtrRestrict distancesIn, const int n)
{
  float* array = (float*)__builtin_assume_aligned(distancesIn, alignment);
  const __m256i increment = _mm256_set1_epi32(8);
  __m256i indicesIn = _mm256_setr_epi32(0, 1, 2, 3, 4, 5, 6, 7);
  __m256i minindices = indicesIn;
  __m256 minvalues = _mm256_load_ps(array);
  for (int i = 8; i < n; i += 8) {
    // Load next 8 elements
    const __m256 values = _mm256_load_ps(array + i);
    // increment the indices
    indicesIn = _mm256_add_epi32(indicesIn, increment);
    // Get a mask indicating when an element is less than the ones we have
    __m256i lt =
      _mm256_castps_si256(_mm256_cmp_ps(values, minvalues, _CMP_LT_OS));
    // b lend select the indices to update
    minindices = _mm256_blendv_epi8(minindices, indicesIn, lt);
    minvalues = _mm256_min_ps(values, minvalues);
  }
  /*
   * Do the final calculation scalar way
   */
  alignas(alignment) float distances[8];
  alignas(alignment) int32_t indices[8];
  _mm256_store_ps(distances, minvalues);
  _mm256_store_si256((__m256i*)(indices), minindices);
  int32_t minIndex = indices[0];
  float minDistance = distances[0];
  for (int i = 1; i < 8; ++i) {
    if (distances[i] < minDistance) {
      minIndex = indices[i];
      minDistance = distances[i];
    }
  }
  return minIndex;
}
/*
 * SSE
 * SSE2 does not have a blend/select instruction.
 * Instruction describes in
 * We AND &
 * - a with the NOT of the mask
 * - b with the mask
 * - The we OR the above 2
 */
#if defined(__SSE4_1__)
static const auto mm_blendv_epi8 = _mm_blendv_epi8;
#elif defined(__SSE2__)
static inline __m128i
SSE2_mm_blendv_epi8(__m128i a, __m128i b, __m128i mask)
{
  return _mm_or_si128(_mm_andnot_si128(mask, a), _mm_and_si128(mask, b));
}
static const auto mm_blendv_epi8 = SSE2_mm_blendv_epi8;
#endif /* on SSE4.1 vs SSE2 for the blend instructions*/
/*
 * SSE intrinsics used
 *
 *  _mm_set1_epi32
 *  Broadcast 32-bit integer a to all elements of dst.
 *
 *  _mm_setr_epi32
 *  https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_setr_epi32&expand=431,452,426,4946,4946,4988&techs=SSE2
 *
 *  _mm_load_ps
 *  Set packed 32-bit integers in dst with the supplied values in reverse order.
 *
 *  dst = _mm_add_epi32 (a,b)
 *  Add packed 32-bit integers in a and b, and store the results in dst.
 *
 *   dst = _mm_min_ps (a,b)
 *   Compare packed single-precision (32-bit) floating-point elements in a and
 * b, and store packed minimum values in dst.
 *
 *   dst =  _mm_cmplt_ps ( a, b)
 *   Compare packed single-precision (32-bit) floating-point elements in a and b
 * for less-than, and store the results in dst.
 *
 *    _mm_castps_si128
 *    Cast vector of type __m128 to type __m128i. This intrinsic is only used
 * for compilation and does not generate any instructions, thus it has zero
 * latency.
 */
__attribute__((target("sse4.2,sse2"))) 
int32_t
findMinimumIndex(const floatPtrRestrict distancesIn, const int n)
{
  float* array = (float*)__builtin_assume_aligned(distancesIn, alignment);
  /* Assuming SSE do 2 vectors of 4 elements in a time
   * one might want to revisit for AVX2 */
  const __m128i increment = _mm_set1_epi32(8);
  __m128i indices1 = _mm_setr_epi32(0, 1, 2, 3);
  __m128i indices2 = _mm_setr_epi32(4, 5, 6, 7);
  __m128i minindices1 = indices1;
  __m128i minindices2 = indices2;
  __m128 minvalues1 = _mm_load_ps(array);
  __m128 minvalues2 = _mm_load_ps(array + 4);

  for (int i = 8; i < n; i += 8) {
    // Load 8 elements at a time in 2 vectors of size 4
    const __m128 values1 = _mm_load_ps(array + i);     // first 4
    const __m128 values2 = _mm_load_ps(array + i + 4); // second 4
    // Handle the first 4
    indices1 = _mm_add_epi32(indices1, increment);
    __m128i lt1 = _mm_castps_si128(_mm_cmplt_ps(values1, minvalues1));
    minindices1 = mm_blendv_epi8(minindices1, indices1, lt1);
    minvalues1 = _mm_min_ps(values1, minvalues1);
    // Handle the second 4
    indices2 = _mm_add_epi32(indices2, increment);
    __m128i lt2 = _mm_castps_si128(_mm_cmplt_ps(values2, minvalues2));
    minindices2 = mm_blendv_epi8(minindices2, indices2, lt2);
    minvalues2 = _mm_min_ps(values2, minvalues2);
  }
  /*
   * Do the final calculation scalar way
   */
  alignas(alignment) float distances[8];
  alignas(alignment) int32_t indices[8];
  _mm_store_ps(distances, minvalues1);
  _mm_store_ps(distances + 4, minvalues2);
  _mm_store_si128((__m128i*)(indices), minindices1);
  _mm_store_si128((__m128i*)(indices + 4), minindices2);

  int32_t minIndex = indices[0];
  float minDistance = distances[0];
  for (int i = 1; i < 8; ++i) {
    if (distances[i] < minDistance) {
      minIndex = indices[i];
      minDistance = distances[i];
    }
  }
  return minIndex;
}
#endif // end of x86_64 versions

/* Always fall back to a simple default version with no intrinsics */
__attribute__((target("default")))
#endif // HAVE_FUNCTION_MULTIVERSIONING

int32_t
findMinimumIndex(const floatPtrRestrict distancesIn, const int n)
{
  float* array = (float*)__builtin_assume_aligned(distancesIn, alignment);
  float minvalue = array[0];
  size_t minIndex = 0;
  for (int i = 0; i < n; ++i) {
    const float value = array[i];
    if (value < minvalue) {
      minIndex = i;
      minvalue = value;
    }
  }
  return minIndex;
}

/*
 * Find Minimum Index STL style.
 * This works quite well in Clang8 but not in gcc8
 * https://its.cern.ch/jira/projects/ATLASRECTS/issues/ATLASRECTS-5244
 */
int32_t
findMinimumIndexSTL(const floatPtrRestrict distancesIn, const int n)
{
  float* array = (float*)__builtin_assume_aligned(distancesIn, alignment);
  return std::distance(array, std::min_element(array, array + n));
}

/*
 * Find the index of the 2 smaller values
 */
std::pair<int32_t, int32_t>
findMinimumIndexPair(const floatPtrRestrict distancesIn, const int n)
{

  float* distances = (float*)__builtin_assume_aligned(distancesIn, alignment);
  int32_t mini = 0;
  int32_t mini2 = -1;
  float minDistance = distances[0];
  float minDistance2 = std::numeric_limits<float>::max();

  for (int i = 1; i < n; ++i) {
    if (distances[i] < minDistance) {
      mini2 = mini;
      minDistance2 = minDistance;
      mini = i;
      minDistance = distances[i];
    } else if (distances[i] < minDistance2) {
      mini2 = i;
      minDistance2 = distances[i];
    }
  }

  return std::make_pair(mini, mini2);
}
int32_t
recalculateDistances(const componentPtrRestrict componentsIn,
                     floatPtrRestrict distancesIn,
                     const int32_t mini,
                     const int32_t n)
{
  const Component1D* components =
    static_cast<const Component1D*>(__builtin_assume_aligned(componentsIn, alignment));
  float* distances = static_cast<float*>(__builtin_assume_aligned(distancesIn, alignment));

  const int32_t j = mini;
  const int32_t indexConst = (j + 1) * j / 2;

  int32_t minIndex = 0;
  float minDistance = std::numeric_limits<float>::max();

  const Component1D componentJ = components[j];

  for (int32_t i = 0; i < j; ++i) {
    const Component1D componentI = components[i];
    const int32_t index = indexConst + i;
    if (componentI.cov == 0) {
      distances[index] = std::numeric_limits<float>::max();
      continue;
    }
    const double meanDifference = componentI.mean - componentJ.mean;
    const double covarianceDifference = componentI.cov - componentJ.cov;
    const double invertCovDiff = componentI.invCov - componentJ.invCov;
    const double inverCovSum = componentI.invCov + componentJ.invCov;
    distances[index] = covarianceDifference * invertCovDiff +
                       meanDifference * inverCovSum * meanDifference;
    if (distances[index] < minDistance) {
      minIndex = index;
      minDistance = distances[index];
    }
  }
  for (int32_t i = j + 1; i < n; ++i) {
    const int32_t index = (i + 1) * i / 2 + j;
    const Component1D componentI = components[i];
    if (componentI.cov == 0) {
      distances[index] = std::numeric_limits<float>::max();
      continue;
    }
    const double meanDifference = componentI.mean - componentJ.mean;
    const double covarianceDifference = componentI.cov - componentJ.cov;
    const double invertCovDiff = componentI.invCov - componentJ.invCov;
    const double inverCovSum = componentI.invCov + componentJ.invCov;
    distances[index] = covarianceDifference * invertCovDiff +
                       meanDifference * inverCovSum * meanDifference;
    if (distances[index] < minDistance) {
      minIndex = index;
      minDistance = distances[index];
    }
  }
  return minIndex;
}

// Calculate the distances for all pairs
void
calculateAllDistances(const componentPtrRestrict componentsIn,
                      floatPtrRestrict distancesIn,
                      const int32_t n)
{

  const Component1D* components =
  static_cast<const Component1D*>(__builtin_assume_aligned(componentsIn, alignment));
  float* distances = static_cast<float*>(__builtin_assume_aligned(distancesIn, alignment));

  for (int32_t i = 0; i < n; ++i) {
    const int32_t indexConst = (i + 1) * i / 2;
    const Component1D componentI = components[i];
    for (int32_t j = 0; j < i; ++j) {
      const Component1D componentJ = components[j];
      const double  meanDifference = componentI.mean - componentJ.mean;
      const double covarianceDifference = componentI.cov - componentJ.cov;
      const double invertCovDiff = componentI.invCov - componentJ.invCov;
      const double inverCovSum = componentI.invCov + componentJ.invCov;
      distances[indexConst + j] = covarianceDifference * invertCovDiff +
                                  meanDifference * inverCovSum * meanDifference;
    }
  }
}

// Reset the distances for a row
void
resetDistances(floatPtrRestrict distancesIn,
               const int32_t mini,
               const int32_t n)
{

  float* distances = (float*)__builtin_assume_aligned(distancesIn, alignment);
  const int32_t j = mini;
  const int32_t indexConst = (j + 1) * j / 2;
  for (int32_t i = 0; i < j; ++i) {
    distances[indexConst + i] = std::numeric_limits<float>::max();
  }

  for (int32_t i = j; i < n; ++i) {
    const int32_t index = (i + 1) * i / 2 + j;
    distances[index] = std::numeric_limits<float>::max();
  }
}

} // end namespace GSFUtils
