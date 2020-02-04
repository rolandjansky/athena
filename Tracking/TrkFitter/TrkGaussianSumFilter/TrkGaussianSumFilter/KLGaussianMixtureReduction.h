/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include <utility>

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

struct triangularToIJ
{
  int I = -1;
  int J = -1;
};

typedef float* ATH_RESTRICT floatPtrRestrict;

constexpr int alignment = 32;
// Recalculate the distances for a row of pairs and return the index of the minimum pair
int
recalculateDistances(floatPtrRestrict qonpIn,
                     floatPtrRestrict qonpCovIn,
                     floatPtrRestrict qonpGIn,
                     floatPtrRestrict distancesIn,
                     int mini,
                     int n);

// Calculate the distances for all pairs
void
calculateAllDistances(floatPtrRestrict qonpIn,
                      floatPtrRestrict qonpCovIn,
                      floatPtrRestrict qonpGIn,
                      floatPtrRestrict distancesIn,
                      int n);

// Reset the distances for a row
void
resetDistances(floatPtrRestrict distancesIn, const int mini, const int n);

std::pair<int, int>
findMinimumIndex(const floatPtrRestrict distancesIn, const int n);

} // namespace KLGaussianMixtureReduction

#endif
