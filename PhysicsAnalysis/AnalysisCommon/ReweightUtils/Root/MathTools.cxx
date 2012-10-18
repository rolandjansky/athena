/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define MathTools_cxx
#include "ReweightUtils/MathTools.h"

#include <cmath>

double ln_factorial(unsigned int value) {
  double result = 0.;
  for (int k = value ; k > 0 ; --k) {
    result += std::log(k);
  }
  return result;
}

double ln_factorialApp(unsigned int value) {
  return 0.5*std::log(1.04719755119659763+6.28318530717958623*value) + value*(std::log(value)-1.);
}
