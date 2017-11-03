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

//DECLARE_COMPONENT( TrigHLTJetRec )
DECLARE_COMPONENT( TrigHLTJetRecFromCluster )
DECLARE_COMPONENT( TrigHLTJetRecFromJet )
DECLARE_COMPONENT( TrigHLTJetRecFromTriggerTower )
DECLARE_COMPONENT( TrigHLTJetDiagnostics )
DECLARE_COMPONENT( TrigHLTJetDebug )
DECLARE_COMPONENT( TrigHLTClusterDiagnostics )
DECLARE_COMPONENT( TrigHLTCellDiagnostics )
DECLARE_COMPONENT( TrigHLTRoIDiagnostics )
DECLARE_COMPONENT( TrigHLTHypoDiagnostics )
DECLARE_COMPONENT( TrigHLTPSvsFSDiagnostics )
DECLARE_COMPONENT( TrigHLTEnergyDensity )
DECLARE_COMPONENT( TrigHLTSoftKiller )
DECLARE_COMPONENT( TrigHLTJetDSSelector )
DECLARE_COMPONENT( TrigHLTJetRecGroomer )
DECLARE_COMPONENT( TrigHLTTrackMomentHelpers )

DECLARE_COMPONENT( TriggerPseudoJetGetter )
DECLARE_COMPONENT( PseudoJetSelectorAll )
DECLARE_COMPONENT( PseudoJetSelectorEtaPt )
DECLARE_COMPONENT( IParticleSelectorAll )
DECLARE_COMPONENT( IParticleSelectorEtaPt )
DECLARE_COMPONENT( TrigJetFromPseudojet )

