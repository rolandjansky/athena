/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAUTILS_PARAMETERDOWNWEIGHTER_H
#define EGAMMAUTILS_PARAMETERDOWNWEIGHTER_H

#include "egammaUtils/EigenVectorAndMatrix.h"

namespace ParameterDownWeighter {
  void getDownWeightedVM(EigenVectorAndMatrix& vm1, const EigenVectorAndMatrix& vm2, int indx);
}

#endif
