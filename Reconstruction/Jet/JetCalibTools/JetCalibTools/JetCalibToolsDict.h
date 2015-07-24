/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETCALIBTOOLS_JETCALIBTOOLSDICT_H
#define JETCALIBTOOLS_JETCALIBTOOLSDICT_H 1

// Reflex dictionary generation
// Following instructions on the CP tools twiki:
// https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/DevelopingCPToolsForxAOD
// Special handling for Eigen vectorization

#if defined(__GCCXML__) and not defined(EIGEN_DONT_VECTORIZE)
#define EIGEN_DONT_VECTORIZE
#endif

#include "JetCalibTools/JetCalibrationTool.h"

#endif
