#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigEgammaAnalysisTools/TrigEgammaAnalysisBaseTool.h"
#include "TrigEgammaAnalysisTools/TrigEgammaNavZeeTPBaseTool.h"
#include "TrigEgammaAnalysisTools/TrigEgammaNavZeeTPCounts.h"
#include "TrigEgammaAnalysisTools/TrigEgammaNavZeeTPRes.h"
#include "TrigEgammaAnalysisTools/TrigEgammaNavZeeTPEff.h"
#include "TrigEgammaAnalysisTools/TrigEgammaNavZeeTPIneff.h"
#include "TrigEgammaAnalysisTools/TrigEgammaNavZeeTPPerf.h"
#include "../TrigEgammaAnalysisAlg.h"
#include "../TrigEgammaTDToolTest.h"

DECLARE_TOOL_FACTORY(TrigEgammaAnalysisBaseTool)
DECLARE_TOOL_FACTORY(TrigEgammaNavZeeTPBaseTool)
DECLARE_TOOL_FACTORY(TrigEgammaNavZeeTPCounts)
DECLARE_TOOL_FACTORY(TrigEgammaNavZeeTPRes)
DECLARE_TOOL_FACTORY(TrigEgammaNavZeeTPEff)
DECLARE_TOOL_FACTORY(TrigEgammaNavZeeTPIneff)
DECLARE_TOOL_FACTORY(TrigEgammaNavZeeTPPerf)
DECLARE_ALGORITHM_FACTORY(TrigEgammaAnalysisAlg)
DECLARE_ALGORITHM_FACTORY(TrigEgammaTDToolTest)
    
DECLARE_FACTORY_ENTRIES(TrigEgammaAnalysisTools) {
  DECLARE_TOOL(TrigEgammaAnalysisBaseTool)
  DECLARE_TOOL(TrigEgammaNavZeeTPBaseTool)
  DECLARE_TOOL(TrigEgammaNavZeeTPCounts)
  DECLARE_TOOL(TrigEgammaNavZeeTPRes)
  DECLARE_TOOL(TrigEgammaNavZeeTPEff)
  DECLARE_TOOL(TrigEgammaNavZeeTPIneff)
  DECLARE_TOOL(TrigEgammaNavZeeTPPerf)
  DECLARE_ALGORITHM(TrigEgammaAnalysisAlg)
  DECLARE_ALGORITHM(TrigEgammaTDToolTest)
}

