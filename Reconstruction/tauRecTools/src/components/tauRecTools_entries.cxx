#ifndef XAOD_ANALYSIS
#include "../JetSeedBuilder.h"
#include "../TauAxisSetter.h"
#include "../TauCellVariables.h"
#include "../TauTrackFinder.h"
#include "../TauVertexFinder.h"
#include "../TauElectronVetoVariables.h"
#include "../TauShotFinder.h"
#include "../TauPi0ClusterCreator.h"
#include "../TauPi0CreateROI.h"
#include "../TauPi0ClusterScaler.h"
#include "../TauVertexVariables.h"
#endif

#include "tauRecTools/TauCalibrateLC.h"
#include "tauRecTools/TauCommonCalcVars.h"
#include "tauRecTools/TauSubstructureVariables.h"
#include "tauRecTools/MvaTESEvaluator.h"
#include "tauRecTools/MvaTESVariableDecorator.h"
#include "tauRecTools/TauTrackClassifier.h"
#include "tauRecTools/TauTrackRNNClassifier.h"
#include "tauRecTools/CombinedP4FromRecoTaus.h"
#include "tauRecTools/TauPi0ScoreCalculator.h"
#include "tauRecTools/TauPi0Selector.h"
#include "tauRecTools/TauWPDecorator.h"
#include "tauRecTools/TauJetBDTEvaluator.h"
#include "tauRecTools/TauEleOLRDecorator.h"
#include "tauRecTools/TauIDVarCalculator.h"
#include "tauRecTools/TauJetRNNEvaluator.h"
#include "tauRecTools/TauDecayModeNNClassifier.h"
#include "tauRecTools/TauVertexCorrection.h"

#ifndef XAOD_ANALYSIS
DECLARE_COMPONENT( JetSeedBuilder )
DECLARE_COMPONENT( TauAxisSetter )
DECLARE_COMPONENT( TauCellVariables )
DECLARE_COMPONENT( TauTrackFinder )
DECLARE_COMPONENT( TauVertexFinder )
DECLARE_COMPONENT( TauElectronVetoVariables )
DECLARE_COMPONENT( TauShotFinder )
DECLARE_COMPONENT( TauPi0ClusterCreator )
DECLARE_COMPONENT( TauPi0CreateROI )
DECLARE_COMPONENT( TauPi0ClusterScaler )
DECLARE_COMPONENT( TauVertexVariables )
#endif

DECLARE_COMPONENT( TauCalibrateLC )
DECLARE_COMPONENT( MvaTESVariableDecorator )
DECLARE_COMPONENT( MvaTESEvaluator )
DECLARE_COMPONENT( tauRecTools::TauTrackClassifier )
DECLARE_COMPONENT( tauRecTools::TauTrackRNNClassifier )
DECLARE_COMPONENT( tauRecTools::TrackMVABDT )
DECLARE_COMPONENT( tauRecTools::TrackRNN )
DECLARE_COMPONENT( CombinedP4FromRecoTaus )
DECLARE_COMPONENT( TauSubstructureVariables )
DECLARE_COMPONENT( TauCommonCalcVars )
DECLARE_COMPONENT( TauPi0ScoreCalculator )
DECLARE_COMPONENT( TauPi0Selector )
DECLARE_COMPONENT( TauWPDecorator )
DECLARE_COMPONENT( TauJetBDTEvaluator )
DECLARE_COMPONENT( TauEleOLRDecorator )
DECLARE_COMPONENT( TauIDVarCalculator )
DECLARE_COMPONENT( TauJetRNNEvaluator )
DECLARE_COMPONENT( TauDecayModeNNClassifier )
DECLARE_COMPONENT( TauVertexCorrection )
