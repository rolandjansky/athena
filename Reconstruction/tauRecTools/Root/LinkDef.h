/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRecTools/TauCalibrateLC.h"
#include "tauRecTools/TauCommonCalcVars.h"
#include "tauRecTools/TauGenericPi0Cone.h"
#include "tauRecTools/TauIDPileupCorrection.h"
#include "tauRecTools/TauTrackFilter.h"
#include "tauRecTools/TauSubstructureVariables.h"
#include "tauRecTools/TauRecToolBase.h"
#include "tauRecTools/ITauToolBase.h"
#include "tauRecTools/MvaTESVariableDecorator.h"
#include "tauRecTools/MvaTESEvaluator.h"
#include "tauRecTools/CombinedP4FromRecoTaus.h"

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

#endif


#ifdef __CINT__

#pragma link C++ class TauCalibrateLC+;
#pragma link C++ class TauCommonCalcVars+;
#pragma link C++ class TauGenericPi0Cone+;
#pragma link C++ class TauConversion+;
#pragma link C++ class TauIDPileupCorrection+;
#pragma link C++ class TauTrackFilter+;
#pragma link C++ class TauSubstructureVariables+;
#pragma link C++ class TauRecToolBase+;
#pragma link C++ class ITauToolBase+;
#pragma link C++ class MvaTESVariableDecorator+;
#pragma link C++ class MvaTESEvaluator+;
#pragma link C++ class CombinedP4FromRecoTaus+;

#endif
