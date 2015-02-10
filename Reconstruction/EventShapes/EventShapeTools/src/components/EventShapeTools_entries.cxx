#include "EventShapeTools/EventDensityTool.h"
#include "EventShapeTools/EventShapeCopier.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY( EventDensityTool )
DECLARE_TOOL_FACTORY( EventShapeCopier )

DECLARE_FACTORY_ENTRIES( EventShapeTools ) {
  DECLARE_TOOL( EventDensityTool );
  DECLARE_TOOL( EventShapeCopier );

}
