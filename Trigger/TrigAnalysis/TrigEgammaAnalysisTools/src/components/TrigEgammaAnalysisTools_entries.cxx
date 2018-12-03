#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigEgammaAnalysisTools/TrigEgammaAnalysisBaseTool.h"
#include "../TrigEgammaMonTool.h"
#include "../TrigEgammaPhysValMonTool.h"
#include "TrigEgammaAnalysisTools/TrigEgammaNavBaseTool.h"
#include "TrigEgammaAnalysisTools/TrigEgammaNavAnalysisTool.h"
#include "TrigEgammaAnalysisTools/TrigEgammaNavTPBaseTool.h"
#include "TrigEgammaAnalysisTools/TrigEgammaNavTPAnalysisTool.h"
#include "TrigEgammaAnalysisTools/EfficiencyTool.h"
#include "TrigEgammaAnalysisTools/TrigEgammaResolutionTool.h"
#include "TrigEgammaAnalysisTools/TrigEgammaDistTool.h"
#include "TrigEgammaAnalysisTools/TrigEgammaPlotTool.h"
#include "TrigEgammaAnalysisTools/TrigEgammaEventSelection.h"
#include "../TrigEgammaAnalysisAlg.h"
#include "../TrigEgammaTDToolTest.h"

DECLARE_TOOL_FACTORY(TrigEgammaAnalysisBaseTool)
DECLARE_TOOL_FACTORY(TrigEgammaMonTool)
DECLARE_TOOL_FACTORY(TrigEgammaPhysValMonTool)
DECLARE_TOOL_FACTORY(TrigEgammaNavTPBaseTool)
DECLARE_TOOL_FACTORY(TrigEgammaNavAnalysisTool)
DECLARE_TOOL_FACTORY(TrigEgammaNavTPAnalysisTool)
DECLARE_TOOL_FACTORY(EfficiencyTool) 
DECLARE_TOOL_FACTORY(TrigEgammaResolutionTool) 
DECLARE_TOOL_FACTORY(TrigEgammaDistTool) 
DECLARE_TOOL_FACTORY(TrigEgammaPlotTool) 
DECLARE_TOOL_FACTORY(TrigEgammaEventSelection) 
DECLARE_ALGORITHM_FACTORY(TrigEgammaAnalysisAlg)
DECLARE_ALGORITHM_FACTORY(TrigEgammaTDToolTest)
    
DECLARE_FACTORY_ENTRIES(TrigEgammaAnalysisTools) {
  DECLARE_TOOL(TrigEgammaAnalysisBaseTool)
  DECLARE_TOOL(TrigEgammaMonTool)
  DECLARE_TOOL(TrigEgammaPhysValMonTool)
  DECLARE_TOOL(TrigEgammaNavBaseTool)
  DECLARE_TOOL(TrigEgammaNavTPBaseTool)
  DECLARE_TOOL(TrigEgammaNavAnalysisTool)
  DECLARE_TOOL(TrigEgammaNavTPAnalysisTool)
  DECLARE_TOOL(EfficiencyTool)
  DECLARE_TOOL(TrigEgammaResolutionTool)
  DECLARE_TOOL(TrigEgammaDistTool)
  DECLARE_TOOL(TrigEgammaPlotTool)
  DECLARE_TOOL(TrigEgammaEventSelection)
  DECLARE_TOOL(TrigEgammaValidationTool)
  DECLARE_ALGORITHM(TrigEgammaAnalysisAlg)
  DECLARE_ALGORITHM(TrigEgammaTDToolTest)
}

