/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAUTILS_WEIGHTEDMEANCALC_H
#define EGAMMAUTILS_WEIGHTEDMEANCALC_H

#include "egammaUtils/EigenVectorAndMatrix.h"

namespace WeightedMeanCalc {
  EigenVectorAndMatrix getWeightedMean(const EigenVectorAndMatrix& vm1, const EigenVectorAndMatrix& vm2);	
}

#endif
