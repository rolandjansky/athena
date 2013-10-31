#include "TrigVertexFitter/TrigVertexFitter.h"
#include "TrigVertexFitter/TrigPrimaryVertexFitter.h"
#include "TrigVertexFitter/TrigL2VertexFitter.h"
#include "TrigVertexFitter/TrigVertexingTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(TrigVertexFitter)
DECLARE_TOOL_FACTORY(TrigPrimaryVertexFitter)
DECLARE_TOOL_FACTORY(TrigL2VertexFitter)
DECLARE_TOOL_FACTORY(TrigVertexingTool)

DECLARE_FACTORY_ENTRIES(TrigVertexFitter)
{
  DECLARE_TOOL(TrigVertexFitter)
  DECLARE_TOOL(TrigPrimaryVertexFitter)
  DECLARE_TOOL(TrigL2VertexFitter)
  DECLARE_TOOL(TrigVertexingTool)
}

