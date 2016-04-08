#include "GaudiKernel/DeclareFactoryEntries.h"

#include "../DeadMaterialShowerTool.h"
#include "../SimpleFastKillerTool.h"

DECLARE_TOOL_FACTORY( DeadMaterialShowerTool )
DECLARE_TOOL_FACTORY( SimpleFastKillerTool )

DECLARE_FACTORY_ENTRIES( G4FastSimulation ){
  DECLARE_TOOL( DeadMaterialShowerTool )
  DECLARE_TOOL( SimpleFastKillerTool )
}
