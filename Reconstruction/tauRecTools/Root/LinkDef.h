/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "tauRecTools/FTauDecorator.h"
#include "tauRecTools/MvaTESEvaluator.h"
#include "tauRecTools/TauTrackClassifier.h"
#include "tauRecTools/CombinedP4FromRecoTaus.h"
#include "tauRecTools/TauChargedPFOCreator.h"
#include "tauRecTools/TauWPDecorator.h"
#include "tauRecTools/TauJetBDTEvaluator.h"
#include "tauRecTools/TauJetRNNEvaluator.h"
#include "tauRecTools/TauIDVarCalculator.h"
#include "tauRecTools/TauEleOLRDecorator.h"
#include "tauRecTools/BuildTruthTaus.h"
#include "tauRecTools/DiTauProcessorTool.h"
#include "tauRecTools/DiTauIDVarCalculator.h"
#include "tauRecTools/DiTauDiscriminantTool.h"
#include "tauRecTools/DiTauWPDecorator.h"
#include "tauRecTools/MuonTrackRemoval.h"

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
#pragma link C++ class FTauDecorator+;
#pragma link C++ class MvaTESVariableDecorator+;
#pragma link C++ class MvaTESEvaluator+;
#pragma link C++ class tauRecTools::TauTrackClassifier+;
#pragma link C++ class tauRecTools::TrackMVABDT+;
#pragma link C++ class CombinedP4FromRecoTaus+;
#pragma link C++ class TauChargedPFOCreator+;
#pragma link C++ class TauWPDecorator+;
#pragma link C++ class TauJetBDTEvaluator+;
#pragma link C++ class TauJetRNNEvaluator+;
#pragma link C++ class TauIDVarCalculator+;
#pragma link C++ class TauEleOLRDecorator+;
#pragma link C++ class BuildTruthTaus+;
#pragma link C++ class DiTauProcessorTool+;
#pragma link C++ class tauRecTools::DiTauIDVarCalculator+;
#pragma link C++ class tauRecTools::DiTauDiscriminantTool+;
#pragma link C++ class tauRecTools::DiTauWPDecorator+;
#pragma link C++ class tauRecTools::MuonTrackRemoval+;

#endif
