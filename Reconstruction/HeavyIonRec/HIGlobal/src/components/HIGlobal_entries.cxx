#include "HIGlobal/HIEventShapeMaker.h"
#include "HIGlobal/HIEventShapeTriggerFast.h"
#include "HIGlobal/HIEventShapeFillerTool.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( HIEventShapeMaker )
DECLARE_ALGORITHM_FACTORY( HIEventShapeTriggerFast )
DECLARE_TOOL_FACTORY (HIEventShapeFillerTool)

DECLARE_FACTORY_ENTRIES( HIGlobal ) {
    DECLARE_TOOL(HIEventShapeFillerTool);

    DECLARE_ALGORITHM( HIEventShapeMaker );
    DECLARE_ALGORITHM( HIEventShapeTriggerFast );
}

