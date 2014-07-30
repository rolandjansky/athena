#include "GaudiKernel/DeclareFactoryEntries.h"

#include "TrigOfflineSpacePointTool/OfflineSpacePointProviderTool.h"
#include "TrigOfflineSpacePointTool/LazyOfflineSpacePointTool.h"
#include "InDetTrigToolInterfaces/ITrigSCT_SpacePointTool.h"

DECLARE_TOOL_FACTORY(LazyOfflineSpacePointTool)
DECLARE_TOOL_FACTORY(OfflineSpacePointProviderTool)

DECLARE_FACTORY_ENTRIES(TrigOfflineSpacePointTool) {
  DECLARE_TOOL(LazyOfflineSpacePointTool)
  DECLARE_TOOL(OfflineSpacePointProviderTool)
}
