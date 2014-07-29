#include "TrigHLTJetRec/TrigHLTJetRec.h"
#include "TrigHLTJetRec/TriggerPseudoJetGetter.h"
#include "TrigHLTJetRec/TrigHLTJetDiagnostics.h"
#include "TrigHLTJetRec/TrigHLTClusterDiagnostics.h"
#include "TrigHLTJetRec/TrigHLTCellDiagnostics.h"
#include "TrigHLTJetRec/TrigHLTRoIDiagnostics.h"
#include "TrigHLTJetRec/TrigHLTHypoDiagnostics.h"
#include "TrigHLTJetRec/TrigHLTPSvsFSDiagnostics.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( TrigHLTJetRec )
DECLARE_ALGORITHM_FACTORY( TrigHLTJetDiagnostics )
DECLARE_ALGORITHM_FACTORY( TrigHLTClusterDiagnostics )
DECLARE_ALGORITHM_FACTORY( TrigHLTCellDiagnostics )
DECLARE_ALGORITHM_FACTORY( TrigHLTRoIDiagnostics )
DECLARE_ALGORITHM_FACTORY( TrigHLTHypoDiagnostics )
DECLARE_ALGORITHM_FACTORY( TrigHLTPSvsFSDiagnostics )
DECLARE_TOOL_FACTORY( TriggerPseudoJetGetter )

DECLARE_FACTORY_ENTRIES(TrigHLTJetRec) {
    DECLARE_ALGORITHM( TrigHLTJetRec );
    DECLARE_ALGORITHM( TrigHLTJetDiagnostics );
    DECLARE_ALGORITHM( TrigHLTClusterDiagnostics );
    DECLARE_ALGORITHM( TrigHLTCellDiagnostics );
    DECLARE_ALGORITHM( TrigHLTRoIDiagnostics );
    DECLARE_ALGORITHM( TrigHLTHypoDiagnostics );
    DECLARE_ALGORITHM( TrigHLTPSvsFSDiagnostics );
    DECLARE_TOOL( TriggerPseudoJetGetter );

}


