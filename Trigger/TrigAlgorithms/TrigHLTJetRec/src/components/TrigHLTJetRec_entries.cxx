//#include "TrigHLTJetRec/TrigHLTJetRec.h"
#include "TrigHLTJetRec/TrigHLTJetRecFromCluster.h"
#include "TrigHLTJetRec/TrigHLTJetRecFromJet.h"
#include "TrigHLTJetRec/TrigHLTJetRecFromTriggerTower.h"
#include "TrigHLTJetRec/TriggerPseudoJetGetter.h"
#include "TrigHLTJetRec/PseudoJetSelectorAll.h"
#include "TrigHLTJetRec/PseudoJetSelectorEtaPt.h"
#include "TrigHLTJetRec/IParticleSelectorAll.h"
#include "TrigHLTJetRec/IParticleSelectorEtaPt.h"
#include "TrigHLTJetRec/TrigHLTJetDiagnostics.h"
#include "TrigHLTJetRec/TrigHLTJetDebug.h"
#include "TrigHLTJetRec/TrigHLTClusterDiagnostics.h"
#include "TrigHLTJetRec/TrigHLTCellDiagnostics.h"
#include "TrigHLTJetRec/TrigHLTRoIDiagnostics.h"
#include "TrigHLTJetRec/TrigHLTHypoDiagnostics.h"
#include "TrigHLTJetRec/TrigHLTPSvsFSDiagnostics.h"
#include "TrigHLTJetRec/TrigHLTEnergyDensity.h"
#include "TrigHLTJetRec/TrigHLTSoftKiller.h"
#include "TrigHLTJetRec/TrigHLTJetDSSelector.h"
#include "TrigHLTJetRec/TrigHLTJetRecGroomer.h"
#include "TrigHLTJetRec/TrigJetFromPseudojet.h"
#include "TrigHLTJetRec/TrigHLTTrackMomentHelpers.h"

//DECLARE_ALGORITHM_FACTORY( TrigHLTJetRec)
DECLARE_ALGORITHM_FACTORY( TrigHLTJetRecFromCluster)
DECLARE_ALGORITHM_FACTORY( TrigHLTJetRecFromJet)
DECLARE_ALGORITHM_FACTORY( TrigHLTJetRecFromTriggerTower)
DECLARE_ALGORITHM_FACTORY( TrigHLTJetDiagnostics)
DECLARE_ALGORITHM_FACTORY( TrigHLTJetDebug)
DECLARE_ALGORITHM_FACTORY( TrigHLTClusterDiagnostics)
DECLARE_ALGORITHM_FACTORY( TrigHLTCellDiagnostics)
DECLARE_ALGORITHM_FACTORY( TrigHLTRoIDiagnostics)
DECLARE_ALGORITHM_FACTORY( TrigHLTHypoDiagnostics)
DECLARE_ALGORITHM_FACTORY( TrigHLTPSvsFSDiagnostics)
DECLARE_ALGORITHM_FACTORY( TrigHLTEnergyDensity)
DECLARE_ALGORITHM_FACTORY( TrigHLTSoftKiller)
DECLARE_ALGORITHM_FACTORY( TrigHLTJetDSSelector)
DECLARE_ALGORITHM_FACTORY( TrigHLTJetRecGroomer)
DECLARE_ALGORITHM_FACTORY( TrigHLTTrackMomentHelpers)

DECLARE_TOOL_FACTORY(TriggerPseudoJetGetter)
DECLARE_TOOL_FACTORY(PseudoJetSelectorAll)
DECLARE_TOOL_FACTORY(PseudoJetSelectorEtaPt)
DECLARE_TOOL_FACTORY(IParticleSelectorAll)
DECLARE_TOOL_FACTORY(IParticleSelectorEtaPt)
DECLARE_TOOL_FACTORY(TrigJetFromPseudojet)

