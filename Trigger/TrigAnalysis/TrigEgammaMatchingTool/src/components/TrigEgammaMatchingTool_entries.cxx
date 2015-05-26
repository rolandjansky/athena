#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TrigEgammaMatchingTool/TrigEgammaMatchingTool.h"
#include "../TrigEgammaMatchingToolTest.h"

DECLARE_NAMESPACE_TOOL_FACTORY(Trig,TrigEgammaMatchingTool)   
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Trig,TrigEgammaMatchingToolTest) 

DECLARE_FACTORY_ENTRIES(TrigEgammaMatchingTool) {
  DECLARE_NAMESPACE_TOOL(Trig,TrigEgammaMatchingTool)
  DECLARE_NAMESPACE_ALGORITHM(Trig,TrigEgammaMatchingToolTest)
}

