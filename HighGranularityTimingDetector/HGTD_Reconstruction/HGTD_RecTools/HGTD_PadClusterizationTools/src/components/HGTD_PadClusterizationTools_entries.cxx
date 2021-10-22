#include "GaudiKernel/DeclareFactoryEntries.h"
#include "HGTD_PadClusterizationTools/HGTD_ClusterMakerTool.h"
#include "HGTD_PadClusterizationTools/SinglePadClusterTool.h"

using namespace HGTD;

DECLARE_TOOL_FACTORY(HGTD_ClusterMakerTool)

DECLARE_TOOL_FACTORY(SinglePadClusterTool)

DECLARE_FACTORY_ENTRIES(HGTD_PadClusterizationTools) {
  DECLARE_TOOL(HGTD_ClusterMakerTool)
  DECLARE_TOOL(SinglePadClusterTool)
}
