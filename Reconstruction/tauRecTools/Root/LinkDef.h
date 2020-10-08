/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRecTools/TauCalibrateLC.h"
#include "tauRecTools/TauCommonCalcVars.h"
#include "tauRecTools/TauSubstructureVariables.h"
#include "tauRecTools/TauRecToolBase.h"
#include "tauRecTools/ITauToolBase.h"
#include "tauRecTools/MvaTESVariableDecorator.h"
#include "tauRecTools/MvaTESEvaluator.h"
#include "tauRecTools/TauTrackRNNClassifier.h"
#include "tauRecTools/TauTrackClassifier.h"
#include "tauRecTools/CombinedP4FromRecoTaus.h"
#include "tauRecTools/TauWPDecorator.h"
#include "tauRecTools/TauJetBDTEvaluator.h"
#include "tauRecTools/TauIDVarCalculator.h"
#include "tauRecTools/TauJetRNNEvaluator.h"
#include "tauRecTools/TauDecayModeNNClassifier.h"
#include "tauRecTools/TauVertexCorrection.h"

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

#pragma link C++ class TauCalibrateLC+;
#pragma link C++ class TauCommonCalcVars+;
#pragma link C++ class TauSubstructureVariables+;
#pragma link C++ class TauRecToolBase+;
#pragma link C++ class ITauToolBase+;
#pragma link C++ class MvaTESVariableDecorator+;
#pragma link C++ class MvaTESEvaluator+;
#pragma link C++ class tauRecTools::TauTrackClassifier+;
#pragma link C++ class tauRecTools::TrackMVABDT+;
#pragma link C++ class tauRecTools::TrackRNN+;
#pragma link C++ class tauRecTools::TauTrackRNNClassifier+;
#pragma link C++ class CombinedP4FromRecoTaus+;
#pragma link C++ class TauWPDecorator+;
#pragma link C++ class TauJetBDTEvaluator+;
#pragma link C++ class TauIDVarCalculator+;
#pragma link C++ class TauJetRNNEvaluator+;
#pragma link C++ class TauDecayModeNNClassifier+;
#pragma link C++ class ITauVertexCorrection+;
#pragma link C++ class TauVertexCorrection+;

#endif
