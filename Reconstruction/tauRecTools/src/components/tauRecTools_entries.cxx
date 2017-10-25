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
DECLARE_TOOL_FACTORY( JetSeedBuilder             )
DECLARE_TOOL_FACTORY( LockTauContainers          )
DECLARE_TOOL_FACTORY( TauAxisSetter         )
DECLARE_TOOL_FACTORY( TauCalibrateEM             )
DECLARE_TOOL_FACTORY( TauCellVariables           )
DECLARE_TOOL_FACTORY( TauTrackFinder             )
DECLARE_TOOL_FACTORY( TauVertexFinder            )
DECLARE_TOOL_FACTORY( TauElectronVetoVariables             )
DECLARE_TOOL_FACTORY( TauShotFinder              )
DECLARE_TOOL_FACTORY( TauPi0ClusterCreator   )
DECLARE_TOOL_FACTORY( TauPi0CreateROI        )
DECLARE_TOOL_FACTORY( PhotonConversionPID )
DECLARE_TOOL_FACTORY( PhotonConversionVertex )
DECLARE_TOOL_FACTORY( TauConversionFinder )
DECLARE_TOOL_FACTORY( TauConversionTagger )
DECLARE_TOOL_FACTORY( TauVertexVariables )
//DECLARE_TOOL_FACTORY( tauCalibrateWeightTool )
DECLARE_TOOL_FACTORY( TauTestDump )
#endif

DECLARE_TOOL_FACTORY( TauCalibrateLC             )
DECLARE_TOOL_FACTORY( TauIDPileupCorrection             )
DECLARE_TOOL_FACTORY( TauProcessorTool             )
DECLARE_TOOL_FACTORY( TauBuilderTool             )
DECLARE_TOOL_FACTORY( MvaTESVariableDecorator )
DECLARE_TOOL_FACTORY( MvaTESEvaluator )
DECLARE_NAMESPACE_TOOL_FACTORY( tauRecTools, TauTrackClassifier  )
DECLARE_NAMESPACE_TOOL_FACTORY( tauRecTools, TrackMVABDT         )
DECLARE_TOOL_FACTORY( CombinedP4FromRecoTaus )
DECLARE_TOOL_FACTORY( TauTrackFilter )
DECLARE_TOOL_FACTORY( TauGenericPi0Cone )
DECLARE_TOOL_FACTORY( TauSubstructureVariables     )
DECLARE_TOOL_FACTORY( TauCommonCalcVars          )
DECLARE_TOOL_FACTORY( TauPi0ClusterScaler  )
DECLARE_TOOL_FACTORY( TauPi0ScoreCalculator  )
DECLARE_TOOL_FACTORY( TauPi0Selector        )
DECLARE_TOOL_FACTORY( TauWPDecorator )
DECLARE_NAMESPACE_TOOL_FACTORY( tauRecTools, DiTauDiscriminantTool )
DECLARE_NAMESPACE_TOOL_FACTORY( tauRecTools, DiTauIDVarCalculator )
DECLARE_TOOL_FACTORY( TauJetBDTEvaluator )
DECLARE_TOOL_FACTORY( TauEleOLRDecorator )
DECLARE_TOOL_FACTORY( TauIDVarCalculator )

