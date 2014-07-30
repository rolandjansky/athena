#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrigOnlineSpacePointTool/PixelSpacePointTool.h"
#include "TrigOnlineSpacePointTool/SCT_SpacePointTool.h"
#include "TrigOnlineSpacePointTool/PixelClusterCacheTool.h"
#include "TrigOnlineSpacePointTool/SCT_ClusterCacheTool.h"
#include "TrigOnlineSpacePointTool/OnlineSpacePointProviderTool.h"
#include "TrigOnlineSpacePointTool/FastSCT_RodDecoder.h"
#include "../TrigL2LayerNumberTool.h"
#include "../TrigSpacePointConversionTool.h"

DECLARE_TOOL_FACTORY(PixelSpacePointTool)
DECLARE_TOOL_FACTORY(SCT_SpacePointTool)
DECLARE_TOOL_FACTORY(PixelClusterCacheTool)
DECLARE_TOOL_FACTORY(SCT_ClusterCacheTool)
DECLARE_TOOL_FACTORY(FastSCT_RodDecoder)
DECLARE_TOOL_FACTORY(OnlineSpacePointProviderTool)
DECLARE_TOOL_FACTORY(TrigL2LayerNumberTool)
DECLARE_TOOL_FACTORY(TrigSpacePointConversionTool)

DECLARE_FACTORY_ENTRIES(TrigOnlineSpacePointTool) {
  DECLARE_TOOL(PixelSpacePointTool)
  DECLARE_TOOL(SCT_SpacePointTool)
  DECLARE_TOOL(PixelClusterCacheTool)
  DECLARE_TOOL(SCT_ClusterCacheTool)
  DECLARE_TOOL(FastSCT_RodDecoder)
  DECLARE_TOOL(OnlineSpacePointProviderTool)
  DECLARE_TOOL(TrigL2LayerNumberTool)
  DECLARE_TOOL(TrigSpacePointConversionTool)
}
