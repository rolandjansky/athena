#include "GaudiKernel/DeclareFactoryEntries.h"
#include "HGTD_PadClusterizationTools/HGTD_ClusterMakerTool.h"

using namespace HGTD;

DECLARE_TOOL_FACTORY(HGTD_ClusterMakerTool)

DECLARE_FACTORY_ENTRIES( HGTD_PadClusterizationTools )
{
  DECLARE_TOOL( HGTD_ClusterMakerTool )
}
