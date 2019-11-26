/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
  KLGaussianMixReductionUtils.h  -  description
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

namespace KLGaussianMixtureReduction{


typedef float* ATH_RESTRICT floatPtrRestrict;

constexpr int alignment =32;
// Recalculate the distances for a row of pairs and return the index of the minimum pair
int recalculateDistances(floatPtrRestrict qonpIn,
                         floatPtrRestrict qonpCovIn,
                         floatPtrRestrict qonpGIn,
                         floatPtrRestrict distancesIn,
                         int mini,
                         int n);

// Calculate the distances for all pairs
void calculateAllDistances(floatPtrRestrict qonpIn,
                           floatPtrRestrict qonpCovIn,
                           floatPtrRestrict qonpGIn,
                           floatPtrRestrict distancesIn,
                           int n) ;

// Reset the distances for a row
void resetDistances(floatPtrRestrict distancesIn, const int mini, const int n) ;


std::pair<int, int> findMinimumIndex(const floatPtrRestrict distancesIn, const int n) ;


}


#endif
