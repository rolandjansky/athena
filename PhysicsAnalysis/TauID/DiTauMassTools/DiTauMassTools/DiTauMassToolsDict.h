/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DITAUMASSTOOLS_DITAUMASSTOOLSDICT_H
#define DITAUMASSTOOLS_DITAUMASSTOOLSDICT_H


// Special handling for Eigen vectorization
#if defined(__GCCXML__) and not defined(EIGEN_DONT_VECTORIZE)
#define EIGEN_DONT_VECTORIZE
#endif

#include "DiTauMassTools/IMissingMassTool.h"
#include "DiTauMassTools/MissingMassTool.h"

#endif
