#include "GaudiKernel/DeclareFactoryEntries.h"

#include "FTK_RecTools/FTK_VertexFinderTool.h"

DECLARE_TOOL_FACTORY(FTK_VertexFinderTool)

DECLARE_FACTORY_ENTRIES(FTK_RecTools) {
  DECLARE_TOOL(FTK_VertexFinderTool)
}
