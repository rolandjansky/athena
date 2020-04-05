/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "tauRecTools/TauCalibrateLC.h"
#include "tauRecTools/TauSubstructureVariables.h"
#include "tauRecTools/TauCommonCalcVars.h"
#include "tauRecTools/MvaTESVariableDecorator.h"
#include "tauRecTools/MvaTESEvaluator.h"
#include "tauRecTools/CombinedP4FromRecoTaus.h"
#include "tauRecTools/TauTrackClassifier.h"
#include "tauRecTools/TauTrackRNNClassifier.h"
#include "tauRecTools/TauWPDecorator.h"
#include "tauRecTools/TauJetBDTEvaluator.h"
#include "tauRecTools/TauIDVarCalculator.h"
#include "tauRecTools/TauEleOLRDecorator.h"
#include "tauRecTools/TauJetRNNEvaluator.h"

REGISTER_TOOL(TauCalibrateLC)
REGISTER_TOOL(TauCommonCalcVars)
REGISTER_TOOL(TauSubstructureVariables)
REGISTER_TOOL(MvaTESVariableDecorator)
REGISTER_TOOL(MvaTESEvaluator)
REGISTER_TOOL(CombinedP4FromRecoTaus)
REGISTER_TOOL(tauRecTools::TauTrackClassifier)
REGISTER_TOOL(tauRecTools::TauTrackRNNClassifier)
REGISTER_TOOL(tauRecTools::TrackMVABDT)
REGISTER_TOOL(tauRecTools::TrackRNN)
REGISTER_TOOL(TauWPDecorator)
REGISTER_TOOL(TauJetBDTEvaluator)
REGISTER_TOOL(TauEleOLRDecorator)
REGISTER_TOOL(TauIDVarCalculator)
REGISTER_TOOL(TauJetRNNEvaluator)
