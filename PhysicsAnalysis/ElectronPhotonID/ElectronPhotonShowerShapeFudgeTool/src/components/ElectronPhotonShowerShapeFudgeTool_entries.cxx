#include "GaudiKernel/DeclareFactoryEntries.h"

#include "ElectronPhotonShowerShapeFudgeTool/ElectronPhotonShowerShapeFudgeTool.h"

DECLARE_TOOL_FACTORY( ElectronPhotonShowerShapeFudgeTool )


DECLARE_FACTORY_ENTRIES( ElectronPhotonSelectorTools ) 
{
  DECLARE_ALGTOOL( ElectronPhotonShowerShapeFudgeTool )
}


