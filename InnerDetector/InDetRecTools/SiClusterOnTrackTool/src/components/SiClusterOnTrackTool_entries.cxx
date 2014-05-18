#include "GaudiKernel/DeclareFactoryEntries.h"
#include "SiClusterOnTrackTool/PixelClusterOnTrackTool.h"
#include "SiClusterOnTrackTool/SCT_ClusterOnTrackTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY(InDet, PixelClusterOnTrackTool)
DECLARE_NAMESPACE_TOOL_FACTORY(InDet, SCT_ClusterOnTrackTool)

DECLARE_FACTORY_ENTRIES(SiClusterOnTrackTool) {
  DECLARE_NAMESPACE_TOOL(InDet, PixelClusterOnTrackTool)
  DECLARE_NAMESPACE_TOOL(InDet, SCT_ClusterOnTrackTool)
}



