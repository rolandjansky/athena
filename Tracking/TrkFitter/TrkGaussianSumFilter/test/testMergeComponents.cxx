/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkGaussianSumFilter/AlignedDynArray.h"
#include "TrkGaussianSumFilter/KLGaussianMixtureReduction.h"
#include <array>
#include <iostream>

using namespace GSFUtils;
namespace {
constexpr int16_t n =72;
constexpr std::array<Component1D,n> input = {
  { { -4.66462e-06, 1.06618e-11, 9.37928e+10, 0.00608503 },
    { -2.08263e-05, 7.533e-11, 1.32749e+10, 0.0274963 },
    { -4.15487e-05, 4.79975e-11, 2.08344e+10, 0.0591673 },
    { -4.86464e-05, 7.57086e-12, 1.32085e+11, 0.0648575 },
    { -5.1025e-05, 3.16755e-12, 3.15701e+11, 0.0952169 },
    { -5.15536e-05, 2.9173e-12, 3.42782e+11, 0.536818 },
    { -4.71833e-06, 1.1939e-11, 8.3759e+10, 0.000587903 },
    { -2.10663e-05, 7.81055e-11, 1.28032e+10, 0.0026566 },
    { -4.20273e-05, 5.01395e-11, 1.99443e+10, 0.00571686 },
    { -4.92063e-05, 8.77638e-12, 1.13942e+11, 0.00626678 },
    { -5.16121e-05, 4.27124e-12, 2.34124e+11, 0.00920053 },
    { -5.21468e-05, 4.01522e-12, 2.49052e+11, 0.0518911 },
    { -4.58728e-06, 1.12379e-11, 8.89846e+10, 0.000489032 },
    { -2.0481e-05, 7.37793e-11, 1.35539e+10, 0.00220977 },
    { -4.08598e-05, 4.73457e-11, 2.11212e+10, 0.00475507 },
    { -4.78398e-05, 8.24862e-12, 1.21232e+11, 0.00521237 },
    { -5.01789e-05, 3.99012e-12, 2.50619e+11, 0.00765225 },
    { -5.06988e-05, 3.74811e-12, 2.66801e+11, 0.0431424 },
    { -4.78054e-06, 1.18292e-11, 8.45367e+10, 0.000296965 },
    { -2.1344e-05, 7.97519e-11, 1.25389e+10, 0.00134192 },
    { -4.25814e-05, 5.10436e-11, 1.95911e+10, 0.00288774 },
    { -4.9855e-05, 8.5826e-12, 1.16515e+11, 0.00316553 },
    { -5.22926e-05, 3.9579e-12, 2.52659e+11, 0.00464746 },
    { -5.28342e-05, 3.69508e-12, 2.7063e+11, 0.0262122 },
    { -4.46758e-06, 1.74302e-11, 5.73717e+10, 0.000201451 },
    { -1.99466e-05, 7.67504e-11, 1.30293e+10, 0.000910294 },
    { -3.97936e-05, 5.16782e-11, 1.93505e+10, 0.00195881 },
    { -4.65914e-05, 1.45949e-11, 6.85173e+10, 0.0021472 },
    { -4.88695e-05, 1.05557e-11, 9.47354e+10, 0.0031523 },
    { -4.93758e-05, 1.03262e-11, 9.68413e+10, 0.017773 },
    { -3.9244e-06, 1.94312e-11, 5.14636e+10, 2.28683e-05 },
    { -1.75215e-05, 6.52038e-11, 1.53365e+10, 0.000103336 },
    { -3.49554e-05, 4.58576e-11, 2.18066e+10, 0.000222367 },
    { -4.09266e-05, 1.72434e-11, 5.79933e+10, 0.000243755 },
    { -4.29277e-05, 1.41268e-11, 7.07877e+10, 0.000357862 },
    { -4.33724e-05, 1.39496e-11, 7.16864e+10, 0.00201799 },
    { -4.38853e-06, 1.66176e-11, 6.01772e+10, 1.22764e-05 },
    { -1.95937e-05, 7.38574e-11, 1.35396e+10, 5.54734e-05 },
    { -3.90896e-05, 4.96645e-11, 2.01351e+10, 0.000119371 },
    { -4.57671e-05, 1.38817e-11, 7.20373e+10, 0.000130852 },
    { -4.80049e-05, 9.98424e-12, 1.00158e+11, 0.000192104 },
    { -4.85022e-05, 9.76274e-12, 1.0243e+11, 0.00108316 },
    { -3.45701e-06, 5.26509e-11, 1.8993e+10, 8.66949e-06 },
    { -1.54349e-05, 8.81702e-11, 1.13417e+10, 3.91764e-05 },
    { -3.07925e-05, 7.31575e-11, 1.36691e+10, 8.43101e-05 },
    { -3.60521e-05, 5.09531e-11, 1.96259e+10, 9.24221e-05 },
    { -3.78147e-05, 4.85347e-11, 2.06038e+10, 0.000135694 },
    { -3.82063e-05, 4.83973e-11, 2.06623e+10, 0.000765623 },
    { -4.94321e-06, 1.29867e-11, 7.70021e+10, 7.54445e-07 },
    { -2.20705e-05, 8.56111e-11, 1.16807e+10, 3.40926e-06 },
    { -4.40305e-05, 5.49154e-11, 1.82098e+10, 7.33698e-06 },
    { -5.15512e-05, 9.51523e-12, 1.05095e+11, 8.04293e-06 },
    { -5.40715e-05, 4.57064e-12, 2.18788e+11, 1.18086e-05 },
    { -5.46316e-05, 4.28965e-12, 2.33119e+11, 6.66306e-05 },
    { -2.67066e-06, 8.51914e-12, 1.17383e+11, 4.90461e-07 },
    { -1.1924e-05, 2.97175e-11, 3.36503e+10, 2.21633e-06 },
    { -2.37883e-05, 2.07577e-11, 4.81749e+10, 4.76964e-06 },
    { -2.78515e-05, 7.50587e-12, 1.33229e+11, 5.22854e-06 },
    { -2.92132e-05, 6.06258e-12, 1.64946e+11, 7.67648e-06 },
    { -2.95158e-05, 5.98056e-12, 1.67208e+11, 4.33103e-05 },
    { -4.75742e-06, 1.31378e-11, 7.61161e+10, 2.66875e-07 },
    { -2.1241e-05, 8.04056e-11, 1.24369e+10, 1.20598e-06 },
    { -4.23756e-05, 5.1974e-11, 1.92404e+10, 2.59534e-06 },
    { -4.96137e-05, 9.92245e-12, 1.00782e+11, 2.84505e-06 },
    { -5.20393e-05, 5.34254e-12, 1.87177e+11, 4.17709e-06 },
    { -5.25783e-05, 5.08227e-12, 1.96762e+11, 2.35683e-05 },
    { -4.13925e-06, 1.32939e-11, 7.52224e+10, 7.37356e-10 },
    { -1.84807e-05, 6.42154e-11, 1.55726e+10, 3.33188e-09 },
    { -3.68691e-05, 4.2693e-11, 2.3423e+10, 7.16968e-09 },
    { -4.31674e-05, 1.086e-11, 9.20809e+10, 7.85921e-09 },
    { -4.5278e-05, 7.39274e-12, 1.35268e+11, 1.15381e-08 },
    { -4.57471e-05, 7.19569e-12, 1.38972e+11, 6.50527e-08 } }
};
}

int
main()
{
  AlignedDynArray<Component1D, alignment> components(n);
  // Create an array of all components to be merged
  for (int8_t i = 0; i < n; ++i) {
    components[i].mean = input[i].mean;
    components[i].cov = input[i].cov;
    components[i].invCov = input[i].invCov;
    components[i].weight = input[i].weight;
  }
  std::vector<std::pair<int8_t, int8_t>> mergeOrder =
    findMerges(components.buffer(), n, 12);
  for (const auto& i : mergeOrder){
    std::cout << "[" << i.first << ", " << i.second << "]" << '\n';
  }
  return 0;
}

