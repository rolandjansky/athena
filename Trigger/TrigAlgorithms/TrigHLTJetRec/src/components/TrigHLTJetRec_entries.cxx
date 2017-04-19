//#include "TrigHLTJetRec/TrigHLTJetRec.h"
#include "TrigHLTJetRec/TrigHLTJetRecFromCluster.h"
#include "TrigHLTJetRec/TrigHLTJetRecFromJet.h"
#include "TrigHLTJetRec/TrigHLTJetRecFromTriggerTower.h"
#include "TrigHLTJetRec/TriggerPseudoJetGetter.h"
#include "TrigHLTJetRec/PseudoJetSelectorAll.h"
#include "TrigHLTJetRec/PseudoJetSelectorPositivePt.h"
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
#include "GaudiKernel/DeclareFactoryEntries.h"
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
DECLARE_TOOL_FACTORY(PseudoJetSelectorPositivePt)
DECLARE_TOOL_FACTORY(PseudoJetSelectorEtaPt)
DECLARE_TOOL_FACTORY(IParticleSelectorAll)
DECLARE_TOOL_FACTORY(IParticleSelectorEtaPt)
DECLARE_TOOL_FACTORY(TrigJetFromPseudojet)

DECLARE_FACTORY_ENTRIES(TrigHLTJetRec) {
  //DECLARE_ALGORITHM( TrigHLTJetRec);
    DECLARE_ALGORITHM( TrigHLTJetRecFromCluster);
    DECLARE_ALGORITHM( TrigHLTJetRecFromJet);
    DECLARE_ALGORITHM( TrigHLTJetRecFromTriggerTower);
    DECLARE_ALGORITHM( TrigHLTJetDiagnostics);
    DECLARE_ALGORITHM( TrigHLTJetDebug);
    DECLARE_ALGORITHM( TrigHLTClusterDiagnostics);
    DECLARE_ALGORITHM( TrigHLTCellDiagnostics);
    DECLARE_ALGORITHM( TrigHLTRoIDiagnostics);
    DECLARE_ALGORITHM( TrigHLTHypoDiagnostics);
    DECLARE_ALGORITHM( TrigHLTPSvsFSDiagnostics);
    DECLARE_ALGORITHM( TrigHLTEnergyDensity);
    DECLARE_ALGORITHM( TrigHLTSoftKiller);
    DECLARE_ALGORITHM( TrigHLTJetDSSelector);
    DECLARE_ALGORITHM( TrigHLTJetRecGroomer);
    DECLARE_ALGORITHM( TrigHLTTrackMomentHelpers);
    DECLARE_TOOL(TriggerPseudoJetGetter);
    DECLARE_TOOL(PseudoJetSelectorAll);
    DECLARE_TOOL(PseudoJetSelectorPositivePt);
    DECLARE_TOOL(PseudoJetSelectorEtaPt);
    DECLARE_TOOL(IParticleSelectorAll);
    DECLARE_TOOL(IParticleSelectorEtaPt);
    DECLARE_TOOL(TrigJetFromPseudojet)
}


