#include "GaudiKernel/DeclareFactoryEntries.h"
#include "HGTD_TrackTimeExtensionTools/HGTD_IterativeExtensionTool.h"

using namespace HGTD;

DECLARE_TOOL_FACTORY(HGTD_IterativeExtensionTool)

DECLARE_FACTORY_ENTRIES( HGTD_TrackTimeExtensionTools )
{
  DECLARE_TOOL( HGTD_IterativeExtensionTool )
}
