// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "TauAnalysisTools/TauSelectionTool.h"
#include "TauAnalysisTools/TauSmearingTool.h"
#include "TauAnalysisTools/TauTruthMatchingTool.h"
#include "TauAnalysisTools/TauTruthTrackMatchingTool.h"
#include "TauAnalysisTools/TauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/TauOverlappingElectronLLHDecorator.h"
#include "../TauAnalysisToolsExampleAthena.h"

DECLARE_NAMESPACE_TOOL_FACTORY( TauAnalysisTools, TauSelectionTool )
DECLARE_NAMESPACE_TOOL_FACTORY( TauAnalysisTools, TauSmearingTool )
DECLARE_NAMESPACE_TOOL_FACTORY( TauAnalysisTools, TauTruthMatchingTool )
DECLARE_NAMESPACE_TOOL_FACTORY( TauAnalysisTools, TauTruthTrackMatchingTool )
DECLARE_NAMESPACE_TOOL_FACTORY( TauAnalysisTools, TauEfficiencyCorrectionsTool )
DECLARE_NAMESPACE_TOOL_FACTORY( TauAnalysisTools, TauOverlappingElectronLLHDecorator )

DECLARE_NAMESPACE_ALGORITHM_FACTORY( TauAnalysisTools, TauAnalysisToolsExampleAthena )

DECLARE_FACTORY_ENTRIES( TauAnalysisTools )
{
  DECLARE_NAMESPACE_TOOL( TauAnalysisTools, TauSelectionTool )
  DECLARE_NAMESPACE_TOOL( TauAnalysisTools, TauSmearingTool )
  DECLARE_NAMESPACE_TOOL( TauAnalysisTools, TauTruthMatchingTool )
  DECLARE_NAMESPACE_TOOL( TauAnalysisTools, TauTruthTrackMatchingTool )
  DECLARE_NAMESPACE_TOOL( TauAnalysisTools, TauEfficiencyCorrectionsTool )
  DECLARE_NAMESPACE_TOOL( TauAnalysisTools, TauOverlappingElectronLLHDecorator )

  DECLARE_NAMESPACE_ALGORITHM( TauAnalysisTools, TauAnalysisToolsExampleAthena )
}
