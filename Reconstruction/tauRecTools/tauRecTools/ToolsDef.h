/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "tauRecTools/TauCalibrateLC.h"
#include "tauRecTools/TauSubstructureVariables.h"
#include "tauRecTools/TauCommonCalcVars.h"
#include "tauRecTools/TauGenericPi0Cone.h"
#include "tauRecTools/TauIDPileupCorrection.h"
#include "tauRecTools/TauTrackFilter.h"
#include "tauRecTools/MvaTESVariableDecorator.h"
#include "tauRecTools/MvaTESEvaluator.h"
#include "tauRecTools/CombinedP4FromRecoTaus.h"
#include "tauRecTools/TauTrackClassifier.h"
#include "tauRecTools/TauChargedPFOCreator.h"
#include "tauRecTools/TauWPDecorator.h"
#include "tauRecTools/TauJetBDTEvaluator.h"
#include "tauRecTools/TauIDVarCalculator.h"
#include "tauRecTools/TauEleOLRDecorator.h"
#include "tauRecTools/TauProcessorTool.h"
#include "tauRecTools/BuildTruthTaus.h"

REGISTER_TOOL(TauCalibrateLC)
REGISTER_TOOL(TauCommonCalcVars)
REGISTER_TOOL(TauGenericPi0Cone)
REGISTER_TOOL(TauIDPileupCorrection)
REGISTER_TOOL(TauTrackFilter)
REGISTER_TOOL(TauSubstructureVariables)
REGISTER_TOOL(MvaTESVariableDecorator)
REGISTER_TOOL(MvaTESEvaluator)
REGISTER_TOOL(CombinedP4FromRecoTaus)
REGISTER_TOOL(tauRecTools::TauTrackClassifier)
REGISTER_TOOL(tauRecTools::TrackMVABDT)
REGISTER_TOOL(TauChargedPFOCreator)
REGISTER_TOOL(TauWPDecorator)
REGISTER_TOOL(TauJetBDTEvaluator)
REGISTER_TOOL(TauEleOLRDecorator)
REGISTER_TOOL(TauIDVarCalculator)
REGISTER_TOOL(TauProcessorTool)
REGISTER_TOOL(tauRecTools::BuildTruthTaus)
