/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STRANSVERSEMASSUTILS_MT2DEFS_H
#define STRANSVERSEMASSUTILS_MT2DEFS_H

//////////////////////////////////////////////////
//mT2Defs
//This includes some useful typedefs and calculator types.
//The enum Mt2CalculatorType is a list a available calculator
//types.  
//The typedefs are defined to be the necessary base classes
//of the ESD/AOD level objects one is anticipated to 
//encounter, so don't worry ElectronAODContainer is full
//of type<const Electron *> or something.
//////////////////////////////////////////////////

#include "EventKernel/I4Momentum.h"
#include "MissingETEvent/MissingET.h"

#include "Mt2/Mt2Vectors.h"

namespace Mt2{
  enum Mt2_332_CalculatorType{
    Basic332=0,
    Bisect
  };

  enum Mt2_330_CalculatorType{
    Analytic330=0
  };

  typedef I4Momentum * Atl4Vec;
  typedef const MissingET * AtlMET;



}



#endif
