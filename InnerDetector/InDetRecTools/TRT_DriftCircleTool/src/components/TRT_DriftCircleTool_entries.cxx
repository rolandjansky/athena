#include "GaudiKernel/DeclareFactoryEntries.h"
#include "TRT_DriftCircleTool/TRT_DriftCircleTool.h"
#include "TRT_DriftCircleTool/TRT_DriftCircleToolCosmics.h"


DECLARE_NAMESPACE_TOOL_FACTORY(InDet,TRT_DriftCircleTool)
DECLARE_NAMESPACE_TOOL_FACTORY(InDet,TRT_DriftCircleToolCosmics)

DECLARE_FACTORY_ENTRIES(TRT_DriftCircleTool) {
DECLARE_NAMESPACE_TOOL(InDet,TRT_DriftCircleTool)
DECLARE_NAMESPACE_TOOL(InDet,TRT_DriftCircleToolCosmics)
}

