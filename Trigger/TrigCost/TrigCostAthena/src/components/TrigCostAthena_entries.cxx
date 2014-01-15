
// Tools and algos
#include "TrigCostAthena/TrigNtMbtsTool.h"
#include "TrigCostAthena/TrigNtAnpITool.h"
#include "TrigCostAthena/TrigNtVertexTool.h"
#include "TrigCostAthena/TrigNtStoreVertices.h"

// Framework
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Trig, TrigNtMbtsTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trig, TrigNtAnpITool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trig, TrigNtVertexTool )
DECLARE_NAMESPACE_TOOL_FACTORY( Trig, TrigNtStoreVertices )

DECLARE_FACTORY_ENTRIES( TrigCostAthena )
{
  DECLARE_NAMESPACE_TOOL( Trig, TrigNtMbtsTool )
  DECLARE_NAMESPACE_TOOL( Trig, TrigNtAnpITool )
  DECLARE_NAMESPACE_TOOL( Trig, TrigNtVertexTool )
  DECLARE_NAMESPACE_TOOL( Trig, TrigNtStoreVertices )
}
