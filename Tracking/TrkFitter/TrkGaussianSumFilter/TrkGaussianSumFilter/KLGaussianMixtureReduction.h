/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file  KLGaussianMixtureReduction.h
 * @author Anthony Morley , Christos Anastopoulos
 * @date 26th November 2019
 *
 *
 * @brief Uyilities to facilitate the calculation of the
 * divergence between components of the mixture
 * and the merging of similar componets.
 *
 *
 * For a summary  of available methods look
 * https://arxiv.org/pdf/2001.00727.pdf
 *
 * Here we opt for formula 10.
 * For an 1D Normal distributions this becomes:
 *
 * (variance1/variance2) * (variance2/variance1) +
 * (mean1-mean2) * ((1/variance1)+(1/variance2))(mean1-mean2)
 *
 * We use doubles for the intermediate calculations
 * but we store the final distances to short in an array
 * of floats.
 *
 *
 * For pairwise distance comparisons assuming 0 ... N-1 (N total elements )
 * The pairwise distance matrix  can be represented in a triangular array:
 * [ (1,0) ] <br>
 * [ (2,0), (2,1) ] <br>
 * [ (3,0), (3,1), (3,2)] <br>
 * [ (4,0), (4,1), (4,2) , (4,3) <br>
 * [.............................] <br>
 * [(N-1,0),(N-1,1),(N-1,2),(N-1,3) ... (N-1,N-2)]<br>
 *
 * With size 1+2+3+ .... (N-1) = N*(N-1)/2
 *
 * The lexicographical storage allocation function is <br>
 * Loc(i,j) = i*(i-1)/2 + j <br>
 * e.g <br>
 * (1,0) => 1 *(1-1)/2 + 0 => 0 <br>
 * (2,0) => 2 *(2-1)/2 + 0 => 1 <br>
 * (2,1) => 2 *(2-1)/2 + 1 => 2 <br>
 * (3,0) => 3 * (3-1)/2 +0 => 3 <br>
 *
 * Leading to <br>
 * [(1,0),(2,0),(2,1),(3,0),(3,1),(3,2).... (N-1,N-2)]
 *
 *
 * The N-1 Rows  map to the value K of the 1st element in the pair. <br>
 * where K=1,2,3,..,N-1  and each Row has size K. <br>
 * Each Row starts at array positions K*(K-1)/2 <br>
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
 *
 */

#ifndef KLGaussianMixReductionUtils_H
#define KLGaussianMixReductionUtils_H

#include "CxxUtils/features.h"
#include <cstdint>
#include <utility>
#include <vector>

namespace GSFUtils {

/**
 * @brief Alignment used  for SIMD
 */
constexpr size_t alignment = 32;

/**
 * @brief struct representing 1D component
 * Negative weight means invalidated component
 */
struct Component1D
{
  double mean = 0.;
  double cov = 0.;
  double invCov = 1e10;
  double weight = 0.;
};

/**
 * @brief Merge the componentsIn and return
 * which componets got merged
 *
 * inputSize is expected to be >0, <128
 * and reducedSize < inputsize. Invalid input
 * will cause a runtime exception
 *
 * Furthemore, the input component array is assumed to be
 * GSFUtils::alignment aligned.
 * Can be created via the AlignedDynArray.
 */
std::vector<std::pair<int16_t, int16_t>>
findMerges(Component1D* componentsIn,
           const int16_t inputSize,
           const int16_t reducedSize);

/**
 * @brief For finding the index of the minumum pairwise distance
 * we opt for SIMD and function multiversioning
 */
#if HAVE_FUNCTION_MULTIVERSIONING
#if defined(__x86_64__)
__attribute__((target("avx2"))) int32_t
findMinimumIndex(const float* distancesIn, const int32_t n);

__attribute__((target("sse4.1"))) int32_t
findMinimumIndex(const float* distancesIn, const int32_t n);
#endif // x86_64 specific targets

__attribute__((target("default")))
#endif // function multiversioning
int32_t
findMinimumIndex(const float* distancesIn, const int32_t n);

} // namespace KLGaussianMixtureReduction

#endif
