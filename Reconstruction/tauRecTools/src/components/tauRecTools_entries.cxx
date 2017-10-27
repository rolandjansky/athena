#ifndef XAOD_ANALYSIS
#include "../JetSeedBuilder.h"
#include "../LockTauContainers.h"
#include "../TauAxisSetter.h"
#include "../TauCalibrateEM.h"
#include "../TauCellVariables.h"
#include "../TauTrackFinder.h"
#include "../TauVertexFinder.h"
#include "../TauElectronVetoVariables.h"
#include "../TauShotFinder.h"
#include "../TauPi0ClusterCreator.h"
#include "../TauPi0CreateROI.h"
#include "../TauConversionFinder.h"
#include "../PhotonConversionPID.h"
#include "../PhotonConversionVertex.h"
#include "../TauConversionTagger.h"
#include "../TauVertexVariables.h"
#include "../TauTestDump.h"
//#include "../tauCalibrateWeightTool.h"  //for trigger
#endif
#include "tauRecTools/TauTrackFilter.h"
#include "tauRecTools/TauGenericPi0Cone.h"
#include "tauRecTools/TauCalibrateLC.h"
#include "tauRecTools/TauIDPileupCorrection.h"
#include "tauRecTools/TauCommonCalcVars.h"
#include "tauRecTools/TauSubstructureVariables.h"
#include "tauRecTools/TauProcessorTool.h"
#include "tauRecTools/TauBuilderTool.h"
#include "tauRecTools/MvaTESEvaluator.h"
#include "tauRecTools/MvaTESVariableDecorator.h"
#include "tauRecTools/TauTrackClassifier.h"
#include "tauRecTools/CombinedP4FromRecoTaus.h"
#include "tauRecTools/TauPi0ClusterScaler.h"
#include "tauRecTools/TauPi0ScoreCalculator.h"
#include "tauRecTools/TauPi0Selector.h"
#include "tauRecTools/TauWPDecorator.h"
#include "tauRecTools/DiTauDiscriminantTool.h"
#include "tauRecTools/DiTauIDVarCalculator.h"
#include "tauRecTools/TauJetBDTEvaluator.h"
#include "tauRecTools/TauEleOLRDecorator.h"
#include "tauRecTools/TauIDVarCalculator.h"


#ifndef XAOD_ANALYSIS
DECLARE_COMPONENT( JetSeedBuilder )
DECLARE_COMPONENT( LockTauContainers )
DECLARE_COMPONENT( TauAxisSetter )
DECLARE_COMPONENT( TauCalibrateEM )
DECLARE_COMPONENT( TauCellVariables )
DECLARE_COMPONENT( TauTrackFinder )
DECLARE_COMPONENT( TauVertexFinder )
DECLARE_COMPONENT( TauElectronVetoVariables )
DECLARE_COMPONENT( TauShotFinder )
DECLARE_COMPONENT( TauPi0ClusterCreator )
DECLARE_COMPONENT( TauPi0CreateROI )
DECLARE_COMPONENT( PhotonConversionPID )
DECLARE_COMPONENT( PhotonConversionVertex )
DECLARE_COMPONENT( TauConversionFinder )
DECLARE_COMPONENT( TauConversionTagger )
DECLARE_COMPONENT( TauVertexVariables )
//DECLARE_COMPONENT( tauCalibrateWeightTool )
DECLARE_COMPONENT( TauTestDump )
#endif

DECLARE_COMPONENT( TauCalibrateLC )
DECLARE_COMPONENT( TauIDPileupCorrection )
DECLARE_COMPONENT( TauProcessorTool )
DECLARE_COMPONENT( TauBuilderTool )
DECLARE_COMPONENT( MvaTESVariableDecorator )
DECLARE_COMPONENT( MvaTESEvaluator )
DECLARE_COMPONENT( tauRecTools::TauTrackClassifier )
DECLARE_COMPONENT( tauRecTools::TrackMVABDT )
DECLARE_COMPONENT( CombinedP4FromRecoTaus )
DECLARE_COMPONENT( TauTrackFilter )
DECLARE_COMPONENT( TauGenericPi0Cone )
DECLARE_COMPONENT( TauSubstructureVariables )
DECLARE_COMPONENT( TauCommonCalcVars )
DECLARE_COMPONENT( TauPi0ClusterScaler )
DECLARE_COMPONENT( TauPi0ScoreCalculator )
DECLARE_COMPONENT( TauPi0Selector )
DECLARE_COMPONENT( TauWPDecorator )
DECLARE_COMPONENT( tauRecTools::DiTauDiscriminantTool )
DECLARE_COMPONENT( tauRecTools::DiTauIDVarCalculator )
DECLARE_COMPONENT( TauJetBDTEvaluator )
DECLARE_COMPONENT( TauEleOLRDecorator )
DECLARE_COMPONENT( TauIDVarCalculator )

