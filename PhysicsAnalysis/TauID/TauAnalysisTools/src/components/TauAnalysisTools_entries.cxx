// Gaudi/Athena include(s):
#include "GaudiKernel/DeclareFactoryEntries.h"

// Local include(s):
#include "TauAnalysisTools/CommonEfficiencyTool.h"
#include "TauAnalysisTools/CommonSmearingTool.h"
#include "TauAnalysisTools/TauSelectionTool.h"
#include "TauAnalysisTools/TauSmearingTool.h"
#include "TauAnalysisTools/TauSmearingRun1Tool.h"
#include "TauAnalysisTools/TauTruthMatchingTool.h"
#include "TauAnalysisTools/TauTruthTrackMatchingTool.h"
#include "TauAnalysisTools/TauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/TauEfficiencyContJetIDTool.h"
#include "TauAnalysisTools/TauEfficiencyCorrectionsTool.h"
#include "TauAnalysisTools/TauEfficiencyEleIDTool.h"
#include "TauAnalysisTools/TauEfficiencyJetIDTool.h"
#include "TauAnalysisTools/TauEfficiencyTriggerTool.h"
#include "TauAnalysisTools/TauOverlappingElectronLLHDecorator.h"
#include "TauAnalysisTools/DiTauTruthMatchingTool.h"
#include "../TauAnalysisToolsExampleAthena.h"

DECLARE_NAMESPACE_TOOL_FACTORY( TauAnalysisTools , DiTauTruthMatchingTool )

DECLARE_NAMESPACE_TOOL_FACTORY( TauAnalysisTools, CommonEfficiencyTool )
DECLARE_NAMESPACE_TOOL_FACTORY( TauAnalysisTools, CommonSmearingTool )
DECLARE_NAMESPACE_TOOL_FACTORY( TauAnalysisTools, TauSelectionTool )
DECLARE_NAMESPACE_TOOL_FACTORY( TauAnalysisTools, TauSmearingTool )
DECLARE_NAMESPACE_TOOL_FACTORY( TauAnalysisTools, TauSmearingRun1Tool )
DECLARE_NAMESPACE_TOOL_FACTORY( TauAnalysisTools, TauTruthMatchingTool )
DECLARE_NAMESPACE_TOOL_FACTORY( TauAnalysisTools, TauTruthTrackMatchingTool )
DECLARE_NAMESPACE_TOOL_FACTORY( TauAnalysisTools, TauEfficiencyCorrectionsTool )
DECLARE_NAMESPACE_TOOL_FACTORY( TauAnalysisTools, TauEfficiencyContJetIDTool )
DECLARE_NAMESPACE_TOOL_FACTORY( TauAnalysisTools, TauEfficiencyEleIDTool )
DECLARE_NAMESPACE_TOOL_FACTORY( TauAnalysisTools, TauEfficiencyJetIDTool )
DECLARE_NAMESPACE_TOOL_FACTORY( TauAnalysisTools, TauEfficiencyTriggerTool )
DECLARE_NAMESPACE_TOOL_FACTORY( TauAnalysisTools, TauOverlappingElectronLLHDecorator )

DECLARE_NAMESPACE_ALGORITHM_FACTORY( TauAnalysisTools, TauAnalysisToolsExampleAthena )

DECLARE_FACTORY_ENTRIES( TauAnalysisTools )
{
  DECLARE_NAMESPACE_TOOL( TauAnalysisTools, CommonEfficiencyTool )
  DECLARE_NAMESPACE_TOOL( TauAnalysisTools, CommonSmearingTool )
  DECLARE_NAMESPACE_TOOL( TauAnalysisTools, TauSelectionTool )
  DECLARE_NAMESPACE_TOOL( TauAnalysisTools, TauSmearingTool )
  DECLARE_NAMESPACE_TOOL( TauAnalysisTools, TauSmearingRun1Tool )
  DECLARE_NAMESPACE_TOOL( TauAnalysisTools, TauTruthMatchingTool )
  DECLARE_NAMESPACE_TOOL( TauAnalysisTools, TauTruthTrackMatchingTool )
  DECLARE_NAMESPACE_TOOL( TauAnalysisTools, TauEfficiencyCorrectionsTool )
  DECLARE_NAMESPACE_TOOL( TauAnalysisTools, TauEfficiencyContJetIDTool )
  DECLARE_NAMESPACE_TOOL( TauAnalysisTools, TauEfficiencyEleIDTool )
  DECLARE_NAMESPACE_TOOL( TauAnalysisTools, TauEfficiencyJetIDTool )
  DECLARE_NAMESPACE_TOOL( TauAnalysisTools, TauEfficiencyTriggerTool )
  DECLARE_NAMESPACE_TOOL( TauAnalysisTools, TauOverlappingElectronLLHDecorator )

  DECLARE_NAMESPACE_TOOL( TauAnalysisTools , DiTauTruthMatchingTool )

  DECLARE_NAMESPACE_ALGORITHM( TauAnalysisTools, TauAnalysisToolsExampleAthena )
  
}
