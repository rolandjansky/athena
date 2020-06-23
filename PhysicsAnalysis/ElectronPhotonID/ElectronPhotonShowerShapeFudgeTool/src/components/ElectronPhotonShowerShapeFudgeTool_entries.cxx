#include "GaudiKernel/DeclareFactoryEntries.h"

#include "ElectronPhotonShowerShapeFudgeTool/ElectronPhotonShowerShapeFudgeTool.h"
#include "ElectronPhotonShowerShapeFudgeTool/ElectronPhotonVariableCorrectionTool.h"

DECLARE_TOOL_FACTORY( ElectronPhotonShowerShapeFudgeTool )
DECLARE_TOOL_FACTORY( ElectronPhotonVariableCorrectionTool )


DECLARE_FACTORY_ENTRIES( ElectronPhotonShowerShapeFudgeTool ) 
{
  DECLARE_ALGTOOL( ElectronPhotonShowerShapeFudgeTool )
  DECLARE_ALGTOOL( ElectronPhotonVariableCorrectionTool )
}


