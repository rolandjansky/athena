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

/**
 *   Assuming 0..... N  (N+1 elements)
 *   the pairwise distance matrix  can be represented in a trangular way:
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
 *   qonpIn,qonpCovIn,qonpGIn have size N+1 [0......N]
 *   distancesIn  has size N*(N+1)/2
 *
 *   We also want to map the position in the distance array to the (i,j) of
 *   the elements
 *   0--> (0,0)
 *   1-->(1,0)
 *   2--> (1,1)
 *
 */

struct triangularToIJ{
  int32_t I=-1;
  int32_t J=-1;
};
typedef float* ATH_RESTRICT floatPtrRestrict;

constexpr int32_t alignment =32;

//find the  index of the smaller value

#if HAVE_FUNCTION_MULTIVERSIONING
#if defined(__x86_64__)
__attribute__((target("avx2"))) 
int32_t findMinimumIndex(const floatPtrRestrict distancesIn, const int32_t n);
__attribute__((target("sse4.2,sse2"))) 
int32_t findMinimumIndex(const floatPtrRestrict distancesIn, const int32_t n);
#endif //x86_64 specific targets
__attribute__((target("default"))) 
#endif// function multiversioning
int32_t findMinimumIndex(const floatPtrRestrict distancesIn, const int32_t n);
//find the index of the smaller value STL style
int32_t findMinimumIndexSTL(const floatPtrRestrict distancesIn, const int n);

//find a pair of the  indices of the 2 smaller values
std::pair<int32_t, int32_t> findMinimumIndexPair(const floatPtrRestrict distancesIn, const int32_t n) ;

// Recalculate the distances for a row of pairs and return the index of the minimum pair
int32_t recalculateDistances(floatPtrRestrict qonpIn,
                         floatPtrRestrict qonpCovIn,
                         floatPtrRestrict qonpGIn,
                         floatPtrRestrict distancesIn,
                         int32_t mini,
                         int32_t n);

// Calculate the distances for all pairs
void calculateAllDistances(floatPtrRestrict qonpIn,
                           floatPtrRestrict qonpCovIn,
                           floatPtrRestrict qonpGIn,
                           floatPtrRestrict distancesIn,
                           int32_t n) ;

// Reset the distances for a row
void resetDistances(floatPtrRestrict distancesIn, const int32_t mini, const int32_t n) ;

} // namespace KLGaussianMixtureReduction

#endif
