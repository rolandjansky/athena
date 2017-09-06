/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STANDALONE//for standalone compilation only
#include "TopFakes/TopFakesMMWeightCalculator.h"
#endif
#include "TopFakes/FakesWeights.h"
#include "TopFakes/MMEffSet.h"
#include "TopFakes/MMEfficiency.h"
#include "TopFakes/MatrixUtils.h"

#ifdef __CINT__

#ifndef STANDALONE//for standalone compilation only
#pragma extra_include "TopFakes/TopFakesMMWeightCalculator.h";
#endif
#pragma extra_include "TopFakes/FakesWeights.h";
#pragma extra_include "TopFakes/MMEffSet.h";
#pragma extra_include "TopFakes/MMEfficiency.h";
#pragma extra_include "TopFakes/MatrixUtils.h";

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

//for loading the object selection at run time
#ifndef STANDALONE//for standalone compilation only
#pragma link C++ class top::TopFakesMMWeightCalculator+;
#endif
#pragma link C++ class FakesWeights+;
#pragma link C++ class MMEffSet+;
#pragma link C++ class MMEfficiency+;
#pragma link C++ class MatrixUtils+;

#endif
