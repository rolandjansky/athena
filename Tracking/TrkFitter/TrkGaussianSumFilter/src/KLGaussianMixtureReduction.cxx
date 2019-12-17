/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkGaussianSumFilter/KLGaussianMixtureReduction.h"
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

void
KLGaussianMixtureReduction::resetDistances(floatPtrRestrict distancesIn,
                                           const int mini,
                                           const int n) 
{

  float* distances = (float*)__builtin_assume_aligned(distancesIn, alignment);
  int j = mini;
  int indexConst = (j + 1) * j / 2;
  for (int i = 0; i < j; ++i) {
    distances[indexConst + i] = std::numeric_limits<float>::max();
  }

  for (int i = j; i < n; ++i) {
    int index = (i + 1) * i / 2 + j;
    distances[index] = std::numeric_limits<float>::max();
  }
}

void
KLGaussianMixtureReduction::calculateAllDistances(floatPtrRestrict qonpIn,
                                                  floatPtrRestrict qonpCovIn,
                                                  floatPtrRestrict qonpGIn,
                                                  floatPtrRestrict distancesIn,
                                                  int n) 
{

  float* qonp = (float*)__builtin_assume_aligned(qonpIn, alignment);
  float* qonpCov = (float*)__builtin_assume_aligned(qonpCovIn, alignment);
  float* qonpG = (float*)__builtin_assume_aligned(qonpGIn, alignment);
  float* distances = (float*)__builtin_assume_aligned(distancesIn, alignment);

  for (int i = 0; i < n; ++i) {
    int indexConst = (i + 1) * i / 2;
    float qonpi = qonp[i];
    float qonpCovi = qonpCov[i];
    float qonpGi = qonpG[i];
    for (int j = 0; j < i; ++j) {
      float parametersDifference = qonpi - qonp[j];
      float covarianceDifference = qonpCovi - qonpCov[j];
      float G_difference = qonpG[j] - qonpGi;
      float G_sum = qonpGi + qonpG[j];
      distances[indexConst + j] =
        covarianceDifference * G_difference + parametersDifference * G_sum * parametersDifference;

     //\log \frac{\sigma_2}{\sigma_1} + \frac{\sigma_1^2 + (\mu_1 - \mu_2)^2}{2 \sigma_2^2} - \frac{1}{2}
    }
  }
}

int
KLGaussianMixtureReduction::recalculateDistances(floatPtrRestrict qonpIn,
                                                 floatPtrRestrict qonpCovIn,
                                                 floatPtrRestrict qonpGIn,
                                                 floatPtrRestrict distancesIn,
                                                 int mini,
                                                 int n) 
{

  float* qonp = (float*)__builtin_assume_aligned(qonpIn, alignment);
  float* qonpCov = (float*)__builtin_assume_aligned(qonpCovIn, alignment);
  float* qonpG = (float*)__builtin_assume_aligned(qonpGIn, alignment);
  float* distances = (float*)__builtin_assume_aligned(distancesIn, alignment);

  int j = mini;
  int indexConst = (j + 1) * j / 2;

  int minIndex = 0; // The distance will always be 1e30 for this entry;
  float minDistance = std::numeric_limits<float>::max();

  for (int i = 0; i < j; ++i) {
    if (qonpCov[i] == 0) {
      distances[indexConst + i] = std::numeric_limits<float>::max();
      continue;
    }
    float parametersDifference = qonp[i] - qonp[j];
    float covarianceDifference = qonpCov[i] - qonpCov[j];
    float G_difference = qonpG[j] - qonpG[i];
    float G_sum = qonpG[i] + qonpG[j];
    int index = indexConst + i;
    distances[index] = covarianceDifference * G_difference + parametersDifference * G_sum * parametersDifference;
    if (distances[index] < minDistance) {
      minIndex = index;
      minDistance = distances[index];
    }
  }

  for (int i = j + 1; i < n; ++i) {
    int index = (i + 1) * i / 2 + j;
    if (qonpCov[i] == 0) {
      distances[index] = std::numeric_limits<float>::max();
      continue;
    }
    float parametersDifference = qonp[i] - qonp[j];
    float covarianceDifference = qonpCov[i] - qonpCov[j];
    float G_difference = qonpG[j] - qonpG[i];
    float G_sum = qonpG[i] + qonpG[j];
    distances[index] = covarianceDifference * G_difference + parametersDifference * G_sum * parametersDifference;
    if (distances[index] < minDistance) {
      minIndex = index;
      minDistance = distances[index];
    }
  }
  return minIndex;
}

std::pair<int, int>
KLGaussianMixtureReduction::findMinimumIndex(const floatPtrRestrict distancesIn, const int n) 
{

  float* distances = (float*)__builtin_assume_aligned(distancesIn, alignment);
  int mini = 0; // always 1e30
  int mini2 = -1;
  float minDistance = distances[0];
  float minDistance2 = std::numeric_limits<float>::max();

  for (int i = 1; i < n; ++i) {
    if (distances[i] < minDistance) {
      mini2 = mini;
      minDistance2 = minDistance;
      mini = i;
      minDistance = distances[i];
    } else if (distances[i] < minDistance2){
      mini2 = i;
      minDistance2 = distances[i];
    }
  }

  return std::make_pair(mini, mini2);
}


