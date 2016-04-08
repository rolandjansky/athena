#include "EventShapeTools/EventDensityTool.h"
#include "EventShapeTools/EventShapeCopier.h"
#include "../EventDensityAthAlg.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY( EventDensityTool )
DECLARE_TOOL_FACTORY( EventShapeCopier )
DECLARE_ALGORITHM_FACTORY( EventDensityAthAlg )

DECLARE_FACTORY_ENTRIES( EventShapeTools ) {
  DECLARE_TOOL( EventDensityTool );
  DECLARE_TOOL( EventShapeCopier );
  DECLARE_ALGORITHM( EventDensityAthAlg );

}
