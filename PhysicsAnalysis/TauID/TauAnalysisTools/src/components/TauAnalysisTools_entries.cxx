// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "TauAnalysisTools/TauSelectionTool.h"
#include "TauAnalysisTools/TauSmearingTool.h"
#include "TauAnalysisTools/TauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/TauEfficiencyJetIDTool.h"
// #include "TauAnalysisTools/TauEfficiencyEleIDTool.h"
// #include "TauAnalysisTools/TauEfficiencyContJetIDTool.h"
#include "../TauAnalysisToolsExampleAthena.h"

DECLARE_NAMESPACE_TOOL_FACTORY( TauAnalysisTools, TauSelectionTool )
DECLARE_NAMESPACE_TOOL_FACTORY( TauAnalysisTools, TauSmearingTool )
DECLARE_NAMESPACE_TOOL_FACTORY( TauAnalysisTools, TauEfficiencyCorrectionsTool )
DECLARE_NAMESPACE_TOOL_FACTORY( TauAnalysisTools, TauEfficiencyJetIDTool )
// DECLARE_NAMESPACE_TOOL_FACTORY( TauAnalysisTools, TauEfficiencyEleIDTool )
// DECLARE_NAMESPACE_TOOL_FACTORY( TauAnalysisTools, TauEfficiencyContJetIDTool )

DECLARE_NAMESPACE_ALGORITHM_FACTORY( TauAnalysisTools, TauAnalysisToolsExampleAthena )

DECLARE_FACTORY_ENTRIES( TauAnalysisTools ) {
	DECLARE_NAMESPACE_TOOL( TauAnalysisTools, TauSelectionTool )
	DECLARE_NAMESPACE_TOOL( TauAnalysisTools, TauSmearingTool )
	DECLARE_NAMESPACE_TOOL( TauAnalysisTools, TauEfficiencyCorrectionsTool )
	DECLARE_NAMESPACE_TOOL( TauAnalysisTools, TauEfficiencyJetIDTool )
        // DECLARE_NAMESPACE_TOOL( TauAnalysisTools, TauEfficiencyEleIDTool )
	// DECLARE_NAMESPACE_TOOL( TauAnalysisTools, TauEfficiencyContJetIDTool )

	DECLARE_NAMESPACE_ALGORITHM( TauAnalysisTools, TauAnalysisToolsExampleAthena )
}
