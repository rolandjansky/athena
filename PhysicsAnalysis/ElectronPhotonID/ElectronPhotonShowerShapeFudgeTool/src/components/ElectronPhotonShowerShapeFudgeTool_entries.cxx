#include "GaudiKernel/DeclareFactoryEntries.h"

#include "ElectronPhotonShowerShapeFudgeTool/ElectronPhotonShowerShapeFudgeTool.h"
#include "ElectronPhotonShowerShapeFudgeTool/ElectronPhotonVariableCorrectionToolWrapper.h"

DECLARE_TOOL_FACTORY( ElectronPhotonShowerShapeFudgeTool )
DECLARE_TOOL_FACTORY( ElectronPhotonVariableCorrectionToolWrapper )


DECLARE_FACTORY_ENTRIES( ElectronPhotonShowerShapeFudgeTool ) 
{
  DECLARE_ALGTOOL( ElectronPhotonShowerShapeFudgeTool )
  DECLARE_ALGTOOL( ElectronPhotonVariableCorrectionToolWrapper )
}


