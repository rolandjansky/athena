/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
  KLGaussianMixReduction.h  -  description
  -------------------------------------------------
begin                : 26th November 2019
author               : amorley, Christos
decription           : Function that help implement a
"Kullback-Leibler Approach to Gaussian Mixture Reduction"
 *********************************************************************************/

#ifndef KLGaussianMixReductionUtils_H
#define KLGaussianMixReductionUtils_H

#include "CxxUtils/restrict.h"
#include "CxxUtils/features.h"
#include <utility>  
#include <cstdint>

namespace GSFUtils {

/*
 * Alignment used  for SIMD
 */

constexpr int32_t alignment =32;

/**
 * The main pupropse of the utilities here
 * are to facilitate the calculation of the 
 * Kullback-Leible divergence between components
 * of the mixture and the merging of the componets.
 *
 * The dissimilarity measure/distance metric 
 * 2*[ D(P1||P2)+ D(P2||P1)]
 *
 * For 1D Normal distributions with this is given
 * by
 *
 * (variance1-variance2) (1/variance1 - 1/variance2) - 
 * (mean1-mean2)(1/variance+1/variance)(mean1-mean2)
*/

struct Component1D{
  float mean=0.;
  float cov=0.;
  float invCov=1e10; 
};

/**
 *   For pairwise distance comparisons assuming 0..... N  (N+1 elements)
 *   the pairwise distance matrix  can be represented in a triangular array:
 *
 *   [ (0,0)...............
 *   [ (1,0), (1,1)........
 *   [ (2,0), (2,1), (2,2)
 *   [.......................
 *   [.............................
 *   [...................................(N,N)]
 *
 *   Stored as
 *   [(0,0),(1,0),(1,1),(2,0),(2,1),(2,2)...... (N,N)]
 *   with size N*(N+1)/2
 *
 *   In the following methods:
 *   componentsIn has size N+1 [0......N]
 *   distancesIn  has size N*(N+1)/2
 *
 *   We also want to map the position in the distance array to the (i,j) of
 *   the elements
 *   0--> (0,0)
 *   1--> (1,0)
 *   2--> (1,1)
 *   3--> (2,0)
 *   e.g the lexicographical storage allocation function is
 *   Loc( i, j) = i*(i+1)/2 + j 
 */
struct triangularToIJ{
  int32_t I=-1;
  int32_t J=-1;
};

/*
 * Some usefull typedefs
 */
typedef float* ATH_RESTRICT floatPtrRestrict;
typedef Component1D* ATH_RESTRICT componentPtrRestrict;

/**
 * For finding the index of the minumum pairwise distance
 * we opt for SIMD and function multiversioning
 */
#if HAVE_FUNCTION_MULTIVERSIONING
#if defined(__x86_64__)
__attribute__((target("avx2")))
int32_t
findMinimumIndex(const floatPtrRestrict distancesIn, const int32_t n);
__attribute__((target("sse4.2,sse2"))) 
int32_t
findMinimumIndex(const floatPtrRestrict distancesIn, const int32_t n);
#endif //x86_64 specific targets
__attribute__((target("default")))
#endif// function multiversioning
int32_t
findMinimumIndex(const floatPtrRestrict distancesIn, const int32_t n);


/**
 * Extra methods for finding the index of the minimum
 * pairwise distance
 */
int32_t
findMinimumIndexSTL(const floatPtrRestrict distancesIn, const int n);
//find a pair of the  indices of the 2 smaller values
std::pair<int32_t, int32_t>
findMinimumIndexPair(const floatPtrRestrict distancesIn, const int32_t n);

/**
 * Recalculate the distances given a merged input 
 * and return the index of the minimum pair
 */
int32_t
recalculateDistances(const componentPtrRestrict componentsIn,
                     floatPtrRestrict distancesIn,
                     const int32_t mini,
                     const int32_t n);

// Calculate the distances for all pairs
void
calculateAllDistances(const componentPtrRestrict componentsIn,
                      floatPtrRestrict distancesIn,
                      const int32_t n);

// Reset the distances for a row
void
resetDistances(floatPtrRestrict distancesIn,
               const int32_t mini,
               const int32_t n);

} // namespace KLGaussianMixtureReduction

#endif
