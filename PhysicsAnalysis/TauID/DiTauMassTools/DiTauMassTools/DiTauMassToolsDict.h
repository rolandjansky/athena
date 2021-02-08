/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DITAUMASSTOOLS_DITAUMASSTOOLSDICT_H
#define DITAUMASSTOOLS_DITAUMASSTOOLSDICT_H


// Special handling for Eigen vectorization
#if defined(__GCCXML__) and not defined(EIGEN_DONT_VECTORIZE)
#define EIGEN_DONT_VECTORIZE
#endif

#include "DiTauMassTools/IMissingMassTool.h"
#include "DiTauMassTools/MissingMassTool.h"

#include "DiTauMassTools/HelperFunctions.h"
#include "DiTauMassTools/MissingMassInput.h"
#include "DiTauMassTools/MissingMassOutput.h"
#include "DiTauMassTools/MissingMassProb.h"
#include "DiTauMassTools/MissingMassCalculatorV2.h"
#include "DiTauMassTools/MissingMassToolV2.h"
#endif
